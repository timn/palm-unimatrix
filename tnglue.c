/* $Id: tnglue.c,v 1.3 2003/04/25 23:24:38 tim Exp $
 *
 * tnglue. See tnglue.h for more info
 * Created: 2002-07-11
 */

#include <PalmOS.h>
#include "tnglue.h"

UInt8 gColorMode=COLORMODE_35;

Err TNGlueColorInit(void) {
  Err err;
  UInt32 romVersion;
  UInt32 requiredDepth = TN_GRAY_COLORDEPTH;
  UInt32 currentDepth = 0;
  Boolean hasColor=false;

  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
  if (romVersion < 0x04000000) TNSetColorMode(COLORMODE_35);
  else TNSetColorMode(COLORMODE_40);

  // Get Display mode
  err = WinScreenMode(winScreenModeGet, NULL, NULL, &currentDepth, &hasColor);
  // Try to set higher, used to get inofficial gray scale support
  if (!hasColor && (currentDepth < requiredDepth)) {
    // Although we might set an err != errNone above we do not care about the result.
    // That may happen on some Palms that do not even support grayscale, like the
    // Palm III. This causes BadBugs(TM)
    err = WinScreenMode(winScreenModeSet, NULL, NULL, &requiredDepth, &hasColor);
  }
  return errNone;
}


Boolean TNisColored(void) {
  UInt32 currentDepth = 0;
  Boolean hasColor=false;
  WinScreenMode(winScreenModeGet, NULL, NULL, &currentDepth, &hasColor);
  return hasColor;
}


void TNSetColorMode(UInt8 newmode) {
  gColorMode=newmode;
}


static void TNSetColorRGB(RGBColorType *new, RGBColorType *old, UInt8 type) {
  if (gColorMode == COLORMODE_35) {
    IndexedColorType oldCI=0;
    IndexedColorType newCI=0;

    newCI=WinRGBToIndex(new);

    if (type == TN_CT_FORE)  oldCI=WinSetForeColor(newCI);
    else if (type == TN_CT_BACK)  oldCI=WinSetBackColor(newCI);
    else if (type == TN_CT_TEXT)  oldCI=WinSetTextColor(newCI);

    if (old != NULL)  WinIndexToRGB(oldCI, old);
  } else {

    if (type == TN_CT_FORE)  WinSetForeColorRGB(new, old);
    else if (type == TN_CT_BACK)  WinSetBackColorRGB(new, old);
    else if (type == TN_CT_TEXT)  WinSetTextColorRGB(new, old);

  }
}

void TNSetForeColorRGB(RGBColorType *new, RGBColorType *old) {
  TNSetColorRGB(new, old, TN_CT_FORE);
}


void TNSetBackColorRGB(RGBColorType *new, RGBColorType *old) {
  TNSetColorRGB(new, old, TN_CT_BACK);
}

void TNSetTextColorRGB(RGBColorType *new, RGBColorType *old) {
  TNSetColorRGB(new, old, TN_CT_TEXT);
}


/*****************************************************************************
* Function: TNDrawCharsToFitWidth
*
* Description:  Helper function to draw string in global find
*****************************************************************************/
void TNDrawCharsToFitWidth(const char *s, RectanglePtr r) {
  Int16   stringLength = StrLen(s);
  Int16   pixelWidth = r->extent.x;
  Boolean truncate;

  // FntCharsInWidth will update stringLength to the 
  // maximum without exceeding the width
  FntCharsInWidth(s, &pixelWidth, &stringLength, &truncate);
  WinDrawChars(s, stringLength, r->topLeft.x, r->topLeft.y);
}




UInt16 TNGetObjectIndexFromPtr(FormType *form, void *formObj) {
  UInt16 index;
  UInt16 objIndex = frmInvalidObjectId;
  UInt16 numObjects = FrmGetNumberOfObjects(form);
  for (index = 0; index < numObjects; index++) {
    if (FrmGetObjectPtr(form, index) == formObj) {
    // Found it
    objIndex = index;
    break;
    }
  }

  return objIndex;
}


UInt32 TNPalmOSVersion(void) {
  UInt32 romVersion=0;

	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

  return romVersion;
}
