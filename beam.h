
/***************************************************************************
 *  beam.h - Functions for beaming
 *
 *  Generated: 2002-05-02
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: beam.h,v 1.3 2005/05/28 12:59:14 tim Exp $
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

#ifndef __BEAM_H
#define __BEAM_H

#define BEAM_DESC "UniMatrix Database"
#define BEAM_NAME "UniMatrix"
#define BEAM_ENDG ".umx"

#define BEAM_COURSE 3
#define BEAM_SEMESTER 2

extern void BeamCourse(UInt16 courseIndex) SECOND_SECTION;
extern void BeamCourseByCID(UInt16 cid) SECOND_SECTION;
extern Err BeamReceive(DmOpenRef cats, DmOpenRef dogs, ExgSocketPtr socketPtr) SECOND_SECTION;
extern void BeamSemester(UInt16 category) SECOND_SECTION;

#endif // __BEAM_h
