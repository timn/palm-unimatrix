/* $Id: alarm.h,v 1.4 2003/11/20 22:55:20 tim Exp $
 *
 * Support for exam alarms
 * Created: 2003/04/19
 * Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.
 */

#ifndef ALARM_H
#define ALARM_H

#include "UniMatrix.h"

#define ALARM_NOTFOUND 0xFFFFFFFF
#define ALARM_REQVER 0x04000000

// Alarm advance - the period of time before the appointment that the 
// alarm should sound.
//
typedef enum {aauMinutes, aauHours, aauDays} AlarmUnitType;

typedef struct {
  UInt8         useAlarm             :1;
	UInt8         advance 			       :7;  // Alarm advance (-1 = no alarm)
	AlarmUnitType	advanceUnit;	            // minutes, hours, days
	UInt16        repeatCount;
	UInt16        repeatInterval;           // in minutes
	UInt32        soundUniqueRecID;
} AlarmInfoType;

extern void AlarmReset(DmOpenRef cats);
extern void AlarmTriggered(DmOpenRef cats, SysAlarmTriggeredParamType *cmdPBP);
extern Boolean AttentionBottleNeckProc(DmOpenRef cats, AttnLaunchCodeArgsType *paramP);
extern Boolean AlarmFormHandleEvent(EventPtr event) SECOND_SECTION;

#endif /* ALARM_H */
