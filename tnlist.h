/* $Id: tnlist.h,v 1.1 2005/05/27 14:59:50 tim Exp $
 *
 * Palm specific list
 * Created: April 7th 2003
 */

#ifndef TNLIST_H
#define TNLIST_H

#include <PalmOS.h>

typedef Int16   TNlistComparFunc (void *a, void *b);
typedef Boolean TNlistFunc (void *data);

typedef struct _TNlist TNlist;

struct _TNlist {
  void    *data;
  TNlist  *prev;
  TNlist  *next;
};

extern TNlist*  TNlistCreateNode(void *data);
extern TNlist*  TNlistAppend(TNlist *list, void *data);
extern void     TNlistFree(TNlist *list);
extern void     TNlistFreeNode(TNlist *list);
extern TNlist*  TNlistRemove(TNlist *list, void *data);
extern TNlist*  TNlistSearch(TNlist *list, void *data, TNlistComparFunc *func);
extern void*    TNlistGetNth(TNlist *list, UInt16 n);
extern TNlist*  TNlistGetNext(TNlist *list);
extern void*    TNlistGetData(TNlist *list);

#endif //TNLIST_H
