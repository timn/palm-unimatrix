/* $Id: edit.c,v 1.5 2003/10/15 21:42:49 tim Exp $
 *
 * Code for editing times, events, courses
 */

#include "UniMatrix.h"
#include "edit.h"
#include "database.h"
#include "gadget.h"
#include "delete.h"
#include "beam.h"
#include "clist.h"
#include "ctype.h"
#include "tnglue.h"
#include "cache.h"
#include "notes.h"

// Convert a date in a DateType structure to a signed int.
#define TimeToInt(time) (*(Int16 *) &time)


UInt16 gEditTimeNumCourses=0, gEditCourseID=0, gEditCourseInd=0;
Char **gEditTimeItemList, gEditTimeTrigger[2*timeStringLength+4], gEditCourseTypeTrigger[CTYPE_MAXLENGTH+4+CTYPE_SHORT_MAXLENGTH];
UInt16 *gEditTimeItemIDs=NULL, *gEditTimeItemInd=NULL;
TimeType gEditTimeBegin, gEditTimeEnd;
UInt8 gEditCourseType=0;
Boolean gEditTimeIsAdd=0;

extern UInt16 gMenuCurrentForm;

/*****************************************************************************
* Functions for editing COURSES
*****************************************************************************/
static void EditCourseFormInit(FormType *frm) {
  ControlType *ctl;

  ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_cd_type_trigger));

  if (gEditTimeIsAdd) {
    gEditCourseType=0;
  } else {
    MemHandle mc, mname, mteacher, memail, mwebsite, mphone, old;
    CourseDBRecord c;
    FieldType *fName, *fTeacher, *fEmail, *fWeb, *fPhone;
    Char *buffer;

    mc = DmQueryRecord(DatabaseGetRef(), gEditCourseInd);
    UnpackCourse(&c, MemHandleLock(mc));
    gEditCourseID=c.id;
    gEditCourseType=c.ctype;

    fName = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_name));
    fTeacher = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_teacher));
    fEmail = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_email));
    fWeb = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_website));
    fPhone = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_phone));

    mname=MemHandleNew(StrLen(c.name)+1);
    mteacher=MemHandleNew(StrLen(c.teacherName)+1);
    memail=MemHandleNew(StrLen(c.teacherEmail)+1);
    mwebsite=MemHandleNew(StrLen(c.website)+1);
    mphone=MemHandleNew(StrLen(c.teacherPhone)+1);


    // Copy contents to the memory handle
    buffer=(Char *)MemHandleLock(mname);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrNCopy(buffer, c.name, StrLen(c.name));
    MemHandleUnlock(mname);

    buffer=(Char *)MemHandleLock(mteacher);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrNCopy(buffer, c.teacherName, StrLen(c.teacherName));
    MemHandleUnlock(mteacher);

    buffer=(Char *)MemHandleLock(memail);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrNCopy(buffer, c.teacherEmail, StrLen(c.teacherEmail));
    MemHandleUnlock(memail);

    buffer=(Char *)MemHandleLock(mwebsite);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrNCopy(buffer, c.website, StrLen(c.website));
    MemHandleUnlock(mwebsite);

    buffer=(Char *)MemHandleLock(mphone);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrNCopy(buffer, c.teacherPhone, StrLen(c.teacherPhone));
    MemHandleUnlock(mphone);


    // Load fields
    old = FldGetTextHandle(fName);
    FldSetTextHandle(fName, mname);
    if (old != NULL)    MemHandleFree(old); 

    old = FldGetTextHandle(fEmail);
    FldSetTextHandle(fEmail, memail);
    if (old != NULL)    MemHandleFree(old); 

    old = FldGetTextHandle(fTeacher);
    FldSetTextHandle(fTeacher, mteacher);
    if (old != NULL)    MemHandleFree(old); 

    old = FldGetTextHandle(fWeb);
    FldSetTextHandle(fWeb, mwebsite);
    if (old != NULL)    MemHandleFree(old); 

    old = FldGetTextHandle(fPhone);
    FldSetTextHandle(fPhone, mphone);
    if (old != NULL)    MemHandleFree(old); 

    MemHandleUnlock(mc);
  }

  CourseTypeGetName(gEditCourseTypeTrigger, gEditCourseType);
  CtlSetLabel(ctl, gEditCourseTypeTrigger);


}

