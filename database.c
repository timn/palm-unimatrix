/* $Id: database.c,v 1.5 2003/04/29 23:03:48 tim Exp $
 *
 * Database handling, another central piece in UniMatrix
 */

#include "database.h"
#include "ctype.h"
#include "cache.h"
#include "alarm.h"
#include "prefs.h"

DmOpenRef gDatabase[DATABASE_NUM]={NULL, NULL};
UInt16    gCategory=0;
extern    UniMatrixPrefs gPrefs;

/*****************************************************************************
* Function:  OpenDatabase
*
* Description:  Opens/creates the application's database.
*****************************************************************************/
Err OpenDatabase(void) {
	Err err = errNone;
  UInt16 version;

  if (! gDatabase[DB_MAIN]) {
    gDatabase[DB_MAIN] = DmOpenDatabaseByTypeCreator(DATABASE_TYPE, APP_CREATOR, dmModeReadWrite);
    if (!gDatabase[DB_MAIN])  {
      LocalID dbID;
      err = CreateDatabase(DATABASE_NAME, &dbID);
      if (err == errNone)  gDatabase[DB_MAIN] = DmOpenDatabaseByTypeCreator(DATABASE_TYPE, APP_CREATOR, dmModeReadWrite);

    }
  }

  if (! gDatabase[DB_DATA]) {
    gDatabase[DB_DATA] = DmOpenDatabaseByTypeCreator(DATABASE_DATA_TYPE, APP_CREATOR, dmModeReadWrite);
    if (!gDatabase[DB_DATA]) {
      err = DmCreateDatabase(DATABASE_CARD, DATABASE_DATA_NAME, APP_CREATOR, DATABASE_DATA_TYPE, false);
      if (err == errNone)  gDatabase[DB_DATA] = DmOpenDatabaseByTypeCreator(DATABASE_DATA_TYPE, APP_CREATOR, dmModeReadWrite);
      if (gDatabase[DB_DATA]) {
        UInt8 i;
        for (i=0; i < CTYPE_DEF_NUM; ++i) {
          CourseTypeDBRecord ct;
          MemHandle m;
          UInt16 newIndex=dmMaxRecordIndex;

          MemSet(&ct, sizeof(ct), 0);
          ct.type = TYPE_CTYP;
          ct.id = i;

          m = DmGetResource(strRsc, CTYPE_DEF_START+i);
          StrCopy(ct.name, MemHandleLock(m));
          MemHandleUnlock(m);

          m = DmGetResource(strRsc, CTYPE_DEF_SH_START+i);
          StrCopy(ct.shortName, MemHandleLock(m));
          MemHandleUnlock(m);

          m = DmNewRecord(gDatabase[DB_DATA], &newIndex, sizeof(CourseTypeDBRecord));
          DmWrite(MemHandleLock(m), 0, &ct, sizeof(CourseTypeDBRecord));
          MemHandleUnlock(m);
          DmReleaseRecord(gDatabase[DB_DATA], newIndex, false);
        }
      }
    }
  }

  if (gDatabase[DB_MAIN]) {
    UniAppInfoType *appInfo;

    /* DEBUG code, needed after editing pdb in hex editor
    LocalID id;
    UInt32 time=TimGetSeconds();
    DmOpenDatabaseInfo(gDatabase[DB_MAIN], &id, NULL, NULL, NULL, NULL);
    DmSetDatabaseInfo(0, id, NULL, NULL, NULL, &time, &time, NULL, NULL, NULL, NULL, NULL, NULL);
    */
    appInfo = (UniAppInfoType *)MemLocalIDToLockedPtr(DmGetAppInfoID(gDatabase[DB_MAIN]), DATABASE_CARD);
    version = appInfo->version;
    MemPtrUnlock(appInfo);
    if (version < DATABASE_VERSION) {
      if ((err = DatabaseConvert(version)) != errNone)  return err;
    }
    DatabaseSort();
  }

  return err;
} // OpenDatabase




