/* $Id: alarm.c,v 1.9 2003/11/20 22:55:20 tim Exp $
 *
 * Support for exam alarms
 * Created: 2003/04/19
 * Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.
 */

#include "alarm.h"
#include "database.h"
#include "exams.h"
#include "tnglue.h"
#include "prefs.h"
#include "clist.h"

#define alarmDetailDescHTextOffset		37
#define alarmDetailDescYTextOffset		4

// Max len of sound trigger label placeholder
#define soundTriggerLabelLen          32


extern UInt16 gMenuCurrentForm;
extern UniMatrixPrefs gPrefs;


// Placeholder for sound trigger label
static Char * gAlarmSoundTriggerLabel;

// handle to the list containing names and DB info of MIDI tracks.
// Each entry is of type SndMidiListItemType.
static MemHandle	gMidiListH;
// number of entries in the MIDI list
static UInt16	gMidiCount;


/*****************************************************************************
* FUNCTION:     AlarmAdvanceSeconds
*
* DESCRIPTION:  Returns the number of seconds that an alert should ring in
*               as set in the alarm preferences
*
* PARAMETERS:   prefs - Pointer to UniMatrixPrefs struct
* RETURNS:      number of seconds the alarm should ring early
*****************************************************************************/
static UInt32 AlarmAdvanceSeconds(UniMatrixPrefs *prefs) {
  UInt32 factor = 60;

  switch (prefs->alarmInfo.advanceUnit) {
    case aauMinutes: factor = 60;     break;
    case aauHours:   factor = 3600;   break;
    case aauDays:    factor = 86400;  break;
    default:         factor = 60;     break;
  }

  return (prefs->alarmInfo.advance * factor);
}


/*****************************************************************************
* FUNCTION:     AlarmFindNext
*
* DESCRIPTION:  Finds the time the next alarm must be set to
*
* PARAMETERS:   database reference to the categorized db, preferences struct ptr
*               time where to start searching, pointer to offset value (needed
*               for the feature that makes sure, that the palm does not ring for
*               an exam while another exam is running...)
* RETURNS:      time of next alarm in number of seconds since the epoch
*****************************************************************************/
static UInt32 AlarmFindNext(DmOpenRef cats, UniMatrixPrefs *prefs, UInt32 startAt, UInt32 *alarmOffset) {
  MemHandle m;
  UInt16 index=0;
  UInt32 foundTimeBegin=0xFFFFFFFF, foundTimeEnd=0;
  UInt32 nowAndAdv = startAt + AlarmAdvanceSeconds(prefs);
  DateTimeType tempDate;

  while ((m = DmQueryNextInCategory(cats, &index, prefs->curCat)) != NULL) {
    ExamDBRecord *ex;

    ex = MemHandleLock(m);
    if (ex->type == TYPE_EXAM) {
      DateTimeType dateTime;
      UInt32 thisTime=0;

      dateTime.year = ex->date.year + MAC_SHIT_YEAR_CONSTANT;
      dateTime.month = ex->date.month;
      dateTime.day = ex->date.day;
      dateTime.hour = ex->begin.hours;
      dateTime.minute = ex->begin.minutes;
      dateTime.second = 0;
      thisTime = TimDateTimeToSeconds(&dateTime);

      if (thisTime >= nowAndAdv) {
        foundTimeBegin = thisTime;
        dateTime.hour = ex->end.hours;
        dateTime.minute = ex->end.minutes;
        foundTimeEnd = TimDateTimeToSeconds(&dateTime);
        MemHandleUnlock(m);
        break;
      }

    }
    MemHandleUnlock(m);
    index += 1;
  }

  if (foundTimeBegin != ALARM_NOTFOUND) {
    UInt32 ringTime = foundTimeBegin - AlarmAdvanceSeconds(prefs);
    UInt32 lastTimeEnd=0xFFFFFFFF;

    index -= 1;
    *alarmOffset = 0;

    while ((m = TNDmQueryPrevInCategory(cats, &index, prefs->curCat)) != NULL) {
      ExamDBRecord *ex;

      ex = MemHandleLock(m);
      if (ex->type == TYPE_EXAM) {
        DateTimeType dateTime;
        UInt32 thisTimeBegin=0, thisTimeEnd=0;

        dateTime.year = ex->date.year + MAC_SHIT_YEAR_CONSTANT;
        dateTime.month = ex->date.month;
        dateTime.day = ex->date.day;
        dateTime.hour = ex->begin.hours;
        dateTime.minute = ex->begin.minutes;
        dateTime.second = 0;
        thisTimeBegin = TimDateTimeToSeconds(&dateTime);

        dateTime.hour = ex->end.hours;
        dateTime.minute = ex->end.minutes;
        thisTimeEnd = TimDateTimeToSeconds(&dateTime);

        if ((ringTime >= thisTimeBegin) && (ringTime <= thisTimeEnd)) {
          // We have a collision... We ring em after the exam
          *alarmOffset = (thisTimeEnd - ringTime) + (5 * minutesInSeconds);
          lastTimeEnd = thisTimeEnd;
          MemHandleUnlock(m);
          break;
        }

      }
      index -= 1;
      MemHandleUnlock(m);
    }

    if (*alarmOffset > 0) {
      // We have an offset, now check if the time with the offset does collide
      // if so try to resolv the collision by moving the alarm further.

      index += 1;
      while ((m = DmQueryNextInCategory(cats, &index, prefs->curCat)) != NULL) {
        ExamDBRecord *ex;

        ringTime = foundTimeBegin - AlarmAdvanceSeconds(prefs) + *alarmOffset;

        ex = MemHandleLock(m);
        if (ex->type == TYPE_EXAM) {
          DateTimeType dateTime;
          UInt32 thisTimeBegin=0, thisTimeEnd=0;

          dateTime.year = ex->date.year + MAC_SHIT_YEAR_CONSTANT;
          dateTime.month = ex->date.month;
          dateTime.day = ex->date.day;
          dateTime.hour = ex->begin.hours;
          dateTime.minute = ex->begin.minutes;
          dateTime.second = 0;
          thisTimeBegin = TimDateTimeToSeconds(&dateTime);

          dateTime.hour = ex->end.hours;
          dateTime.minute = ex->end.minutes;
          thisTimeEnd = TimDateTimeToSeconds(&dateTime);
   
          // StrPrintF(temp, "%lu <= %lu <= %lu ? (LastEnd: %lu)", thisTimeBegin, ringTime, thisTimeEnd, lastTimeEnd);
          // FrmCustomAlert(ALERT_debug, temp, "", "");


          if ((ringTime >= thisTimeBegin) && (ringTime <= thisTimeEnd)) {
            // We have a collision... We ring em after the exam

            if (thisTimeBegin == foundTimeBegin) {
              // we do not want to ring for an exam after it in any case!
              // So we ring right at the beginning of the exam. That should be OK...
              if (lastTimeEnd != ALARM_NOTFOUND) {
                if (lastTimeEnd < thisTimeBegin) {
                  // Ring two minutes before the exam. That is quaranteed to be
                  // after the previous exam since we have only 5 min steps for times
                  *alarmOffset = AlarmAdvanceSeconds(prefs) - (2 * minutesInSeconds);
                  FrmCustomAlert(ALERT_debug, "Ringing two mins before exam", "", "");
                } else {
                  *alarmOffset = AlarmAdvanceSeconds(prefs);
                  FrmCustomAlert(ALERT_debug, "Ringing at begin of exam 1", "", "");
                }
              } else {
                *alarmOffset = AlarmAdvanceSeconds(prefs);
                FrmCustomAlert(ALERT_debug, "Ringing at begin of exam 2", "", "");
              }
              MemHandleUnlock(m);
              break;

            } else if ((lastTimeEnd != ALARM_NOTFOUND) && (lastTimeEnd != thisTimeBegin) ) {
              // There are NOT two exams following RIGHT after another
              // so there is some room to place the alarm...
              *alarmOffset -= (UInt32)((thisTimeBegin - lastTimeEnd) / 2);
              FrmCustomAlert(ALERT_debug, "Collision completely solved", "", "");
              MemHandleUnlock(m);
              break;
            } else {
              *alarmOffset +=  (thisTimeEnd - ringTime) + (5 * minutesInSeconds);
              FrmCustomAlert(ALERT_debug, "Damnit. Collision needs eventually more solving", "", "");
            }
          } else {
            // We can only have collisions with "next" exam, not with the one after that
            MemHandleUnlock(m);
            break;
          }
        }
        MemHandleUnlock(m);
        index += 1;
      }
    }
  }
  if (foundTimeBegin != ALARM_NOTFOUND)  foundTimeBegin -= AlarmAdvanceSeconds(prefs);

  if (foundTimeBegin != ALARM_NOTFOUND) {
    TimSecondsToDateTime(foundTimeBegin + *alarmOffset, &tempDate);
    //StrPrintF(temp, "Set alarm for %u:%u:%u", tempDate.hour, tempDate.minute, tempDate.second);
    //FrmCustomAlert(ALERT_debug, temp, "", "");
  }
  
  return (foundTimeBegin + *alarmOffset);
}

