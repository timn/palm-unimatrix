/* $Id: gadget.c,v 1.7 2005/05/27 15:01:35 tim Exp $
*
* THE heart of UniMatrix. This is the center piece of code in UniMatrix
*/

#include "UniMatrix.h"
#include "gadget.h"
#include "tnglue.h"
#include "ctype.h"
#include "clist.h"
#include "prefs.h"
#include "cache.h"
#include "notes.h"
#include "tnlist.h"

extern UniMatrixPrefs gPrefs;

FormPtr gForm=NULL;
UInt16  gGadgetID=0,
        gHintGadgetID=0,
        gCourseIndex=0,
        gTimeIndex=0,
        gTimeDrawnIndex=0,
        gHintNote=0;
Boolean gHintDrawn=false,
        gGadgetCompleteRedraw=true;
UInt8   gGadgetCurMinHour = GADGET_MIN_HOUR,
        gGadgetCurMaxHour = GADGET_MAX_HOUR,
        gGadgetDaysNum    = GADGET_DEFAULT_NUMDAYS,
        gGadgetDaysWidth=GADGET_TOTAL_DRAWWIDTH/GADGET_DEFAULT_NUMDAYS,
        gGadgetCurScreen=GADGET_SCREEN_DAY;
TimeType gGadgetLastTimeline={0x00, 0x00};
TNlist  *gGadgetTimeList = NULL;


/*****************************************************************************
* Function: GadgetCalcTimeTop
*
* Description: Calculates top offset for drawing
*****************************************************************************/
static UInt8 GadgetCalcTimeTop(TimeType begin) {
  Int8 tmp;

  if (gGadgetCurScreen == GADGET_SCREEN_DAY) {
    tmp = begin.minutes / 15;
    //      Hours as offset     4: hours has 4 * 15 mins
    //                                  2: 2 pixel per 15 mins    again here         if more than 5 mins above 15 min add 1
    tmp=((begin.hours - GADGET_MIN_HOUR)*4)*2 + tmp*2 + (((begin.minutes - tmp*15) >= 5) ? 1 : 0 );

    if (tmp < 0)  tmp = 0;
  } else { // GADGET_SCREEN_NIGHT
    if ((begin.hours < 20) && (begin.hours >= 8)) {
      tmp = 0;
    } else {
      tmp = begin.minutes / 15;
      if (begin.hours < 8) {
        tmp = ((begin.hours +4 )*4)*2 + tmp*2 + (((begin.minutes - tmp*15) >= 5) ? 1 : 0 );
      } else {
        tmp=((begin.hours - 20)*4)*2 + tmp*2 + (((begin.minutes - tmp*15) >= 5) ? 1 : 0 );
      }
    }
  }
  return (UInt8)tmp;
}

/*****************************************************************************
* Function: GadgetCalcTimeHeight
*
* Description: Computes height for time to be drawn
*****************************************************************************/
static UInt8 GadgetCalcTimeHeight(TimeType begin, TimeType end) {
  TimeType diffTime;
  UInt16 diff, tmp;

  if (gGadgetCurScreen == GADGET_SCREEN_NIGHT) {
    if  ((begin.hours < 20) && (begin.hours >= 8)) {
      begin.hours=20;
      begin.minutes=0;
    }
  } else { // GADGET_SCREEN_DAY
    if (begin.hours < 8) {
      begin.hours=8;
      begin.minutes=0;
    }
  }

  diff = (end.hours*60+end.minutes) - (begin.hours*60+begin.minutes);

  diffTime.hours=diff/60;
  diffTime.minutes=diff % 60;

  tmp = diffTime.minutes / 15;
  tmp = (diffTime.hours * 4) * 2 + tmp * 2 + (((diffTime.minutes - tmp*15) >= 5) ? 1 : 0);


  return tmp;
}


/*****************************************************************************
* Function: GadgetEventIsVisible
*
* Description: Checks if an event is currently visible
*****************************************************************************/
static Boolean GadgetEventIsVisible(TimeDBRecord *t) {
  if ((t->type == TYPE_TIME) && (t->day < gGadgetDaysNum)) {
    if (gGadgetCurScreen == GADGET_SCREEN_DAY) {
      if ( ((t->begin.hours >= 8) && (t->begin.hours < 20)) ||
           ((t->end.hours >= 8) && (t->end.hours < 20)) ) {
        // We are currently displaying the day and begin or end are at day
        return true;
      }
    } else { // GADGET_SCREEN_NIGHT
      if ( (t->begin.hours < 8) || (t->begin.hours >= 20) ||
           (t->end.hours < 8) || (t->end.hours >= 20) ) {
        // We are currently displaying the day and begin or end are at day
        return true;
      }
    }
  }

  return false;
}


/*****************************************************************************
* Function: GadgetDrawWeekdays
*
* Description: Draws the weekdays, extra function since called in
*              GadgetDrawTimeline
*****************************************************************************/
void GadgetDrawWeekdays(void) {
  UInt8 i;
  MemHandle mh;
  Char *text;
  RGBColorType color, prevColor;
  DateTimeType now;
  Int16 dow;
  RectangleType bounds, rect;
  UInt16 gadgetIndex;

  // Get info about Gadget
  gadgetIndex = FrmGetObjectIndex(gForm, gGadgetID);
  FrmGetObjectBounds(gForm, gadgetIndex, &bounds);

  // Initialize time constants
  TimSecondsToDateTime(TimGetSeconds(), &now);
  dow = DayOfWeek(now.month, now.day, now.year);
  
  RctSetRectangle(&rect, bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT, bounds.topLeft.y,
                         130, FntLineHeight()+2);

  // Erase background
  WinEraseRectangle(&rect, 0);

  for (i=0; i < gGadgetDaysNum; ++i) {
    Int16 leftoff;
    mh = DmGetResource(strRsc, GADGET_STRINGS_WDAYSTART+i);
    text = MemHandleLock(mh);
    leftoff = (gGadgetDaysWidth+2 - FntLineWidth(text, MemPtrSize(text))) / 2;
    if (TNisColored() && (dow == i+1)) {
      color.r = 0xFF;  color.g = 0x00;  color.b = 0x00;
      TNSetTextColorRGB(&color, &prevColor);
    }
    WinDrawChars(text, StrLen(text), bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+i*gGadgetDaysWidth+i+leftoff+2, bounds.topLeft.y);
    if (dow == i+1) {
      if (TNisColored()) {
        TNSetTextColorRGB(&prevColor, NULL);
      } else {
        // Draw some kind of underline to determine current day
        Int16 lineWidth=FntLineWidth(text, StrLen(text));
        WinDrawLine(rect.topLeft.x+i*gGadgetDaysWidth+i+leftoff+1, rect.topLeft.y+FntLineHeight(),
                    rect.topLeft.x+i*gGadgetDaysWidth+i+leftoff+1+lineWidth, rect.topLeft.y+FntLineHeight());
      }
    }
    MemHandleUnlock(mh);
  }
}

