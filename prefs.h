/* $Id: prefs.h,v 1.4 2003/04/25 23:24:38 tim Exp $
 *
 * Preferences code
 */

#ifndef PREFS_H
#define PREFS_H

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

#endif // PREFS_H
