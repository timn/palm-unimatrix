/* $Id: clist.h,v 1.3 2003/03/13 14:56:47 tim Exp $
 *
 * Courses List functions
 * Created: 2002-07-11
 */

#define CLIST_SEARCH_INDEX 1
#define CLIST_SEARCH_ID 2

extern UInt16 CourseListGen(Char **itemList, UInt16 *courseID, UInt16 *courseInd, UInt16 numItems, UInt16 curInd, UInt8 searchFor) SECOND_SECTION;
extern Boolean CourseListHandleEvent(EventPtr event) SECOND_SECTION;
extern void CourseGetName(UInt16 courseID, MemHandle *charHandle, Boolean longformat) SECOND_SECTION;
extern UInt8 CourseGetType(UInt16 courseID) SECOND_SECTION;
extern Boolean CourseGetIndex(UInt16 courseID, UInt16 *index) SECOND_SECTION;
extern UInt16 CourseNewID(DmOpenRef cat, UInt16 category) SECOND_SECTION;
extern UInt16 CountCourses(void) SECOND_SECTION;