/*****************************************************************************
* Function: GadgetDraw
*
* Description: Draws the basic gadget (grid, days, times)
*****************************************************************************/
void GadgetDraw(Boolean eraseFirst) {
  RectangleType bounds;
  UInt8 i, gadgetWidth;
  RGBColorType color, prevColor;
  UInt16 gadgetIndex;
  RectangleType rect;
  FormType *frm=gForm;

  // Check if GadgetSet has already been called. If not => die
  if (! frm) return;

  gadgetIndex = FrmGetObjectIndex(frm, gGadgetID);
  FrmGetObjectBounds(frm, gadgetIndex, &bounds);

  // Erase background
  if (eraseFirst)  WinEraseRectangle(&bounds, 0);

  // Write Times
  if (gGadgetCurScreen == GADGET_SCREEN_DAY) {
    for (i=0; i < 7; i++) { // 7 because we have to write 7 times 08:00, 10:00,..., 20:00
      Char time[timeStringLength];
      if (i < 1)  StrPrintF(time, "0%u:00", 8+i*2);
      else  StrPrintF(time, "%u:00", 8+i*2);
      WinDrawChars(time, StrLen(time), bounds.topLeft.x+GADGET_BASELEFT,
                   (i != 7 - 1) ? bounds.topLeft.y+GADGET_STRINGS_TOP+i*GADGET_STRINGS_OFFSET
                   /* ^last^ */ : bounds.topLeft.y+GADGET_STRINGS_TOP+i*GADGET_STRINGS_OFFSET-2);
    }
  } else {
    for (i=0; i < 2; i++) {
      Char time[timeStringLength];
      StrPrintF(time, "%u:00", 20+i*2);
      WinDrawChars(time, StrLen(time), bounds.topLeft.x+GADGET_BASELEFT,
                   bounds.topLeft.y+GADGET_STRINGS_TOP+i*GADGET_STRINGS_OFFSET);
    }
    for (i=2; i < 7; i++) {
      Char time[timeStringLength];
      StrPrintF(time, "0%u:00", (i-2)*2);
      WinDrawChars(time, StrLen(time), bounds.topLeft.x+GADGET_BASELEFT,
                   (i != 7 - 1) ? bounds.topLeft.y+GADGET_STRINGS_TOP+i*GADGET_STRINGS_OFFSET
                   /* ^last^ */ : bounds.topLeft.y+GADGET_STRINGS_TOP+i*GADGET_STRINGS_OFFSET-2);
    }
  }

  // Write Weekdays
  GadgetDrawWeekdays();

  // Draw Grid: Color is light gray on colored palms, dark gray on gray palms
  if (TNisColored()) {
    color.r = 0x77;  color.g = 0x77;  color.b = 0x77;
  } else {
    color.r = 0x44;  color.g = 0x44;  color.b = 0x44;
  }

  TNSetForeColorRGB(&color, &prevColor);

  gadgetWidth=gGadgetDaysNum * gGadgetDaysWidth + gGadgetDaysNum - 1;
  RctSetRectangle(&rect, bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT, bounds.topLeft.y+GADGET_TOP, gadgetWidth, GADGET_HEIGHT);
  WinDrawRectangleFrame(simpleFrame, &rect);
  for(i=1; i <= 5; ++i) { // Draw the 5 gray dashed lines
    if ( (gGadgetCurScreen == GADGET_SCREEN_NIGHT) && (i == 2)) {
      WinDrawLine(bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT, bounds.topLeft.y+GADGET_TOP+i*GADGET_STRINGS_OFFSET,
                  bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gadgetWidth-1, bounds.topLeft.y+GADGET_TOP+i*GADGET_STRINGS_OFFSET);
    } else {
      WinDrawGrayLine(bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT, bounds.topLeft.y+GADGET_TOP+i*GADGET_STRINGS_OFFSET,
                      bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gadgetWidth-1, bounds.topLeft.y+GADGET_TOP+i*GADGET_STRINGS_OFFSET);
    }
  }
  for(i=1; i <= gGadgetDaysNum -1; ++i) {
    WinDrawLine(bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+i*gGadgetDaysWidth+(i-1), bounds.topLeft.y+GADGET_TOP,
                bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+i*gGadgetDaysWidth+(i-1), bounds.topLeft.y+GADGET_TOP+GADGET_HEIGHT-1);
  }

  TNSetForeColorRGB(&prevColor, NULL);
}