static Boolean EditCourseSave(FormType *frm) {
  ListType *lst;
  MemHandle mc;
  FieldType *fName, *fTeacher, *fEmail, *fWeb, *fPhone;
  Char *name, *email, *teacher, *website, *phone;

  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_cd_type));

  fName = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_name));
  fTeacher = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_teacher));
  fEmail = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_email));
  fWeb = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_website));
  fPhone = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_cd_phone));

  name = FldGetTextPtr(fName);
  email = FldGetTextPtr(fEmail);
  teacher = FldGetTextPtr(fTeacher);
  website = FldGetTextPtr(fWeb);
  phone = FldGetTextPtr(fPhone);

  if ( (name == NULL) || (StrLen(name) == 0)) {
    FrmAlert(ALERT_ec_noname);
    return false;
  } else if ( (teacher == NULL) || (StrLen(teacher) == 0)) {
    FrmAlert(ALERT_ec_noteacher);
    return false;
  } else {
    // Just need to change values, since we have opened the record directly...
    CourseDBRecord cr;
    Char empty[1]={'\0'};
    UInt16 index=0;

    // Create or load record
    if (gEditTimeIsAdd) {
      mc = DmNewRecord(DatabaseGetRef(), &index, 1);
    } else {
      index = gEditCourseInd;
      mc = DmGetRecord(DatabaseGetRef(), index);
    }

    if (email == NULL) email=empty;
    if (website == NULL) website=empty;
    if (phone == NULL) phone=empty;

    cr.type=TYPE_COURSE;
    cr.ctype=gEditCourseType;

    if (gEditTimeIsAdd) {
      cr.id = CourseNewID(DatabaseGetRefN(DB_MAIN), DatabaseGetCat());
    } else {
      cr.id=gEditCourseID;
    }

    cr.name=name;
    cr.teacherName=teacher;
    cr.teacherPhone=phone;
    cr.teacherEmail=email;
    cr.website=website;

    PackCourse(&cr, mc);

    DmReleaseRecord(DatabaseGetRefN(DB_MAIN), index, false);

    if (gEditTimeIsAdd) {
      UInt16 attr=0;
      DmRecordInfo(DatabaseGetRefN(DB_MAIN), index, &attr, NULL, NULL);
      attr |= DatabaseGetCat();
      DmSetRecordInfo(DatabaseGetRefN(DB_MAIN), index, &attr, NULL);
    }

    DatabaseSort();
    CacheReset();
  }

  return true;
}


void EditCourse(UInt16 courseInd) {
  gEditCourseInd=courseInd;
  gEditTimeIsAdd=false;
  FrmPopupForm(FORM_course_det);
}

void AddCourse(void) {
  gEditTimeIsAdd=true;
  FrmPopupForm(FORM_course_det);
}

Boolean EditCourseFormHandleEvent(EventPtr event) {
  Boolean handled = false;
  FormType *frm=FrmGetActiveForm();

  if (event->eType == ctlSelectEvent) {
    switch (event->data.ctlSelect.controlID) {

      case BUTTON_cd_ok:
        if (EditCourseSave(frm)) {
          // Not (yet) needed: EditCourseFormFree();
          FrmReturnToForm(gMenuCurrentForm);
          FrmUpdateForm(gMenuCurrentForm, frmRedrawUpdateCode);
        }
        handled=true;
        break;

      case BUTTON_cd_cancel:
        // Not (yet) needed: EditCourseFormFree();
        FrmReturnToForm(gMenuCurrentForm);
        FrmUpdateForm(gMenuCurrentForm, frmRedrawUpdateCode);
        handled=true;
        break;


      case LIST_cd_type_trigger:
        gEditCourseType = CourseTypeListPopup(LIST_cd_type, LIST_cd_type_trigger, gEditCourseType, gEditCourseTypeTrigger);
        CourseTypeGetName(gEditCourseTypeTrigger, gEditCourseType);
        CtlSetLabel(GetObjectPtr(LIST_cd_type_trigger), gEditCourseTypeTrigger);
        handled=true;
        break;


      default:
        break;
    }   
  } else if (event->eType == frmUpdateEvent) {
      // redraws the form if needed
      ControlType *ctl;
      frm = FrmGetActiveForm();
      ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_cd_type_trigger));

      CourseTypeGetName(gEditCourseTypeTrigger, gEditCourseType);
      CtlSetLabel(ctl, gEditCourseTypeTrigger);

      FrmDrawForm (frm);
      handled = true;
    } else if (event->eType == frmOpenEvent) {
      // initializes and draws the form at program launch
      frm = FrmGetActiveForm();
      EditCourseFormInit(frm);
      FrmDrawForm (frm);
      handled = true;
    } else if (event->eType == frmCloseEvent) {
      // this is done if program is closed
      // usually NOT called, since we use FrmReturnToForm!
      // But maybe if user just presses home button while form is shown
      
    }



  return handled;
}