/*****************************************************************************
* FUNCTION:     AlarmPostTriggered
*
* DESCRIPTION:  Posts exams matching the given time to the attention manager
*
* PARAMETERS:   open cats db, prefs, time for which we should show events
* RETURNS:      nothing
*****************************************************************************/
static void AlarmPostTriggered(DmOpenRef cats, UniMatrixPrefs *prefs, UInt32 alarmTime) {
  UInt16 index=0, cardNo=0;
  UInt32 examAlarm=0;
  DateTimeType dateTime;
  MemHandle m;
	DmSearchStateType searchInfo;
  LocalID dbID;
  UInt32 searchTime = alarmTime + AlarmAdvanceSeconds(prefs);

	DmGetNextDatabaseByTypeCreator (true, &searchInfo, sysFileTApplication, APP_CREATOR, true, &cardNo, &dbID);

  while ((m = DmQueryNextInCategory(cats, &index, prefs->curCat))) {
    ExamDBRecord *ex;

    ex = MemHandleLock(m);
    if (ex->type == TYPE_EXAM) {
      // We have an exam, insert if above current offset

      dateTime.year = ex->date.year + MAC_SHIT_YEAR_CONSTANT;
      dateTime.month = ex->date.month;
      dateTime.day = ex->date.day;
      dateTime.hour = ex->begin.hours;
      dateTime.minute = ex->begin.minutes;
      dateTime.second = 0;
      examAlarm = TimDateTimeToSeconds(&dateTime);

      if (examAlarm == searchTime) {
        UInt32 uid=0;

        DmRecordInfo(cats, index, NULL, &uid, NULL);
        AttnGetAttention(cardNo, dbID, uid, NULL, kAttnLevelInsistent, kAttnFlagsUseUserSettings, 300, 3);
      }
    }
    MemHandleUnlock(m);
    index += 1;
  }
}

