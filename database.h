
/***************************************************************************
 *  database.h - database handling
 *
 *  Generated: 2002
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: database.h,v 1.5 2005/05/28 12:59:14 tim Exp $
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

#ifndef __DATABASE_H_
#define __DATABASE_H_

#include "UniMatrix.h"

/** Two databases
 * We use two since we have two types of content for the DB:
 *
 *   Categorized content aka "Cats"
 * Categories are used for semesters. We save for example courses, events, exams
 * per semester
 *
 *   UNcategorized content aka "Dogs"
 * These are for example the course types. They are persistent for all semesters,
 * they could be made semester dependant, but that wouldn't really make sense
 */

#define DATABASE_NAME "UniMatrixDB"
#define DATABASE_TYPE 'Time'

#define DATABASE_DATA_NAME "UniMatrixDataDB"
#define DATABASE_DATA_TYPE 'Data'

// Used for convert
#define DATABASE_TEMPNAME "UniMatrixDB_TEMP"

#define DATABASE_CARD 0
#define DATABASE_VERSION 4

#define DATABASE_NUM 2
#define DB_MAIN 0
#define DB_DATA 1

// That is just for maso-coders...
#define MAC_SHIT_YEAR_CONSTANT 1904

// Types 0 < i < 32 are reserved for main DB
// Types 32 <= i < 64 are reserved for data DB
#define TYPE_COURSE 1
#define TYPE_TIME 2
#define TYPE_EXAM 3
#define TYPE_NOTE 4
#define TYPE_CTYP 32

#define offsetof(s,m) ((UInt16)&(((s *)0)->m))


typedef struct {
   AppInfoType  appInfo;
   UInt16       version;
} UniAppInfoType;

typedef struct {
  UInt8       type;
  UInt8       ctype;
  UInt16      id;
  const Char *name;
  const Char *teacherName;
  const Char *teacherEmail;
  const Char *teacherPhone;
  const Char *website;
} CourseDBRecord;

typedef struct {
  UInt8     type;
  UInt8     day;
  UInt16    course;
  UInt16    note;
  TimeType  begin;
  TimeType  end;
  UInt8     color[3];
  Char      room[17];
} TimeDBRecord;


typedef struct {
  UInt8   type;
  UInt8   ctype;
  UInt16  id;
  Char    name[1];
} PackedCourseDBRecord;

typedef struct {
  UInt8 type;
  UInt8 id;
  Char  shortName[4];
  Char  name[21];  // We take the overhead...
} CourseTypeDBRecord;

typedef struct {
  UInt8 type;
  UInt8 id;
  UInt16 course;
  UInt16 note;
  UInt16 flags;
  DateType date;
  TimeType begin;
  TimeType end;
  Char room[17]; // BIG baby :-)
} ExamDBRecord;

typedef struct {
  UInt8 type;
  UInt8 reserved;
  UInt16 id;
  Char   note[1];
} NoteDBRecord;

extern Err  OpenDatabase(void) THIRD_SECTION;
extern void CloseDatabase(void) THIRD_SECTION;
extern DmOpenRef DatabaseGetRef(void) THIRD_SECTION;
extern DmOpenRef DatabaseGetRefN(UInt8 num) THIRD_SECTION;
extern void DeleteDatabase(void) THIRD_SECTION;
extern Err DatabaseConvert(UInt16 oldVersion) THIRD_SECTION;
extern Err CreateDatabase(const Char *dbname, LocalID *dbID) THIRD_SECTION;
extern UInt16 DatabaseGetCat(void);
extern void DatabaseSetCat(UInt16 newcat);

extern void PackCourse(CourseDBRecord *course, MemHandle courseDBEntry);
extern void UnpackCourse(CourseDBRecord *course, const MemPtr mp);

extern void DatabaseSort(void);
extern void DatabaseSortByDBRef(DmOpenRef db);
extern Int16 DatabaseCompare(void *rec1, void *rec2, Int16 other, SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo, MemHandle appInfoH);





/** Compatibility declarations to make convert of db possible, to be dropped in later version */
typedef struct {
  UInt8 type;
  UInt8 day;
  UInt16 course;
  UInt8 begin;
  UInt8 end;
  UInt8 color[3];
  Char room[9];
} TimeDBRecord_v1;

typedef struct {
  UInt8 type;
  UInt8 day;
  UInt16 course;
  TimeType begin;
  TimeType end;
  UInt8 color[3];
  Char room[17];
} TimeDBRecord_v3;


typedef struct {
  UInt8 type;
  UInt8 id;
  UInt16 course;
  UInt16 flags;
  DateType date;
  TimeType begin;
  TimeType end;
  Char room[17]; // BIG baby :-)
} ExamDBRecord_v3;


#endif /* __DATABASE_H_ */
