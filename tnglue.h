/* $Id: tnglue.h,v 1.2 2003/03/13 14:56:47 tim Exp $
 *
 * tnglue.h: Glue code to add some functions missing in 3.5 but present in 4.0+
 *
 * - Color functions, glue to get 3.5 working...
 *   Or: Why the hell didn't they get WinSetForecolorRGB into 3.5!?
 * - Pointer Retrieval
 *
 * Created: 2002-07-11
 */

#define COLORMODE_40 1
#define COLORMODE_35 2

#define TN_CT_FORE 1
#define TN_CT_BACK 2
#define TN_CT_TEXT 3

// Apps needing B/W should use 1, grayscale apps use 4 (16 grays)
// We just try it...
#define TN_GRAY_COLORDEPTH 4

extern Boolean TNisColored(void);
extern Err TNGlueColorInit(void);
extern void TNSetColorMode(UInt8 newmode);
extern void TNSetForeColorRGB(RGBColorType *new, RGBColorType *old);
extern void TNSetBackColorRGB(RGBColorType *new, RGBColorType *old);
extern void TNSetTextColorRGB(RGBColorType *new, RGBColorType *old);
extern void TNDrawCharsToFitWidth(const char *s, RectanglePtr r);
extern UInt16 TNGetObjectIndexFromPtr(FormType *form, void *formObj);
