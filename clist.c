/* $Id: clist.c,v 1.1.1.1 2003/02/06 21:27:23 tim Exp $
 *
 * Course List functions
 * Created: 2002-07-11
 */

#include "UniMatrix.h"
#include "clist.h"
#include "database.h"
#include "gadget.h"
#include "delete.h"
#include "edit.h"
#include "beam.h"
#include "ctype.h"

UInt16 *gCourseInd, gNumCourses;
Char **gCourseList;

// "Shared global" globals
extern Char gCategoryName[dmCategoryLength];
extern UInt16 gMenuCurrentForm;

void CourseGetName(UInt16 courseID, MemHandle *charHandle) {
  Boolean found=false;
  MemHandle m;
  UInt16 index=0;

  while(! found && ((m = DmQueryNextInCategory(DatabaseGetRefN(DB_MAIN), &index, DatabaseGetCat())) != NULL)) {
    Char *s=(Char *)MemHandleLock(m);
    if (s[0] == TYPE_COURSE) {
      CourseDBRecord c;

      UnpackCourse(&c, s);

      if (c.id == courseID) {
        // Found it, put it into the string
        Char *tmp, *shortType;
        MemHandleResize(*charHandle, (StrLen(c.name)+CTYPE_SHORT_MAXLENGTH+CTYPE_ADD_MAXLENGTH+1));
        tmp=MemHandleLock(*charHandle);

        MemSet(tmp, MemPtrSize(tmp), 0);

        shortType =(Char *)MemPtrNew(CTYPE_SHORT_MAXLENGTH+1);
        MemSet(shortType, MemPtrSize(shortType), 0);
        CourseTypeGetShort(shortType, c.ctype);

        StrPrintF(tmp, "%s [%s]", c.name, shortType);
        MemPtrFree(shortType);
        MemHandleUnlock(*charHandle);
        found = true;
      }
    }
    MemHandleUnlock(m);
    index += 1;
  }
}


UInt8 CourseGetType(UInt16 courseID) {
  MemHandle m;
  UInt16 index=0;

  while((m = DmQueryNextInCategory(DatabaseGetRefN(DB_MAIN), &index, DatabaseGetCat())) != NULL) {
    Char *s=(Char *)MemHandleLock(m);
    if (s[0] == TYPE_COURSE) {
      CourseDBRecord c;

      UnpackCourse(&c, s);

      if (c.id == courseID) {
        // Found it, put it into the string
        MemHandleUnlock(m);
        return c.ctype;
      }
    }
    MemHandleUnlock(m);
    index += 1;
  }
return 0;
}