/*****************************************************************************
* FUNCTION:     AlarmSetTrigger
*
* DESCRIPTION:  Sets alarm to given time with given reference value
*
* PARAMETERS:   time the alarm should ring, ref value, offset in UniMatrix
* RETURNS:      nothing
*****************************************************************************/
static void AlarmSetTrigger(UInt32 alarmTime, UInt32 ref) {
  if (alarmTime != ALARM_NOTFOUND) {
    DmSearchStateType searchInfo;
    UInt16 cardNo;
    LocalID dbID;

  	DmGetNextDatabaseByTypeCreator (true, &searchInfo, sysFileTApplication, APP_CREATOR, true, &cardNo, &dbID);

    AlmSetAlarm(cardNo, dbID, ref, alarmTime, false);
  }
}


/*****************************************************************************
* FUNCTION:     AlarmReset
*
* DESCRIPTION:  deletes currently set alarm and (if applicable) sets new alarm
*
* PARAMETERS:   open cats db
* RETURNS:      nothing
*****************************************************************************/
void AlarmReset(DmOpenRef cats) {
  UInt16 cardNo;
  LocalID dbID;
  UniMatrixPrefs prefs;

  if (TNPalmOSVersion() < ALARM_REQVER) return;

  PrefLoadPrefs(&prefs);

	SysCurAppDatabase(&cardNo, &dbID);
  AlmSetAlarm(cardNo, dbID, 0, 0, false);

  if (prefs.alarmInfo.useAlarm) {
    UInt32 nextAlarm=0, nextOffset=0;
    nextAlarm = AlarmFindNext(cats, &prefs, TimGetSeconds(), &nextOffset);
    if (nextAlarm != ALARM_NOTFOUND)  AlmSetAlarm(cardNo, dbID, nextOffset, nextAlarm, false);
  }
}


/*****************************************************************************
* FUNCTION:     AlarmTriggered
*
* DESCRIPTION:  Gets called when an alarm is triggered, does everything needed
*               to handle alarmtriggered cmd
*
* PARAMETERS:   open cats db, pointer to cmd struct
* RETURNS:      nothing
*****************************************************************************/
void AlarmTriggered(DmOpenRef cats, SysAlarmTriggeredParamType *cmdPBP) {
  UInt32						alarmTime, nextAlarm, nextOffset=0;
  UniMatrixPrefs    prefs;

  // all triggered alarms are sent to attention manager for display so there is
  // no need for alarm manager to send the sysAppLaunchCmdDisplayAlarm launchcode
  cmdPBP->purgeAlarm = true;

  // Establish the time for which alarms need to be retrieved.
  alarmTime= cmdPBP->alarmSeconds - cmdPBP->ref;

  PrefLoadPrefs(&prefs);

  // Post currently "running" alerts
  AlarmPostTriggered(cats, &prefs, alarmTime);

  // Set the alarm trigger for the time of the next alarm to ring.
  nextAlarm = AlarmFindNext(cats, &prefs, alarmTime + minutesInSeconds, &nextOffset);
  AlarmSetTrigger(nextAlarm, nextOffset);

}


