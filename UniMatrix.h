/***************************************************************************
 *  UniMatrix.h - Main program header
 *
 *  Generated: 2002
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: UniMatrix.h,v 1.5 2005/05/28 12:59:14 tim Exp $
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


typedef enum {
	appLaunchCmdAlarmEventGoto = sysAppLaunchCmdCustomBase
} UniMatrixCustomLaunchCodes;

extern Boolean HandleMenuEvent (UInt16 command);
extern Boolean HandleMenuOpenEvent(EventType *event);
extern void *GetObjectPtr(UInt16 objectID);

#endif /* __UNIMATRIX_H_ */
