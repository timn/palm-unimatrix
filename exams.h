/* $Id: exams.h,v 1.5 2003/04/25 23:24:38 tim Exp $
 *
 * Everything you wanna know about exam code :-)
 * Created: 2002-07-11
 */



#define EXCOL_DONE 0
#define EXCOL_COURSE 1
#define EXCOL_NOTE 2
#define EXCOL_DATE 3
#define EXCOL_TIME 4
#define EXCOL_SELI 5

#define EX_MAX_ROWS 10
#define EX_NUM_HANDLECOLS 3

#define EX_FLAG_DONE 0x0001

extern void ExamSetGoto(UInt32 uniqueID);
extern Boolean ExamsFormHandleEvent(EventPtr event) SECOND_SECTION;
extern Boolean ExamDetailsFormHandleEvent(EventPtr event) SECOND_SECTION;