/*****************************************************************************
* Functions for editing CATEGORIES
*****************************************************************************/

void CatPreEdit(UInt16 numRecords, UInt32 *recordList) {
  UInt16 index=0;
  UInt16 i=0;

  while( (i < numRecords) && (DmSeekRecordInCategory(DatabaseGetRef(), &index, 0, dmSeekForward, DELETE_CATEGORY) == errNone)) {
    DmRecordInfo(DatabaseGetRef(), index, NULL, &recordList[i], NULL);
    i += 1;
    index += 1;
  }
}

void CatPostEdit(UInt16 numRecords, UInt32 *recordList) {
  UInt16 index=0;
  Char categoryName[dmCategoryLength];

  CategoryGetName(DatabaseGetRef(), 0, categoryName);
  if (StrLen(categoryName) == 0) {
    MemHandle m;
    UInt16 index=0;
    while( (m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL) {
      DmRemoveRecord(DatabaseGetRef(), index);
      index = 0;
    }
  } else {
    while( (DmSeekRecordInCategory(DatabaseGetRef(), &index, 0, dmSeekForward, DELETE_CATEGORY) == errNone)) {
      UInt32 uniqueID;
      UInt16 i;
      Boolean found=false;
      DmRecordInfo(DatabaseGetRef(), index, NULL, &uniqueID, NULL);

      // VERY bad search, should do better after DatAlg, but just no bock right now :-)
      for (i=0; (!found && (i < numRecords)); ++i)
        if (recordList[i] == uniqueID)    found=true;

      if (! found)    DmRemoveRecord(DatabaseGetRef(), index);
      else            index += 1;
    }
  }
  DatabaseSort();
}





/*****************************************************************************
* Functions for editing EVENTS (also referred to as times)
*****************************************************************************/

void EditTime(void) {
  gEditTimeIsAdd=false;
  FrmPopupForm(FORM_evt_det);
}

void AddTime(void) {
  gEditTimeIsAdd=true;
  FrmPopupForm(FORM_evt_det);
}

static void EditTimeSetColor(FormType *frm, RGBColorType *color) {
  MemHandle mr, mg, mb, old;
  FieldType *r, *g, *b;
  Char *buffer;
  
  r = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_r));
  g = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_g));
  b = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_b));

  mr=MemHandleNew(4); // 4 is 3 digits + \0
  mg=MemHandleNew(4); // 4 is 3 digits + \0
  mb=MemHandleNew(4); // 4 is 3 digits + \0

  // Copy contents to the memory handle
  buffer=(Char *)MemHandleLock(mr);
  MemSet(buffer, MemPtrSize(buffer), 0);
  StrPrintF(buffer, "%u", color->r);
  MemHandleUnlock(mr);

  buffer=(Char *)MemHandleLock(mg);
  MemSet(buffer, MemPtrSize(buffer), 0);
  StrPrintF(buffer, "%u", color->g);
  MemHandleUnlock(mg);

  buffer=(Char *)MemHandleLock(mb);
  MemSet(buffer, MemPtrSize(buffer), 0);
  StrPrintF(buffer, "%u", color->b);
  MemHandleUnlock(mb);

  // Load fields
  old = FldGetTextHandle(r);
  FldSetTextHandle(r, mr);
  if (old != NULL)    MemHandleFree(old); 

  old = FldGetTextHandle(g);
  FldSetTextHandle(g, mg);
  if (old != NULL)    MemHandleFree(old); 

  old = FldGetTextHandle(b);
  FldSetTextHandle(b, mb);
  if (old != NULL)    MemHandleFree(old);

  FldDrawField(r);
  FldDrawField(g);
  FldDrawField(b);
}