/*****************************************************************************
* FUNCTION:     AlarmDraw
*
* DESCRIPTION:  Function that handles drawing when called by Attention Manager
*
* PARAMETERS:   open cats db, prefs, uniqueID of record that should be drawn,
*               Attention manager cmd struct ptr, switch whether to draw the
*               detailed description (true) or the list view (false)
* RETURNS:      nothing
*****************************************************************************/
static void AlarmDraw(DmOpenRef cats, UniMatrixPrefs *prefs, UInt32 uniqueID, AttnCommandArgsType *paramsP, Boolean drawDetail) {
  MemHandle resH;
  MemPtr resP;
  FontID curFont;
  Coord x, y;
  UInt16 index=0;
  Err err;
  MemHandle examH, courseH, examStrH;
  CourseDBRecord course;
  ExamDBRecord *exam;
  Char *examStr, *template;
	Char				dowNameStr[dowDateStringLength];
	Char 				dateStr[longDateStrLength];
	Char				timeStr [timeStringLength];
	DateFormatType	dateFormat;
	TimeFormatType	timeFormat;
  DateTimeType dateTime;
  Char chr;


  // Get needed data
  err = DmFindRecordByID(cats, uniqueID, &index);
  if (err != errNone)  return;

  examH = DmQueryRecord(cats, index);
  if (! examH) return;
  exam = (ExamDBRecord *)MemHandleLock(examH);
  dateTime.year = exam->date.year + MAC_SHIT_YEAR_CONSTANT;
  dateTime.month = exam->date.month;
  dateTime.day = exam->date.day;
  dateTime.hour = exam->begin.hours;
  dateTime.minute = exam->begin.minutes;


  if (! CourseGetIndex(cats, prefs->curCat, exam->course, &index)) return;
  courseH = DmQueryRecord(cats, index);
  if (! courseH) return;
  UnpackCourse(&course, MemHandleLock(courseH));

  examStrH = DmGetResource(strRsc, ALARM_exam);
  examStr = MemHandleLock(examStrH);

	// Get the date and time formats.
	dateFormat = (DateFormatType)PrefGetPreference(prefDateFormat);
	timeFormat = (TimeFormatType)PrefGetPreference(prefTimeFormat);

  if (drawDetail) {

    // Draw the icon 
    if (TNisColored()) {
      resH = DmGetResource(bitmapRsc, BITMAP_alarm_big_color); 
    } else {
      resH = DmGetResource(bitmapRsc, BITMAP_alarm_big_bw); 
    }
    WinDrawBitmap(MemHandleLock(resH), paramsP->drawDetail.bounds.topLeft.x,
                                       paramsP->drawDetail.bounds.topLeft.y + alarmDetailDescYTextOffset); 
    MemHandleUnlock(resH); 
    DmReleaseResource(resH); 

    // Draw the text. The content of the string depends on the 
    // uniqueID that accompanies the kAttnCommandDrawDetail 
    // command 
    curFont = FntSetFont (largeBoldFont); 

    x = paramsP->drawDetail.bounds.topLeft.x + alarmDetailDescHTextOffset;
    y = paramsP->drawDetail.bounds.topLeft.y + alarmDetailDescYTextOffset;

    // Date string
    // Get the day-of-week name and the system formatted date
    DateTemplateToAscii("^1l", dateTime.month, dateTime.day, dateTime.year, dowNameStr, sizeof(dowNameStr));
    DateToAscii(dateTime.month, dateTime.day, dateTime.year, dateFormat, dateStr);
    
    resH = DmGetResource(strRsc, ALARM_date_template);
    resP = MemHandleLock(resH);
    template = TxtParamString(resP, dowNameStr, dateStr, NULL, NULL);
    MemPtrUnlock(resP);
    DmReleaseResource(resH);

    WinDrawChars(template, StrLen(template), x, y);
    MemPtrFree((MemPtr)template);


    // Time string
    y += FntLineHeight();
  
    TimeToAscii (dateTime.hour, dateTime.minute, timeFormat, timeStr);
    WinDrawChars (timeStr, StrLen (timeStr), x, y);
  
    x += FntCharsWidth (timeStr, StrLen (timeStr)) + FntCharWidth (spaceChr);
    chr = '-';
    WinDrawChars (&chr, 1, x, y);
  
    TimeToAscii (exam->end.hours, exam->end.minutes, timeFormat, timeStr);
    x += FntCharWidth (chr) + FntCharWidth (spaceChr);
    WinDrawChars (timeStr, StrLen (timeStr), x, y);


    // Exam header
  	x = paramsP->drawDetail.bounds.topLeft.x + alarmDetailDescHTextOffset;
    y += 2 * FntLineHeight();

    resH = DmGetResource(strRsc, ALARM_exam);
    resP = MemHandleLock(resH);
    WinDrawChars(resP, StrLen(resP), x, y);
    MemPtrUnlock(resP);
    DmReleaseResource(resH);


    // Exam course name
    y += FntLineHeight();
    WinDrawChars(course.name, StrLen(course.name), x, y);

    FntSetFont(curFont); 

  } else {
    // Draw the icon. Ignore the 'selected' flag for this example 
    BitmapType *icon;
    UInt16 iconOffset=0;
    DateTimeType today;
    Coord icon_width=0;

    resH = DmGetResource(bitmapRsc, BITMAP_alarm_small_color); 
    icon = (BitmapType *)(MemHandleLock(resH)); 
    BmpGetDimensions(icon, &icon_width, NULL, NULL);
    
    // center it in the space allotted 
    iconOffset = ((kAttnListMaxIconWidth - icon_width)/2);

    x = paramsP->drawList.bounds.topLeft.x; 
    y = paramsP->drawList.bounds.topLeft.y; 

    WinDrawBitmap(icon, x + iconOffset, y);
    MemHandleUnlock(resH); 
    DmReleaseResource(resH); 

    // Draw the text 
    curFont = FntSetFont(stdFont); 

    // draw the time information for the event if the event has a time and the duration is > 0.
    // Draw the event's start time
    TimeToAscii (dateTime.hour, dateTime.minute, timeFormat, timeStr);
  
    x = paramsP->drawList.bounds.topLeft.x + kAttnListTextOffset;
  
    WinDrawChars (timeStr, StrLen (timeStr), x, y);
    x += FntCharsWidth (timeStr, StrLen (timeStr));

    // draw the event's end time if its duration is > 0
    x += (FntCharWidth(spaceChr) / 2);
    chr = '-';
    WinDrawChars (&chr, 1, x, y);
    x += FntCharWidth(chr) + (FntCharWidth(spaceChr) / 2);
    
    TimeToAscii (exam->end.hours, exam->end.minutes, timeFormat, timeStr);
    WinDrawChars (timeStr, StrLen (timeStr), x, y);
    x += FntCharsWidth(timeStr, StrLen (timeStr)) + FntCharWidth(spaceChr);

  	// Draw the event's date
  	// If the event occurs today, draw the 
  	TimSecondsToDateTime (TimGetSeconds(), &today);	

	  if ( (today.day == dateTime.day) && (today.month == dateTime.month) && (today.year == dateTime.year)) {
      resH = DmGetResource (strRsc, ALARM_STRING_today);
      resP = MemHandleLock(resH);
      WinDrawChars (resP, StrLen (resP), x, y);
      MemPtrUnlock (resP);
    } else {
      DateToAscii(dateTime.month, dateTime.day, dateTime.year, dateFormat, dateStr);
      WinDrawChars (dateStr, StrLen (dateStr), x, y);
    }

    // Exam course name
    y += FntLineHeight();
    x = paramsP->drawList.bounds.topLeft.x + kAttnListTextOffset;
    WinDrawChars(examStr, StrLen(examStr), x, y);
    x += FntCharsWidth(examStr, StrLen(examStr)) + FntCharWidth(spaceChr);
    WinDrawChars(course.name, StrLen(course.name), x, y);

    
    FntSetFont(curFont);
  }

  MemHandleUnlock(courseH);
  MemHandleUnlock(examH);
  MemHandleUnlock(examStrH);
  DmReleaseResource(examStrH);
}


