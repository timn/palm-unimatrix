/* $Id: exams.c,v 1.5 2003/06/18 11:10:11 tim Exp $
 *
 * Exams functions
 * Created: 2002-09-21
 */

#include "UniMatrix.h"
#include "exams.h"
#include "database.h"
#include "edit.h"
#include "gadget.h"
#include "clist.h"
#include "delete.h"
#include "tnglue.h"
#include "beam.h"
#include "notes.h"
#include "alarm.h"

// "Shared global" globals
extern Char gCategoryName[dmCategoryLength];
extern UInt16 gMenuCurrentForm;

Char **gExamDetailsItemList, gExamDetailsTimeTrigger[2*timeStringLength+3], gExamDetailsDateTrigger[longDateStrLength];
UInt16 *gExamDetailsItemIDs=NULL, *gExamDetailsItemInd=NULL, gExamDetailsNumCourses, gExamsSelRow=0, gExamsOffset=0;
DateType gExamDetailsDate;
TimeType gExamDetailsBegin, gExamDetailsEnd;
UInt32 gExamsLastSelRowUID=0; // Unique ID of selected record


static void TableDrawSelection(void *table, Int16 row, Int16 column, RectangleType *bounds) {
  RGBColorType red={0x00, 0xFF, 0x00, 0x00};
  RectangleType r;

  RctSetRectangle(&r, bounds->topLeft.x, bounds->topLeft.y+(bounds->extent.y/2)-3, 4, 7);

  if (gExamsSelRow == row) {
    TNSetForeColorRGB(&red, NULL);
    // WinDrawRectangle(&r,0);    
    WinDrawPixel(r.topLeft.x, r.topLeft.y+3);
    WinDrawLine(r.topLeft.x+1, r.topLeft.y+2, r.topLeft.x+1, r.topLeft.y+4);
    WinDrawLine(r.topLeft.x+2, r.topLeft.y+1, r.topLeft.x+2, r.topLeft.y+5);
    WinDrawLine(r.topLeft.x+3, r.topLeft.y, r.topLeft.x+3, r.topLeft.y+6);
  } else {
    WinEraseRectangle(&r, 0);    
  }

}

static void TableDrawData(void *table, Int16 row, Int16 column, RectangleType *bounds) {
  UInt16 index=TblGetRowID(table, row);
  ExamDBRecord *ex;
  MemHandle mex;
  RGBColorType fore={0x00, 0x00, 0x00, 0x00}, back={0x00, 0xFF, 0xFF, 0xFF};

  if (! TblRowUsable(table, row)) return;
  
  TNSetBackColorRGB(&back, NULL);
  TNSetForeColorRGB(&back, NULL);
  WinDrawRectangle(bounds, 0);

  mex = DmQueryRecord(DatabaseGetRefN(DB_MAIN), index);
  ex = (ExamDBRecord *)MemHandleLock(mex);

  if (column == EXCOL_COURSE) {
    Char *temp;
    MemHandle m=MemHandleNew(1);
    CourseGetName(ex->course, &m, true);
    temp = MemHandleLock(m);

    TNSetForeColorRGB(&fore, NULL);
    TNDrawCharsToFitWidth(temp, bounds);

    MemHandleUnlock(m);
    MemHandleFree(m);
  } else if (column == EXCOL_NOTE) {
    if (ex->note) {
      Char noteSymb[2] = { GADGET_NOTESYMBOL, 0 };
      FontID oldFont = FntSetFont(symbolFont);

      TNDrawCharsToFitWidth(noteSymb, bounds);
      FntSetFont(oldFont);
    }
  } else if (column == EXCOL_DATE) {
    Char dateTemp[dateStringLength];
    DateToAscii(ex->date.month, ex->date.day, ex->date.year+MAC_SHIT_YEAR_CONSTANT, PrefGetPreference(prefDateFormat), dateTemp);
    TNDrawCharsToFitWidth(dateTemp, bounds);
  } else if (column == EXCOL_TIME) {
    Char timeTemp[timeStringLength];
    TimeToAscii(ex->begin.hours, ex->begin.minutes, PrefGetPreference(prefTimeFormat), timeTemp);
    TNDrawCharsToFitWidth(timeTemp, bounds);
  }


  if (ex->flags & EX_FLAG_DONE) {
    RGBColorType red = {0x00, 0xFF, 0x00, 0x00}, old;
    Int16 yCoord=bounds->topLeft.y+(bounds->extent.y / 2);
    TNSetForeColorRGB(&red, &old);
    WinDrawLine(bounds->topLeft.x, yCoord, bounds->topLeft.x+bounds->extent.x, yCoord);
    TNSetForeColorRGB(&old, NULL);
  }

  MemHandleUnlock(mex);
}


