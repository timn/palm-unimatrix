/* $Id: exams.h,v 1.4 2003/04/18 23:34:59 tim Exp $
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

// That is just for maso-coders...
#define MAC_SHIT_YEAR_CONSTANT 1904

#define EX_FLAG_DONE 0x0001

extern Boolean ExamsFormHandleEvent(EventPtr event) SECOND_SECTION;
extern Boolean ExamDetailsFormHandleEvent(EventPtr event) SECOND_SECTION;