/*****************************************************************************
* FUNCTION:     AlarmPlaySound
*
* DESCRIPTION:  Plays Sound identified by its unique ID
*
* PARAMETERS:   unique ID of the MIDI
* RETURNS:      nothing
*****************************************************************************/
static void AlarmPlaySound(UInt32 uniqueRecID) {
  Err                 err;
  MemHandle	          midiH;          // handle of MIDI record
  SndMidiRecHdrType  *midiHdrP;       // pointer to MIDI record header
  UInt8*              midiStreamP;    // pointer to MIDI stream beginning with the 'MThd'
                                      // SMF header chunk
  UInt16              cardNo;         // card number of System MIDI database
  LocalID             dbID;           // Local ID of System MIDI database
  DmOpenRef	          midiDB = NULL;  // reference to open database
  UInt16		          recIndex;       // record index of the MIDI record to play
  SndSmfOptionsType	  smfOpt;         // SMF play options
  DmSearchStateType	  searchState;    // search state for finding the System MIDI database
  
    
  // Find the system MIDI database
  err = DmGetNextDatabaseByTypeCreator(true, &searchState, sysFileTMidi, sysFileCSystem, true, &cardNo, &dbID);
  if ( err != errNone )  return; // DB not found
  
  // Open the MIDI database in read-only mode
  midiDB = DmOpenDatabase (cardNo, dbID, dmModeReadOnly);
  if ( ! midiDB ) return;
  
  // Find the MIDI track record
  err = DmFindRecordByID (midiDB, uniqueRecID, &recIndex);
  if ( err ) {
    // record not found
    DmCloseDatabase(midiDB);
    return;
  }
    
  // Lock the record and play the sound
  // Find the record handle and lock the record
  midiH = DmQueryRecord(midiDB, recIndex);
  midiHdrP = MemHandleLock(midiH);
    
  // Get a pointer to the SMF stream
  midiStreamP = (UInt8*)midiHdrP + midiHdrP->bDataOffset;
    
  // Play the sound (ignore the error code)
  // The sound can be interrupted by a key/digitizer event
  smfOpt.dwStartMilliSec = 0;
  smfOpt.dwEndMilliSec = sndSmfPlayAllMilliSec;
  smfOpt.amplitude = PrefGetPreference(prefAlarmSoundVolume);
  smfOpt.interruptible = true;
  smfOpt.reserved = 0;
  err = SndPlaySmf (NULL, sndSmfCmdPlay, midiStreamP, &smfOpt, NULL, NULL, false);
    
  MemPtrUnlock (midiHdrP);
  if ( midiDB )  DmCloseDatabase (midiDB);
}


/*****************************************************************************
* FUNCTION:     AlarmGoto
*
* DESCRIPTION:  Restarts app and goes to the wanted exam
*
* PARAMETERS:   unique ID of the exam we rang for
* RETURNS:      nothing
*****************************************************************************/
static void AlarmGoto(UInt32 uniqueID) {
	DmSearchStateType searchInfo;
  UInt16 cardNo;
  LocalID dbID;
  UInt32 *gotoInfoP;

	gotoInfoP = (UInt32*)MemPtrNew (sizeof(UInt32));
	ErrFatalDisplayIf ((!gotoInfoP), "Out of memory");
	MemPtrSetOwner(gotoInfoP, 0);

	// initialize the goto params structure so that datebook will open day view 
	// with the specified item selected
	*gotoInfoP = uniqueID;

	DmGetNextDatabaseByTypeCreator (true, &searchInfo, sysFileTApplication, APP_CREATOR, true, &cardNo, &dbID);
  SysUIAppSwitch(cardNo, dbID, appLaunchCmdAlarmEventGoto, gotoInfoP);
}


/*****************************************************************************
* FUNCTION:     AttentionBottleNeckProc
*
* DESCRIPTION:  Function called for attention manager requests
*
* PARAMETERS:   open cats db, attention manager args struct ptr
* RETURNS:      true on success (always :-)
*****************************************************************************/
Boolean AttentionBottleNeckProc(DmOpenRef cats, AttnLaunchCodeArgsType *paramP) {
//	AttnCommandArgsType * argsP = paramP->commandArgsP;
  UniMatrixPrefs prefs;

  PrefLoadPrefs(&prefs);

  switch (paramP->command) {
    case kAttnCommandDrawDetail:
      AlarmDraw(cats, &prefs, paramP->userData, paramP->commandArgsP, true);
      break;			
  
    case kAttnCommandDrawList:
      AlarmDraw(cats, &prefs, paramP->userData, paramP->commandArgsP, false);
      break;

    case kAttnCommandPlaySound:
      AlarmPlaySound(prefs.alarmInfo.soundUniqueRecID);
      break;
            
/*
    case kAttnCommandGotIt:
      {
      if (argsP->gotIt.dismissedByUser)
        DeleteAlarm(paramP->userData);
      }
      break;			
*/
    case kAttnCommandGoThere:
      AlarmGoto(paramP->userData);		
      break;	
  /*
    case kAttnCommandIterate:
      // if the argument is nil, this is a "tickle from the attention manager
      // asking the application to validate the specified entry
      // this may happen at interrupt time - do not use globals here
      if (argsP == NULL)
        ValidateAlarm(paramP->userData);
      
      // otherwise, this launch code was received from attention manager in
      // response to an AttnIterate made by Date Book to update the posted alarms
      // with respect to one of the following occurrences
      else
        switch (argsP->iterate.iterationData) 
          {
          // When the user changes the nag parameters, assign the new value to each alarm currently
          // in the attention manager queue.
          // THIS WILL ONLY OCCUR WHEN APP IS RUNNING SO CACHED GLOBALS ARE USED FOR SPEED
          case SoundRepeatChanged:
            {
            UInt16 		cardNo;
            LocalID 		dbID;
            DmSearchStateType searchInfo;
  
            DmGetNextDatabaseByTypeCreator (true, &searchInfo, sysFileTApplication, sysFileCDatebook, true, &cardNo, &dbID);
            AttnUpdate(cardNo, dbID, paramP->userData, NULL, NULL, &AlarmSoundRepeatInterval, &AlarmSoundRepeatCount);
            break;			
            }
            
          default:
            ValidateAlarm(paramP->userData);
          }
      break;	
      
  //		case AttnCommand_nag:		// ignored by DateBook
  //			break;
*/
  }
  
  return true;

}





