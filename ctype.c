/* $Id: ctype.c,v 1.1 2003/02/06 21:27:23 tim Exp $
 *
 * Course Type related stuff
 * Created: 2002-08-31
 */

#include "UniMatrix.h"
#include "ctype.h"
#include "database.h"
#include "clist.h"


UInt16 gCourseTypeCurrentForm=FORM_main, gNumCourseTypes;
UInt8 *gCourseTypeID;
Char **gCourseTypeList;

extern UInt8 gEditCourseType;


static UInt16 CourseTypesNum(void) {
  MemHandle m;
  UInt16 index=0, numItems=0;

  while (((m = DmQueryNextInCategory(DatabaseGetRefN(DB_DATA), &index, 0)) != NULL)) {
    Char *s = (Char *)MemHandleLock(m);
    if (s[0] == TYPE_CTYP)  numItems++;
    MemHandleUnlock(m);
    index += 1;
  }

  return numItems;
}

static UInt16 CourseTypeFillList(Char **itemList, UInt8 *itemIDs, UInt16 numItems, UInt8 selectedID, Boolean edit) {
  MemHandle m;
  UInt16 index=0, i=0, selectedItem=0;
  Char *tempString;

  while((i < numItems) && ((m = DmQueryNextInCategory(DatabaseGetRefN(DB_DATA), &index, 0)) != NULL)) {
    CourseTypeDBRecord *ct = (CourseTypeDBRecord *)MemHandleLock(m);
    if (ct->type == TYPE_CTYP) {
      tempString = (Char *)MemPtrNew(StrLen(ct->name)+4+CTYPE_SHORT_MAXLENGTH);
      StrPrintF(tempString, "%s [%s]", ct->name, ct->shortName);
      itemList[i] = tempString;
      itemIDs[i] = ct->id;
      if (ct->id == selectedID)  selectedItem=i;
      i += 1;
    }
    MemHandleUnlock(m);
    index += 1;
  }

  return selectedItem;
}


/*****************************************************************************
* Function: CourseTypeListPopup
*
* Description: Manages the coursetype list
*****************************************************************************/
UInt8 CourseTypeListPopup(UInt16 listID, UInt16 triggerID, UInt8 selected, Char *triggerLabel) {
  UInt16 numItems=0, selectedItem=0, i=0;
  MemHandle m;
  Char **itemList=NULL, *tempString;
  UInt8 *itemIDs=NULL;
  Int16 tapped;
  UInt8 selectedID=selected;
  ListType *lst=GetObjectPtr(listID);


  numItems = CourseTypesNum();
  itemList = (Char **) MemPtrNew((numItems+1) * sizeof(Char *));
  itemIDs = (UInt8 *) MemPtrNew((numItems+1) * sizeof(UInt8));

  selectedItem = CourseTypeFillList(itemList, itemIDs, numItems, selectedID, true);

  m = DmGetResource(strRsc, STRING_edit_ctype);
  tempString = (Char *)MemHandleLock(m);
  itemList[numItems] = MemPtrNew(StrLen(tempString)+1);
  MemSet(itemList[numItems], MemPtrSize(itemList[numItems]), 0);
  StrCopy(itemList[numItems], tempString);
  MemHandleUnlock(m);
  numItems += 1;


  LstSetListChoices(lst, itemList, numItems);
  LstSetSelection(lst, selectedItem);

  if (numItems < 7) LstSetHeight(lst, numItems);
  else              LstSetHeight(lst, 7);

  tapped = LstPopupList(lst);


  if (tapped == -1) {
    // Don't do anything
  } else if (tapped == (numItems-1)) {
    // Call edit
    gCourseTypeCurrentForm=FrmGetFormId(FrmGetActiveForm());
    FrmPopupForm(FORM_coursetypes);
  } else {
    //User tapped an entry
    ControlType *ctl = GetObjectPtr(triggerID);
    StrCopy(triggerLabel, LstGetSelectionText(lst, tapped));
    CtlSetLabel(ctl, triggerLabel);
    selectedID=itemIDs[tapped];
  }

  for (i=0; i < numItems; ++i)
    MemPtrFree((MemPtr)itemList[i]);
  MemPtrFree((MemPtr)itemList);
  MemPtrFree((MemPtr)itemIDs);

  return selectedID;
}

