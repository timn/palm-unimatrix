/* $Id: notes.h,v 1.2 2003/04/25 23:24:38 tim Exp $
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

extern void NoteSet(UInt16 noteItemIndex, UInt16 returnForm);
extern Boolean NoteViewHandleEvent (EventType *event);
extern Boolean NoteGetIndex(UInt16 noteID, UInt16 *noteIndex);
extern UInt16 NoteGetNewID(DmOpenRef cats, UInt16 category);
extern void NoteDelete(UInt16 *noteItemIndex);

#endif // NOTES_H