UInt16 CourseListGen(Char **itemList, UInt16 *courseID, UInt16 *courseInd, UInt16 numItems, UInt16 curInd, UInt8 searchFor) {
  UInt16 index=0, i=0, rv=0;
  MemHandle m;
  UInt16 *tmpCourseInd=NULL;

  if (courseInd == NULL)
    tmpCourseInd = (UInt16 *)MemPtrNew(numItems * sizeof(UInt16));

  while((i < numItems) && ((m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL)) {
    Char *s=(Char *)MemHandleLock(m);
    if (s[0] == TYPE_COURSE) {
      UInt16 j;
      CourseDBRecord c;
      Char *tempString, *shortType;

      UnpackCourse(&c, s);

      tempString=(Char *)MemPtrNew(StrLen(c.name)+3+StrLen(c.teacherName)+CTYPE_SHORT_MAXLENGTH+CTYPE_ADD_MAXLENGTH+1);
      shortType =(Char *)MemPtrNew(CTYPE_SHORT_MAXLENGTH+1);
      MemSet(shortType, MemPtrSize(shortType), 0);
      CourseTypeGetShort(shortType, c.ctype);
      // StrPrintF(shortType, "T");
      StrPrintF(tempString, "%s (%s) [%s]", c.name, c.teacherName, shortType);
      MemPtrFree((MemPtr)shortType);

      // Do a nice insertion sort here. DatAlg MUST have been good for something :-)
      j = i;
      while ( (j > 0) && (StrCompare(itemList[j-1], tempString) > 0) ) {
        itemList[j] = itemList[j-1];
        if (courseInd != NULL)  courseInd[j]=courseInd[j-1];
        else                    tmpCourseInd[j]=tmpCourseInd[j-1];
        if (courseID != NULL)  courseID[j]=courseID[j-1];
        j -= 1;
      }
 
      itemList[j]=tempString;
      if (courseInd != NULL)  courseInd[j]=index;
      else                    tmpCourseInd[j]=index;
      if (courseID != NULL)  courseID[j]=c.id;


      i += 1;
    }
    MemHandleUnlock(m);
    index += 1;
  }

  // We iterate through all to get the item to be selected
  // We could do that in the loop above, but we would need
  // a) to mess with insertion sort (make it slow)
  // b) to make the code ugly at two points
  // c) We don't care since we expect about 20 courses, that's nothing to iterate...
  // We implement the loop multiple times (once for each search type, last param)
  // so that we do not need a condition inside the loop for every run
  i = 0;

  if (searchFor == CLIST_SEARCH_INDEX) {
    while (! rv && (i < numItems)) {
      if (courseInd == NULL) {
        if (tmpCourseInd[i] == curInd)  rv = i;
      } else {
        if (courseInd[i] == curInd)  rv = i;
      }
      i += 1;
    }
  } else if ((searchFor == CLIST_SEARCH_ID) && (courseID != NULL)) {
    while (! rv && (i < numItems)) {
      if (courseID[i] == curInd)  rv = i;
      i += 1;
    }
  }

  if (tmpCourseInd != NULL)
    MemPtrFree((MemPtr)tmpCourseInd);

  return rv;
}





/*****************************************************************************
* Function: DrawCourses
*
* Description: local function to fill the course list
*****************************************************************************/
static void DrawCourses(ListType *lst) {
  MemHandle mWebsite, mEmail, old;
  Char *buffer;
  FieldType *fldWebsite, *fldEmail;
  
  gNumCourses=CountCourses();
  gCourseList = (Char **) MemPtrNew(gNumCourses * sizeof(Char *));
  gCourseInd = (UInt16 *) MemPtrNew(gNumCourses * sizeof(UInt16));

  CourseListGen(gCourseList, NULL, gCourseInd, gNumCourses, 0, CLIST_SEARCH_INDEX);
  LstSetListChoices(lst, gCourseList, gNumCourses);
  LstSetSelection(lst, -1);

  
  fldWebsite = GetObjectPtr(FIELD_cl_website);
  fldEmail = GetObjectPtr(FIELD_cl_email);
  
  mWebsite = MemHandleNew(4);
  buffer = MemHandleLock(mWebsite);
  MemSet(buffer, 4, 0);
  StrCopy(buffer, "-?-");
  MemHandleUnlock(mWebsite);

  old = FldGetTextHandle(fldWebsite);
  FldSetTextHandle(fldWebsite, mWebsite);
  if (old != NULL)  MemHandleFree(old); 

  mEmail = MemHandleNew(4);
  buffer = MemHandleLock(mEmail);
  MemSet(buffer, 4, 0);
  StrCopy(buffer, "-?-");
  MemHandleUnlock(mEmail);

  old = FldGetTextHandle(fldEmail);
  FldSetTextHandle(fldEmail, mEmail);
  if (old != NULL)  MemHandleFree(old); 

  
}


static void CleanupCourselist(void) {
  if (gNumCourses) {
    UInt16 i;
    for (i=0; i<gNumCourses; i++)
       MemPtrFree((MemPtr) gCourseList[i]);
    MemPtrFree((MemPtr) gCourseList);
    gNumCourses=0;
    MemPtrFree((MemPtr) gCourseInd);
  }
}

static Boolean CourseListHandleSelection(void) {
  MemHandle m, mWebsite, mEmail, old;
  CourseDBRecord c;
  FieldType *fldWebsite, *fldEmail;
  Char *buffer;
  
  m = DmQueryRecord(DatabaseGetRefN(DB_MAIN), gCourseInd[LstGetSelection(GetObjectPtr(LIST_courses))]);
  if (! m)  return true;
  UnpackCourse(&c, MemHandleLock(m));
  fldWebsite = GetObjectPtr(FIELD_cl_website);
  fldEmail = GetObjectPtr(FIELD_cl_email);

  if (StrLen(c.website) == 0) {
    mWebsite = MemHandleNew(4);
    buffer=MemHandleLock(mWebsite);
    MemSet(buffer, 4, 0);
    StrCopy(buffer, "-?-");
  } else {
    mWebsite = MemHandleNew(StrLen(c.website)+1);
    buffer = MemHandleLock(mWebsite);
    MemSet(buffer, StrLen(c.website)+1, 0);
    StrCopy(buffer, c.website);
  }
  MemHandleUnlock(mWebsite);
  old = FldGetTextHandle(fldWebsite);
  FldSetTextHandle(fldWebsite, mWebsite);
  if (old != NULL)  MemHandleFree(old); 
  FldDrawField(fldWebsite);

  if (StrLen(c.teacherEmail) == 0) {
    mEmail = MemHandleNew(4);
    buffer = MemHandleLock(mEmail);
    MemSet(buffer, 4, 0);
    StrCopy(buffer, "-?-");
  } else {
    mEmail = MemHandleNew(StrLen(c.teacherEmail)+1);
    buffer = MemHandleLock(mEmail);
    MemSet(buffer, StrLen(c.teacherEmail)+1, 0);
    StrCopy(buffer, c.teacherEmail);
  }
  MemHandleUnlock(mEmail);
  old = FldGetTextHandle(fldEmail);
  FldSetTextHandle(fldEmail, mEmail);
  if (old != NULL)  MemHandleFree(old); 
  FldDrawField(fldEmail);

  MemHandleUnlock(m);

  return false;
}


Boolean CourseListHandleEvent(EventPtr event) {
  FormPtr frm=FrmGetActiveForm();
  Boolean handled = false;
  ListType *lstP=GetObjectPtr(LIST_courses);
  Boolean categoryEdited, reDraw=false;
  UInt16 category, numRecords;
  ControlType *ctl;
  UInt32 *recordList;

  if (event->eType == ctlSelectEvent) {
    // button handling
    switch (event->data.ctlSelect.controlID) {
      case BUTTON_courselist_back:
        handled=true;
        FrmGotoForm(FORM_main);
        break;

      case BUTTON_courselist_del:
        handled=true;
        if (LstGetSelection(lstP) == noListSelection) {
          FrmAlert(ALERT_clist_noitem);
        } else {
          DeleteCourse(gCourseInd[LstGetSelection(lstP)]);
          CleanupCourselist();
          DrawCourses(lstP);
          FrmDrawForm(FrmGetActiveForm());
        }
        break;

      case BUTTON_courselist_add:
        handled=true;
        gMenuCurrentForm=FORM_courselist;
        AddCourse();
        break;

      case BUTTON_courselist_edit:
        handled=true;
        if (LstGetSelection(lstP) == noListSelection) {
          FrmAlert(ALERT_clist_noitem);
        } else {
          gMenuCurrentForm=FORM_courselist;
          EditCourse(gCourseInd[LstGetSelection(lstP)]);
        }
        break;

      case BUTTON_courselist_beam:
        handled=true;
        if (LstGetSelection(lstP) == noListSelection) {
          FrmAlert(ALERT_clist_noitem);
        } else {
          BeamCourse(gCourseInd[LstGetSelection(lstP)]);
        }
        break;


      case LIST_cl_cat_trigger:
        handled=true;
        category=DatabaseGetCat();
        numRecords=DmNumRecordsInCategory(DatabaseGetRef(), DELETE_CATEGORY);
        recordList=(UInt32 *)MemPtrNew(numRecords * sizeof(UInt32));
        CatPreEdit(numRecords, recordList);
        categoryEdited = CategorySelect(DatabaseGetRef(), frm,
                                        LIST_cl_cat_trigger, LIST_cl_cat, false,
                                        &category, gCategoryName, 0,
                                        STRING_cat_edit); // categoryDefaultEditCategoryString
        if (categoryEdited || (category != DatabaseGetCat())) {
          reDraw=true;
          DatabaseSetCat(category);
          ctl = GetObjectPtr(LIST_cl_cat_trigger);
          CategoryGetName(DatabaseGetRef(), category, gCategoryName); 
          CategorySetTriggerLabel(ctl, gCategoryName);
        }
        CatPostEdit(numRecords, recordList);
        if (reDraw) {
          CleanupCourselist();
          DrawCourses(lstP);
          FrmDrawForm(FrmGetActiveForm());
        }
        if (recordList != NULL)  MemPtrFree((MemPtr)recordList);
        break;



      default:
        break;
    }
  } else if (event->eType == keyDownEvent) {
    // We have a hard button assigned and it was pressed
    if (TxtCharIsHardKey(event->data.keyDown.modifiers, event->data.keyDown.chr)) {
      if (! (event->data.keyDown.modifiers & poweredOnKeyMask)) {
        FrmGotoForm(FORM_main);
        handled = true;
      }
    } else if (EvtKeydownIsVirtual(event)) {
      // Up or down keys pressed
      ListType *lst=GetObjectPtr(LIST_courses);
      switch (event->data.keyDown.chr) {
        case vchrPageUp:
          if (LstGetSelection(lst) == noListSelection) {
            LstSetSelection(lst, gNumCourses-1);
            CourseListHandleSelection();
          } else if (LstGetSelection(lst) > 0) {
            LstSetSelection(lst, LstGetSelection(lst)-1);
            CourseListHandleSelection();
          }
          handled=true;
          break;
        
        case vchrPageDown:
          if (LstGetSelection(lst) == noListSelection) {
            LstSetSelection(lst, 0);
            CourseListHandleSelection();
          } else if (LstGetSelection(lst) < (gNumCourses-1)) {
            LstSetSelection(lst, LstGetSelection(lst)+1);
            CourseListHandleSelection();
          }
          handled=true;
          break;

        default:
          break;
      }
    }
  } else if (event->eType == lstSelectEvent) {
    return CourseListHandleSelection();
  } else if (event->eType == menuOpenEvent) {
    return HandleMenuOpenEvent(event);
  } else if (event->eType == menuEvent) {
    // forwarding of menu events
    return HandleMenuEvent(event->data.menu.itemID);
  } else if (event->eType == frmOpenEvent) {
    // initializes and draws the form
    ControlType *ctl;

    frm = FrmGetActiveForm();
    lstP=GetObjectPtr(LIST_courses);
    FrmDrawForm (frm);
    DrawCourses(lstP);
    FrmDrawForm (frm);

    ctl = GetObjectPtr(LIST_cl_cat_trigger);
    CategoryGetName (DatabaseGetRef(), DatabaseGetCat(), gCategoryName); 
    CategorySetTriggerLabel (ctl, gCategoryName); 

    WinDrawLine(1, 140, 158, 140);
    
    handled = true;
  } else if (event->eType == frmUpdateEvent) {
    // redraws the form if needed
    CleanupCourselist();
    DrawCourses(GetObjectPtr(LIST_courses));
    FrmDrawForm (frm);
    WinDrawLine(1, 140, 158, 140);
    handled = false;
  } else if (event->eType == frmCloseEvent) {
    // this is done if form is closed
    CleanupCourselist();
  }

  return (handled);

}