static void EditTimeFindColor(FormType *frm, UInt16 courseID) {
  UInt16 index=0;
  MemHandle m;
  TimeDBRecord *t;
  RGBColorType color;
  Boolean found=false;

  color.r=0; color.b=0; color.g=0;
  while(!found && ((m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL)) {
    t=(TimeDBRecord *)MemHandleLock(m);
    if ((t->type == TYPE_TIME) && (t->course == courseID)) {
      color.r=t->color[0];
      color.g=t->color[1];
      color.b=t->color[2];
      found = true;
    }
    MemHandleUnlock(m);
    index += 1;
  }
  if (found || (! (FldDirty(GetObjectPtr(FIELD_ed_r)) ||
                   FldDirty(GetObjectPtr(FIELD_ed_g)) ||
                   FldDirty(GetObjectPtr(FIELD_ed_b)) )) ) {
    EditTimeSetColor(frm, &color);
  }
}

static void EditTimeFormInit(FormType *frm) {
  UInt16 selectedCourse=0;
  MemHandle mt, mroom, old;
  ListType *lst, *day;
  ControlType *ctl, *dayt;
  FieldType *room;
  TimeDBRecord *t;
  Char *buffer;
  RGBColorType color;

  gEditTimeNumCourses = CountCourses();

  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_ed_course));
  day = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_ed_day));

  ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_ed_course_trigger));
  dayt = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_ed_day_trigger));

  gEditTimeItemList = (Char **) MemPtrNew(gEditTimeNumCourses * sizeof(Char *));
  gEditTimeItemIDs = (UInt16 *) MemPtrNew(gEditTimeNumCourses * sizeof(UInt16));
  gEditTimeItemInd = (UInt16 *) MemPtrNew(gEditTimeNumCourses * sizeof(UInt16));

  if (gEditTimeIsAdd) {
    // Init form as "add" form
  
    gEditTimeBegin.hours = 8;
    gEditTimeBegin.minutes = 15;
    gEditTimeEnd.hours = 9;
    gEditTimeEnd.minutes = 45;

    selectedCourse = CourseListGen(gEditTimeItemList, gEditTimeItemIDs, gEditTimeItemInd, gEditTimeNumCourses, 0, CLIST_SEARCH_INDEX);

    EditTimeFindColor(frm, gEditTimeItemIDs[selectedCourse]);

    LstSetSelection(day, 0);
    CtlSetLabel(dayt, LstGetSelectionText(day, 0));

    
  } else {
    // We edit a record

    mt = DmQueryRecord(DatabaseGetRef(), GadgetGetHintTimeIndex());
    t = (TimeDBRecord *)MemHandleLock(mt);

    gEditTimeBegin.hours=t->begin.hours;
    gEditTimeBegin.minutes=t->begin.minutes;
    gEditTimeEnd.hours=t->end.hours;
    gEditTimeEnd.minutes=t->end.minutes;

    color.r=t->color[0];
    color.g=t->color[1];
    color.b=t->color[2];
    EditTimeSetColor(frm, &color);

    room = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_room));

    mroom=MemHandleNew(sizeof(t->room));

    buffer=(Char *)MemHandleLock(mroom);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrNCopy(buffer, t->room, sizeof(t->room)-1);
    MemHandleUnlock(mroom);


    // Load fields
    old = FldGetTextHandle(room);
    FldSetTextHandle(room, mroom);
    if (old != NULL)    MemHandleFree(old); 

    selectedCourse = CourseListGen(gEditTimeItemList, gEditTimeItemIDs, gEditTimeItemInd, gEditTimeNumCourses, GadgetGetHintCourseIndex(), CLIST_SEARCH_INDEX);

    LstSetSelection(day, t->day);
    CtlSetLabel(dayt, LstGetSelectionText(day, t->day));
   
    MemHandleUnlock(mt);
  }

  EditTimeSetSelectorText(GetObjectPtr(SELECTOR_ed_time), &gEditTimeBegin, &gEditTimeEnd, gEditTimeTrigger);

  LstSetListChoices(lst, gEditTimeItemList, gEditTimeNumCourses);
  if (gEditTimeNumCourses < 6)  LstSetHeight(lst, gEditTimeNumCourses);
  else                 LstSetHeight(lst, 6);
  LstSetSelection(lst, selectedCourse);
  CtlSetLabel(ctl, LstGetSelectionText(lst, selectedCourse));

}



