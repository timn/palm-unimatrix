
/***************************************************************************
 *  notes.h - Note support functions
 *
 *  Generated: 2003/04/17
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: notes.h,v 1.4 2005/05/28 12:59:14 tim Exp $
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


#ifndef __NOTES_H
#define __NOTES_H

#include "UniMatrix.h"
#include "tnglue.h"
#include "database.h"
#include "clist.h"
#include "gadget.h"

extern void NoteSet(UInt16 noteItemIndex, UInt16 returnForm) SECOND_SECTION;
extern Boolean NoteViewHandleEvent (EventType *event) SECOND_SECTION;
extern Boolean NoteGetIndex(UInt16 noteID, UInt16 *noteIndex) SECOND_SECTION;
extern UInt16 NoteGetNewID(DmOpenRef cats, UInt16 category) SECOND_SECTION;
extern void NoteDelete(UInt16 *noteItemIndex) SECOND_SECTION;

#endif // __NOTES_H
