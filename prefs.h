
/***************************************************************************
 *  prefs.h - Handling of preferences
 *
 *  Generated: 2002-09-21
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: prefs.h,v 1.5 2005/05/28 12:59:14 tim Exp $
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

#ifndef __PREFS_H
#define __PREFS_H

#define PREFS_VERSION 3
#define PREFS_ID 1

#include "alarm.h"

typedef struct {
  UInt16        curCat;

  UInt8         numDays         :3;
  UInt8         showTypes       :1;
  UInt8         showTimeline    :1;
  UInt8         showShortNames  :1;
  UInt8         muteExams       :1;
  UInt8         muteEvents      :1;

  UInt8         reserved1;
  UInt16        reserved2;
  UInt16        reserved3;

  AlarmInfoType alarmInfo;

} UniMatrixPrefs;

extern Boolean SettingsFormHandleEvent(EventPtr event) SECOND_SECTION;
extern void PrefLoadPrefs(UniMatrixPrefs *prefs);
extern void PrefSavePrefs(UniMatrixPrefs *prefs) SECOND_SECTION;


// OLD, for import
typedef struct {
  UInt8         curCat;
  UInt8         numDays;
  UInt8         showTypes;
  UInt8         showTimeline;
  UInt8         showShortNames;
} UniMatrixPrefs_v2;

#endif // __PREFS_H