/*****************************************************************************
* Function: GadgetDrawTimeline
*
* Description: Draws a line for the current time
*****************************************************************************/
void GadgetDrawTimeline(GadgetTimelineDrawType drawType) {
  DateTimeType dt;
  TimeType now;

  TimSecondsToDateTime(TimGetSeconds(), &dt);

  if (drawType == gtDraw) {
    now.hours=dt.hour;
    now.minutes=dt.minute;
    gGadgetLastTimeline.hours=now.hours;
    gGadgetLastTimeline.minutes=now.minutes;
    GadgetDrawWeekdays();
  } else {
    now.hours=gGadgetLastTimeline.hours;
    now.minutes=gGadgetLastTimeline.minutes;
  }

  if ( ((gGadgetCurScreen == GADGET_SCREEN_DAY) && (now.hours >= 8) && (now.hours < 20)) ||
       ((gGadgetCurScreen == GADGET_SCREEN_NIGHT) && ((now.hours < 8) || (now.hours >= 20))) ) {

    UInt8 y, gadgetWidth;
    UInt16 gadgetIndex;
    FormType *frm;
    RectangleType bounds;
    RGBColorType red={0x00, 0xFF, 0x00, 0x00}, old;

    y = GadgetCalcTimeTop(now);

    frm = FrmGetActiveForm();
    gadgetIndex = FrmGetObjectIndex(frm, gGadgetID);
    FrmGetObjectBounds(frm, gadgetIndex, &bounds);
    gadgetWidth=gGadgetDaysNum * gGadgetDaysWidth + gGadgetDaysNum - 1;

    WinInvertLine(bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT, bounds.topLeft.y+GADGET_TOP+y,
                  bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gadgetWidth-1, bounds.topLeft.y+GADGET_TOP+y);

    if (drawType == gtDraw) {
      TNSetForeColorRGB(&red, &old);
      WinDrawPixel(bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gGadgetDaysNum*gGadgetDaysWidth+gGadgetDaysNum+1, bounds.topLeft.y+GADGET_TOP+y);
      WinDrawLine(bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gGadgetDaysNum*gGadgetDaysWidth+gGadgetDaysNum+2, bounds.topLeft.y+GADGET_TOP+y-1,
                  bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gGadgetDaysNum*gGadgetDaysWidth+gGadgetDaysNum+2, bounds.topLeft.y+GADGET_TOP+y+1);
      WinDrawLine(bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gGadgetDaysNum*gGadgetDaysWidth+gGadgetDaysNum+3, bounds.topLeft.y+GADGET_TOP+y-2,
                  bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gGadgetDaysNum*gGadgetDaysWidth+gGadgetDaysNum+3, bounds.topLeft.y+GADGET_TOP+y+2);
      TNSetForeColorRGB(&old, NULL);
    } else if (drawType == gtErase) {
      RectangleType rect;
      RctSetRectangle(&rect,
                      bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gGadgetDaysNum*gGadgetDaysWidth+gGadgetDaysNum+1, bounds.topLeft.y+GADGET_TOP+y-2,
                      bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+gGadgetDaysNum*gGadgetDaysWidth+gGadgetDaysNum+3, bounds.topLeft.y+GADGET_TOP+y+2);
      WinEraseRectangle(&rect, 0);
    }
  }
}

    
/*****************************************************************************
* Function: GadgetRedraw
*
* Description: Redraws without erasing first, avoids flicker and does not
*              select next item, behaves more like the user expects
*****************************************************************************/
void GadgetRedraw() {
  if (gPrefs.showTimeline)  GadgetDrawTimeline(gtErase);
  GadgetDraw(gGadgetCompleteRedraw);
  GadgetDrawEvents();
  if (gPrefs.showTimeline)  GadgetDrawTimeline(gtDraw);
  if (gGadgetCompleteRedraw) {
    gHintDrawn=false;
    GadgetDrawHintNext();
  } else {
    GadgetDrawHintCurrent();
  }
  gGadgetCompleteRedraw=false;
}

inline Boolean
GadgetEventTimesCollide( UInt8 t1_day, UInt16 t1_begin, UInt16 t1_end,
			 UInt8 t2_day, UInt16 t2_begin, UInt16 t2_end )
{
  // days must be equal to have a collision
  // four cases have to be cared:
  // 1) t1 begin is in t2 == t2 end is in t1
  // 2) t1 end is in t2 == t2 begin is in t1
  // 3) t1 is in t2
  // 4) t2 is in t1

  return ( (t1_day == t2_day) &&
           (
	    ((t1_begin >= t2_begin) && (t1_begin < t2_end)) ||
	    ((t1_end > t2_begin) && (t1_end <= t2_end)) ||
	    ((t1_begin <= t2_begin) && (t1_end >= t2_end)) ||
	    ((t1_begin >= t2_begin) && (t1_end <= t2_end))
	    )
	  );
}

Boolean
GadgetEventsCollide(TimeDBRecord *t1, TimeDBRecord *t2)
{
  UInt16 t1_begin=TimeToInt(t1->begin), t1_end=TimeToInt(t1->end);
  UInt16 t2_begin=TimeToInt(t2->begin), t2_end=TimeToInt(t2->end);

  return GadgetEventTimesCollide(t1->day, t1_begin, t1_end,
				 t2->day, t2_begin, t2_end);
}


/*****************************************************************************
* Function: GadgetDrawEvents
*
* Description: Draws all events, used by GadgetRedraw and GadgetDrawComplete
*****************************************************************************/
void
GadgetDrawEvents()
{
  UInt8 pos = 0;
  UInt8 num_times = 0;
  UInt16 index=0, time_index;
  MemHandle m, ml; // ml = memhandle for look-ahead, would have to be an array for >2 MAX_AT_A_TIME
  TimeDBRecord *t = NULL;
  TimeDBRecord *tl = NULL; // look_ahead time

  UInt8 last_day = 0;
  UInt16 last_begin = 0, last_end = 0;
  UInt8 last_pos = 0;
  
  TNlistFree(gGadgetTimeList);
  gGadgetTimeList = NULL;

  ml = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat());

  while( ml != NULL ) {

    Boolean look_ahead_not_visible = true;

    m = ml;
    time_index = index;
    t=(TimeDBRecord *)MemHandleLock(m);

    while (look_ahead_not_visible) {
      index += 1;
      ml = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat());

      if (ml != NULL) {
	tl = (TimeDBRecord *)MemHandleLock(ml);
	if ( (tl->type == TYPE_TIME) && GadgetEventIsVisible(tl) ) {
	  look_ahead_not_visible = false;
	} else {
	  MemHandleUnlock(ml);
	  ml = NULL;
	  tl = NULL;
	}
      } else {
	break;
      }
    }

    if ((t->type == TYPE_TIME) && GadgetEventIsVisible(t) ) {

      RGBColorType c;
      GadgetTimeListType *gtl;
      UInt16 current_begin, current_end;

      current_begin = TimeToInt(t->begin);
      current_end = TimeToInt(t->end);

      // How many entries?
      if ( GadgetEventTimesCollide(last_day, last_begin, last_end,
				   t->day, current_begin, current_end) ) {
	// we collide with previous event, draw right
	if (last_pos == 0) {
	  pos = 1;
	} else {
	  pos = 0;
	}
	if (last_end < current_end) {
	  last_begin = current_begin;
	  last_end = current_end;
	  last_day = t->day;
	  last_pos = pos;
	}
	num_times = 2;
      } else {
	// We do not collide with last event, check if we collide with next event
	if ( (tl != NULL) && (tl->type == TYPE_TIME) && GadgetEventsCollide(t, tl) ) {
	  // Next event is visible and collides with current event 
	  last_begin = current_begin;
	  last_end = current_end;
	  last_day = t->day;
	  num_times = 2;
	  pos = 0;
	  last_pos = pos;
	} else {
	  // We do not collide with previous or next event => no collision
	  last_begin = TimeToInt(t->begin);
	  last_end = TimeToInt(t->end);
	  last_day = t->day;
	  num_times = 1;
	  pos = 0;	  
	  last_pos = pos;
	}
      }


      c.r=t->color[0];
      c.g=t->color[1];
      c.b=t->color[2];

      gtl = (GadgetTimeListType *)MemPtrNew(sizeof(GadgetTimeListType));
      gtl->index = time_index;
      gtl->num = num_times;
      gtl->pos = pos;
      GadgetTimeSetRect(&(gtl->rect), (TimeType)t->begin, (TimeType)t->end, t->day, num_times, pos);

      gGadgetTimeList = TNlistAppend(gGadgetTimeList, gtl);

      GadgetDrawTime((TimeType)t->begin, (TimeType)t->end, t->day, &c, t->course, num_times, pos);

    }
    MemHandleUnlock(m);
    if (ml != NULL) {
      MemHandleUnlock(ml);
      tl = NULL;
    }
  }
}

