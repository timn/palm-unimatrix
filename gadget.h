/* $Id: gadget.h,v 1.1 2003/02/06 21:27:23 tim Exp $
 *
 * The headers for th heart of UniMatrix
 */

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

#define GADGET_FEAT_NUM 2
#define GADGET_FEAT_SHOWTYPES 0
#define GADGET_FEAT_SHOWTIMELINE 1


// Local definition for timeline drawtype
typedef enum GadgetTimelineDrawType {
  gtDraw, gtErase
} GadgetTimelineDrawType;


extern void DrawCharsToFitWidth(const char *s, RectanglePtr r);
extern void GadgetDraw(Boolean eraseFirst);
extern void GadgetDrawTime(TimeType begin, TimeType end, UInt8 day, RGBColorType *color, UInt16 courseID);
extern void GadgetRedraw(void);
extern void GadgetDrawEvents(void);
extern void GadgetDrawTimeline(GadgetTimelineDrawType drawType);
extern void GadgetDrawHint(const char *toptext, const char *bottext);
extern Boolean GadgetHandler(FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP);
extern void GadgetTap(FormGadgetType *pGadget, EventType *event);
extern UInt16 GadgetGetHintTimeIndex(void);
extern void GadgetDrawHintCurrent(void);
extern void GadgetDrawHintNext(void);
extern void GadgetDrawHintErase(void);
extern TimeFormatType GadgetGetTimeFormat(void);
extern void GadgetSwitchScreen(void);
extern void GadgetSet(FormPtr frm, UInt16 gadgetID, UInt16 hintGadgetID, UInt8 numDays);
extern void GadgetSetNeedsCompleteRedraw(Boolean need);
extern void GadgetSetNumDays(UInt8 numDays);
extern void GadgetSetFeature(UInt8 feature, UInt8 value);
extern void GadgetSetHintTimeIndex(UInt16 newTimeInd);
extern UInt16 GadgetGetHintCourseIndex(void);
extern void GadgetSetHintCourseIndex(UInt16 newCourseInd);