/*****************************************************************************
* Function:  CloseDatabase
*
* Description:  Closes the application's database.
*****************************************************************************/
void CloseDatabase(void) {
	if (gDatabase[DB_MAIN])  DmCloseDatabase(gDatabase[DB_MAIN]);
  gDatabase[DB_MAIN] = NULL;
  if (gDatabase[DB_DATA])  DmCloseDatabase(gDatabase[DB_DATA]);
  gDatabase[DB_DATA] = NULL;
}

/*****************************************************************************
* Function:  DatabaseGetRef
*
* Description: Returns the database pointer
*****************************************************************************/
DmOpenRef DatabaseGetRef(void) {
  return gDatabase[DB_MAIN];
}

/*****************************************************************************
* Function:  DatabaseGetRefN
*
* Description: Returns the database pointer from array
*****************************************************************************/
DmOpenRef DatabaseGetRefN(UInt8 num) {
  // We do not check to improbe performance, since this is NEVER a user give
  // value this just means that the programmer has to be careful, that should
  // not be a problem with nicely used constants
  // if (num >= DATABASE_NUM) return gDatabase[DB_MAIN];
  return gDatabase[num];
}


/*****************************************************************************
* Function:  DeleteDatabase
*
* Description: Deletes the database
*****************************************************************************/
void DeleteDatabase(void) {
  LocalID dbID;

  dbID = DmFindDatabase(DATABASE_CARD, DATABASE_NAME);

  if (dbID != 0) {
    // Database does exist, we do not care about result.
    DmDeleteDatabase(DATABASE_CARD, dbID);
  }  

}

