/* $Id: UniMatrix.c,v 1.3 2003/04/18 23:34:59 tim Exp $
 *
 * UniMatrix main, event handling
 * Created: July 2002
 */

#include "UniMatrix.h"
#include "gadget.h"
#include "database.h"
#include "delete.h"
#include "clist.h"
#include "tnglue.h"
#include "edit.h"
#include "beam.h"
#include "prefs.h"
#include "ctype.h"
#include "exams.h"
#include "cache.h"
#include "notes.h"

Char gCategoryName[dmCategoryLength];
UInt16 gMenuCurrentForm=FORM_main;
UniMatrixPrefs gPrefs;
UInt32 gMainRepeat=false;

// backup of key parameters
UInt16 gKeyP1,gKeyP2,gKeyP3;
Boolean gKeyP4;


/***********************************************************************
 * FUNCTION:    GetObjectPtr
 * DESCRIPTION: This routine returns a pointer to an object in the current
 *              form.
 * PARAMETERS:  formId - id of the form to display
 ***********************************************************************/
void * GetObjectPtr(UInt16 objectID) {
	FormPtr frmP = FrmGetActiveForm();
	return (FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, objectID)));
}


/***********************************************************************
 * function is called at program start
 * you can put your own initialization jobs there
 ***********************************************************************/
static UInt16 StartApplication (void) {
  Err err = 0;

	// Initialize the random number seed;
	SysRandom( TimGetSeconds() );

  // Load prefs
  PrefLoadPrefs(&gPrefs);
  
  // Initialize TNglue
  err = TNGlueColorInit();

  // Initialize Cache
  CacheInit();

  // Open Database
  if (err == errNone)  err = OpenDatabase();
  if (err == errNone)  DatabaseSetCat(gPrefs.curCat);

  
  return (err);
}

/***********************************************************************
 * function is checking ROM version of the palmos operating system
 * if ROM version is less then required the result of this function is
 * sysErrRomIncompatible
 ***********************************************************************/