static void ExamsTableInit(void) {
  TableType *table=GetObjectPtr(TABLE_exams);
  UInt16 i, j;
  MemHandle m;
  UInt16 index=0;

  gExamsSelRow = 0;  
  
  for (i=0; i < TblGetNumberOfRows(table); ++i) {
    TblSetItemStyle(table, i, EXCOL_DONE, checkboxTableItem);
    TblSetItemStyle(table, i, EXCOL_COURSE, customTableItem);
    TblSetItemStyle(table, i, EXCOL_NOTE, customTableItem);
    TblSetItemStyle(table, i, EXCOL_DATE, customTableItem);
    TblSetItemStyle(table, i, EXCOL_TIME, customTableItem);
    TblSetItemStyle(table, i, EXCOL_SELI, customTableItem);
  }

  TblSetColumnSpacing(table, EXCOL_DONE, 2);
  TblSetColumnSpacing(table, EXCOL_COURSE, 1);
  TblSetColumnSpacing(table, EXCOL_NOTE, 3);
  TblSetColumnSpacing(table, EXCOL_DATE, 1);
  TblSetColumnSpacing(table, EXCOL_TIME, 3);

  TblSetColumnUsable(table, EXCOL_DONE, true);
  TblSetColumnUsable(table, EXCOL_COURSE, true);
  TblSetColumnUsable(table, EXCOL_NOTE, true);
  TblSetColumnUsable(table, EXCOL_DATE, true);
  TblSetColumnUsable(table, EXCOL_TIME, true);
  TblSetColumnUsable(table, EXCOL_SELI, true);

  for (i=0; i < TblGetNumberOfRows(table); ++i) {
    TblSetRowUsable(table, i, false);
  }

  TblSetCustomDrawProcedure(table, EXCOL_COURSE, TableDrawData);
  TblSetCustomDrawProcedure(table, EXCOL_NOTE, TableDrawData);
  TblSetCustomDrawProcedure(table, EXCOL_DATE, TableDrawData);
  TblSetCustomDrawProcedure(table, EXCOL_TIME, TableDrawData);
  TblSetCustomDrawProcedure(table, EXCOL_SELI, TableDrawSelection);

  i = 0; j = 0;
  while ((i < EX_MAX_ROWS) && (m = DmQueryNextInCategory(DatabaseGetRefN(DB_MAIN), &index, DatabaseGetCat()))) {
    UInt32 uid=0;
    ExamDBRecord *ex;

    DmRecordInfo(DatabaseGetRefN(DB_MAIN), index, NULL, &uid, NULL);
    ex = MemHandleLock(m);
    if (ex->type == TYPE_EXAM) {
      // We have an exam, insert if above current offset
      if (j >= gExamsOffset) {
        UInt16 done = ex->flags & EX_FLAG_DONE;
        TblInsertRow(table, i);
        TblSetRowID(table, i, index);
        TblSetRowData(table, i, uid);
        if (uid == gExamsLastSelRowUID)  gExamsSelRow = i;
        TblSetRowUsable(table, i, true);
        TblMarkRowInvalid(table, i);
        TblSetItemInt(table, i, EXCOL_DONE, done);
        i += 1;
      }
      j += 1;
    }
    MemHandleUnlock(m);
    index += 1;
  }

  // Check if there are displayed exams. If there are not, hide the
  // buttons for edit/delete/beam, otherwise show
  if (i > 0) {
    CtlShowControl(GetObjectPtr(BUTTON_ex_edit));
    CtlShowControl(GetObjectPtr(BUTTON_ex_del));
    CtlShowControl(GetObjectPtr(BUTTON_ex_beam));
    CtlShowControl(GetObjectPtr(BUTTON_ex_note));
  } else {
    CtlHideControl(GetObjectPtr(BUTTON_ex_edit));
    CtlHideControl(GetObjectPtr(BUTTON_ex_del));
    CtlHideControl(GetObjectPtr(BUTTON_ex_beam));
    CtlHideControl(GetObjectPtr(BUTTON_ex_note));
  }

  // decide and (show/hide) whether to have enabled or disabled down button
  // There must be another record, otherwise our course we assigned this exam to would
  // not exist, that should never happen. But to cover my future mistakes
  // index has been increased one in the loop.
  if ( (m = DmQueryNextInCategory(DatabaseGetRefN(DB_MAIN), &index, DatabaseGetCat())) != NULL) {
    // We have more records, are there exams?
    Char *s;
    s = MemHandleLock(m);
    // Since the DB is sorted we do not need to search for exams but just check the next
    // record. If it is not an exam record there won't be any later!
    if (s[0] == TYPE_EXAM) {
      CtlHideControl(GetObjectPtr(REPEAT_ex_down_dis));
      CtlShowControl(GetObjectPtr(REPEAT_ex_down));
    } else {
      CtlHideControl(GetObjectPtr(REPEAT_ex_down));
      CtlShowControl(GetObjectPtr(REPEAT_ex_down_dis));
    }
    MemHandleUnlock(m);
  } else {
    CtlHideControl(GetObjectPtr(REPEAT_ex_down));
    CtlShowControl(GetObjectPtr(REPEAT_ex_down_dis));
  }

  // decide (and show/hide) whether to have enabled or disabled up button
  if (gExamsOffset > 0) {
    CtlHideControl(GetObjectPtr(REPEAT_ex_up_dis));
    CtlShowControl(GetObjectPtr(REPEAT_ex_up));
  } else {
    CtlHideControl(GetObjectPtr(REPEAT_ex_up));
    CtlShowControl(GetObjectPtr(REPEAT_ex_up_dis));
  }
}

