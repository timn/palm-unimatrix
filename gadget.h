/* $Id: gadget.h,v 1.4 2005/05/27 15:04:02 tim Exp $
 *
 * The headers for th heart of UniMatrix
 */

#ifndef GADGET_H
#define GADGET_H

#include "database.h"

#define GADGET_SCREEN_DAY 1
#define GADGET_SCREEN_NIGHT 2

#define GADGET_STRINGS_OFFSET 16
#define GADGET_STRINGS_TOP 9
#define GADGET_BASELEFT 2

#define GADGET_STRINGS_WDAYSTART 8111

#define GADGET_DEFAULT_NUMDAYS 5
#define GADGET_TOTAL_DRAWWIDTH 120
// No longer used since it needs to be calculated on the fly
// define GADGET_DAYS_WIDTH 24
// define GADGET_WIDTH (GADGET_DAYS_NUM*GADGET_DAYS_WIDTH+GADGET_DAYS_NUM-1)
#define GADGET_HEIGHT ((7-1) * GADGET_STRINGS_OFFSET)
#define GADGET_TOP GADGET_STRINGS_TOP + 5
#define GADGET_LEFT 25

#define GADGET_DAYVALUE 1440
#define GADGET_MAX_HOUR 20
#define GADGET_MIN_HOUR 8
#define GADGET_MAX_PIXELHEIGHT 96

#define GADGET_NOTESYMBOL 11

// Changing this to anything >2 _will_ break the code! See GadgetDrawEvents. The look-ahead
// needs to be changed for that case!
#define GADGET_MAX_AT_A_TIME 2

// Local definition for timeline drawtype
typedef enum GadgetTimelineDrawType {
  gtDraw, gtErase
} GadgetTimelineDrawType;

typedef struct {
  UInt16 index;
  RectangleType rect;
  UInt8 num;
  UInt8 pos;
} GadgetTimeListType;


extern void DrawCharsToFitWidth(const char *s, RectanglePtr r) THIRD_SECTION;
extern void GadgetDraw(Boolean eraseFirst) THIRD_SECTION;
extern void GadgetDrawTime(TimeType begin, TimeType end, UInt8 day, RGBColorType *color, UInt16 courseID, UInt8 num_times, UInt8 pos) THIRD_SECTION;
extern void GadgetRedraw(void) THIRD_SECTION;
extern void GadgetDrawEvents(void) THIRD_SECTION;
extern void GadgetDrawTimeline(GadgetTimelineDrawType drawType) THIRD_SECTION;
extern void GadgetDrawHint(const char *toptext, const char *bottext, UInt16 note) THIRD_SECTION;
extern Boolean GadgetHandler(FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP) THIRD_SECTION;
extern Boolean GadgetHintHandler(FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP) THIRD_SECTION;
extern void GadgetTap(FormGadgetType *pGadget, EventType *event) THIRD_SECTION;
extern void GadgetHintTap(FormGadgetType *pGadget, EventType *event) THIRD_SECTION;
extern UInt16 GadgetGetHintTimeIndex(void) THIRD_SECTION;
extern void GadgetDrawHintCurrent(void) THIRD_SECTION;
extern void GadgetDrawHintNext(void) THIRD_SECTION;
extern void GadgetDrawHintErase(void) THIRD_SECTION;
extern TimeFormatType GadgetGetTimeFormat(void) THIRD_SECTION;
extern void GadgetSwitchScreen(void) THIRD_SECTION;
extern void GadgetSet(FormPtr frm, UInt16 gadgetID, UInt16 hintGadgetID) THIRD_SECTION;
extern void GadgetSetNeedsCompleteRedraw(Boolean need) THIRD_SECTION;
extern void GadgetSetNumDays(UInt8 numDays) THIRD_SECTION;
extern void GadgetSetHintTimeIndex(UInt16 newTimeInd) THIRD_SECTION;
extern UInt16 GadgetGetHintCourseIndex(void) THIRD_SECTION;
extern void GadgetSetHintCourseIndex(UInt16 newCourseInd) THIRD_SECTION;
extern void GadgetDrawStep(WinDirectionType direction) THIRD_SECTION;
extern void GadgetTimeSetRect(RectangleType *rect, TimeType begin, TimeType end, UInt8 day, UInt8 num_times, UInt8 pos) THIRD_SECTION;


#endif // GADGET_H