/******************************************
 * MIDI List stuff
 ******************************************/
/*****************************************************************************
* FUNCTION:     MidiPickListCreate
*
* DESCRIPTION:  Creates the list with MIDI names
*
* PARAMETERS:   pointer to a ListType form element, callback function for
*               drawing the list
* RETURNS:      nothing
*****************************************************************************/
static void MidiPickListCreate(FormType *frm, ListPtr listP, ListDrawDataFuncPtr funcP) {
  SndMidiListItemType*	midiListP;
  UInt16		i;
  UInt16		listWidth;
  UInt16		maxListWidth;
  RectangleType r, listBounds;
  UInt16 listW=0, listX=0;
    
  // Load list of midi record entries
  if ( ! SndCreateMidiList(sysFileCSystem, false, &gMidiCount, &gMidiListH)) {
    gMidiListH = 0;
    gMidiCount = 0;
    return;
  }


  // Now set the list to hold the number of sounds found.  There
  // is no array of text to use.
  LstSetListChoices(listP, NULL, gMidiCount);
  
  // Now resize the list to the number of panel found
  LstSetHeight (listP, gMidiCount);
  
  // Because there is no array of text to use, we need a function
  // to interpret the panelIDsP list and draw the list items.
  LstSetDrawFunction(listP, funcP);
  
  // Make the list as wide as possible to display the full sound names
  // when it is popped winUp.
  
  // Lock MIDI sound list
  midiListP = MemHandleLock(gMidiListH);
  // Initialize max width
  maxListWidth = 0;
  
  FrmGetObjectBounds(frm, FrmGetObjectIndexFromPtr(frm, listP), &listBounds);
  listW = listBounds.extent.x;
  listX = listBounds.topLeft.x;

  // Iterate through each item and get its width
  for (i = 0; i < gMidiCount; i++) {
      // Get the width of this item
      listWidth = FntCharsWidth(midiListP[i].name, StrLen(midiListP[i].name));
      // If item width is greater that max, swap it
      if (listWidth > maxListWidth) {
        maxListWidth = listWidth;
      }
  }

  // Unlock MIDI sound list
  MemPtrUnlock(midiListP);
  // Set list width to max width + left margin + right margin
  listW = maxListWidth + 4;
  // Get pref dialog window extent
  FrmGetFormBounds(FrmGetActiveForm(), &r);
  // Make sure width is not more than window extent
  if (listW > r.extent.x) {
    listW = r.extent.x;
  }
  // Move list left if it doesnt fit in window
  if (listX + listW > r.extent.x) {
      listX = r.extent.x - listX;
  }

  RctSetRectangle(&listBounds, listX, listBounds.topLeft.y,
                               listW, listBounds.extent.y);
  FrmSetObjectBounds(frm, FrmGetObjectIndexFromPtr(frm, listP), &listBounds);
}


/*****************************************************************************
* FUNCTION:     SetSoundLabel
*
* DESCRIPTION:  Sets the trigger label (hard coded, not parameterized) with
*               selected sound name
*
* PARAMETERS:   Pointer to the alarm prefs form, label to set the trigger to
* RETURNS:      nothing
*****************************************************************************/
static void SetSoundLabel(FormPtr formP, const char* labelP) {
	ControlPtr	triggerP;
	UInt16			triggerIdx;

	// Copy the label, winUp to the max into the ptr
	StrNCopy(gAlarmSoundTriggerLabel, labelP, soundTriggerLabelLen);
	// Terminate string at max len
	gAlarmSoundTriggerLabel[soundTriggerLabelLen - 1] = '\0';
	// Get trigger idx
	triggerIdx = FrmGetObjectIndex(formP, ALARM_sound_trigger);
	// Get trigger control ptr
	triggerP = FrmGetObjectPtr(formP, triggerIdx);
	// Use category routines to truncate it
	CategoryTruncateName(gAlarmSoundTriggerLabel, ResLoadConstant(ALARM_sound_trigger_width));
	// Set the label
	CtlSetLabel(triggerP, gAlarmSoundTriggerLabel);
}