static void ExamDelete(void) {
  MemHandle mex, m;
  ExamDBRecord *ex;
  UInt16 index=0, pressedButton=0;
  Char *courseName, timeTemp[timeStringLength], dateTemp[longDateStrLength];

  DmFindRecordByID(DatabaseGetRefN(DB_MAIN), gExamsLastSelRowUID, &index);
  mex = DmQueryRecord(DatabaseGetRefN(DB_MAIN), index);
  ex = (ExamDBRecord *)MemHandleLock(mex);

  m=MemHandleNew(1);
  CourseGetName(ex->course, &m, true);
  courseName = MemHandleLock(m);

  DateToAscii(ex->date.month, ex->date.day, ex->date.year+MAC_SHIT_YEAR_CONSTANT, PrefGetPreference(prefLongDateFormat), dateTemp);
  TimeToAscii(ex->begin.hours, ex->begin.minutes, PrefGetPreference(prefTimeFormat), timeTemp);

  pressedButton = FrmCustomAlert(ALERT_ex_dodel, courseName, dateTemp, timeTemp);

  MemHandleUnlock(m);
  MemHandleFree(m);
  MemHandleUnlock(mex);

  if (pressedButton == 0) {
    // OK, the user really wants us to delete the record
    NoteDelete(&index);
    DmRemoveRecord(DatabaseGetRefN(DB_MAIN), index);
    gExamsSelRow=0;
    FrmUpdateForm(FORM_exams, frmRedrawUpdateCode);
  }
}


static void ExamBeam(void) {
  MemHandle mex;
  ExamDBRecord *ex;
  UInt16 index=0;

  DmFindRecordByID(DatabaseGetRefN(DB_MAIN), gExamsLastSelRowUID, &index);
  mex = DmQueryRecord(DatabaseGetRefN(DB_MAIN), index);
  ex = (ExamDBRecord *)MemHandleLock(mex);

  MemHandleUnlock(mex);

  BeamCourseByCID(ex->course);
}

