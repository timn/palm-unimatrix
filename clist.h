/* $Id: clist.h,v 1.1 2003/02/06 21:27:23 tim Exp $
 *
 * Courses List functions
 * Created: 2002-07-11
 */

#define CLIST_SEARCH_INDEX 1
#define CLIST_SEARCH_ID 2

extern UInt16 CourseListGen(Char **itemList, UInt16 *courseID, UInt16 *courseInd, UInt16 numItems, UInt16 curInd, UInt8 searchFor);
extern Boolean CourseListHandleEvent(EventPtr event);
extern void CourseGetName(UInt16 courseID, MemHandle *charHandle);
extern UInt8 CourseGetType(UInt16 courseID);