static Boolean EditTimeSave(FormType *frm) {
  ListType *lst, *day;
  ControlType *ctl, *dayt;
  FieldType *r, *g, *b, *room;

  lst = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_ed_course));
  day = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_ed_day));

  ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_ed_course_trigger));
  dayt = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LIST_ed_day_trigger));

  r = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_r));
  g = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_g));
  b = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_b));
  room = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_room));


  if (EditTimeCheckCollision(gEditTimeBegin, gEditTimeEnd, LstGetSelection(day), ((gEditTimeIsAdd) ? 0 : GadgetGetHintTimeIndex()), !gEditTimeIsAdd)) {
    FrmAlert(ALERT_timeCollision);
    return false;
  } else {
    Char *e = FldGetTextPtr(room);
    if ((e == NULL) || (StrLen(e) == 0)) {
      FrmAlert(ALERT_noroom);
      return false;
    } else {

      // Just need to change values, since we have opened the record directly...
      TimeDBRecord tt, *t;
      MemHandle mt;
      Char *a, *d, *c;
      UInt16 index;

      // Prepare record here since we want to use it with DmFindSortPosition(..)
      tt.type=TYPE_TIME;
      tt.course=gEditTimeItemIDs[LstGetSelection(lst)];
      tt.note = 0;
      tt.day = LstGetSelection(day);
      tt.begin.hours = gEditTimeBegin.hours;
      tt.begin.minutes = gEditTimeBegin.minutes;
      tt.end.hours = gEditTimeEnd.hours;
      tt.end.minutes = gEditTimeEnd.minutes;
      a = FldGetTextPtr(r); if (! a) a = "0";
      d = FldGetTextPtr(g); if (! d) d = "0";
      c = FldGetTextPtr(b); if (! c) c = "0";
      tt.color[0] = (UInt8)StrAToI(a);
      tt.color[1] = (UInt8)StrAToI(d);
      tt.color[2] = (UInt8)StrAToI(c);

      StrNCopy(tt.room, e, sizeof(tt.room));
      if (! tt.room) StrCopy(tt.room, "");


      // Find position
      if (gEditTimeIsAdd) {
        index = DmFindSortPosition(DatabaseGetRefN(DB_MAIN), &tt, NULL, DatabaseCompare, 0);
        mt = DmNewRecord(DatabaseGetRefN(DB_MAIN), &index, sizeof(TimeDBRecord));      
      } else {
        index = GadgetGetHintTimeIndex();
        mt = DmGetRecord(DatabaseGetRefN(DB_MAIN), index);
      }
      t = (TimeDBRecord *)MemHandleLock(mt);

      if (! gEditTimeIsAdd) {
        tt.note = t->note;
      }

      DmWrite(t, 0, &tt, sizeof(TimeDBRecord));
      // Highlight new record.. Current course index+1 since courses get shifted by one because we sort
      // like "... -> times -> courses"). Time index is index since we used DmFindSortPosition() to find it
      GadgetSetHintCourseIndex(gEditTimeItemInd[LstGetSelection(lst)]+1);
      GadgetSetHintTimeIndex(index);
      MemHandleUnlock(mt);
      DmReleaseRecord(DatabaseGetRef(), index, false);

      if (gEditTimeIsAdd) {
        UInt16 attr;
        DmRecordInfo(DatabaseGetRefN(DB_MAIN), index, &attr, NULL, NULL);
        attr |= DatabaseGetCat();
        DmSetRecordInfo(DatabaseGetRefN(DB_MAIN), index, &attr, NULL);
      } else {
        GadgetSetNeedsCompleteRedraw(true);
      }

      DatabaseSort();

    }
  }

  return true;
}