void ExamSetGoto(UInt32 uniqueID) {
  gExamsLastSelRowUID = uniqueID;
}

Boolean ExamsFormHandleEvent(EventPtr event) {
  FormPtr frm=FrmGetActiveForm();
  Boolean handled = false;
  Boolean categoryEdited, reDraw=false;
  UInt16 category, numRecords;
  ControlType *ctl;
  UInt32 *recordList;

  if (event->eType == ctlSelectEvent) {
    // button handling
    switch (event->data.ctlSelect.controlID) {
      case BUTTON_ex_back:
        handled=true;
        FrmGotoForm(FORM_main);
        break;

      case BUTTON_ex_add:
        handled=true;
        if (CountCourses() > 0) {
          gExamsLastSelRowUID=0;
          FrmPopupForm(FORM_exam_details);
        } else {
          FrmAlert(ALERT_nocourses);
        }
        break;

      case BUTTON_ex_edit:
        handled=true;
        gExamsLastSelRowUID=TblGetRowData(GetObjectPtr(TABLE_exams), gExamsSelRow);
        FrmPopupForm(FORM_exam_details);
        break;

      case BUTTON_ex_note:
      {
        UInt16 index=0;

        handled=true;

        gExamsLastSelRowUID=TblGetRowData(GetObjectPtr(TABLE_exams), gExamsSelRow);
        DmFindRecordByID(DatabaseGetRefN(DB_MAIN), gExamsLastSelRowUID, &index);
        NoteSet(index, FORM_exams);
        FrmPopupForm(NewNoteView);
        break;
      }

      case BUTTON_ex_del:
        handled=true;
        gExamsLastSelRowUID=TblGetRowData(GetObjectPtr(TABLE_exams), gExamsSelRow);
        ExamDelete();
        break;

      case BUTTON_ex_beam:
        handled=true;
        gExamsLastSelRowUID=TblGetRowData(GetObjectPtr(TABLE_exams), gExamsSelRow);
        ExamBeam();
        break;

      case LIST_ex_cat_trigger:
        handled=true;
        category=DatabaseGetCat();
        numRecords=DmNumRecordsInCategory(DatabaseGetRef(), DELETE_CATEGORY);
        recordList=(UInt32 *)MemPtrNew(numRecords * sizeof(UInt32));
        CatPreEdit(numRecords, recordList);
        categoryEdited = CategorySelect(DatabaseGetRef(), frm,
                                        LIST_ex_cat_trigger, LIST_ex_cat, false,
                                        &category, gCategoryName, 0,
                                        STRING_cat_edit); // categoryDefaultEditCategoryString
        if (categoryEdited || (category != DatabaseGetCat())) {
          reDraw=true;
          DatabaseSetCat(category);
          ctl = GetObjectPtr(LIST_ex_cat_trigger);
          CategoryGetName(DatabaseGetRef(), category, gCategoryName); 
          CategorySetTriggerLabel(ctl, gCategoryName);
        }
        CatPostEdit(numRecords, recordList);
        if (reDraw) {
          gExamsOffset=0;
          gExamsSelRow=0;
          FrmUpdateForm(FORM_exams, frmRedrawUpdateCode);
        }
        if (recordList != NULL)  MemPtrFree((MemPtr)recordList);
        break;

      default:
        break;
    }
  } else if (event->eType == tblEnterEvent) {
    UInt16 i;
    
    if (event->data.tblEnter.column == EXCOL_DONE) {
      handled=false;
    } else if (event->data.tblEnter.column == EXCOL_NOTE) {
      MemHandle m;
      Boolean hasNote=false;

      gExamsSelRow=event->data.tblEnter.row;
      for (i=0; i < TblGetNumberOfRows(event->data.tblEnter.pTable); ++i) {
        RectangleType r;
        TblGetItemBounds(event->data.tblEnter.pTable, i, EXCOL_SELI, &r);
        TableDrawSelection(event->data.tblEnter.pTable, i, event->data.tblEnter.column, &r);
      }


      m = DmQueryRecord(DatabaseGetRefN(DB_MAIN), TblGetRowID(event->data.tblEnter.pTable, event->data.tblEnter.row));
      if (m) {
        ExamDBRecord *ex = (ExamDBRecord *)MemHandleLock(m);
        if (ex->note) hasNote = true;
        else          hasNote = false;
        MemHandleUnlock(m);
      }

      if (hasNote) {
        Coord newPointX, newPointY;
        Boolean isPenDown=false, drawn=false;
        RectangleType fieldBounds;
        IndexedColorType curForeColor, curBackColor, curTextColor;
        Char noteSymb[2] = { GADGET_NOTESYMBOL, 0 };
        FontID oldFont;
  
  
        EvtGetPen(&newPointX, &newPointY, &isPenDown);
        TblGetItemBounds(event->data.tblEnter.pTable, event->data.tblEnter.row, EXCOL_NOTE, &fieldBounds);
  
        oldFont = FntSetFont(symbolFont);
        while (isPenDown){
          if (! drawn && RctPtInRectangle(newPointX, newPointY, &fieldBounds)) {
            curForeColor = WinSetForeColor(UIColorGetTableEntryIndex(UIObjectSelectedForeground));
            curBackColor = WinSetBackColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
            curTextColor = WinSetTextColor(UIColorGetTableEntryIndex(UIObjectSelectedForeground));
            TNDrawCharsToFitWidth(noteSymb, &fieldBounds);
            WinSetForeColor(curForeColor);
            WinSetForeColor(curBackColor);
            WinSetForeColor(curTextColor);
            drawn = true;
          } else if (drawn && ! RctPtInRectangle(newPointX, newPointY, &fieldBounds)) {
            curForeColor = WinSetForeColor(UIColorGetTableEntryIndex(UIObjectForeground));
            curBackColor = WinSetBackColor(UIColorGetTableEntryIndex(UIObjectFill));
            curTextColor = WinSetTextColor(UIColorGetTableEntryIndex(UIObjectForeground));
            TNDrawCharsToFitWidth(noteSymb, &fieldBounds);
            WinSetForeColor(curForeColor);
            WinSetForeColor(curBackColor);
            WinSetForeColor(curTextColor);
            drawn = false;
          }
          EvtGetPen(&newPointX, &newPointY, &isPenDown);
        }
        FntSetFont(oldFont);
      } else {
        handled = true;
      }
    } else {
      gExamsSelRow=event->data.tblEnter.row;
      for (i=0; i < TblGetNumberOfRows(event->data.tblEnter.pTable); ++i) {
        RectangleType r;
        TblGetItemBounds(event->data.tblEnter.pTable, i, EXCOL_SELI, &r);
        TableDrawSelection(event->data.tblEnter.pTable, i, event->data.tblEnter.column, &r);
      }
      handled=true;
    }
  } else if (event->eType == tblSelectEvent) {
    if (event->data.tblEnter.column == EXCOL_DONE) {
      MemHandle mex;
      ExamDBRecord *ex;
      Boolean done=(TblGetItemInt(event->data.tblSelect.pTable, event->data.tblSelect.row, event->data.tblSelect.column) == 0) ? false : true;
      UInt16 flags, index=TblGetRowID(event->data.tblSelect.pTable, event->data.tblSelect.row);

      mex=DmGetRecord(DatabaseGetRefN(DB_MAIN), index);
      ex = MemHandleLock(mex);
      flags = ex->flags;

      if (done) {
        flags |= EX_FLAG_DONE;
      } else {
        flags &= (EX_FLAG_DONE ^ 0xFFFF);
      }

      DmWrite(ex, offsetof(ExamDBRecord, flags), &flags, sizeof(flags));
      DmReleaseRecord(DatabaseGetRefN(DB_MAIN), index, false);

      TblMarkRowInvalid(event->data.tblSelect.pTable, event->data.tblSelect.row);
      TblRedrawTable(event->data.tblSelect.pTable);

      MemHandleUnlock(mex);

    } else if (event->data.tblEnter.column == EXCOL_NOTE) {
      ControlType *ctl=GetObjectPtr(BUTTON_ex_note);
      // Don't need code twice, just read ctlSelect Event for BUTTON_ex_note
      CtlHitControl(ctl);
    }
    handled=true;
  } else if (event->eType == ctlRepeatEvent) {
    // Repeat buttons pressed
    if( event->data.ctlRepeat.controlID == REPEAT_ex_up )
      gExamsOffset -= 1;
    else
      gExamsOffset += 1;

    ExamsTableInit();
    TblMarkTableInvalid(GetObjectPtr(TABLE_exams));
    TblRedrawTable(GetObjectPtr(TABLE_exams));
  } else if (event->eType == keyDownEvent) {
    // We have a hard button assigned and it was pressed
    if (TxtCharIsHardKey(event->data.keyDown.modifiers, event->data.keyDown.chr)) {
      if (! (event->data.keyDown.modifiers & poweredOnKeyMask)) {
        FrmGotoForm(FORM_main);
        handled = true;
      }
    }
  } else if (event->eType == menuOpenEvent) {
    return HandleMenuOpenEvent(event);
  } else if (event->eType == menuEvent) {
    // forwarding of menu events
    return HandleMenuEvent(event->data.menu.itemID);
  } else if (event->eType == frmOpenEvent) {
    // initializes and draws the form
    ControlType *ctl;

    gExamsOffset=0;
    ExamsTableInit();
    FrmDrawForm (frm);

    ctl = GetObjectPtr(LIST_ex_cat_trigger);
    CategoryGetName (DatabaseGetRef(), DatabaseGetCat(), gCategoryName); 
    CategorySetTriggerLabel (ctl, gCategoryName); 

    handled = true;
  } else if (event->eType == frmUpdateEvent) {
    // redraws the form if needed
    gExamsOffset=0;
    ExamsTableInit();
    FrmDrawForm(frm);
    handled = false;
  } else if (event->eType == frmCloseEvent) {
    // this is done if form is closed
    // TblEraseTable(GetObjectPtr(TABLE_exams));
    // ExamsFormFree();
    FrmEraseForm(frm);
  }

  return (handled);

}


