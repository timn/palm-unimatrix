
/***************************************************************************
 *  alarm.h - Support for exam alarms
 *
 *  Generated: 2003/04/19
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *  Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.
 *  All rights reserved.
 *
 *  $Id: alarm.h,v 1.5 2005/05/28 12:59:14 tim Exp $
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

#ifndef __ALARM_H
#define __ALARM_H

#include "UniMatrix.h"

#define ALARM_NOTFOUND 0xFFFFFFFF
#define ALARM_REQVER 0x04000000

// Alarm advance - the period of time before the appointment that the 
// alarm should sound.
//
typedef enum {aauMinutes, aauHours, aauDays} AlarmUnitType;

typedef struct {
  UInt8         useAlarm             :1;
  UInt8         advance              :7;  // Alarm advance (-1 = no alarm)
  AlarmUnitType	advanceUnit;	          // minutes, hours, days
  UInt16        repeatCount;
  UInt16        repeatInterval;           // in minutes
  UInt32        soundUniqueRecID;
} AlarmInfoType;

extern void AlarmReset(DmOpenRef cats);
extern void AlarmTriggered(DmOpenRef cats, SysAlarmTriggeredParamType *cmdPBP);
extern Boolean AttentionBottleNeckProc(DmOpenRef cats, AttnLaunchCodeArgsType *paramP);
extern Boolean AlarmFormHandleEvent(EventPtr event) SECOND_SECTION;

#endif /* __ALARM_H */