static void EditTimeFormFree(void) {
  UInt16 i;
  for(i=0; i < gEditTimeNumCourses; ++i)
    MemPtrFree((MemPtr) gEditTimeItemList[i]);
  MemPtrFree((MemPtr) gEditTimeItemList);
  MemPtrFree((MemPtr) gEditTimeItemIDs);
  MemPtrFree((MemPtr) gEditTimeItemInd);
}


static void EditTimePickColor(FormType *frm) {
  RGBColorType color;
  IndexedColorType paletteColor;
  FieldType *r, *g, *b;
  Boolean ok;

  r = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_r));
  g = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_g));
  b = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_ed_b));


  color.r = (UInt8)StrAToI((FldGetTextPtr(r) == NULL) ? "0" : FldGetTextPtr(r));
  color.g = (UInt8)StrAToI((FldGetTextPtr(g) == NULL) ? "0" : FldGetTextPtr(g));
  color.b = (UInt8)StrAToI((FldGetTextPtr(b) == NULL) ? "0" : FldGetTextPtr(b));

  if (TNisColored())
    ok = UIPickColor(&paletteColor, &color, UIPickColorStartRGB, NULL, NULL);
  else 
    ok = UIPickColor(&paletteColor, &color, UIPickColorStartPalette, NULL, NULL);
  
  if (ok) {
    // User clicked "OK"
    Char *buffer;
    MemHandle mr, mg, mb, old;

    mr=MemHandleNew(4); // 4 is 3 digits + \0
    mg=MemHandleNew(4); // 4 is 3 digits + \0
    mb=MemHandleNew(4); // 4 is 3 digits + \0

    // Copy contents to the memory handle
    buffer=(Char *)MemHandleLock(mr);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrPrintF(buffer, "%u", color.r);
    MemHandleUnlock(mr);

    buffer=(Char *)MemHandleLock(mg);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrPrintF(buffer, "%u", color.g);
    MemHandleUnlock(mg);

    buffer=(Char *)MemHandleLock(mb);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrPrintF(buffer, "%u", color.b);
    MemHandleUnlock(mb);

    // Load fields
    old = FldGetTextHandle(r);
    FldSetTextHandle(r, mr);
    if (old != NULL)    MemHandleFree(old); 

    old = FldGetTextHandle(g);
    FldSetTextHandle(g, mg);
    if (old != NULL)    MemHandleFree(old); 

    old = FldGetTextHandle(b);
    FldSetTextHandle(b, mb);
    if (old != NULL)    MemHandleFree(old); 

  }

  FldDrawField(r);
  FldDrawField(g);
  FldDrawField(b);
}


void EditTimeSetSelectorText(ControlType *ctl, TimeType *begin, TimeType *end, Char *label) {
  Char *str;
  TimeFormatType timeFormat=GadgetGetTimeFormat();

  str = label;
  TimeToAscii (begin->hours, begin->minutes, timeFormat, str);
  str += StrLen (str);
  *str++ = spaceChr;
  *str++ = '-';
  *str++ = spaceChr;
  TimeToAscii(end->hours, end->minutes, timeFormat, str);
  CtlSetLabel(ctl, label);
}

void EditTimeGetTime(ControlType *ctl, TimeType *begin, TimeType *end, Char *label) {
  Char *title;
  Boolean clickedOK=false;
  TimeType tmpBegin, tmpEnd;

  title = MemHandleLock(DmGetResource (strRsc, STRING_ed_title));
  MemMove(&tmpBegin, begin, sizeof(TimeType));
  MemMove(&tmpEnd, end, sizeof(TimeType));
  clickedOK=SelectTime(&tmpBegin, &tmpEnd, false, title, 8, 20, 8);
  MemPtrUnlock(title);

  if (clickedOK) {
    if (TimeToInt(tmpBegin) != noTime) {
      MemMove(begin, &tmpBegin, sizeof(TimeType));
      MemMove(end, &tmpEnd, sizeof(TimeType));
      EditTimeSetSelectorText(ctl, begin, end, label);
    } else {
      FrmAlert(ALERT_untimed_imp);
    }
  }

}


