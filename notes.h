/* $Id: notes.h,v 1.3 2003/04/29 23:03:48 tim Exp $
 *
 * Note support functions
 * Created: 2003/04/17
 */

#ifndef NOTES_H
#define NOTES_H

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

#endif // NOTES_H