/** Exam Details
 *  #################################################################################
 */

static void ExamDetailsSetTriggers(ControlType *dtr, ControlType *ttr) {
  DateToAscii(gExamDetailsDate.month, gExamDetailsDate.day, gExamDetailsDate.year+MAC_SHIT_YEAR_CONSTANT, PrefGetPreference(prefLongDateFormat), gExamDetailsDateTrigger);
  CtlSetLabel(dtr, gExamDetailsDateTrigger);
  // Time Trigger not longer set, done during EditTimeGetTime call
}


static void ExamDetailsFormInit(FormType *frm) {
  UInt16 selectedCourse=0;
  ListType *course;
  ControlType *course_tr, *date_tr, *time_tr;

  course = GetObjectPtr(LIST_exd_course);
  course_tr = GetObjectPtr(LIST_exd_course_trigger);
  date_tr = GetObjectPtr(SELECTOR_exd_date);
  time_tr = GetObjectPtr(SELECTOR_exd_time);

  gExamDetailsNumCourses = CountCourses();

  gExamDetailsItemList = (Char **) MemPtrNew(gExamDetailsNumCourses * sizeof(Char *));
  gExamDetailsItemIDs = (UInt16 *) MemPtrNew(gExamDetailsNumCourses * sizeof(UInt16));
  gExamDetailsItemInd = (UInt16 *) MemPtrNew(gExamDetailsNumCourses * sizeof(UInt16));

  if (gExamsLastSelRowUID == 0) {
    // ADD
    DateTimeType dt;

    selectedCourse = CourseListGen(gExamDetailsItemList, gExamDetailsItemIDs, gExamDetailsItemInd, gExamDetailsNumCourses, 0, CLIST_SEARCH_INDEX);

    TimSecondsToDateTime(TimGetSeconds(), &dt);
    gExamDetailsDate.year=dt.year-MAC_SHIT_YEAR_CONSTANT;

    gExamDetailsDate.month=dt.month;
    gExamDetailsDate.day=dt.day;

    gExamDetailsBegin.hours=14;
    gExamDetailsBegin.minutes=0;

    gExamDetailsEnd.hours=15;
    gExamDetailsEnd.minutes=30;

  } else {
    MemHandle mex, mRoom, old;
    ExamDBRecord *ex;
    UInt16 index=0;
    Char *buffer;
    FieldType *fldRoom;

    fldRoom=GetObjectPtr(FIELD_exd_room);

    DmFindRecordByID(DatabaseGetRefN(DB_MAIN), gExamsLastSelRowUID, &index);

    mex = DmQueryRecord(DatabaseGetRefN(DB_MAIN), index);
    ex = (ExamDBRecord *)MemHandleLock(mex);

    selectedCourse = CourseListGen(gExamDetailsItemList, gExamDetailsItemIDs, gExamDetailsItemInd, gExamDetailsNumCourses, ex->course, CLIST_SEARCH_ID);

    gExamDetailsDate.year=ex->date.year;
    gExamDetailsDate.month=ex->date.month;
    gExamDetailsDate.day=ex->date.day;

    gExamDetailsBegin.hours=ex->begin.hours;
    gExamDetailsBegin.minutes=ex->begin.minutes;

    gExamDetailsEnd.hours=ex->end.hours;
    gExamDetailsEnd.minutes=ex->end.minutes;

    // Copy contents to the memory handle
    mRoom=MemHandleNew(StrLen(ex->room)+1);
    buffer=(Char *)MemHandleLock(mRoom);
    MemSet(buffer, MemPtrSize(buffer), 0);
    StrNCopy(buffer, ex->room, StrLen(ex->room));
    MemHandleUnlock(mRoom);

    // Load fields
    old = FldGetTextHandle(fldRoom);
    FldSetTextHandle(fldRoom, mRoom);
    if (old != NULL)    MemHandleFree(old); 

    MemHandleUnlock(mex);
  }


  LstSetListChoices(course, gExamDetailsItemList, gExamDetailsNumCourses);
  if (gExamDetailsNumCourses < 5)  LstSetHeight(course, gExamDetailsNumCourses);
  else                 LstSetHeight(course, 5);
  LstSetSelection(course, selectedCourse);
  CtlSetLabel(course_tr, LstGetSelectionText(course, selectedCourse));

  ExamDetailsSetTriggers(date_tr, time_tr);
  EditTimeSetSelectorText(time_tr, &gExamDetailsBegin, &gExamDetailsEnd, gExamDetailsTimeTrigger);

}