/*****************************************************************************
* Function: DatabaseConvert
*
* Description: Converts databases from older version to current version
*****************************************************************************/
Err DatabaseConvert(UInt16 oldVersion) {
  DmOpenRef old, new;
  LocalID newID, oldID;
  UInt16 numRecords, i, atP, DBversion=DATABASE_VERSION, attr, cat;
  UInt8 ctype;
  MemHandle oh, nh;
  Char *s, newName[32]=DATABASE_NAME;
  MemPtr p;
  UniAppInfoType *appInfoOld, *appInfoNew;

  if (! gDatabase[DB_MAIN])       return dmErrNoOpenDatabase;
  if (FrmAlert(ALERT_oldDB) != 0) return dmErrCorruptDatabase;


  old = gDatabase[DB_MAIN];
  DmOpenDatabaseInfo(old, &oldID, NULL, NULL, NULL, NULL);
  appInfoOld = (UniAppInfoType *)MemLocalIDToLockedPtr(DmGetAppInfoID(old), DATABASE_CARD);

  CreateDatabase(DATABASE_TEMPNAME, &newID);
  new = DmOpenDatabase(DATABASE_CARD, newID, dmModeReadWrite);
  appInfoNew = (UniAppInfoType *)MemLocalIDToLockedPtr(DmGetAppInfoID(new), DATABASE_CARD);

  DmWrite(appInfoNew, 0, appInfoOld, sizeof(UniAppInfoType));
  DmWrite(appInfoNew, offsetof(UniAppInfoType, version), &DBversion, sizeof(DBversion));
  MemPtrUnlock(appInfoOld);
  MemPtrUnlock(appInfoNew);

  switch (oldVersion) {
    case 0:
    case 1:
      // We have a version one database, type to be converted is TimeDBRecord_v1

      numRecords=DmNumRecords(old);
      for (i=0; i < numRecords; ++i) {
        atP=i;
        oh = DmQueryRecord(old, i);

        DmRecordInfo(old, i, &attr, NULL, NULL);
        cat = attr & dmRecAttrCategoryMask;

        s = (Char *)MemHandleLock(oh);
        if (s[0] == TYPE_COURSE) {
          // We have to change the course type
          nh = DmNewRecord(new, &atP, MemPtrSize(s));
          if (nh) {
            p = MemHandleLock(nh);
            DmWrite(p, 0, s, MemPtrSize(s));
            s = (Char *)p;
            ctype = s[offsetof(CourseDBRecord, ctype)];
            // We added a course type...
            if (ctype > 1)  ctype += 1;              
            DmWrite(p, offsetof(CourseDBRecord, ctype), &ctype, sizeof(ctype));
            MemHandleUnlock(nh);
          }
          DmReleaseRecord(new, atP, false);
        } else {
          // Convert...
          TimeDBRecord_v1 *ot;
          TimeDBRecord *nt;

          nh = DmNewRecord(new, &atP, sizeof(TimeDBRecord));
          if (nh) {
            ot = (TimeDBRecord_v1 *)s;
            p = (TimeDBRecord *)MemHandleLock(nh);
            nt = (TimeDBRecord *)MemPtrNew(sizeof(TimeDBRecord));
            MemSet(nt, sizeof(TimeDBRecord), 0);
            nt->type = ot->type;
            nt->day = ot->day;
            nt->course = ot->course;
            nt->begin.hours = (UInt8)(ot->begin / 4) + 8;
            nt->begin.minutes = (ot->begin % 4) * 15;
            nt->end.hours = (UInt8)(ot->end / 4) + 8;
            nt->end.minutes = (ot->end % 4) * 15;
            nt->color[0] = ot->color[0];
            nt->color[1] = ot->color[1];
            nt->color[2] = ot->color[2];
            StrNCopy(nt->room, ot->room, 8);
            DmWrite(p, 0, nt, sizeof(TimeDBRecord));

            MemPtrFree((MemPtr)nt);
            MemHandleUnlock(nh);
          }
          DmReleaseRecord(new, atP, false);
        }


        DmRecordInfo(new, atP, &attr, NULL, NULL);
        attr |= cat;
        DmSetRecordInfo(new, atP, &attr, NULL);

        MemHandleUnlock(oh);
      }      

        DmCloseDatabase(old);
        DmInsertionSort(new, DatabaseCompare, 0);
        gDatabase[DB_MAIN]=new;

        DmDeleteDatabase(DATABASE_CARD, oldID);
        DmSetDatabaseInfo(DATABASE_CARD, newID, newName, NULL, NULL, NULL, NULL, 
                          NULL, NULL, NULL, NULL, NULL, NULL);

        appInfoOld = (UniAppInfoType *)MemLocalIDToLockedPtr(DmGetAppInfoID(old), DATABASE_CARD);
        oldID = newID;

        CreateDatabase(DATABASE_TEMPNAME, &newID);
        new = DmOpenDatabase(DATABASE_CARD, newID, dmModeReadWrite);
        appInfoNew = (UniAppInfoType *)MemLocalIDToLockedPtr(DmGetAppInfoID(new), DATABASE_CARD);

        DmWrite(appInfoNew, 0, appInfoOld, sizeof(UniAppInfoType));
        DmWrite(appInfoNew, offsetof(UniAppInfoType, version), &DBversion, sizeof(DBversion));
        MemPtrUnlock(appInfoOld);
        MemPtrUnlock(appInfoNew);
      
    case 2:   DmInsertionSort(old, DatabaseCompare, 0);

    case 3:
      // This introduced notes, need to set all not IDs for exams and events to 0

      numRecords=DmNumRecords(old);
      for (i=0; i < numRecords; ++i) {
        atP=i;
        oh = DmQueryRecord(old, i);

        DmRecordInfo(old, i, &attr, NULL, NULL);
        cat = attr & dmRecAttrCategoryMask;

        s = (Char *)MemHandleLock(oh);
        if (s[0] == TYPE_TIME) {
          // Convert...
          TimeDBRecord_v3 *ot;
          TimeDBRecord    *nt;

          nh = DmNewRecord(new, &atP, sizeof(TimeDBRecord));
          if (nh) {
            ot = (TimeDBRecord_v3 *)s;
            p = (TimeDBRecord *)MemHandleLock(nh);
            nt = (TimeDBRecord *)MemPtrNew(sizeof(TimeDBRecord));
            MemSet(nt, sizeof(TimeDBRecord), 0);
            nt->type = ot->type;
            nt->day = ot->day;
            nt->course = ot->course;
            nt->note = 0; // This must be changed for higher version convert
            nt->begin.hours = ot->begin.hours;
            nt->begin.minutes = ot->begin.minutes;
            nt->end.hours = ot->end.hours;
            nt->end.minutes = ot->end.minutes;
            nt->color[0] = ot->color[0];
            nt->color[1] = ot->color[1];
            nt->color[2] = ot->color[2];
            StrNCopy(nt->room, ot->room, 17);
            DmWrite(p, 0, nt, sizeof(TimeDBRecord));

            MemPtrFree((MemPtr)nt);
            MemHandleUnlock(nh);
          }
          DmReleaseRecord(new, atP, false);
        } else if (s[0] == TYPE_EXAM) {
          ExamDBRecord_v3 *oe;
          ExamDBRecord    *ne;

          nh = DmNewRecord(new, &atP, sizeof(ExamDBRecord));
          if (nh) {
            oe = (ExamDBRecord_v3 *)s;
            p = (ExamDBRecord *)MemHandleLock(nh);
            ne = (ExamDBRecord *)MemPtrNew(sizeof(ExamDBRecord));
            MemSet(ne, sizeof(ExamDBRecord), 0);
            ne->type = oe->type;
            ne->id = oe->id;
            ne->course = oe->course;
            ne->note = 0;
            ne->flags = oe->flags;
            ne->date.year = oe->date.year;
            ne->date.month = oe->date.month;
            ne->date.day = oe->date.day;
            ne->begin.hours = oe->begin.hours;
            ne->begin.minutes = oe->begin.minutes;
            ne->end.hours = oe->end.hours;
            ne->end.minutes = oe->end.minutes;
            StrNCopy(ne->room, oe->room, 17);
            DmWrite(p, 0, ne, sizeof(ExamDBRecord));

            MemPtrFree((MemPtr)ne);
            MemHandleUnlock(nh);
          }
          DmReleaseRecord(new, atP, false);
        } else {
          // Just copy to new record
          nh = DmNewRecord(new, &atP, MemHandleSize(oh));
          DmWrite(MemHandleLock(nh), 0, s, MemHandleSize(oh));
          MemHandleUnlock(nh);
          DmReleaseRecord(new, atP, false);
        }

        DmRecordInfo(new, atP, &attr, NULL, NULL);
        attr |= cat;
        DmSetRecordInfo(new, atP, &attr, NULL);

        MemHandleUnlock(oh);
      }      


      break;
    default:
      DmCloseDatabase(new);
      return dmErrCorruptDatabase;
      break;
  }

  DmCloseDatabase(old);
  DmInsertionSort(new, DatabaseCompare, 0);
  gDatabase[DB_MAIN]=new;

  DmDeleteDatabase(DATABASE_CARD, oldID);
  DmSetDatabaseInfo(DATABASE_CARD, newID, newName, NULL, NULL, NULL, NULL, 
                    NULL, NULL, NULL, NULL, NULL, NULL);

  return errNone;
}