void
GadgetTimeSetRect(RectangleType *rect, TimeType begin, TimeType end, UInt8 day, UInt8 num_times, UInt8 pos)
{
  UInt8 top, height, width, width_base;
  RectangleType bounds;
  UInt16 gadgetIndex = FrmGetObjectIndex(gForm, gGadgetID);

  FrmGetObjectBounds(gForm, gadgetIndex, &bounds);

  height = GadgetCalcTimeHeight(begin, end);
  top = GadgetCalcTimeTop(begin);

  if ( (top + height) > GADGET_MAX_PIXELHEIGHT)
    height -= ((top + height) - GADGET_MAX_PIXELHEIGHT);

  width = gGadgetDaysWidth / num_times;
  width_base = width;
  if (pos == (GADGET_MAX_AT_A_TIME-1)) {
    // It's the last item, add any pixels that get cut off by the division otherwise
    // (like: width: 20, num_items = 3, item width = 6, 2 got cut off, add them to the last entry)
    width += (gGadgetDaysWidth - (num_times * width));
  }

  RctSetRectangle(rect,
		  // Left          Left Offset                 Days to left          lines between days  position offset
		  bounds.topLeft.x+GADGET_BASELEFT+GADGET_LEFT+(gGadgetDaysWidth*day)+day              + pos * width_base,
		  // Top           Top Offset      hours
		  bounds.topLeft.y+GADGET_TOP +top,
		  // Width
		  width,
		  // 2px per 15min
		  height);

}

/*****************************************************************************
* Function: GadgetDrawTime
*
* Description: Show a time in the grid
*****************************************************************************/
void
GadgetDrawTime(TimeType begin, TimeType end, UInt8 day, RGBColorType *color, UInt16 courseID, UInt8 num_times, UInt8 pos)
{
  RectangleType rect;
  RGBColorType prevColor, inverted;

  // Sort out bogus requests, could be more intelligent, maybe later...
  if (day >= gGadgetDaysNum) return;

  // do nothing if Gadget has not yet been GadgetSet
  if (! gForm) return;
  if (! gGadgetID) return;

  GadgetTimeSetRect(&rect, begin, end, day, num_times, pos);


  TNSetForeColorRGB(color, &prevColor);
  WinDrawRectangle(&rect, 0);

  if ( (gPrefs.showTypes || gPrefs.showShortNames) && (rect.extent.y >= FntLineHeight())) {

    RGBColorType oldBack, oldText;

    // Get inverted color
    inverted.r = 255 - color->r;
    inverted.g = 255 - color->g;
    inverted.b = 255 - color->b;

    RctSetRectangle(&rect, rect.topLeft.x+2, rect.topLeft.y, rect.extent.x-4, rect.extent.y);

    TNSetTextColorRGB(&inverted, &oldText);
    TNSetBackColorRGB(color, &oldBack);

    if (gPrefs.showTypes) {
      MemHandle shortName=MemHandleNew(1);;
      CourseTypeGetShortByCourseID(&shortName, courseID);
      TNDrawCharsToFitWidth((Char *)MemHandleLock(shortName), &rect);
      MemHandleUnlock(shortName);
      MemHandleFree(shortName);
    } else if (gPrefs.showShortNames) {
      MemHandle courseName=MemHandleNew(1);
      CourseGetName(courseID, &courseName, false);

      TNDrawCharsToFitWidth((Char *)MemHandleLock(courseName), &rect);

      MemHandleUnlock(courseName);
      MemHandleFree(courseName);
    }

    TNSetBackColorRGB(&oldBack, NULL);
    TNSetTextColorRGB(&oldText, NULL);
  }

  TNSetForeColorRGB(&prevColor, NULL);
}






/*****************************************************************************
* Function: GadgetDrawHint
*
* Description: Draw the hintbox with the hint
*****************************************************************************/
void GadgetDrawHint(const char *toptext, const char *bottext, UInt16 note) {
  RectangleType bounds;
  UInt16 gadgetIndex;
  RectangleType rect, textbox_top, textbox_bot;
  FontID oldFont;
  ControlType *ctl;
  Char noteSymb[2] = { GADGET_NOTESYMBOL, 0 };
  // Not needed any longer, the hint now has its own Gadget
  // UInt16 top = GADGET_TOP + (GADGET_STRINGS_NUM-1) * GADGET_STRINGS_OFFSET + 5;  // 6 - const = VALUE BELOW

  if (! gForm) return;

  gadgetIndex = FrmGetObjectIndex(gForm, gHintGadgetID);
  FrmGetObjectBounds(gForm, gadgetIndex, &bounds);

  WinEraseRectangle(&bounds, 0);

  RctSetRectangle(&rect,
                  bounds.topLeft.x+1,  // +1 for border
                  bounds.topLeft.y+1, // Put VALUE BELOW here.... +top+1 removed because of own Gadget
                  bounds.extent.x-3, bounds.extent.y - 3); // -4 for bottom border

  // Erase Gadget area
  WinSetForeColor(UIColorGetTableEntryIndex(UIObjectFrame));
  WinDrawRectangleFrame(popupFrame, &rect);

  RctSetRectangle(&textbox_top, rect.topLeft.x+2, rect.topLeft.y,
                                rect.extent.x-2,(Int16)((rect.extent.y-2)/2));
  RctSetRectangle(&textbox_bot, textbox_top.topLeft.x, textbox_top.topLeft.y+textbox_top.extent.y+1,
                                textbox_top.extent.x, textbox_top.extent.y);

  oldFont=FntSetFont(boldFont);
  TNDrawCharsToFitWidth(toptext, &textbox_top);
  FntSetFont(oldFont);
  TNDrawCharsToFitWidth(bottext, &textbox_bot);


  gHintNote = note;
  if (note) {
    // This time has a note
    oldFont = FntSetFont(symbolFont);
    RctSetRectangle(&rect, rect.topLeft.x+rect.extent.x-8,
                           rect.topLeft.y+rect.extent.y-12,
                           FntLineWidth(noteSymb, 1), FntLineHeight());
  
    TNDrawCharsToFitWidth(noteSymb, &rect);
    FntSetFont(oldFont);
  }

  ctl=GetObjectPtr(BUTTON_edit);
  CtlShowControl(ctl);
  ctl=GetObjectPtr(BUTTON_beam);
  CtlShowControl(ctl);
}




