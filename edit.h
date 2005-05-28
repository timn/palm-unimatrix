
/***************************************************************************
 *  edit.h - Functions for editing courses, events, times, ...
 *
 *  Generated: 2002
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: edit.h,v 1.4 2005/05/28 12:59:14 tim Exp $
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

#ifndef __EDIT_H
#define __EDIT_H

extern void CatPreEdit(UInt16 numRecords, UInt32 *recordList) SECOND_SECTION;
extern void CatPostEdit(UInt16 numRecords, UInt32 *recordList) SECOND_SECTION;
extern void EditCourse(UInt16 courseInd) SECOND_SECTION;
extern Boolean EditCourseAutoFill(EventPtr event) SECOND_SECTION;
extern Boolean EditCourseFormHandleEvent(EventPtr event);
extern Boolean EditTimeAutoFill(EventPtr event) SECOND_SECTION;
extern Boolean EditTimeFormHandleEvent(EventPtr event);
extern void EditTimeGetTime(ControlType *ctl, TimeType *begin, TimeType *end, Char *label) SECOND_SECTION;
extern void EditTimeSetSelectorText(ControlType *ctl, TimeType *begin, TimeType *end, Char *label);
extern Boolean EditTimeCheckCollision(TimeType begin, TimeType end, UInt8 day, UInt16 notIndex, Boolean checkIndex);
extern void EditTime(void);
extern void AddCourse(void);
extern void AddTime(void);

#endif