/*****************************************************************************
* Function:  CreateDatabase
*
* Description: Create a clean database. Used when receiving a beam
*****************************************************************************/
Err CreateDatabase(const Char *dbname, LocalID *dbID) {
  Err err;

  err = DmCreateDatabase(DATABASE_CARD, dbname, APP_CREATOR, DATABASE_TYPE, false);
  if (!err) {
    DmOpenRef db;
    UInt16 DBversion=DATABASE_VERSION;
    MemHandle h;
    LocalID appInfoID;
    UniAppInfoType *appInfoP;

    *dbID = DmFindDatabase(DATABASE_CARD, dbname);
    db = DmOpenDatabase(DATABASE_CARD, *dbID, dmModeReadWrite);
    if (!db) return DmGetLastErr();

    // Allocate app info in storage heap.
    h = DmNewHandle(db, sizeof(UniAppInfoType));
    if (!h) return dmErrMemError;
  
    // Associate app info with database.
    appInfoID = MemHandleToLocalID(h);
    DmSetDatabaseInfo(DATABASE_CARD, *dbID, NULL, NULL, NULL, NULL, NULL,
                      NULL, NULL, &appInfoID, NULL, NULL, NULL);
  
    // Initialize app info block to 0.
    appInfoP = MemHandleLock(h);
    DmSet(appInfoP, 0, sizeof(UniAppInfoType), 0);

    // Initialize the categories.
    CategoryInitialize ((AppInfoPtr) appInfoP, APP_CATEGORIES);
    DmWrite(appInfoP, offsetof(UniAppInfoType, version), &DBversion, sizeof(DBversion));

    // Unlock the app info block.
    MemPtrUnlock(appInfoP);

		DmCloseDatabase(db);

  }
  return err;
}