/*****************************************************************************
* FUNCTION:     MidiPickListDrawItem
*
* DESCRIPTION:  callback to draw sound names in the sound list
*
* PARAMETERS:   number of item to draw, rectangle to draw in, unused param
* RETURNS:      nothing
*****************************************************************************/
static void MidiPickListDrawItem (Int16 itemNum, RectanglePtr bounds, Char **unusedP) {
  Char *	itemTextP;
  Int16		itemTextLen;
  Int16		itemWidth;
  SndMidiListItemType*	listP;
  
  ErrNonFatalDisplayIf(itemNum >= gMidiCount, "index out of bounds");
  
  // Bail out if MIDI sound list is empty
  if (gMidiListH == NULL)  return;
  
  listP = MemHandleLock(gMidiListH);
  
  itemTextP = listP[itemNum].name;
  
  // Truncate the item with an ellipsis if it doesnt fit in the list width.
  // Get the item text length
  itemTextLen = StrLen(itemTextP);
  // Get the width of the text
  itemWidth = FntCharsWidth(itemTextP, itemTextLen);
  // Does it fit?
  if (itemWidth <= bounds->extent.x) {
    // Draw entire item text as is
    WinDrawChars(itemTextP, itemTextLen, bounds->topLeft.x, bounds->topLeft.y);
  } else {
    // We're going to truncate the item text
    Boolean	ignored;
    char		ellipsisChar = chrEllipsis;
    // Set the new max item width
    itemWidth = bounds->extent.x - FntCharWidth(ellipsisChar);
    // Find the item length that fits in the bounds minus the ellipsis
    FntCharsInWidth(itemTextP, &itemWidth, &itemTextLen, &ignored);
    // Draw item text that fits
    WinDrawChars(itemTextP, itemTextLen, bounds->topLeft.x, bounds->topLeft.y);
    // Draw ellipsis char
    WinDrawChars(&ellipsisChar, 1, bounds->topLeft.x + itemWidth, bounds->topLeft.y);
  }

  // Unlock list items
  MemPtrUnlock(listP);
}


/*****************************************************************************
* FUNCTION:     MidiPickListFree
*
* DESCRIPTION:  Frees memory associated to sound list
*
* PARAMETERS:   nothing
* RETURNS:      nothing
*****************************************************************************/
static void MidiPickListFree(void) {
  if ( gMidiListH )	{
    MemHandleFree(gMidiListH);
    gMidiListH = 0;
    gMidiCount = 0;
  }
}


/*****************************************************************************
* FUNCTION:     AlarmFormInit
*
* DESCRIPTION:  Initializes the alarm prefs form
*
* PARAMETERS:   form ptr to the alarm prefs form
* RETURNS:      nothing
*****************************************************************************/
static void AlarmFormInit(FormType *frm) {
  ControlType *ctl;
  ListType *lst;
  UInt16 listPos=0, item=0;
  MemHandle new, old;
  Char *tmp;
  FieldType *fld=GetObjectPtr(ALARM_time);


  // Enable alarm?  
  ctl = GetObjectPtr(ALARM_use);
  CtlSetValue(ctl, gPrefs.alarmInfo.useAlarm);

  // Time
  new = MemHandleNew(maxStrIToALen);
  tmp = (Char *)MemHandleLock(new);
  StrIToA(tmp, gPrefs.alarmInfo.advance);
  old = FldGetTextHandle(fld);
  FldSetTextHandle(fld, new);
  if (old)  MemHandleFree(old);

  // Time Unit
  ctl = GetObjectPtr(ALARM_unit_trigger);
  lst = GetObjectPtr(ALARM_unit);
  switch (gPrefs.alarmInfo.advanceUnit) {
    case aauMinutes: listPos = 0; break;
    case aauHours: listPos = 1; break;
    case aauDays: listPos = 2; break;
    default: listPos = 0; break;
  }
  LstSetSelection(lst, listPos);
  CtlSetLabel(ctl, LstGetSelectionText(lst, listPos));

  // Repeat count / remind list
  ctl = GetObjectPtr(ALARM_remind_trigger);
  lst = GetObjectPtr(ALARM_remind);
  switch (gPrefs.alarmInfo.repeatCount) {
    case 1: listPos = 0; break;
    case 2: listPos = 1; break;
    case 3: listPos = 2; break;
    case 5: listPos = 3; break;
    case 10: listPos = 4; break;
    default: listPos = 2; break;
  }
  LstSetSelection(lst, listPos);
  CtlSetLabel(ctl, LstGetSelectionText(lst, listPos));

  // Repeat interval list
  ctl = GetObjectPtr(ALARM_repeat_trigger);
  lst = GetObjectPtr(ALARM_repeat);
  switch (gPrefs.alarmInfo.repeatInterval) {
    case 1: listPos = 0; break;
    case 5: listPos = 1; break;
    case 10: listPos = 2; break;
    case 30: listPos = 3; break;
    default: listPos = 1; break;
  }
  LstSetSelection(lst, listPos);
  CtlSetLabel(ctl, LstGetSelectionText(lst, listPos));

  
  // Alarm sound stuff
  lst = GetObjectPtr(ALARM_sound);

  MidiPickListCreate(frm, lst, MidiPickListDrawItem);
  
  // Find selected item
  // Default to first sound in list
  item = 0;

  // Lock MIDI sound list
  if ( gMidiListH ) {
  	SndMidiListItemType*	midiListP;
    UInt16 i;

    midiListP = MemHandleLock(gMidiListH);

    // Iterate through each item and get its unique ID
    for (i = 0; i < gMidiCount; ++i) {
      if (midiListP[i].uniqueRecID == gPrefs.alarmInfo.soundUniqueRecID) {
        item = i;
        break;		// exit for loop
      }
    }

    // Set the list selection
    LstSetSelection (lst, item);

    // Init the sound trigger label
    // Create a new ptr to hold the label
    gAlarmSoundTriggerLabel = MemPtrNew(soundTriggerLabelLen);
    // Check for mem failure
    ErrFatalDisplayIf(gAlarmSoundTriggerLabel == NULL, "Out of memory");
    // Set the trigger label
    SetSoundLabel(frm, midiListP[item].name);

    // Unlock MIDI sound list
    MemPtrUnlock(midiListP);
  }

}


