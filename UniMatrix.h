/* $Id: UniMatrix.h,v 1.3 2003/03/13 14:56:47 tim Exp $
 *
 */

#ifndef __UNIMATRIX_H_
#define __UNIMATRIX_H_

#define SECOND_SECTION __attribute__ ((section ("SECSEC")))
#define THIRD_SECTION __attribute__ ((section ("THDSEC")))

// define DEBUG

#include <PalmOS.h>
#include "resources.h"

#define MINVERSION 0x03500000
#define APP_CREATOR 'UMTX'

#define UNICHAT_APP_CREATOR 'UCHT'
#define UNIMENSA_APP_CREATOR 'UMEN'

extern Boolean HandleMenuEvent (UInt16 command);
extern Boolean HandleMenuOpenEvent(EventType *event);
extern void *GetObjectPtr(UInt16 objectID);

#endif /* __UNIMATRIX_H_ */
