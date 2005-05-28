
/***************************************************************************
 *  delete.h - Functions to delete courses and events
 *
 *  Generated: 2002
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: delete.c,v 1.4 2005/05/28 12:59:14 tim Exp $
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "delete.h"
#include "ctype.h"
#include "gadget.h"
#include "notes.h"

/*****************************************************************************
 * Function:  DeleteEntry
 *
 * Description:  Deletes an entry
 *****************************************************************************/
void
DeleteEntry(void)
{
  MemHandle mc, mt, mh, type;
  TimeDBRecord *t;
  CourseDBRecord c;
  Char *day, *bot, begin[timeStringLength], end[timeStringLength];
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

  type = MemHandleNew(1);
  CourseTypeGetShort(&type, c.ctype);

  StrPrintF(bot, "%s %s - %s", day, begin, end);

  pressedButton=FrmCustomAlert(ALERT_dodel, c.name, (Char *)MemHandleLock(type), bot);

  MemHandleUnlock(type);
  MemHandleFree(type);
  MemPtrFree((MemPtr)bot);
  MemHandleUnlock(mc);
  MemHandleUnlock(mt);
  MemHandleUnlock(mh);

  if (pressedButton == 0) {
    // First Button => Yes, delete
    Err err=errNone;
    UInt16 deleteIndex = GadgetGetHintTimeIndex();
    NoteDelete(&deleteIndex);
    err=DmRemoveRecord(DatabaseGetRef(), deleteIndex);
    if (err) FrmCustomAlert(ALERT_debug, "Delete failed #132-a", "", "");
    // We cannot just redraw, unfortunately :-(
    GadgetSetNeedsCompleteRedraw(true);
    FrmDrawForm(FrmGetActiveForm());
  }
}


void
DeleteCourse(UInt16 courseInd)
{
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
