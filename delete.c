/* $Id: delete.c,v 1.1 2003/02/06 21:27:23 tim Exp $
 *
 * All you need to delete courses and events
 */

#include "delete.h"
#include "ctype.h"
#include "gadget.h"

/*****************************************************************************
* Function:  DeleteEntry
*
* Description:  Deletes an entry
*****************************************************************************/
void DeleteEntry(void) {
  MemHandle mc, mt, mh;
  TimeDBRecord *t;
  CourseDBRecord c;
  Char *day, *bot, begin[timeStringLength], end[timeStringLength], type[CTYPE_SHORT_MAXLENGTH+1];
  UInt16 pressedButton=0;
  TimeFormatType timeFormat=tfColon24h;

  mc = DmQueryRecord(DatabaseGetRef(), GadgetGetHintCourseIndex());
  UnpackCourse(&c, MemHandleLock(mc));

  mt = DmQueryRecord(DatabaseGetRef(), GadgetGetHintTimeIndex());
  t = (TimeDBRecord *)MemHandleLock(mt);

  // Mi 08:00 - 09:30         <-- Example
  // 2    5   3   5   3       <-- Num Chars for MemPtrNew
  bot=(Char *)MemPtrNew(19);
  MemSet(bot, MemPtrSize(bot), 0);

  TimeToAscii(t->begin.hours, t->begin.minutes, timeFormat, begin);
  TimeToAscii(t->end.hours, t->end.minutes, timeFormat, end);

  mh = DmGetResource(strRsc, GADGET_STRINGS_WDAYSTART+t->day);
  day = (Char *)MemHandleLock(mh);

  CourseTypeGetShort(type, c.ctype);

  StrPrintF(bot, "%s %s - %s", day, begin, end);

  pressedButton=FrmCustomAlert(ALERT_dodel, c.name, type, bot);

  MemPtrFree((MemPtr)bot);
  MemHandleUnlock(mc);
  MemHandleUnlock(mt);
  MemHandleUnlock(mh);

  if (pressedButton == 0) {
    // First Button => Yes, delete
    Err err=DmRemoveRecord(DatabaseGetRef(), GadgetGetHintTimeIndex());
    if (err) FrmCustomAlert(ALERT_debug, "Delete failed #132-a", "", "");
    // We cannot just redraw, unfortunately :-(
    GadgetSetNeedsCompleteRedraw(true);
    FrmDrawForm(FrmGetActiveForm());
  }
}


void DeleteCourse(UInt16 courseInd) {
  MemHandle mc, m;
  UInt16 index=0, courseID=0, pressedButton=1;
  CourseDBRecord c;

  mc = DmQueryRecord(DatabaseGetRef(), courseInd);
  if (!mc) return;
  UnpackCourse(&c, MemHandleLock(mc));
  courseID=c.id;

  pressedButton=FrmCustomAlert(ALERT_dodelc, c.name, "", "");

  MemHandleUnlock(mc);

  if (pressedButton == 0) {
    // First Button => Yes, delete
    Err err=DmRemoveRecord(DatabaseGetRef(), courseInd);
    if (err) FrmCustomAlert(ALERT_debug, "Löschen schlug fehl", "", "");
    else while((m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL) {
      Char *s=MemHandleLock(m);
      Boolean doDel=false;
      if (s[0] == TYPE_TIME) {
        TimeDBRecord *t=(TimeDBRecord *)s;
        if (t->course == courseID)  doDel=true;
      } else if (s[0] == TYPE_EXAM) {
        ExamDBRecord *e=(ExamDBRecord *)s;
        if (e->course == courseID)  doDel=true;
      }
      MemHandleUnlock(m);
      if (doDel) {
        DmRemoveRecord(DatabaseGetRef(), index);
      } else {
        index += 1;
      }
    }
    GadgetSetNeedsCompleteRedraw(true);
  }

}
