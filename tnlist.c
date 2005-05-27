/* $Id: tnlist.c,v 1.1 2005/05/27 14:59:50 tim Exp $
 *
 * Palm specific list
 * Created: April 7th 2003
 */

#include "tnlist.h"


TNlist* TNlistCreateNode(void *data) {
  TNlist *l=MemPtrNew(sizeof(TNlist));
  l->data=data;
  l->prev=NULL;
  l->next=NULL;
  return l;
}


void TNlistFreeNode(TNlist *node) {
  if (node) {
    MemPtrFree((MemPtr)node->data);
    MemPtrFree((MemPtr)node);
  }
}


TNlist* TNlistAppend(TNlist *list, void *data) {

  TNlist *new=TNlistCreateNode(data);
  
  if (list) {
    TNlist *l=list;

    while (l->next != NULL)  l = l->next;

    new->prev=l;    
    l->next=new;
    return list;
  } else {
    return new;
  }
}

void TNlistRemoveLink(TNlist *list) {
  if (list) {
    if (list->prev)  list->prev->next = list->next;
    if (list->next)  list->next->prev = list->prev;
  }
}


TNlist* TNlistRemove(TNlist *list, void *data) {
  TNlist *tmp=list;

  while (tmp) {
    if (tmp->data != data) {
      tmp = tmp->next;
    } else {
      // Found entry
      if (tmp == list)  list = list->next;

      TNlistRemoveLink(tmp);
      TNlistFreeNode(tmp);
      break;
    }
  }
  return list;
}
  

void TNlistFree(TNlist *list) {
  TNlist *last;
  while (list != NULL) {
    last = list;
    list = list->next;
    TNlistFreeNode(last);
  }
}


TNlist* TNlistInsertSorted(TNlist *list, void *data, TNlistComparFunc *compFunc) {
  
  TNlist *new=TNlistCreateNode(data);

  if (list == NULL) {
    return new;
  } else {
    Int16 cmp;
    TNlist *tmp_list=list;

    cmp = compFunc(new->data, tmp_list->data);

    while (tmp_list->next && (cmp > 0)) {
      tmp_list = tmp_list->next;
      cmp = compFunc(new->data, tmp_list->data);
    }

    if (!tmp_list->next && (cmp > 0)) {
      new->prev=tmp_list;
      tmp_list->next=new;
      return list;
    }

    if (tmp_list->prev) {
      tmp_list->prev->next = new;
      new->prev = tmp_list->prev;
    }
    new->next = tmp_list;
    tmp_list->prev = new;

    if (tmp_list == list) {
      return new;
    } else {
      return list;
    }
  }
}


void TNlistForeach(TNlist *list, TNlistFunc *func) {
  while (list && func(list->data)) list = list->next;
}


TNlist* TNlistSearch(TNlist *list, void *data, TNlistComparFunc *func) {
  Int16 cmp = 1;

  while (list && (cmp != 0)) {
    cmp = func(list->data, data);
    if (cmp != 0)  list = list->next;
  }

  if (list && (cmp == 0)) {
    return list;
  } else {
    return NULL;
  }
}

void* TNlistGetNth(TNlist *list, UInt16 n) {
  TNlist *tmp=list;

  while ( (n-- >= 1) && tmp ) {
    tmp = tmp->next;
  }

  return tmp ? tmp->data : NULL;
}

TNlist*
TNlistGetNext(TNlist *list)
{
  if (list != NULL) {
    return list->next;
  } else {
    return NULL;
  }
}

void*
TNlistGetData(TNlist *list)
{
  if (list != NULL) {
    return list->data;
  } else {
    return NULL;
  }
}
