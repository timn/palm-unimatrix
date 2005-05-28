
/***************************************************************************
 *  tnglue.h - Glue code to add some functions missing in 3.5 but present
 *             in 4.0+
 *
 *           - Color functions, glue to get 3.5 working...
 *             Or: Why the hell didn't they get WinSetForecolorRGB into 3.5!?
 *            - Pointer Retrieval
 *
 *
 *  Generated: 2002-07-11
 *  Copyright  2002-2005  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: tnglue.h,v 1.5 2005/05/28 12:59:14 tim Exp $
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __TNGLUE_H
#define __TNGLUE_H

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
extern UInt32 TNPalmOSVersion(void);
extern MemHandle TNDmQueryPrevInCategory(DmOpenRef db, UInt16 *index, UInt16 category);

#endif //__TNGLUE_H