/*****************************************************************************
* Function: EditTimeCheckCollision
*
* Description: Checks if given times collides with an existing event/time
*              record. Returns true on collision and false otherwise
*****************************************************************************/
Boolean EditTimeCheckCollision(TimeType begin, TimeType end, UInt8 day, UInt16 notIndex, Boolean checkIndex) {
  Boolean rv=false;
  UInt16 beginVal=TimeToInt(begin), endVal=TimeToInt(end);
  UInt16 index=0;
  MemHandle m;
  Char *s;

  while (!rv && ((m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL)) {
    if ( !checkIndex || (index != notIndex)) {
      s = MemHandleLock(m);
      if (s[0] == TYPE_TIME) {
        TimeDBRecord *t=(TimeDBRecord *)s;
        UInt16 beginTmp=TimeToInt(t->begin), endTmp=TimeToInt(t->end);
        if (day == t->day) {
          // days must be equal to have a collision
          // Three cases have to be cared:
          // 1) Existing begin is in the new time
          // 2) existing end is in the new time
          // 3) complete time is in the new time
          if ( ((beginVal >= beginTmp) && (beginVal < endTmp)) ||
               ((endVal > beginTmp) && (endVal <= endTmp)) ||
               ((beginVal <= beginTmp) && (endVal >= endTmp)) ) {
            rv = true;
          }
        }
      }
      MemHandleUnlock(m);
    }
    index += 1;
  }

  return rv;
}

Boolean EditTimeFormHandleEvent(EventPtr event) {
  Boolean handled = false;
  FormType *frm=FrmGetActiveForm();

  if (event->eType == ctlSelectEvent) {
    switch (event->data.ctlSelect.controlID) {

      case BUTTON_ed_ok:
        if (EditTimeSave(frm)) {
          EditTimeFormFree();
          FrmReturnToForm(gMenuCurrentForm);
          FrmUpdateForm(gMenuCurrentForm, frmRedrawUpdateCode);
        }
        handled=true;
        break;

      case BUTTON_ed_cancel:
        EditTimeFormFree();
        FrmReturnToForm(gMenuCurrentForm);
        FrmUpdateForm(gMenuCurrentForm, frmRedrawUpdateCode);
        handled=true;
        break;

      case BUTTON_ed_del:
        EditTimeFormFree();
        FrmReturnToForm(gMenuCurrentForm);
        FrmUpdateForm(gMenuCurrentForm, frmRedrawUpdateCode);
        DeleteEntry();
        handled=true;
        break;

      case BUTTON_ed_beam:
        BeamCourse(GadgetGetHintCourseIndex());
        handled=true;
        break;

      case BUTTON_ed_note:
        NoteSet(GadgetGetHintTimeIndex(), FORM_evt_det);
        FrmPopupForm(NewNoteView);
        break;

      case BUTTON_ed_color:
        EditTimePickColor(frm);
        handled=true;
        break;

      case SELECTOR_ed_time:
        EditTimeGetTime(GetObjectPtr(SELECTOR_ed_time), &gEditTimeBegin, &gEditTimeEnd, gEditTimeTrigger);
        handled=true;
        break;

      default:
        break;
    }
  } else if (event->eType == popSelectEvent) {
    if (event->data.popSelect.listID == LIST_ed_course) {
      EditTimeFindColor(frm, gEditTimeItemIDs[event->data.popSelect.selection]);
    }
  } else if (event->eType == frmUpdateEvent) {
      // redraws the form if needed
      frm = FrmGetActiveForm();
      FrmDrawForm(frm);
      handled = true;
    } else if (event->eType == frmOpenEvent) {
      // initializes and draws the form at program launch
      frm = FrmGetActiveForm();
      FrmDrawForm(frm);
      EditTimeFormInit(frm);
      if (gEditTimeIsAdd) {
        CtlHideControl(GetObjectPtr(BUTTON_ed_beam));
        CtlHideControl(GetObjectPtr(BUTTON_ed_del));
        CtlHideControl(GetObjectPtr(BUTTON_ed_note));
      } else {
        CtlShowControl(GetObjectPtr(BUTTON_ed_beam));
        CtlShowControl(GetObjectPtr(BUTTON_ed_del));
        CtlShowControl(GetObjectPtr(BUTTON_ed_note));
      }
      handled = true;
    } else if (event->eType == frmCloseEvent) {
      // this is done if program is closed
      // usually NOT called, since we use FrmReturnToForm!
      // But maybe if user just presses home button while form is shown
      EditTimeFormFree();
    }



  return handled;
}
