/* $Id: edit.h,v 1.3 2005/05/28 09:49:54 tim Exp $
 *
 * Functions for editing courses, events, times, ...
 */


extern void CatPreEdit(UInt16 numRecords, UInt32 *recordList) SECOND_SECTION;
extern void CatPostEdit(UInt16 numRecords, UInt32 *recordList) SECOND_SECTION;
extern void EditCourse(UInt16 courseInd) SECOND_SECTION;
extern Boolean EditCourseAutoFill(EventPtr event) SECOND_SECTION;
extern Boolean EditCourseFormHandleEvent(EventPtr event);
extern Boolean EditTimeAutoFill(EventPtr event) SECOND_SECTION;
extern Boolean EditTimeFormHandleEvent(EventPtr event);
extern void EditTimeGetTime(ControlType *ctl, TimeType *begin, TimeType *end, Char *label) SECOND_SECTION;
extern void EditTimeSetSelectorText(ControlType *ctl, TimeType *begin, TimeType *end, Char *label);
extern Boolean EditTimeCheckCollision(TimeType begin, TimeType end, UInt8 day, UInt16 notIndex, Boolean checkIndex);
extern void EditTime(void);
extern void AddCourse(void);
extern void AddTime(void);