static void ExamDetailsFormFree(void) {
  UInt16 i;
  for(i=0; i < gExamDetailsNumCourses; ++i)
    MemPtrFree((MemPtr) gExamDetailsItemList[i]);
  MemPtrFree((MemPtr) gExamDetailsItemList);
  MemPtrFree((MemPtr) gExamDetailsItemIDs);
  MemPtrFree((MemPtr) gExamDetailsItemInd);
}

static void ExamDetailsGetDate(void) {
  Char *title;
  Boolean clickedOK=false;
  Int16 year=gExamDetailsDate.year+MAC_SHIT_YEAR_CONSTANT, month=gExamDetailsDate.month, day=gExamDetailsDate.day;

  title = MemHandleLock(DmGetResource (strRsc, STRING_ed_title));
  clickedOK=SelectDay(selectDayByDay, &month, &day, &year, title);
  MemPtrUnlock(title);

  if (clickedOK) {
    gExamDetailsDate.year=year-MAC_SHIT_YEAR_CONSTANT;
    gExamDetailsDate.month=month;
    gExamDetailsDate.day=day;
    ExamDetailsSetTriggers(GetObjectPtr(SELECTOR_exd_date), GetObjectPtr(SELECTOR_exd_time));
  }

}

static Boolean ExamDetailsFormSave(void) {
  MemHandle newExam=NULL;
  UInt16 index = dmMaxRecordIndex;
  ListType *course;
  ControlType *course_tr, *date_tr, *time_tr;
  Char *room;
  Char empty[1]={'\0'};
  FieldType *fldRoom;

  course = GetObjectPtr(LIST_exd_course);
  course_tr = GetObjectPtr(LIST_exd_course_trigger);
  date_tr = GetObjectPtr(SELECTOR_exd_date);
  time_tr = GetObjectPtr(SELECTOR_exd_time);

  fldRoom = GetObjectPtr(FIELD_exd_room);
  room = FldGetTextPtr(fldRoom);
  if (room == NULL)  room=empty;

  if (gExamsLastSelRowUID == 0) {
    // New record
    newExam = DmNewRecord(DatabaseGetRefN(DB_MAIN), &index, sizeof(ExamDBRecord));
  } else {
    // Load record
    DmFindRecordByID(DatabaseGetRefN(DB_MAIN), gExamsLastSelRowUID, &index);
    newExam = DmGetRecord(DatabaseGetRefN(DB_MAIN), index);
  }
  if (! newExam) {
    // Could not create entry
    FrmAlert(ALERT_nomem);
    return false;
  } else {
    UInt16 attr=0;
    ExamDBRecord ex, *ep;

    ep = (ExamDBRecord *)MemHandleLock(newExam);

    ex.type=TYPE_EXAM;
    ex.course=gExamDetailsItemIDs[LstGetSelection(course)];
    ex.note = (gExamsLastSelRowUID == 0) ? 0 : ep->note;
    ex.date.year = gExamDetailsDate.year;
    ex.date.month = gExamDetailsDate.month;
    ex.date.day = gExamDetailsDate.day;
    ex.begin.hours = gExamDetailsBegin.hours;
    ex.begin.minutes = gExamDetailsBegin.minutes;
    ex.end.hours = gExamDetailsEnd.hours;
    ex.end.minutes = gExamDetailsEnd.minutes;
    ex.flags = 0x0000;
    StrNCopy(ex.room, room, sizeof(ex.room));

    DmWrite(ep, 0, &ex, sizeof(ExamDBRecord));
    MemHandleUnlock(newExam);
    DmReleaseRecord(DatabaseGetRef(), index, false);
    DmRecordInfo(DatabaseGetRef(), index, &attr, NULL, NULL);
    attr |= DatabaseGetCat();
    DmSetRecordInfo(DatabaseGetRef(), index, &attr, NULL);

    DatabaseSort();
  } 

  AlarmReset(DatabaseGetRef());
  return true;
}