static Err RomVersionCompatible (UInt32 requiredVersion, UInt16 launchFlags) {
	UInt32 romVersion;

	// See if we're on in minimum required version of the ROM or later.
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if (romVersion < requiredVersion) {
		if ((launchFlags & (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
  			(sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) {
			FrmAlert (ALERT_ROMIncompatible);

			// PalmOS before 3.5 will continuously relaunch this app unless we switch to
			// another safe one.
			AppLaunchWithCommand(sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch, NULL);
    }

    return (sysErrRomIncompatible);
  }

  return (0);
}

/***********************************************************************
 * Notification Handle
 ***********************************************************************/
static Err HandleNotification(SysNotifyParamType *notifyParams) {
  switch (notifyParams->notifyType) {
    case sysNotifyLateWakeupEvent:
      if (FrmGetActiveFormID() == FORM_main) {
        // We are on the main form, we have a LateWakup notification => Highlight next event
        GadgetDrawHintNext();
      }
      break;

    default:
      break;
  }

  return errNone;
}



/***********************************************************************
 * handling for the main drop down menu actions
 ***********************************************************************/
Boolean HandleMenuEvent (UInt16 command){
  Boolean handled = false;
  MenuEraseStatus(0);
  gMenuCurrentForm=FrmGetFormId(FrmGetActiveForm());

  switch (command) {
    case MENUITEM_about:
      FrmDoDialog(FrmInitForm(FORM_about));
      handled=true;
      break;

    case MENUITEM_course:
      AddCourse();
      handled=true;
      break;

    case MENUITEM_time:
      if (CountCourses() != 0) {
        AddTime();
      } else {
        FrmAlert(ALERT_nocourses);
      }
      handled=true;
      break;

    case MENUITEM_courselist:
      FrmGotoForm(FORM_courselist);
      handled=true;
      break;

    case MENUITEM_settings:
      FrmPopupForm(FORM_settings);
      handled=true;
      break;

    case MENUITEM_exams:
      FrmGotoForm(FORM_exams);
      handled=true;
      break;

    case MENUITEM_beam:
      BeamSemester(DatabaseGetCat());
      handled=true;
      break;

    case MENUITEM_chat:
			AppLaunchWithCommand(UNICHAT_APP_CREATOR, sysAppLaunchCmdNormalLaunch, NULL);
      handled=true;
      break;

    case MENUITEM_mensa:
			AppLaunchWithCommand(UNIMENSA_APP_CREATOR, sysAppLaunchCmdNormalLaunch, NULL);
      handled=true;
      break;

    default:
      break;
  }

  return handled;
}

Boolean HandleMenuOpenEvent(EventType *event) {
  DmSearchStateType searchState;
  UInt16 cardNo;
  LocalID dbID;
  Boolean needsSeparator=false;
  if (DmGetNextDatabaseByTypeCreator(true, &searchState, 0, UNICHAT_APP_CREATOR, true, &cardNo, &dbID) != dmErrCantFind) {
    MenuAddItem(MENUITEM_exams, MENUITEM_chat, 'C', "UniChat");
    needsSeparator=true;
  }
  if (DmGetNextDatabaseByTypeCreator(true, &searchState, 0, UNIMENSA_APP_CREATOR, true, &cardNo, &dbID) != dmErrCantFind) {
    MenuAddItem(MENUITEM_exams, MENUITEM_mensa, 'M', "UniMensa");
    needsSeparator=true;
  }

  if (needsSeparator) {
    Char tmpSep[2];
    tmpSep[0]=MenuSeparatorChar;
    tmpSep[1]=0;
    MenuAddItem(MENUITEM_exams, MENUITEM_addsep, 0, tmpSep);
}

  return true;
}


/***********************************************************************
 * handling for form and control actions
 * menu actions are forwarded to MainFormDoCommand
 ***********************************************************************/
static Boolean MainFormHandleEvent (EventPtr event){
  FormType *frm;
  Boolean handled = false;
  Boolean categoryEdited, reDraw=false;
  UInt16 category, numRecords;
  ControlType *ctl;
  UInt32 *recordList;

  
  if (event->eType == ctlSelectEvent) {
    // button handling
    handled = true;
    switch (event->data.ctlSelect.controlID) {
      // the ok button - this leaves the application

      case LIST_cat_trigger:
        frm = FrmGetActiveForm();
        category=DatabaseGetCat();
        numRecords=DmNumRecordsInCategory(DatabaseGetRef(), DELETE_CATEGORY);
        recordList=(UInt32 *)MemPtrNew(numRecords * sizeof(UInt32));
        CatPreEdit(numRecords, recordList);
        categoryEdited = CategorySelect(DatabaseGetRef(), frm,
                                        LIST_cat_trigger, LIST_cat, false,
                                        &category, gCategoryName, 0,
                                        STRING_cat_edit); // categoryDefaultEditCategoryString
        if (categoryEdited || (category != DatabaseGetCat())) {
          reDraw=true;
          DatabaseSetCat(category);
          ctl = GetObjectPtr(LIST_cat_trigger);
          CategoryGetName(DatabaseGetRef(), DatabaseGetCat(), gCategoryName); 
          CategorySetTriggerLabel(ctl, gCategoryName); 
        }
        CatPostEdit(numRecords, recordList);
        if (reDraw)  {
          GadgetSetNeedsCompleteRedraw(true);
          FrmDrawForm(frm);
        }
        if (recordList != NULL)    MemPtrFree((MemPtr)recordList);
        break;

      case BUTTON_beam:
        BeamCourse(GadgetGetHintCourseIndex());
        break;

      case BUTTON_edit:
        gMenuCurrentForm=FORM_main;
        EditTime();
        break;

      case BUTTON_next:
        GadgetDrawHintNext();
        break;

      default:
        break;
      }
    } else if (event->eType == keyDownEvent) {
      // We have a hard button assigned and it was pressed
      if (TxtCharIsHardKey(event->data.keyDown.modifiers, event->data.keyDown.chr)) {
        if (! (event->data.keyDown.modifiers & poweredOnKeyMask)) {
          GadgetDrawHintNext();
          handled = true;
        }
      } else if (EvtKeydownIsVirtual(event)) {
        // Up or down keys pressed
        switch (event->data.keyDown.chr) {
          case vchrPageUp:
            if (event->data.keyDown.modifiers & autoRepeatKeyMask) {
              if (! gMainRepeat) {
                GadgetSwitchScreen();
                gMainRepeat = true;
              }
            } else {
              GadgetDrawStep(winUp);
              gMainRepeat = false;
            }
            handled=true;
            break;

          case vchrPageDown:
            if (event->data.keyDown.modifiers & autoRepeatKeyMask) {
              if (! gMainRepeat) {
                GadgetSwitchScreen();
                gMainRepeat = true;
              }
            } else {
              GadgetDrawStep(winDown);
              gMainRepeat = false;
            }
            handled=true;
            break;

          case vchrSendData:
            BeamCourse(GadgetGetHintCourseIndex());
            handled=true;
            break;

          default:
            break;
        }
      }
    } else if (event->eType == menuEvent) {
      // forwarding of menu events
      return HandleMenuEvent(event->data.menu.itemID);
    } else if (event->eType == menuOpenEvent) {
      return HandleMenuOpenEvent(event);
    } else if (event->eType == frmUpdateEvent) {
      // redraws the form if needed
      frm = FrmGetActiveForm();
      FrmDrawForm(frm);
      // GadgetDrawHintNext();
      handled = true;
    } else if (event->eType == frmOpenEvent) {
      ControlType *ctl;
      LocalID dbID;
      UInt16 cardNo;
      Boolean newKeyP4=false;
      UInt16 newKeyP2=0xFFFF;

      // initializes and draws the form at program launch
      frm = FrmGetActiveForm();

      GadgetSet(frm, GADGET_main, GADGET_hint);
      FrmSetGadgetHandler(frm, FrmGetObjectIndex(frm, GADGET_main), GadgetHandler);
      FrmSetGadgetHandler(frm, FrmGetObjectIndex(frm, GADGET_hint), GadgetHintHandler);

      FrmDrawForm(frm);
      GadgetDrawHintNext();

      ctl = GetObjectPtr(LIST_cat_trigger);
      CategoryGetName (DatabaseGetRef(), DatabaseGetCat(), gCategoryName); 
      CategorySetTriggerLabel (ctl, gCategoryName); 

      DmOpenDatabaseInfo(DatabaseGetRefN(DB_MAIN), &dbID, NULL, NULL, &cardNo, NULL);
      SysNotifyRegister(cardNo, dbID, sysNotifyLateWakeupEvent, HandleNotification, sysNotifyNormalPriority, NULL);

    	KeyRates(false,&gKeyP1, &gKeyP2, &gKeyP3, &gKeyP4);
	    KeyRates(true, &gKeyP1, &newKeyP2, &gKeyP3, &newKeyP4);

      handled = true;
    } else if (event->eType == frmCloseEvent) {
      // this is done if program is closed
      LocalID dbID;
      UInt16 cardNo;
      DmOpenDatabaseInfo(DatabaseGetRefN(DB_MAIN), &dbID, NULL, NULL, &cardNo, NULL);
      SysNotifyUnregister(cardNo, dbID, sysNotifyLateWakeupEvent, sysNotifyNormalPriority);
      // Restore original key rates
	    KeyRates(true, &gKeyP1, &gKeyP2, &gKeyP3, &gKeyP4);
    }

  return (handled);
}


/***********************************************************************
 * chooses an displays the requested form
 ***********************************************************************/
static Boolean AppHandleEvent( EventPtr eventP) {
	UInt16 formId;
	FormPtr frmP;

	switch (eventP->eType) {
  	case frmLoadEvent:
      // Load the form resource.
		  formId = eventP->data.frmLoad.formID;
		  frmP = FrmInitForm(formId);
		  FrmSetActiveForm(frmP);

      // Set the event handler for the form.  The handler of the currently
      // active form is called by FrmHandleEvent each time is receives an
      // event.
		  switch (formId) {
			  case FORM_main:
				  FrmSetEventHandler(frmP, MainFormHandleEvent);
				  break;

  			case FORM_courselist: FrmSetEventHandler(frmP, CourseListHandleEvent); break;

        case FORM_evt_det:    FrmSetEventHandler(frmP, EditTimeFormHandleEvent); break;

        case FORM_course_det: FrmSetEventHandler(frmP, EditCourseFormHandleEvent); break;

        case FORM_settings:   FrmSetEventHandler(frmP, SettingsFormHandleEvent); break;

        case FORM_coursetypes: FrmSetEventHandler(frmP, CourseTypeFormHandleEvent); break;

        case FORM_exams:      FrmSetEventHandler(frmP, ExamsFormHandleEvent); break;

        case FORM_exam_details: FrmSetEventHandler(frmP, ExamDetailsFormHandleEvent); break;
        
        case NewNoteView:     FrmSetEventHandler(frmP, NoteViewHandleEvent); break;

        default:
   				ErrNonFatalDisplay("Invalid Form Load Event");
		  		break;
			}
		  break;

	  default:
		  return false;
	}
	return true;
}


/***********************************************************************
 * main event loop; loops until appStopEvent is caught or
 * QuitApp is set
 ***********************************************************************/
static void AppEventLoop(void){
	UInt16 error;
	EventType event;


	do {
		EvtGetEvent(&event, evtWaitForever);


		if (! SysHandleEvent(&event))
			if (! MenuHandleEvent(0, &event, &error))
				if (! AppHandleEvent(&event))
					FrmDispatchEvent(&event);

// Check the heaps after each event
		#if EMULATION_LEVEL != EMULATION_NONE
			MemHeapCheck(0);
			MemHeapCheck(1);
		#endif

	} while (event.eType != appStopEvent);

}

/***********************************************************************
 * application is finished, so we have to clean the desktop behind us
 ***********************************************************************/
static void StopApplication (void){
  gPrefs.curCat = DatabaseGetCat();
  PrefSavePrefs(&gPrefs);
	FrmCloseAllForms ();
  CacheFree();
	CloseDatabase();
}


/***********************************************************************
 * main function
 ***********************************************************************/
UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags){
	UInt16 error = RomVersionCompatible (MINVERSION, launchFlags);
	if (error) return (error);

  /***
  * NORMAL STARTUP
  ****/
	if ( cmd == sysAppLaunchCmdNormalLaunch ) {
    error = StartApplication ();
    if (error) {
			// PalmOS before 3.5 will continuously relaunch this app unless we switch to
			// another safe one.
      if (error != dmErrCorruptDatabase) {
        FrmCustomAlert(ALERT_debug, "Please reports this bug! Give your Palm device and PalmOS version, this BadBug(TM) should not happen.", "", "");
      }
      StopApplication();
			AppLaunchWithCommand(sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch, NULL);
      return error;
    }

    FrmGotoForm(FORM_main);

		AppEventLoop ();
		StopApplication ();

  /***
  * FIND
  ****/
/*
	} else if (cmd == sysAppLaunchCmdSaveData) {
    FrmSaveAllForms();
  } else if (cmd == sysAppLaunchCmdFind) {
    PalmGlobalFind((FindParamsPtr)cmdPBP);
*/

  /***
  * GoTo
  ****/
/*	} else if (cmd == sysAppLaunchCmdGoTo) {
    Boolean launched = launchFlags & sysAppLaunchFlagNewGlobals;

    if (launched) {
      error = StartApplication();
      if (! error) {
        GoToItem((GoToParamsPtr)cmdPBP, launched);
        AppEventLoop();
        StopApplication();
      }
    } else {
      GoToItem((GoToParamsPtr)cmdPBP, launched);
    }
*/

  /***
  * BEAMING
  ****/
	} else if (cmd == sysAppLaunchCmdSyncNotify) {
    // Register with the Exchange Manager
    ExgRegisterData(APP_CREATOR, exgRegExtensionID, "umx");
  } else if (cmd == sysAppLaunchCmdExgAskUser) {
    // Always assume "Yes" as answer to the accept dialog since we display our
    // own on which the user can cancel the data
    ExgAskParamType *exgAskParam = (ExgAskParamType *)cmdPBP;
    exgAskParam->result=exgAskOk;
	} else if (cmd == sysAppLaunchCmdExgReceiveData) {
    DmOpenRef cats=NULL, dogs=NULL;
    // Is app active?
    if (launchFlags & sysAppLaunchFlagSubCall) {
      // Quit Forms
      FrmSaveAllForms();

      cats = DatabaseGetRefN(DB_MAIN);
      dogs = DatabaseGetRefN(DB_DATA);
      error = BeamReceive(cats, dogs, (ExgSocketPtr) cmdPBP);
      CacheReset();
      FrmGotoForm(FORM_main);

    } else {
      // Another app was running when we were called
      cats = DmOpenDatabaseByTypeCreator(DATABASE_TYPE, APP_CREATOR, dmModeReadWrite);
      dogs = DmOpenDatabaseByTypeCreator(DATABASE_DATA_TYPE, APP_CREATOR, dmModeReadWrite);
      if (! (cats && dogs)) {
        FrmAlert(ALERT_beamdbfail);
      } else {
        error=BeamReceive(cats, dogs, (ExgSocketPtr)cmdPBP);
      }
      if (cats)  DmCloseDatabase(cats);
      if (dogs)  DmCloseDatabase(dogs);
    }
	}

	return 0;
}