/*****************************************************************************
* FUNCTION:     AlarmFormSave
*
* DESCRIPTION:  Saves settings of the alarm prefs form
*
* PARAMETERS:   form ptr to the alarm prefs form
* RETURNS:      true if saving was successful (nothing that can prevent that
*               right now checked)
*****************************************************************************/
static Boolean AlarmFormSave(FormType *frm) {
  ControlType *ctl;
  ListType *lst;
  FieldType *fld;
  AlarmUnitType unit;
  UInt16 ui16;
  SndMidiListItemType*	sndList;
  UInt16 sndItem=0;
  
  ctl = GetObjectPtr(ALARM_use);
  gPrefs.alarmInfo.useAlarm = (CtlGetValue(ctl));

  fld = GetObjectPtr(ALARM_time);
  lst = GetObjectPtr(ALARM_unit);
  switch (LstGetSelection(lst)) {
    case 0: unit = aauMinutes; break;
    case 1: unit = aauHours; break;
    case 2: unit = aauDays; break;
    default: unit = aauMinutes; break;
  }
  gPrefs.alarmInfo.advance = (UInt8)StrAToI(FldGetTextPtr(fld));
  gPrefs.alarmInfo.advanceUnit = unit;


  // Repeat count / remind list
  lst = GetObjectPtr(ALARM_remind);
  switch (LstGetSelection(lst)) {
    case 0: ui16 = 1; break;
    case 1: ui16 = 2; break;
    case 2: ui16 = 3; break;
    case 3: ui16 = 5; break;
    case 4: ui16 = 10; break;
    default: ui16 = 3; break;
  }
  gPrefs.alarmInfo.repeatCount = ui16;

  // Repeat interval list
  lst = GetObjectPtr(ALARM_repeat);
  switch (LstGetSelection(lst)) {
    case 0: ui16 = 1; break;
    case 1: ui16 = 5; break;
    case 2: ui16 = 10; break;
    case 3: ui16 = 30; break;
    default: ui16 = 5; break;
  }
  gPrefs.alarmInfo.repeatInterval = ui16;


  lst = GetObjectPtr(ALARM_sound);
  sndItem = LstGetSelection(lst);

  sndList = MemHandleLock(gMidiListH);
  gPrefs.alarmInfo.soundUniqueRecID = sndList[sndItem].uniqueRecID;
  MemPtrUnlock(sndList);
  
  PrefSavePrefs(&gPrefs);
  AlarmReset(DatabaseGetRef());

  return true;
}


/*****************************************************************************
* FUNCTION:     AlarmFormFree
*
* DESCRIPTION:  Frees memory related to form items
*
* PARAMETERS:   nothing
* RETURNS:      nothing
*****************************************************************************/
static void AlarmFormFree(void) {
  // Free the MIDI pick list
  MidiPickListFree();

  // Free the sound trigger label placeholder
  if ( gAlarmSoundTriggerLabel ) {
    MemPtrFree(gAlarmSoundTriggerLabel);
    gAlarmSoundTriggerLabel = NULL;
  }
}


/*****************************************************************************
* FUNCTION:     AlarmFormHandleEvent
*
* DESCRIPTION:  Handles events for the alarm prefs form
*
* PARAMETERS:   ptr to the event info struct
* RETURNS:      true if event handled completely and successfully
*****************************************************************************/
Boolean AlarmFormHandleEvent(EventPtr event) {
  Boolean handled = false;
  FormType *frm=FrmGetActiveForm();

  if (event->eType == ctlSelectEvent) {
    switch (event->data.ctlSelect.controlID) {

      case ALARM_ok:
        if (AlarmFormSave(frm)) {
          FrmReturnToForm(gMenuCurrentForm);
          FrmUpdateForm(gMenuCurrentForm, frmRedrawUpdateCode);
          AlarmFormFree();
        }
        handled=true;
        break;

      case ALARM_cancel:
        FrmReturnToForm(gMenuCurrentForm);
        FrmUpdateForm(gMenuCurrentForm, frmRedrawUpdateCode);
        AlarmFormFree();
        handled=true;
        break;

      default:
        break;
    }   
  } else if (event->eType == popSelectEvent) {
    if (event->data.popSelect.listID == ALARM_sound) {
      SndMidiListItemType *sndList;
      UInt16 sndItem=0;

      sndItem = event->data.popSelect.selection;

      sndList = MemHandleLock(gMidiListH);

      CtlEraseControl(event->data.popSelect.controlP);
      SetSoundLabel(frm, sndList[sndItem].name);
      CtlDrawControl(event->data.popSelect.controlP);

      AlarmPlaySound(sndList[sndItem].uniqueRecID);

      MemPtrUnlock(sndList);

      handled = true;
    }	
  } else if (event->eType == frmUpdateEvent) {
      // redraws the form if needed
      frm = FrmGetActiveForm();
      FrmDrawForm (frm);
      handled = true;
    } else if (event->eType == frmOpenEvent) {
      // initializes and draws the form at program launch
      frm = FrmGetActiveForm();
      AlarmFormInit(frm);
      FrmDrawForm (frm);
      handled = true;
    } else if (event->eType == frmCloseEvent) {
      // this is done if program is closed
      // NOT called, since we use FrmReturnToForm!
    }



  return handled;
}
