/* $Id: prefs.h,v 1.3 2003/03/13 14:56:47 tim Exp $
 *
 * Preferences code
 */

#define PREFS_VERSION 2
#define PREFS_ID 1

typedef struct {
  UInt8 curCat;
  UInt8 numDays;
  UInt8 showTypes;
  UInt8 showTimeline;
  UInt8 showShortNames;
} UniMatrixPrefs;

extern Boolean SettingsFormHandleEvent(EventPtr event) SECOND_SECTION;
extern void PrefLoadPrefs(UniMatrixPrefs *prefs) SECOND_SECTION;
extern void PrefSavePrefs(UniMatrixPrefs *prefs) SECOND_SECTION;