void CourseTypeGetName(Char *name, UInt8 id) {
  Boolean found=false;
  UInt16 index = 0;
  MemHandle m;

  while(! found && ((m = DmQueryNextInCategory(DatabaseGetRefN(DB_DATA), &index, 0)) != NULL)) {
    CourseTypeDBRecord *ct = (CourseTypeDBRecord *)MemHandleLock(m);
    if ((ct->type == TYPE_CTYP) && (ct->id == id) ) {
      StrPrintF(name, "%s [%s]", ct->name, ct->shortName);
      found=true;
    }
    MemHandleUnlock(m);
    index += 1;
  }

  if (!found) {
    Char *tmp;
    m = DmGetResource(strRsc, STRING_ctype_unknown);
    tmp = (Char *)MemHandleLock(m);
    StrNCopy(name, tmp, CTYPE_MAXLENGTH);
    MemHandleUnlock(m);
  }
}


void CourseTypeGetShort(Char *name, UInt8 id) {
  Boolean found=false;
  UInt16 index = 0;
  MemHandle m;

  while(! found && ((m = DmQueryNextInCategory(DatabaseGetRefN(DB_DATA), &index, 0)) != NULL)) {
    CourseTypeDBRecord *ct = (CourseTypeDBRecord *)MemHandleLock(m);
    if ((ct->type == TYPE_CTYP) && (ct->id == id) ) {
      StrPrintF(name, "%s", ct->shortName);
      found=true;
    }
    MemHandleUnlock(m);
    index += 1;
  }

  if (!found) {
    Char *tmp;
    m = DmGetResource(strRsc, STRING_ctype_short_unknown);
    tmp = (Char *)MemHandleLock(m);
    StrNCopy(name, tmp, CTYPE_SHORT_MAXLENGTH);
    MemHandleUnlock(m);
  }

}


void CourseTypeGetShortByCourseID(Char *name, UInt16 courseID) {
  UInt8 courseType = CourseGetType(courseID);
  CourseTypeGetShort(name, courseType);
}

Boolean CourseTypeGetDBIndex(UInt8 courseTypeID, UInt16 *courseTypeDBindex) {
  Boolean found=false;
  MemHandle m;
  UInt16 index=0;
  while(! found && ((m = DmQueryNextInCategory(DatabaseGetRefN(DB_DATA), &index, 0)) != NULL)) {
    CourseTypeDBRecord *ct = (CourseTypeDBRecord *)MemHandleLock(m);
    if ((ct->type == TYPE_CTYP) && (ct->id == courseTypeID) ) {
      found=true;
    } else {
      index++;
    }
    MemHandleUnlock(m);
  }
  if (found)  *courseTypeDBindex = index;
  return found;
}



/* LOCAL *********************************************************************
* Function: CourseTypeEdit
*
* Description: Edit a course type
*****************************************************************************/
static void CourseTypeEdit(UInt8 id) {
  FormType *frm, *previousForm;
  FieldType *fldName, *fldShortName;
  CourseTypeDBRecord *savedCT=NULL;
  UInt16 clickedButton;
  MemHandle m, old, name, shortName;
  UInt16 index;
  Char *buffer;

  previousForm = FrmGetActiveForm();
  frm = FrmInitForm(FORM_ct_details);
  FrmSetActiveForm(frm);
  fldName = GetObjectPtr(FIELD_ctdet_name);
  fldShortName = GetObjectPtr(FIELD_ctdet_short);

  // Look for index of entry
  if (! CourseTypeGetDBIndex(id, &index)) return;

  // Get MemHandle and lock it
  m = DmQueryRecord(DatabaseGetRefN(DB_DATA), index);
  savedCT = MemHandleLock(m);
  
  // Set fields
  name=MemHandleNew(StrLen(savedCT->name)+1);
  shortName=MemHandleNew(StrLen(savedCT->shortName)+1);

  buffer=(Char *)MemHandleLock(name);
  MemSet(buffer, MemPtrSize(buffer), 0);
  StrCopy(buffer, savedCT->name);
  MemHandleUnlock(name);

  buffer=(Char *)MemHandleLock(shortName);
  MemSet(buffer, MemPtrSize(buffer), 0);
  StrCopy(buffer, savedCT->shortName);
  MemHandleUnlock(shortName);


  old = FldGetTextHandle(fldName);
  FldSetTextHandle(fldName, name);
  if (old != NULL)    MemHandleFree(old); 

  old = FldGetTextHandle(fldShortName);
  FldSetTextHandle(fldShortName, shortName);
  if (old != NULL)    MemHandleFree(old); 

  clickedButton = FrmDoDialog(frm);

  if (clickedButton == BUTTON_ctdet_ok) {
    // Update entry in database
    Char *ptrName=FldGetTextPtr(fldName), *ptrShortName=FldGetTextPtr(fldShortName);

    if ( (ptrName == NULL) || (StrLen(ptrName) == 0) ||
         (ptrShortName == NULL) || (StrLen(ptrShortName) == 0) ) {
      FrmAlert(ALERT_ctdet_inv);
    } else {
      CourseTypeDBRecord ct;
      MemSet(&ct, sizeof(ct), 0);
      ct.type = savedCT->type;
      ct.id = savedCT->id;
      StrNCopy(ct.name, ptrName, sizeof(ct.name));
      StrNCopy(ct.shortName, ptrShortName, sizeof(ct.shortName));
      DmWrite(savedCT, 0, &ct, sizeof(CourseTypeDBRecord));
    }
  }


  MemHandleUnlock(m);

  FrmSetActiveForm(previousForm);
  FrmDeleteForm(frm);

}


