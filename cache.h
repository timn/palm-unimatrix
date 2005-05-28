
/***************************************************************************
 *  cache.h - Cache functions for ID -> String caches
 *
 *  Generated: 2003-03-03
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: cache.h,v 1.2 2005/05/28 12:59:14 tim Exp $
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

#ifndef __CACHE_H
#define __CACHE_H


// CacheID < 0 is invalid, meaning that Cache has not been initialized
typedef Int16 CacheID;

#define CACHE_UNINIT -1

typedef void CacheLoadFunc (CacheID id, UInt16 *ids, Char **values, UInt16 numItems);
typedef void CacheFreeFunc (CacheID id, UInt16 *ids, Char **values, UInt16 numItems);
typedef UInt16 CacheNumIFunc (CacheID id);

typedef struct {
  CacheID        cacheID;
  UInt16         numItems;
  MemHandle      ids;
  MemHandle      values;
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

#endif // __CACHE_H
