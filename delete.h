/* $Id: delete.h,v 1.1.1.1 2003/02/06 21:27:23 tim Exp $
 *
 * Functions for deleting courses and events
 */

#ifndef __DELETE_H_
#define __DELETE_H_

#include "UniMatrix.h"

#define DELETE_CATEGORY 0

extern void DeleteEntry(void);
extern void DeleteCourse(UInt16 courseInd);

#endif /* __DELETE_H_ */