/*****************************************************************************
* Function:  DatabaseGetCat
*
* Description: Returns the current Category
*****************************************************************************/
UInt16 DatabaseGetCat(void) {
  return gCategory;
}

/*****************************************************************************
* Function:  DatabaseSetCat
*
* Description: Sets the current Category
*****************************************************************************/
void DatabaseSetCat(UInt16 newcat) {
  gCategory=newcat;
  gPrefs.curCat = newcat;
  PrefSavePrefs(&gPrefs);
  CacheReset();
  AlarmReset(gDatabase[DB_MAIN]);
}


void UnpackCourse(CourseDBRecord *course, const MemPtr mp) {

  const PackedCourseDBRecord *packedCourse = (PackedCourseDBRecord *)mp;
  const Char *s = packedCourse->name;

  course->type=packedCourse->type;
  course->ctype=packedCourse->ctype;
  course->id=packedCourse->id;
  course->name=(Char *)s;

  s += StrLen(s) + 1;
  course->teacherName = (Char *)s;

  s += StrLen(s) + 1;
  course->teacherEmail = (Char *)s;

  s += StrLen(s) + 1;
  course->teacherPhone = (Char *)s;

  s += StrLen(s) + 1;
  course->website = (Char *)s;
}

void PackCourse(CourseDBRecord *course, MemHandle courseDBEntry) {
  UInt16 length=0, offset=0;
  Char *s;

  length = sizeof(course->type) + sizeof(course->id) + sizeof(course->ctype)
           + StrLen(course->name) + 1 // +1 for String terminator
           + StrLen(course->teacherName) + 1
           + StrLen(course->teacherEmail) + 1
           + StrLen(course->teacherPhone) + 1
           + StrLen(course->website) + 1;


  if (MemHandleResize(courseDBEntry, length) == errNone) {
    s = MemHandleLock(courseDBEntry);
    offset = 0;
    // DmSet(s, offset, length, 0);

    DmWrite(s, offset, &course->type, sizeof(course->type));
    offset += sizeof(course->type);
    DmWrite(s, offset, &course->ctype, sizeof(course->ctype));
    offset += sizeof(course->ctype);
    DmWrite(s, offset, &course->id, sizeof(course->id));
    offset += sizeof(course->id);
    DmStrCopy(s, offset, course->name);
    offset += StrLen(course->name) + 1;
    DmStrCopy(s, offset, course->teacherName);
    offset += StrLen(course->teacherName) + 1;
    DmStrCopy(s, offset, course->teacherEmail);
    offset += StrLen(course->teacherEmail) + 1;
    DmStrCopy(s, offset, course->teacherPhone);
    offset += StrLen(course->teacherPhone) + 1;
    DmStrCopy(s, offset, course->website);

    MemHandleUnlock(courseDBEntry);

  } else {
    FrmAlert(ALERT_nomem);
  }

}

