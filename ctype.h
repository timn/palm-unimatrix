
/***************************************************************************
 *  ctype.h - Course types
 *
 *  Generated: 2002-08-31
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: ctype.h,v 1.3 2005/05/28 12:59:14 tim Exp $
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

#ifndef __CTYPE_H
#define __CTYPE_H

#define CTYPE_SHORT_MAXLENGTH 3
#define CTYPE_MAXLENGTH 20
// this is a space and [ and ], NOT the NULL termination! Calc that extra!
#define CTYPE_ADD_MAXLENGTH 3
#define CTYPE_DEF_NUM 6

extern UInt8 CourseTypeListPopup(UInt16 listID, UInt16 triggerID, UInt8 selected, Char *triggerLabel) THIRD_SECTION;
extern void CourseTypeGetName(Char *name, UInt8 id) THIRD_SECTION;
extern void CourseTypeGetShort(MemHandle *charHandle, UInt8 id) THIRD_SECTION;
extern void CourseTypeGetShortByCourseID(MemHandle *charHandle, UInt16 courseID) THIRD_SECTION;
extern Boolean CourseTypeFormHandleEvent(EventPtr event) THIRD_SECTION;
extern Boolean CourseTypeGetDBIndex(UInt8 courseTypeID, UInt16 *courseTypeDBindex) THIRD_SECTION;

#endif // __CTYPE_H