/* LOCAL *********************************************************************
* Function: CourseTypeAdd
*
* Description: Add an course type
*****************************************************************************/
static void CourseTypeAdd(void) {
  FormType *frm, *previousForm;
  FieldType *fldName, *fldShortName;
  CourseTypeDBRecord *savedCT=NULL;
  UInt16 clickedButton;
  UInt16 index=0;

  previousForm = FrmGetActiveForm();
  frm = FrmInitForm(FORM_ct_details);
  FrmSetActiveForm(frm);
  fldName = GetObjectPtr(FIELD_ctdet_name);
  fldShortName = GetObjectPtr(FIELD_ctdet_short);

  clickedButton = FrmDoDialog(frm);

  if (clickedButton == BUTTON_ctdet_ok) {
    // Update entry in database
    Char *ptrName=FldGetTextPtr(fldName), *ptrShortName=FldGetTextPtr(fldShortName);

    if ( (ptrName == NULL) || (StrLen(ptrName) == 0) ||
         (ptrShortName == NULL) || (StrLen(ptrShortName) == 0) ) {
      FrmAlert(ALERT_ctdet_inv);
    } else {
      MemHandle newType;
      UInt16 newIndex=dmMaxRecordIndex;

      newType = DmNewRecord(DatabaseGetRefN(DB_DATA), &newIndex, sizeof(CourseTypeDBRecord));
      if (! newType) {
        // Could not create entry
        FrmAlert(ALERT_nomem);
      } else {

        CourseTypeDBRecord ct;
        UInt8 newID=0;
        MemHandle m=NULL;

        while(((m = DmQueryNextInCategory(DatabaseGetRefN(DB_DATA), &index, 0)) != NULL)) {
          savedCT = (CourseTypeDBRecord *)MemHandleLock(m);
          if ((savedCT->type == TYPE_CTYP) && (savedCT->id >= newID) )
            newID = savedCT->id;
          MemHandleUnlock(m);
          index += 1;
        }

        newID += 1;

        MemSet(&ct, sizeof(ct), 0);
        ct.type = TYPE_CTYP;
        ct.id = newID;
        StrNCopy(ct.name, ptrName, sizeof(ct.name));
        StrNCopy(ct.shortName, ptrShortName, sizeof(ct.shortName));
      
        DmWrite(MemHandleLock(newType), 0, &ct, sizeof(CourseTypeDBRecord));
        MemHandleUnlock(newType);
        DmReleaseRecord(DatabaseGetRefN(DB_DATA), newIndex, false);
      }
    }
  }


  FrmSetActiveForm(previousForm);
  FrmDeleteForm(frm);

}