/*****************************************************************************
* Function: GadgetDrawHintErase
*
* Description: Erase the area where the hintbox goes
*****************************************************************************/
void GadgetDrawHintErase(void) {
  RectangleType bounds;
  UInt16 gadgetIndex;
  ControlType *ctl;

  if (! gForm) return;

  gadgetIndex = FrmGetObjectIndex(gForm, gHintGadgetID);
  FrmGetObjectBounds(gForm, gadgetIndex, &bounds);

  WinEraseRectangle(&bounds, 0);

  ctl=GetObjectPtr(BUTTON_edit);
  CtlHideControl(ctl);
  ctl=GetObjectPtr(BUTTON_beam);
  CtlHideControl(ctl);

}

/*****************************************************************************
* Function: GadgetDrawHintCurrent
*
* Description: Draw hint for current gTimeIndex if needed (may be forced NOT
*              to draw with GadgetSetNeedsRedraw(false));
*****************************************************************************/
void GadgetDrawHintCurrent(void) {
  Char *tmp, *bot, begin[timeStringLength], end[timeStringLength], *day;
  MemHandle mc, mt, mh, type;
  CourseDBRecord c;
  TimeDBRecord *tc;
  RectangleType rect, bounds;
  UInt16 gadgetIndex = FrmGetObjectIndex(gForm, gGadgetID);
  RGBColorType color, prevColor;
  UInt16 attr;

  // Need to check that due to damn f*** DmRecordInfo which will show a
  // fatal alert when called on non-existing record (this happens for example
  // right after creating the new database...
  if ((gTimeIndex >= DmNumRecords(DatabaseGetRef())) ||
      (gCourseIndex >= DmNumRecords(DatabaseGetRef())) )  return;

  if (gHintDrawn) {
    // Delete border around previous entry
    if ( (gTimeDrawnIndex < DmNumRecords(DatabaseGetRef())) &&
       (DmRecordInfo(DatabaseGetRef(), gTimeDrawnIndex, &attr, NULL, NULL) == errNone) ) {
      attr &= dmRecAttrCategoryMask;
      if (attr == DatabaseGetCat()) {
    
        mt = DmQueryRecord(DatabaseGetRef(), gTimeDrawnIndex);
        if (mt) {
          // mt may be null, for example if next is drawn after delete!
          tc = (TimeDBRecord *)MemHandleLock(mt);
          if ((tc->type == TYPE_TIME) && GadgetEventIsVisible(tc) ) {

	    TNlist *tmpl = gGadgetTimeList;
	    GadgetTimeListType *gtl = NULL;
	    while (tmpl != NULL) {
	      gtl = tmpl->data;
	      if (gtl->index == gTimeDrawnIndex) {
		break;
	      }
	      tmpl = tmpl->next;
	    }
 
            color.r=tc->color[0];
            color.g=tc->color[1];
            color.b=tc->color[2];
            if (gPrefs.showTimeline)  GadgetDrawTimeline(gtErase);

	    if (gtl != NULL) {
	      GadgetDrawTime(tc->begin, tc->end, tc->day, &color, tc->course, gtl->num, gtl->pos);
	    }
            if (gPrefs.showTimeline)  GadgetDrawTimeline(gtDraw);
          }
          MemHandleUnlock(mt);
        }
      }
    }
  }

  gTimeDrawnIndex=gTimeIndex;
  gHintDrawn=true;

  if (DmRecordInfo(DatabaseGetRef(), gCourseIndex, &attr, NULL, NULL) == errNone) {
    attr &= dmRecAttrCategoryMask;
    if (attr == DatabaseGetCat()) {
      // Record is in currently displayed category

      mc = DmQueryRecord(DatabaseGetRef(), gCourseIndex);
      if (! mc)  return;

      mt = DmQueryRecord(DatabaseGetRef(), gTimeIndex);
      if (! mt) return;

      UnpackCourse(&c, MemHandleLock(mc));
      tc = (TimeDBRecord *)MemHandleLock(mt);

      if ( GadgetEventIsVisible(tc) ) {

	TNlist *tmpl = gGadgetTimeList;
	GadgetTimeListType *gtl = NULL;
	while (tmpl != NULL) {
	  gtl = tmpl->data;
	  if (gtl->index == gTimeDrawnIndex) {
	    break;
	  }
	  tmpl = tmpl->next;
	}

        mh = DmGetResource(strRsc, GADGET_STRINGS_WDAYSTART+tc->day);
        day = (Char *)MemHandleLock(mh);

        // Lecture Name (Teacher) [Typ]
        tmp=(Char *)MemPtrNew(StrLen(c.name)+StrLen(c.teacherName)+4+3+CTYPE_SHORT_MAXLENGTH);
        MemSet(tmp, MemPtrSize(tmp), 0);
        type = MemHandleNew(1);
        CourseTypeGetShort(&type, c.ctype);
        StrPrintF(tmp, "%s (%s) [%s]", c.name, c.teacherName, (Char *)MemHandleLock(type));
        MemHandleUnlock(type);
        MemHandleFree(type);


        // Fr 08:00 - 09:30 (Room)          <-- Example
        // 3    5   3   5   3+StrLen(room)  <-- Num Chars for MemPtrNew
        bot=(Char *)MemPtrNew(20+sizeof(tc->room)+MemPtrSize(day));
        MemSet(bot, MemPtrSize(bot), 0);

        TimeToAscii(tc->begin.hours, tc->begin.minutes, GadgetGetTimeFormat(), begin);
        TimeToAscii(tc->end.hours, tc->end.minutes, GadgetGetTimeFormat(), end);

        mh = DmGetResource(strRsc, GADGET_STRINGS_WDAYSTART+tc->day);

        StrPrintF(bot, "%s %s - %s (%s)", day, begin, end, tc->room);

        FrmGetObjectBounds(gForm, gadgetIndex, &bounds);

	if (gtl != NULL) {
	  GadgetTimeSetRect(&rect, tc->begin, tc->end, tc->day, gtl->num, gtl->pos);
	  RctSetRectangle(&rect,
			  //             + inset (two boxes, one black, one white)
			  rect.topLeft.x + 2,
			  rect.topLeft.y + 2,
			  // width      - 2 * inset
			  rect.extent.x - 4,
			  // height     - 2 * inset
			  rect.extent.y - 4
			  );

	  /* Invert color, looks not so nice aka bad
	     color.r=255- tc->color[0];
	     color.g=255- tc->color[1];
	     color.b=255- tc->color[2];
	  */
	  color.r=255;
	  color.g=255;
	  color.b=255;
	  TNSetForeColorRGB(&color, &prevColor);
	  WinDrawRectangleFrame(simpleFrame, &rect);
	  color.r=0;
	  color.g=0;
	  color.b=0;
	  RctSetRectangle(&rect, rect.topLeft.x-1, rect.topLeft.y-1, rect.extent.x+2, rect.extent.y+2);
	  TNSetForeColorRGB(&color, NULL);
	  WinDrawRectangleFrame(simpleFrame, &rect);
	  TNSetForeColorRGB(&prevColor, NULL);
	}

       // WinInvertRectangleFrame(simpleFrame, &rect);
        GadgetDrawHint(tmp, bot, tc->note);

        MemPtrFree((MemPtr) tmp);
        MemPtrFree((MemPtr) bot);
        MemHandleUnlock(mh);
      } else {
        MemHandleUnlock(mc);
        MemHandleUnlock(mt);
        GadgetDrawHintNext();
        return;
      }
      MemHandleUnlock(mc);
      MemHandleUnlock(mt);
    } // End attr == current category
  }
}



