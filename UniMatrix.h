/* $Id: UniMatrix.h,v 1.2 2003/02/07 01:07:52 tim Exp $
 *
 */

#ifndef __UNIMATRIX_H_
#define __UNIMATRIX_H_

#define SECOND_SECTION __attribute__ ((section ("SECSEC")))

// define DEBUG

#include <PalmOS.h>
#include "resources.h"

#define MINVERSION 0x03500000
#define APP_CREATOR 'UMTX'

#define UNICHAT_APP_CREATOR 'UCHT'
#define UNIMENSA_APP_CREATOR 'UMEN'

#define PREFS_VERSION 1
#define PREFS_CURCAT 1
#define PREFS_NUMDAYS 2
#define PREFS_SHOWTYPES 3
#define PREFS_SHOWTIMELINE 4



extern Boolean HandleMenuEvent (UInt16 command);
extern Boolean HandleMenuOpenEvent(EventType *event);
extern void *GetObjectPtr(UInt16 objectID);

#endif /* __UNIMATRIX_H_ */
