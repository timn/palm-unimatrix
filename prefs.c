/* $Id: prefs.c,v 1.1 2003/02/06 21:27:23 tim Exp $
 *
 * Preferences
 */

#include "UniMatrix.h"
#include "prefs.h"
#include "gadget.h"

extern UInt16 gMenuCurrentForm;


/*****************************************************************************
* Functions for editing SETTINGS
*****************************************************************************/

static void SettingsFormInit(FormType *frm) {
  ControlType *ctl, *ctl_ShowType, *ctl_ShowTime;
  UInt8 value_UInt8;
  UInt16 size_UInt8=sizeof(UInt8);
  
  ctl = GetObjectPtr(CHECKBOX_sets_saturday);
  ctl_ShowType = GetObjectPtr(CHECKBOX_sets_showtype);
  ctl_ShowTime = GetObjectPtr(CHECKBOX_sets_showtime);

  if (PrefGetAppPreferences(APP_CREATOR, PREFS_NUMDAYS, &value_UInt8, &size_UInt8, false) == noPreferenceFound)
    value_UInt8=GADGET_DEFAULT_NUMDAYS;
  CtlSetValue(ctl, (value_UInt8 != GADGET_DEFAULT_NUMDAYS));

  if (PrefGetAppPreferences(APP_CREATOR, PREFS_SHOWTYPES, &value_UInt8, &size_UInt8, false) == noPreferenceFound)
    value_UInt8=0;
  CtlSetValue(ctl_ShowType, value_UInt8);

  if (PrefGetAppPreferences(APP_CREATOR, PREFS_SHOWTIMELINE, &value_UInt8, &size_UInt8, false) == noPreferenceFound)
    value_UInt8=0;
  CtlSetValue(ctl_ShowTime, value_UInt8);

}

static Boolean SettingsSave(FormType *frm) {
  ControlType *ctl, *ctl_ShowType, *ctl_ShowTime;
  UInt8 value_UInt8;
  
  ctl_ShowType = GetObjectPtr(CHECKBOX_sets_showtype);
  ctl_ShowTime = GetObjectPtr(CHECKBOX_sets_showtime);
  ctl=GetObjectPtr(CHECKBOX_sets_saturday);

  value_UInt8 = (CtlGetValue(ctl)) ? 6 : 5;
  PrefSetAppPreferences(APP_CREATOR, PREFS_NUMDAYS, PREFS_VERSION, &value_UInt8, sizeof(value_UInt8), false);
  GadgetSetNumDays(value_UInt8);

  ctl_ShowType=GetObjectPtr(CHECKBOX_sets_showtype);
  value_UInt8 = (CtlGetValue(ctl_ShowType)) ? 1 : 0;
  PrefSetAppPreferences(APP_CREATOR, PREFS_SHOWTYPES, PREFS_VERSION, &value_UInt8, sizeof(value_UInt8), false);
  GadgetSetFeature(GADGET_FEAT_SHOWTYPES, value_UInt8);

  ctl_ShowType=GetObjectPtr(CHECKBOX_sets_showtime);
  value_UInt8 = (CtlGetValue(ctl_ShowTime)) ? 1 : 0;
  PrefSetAppPreferences(APP_CREATOR, PREFS_SHOWTIMELINE, PREFS_VERSION, &value_UInt8, sizeof(value_UInt8), false);
  GadgetSetFeature(GADGET_FEAT_SHOWTIMELINE, value_UInt8);

  return true;
}


Boolean SettingsFormHandleEvent(EventPtr event) {
  Boolean handled = false;
  FormType *frm=FrmGetActiveForm();

  if (event->eType == ctlSelectEvent) {
    switch (event->data.ctlSelect.controlID) {

      case BUTTON_sets_ok:
        if (SettingsSave(frm)) {
          // Not (yet) needed: EditCourseFormFree();
          FrmReturnToForm(gMenuCurrentForm);
          FrmUpdateForm(gMenuCurrentForm, frmRedrawUpdateCode);
        }
        handled=true;
        break;

      case BUTTON_sets_cancel:
        // Not (yet) needed: EditCourseFormFree();
        FrmReturnToForm(gMenuCurrentForm);
        FrmUpdateForm(gMenuCurrentForm, frmRedrawUpdateCode);
        handled=true;
        break;

      default:
        break;
    }   
  } else if (event->eType == frmUpdateEvent) {
      // redraws the form if needed
      frm = FrmGetActiveForm();
      FrmDrawForm (frm);
      handled = true;
    } else if (event->eType == frmOpenEvent) {
      // initializes and draws the form at program launch
      frm = FrmGetActiveForm();
      SettingsFormInit(frm);
      FrmDrawForm (frm);
      handled = true;
    } else if (event->eType == frmCloseEvent) {
      // this is done if program is closed
      // NOT called, since we use FrmReturnToForm!
    }



  return handled;
}

