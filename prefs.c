
/***************************************************************************
 *  prefs.h - Handling of preferences
 *
 *  Generated: 2002-09-21
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: prefs.c,v 1.4 2005/05/28 12:59:14 tim Exp $
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

#include "UniMatrix.h"
#include "prefs.h"
#include "gadget.h"

extern UInt16         gMenuCurrentForm;
extern UniMatrixPrefs gPrefs;

void
PrefLoadPrefs(UniMatrixPrefs *prefs)
{
  UInt16 prefsSize=0;
  Int16 version;

  version = PrefGetAppPreferences(APP_CREATOR, PREFS_ID, NULL, &prefsSize, false);
  
  if (version == noPreferenceFound) {
    MemSet(prefs, sizeof(UniMatrixPrefs), 0);
    prefs->numDays = 5;
  } else if (version != PREFS_VERSION) {
    // Attempt import if old was smaller
    if (version == 2) {
      MemHandle m=MemHandleNew(prefsSize);
      MemPtr mp = MemHandleLock(m);
      UniMatrixPrefs_v2 *oldPrefs;
  
      PrefGetAppPreferences(APP_CREATOR, PREFS_ID, (Char *)mp, &prefsSize, false);
      oldPrefs = (UniMatrixPrefs_v2 *)mp;

      prefs->curCat    = oldPrefs->curCat;
      // Seems strange but is needed :-(
      prefs->numDays = oldPrefs->numDays;
      prefs->showTypes = oldPrefs->showTypes;
      prefs->showTimeline = oldPrefs->showTimeline;
      prefs->showShortNames = oldPrefs->showShortNames;

      PrefSetAppPreferences(APP_CREATOR, PREFS_ID, version, NULL, 0, false);

      MemHandleUnlock(m);
      MemHandleFree(m);

      PrefSavePrefs(prefs);

    } else if (prefsSize <= sizeof(UniMatrixPrefs)) {

      MemHandle m=MemHandleNew(prefsSize);
      MemPtr mp = MemHandleLock(m);
  
      PrefGetAppPreferences(APP_CREATOR, PREFS_ID, (Char *)mp, &prefsSize, false);
  
      MemSet(prefs, sizeof(UniMatrixPrefs), 0);
      MemMove(prefs, mp, prefsSize);
      prefs->numDays = 5;

      PrefSetAppPreferences(APP_CREATOR, PREFS_ID, version, NULL, 0, false);

      MemHandleUnlock(m);
      MemHandleFree(m);

      PrefSavePrefs(prefs);
    }
  } else {
    // Load
    PrefGetAppPreferences(APP_CREATOR, PREFS_ID, prefs, &prefsSize, false);
  }

}

void
PrefSavePrefs(UniMatrixPrefs *prefs)
{
  PrefSetAppPreferences(APP_CREATOR, PREFS_ID, PREFS_VERSION, prefs, sizeof(UniMatrixPrefs), false);
}


/*****************************************************************************
* Functions for editing SETTINGS
*****************************************************************************/

static void
SettingsFormInit(FormType *frm)
{
  ControlType *ctl;
  
  ctl = GetObjectPtr(CHECKBOX_sets_saturday);
  CtlSetValue(ctl, (gPrefs.numDays != GADGET_DEFAULT_NUMDAYS));

  ctl = GetObjectPtr(CHECKBOX_sets_showtype);
  CtlSetValue(ctl, gPrefs.showTypes);

  ctl = GetObjectPtr(CHECKBOX_sets_showshort);
  CtlSetValue(ctl, gPrefs.showShortNames);

  ctl = GetObjectPtr(CHECKBOX_sets_showtime);
  CtlSetValue(ctl, gPrefs.showTimeline);

}

static Boolean
SettingsSave(FormType *frm)
{
  ControlType *ctl;
  
  ctl=GetObjectPtr(CHECKBOX_sets_saturday);
  gPrefs.numDays = (CtlGetValue(ctl)) ? 6 : 5;
  GadgetSetNumDays(gPrefs.numDays);

  ctl=GetObjectPtr(CHECKBOX_sets_showtype);
  gPrefs.showTypes = (CtlGetValue(ctl)) ? 1 : 0;

  ctl=GetObjectPtr(CHECKBOX_sets_showtime);
  gPrefs.showTimeline = (CtlGetValue(ctl)) ? 1 : 0;

  ctl=GetObjectPtr(CHECKBOX_sets_showshort);
  gPrefs.showShortNames = (CtlGetValue(ctl)) ? 1 : 0;

  PrefSavePrefs(&gPrefs);
  return true;
}


Boolean
SettingsFormHandleEvent(EventPtr event)
{
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

      case CHECKBOX_sets_showshort:
        if (CtlGetValue(event->data.ctlSelect.pControl)) {
          CtlSetValue(GetObjectPtr(CHECKBOX_sets_showtype), 0);
        }
        break;

      case CHECKBOX_sets_showtype:
        if (CtlGetValue(event->data.ctlSelect.pControl)) {
          CtlSetValue(GetObjectPtr(CHECKBOX_sets_showshort), 0);
        }
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

