/* $Id: beam.c,v 1.1 2003/02/06 21:27:23 tim Exp $
 *
 * Beam functions
 * Created: 2002-05-02
 */

#include "UniMatrix.h"
#include "beam.h"
#include "database.h"
#include "ctype.h"


#ifdef BEAM_DEBUG
#warning Beam DEBUG is ON
#endif

/***********************************************************************
 * Send Functions
 ***********************************************************************/


static Err BeamBytes(ExgSocketPtr s, void *buffer, UInt32 bytesToSend) {
  Err err=0;

  while (!err && (bytesToSend >0)) {
    UInt32 bytesSent = ExgSend(s, buffer, bytesToSend, &err);
    bytesToSend -= bytesSent;
    buffer = ((Char *)buffer) + bytesSent;
  }

return err;
}


static void BeamCourseByIndex(ExgSocketType *s, UInt16 courseIndex) {
  MemHandle mc, m;
  MemPtr pc;
  UInt16 numBytes=0, courseID=0, index=0;
  CourseDBRecord c;
  Err err=errNone;

  mc = DmQueryRecord(DatabaseGetRef(), courseIndex);
  pc = MemHandleLock(mc);
  UnpackCourse(&c, pc);
  courseID=c.id;
  
  
  // Look for index of course type
  if (CourseTypeGetDBIndex(c.ctype, &index)) {
    // Get MemHandle and lock it
    MemHandle m = DmQueryRecord(DatabaseGetRefN(DB_DATA), index);
    // Send Course Type
    numBytes=MemHandleSize(m);
    BeamBytes(s, &numBytes, sizeof(numBytes));
    BeamBytes(s, MemHandleLock(m), numBytes);
    MemHandleUnlock(m);
  }


  // Send Course
  numBytes=MemHandleSize(mc);
  BeamBytes(s, &numBytes, sizeof(numBytes));
  BeamBytes(s, pc, numBytes);
  MemHandleUnlock(mc);

  while(!err && ((m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL)) {
    Char *b=MemHandleLock(m);
    if (b[0] == TYPE_TIME) {
      TimeDBRecord *t=(TimeDBRecord *)b;
      if (t->course == courseID) {
        numBytes=MemHandleSize(m);
        err = BeamBytes(s, &numBytes, sizeof(numBytes));
        err = BeamBytes(s, b, numBytes);
      }
    } else if (b[0] == TYPE_EXAM) {
      ExamDBRecord *e=(ExamDBRecord *)b;
      if (e->course == courseID) {
        numBytes=MemHandleSize(m);
        err = BeamBytes(s, &numBytes, sizeof(numBytes));
        err = BeamBytes(s, b, numBytes);
      }
    }
    MemHandleUnlock(m);
    index += 1;
  }
}

static Err BeamInit(ExgSocketType *s, Char *description) {
  MemSet(s, sizeof(ExgSocketType), 0);
  s->description=description;
  s->target=APP_CREATOR;
  // s.localMode=1;

  return ExgPut(s);
}


static Err BeamFinish(ExgSocketType *s) {
  Err err=errNone;
  return ExgDisconnect(s, err);
}


void BeamCourse(UInt16 courseIndex) {
  // We do not care about secret records, there shouldn't be any...
  Char *courseName;
  CourseDBRecord c;
  MemHandle mc;
  ExgSocketType s;
  Err err;
  UInt8 beamType=BEAM_COURSE;

  mc = DmQueryRecord(DatabaseGetRef(), courseIndex);
  UnpackCourse(&c, MemHandleLock(mc));

  courseName=MemPtrNew(StrLen(c.name)+1);
  MemSet(courseName, MemPtrSize(courseName), 0);
  StrNCopy(courseName, c.name, StrLen(c.name));
  MemHandleUnlock(mc);
  
  err = BeamInit(&s, courseName);
  if (! err) {
    err = BeamBytes(&s, &beamType, sizeof(beamType));
    BeamCourseByIndex(&s, courseIndex);
  }
  err = BeamFinish(&s);
  MemPtrFree((MemPtr) courseName);
}



void BeamSemester(UInt16 category) {
  ExgSocketType s;
  Err err;
  UInt8 beamType=BEAM_SEMESTER;
  Char categoryName[dmCategoryLength]="";

  CategoryGetName(DatabaseGetRefN(DB_MAIN), category, categoryName); 

  err = BeamInit(&s, categoryName);
  if (! err) {
    UInt16 index=0;
    MemHandle m;
    err = BeamBytes(&s, &beamType, sizeof(beamType));
    while ((m = DmQueryNextInCategory(DatabaseGetRef(), &index, category)) != NULL) {
      Char *st=MemHandleLock(m);
      UInt8 entryType=(UInt8)st[0];
      MemHandleUnlock(m);
      if (entryType == TYPE_COURSE) {
        BeamCourseByIndex(&s, index);
      }
      index += 1;
    }
  }
  err = BeamFinish(&s);

}



/***********************************************************************
 * Receive Functions
 ***********************************************************************/
static Err BeamReadRecordIntoDB(DmOpenRef cats, DmOpenRef dogs, ExgSocketPtr socketPtr, UInt32 numBytes,
                                UInt16 category, UInt16 *courseID, UInt8 *courseType) {
  char *buffer=NULL;
  Err  err=0;
  UInt32 bytesReceived=0, numBytesToRead=numBytes;
  void *curpos=NULL;
  Boolean allocatedRec = false, doAlloc=true, doAllocInDogs=false;
  MemHandle recHandle=NULL;
  Char *recPtr=NULL;
  Char *t=MemPtrNew(numBytes);
  UInt16 index = dmMaxRecordIndex, searchIndex=0;
  MemHandle m;
                              
  buffer=(char *)MemPtrNew(numBytesToRead);
  curpos=t;

  // Receive the record
  do {
    bytesReceived = ExgReceive(socketPtr, buffer, numBytesToRead, &err);
    numBytes -= bytesReceived;

    MemMove(curpos, buffer, bytesReceived);
    curpos += bytesReceived;
  } while (!err && (bytesReceived > 0) && (numBytes > 0));

  MemPtrFree((MemPtr) buffer);

  // Pre handling for record to prevent duplicates from happen
  if (t[0] == TYPE_CTYP) {
    // It's a course type. check if that already exists
    CourseTypeDBRecord *ct=(CourseTypeDBRecord *)t;
    UInt8 newID=0;
    while ( doAlloc && ((m = DmQueryNextInCategory(dogs, &searchIndex, 0)) != NULL)) {
      Char *b=MemHandleLock(m);
      if (b[0] == TYPE_CTYP) {
        CourseTypeDBRecord *tct = (CourseTypeDBRecord *)b;
        if (tct->id > newID)  newID=tct->id;
        if (StrCompare(ct->name, tct->name) == 0) {
          // Strings match, we already have such a course type
          *courseType=tct->id;
          doAlloc=false;
        }
      }
      searchIndex += 1;
      MemHandleUnlock(m);
    }
    newID += 1;
    if (doAlloc)  *courseType = newID;
    doAllocInDogs=true;
  } else if (t[0] == TYPE_COURSE) {
    // It's a course, give it a new course ID
    PackedCourseDBRecord *pc=(PackedCourseDBRecord *)t;

    pc->id = DatabaseGetNewCID(cats, category);
    pc->ctype = *courseType;
    *courseID=pc->id;

    // Test, if we have already a course named like the wanted course
    // name is first field so we can just access it and do not need to unpack the course
    while(doAlloc && ((m = DmQueryNextInCategory(cats, &searchIndex, category)) != NULL)) {
      Char *b=MemHandleLock(m);
      if (b[0] == TYPE_COURSE) {
        PackedCourseDBRecord *tpc = (PackedCourseDBRecord *)b;
        if ((StrCompare(tpc->name, pc->name) == 0) && (tpc->ctype == pc->ctype)) {
          // Strings match, we already have such a course
          *courseID=tpc->id;
          doAlloc=false;
        }
      }
      searchIndex += 1;
      MemHandleUnlock(m);
    }
  } else if (t[0] == TYPE_TIME) {
    TimeDBRecord *tr = (TimeDBRecord *)t;
    tr->course = *courseID;
    // Check if this course already exists
    while(doAlloc && ((m = DmQueryNextInCategory(cats, &searchIndex, category)) != NULL)) {
      Char *b=MemHandleLock(m);
      if (b[0] == TYPE_TIME) {
        TimeDBRecord *tt = (TimeDBRecord *)b;
        if ( (tt->course == *courseID) && (tr->day == tt->day) &&
             (TimeToInt(tr->begin) == TimeToInt(tt->begin)) && (TimeToInt(tr->end) == TimeToInt(tt->end)) ) {
          // Times match
          doAlloc=false;
        }
      }
      searchIndex += 1;
      MemHandleUnlock(m);
    }
  } else if (t[0] == TYPE_EXAM) {
    ExamDBRecord *ex = (ExamDBRecord *)t;
    ex->course = *courseID;
    // Check if this exam already exists
    while(doAlloc && ((m = DmQueryNextInCategory(cats, &searchIndex, category)) != NULL)) {
      Char *b=MemHandleLock(m);
      if (b[0] == TYPE_EXAM) {
        ExamDBRecord *te = (ExamDBRecord *)b;
        if ( (te->course == *courseID) && (DateToDays(te->date) == DateToDays(ex->date)) ) {
          // Exams match
          doAlloc=false;
        }
      }
      searchIndex += 1;
      MemHandleUnlock(m);
    }
  }

  // Store record
  if (doAlloc) {
    if (! doAllocInDogs) {
      // Store record in CATS database
      recHandle = DmNewRecord(cats, &index, numBytesToRead);
      if (!recHandle) {
        err =DmGetLastErr();
        return err;
      }
  
      allocatedRec = true;
      recPtr=MemHandleLock(recHandle);
      err = DmWrite(recPtr, 0, t, numBytesToRead);
      MemHandleUnlock(recHandle);
      MemPtrFree((MemPtr) t);
  
      if (recHandle) {
        UInt16 attr;
        DmReleaseRecord(cats, index, false);
        DmRecordInfo(cats, index, &attr, NULL, NULL);
        attr |= category;
        DmSetRecordInfo(cats, index, &attr, NULL);
      }
      if (err && allocatedRec) DmRemoveRecord(cats, index);
  
      DatabaseSortByDBRef(cats);
    } else {
      // Store in DOGS database
      recHandle = DmNewRecord(dogs, &index, numBytesToRead);
      if (!recHandle) {
        err =DmGetLastErr();
        return err;
      }
  
      allocatedRec = true;
      recPtr=MemHandleLock(recHandle);
      err = DmWrite(recPtr, 0, t, numBytesToRead);
      MemHandleUnlock(recHandle);
      MemPtrFree((MemPtr) t);
  
      if (err && allocatedRec) DmRemoveRecord(dogs, index);
  
      DatabaseSortByDBRef(dogs);
    }
  }

  return err;
}

void BeamCourseByCID(UInt16 cid) {
  MemHandle m;
  Boolean found=false;
  UInt16 index=0, foundIndex=0;
  Char *s;
  while (!found && ((m = DmQueryNextInCategory(DatabaseGetRef(), &index, DatabaseGetCat())) != NULL)) {
    s = MemHandleLock(m);
    if (s[0] == TYPE_COURSE) {
      CourseDBRecord *c=(CourseDBRecord *)s;
      if (c->id == cid) {
        foundIndex=index;
        found=true;
      }
    }
    index += 1;
  }

  if (found)  BeamCourse(foundIndex);
}


Err BeamReceive(DmOpenRef cats, DmOpenRef dogs, ExgSocketPtr socketPtr) {
  Err err=0;
  UInt8 beamType=0, courseType=0;
  UInt16 numBytes=0, courseID=0;
  UInt16 category=0;
  Boolean doReceive=false;

  // I really like those variable names :-)
  if (cats && dogs) {
    err = ExgAccept(socketPtr);
    if (!err) {
      ExgReceive(socketPtr, &beamType, sizeof(beamType), &err);

      if (beamType == BEAM_COURSE) {
        FormType *previousForm=FrmGetActiveForm();
        FormType *frm=FrmInitForm(FORM_beam_cat);
        ListType *lst;
        ControlType *ctl;
        Char categoryName[dmCategoryLength]="";

        FrmSetActiveForm(frm);
        lst=GetObjectPtr(LIST_bc_cat);
        ctl=GetObjectPtr(LIST_bc_cat_trigger);
        CategoryGetName(cats, 0, categoryName); 
        CtlSetLabel(ctl, categoryName);
        CategoryCreateList(cats, lst, 0, false, true, 0, categoryHideEditCategory, true);
        LstSetSelection(lst, 0);
        if (FrmDoDialog(frm) == BUTTON_beam_cat_ok) {
          category=CategoryFind(cats, CtlGetLabel(ctl));
          CategoryFreeList(cats, lst, false, categoryHideEditCategory);
          doReceive=true;
        }

        if (previousForm)  FrmSetActiveForm(previousForm);
        FrmDeleteForm(frm);

      } else if (beamType == BEAM_SEMESTER) {
        Boolean ok=false;
        FormType *previousForm=FrmGetActiveForm();
        FormType *frm=FrmInitForm(FORM_beam_newcat);
        

        FrmSetActiveForm(frm);

        while (! ok) {
          FieldType *fName;
          Char *name;
          MemHandle mName, old;

          fName = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FIELD_bs_cat));

          // Gen MemHandle
          mName=MemHandleNew(StrLen(socketPtr->description)+1);
          name = (Char *)MemHandleLock(mName);
          MemSet(name, MemHandleSize(mName), 0);
          // Copy contents to the memory handle
          StrNCopy(name, socketPtr->description, StrLen(socketPtr->description));
          MemHandleUnlock(mName);
          // Load fields
          old = FldGetTextHandle(fName);
          FldSetTextHandle(fName, mName);
          if (old != NULL)    MemHandleFree(old); 

          if (FrmDoDialog(frm) == BUTTON_bs_ok) {

            doReceive=true;

            name = FldGetTextPtr(fName);
            if ( (name != NULL) && (StrLen(name) > 0) ) {
              // Try to create a new category
              UniAppInfoType *appInfo = (UniAppInfoType *)MemLocalIDToLockedPtr(DmGetAppInfoID(cats), DATABASE_CARD);
              UInt16 i=0;
              UInt16 newCatID=0xFFFF;
              Boolean notFound=true;
              ok = true;
              for (i=0; notFound && (i < dmRecNumCategories); ++i) {
                if (StrLen(appInfo->appInfo.categoryLabels[i]) == 0) {
                  newCatID=i;
                  notFound=false;
                }
              }
  
              MemPtrUnlock((MemPtr)appInfo);
  
              if (newCatID > dmRecNumCategories) {
                // Damn, no more category space left, discard transmission
                FrmAlert(ALERT_beamnocatleft);
                if (previousForm)  FrmSetActiveForm(previousForm);
                FrmDeleteForm(frm);
                return err;
              } else {
                // Fine, found one, use it
                category = newCatID;
                CategorySetName(cats, category, name);
              }
              PrefSetAppPreferences(APP_CREATOR, PREFS_CURCAT, PREFS_VERSION, &category, sizeof(category), false);
            }
          }
        }

        if (previousForm)  FrmSetActiveForm(previousForm);
        FrmDeleteForm(frm);

      }


      if (doReceive) {
        while (!err && (ExgReceive(socketPtr, &numBytes, sizeof(numBytes), &err) > 0)) {
          err = BeamReadRecordIntoDB(cats, dogs, socketPtr, numBytes, category, &courseID, &courseType);
        }
      }
    }
  }

return err;
}
