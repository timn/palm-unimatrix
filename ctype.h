/* $Id: ctype.h,v 1.1 2003/02/06 21:27:23 tim Exp $
 *
 * Course type headers
 * Created: 2002-08-31
 */


#define CTYPE_SHORT_MAXLENGTH 3
#define CTYPE_MAXLENGTH 20
// this is a space and [ and ], NOT the NULL termination! Calc that extra!
#define CTYPE_ADD_MAXLENGTH 3
#define CTYPE_DEF_NUM 6

extern UInt8 CourseTypeListPopup(UInt16 listID, UInt16 triggerID, UInt8 selected, Char *triggerLabel);
extern void CourseTypeGetName(Char *name, UInt8 id);
extern void CourseTypeGetShort(Char *name, UInt8 id);
extern void CourseTypeGetShortByCourseID(Char *name, UInt16 courseID);
extern Boolean CourseTypeFormHandleEvent(EventPtr event);
extern Boolean CourseTypeGetDBIndex(UInt8 courseTypeID, UInt16 *courseTypeDBindex);
