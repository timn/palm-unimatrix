
/***************************************************************************
 *  clist.h - Functions for course list
 *
 *  Generated: 2002-07-11
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: clist.h,v 1.5 2005/05/28 12:59:14 tim Exp $
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

#ifndef __CLIST_H
#define __CLIST_H

#define CLIST_SEARCH_INDEX 1
#define CLIST_SEARCH_ID 2

extern UInt16 CourseListGen(Char **itemList, UInt16 *courseID, UInt16 *courseInd, UInt16 numItems, UInt16 curInd, UInt8 searchFor) SECOND_SECTION;
extern Boolean CourseListHandleEvent(EventPtr event) SECOND_SECTION;
extern void CourseGetName(UInt16 courseID, MemHandle *charHandle, Boolean longformat) SECOND_SECTION;
extern UInt8 CourseGetType(UInt16 courseID) SECOND_SECTION;
extern Boolean CourseGetIndex(DmOpenRef cats, UInt16 category, UInt16 courseID, UInt16 *index);
extern UInt16 CourseNewID(DmOpenRef cat, UInt16 category) SECOND_SECTION;
extern UInt16 CountCourses(void) SECOND_SECTION;

#endif
