/* $Id: edit.h,v 1.2 2003/03/13 14:56:47 tim Exp $
 *
 * Functions for editing courses, events, times, ...
 */


extern void CatPreEdit(UInt16 numRecords, UInt32 *recordList);
extern void CatPostEdit(UInt16 numRecords, UInt32 *recordList);
extern void EditCourse(UInt16 courseInd);
extern Boolean EditCourseFormHandleEvent(EventPtr event);
extern Boolean EditTimeFormHandleEvent(EventPtr event);
extern void EditTimeGetTime(ControlType *ctl, TimeType *begin, TimeType *end, Char *label);
extern void EditTimeSetSelectorText(ControlType *ctl, TimeType *begin, TimeType *end, Char *label);
extern Boolean EditTimeCheckCollision(TimeType begin, TimeType end, UInt8 day, UInt16 notIndex, Boolean checkIndex);
extern void EditTime(void);
extern void AddCourse(void);
extern void AddTime(void);