/*****************************************************************************
* Function: GadgetDrawHintNext
* Complexity: O(N), N = DmNumRecords
*
* Description: Draw info for closest upcoming event.
*****************************************************************************/
void GadgetDrawHintNext(void) {
  MemHandle m;
  UInt16 index=0;
  DateTimeType dt;
  Boolean found=false, foundFirst=false;
  UInt16 curTime, curMinTind=0, wantCourse=0, curMinTime=0xFFFF;
  Int16 diffTime;
  TimeDBRecord *t;

  TimSecondsToDateTime(TimGetSeconds(), &dt);

  if ((dt.weekDay > 0) && (dt.weekDay <= gGadgetDaysNum)) {
    dt.weekDay = dt.weekDay-1;
  } else {
    dt.weekDay = 0;
    dt.hour = 0;
    dt.minute = 0;
  }

  curTime = dt.weekDay * GADGET_DAYVALUE + dt.hour * 60 + dt.minute;

  // Search for record nearest in _past_ to current time
  // NOTE: We assume that the types are sorted "times -> courses"! We use that to reduce everything to ONE while loop
  while( !found && ((m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL)) {
    Char *s=MemHandleLock(m);
    if (s[0] == TYPE_TIME) {
      t = (TimeDBRecord *)s;
      if ( GadgetEventIsVisible(t) ) {
        if (! foundFirst) {
          foundFirst = true;
          curMinTind = index;
          wantCourse = t->course;
        }

        diffTime = (t->day * GADGET_DAYVALUE + t->begin.hours * 60 + t->begin.minutes) - curTime;
        if ( (diffTime > 0) && (diffTime < curMinTime) ) {
          curMinTind = index;
          curMinTime = diffTime;
          wantCourse = t->course;
        }
      }
    } else if (s[0] == TYPE_COURSE) {
      CourseDBRecord c;
      UnpackCourse(&c, s);
      if (c.id == wantCourse) {
        gCourseIndex = index;
        gTimeIndex = curMinTind;
        found=true;
      }
    }
    MemHandleUnlock(m);
    index += 1;
  }

  if (! found || ! foundFirst)  GadgetDrawHintErase();
  else                          GadgetDrawHintCurrent();

}


/*****************************************************************************
* Function: GadgetHandler
*
* Description: We use extended Gadgeting => We need an event handler for it
*****************************************************************************/
Boolean GadgetHandler(FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP) {
  Boolean handled = false; 
  EventType *event = (EventType *)paramP;
  
  switch (cmd) { 
    case formGadgetDrawCmd: 
      //Sent to active gadgets any time form is  
      //drawn or redrawn. 
      GadgetRedraw();
      gadgetP->attr.visible = true; 
      handled = true; 
      break; 
       
    case formGadgetHandleEventCmd: 
      //Sent when form receives a gadget event.  
      //paramP points to EventType structure.

      if (event->eType == frmGadgetEnterEvent) { 
        // penDown in gadget's bounds.
        GadgetTap((FormGadgetType *)gadgetP, event);
        handled = true;
      }
      if (event->eType == frmGadgetMiscEvent) {
        //This event is sent by your application
        //when it needs to send info to the gadget
      }
      break;

    case formGadgetDeleteCmd:
      //Perform any cleanup prior to deletion.
      TNlistFree(gGadgetTimeList);
      gGadgetTimeList = NULL;
      break;

    case formGadgetEraseCmd:
      //FrmHideObject takes care of this if you 
      //return false.
       handled = false;
       break;

    default:
      break;

  }
  return handled;
}


Boolean GadgetHintHandler(FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP) {
  Boolean handled = false; 
  EventType *event = (EventType *)paramP;

  if (cmd == formGadgetDrawCmd) {
    GadgetDrawHintCurrent();
  } else if (cmd == formGadgetHandleEventCmd) {
    if (event->eType == frmGadgetEnterEvent) { 
      // penDown in gadget's bounds.
      GadgetHintTap((FormGadgetType *)gadgetP, event);
      handled = true;
    }
  }
  return handled;
}


/*****************************************************************************
* Function: GadgetDrawStep
*
* Description: Helper function for GadgetTap. Highlights next of previous
*              event on current screen
*****************************************************************************/
void GadgetDrawStep(WinDirectionType direction) {
  MemHandle m;
  UInt16 index=gTimeIndex, wantCourse=0, courseIndex=0;
  Boolean found=false, endLoop=false;
  TimeDBRecord *t=NULL;

  /* ASSUMPTION: We assume that the user does not have 2^16 events
   * in his schedule. Otherwise this can become an endless. loop. but
   * since I think this is a reasonable assumption I will not take any
   * special care for this...
   * If we loop to ID 2^16-1 we will surely find no record and terminate
   * below (m = ... == NULL)
   */

  
  if ( (direction == winUp) && (index > 0) ) {
    while( !endLoop && (DmSeekRecordInCategory(DatabaseGetRefN(DB_MAIN), &index, 1, dmSeekBackward, DatabaseGetCat()) == errNone)) {
      Char *s;
      m = DmQueryRecord(DatabaseGetRefN(DB_MAIN), index);
      s=MemHandleLock(m);
      if (s[0] == TYPE_TIME) {
        t = (TimeDBRecord *)s;
        if ( GadgetEventIsVisible(t) ) {
          // Found entry
          endLoop=true;
          found = true;
          wantCourse = t->course;
        }
      } else {
        // nothing more to search and nothing found
        endLoop=true;
      }
      MemHandleUnlock(m);
    }
  } else if (direction == winDown) {
    index += 1;
    while( !endLoop && ((m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL)) {
      Char *s=MemHandleLock(m);
      if (s[0] == TYPE_TIME) {
        t = (TimeDBRecord *)s;
        if ( GadgetEventIsVisible(t) ) {
          // Found entry
          endLoop=true;
          found = true;
          wantCourse = t->course;
        } else {
          // previous in DB is not visible, search further
          index += 1;
        }
      } else {
        // nothing more to search and nothing found
        endLoop=true;
      }
      MemHandleUnlock(m);
    }
  }
  if (found && CourseGetIndex(DatabaseGetRef(), DatabaseGetCat(), wantCourse, &courseIndex)) {
    GadgetSetHintCourseIndex(courseIndex);
    GadgetSetHintTimeIndex(index);
    GadgetDrawHintCurrent();
  }
}


/*****************************************************************************
* Function: GadgetTap
*
* Description: Handles penDown events (taps) on the gadget
*****************************************************************************/
void GadgetTap(FormGadgetType *pGadget, EventType *event) {
  //you may find it useful to track if they
  //lift the pen still within the boundaries of the gadget
  Boolean isPenDown = true;
  Int16 newPointX, newPointY, startPointX, startPointY;
  UInt16 index;
  RectangleType bounds;

  // This is just needed since we do not want to access internal structure
  // data directly in FormGadgetType (need rect field below)
  index = TNGetObjectIndexFromPtr(FrmGetActiveForm(), pGadget);
  FrmGetObjectBounds(FrmGetActiveForm(), index, &bounds);

  //track the pen down event
  EvtGetPen(&newPointX, &newPointY, &isPenDown);
  startPointX = newPointX;
  startPointY = newPointY;
  while (isPenDown){
    EvtGetPen(&newPointX, &newPointY, &isPenDown);
  }

  if (RctPtInRectangle(newPointX, newPointY, &bounds)) {
    /* the pen up was also in the gadget
       This can mean two things:
       1) we got a strike command
       2) a field was tapped
    */
    RectangleType topRight, bottomRight;
    Boolean found=false, foundTime=false;
    MemHandle m;
    TimeDBRecord *t;
    UInt16 index=0, wantCourse=0, tmp3_4th=0;
    GadgetTimeListType *gtl;
    TNlist *tmpl;

    /* Check for stroke commands */

    tmp3_4th = (3 * bounds.extent.x) / 4;
    RctSetRectangle(&topRight,
                    bounds.topLeft.x+ tmp3_4th,
                    bounds.topLeft.y,
                    bounds.extent.x - tmp3_4th,
                    bounds.extent.y/2
                   );
    RctSetRectangle(&bottomRight,
                    bounds.topLeft.x + tmp3_4th,
                    bounds.topLeft.y + (bounds.extent.y/2),
                    bounds.extent.x - tmp3_4th,
                    bounds.extent.y / 2
                   );

    if (RctPtInRectangle(startPointX, startPointY, &bottomRight) &&
        RctPtInRectangle(newPointX, newPointY, &topRight) ) {
      // Stroke Command: BACK
      GadgetDrawStep(winUp);
      return;
    } else if (RctPtInRectangle(startPointX, startPointY, &topRight) &&
               RctPtInRectangle(newPointX, newPointY, &bottomRight) ) {
      // Stroke Command: NEXT
      GadgetDrawStep(winDown);
      return;
    }


    tmpl = gGadgetTimeList;
    while (tmpl != NULL) {
      gtl = (GadgetTimeListType *)tmpl->data;
      if (RctPtInRectangle(newPointX, newPointY, &(gtl->rect))) {
	m = DmQueryRecord(DatabaseGetRef(), gtl->index);
        if (m) {
          // mt may be null, for example if next is drawn after delete!
          t = (TimeDBRecord *)MemHandleLock(m);

	  // we got a match
	  wantCourse=t->course;
	  gTimeIndex=gtl->index;
	  foundTime=true;

	  MemHandleUnlock(m);
	}
      }
      tmpl = tmpl->next;
    }

    // Search for the clicked time
    while(! found && ((m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL)) {
      Char *s = (Char *)MemHandleLock(m);
      if (s[0] == TYPE_COURSE) {
	if (foundTime) {
	  CourseDBRecord c;
	  UnpackCourse(&c, s);
	  if (c.id == wantCourse) {
	    SndPlaySystemSound(sndClick);
	    gCourseIndex = index;
	  }
	}
	found=true;
      }
      MemHandleUnlock(m);
      index += 1;
    }

    if (found && foundTime) {
      GadgetDrawHintCurrent();
    } else {
      SndPlaySystemSound(sndError);
    }

  } // else outside gadget bounds -> do nothing

}


/*****************************************************************************
* Function: GadgetHintTap
*
* Description: Handles penDown events (taps) on the hint gadget
*****************************************************************************/
void GadgetHintTap(FormGadgetType *pGadget, EventType *event) {
  //you may find it useful to track if they
  //lift the pen still within the boundaries of the gadget
  Boolean isPenDown = true;
  Int16 newPointX, newPointY, startPointX, startPointY;
  UInt16 index;
  RectangleType bounds, rect;
  Char noteSymb[2] = { GADGET_NOTESYMBOL, 0 };
  FontID oldFont;
  Boolean drawn=false;
  IndexedColorType curForeColor, curBackColor, curTextColor;

  // This is just needed since we do not want to access internal structure
  // data directly in FormGadgetType (need rect field below)
  index = TNGetObjectIndexFromPtr(FrmGetActiveForm(), pGadget);
  FrmGetObjectBounds(FrmGetActiveForm(), index, &bounds);

  oldFont = FntSetFont(symbolFont);
  RctSetRectangle(&rect,
                  bounds.topLeft.x+1,  // +1 for border
                  bounds.topLeft.y+1, // Put VALUE BELOW here.... +top+1 removed because of own Gadget
                  bounds.extent.x-3, bounds.extent.y - 3); // -4 for bottom border
  RctSetRectangle(&rect, rect.topLeft.x+rect.extent.x-8,
                         rect.topLeft.y+rect.extent.y-12,
                         FntLineWidth(noteSymb, 1), FntLineHeight());


  //track the pen down event
  EvtGetPen(&newPointX, &newPointY, &isPenDown);
  startPointX = newPointX;
  startPointY = newPointY;
  while (isPenDown){
    EvtGetPen(&newPointX, &newPointY, &isPenDown);

    if (gHintNote) {
      if (! drawn && RctPtInRectangle(newPointX, newPointY, &rect)) {
        curForeColor = WinSetForeColor(UIColorGetTableEntryIndex(UIObjectSelectedForeground));
        curBackColor = WinSetBackColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
        curTextColor = WinSetTextColor(UIColorGetTableEntryIndex(UIObjectSelectedForeground));
        TNDrawCharsToFitWidth(noteSymb, &rect);
        WinSetForeColor(curForeColor);
        WinSetForeColor(curBackColor);
        WinSetForeColor(curTextColor);
        drawn = true;
      } else if (drawn && ! RctPtInRectangle(newPointX, newPointY, &rect)) {
        curForeColor = WinSetForeColor(UIColorGetTableEntryIndex(UIObjectForeground));
        curBackColor = WinSetBackColor(UIColorGetTableEntryIndex(UIObjectFill));
        curTextColor = WinSetTextColor(UIColorGetTableEntryIndex(UIObjectForeground));
        TNDrawCharsToFitWidth(noteSymb, &rect);
        WinSetForeColor(curForeColor);
        WinSetForeColor(curBackColor);
        WinSetForeColor(curTextColor);
        drawn = false;
      }
    }

  }

  FntSetFont(oldFont);

  if (gHintNote && RctPtInRectangle(newPointX, newPointY, &rect)) {
    NoteSet(GadgetGetHintTimeIndex(), FORM_main);
    FrmPopupForm(NewNoteView);
  } // else outside gadget bounds -> do nothing
}



/*****************************************************************************
* Function: GadgetSet
*
* Description: Must be called before draw to set the form the gadget is in
*              and the IDs of the Gadget and the hint gadget
*****************************************************************************/
void GadgetSet(FormPtr frm, UInt16 gadgetID, UInt16 hintGadgetID) {
  gForm=frm;
  gGadgetID=gadgetID;
  gHintGadgetID=hintGadgetID;
  gGadgetDaysNum=gPrefs.numDays;
  gGadgetDaysWidth=GADGET_TOTAL_DRAWWIDTH / gGadgetDaysNum;
  gCourseIndex=0;
  gTimeIndex=0;
  gTimeDrawnIndex=0;
  gHintDrawn=false;
  gGadgetCompleteRedraw=true;
}


/*****************************************************************************
* Function: GadgetSetNumDays
*
* Description: Sets number of days
*****************************************************************************/
void GadgetSetNumDays(UInt8 numDays) {
  gGadgetDaysNum=numDays;
  gGadgetDaysWidth=GADGET_TOTAL_DRAWWIDTH / numDays;
  gGadgetCompleteRedraw=true;
}


/*****************************************************************************
* Function: GadgetGetHintCourseIndex
*
* Description: Get DB Index of current "hinted" entry's course
*****************************************************************************/
UInt16 GadgetGetHintCourseIndex(void) {
  return gCourseIndex;
}


/*****************************************************************************
* Function: GadgetSetHintCourseIndex
*
* Description: Set DB Index of current "hinted" entry's course
*****************************************************************************/
void GadgetSetHintCourseIndex(UInt16 newCourseInd) {
  gCourseIndex = newCourseInd;
}


/*****************************************************************************
* Function: GadgetSetHintTimeIndex
*
* Description: Set DB Index of current "hinted" entry's time
*****************************************************************************/
void GadgetSetHintTimeIndex(UInt16 newTimeInd) {
  gTimeIndex = newTimeInd;
}


/*****************************************************************************
* Function: GadgetGetHintTimeIndex
*
* Description: Get DB Index of current "hinted" entry's time
*****************************************************************************/
UInt16 GadgetGetHintTimeIndex(void) {
  return gTimeIndex;
}


/*****************************************************************************
* Function: GadgetGetTimeFormat
*
* Description: Returns current TimeFormat
*****************************************************************************/
TimeFormatType GadgetGetTimeFormat(void) {
  return (TimeFormatType) PrefGetPreference(prefTimeFormat);
}


/*****************************************************************************
* Function: GadgetSetNeedsCompleteRedraw
*
* Description: On next GadgetRedraw we erase first
*****************************************************************************/
void GadgetSetNeedsCompleteRedraw(Boolean need) {
  gGadgetCompleteRedraw=need;
  CacheReset();
}

/*****************************************************************************
* Function: GadgetSwitchScreen
*
* Description: Switch Screens
*****************************************************************************/
void GadgetSwitchScreen(void) {
  gGadgetCurScreen=3-gGadgetCurScreen;
  gHintDrawn=false;
  gGadgetCompleteRedraw=true;
  GadgetRedraw();
}