/* LOCAL *********************************************************************
* Function: CourseTypeDelete
*
* Description: Delete a course type
*****************************************************************************/
static void CourseTypeDelete(UInt8 id) {
  UInt16 clickedButton;
  MemHandle m=NULL;
  UInt16 index=0;
  CourseTypeDBRecord *ct=NULL;

  // Look for index of entry
  if (! CourseTypeGetDBIndex(id, &index)) return;

  // Get MemHandle and lock it
  m = DmQueryRecord(DatabaseGetRefN(DB_DATA), index);
  ct = MemHandleLock(m);
    
  // We found the record and it is locked
  clickedButton = FrmCustomAlert(ALERT_ct_dodel, ct->name, ct->shortName, "");
  MemHandleUnlock(m);

  if (clickedButton == 0) {
    DmRemoveRecord(DatabaseGetRefN(DB_DATA), index);
  } // else do nothing
}





/*****************************************************************************
* Function: DrawCourseTypes
*
* Description: local function to fill the course types list
*****************************************************************************/
static void DrawCourseTypes(ListType *lst) {

  gNumCourseTypes=CourseTypesNum();
  gCourseTypeList = (Char **) MemPtrNew(gNumCourseTypes * sizeof(Char *));
  gCourseTypeID = (UInt8 *) MemPtrNew(gNumCourseTypes * sizeof(UInt8));

  CourseTypeFillList(gCourseTypeList, gCourseTypeID, gNumCourseTypes, 0, false);
  LstSetListChoices(lst, gCourseTypeList, gNumCourseTypes);
  LstSetSelection(lst, -1);

}


static void CleanupCourseTypeList(void) {
  if (gNumCourseTypes) {
    UInt16 i;
    for (i=0; i<gNumCourseTypes; i++)
       MemPtrFree((MemPtr) gCourseTypeList[i]);
    MemPtrFree((MemPtr) gCourseTypeList);
    gNumCourseTypes=0;
    MemPtrFree((MemPtr) gCourseTypeID);
  }
}



Boolean CourseTypeFormHandleEvent(EventPtr event) {
  FormPtr frm=FrmGetActiveForm();
  Boolean handled = false;
  ListType *lstP=GetObjectPtr(LIST_coursetypes);

  if (event->eType == ctlSelectEvent) {
    // button handling
    switch (event->data.ctlSelect.controlID) {
      case BUTTON_ct_back:
        handled=true;
        CleanupCourseTypeList();
        FrmReturnToForm(gCourseTypeCurrentForm);
        FrmUpdateForm(gCourseTypeCurrentForm, frmRedrawUpdateCode);
        break;

      case BUTTON_ct_add:
        handled=true;
        CourseTypeAdd();
        CleanupCourseTypeList();
        DrawCourseTypes(lstP);
        FrmDrawForm(frm);
        break;

      case BUTTON_ct_edit:
        handled=true;
        if (LstGetSelection(lstP) == noListSelection) {
          FrmAlert(ALERT_clist_noitem);
        } else {
          CourseTypeEdit(gCourseTypeID[LstGetSelection(lstP)]);
          CleanupCourseTypeList();
          DrawCourseTypes(lstP);
          FrmDrawForm(frm);
        }
        break;

      case BUTTON_ct_del:
        handled=true;
        if (LstGetSelection(lstP) == noListSelection) {
          FrmAlert(ALERT_clist_noitem);
        } else {
          CourseTypeDelete(gCourseTypeID[LstGetSelection(lstP)]);
          CleanupCourseTypeList();
          DrawCourseTypes(lstP);
          FrmDrawForm(frm);
        }
        break;


      default:
        break;
    }
  } else if (event->eType == menuOpenEvent) {
    return HandleMenuOpenEvent(event);
  } else if (event->eType == menuEvent) {
    // forwarding of menu events
    return HandleMenuEvent(event->data.menu.itemID);
  } else if (event->eType == frmOpenEvent) {
    // initializes and draws the form

    frm = FrmGetActiveForm();
    lstP=GetObjectPtr(LIST_coursetypes);
    DrawCourseTypes(lstP);
    FrmDrawForm (frm);

  } else if (event->eType == frmUpdateEvent) {
    // redraws the form if needed
    CleanupCourseTypeList();
    DrawCourseTypes(GetObjectPtr(LIST_coursetypes));
    FrmDrawForm (frm);
    handled = false;
  } else if (event->eType == frmCloseEvent) {
    // this is done if form is closed
    CleanupCourseTypeList();
  }

  return (handled);

}
