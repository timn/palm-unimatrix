
/***************************************************************************
 *  exam.h - Handling of exams goes here
 *
 *  Generated: 2002-09-21
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: exams.h,v 1.6 2005/05/28 12:59:14 tim Exp $
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


#ifndef __EXAMS_H
#define __EXAMS_H


#define EXCOL_DONE 0
#define EXCOL_COURSE 1
#define EXCOL_NOTE 2
#define EXCOL_DATE 3
#define EXCOL_TIME 4
#define EXCOL_SELI 5

#define EX_MAX_ROWS 10
#define EX_NUM_HANDLECOLS 3

#define EX_FLAG_DONE 0x0001

extern void ExamSetGoto(UInt32 uniqueID);
extern Boolean ExamsFormHandleEvent(EventPtr event) SECOND_SECTION;
extern Boolean ExamDetailsFormHandleEvent(EventPtr event) SECOND_SECTION;

#endif // __EXAMS_H
