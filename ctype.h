/* $Id: ctype.h,v 1.2 2003/03/13 14:56:47 tim Exp $
 *
 * Course type headers
 * Created: 2002-08-31
 */


#define CTYPE_SHORT_MAXLENGTH 3
#define CTYPE_MAXLENGTH 20
// this is a space and [ and ], NOT the NULL termination! Calc that extra!
#define CTYPE_ADD_MAXLENGTH 3
#define CTYPE_DEF_NUM 6

extern UInt8 CourseTypeListPopup(UInt16 listID, UInt16 triggerID, UInt8 selected, Char *triggerLabel) THIRD_SECTION;
extern void CourseTypeGetName(Char *name, UInt8 id) THIRD_SECTION;
extern void CourseTypeGetShort(MemHandle *charHandle, UInt8 id) THIRD_SECTION;
extern void CourseTypeGetShortByCourseID(MemHandle *charHandle, UInt16 courseID) THIRD_SECTION;
extern Boolean CourseTypeFormHandleEvent(EventPtr event) THIRD_SECTION;
extern Boolean CourseTypeGetDBIndex(UInt8 courseTypeID, UInt16 *courseTypeDBindex) THIRD_SECTION;
