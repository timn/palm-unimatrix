/* $Id: notes.h,v 1.1 2003/04/18 23:34:59 tim Exp $
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

#endif // NOTES_H