/*****************************************************************************
* Function:  DatabaseSort
*
* Description: Sorts the database, does a InsertionSort, since we excpect
*              only a very few records to be changed, probably only one
*****************************************************************************/
void DatabaseSort(void) {
  DmInsertionSort(gDatabase[DB_MAIN], DatabaseCompare, 0);
}


/*****************************************************************************
* Function:  DatabaseSortByDBRef
*
* Description: Sorts the database, does a InsertionSort, since we excpect
*              only a very few records to be changed, probably only one
*              Same as DatabaseSort but this will take an argument with the
*              opened database reference. This is needed for example when
*              receiving a beam and program was not active.
*****************************************************************************/
void DatabaseSortByDBRef(DmOpenRef db) {
  DmInsertionSort(db, DatabaseCompare, 0);
}


/*****************************************************************************
* Function:  DatabaseCompare
*
* Description: Compares two records
*****************************************************************************/
Int16 DatabaseCompare(void *rec1, void *rec2, Int16 other,
                      SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH) {
  Char *s1, *s2;

  s1 = (Char *)rec1;
  s2 = (Char *)rec2;

  /* record types will be sorted descending: notes -> exams -> times -> courses
   * Same types will be sorted ascending
   */

  if (s1[0] > s2[0])  return -1;
  else if (s1[0]< s2[0]) return 1;
  else { //s1[0] == s2[0]
    if (s1[0] == TYPE_COURSE) {
      CourseDBRecord c1, c2;
      UnpackCourse(&c1, rec1);
      UnpackCourse(&c2, rec2);
      if (c1.id < c2.id) return -1;
      else if (c1.id > c2.id) return 1;
      else return StrCompare(c1.name, c2.name); // although that should not happen
                                                // we want to fail nicely
    } else if (s1[0] == TYPE_EXAM) {
      ExamDBRecord *e1=(ExamDBRecord *)rec1, *e2=(ExamDBRecord *)rec2;
      UInt32 t1, t2;
      DateTimeType d;

      d.year = e1->date.year + MAC_SHIT_YEAR_CONSTANT;
      d.month = e1->date.month;
      d.day = e1->date.day;
      d.hour = e1->begin.hours;
      d.minute = e1->begin.minutes;
      d.second = 0;
      t1 = TimDateTimeToSeconds(&d);

      d.year = e2->date.year + MAC_SHIT_YEAR_CONSTANT;
      d.month = e2->date.month;
      d.day = e2->date.day;
      d.hour = e2->begin.hours;
      d.minute = e2->begin.minutes;
      d.second = 0;
      t2 = TimDateTimeToSeconds(&d);

      if (t1 < t2)  return -1;
      else if (t1 > t2)  return 1;
      else  return 0;
    } else if (s1[0] == TYPE_TIME) {
      TimeDBRecord *t1, *t2;
      t1 = (TimeDBRecord *)rec1;
      t2 = (TimeDBRecord *)rec2;
      if (t1->day < t2->day)  return -1;
      else if (t1->day > t2->day)  return 1;
      else { // t1->day == t2->day
        UInt16 begin1, begin2;
        begin1 = TimeToInt(t1->begin);
        begin2 = TimeToInt(t2->begin);
        if (begin1 < begin2)  return -1;
        else if (begin1 > begin2)  return 1;
        return 0;
      }
    } else if (s1[0] == TYPE_CTYP) {      
      CourseTypeDBRecord *ct1, *ct2;
      ct1 = (CourseTypeDBRecord *)rec1;
      ct2 = (CourseTypeDBRecord *)rec2;
      if (ct1->id < ct2->id) return -1;
      else if (ct1->id > ct2->id) return 1;
      else return StrCompare(ct1->name, ct2->name);
    } else {
      // We don't know how to sort unknown types of data...
      return 0;
    }
  }
}
