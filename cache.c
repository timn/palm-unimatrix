/* $Id: cache.c,v 1.2 2003/10/15 21:40:57 tim Exp $
 *
 * Cache functions for ID -> String Caches
 * Created: 2003-03-03
 */

#include "UniMatrix.h"
#include "cache.h"

MemHandle gCaches;
UInt16 gCacheNum=0;

void CacheInit(void) {
  gCaches = MemHandleNew(1);
  gCacheNum = 0;
}

void CacheFree(void) {
  UInt16 i;
  CacheObject **caches=MemHandleLock(gCaches);

  for (i=0; i < gCacheNum; ++i) {
    // Call Cache free func
    if (caches[i]->freeFunc != NULL) {
      caches[i]->freeFunc(caches[i]->cacheID, (UInt16 *)MemHandleLock(caches[i]->ids), (Char **)MemHandleLock(caches[i]->values), caches[i]->numItems);
      MemHandleUnlock(caches[i]->ids);
      MemHandleUnlock(caches[i]->values);
      MemHandleFree(caches[i]->ids);
      MemHandleFree(caches[i]->values);
    }
    MemPtrFree(caches[i]);
  }

  MemHandleUnlock(gCaches);
  MemHandleFree(gCaches);
  gCacheNum = 0;
}

static void CacheSort(CacheID cacheID) {
  CacheObject **caches;
  UInt16 *ids, v=0, i=0, j=0;
  Char **values, *vb;

  if (CacheValid(cacheID)) {
  
    caches = MemHandleLock(gCaches);
    ids = (UInt16 *)MemHandleLock(caches[cacheID]->ids);
    values = (Char **)MemHandleLock(caches[cacheID]->values);
  
    // Insertion sort
    for (i=1; i < caches[cacheID]->numItems; ++i) {
      v = ids[i];
      vb = values[i];
      j = i;
      while (ids[j-1] > v) {
        ids[j] = ids[j-1];
        values[j] = values[j-1];
        j -= 1;
      }
      ids[j] = v;
      values[j] = vb;
    }
  
    MemHandleUnlock(caches[cacheID]->ids);
    MemHandleUnlock(caches[cacheID]->values);
    MemHandleUnlock(gCaches);
  }
}


void CacheReset(void) {
  UInt16 i;
  CacheObject **caches=MemHandleLock(gCaches);

  for (i=0; i < gCacheNum; ++i) {
    // Call Cache free func
    caches[i]->freeFunc(caches[i]->cacheID, (UInt16 *)MemHandleLock(caches[i]->ids), (Char **)MemHandleLock(caches[i]->values), caches[i]->numItems);
    MemHandleUnlock(caches[i]->ids);
    MemHandleUnlock(caches[i]->values);

    caches[i]->numItems = caches[i]->numIFunc(caches[i]->cacheID);
    
    MemHandleResize(caches[i]->ids, caches[i]->numItems * sizeof(UInt16));
    MemHandleResize(caches[i]->values, caches[i]->numItems * sizeof(Char *));

    caches[i]->loadFunc(caches[i]->cacheID, (UInt16 *)MemHandleLock(caches[i]->ids), (Char **)MemHandleLock(caches[i]->values), caches[i]->numItems);
    MemHandleUnlock(caches[i]->ids);
    MemHandleUnlock(caches[i]->values);

    CacheSort(i);

  }

  MemHandleUnlock(gCaches);
}




CacheID CacheRegister(CacheNumIFunc *numIF, CacheLoadFunc *loadF, CacheFreeFunc *freeF) {
  CacheObject *newCache;
  CacheObject **caches;
  CacheID newCacheID;
  MemHandle mIDs, mValues;

  newCacheID = gCacheNum;
  gCacheNum += 1;

  MemHandleResize(gCaches, gCacheNum * sizeof(MemPtr));
  newCache = (CacheObject *)MemPtrNew(sizeof(CacheObject));

  caches = MemHandleLock(gCaches);
  caches[newCacheID] = newCache;
  MemHandleUnlock(gCaches);

  newCache->cacheID=newCacheID;
  newCache->freeFunc = freeF;
  newCache->loadFunc = loadF;
  newCache->numIFunc = numIF;
  newCache->numItems = numIF(newCacheID);

  // If numItems is zero then zero bytes would be allocated, this results
  // in a null handle an causes BadBugs(TM). So care about it and
  // enforce a minimum of one entry!
  mIDs=MemHandleNew(((newCache->numItems > 0) ? newCache->numItems : 1) * sizeof(UInt16));
  mValues=MemHandleNew(((newCache->numItems > 0) ? newCache->numItems : 1) * sizeof (Char *));

  newCache->ids=mIDs;
  newCache->values = mValues;

  if (newCache->loadFunc != NULL) {
    newCache->loadFunc(newCacheID, (UInt16 *)MemHandleLock(newCache->ids), (Char **)MemHandleLock(newCache->values), newCache->numItems);
  }
  MemHandleUnlock(newCache->ids);
  MemHandleUnlock(newCache->values);

  CacheSort(newCacheID);

  return newCacheID;
}

Boolean CacheGet(CacheID cacheID, UInt16 id, MemHandle *charHandle, UInt16 maxLen) {
  Int32 l, r, x;
  CacheObject **caches;
  UInt16 *ids;
  Char **values;
  Boolean rv=false;

  if (CacheValid(cacheID)) {

    caches = MemHandleLock(gCaches);
    
    if (caches[cacheID]->numItems == 0) {
      MemHandleUnlock(gCaches);
      return false;
    }
    
    ids = (UInt16 *)MemHandleLock(caches[cacheID]->ids);
    values = (Char **)MemHandleLock(caches[cacheID]->values);

    // Binary search
    l = 0;
    r = caches[cacheID]->numItems-1;
    do {
      x = (l+r)/2;
      if (id < ids[x]) {
        r = x-1;
      } else {
        l = x+1;
      }
    } while ( (id != ids[x]) && (l <= r));
  
    if (id == ids[x]) {
      Char *dst;
      UInt16 length;

      length = (maxLen == 0) ? StrLen(values[x]) : maxLen;
      MemHandleResize(*charHandle, length+1);
      dst = (Char *)MemHandleLock(*charHandle);
      MemSet(dst, length+1, 0);
      StrNCopy(dst, values[x], length);
      MemHandleUnlock(*charHandle);

      rv = true;
    }

    MemHandleUnlock(caches[cacheID]->ids);
    MemHandleUnlock(caches[cacheID]->values);
    MemHandleUnlock(gCaches);

  }

  return rv;
}

Boolean CacheValid(CacheID id) {
  return ((id >= 0) && (id < gCacheNum));
}
