/* $Id: clist.h,v 1.2 2003/02/07 01:07:52 tim Exp $
 *
 * Courses List functions
 * Created: 2002-07-11
 */

#define CLIST_SEARCH_INDEX 1
#define CLIST_SEARCH_ID 2

extern UInt16 CourseListGen(Char **itemList, UInt16 *courseID, UInt16 *courseInd, UInt16 numItems, UInt16 curInd, UInt8 searchFor) SECOND_SECTION;
extern Boolean CourseListHandleEvent(EventPtr event) SECOND_SECTION;
extern void CourseGetName(UInt16 courseID, MemHandle *charHandle);
extern UInt8 CourseGetType(UInt16 courseID);
extern Boolean CourseGetIndex(UInt16 courseID, UInt16 *index);