Boolean ExamDetailsFormHandleEvent(EventPtr event) {
  FormPtr frm=FrmGetActiveForm();
  Boolean handled = false;

  if (event->eType == ctlSelectEvent) {
    // button handling
    switch (event->data.ctlSelect.controlID) {
      case BUTTON_exd_cancel:
        handled=true;
        ExamDetailsFormFree();
        FrmReturnToForm(FORM_exams);
        FrmUpdateForm(FORM_exams, frmRedrawUpdateCode);
        break;

      case BUTTON_exd_ok:
        handled=true;
        if (ExamDetailsFormSave()) {
          ExamDetailsFormFree();
          FrmReturnToForm(FORM_exams);
          FrmUpdateForm(FORM_exams, frmRedrawUpdateCode);
        }
        break;

      case SELECTOR_exd_date:
        handled=true;
        ExamDetailsGetDate();
        break;

      case SELECTOR_exd_time:
        handled=true;
        EditTimeGetTime(GetObjectPtr(SELECTOR_exd_time), &gExamDetailsBegin, &gExamDetailsEnd, gExamDetailsTimeTrigger);
        break;

      default:
        break;
    }
  } else if (event->eType == menuOpenEvent) {
    return HandleMenuOpenEvent(event);
  } else if (event->eType == menuEvent) {
    // forwarding of menu events
    return HandleMenuEvent(event->data.menu.itemID);
  } else if (event->eType == frmOpenEvent) {
    // initializes and draws the form
    ExamDetailsFormInit(frm);
    FrmDrawForm (frm);
    handled = true;
  } else if (event->eType == frmUpdateEvent) {
    // redraws the form if needed
    FrmDrawForm (frm);
    handled = false;
  } else if (event->eType == frmCloseEvent) {
    // this is done if form is closed
    ExamDetailsFormFree();
  }

  return (handled);
}
