/* $Id: cache.h,v 1.1 2003/03/13 14:56:47 tim Exp $
 *
 * Cache functions
 * Created: 2003-03-03
 */

// CacheID < 0 is invalid, meaning that Cache has not been initialized
typedef Int16 CacheID;

#define CACHE_UNINIT -1

typedef void CacheLoadFunc (CacheID id, UInt16 *ids, Char **values, UInt16 numItems);
typedef void CacheFreeFunc (CacheID id, UInt16 *ids, Char **values, UInt16 numItems);
typedef UInt16 CacheNumIFunc (CacheID id);

typedef struct {
  CacheID cacheID;
  UInt16 numItems;
  MemHandle ids;
  MemHandle values;
  CacheLoadFunc *loadFunc;
  CacheFreeFunc *freeFunc;
  CacheNumIFunc *numIFunc;
} CacheObject;

extern void CacheInit(void);
extern void CacheFree(void);
extern void CacheReset(void);
extern CacheID CacheRegister(CacheNumIFunc *numIF, CacheLoadFunc *loadF, CacheFreeFunc *freeF);
extern Boolean CacheGet(CacheID cacheID, UInt16 id, MemHandle *charHandle, UInt16 maxLen);
extern Boolean CacheValid(CacheID id);
