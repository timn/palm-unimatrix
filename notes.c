/* $Id: notes.c,v 1.2 2003/04/25 23:24:38 tim Exp $
 *
 * Note support functions
 * Created: 2003/04/17
 * Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.
 */

#include "notes.h"

UInt16 gNoteID=0;
UInt16 gNoteIndex=0;
Char   gNoteTitle[32];
UInt16 gNoteItemIndex=0;
UInt16 gNoteReturnForm=FORM_main;

void NoteSet(UInt16 noteItemIndex, UInt16 returnForm) {
  MemHandle m;
  Char *s;
  UInt16 courseID=0;
  Boolean loadTitle=false;

  MemSet(gNoteTitle, 32, 0);
  gNoteItemIndex = noteItemIndex;
  gNoteReturnForm = returnForm;

  m = DmQueryRecord(DatabaseGetRefN(DB_MAIN), gNoteItemIndex);
  s = MemHandleLock(m);

  if (s[0] == TYPE_EXAM) {
    ExamDBRecord *exam = (ExamDBRecord *)s;
    gNoteID = exam->note;
    courseID = exam->course;
    loadTitle = true;
  } else if (s[0] == TYPE_TIME) {
    TimeDBRecord *time = (TimeDBRecord *)s;
    gNoteID = time->note;
    courseID = time->course;
    loadTitle = true;
  } else {
    gNoteID=0;
  }
  MemHandleUnlock(m);

  
  if (loadTitle) {
    MemHandle titleH = MemHandleNew(32);
    Char *titleS;
    
    CourseGetName(courseID, &titleH, true);
    titleS = MemHandleLock(titleH);
    StrNCopy(gNoteTitle, titleS, 31);
    MemHandleUnlock(titleH);
    MemHandleFree(titleH);
  } else {
    StrCopy(gNoteTitle, "Note");
  }

}


Boolean NoteGetIndex(UInt16 noteID, UInt16 *noteIndex) {
  MemHandle m;
  Boolean found=false;
  UInt16 index=0;
  while (!found && ((m = DmQueryNextInCategory(DatabaseGetRefN(DB_MAIN), &index, DatabaseGetCat())) != NULL)) {
    Char *s = (Char *)MemHandleLock(m);
    if (s[0] == TYPE_NOTE) {
      NoteDBRecord *note=(NoteDBRecord *)s;
      if (note->id == noteID) {
        found = true;
        *noteIndex = index;
      }
    }
    index += 1;
    MemHandleUnlock(m);
  }

  return found;
}


static void NoteViewLoad(void) {
  FieldType *fld = GetObjectPtr(NoteField);
  //Char temp[50];
  //StrPrintF(temp, "gNoteID: %u", gNoteID);
  //FrmCustomAlert(ALERT_debug, temp, "", "");
  if (gNoteID) {
    // Load

    UInt16 index=0;

    if (NoteGetIndex(gNoteID, &index)) {
      NoteDBRecord *note;
      MemHandle m;

      gNoteIndex = index;
      m = DmQueryRecord(DatabaseGetRefN(DB_MAIN), index);
      if (m) {
        note = (NoteDBRecord *)MemHandleLock(m);
	      FldSetText (fld, m, offsetof(NoteDBRecord, note), StrLen(note->note)+1);
        MemHandleUnlock(m);
        //FrmCustomAlert(ALERT_debug, "FOUND", "", "");
      } /*else {
        Char temp[50];
        /StrPrintF(temp, "Index: %u",  index);
        FrmCustomAlert(ALERT_debug, "NOT FOUND ", temp, "");
      }*/
    }
  }
}


UInt16 NoteGetNewID(DmOpenRef cats, UInt16 category) {
  MemHandle m;
  UInt16 index=0;
  UInt16 highest=0;
  while (((m = DmQueryNextInCategory(cats, &index, category)) != NULL)) {
    Char *s = (Char *)MemHandleLock(m);
    if (s[0] == TYPE_NOTE) {
      NoteDBRecord *note=(NoteDBRecord *)s;
      if (note->id > highest)  highest=note->id;
    }
    index += 1;
    MemHandleUnlock(m);
  }

  return (highest+1);
}

static void NoteDeleteLocal(void) {
  if (gNoteID) {
    // Was loaded and will be freed by Field => set FldHandle to NULL
    FieldType *fld=GetObjectPtr(NoteField);
    if (fld) FldSetTextHandle (fld, NULL);
  }
  NoteDelete(&gNoteItemIndex);
}


// Will return true if an entry has been deleted
void NoteDelete(UInt16 *noteItemIndex) {
  MemHandle m;
  Char *p;
  UInt8 itemType=0;
  UInt16 noteIndex=0;
  Boolean hasNote=false;

  gNoteID=0;

  m = DmGetRecord(DatabaseGetRefN(DB_MAIN), *noteItemIndex);
  p = MemHandleLock(m);
  itemType = p[0];
  if (itemType == TYPE_EXAM) {
    ExamDBRecord *exam=(ExamDBRecord *)p;
    if (exam->note) {
      NoteGetIndex(exam->note, &noteIndex);
      hasNote = true;
    }
    DmWrite(p, offsetof(ExamDBRecord, note), &gNoteID, sizeof(UInt16));
  } else if (itemType == TYPE_TIME) {
    TimeDBRecord *event=(TimeDBRecord *)p;
    if (event->note) {
      NoteGetIndex(event->note, &noteIndex);
      hasNote = true;
    }
    DmWrite(p, offsetof(TimeDBRecord, note), &gNoteID, sizeof(UInt16));
  }
  MemHandleUnlock(m);
  DmReleaseRecord(DatabaseGetRefN(DB_MAIN), *noteItemIndex, false);

  if (hasNote) {
    DmRemoveRecord(DatabaseGetRefN(DB_MAIN), noteIndex);
    gNoteIndex=0;
    if (itemType < TYPE_NOTE) {
      *noteItemIndex -= 1;
    }
    // We can simply decreasy since notes are BEFORE events and courses
    GadgetSetHintTimeIndex(GadgetGetHintTimeIndex()-1);
    GadgetSetHintCourseIndex(GadgetGetHintCourseIndex()-1);

  }

  
}


static void NoteViewSave(void) {
  FieldType *fld = GetObjectPtr(NoteField);
  MemHandle nn, m;
	Boolean empty;
  Char *p;

  if (FldDirty(fld)) {
		// Release any free space in the note field.
    FldCompactText(fld);
  }

  empty = (FldGetTextLength(fld) == 0);

  if (! gNoteID) {
    // Create new record
    UInt16 index=0xFFFF; // Append record at the end so that gNoteItemIndex does not get invalid
    Char *s;

    if (! empty) {
      nn = DmNewRecord(DatabaseGetRefN(DB_MAIN), &index, sizeof(NoteDBRecord)+FldGetTextLength(fld)); // +1 is already in sizeof(...)
      if (nn) {
        UInt8 ui8 = TYPE_NOTE;
        UInt16 attr;

        gNoteID = NoteGetNewID(DatabaseGetRefN(DB_MAIN), DatabaseGetCat());
        gNoteIndex = index;
        s = MemHandleLock(nn);
        DmSet(s, 0, sizeof(NoteDBRecord)+FldGetTextLength(fld), 0);
        DmWrite(s, 0, &ui8, sizeof(ui8));
        DmWrite(s, offsetof(NoteDBRecord, id), &gNoteID, sizeof(UInt16));
        DmWrite(s, offsetof(NoteDBRecord, note), FldGetTextPtr(fld), FldGetTextLength(fld));
        MemHandleUnlock(nn);
        DmReleaseRecord(DatabaseGetRefN(DB_MAIN), index, false);

        DmRecordInfo(DatabaseGetRefN(DB_MAIN), index, &attr, NULL, NULL);
        attr |= DatabaseGetCat();
        DmSetRecordInfo(DatabaseGetRefN(DB_MAIN), index, &attr, NULL);
        
        GadgetSetHintTimeIndex(GadgetGetHintTimeIndex()+1);
        GadgetSetHintCourseIndex(GadgetGetHintCourseIndex()+1);
      }
    }
  } else {
    // Save
	  // Clear the handle value in the field, otherwise the handle
	  // will be free when the form is disposed of,  this call also unlocks
	  // the handle that contains the note string.
	  FldSetTextHandle (fld, NULL);

    // Remove the note from the record, if it is empty.
    if (empty) {
      NoteDeleteLocal();
      gNoteID = 0;
    }
  }

  m = DmGetRecord(DatabaseGetRefN(DB_MAIN), gNoteItemIndex);
  p = MemHandleLock(m);
  if (p[0] == TYPE_EXAM) {
    DmWrite(p, offsetof(ExamDBRecord, note), &gNoteID, sizeof(UInt16));
  } else if (p[0] == TYPE_TIME) {
    //Char temp[50];
    //StrPrintF(temp, "Writing %u", gNoteID);
    //FrmCustomAlert(ALERT_debug, temp, "", "");
    DmWrite(p, offsetof(TimeDBRecord, note), &gNoteID, sizeof(UInt16));
  } else {
    MemHandleUnlock(m);
    NoteDeleteLocal();
  }
  MemHandleUnlock(m);
  DmReleaseRecord(DatabaseGetRefN(DB_MAIN), gNoteItemIndex, false);

  DatabaseSort();

}


static void NoteViewInit(FormPtr frm) {
  FieldPtr 		fld;
  FieldAttrType	attr;
  
  NoteViewLoad();
  
  // Have the field send events to maintain the scroll bar.
  fld = GetObjectPtr (NoteField);
  FldGetAttributes(fld, &attr);
  attr.hasScrollBar = true;
  FldSetAttributes(fld, &attr);
}


static void NoteViewDrawTitleAndForm (FormPtr frm) {
  Coord x;
  Coord maxWidth;
  Coord formWidth;
  RectangleType r;
  FontID curFont;
  Char* desc;
  RectangleType eraseRect, drawRect;
  UInt16 descLen, ellipsisWidth;
  Char* linefeedP;
  Int16 descWidth;
  IndexedColorType curForeColor;
  IndexedColorType curBackColor;
  IndexedColorType curTextColor;
  UInt8 * lockedWinP;
  
  // Get current record and related info.
  //
  desc = gNoteTitle; //apptRec.description;

  // "Lock" the screen so that all drawing occurs offscreen to avoid
  // the anamolies associated with drawing the Form's title then drawing
  // the NoteView title.  We REALLY need to make a variant for doing
  // this in a more official way!
  //
  WinSetBackColor(UIColorGetTableEntryIndex(UIFormFill));
  lockedWinP = WinScreenLock (winLockErase);

  FrmSetMenu(frm, MENU_main);
  FrmDrawForm(frm);

  // Perform initial set up.
  //
  FrmGetFormBounds(frm, &r);
  formWidth = r.extent.x;
  maxWidth = formWidth - 8;

  linefeedP = StrChr (desc, linefeedChr);
  descLen = (linefeedP == NULL ? StrLen (desc) : linefeedP - desc);
  ellipsisWidth = 0;
      
  RctSetRectangle (&eraseRect, 0, 0, formWidth, FntLineHeight()+4);
  RctSetRectangle (&drawRect, 0, 0, formWidth, FntLineHeight()+2);
  
  // Save/Set window colors and font.  Do this after FrmDrawForm() is called
  // because FrmDrawForm() leaves the fore/back colors in a state that we
  // don't want here.
  //
  curForeColor = WinSetForeColor(UIColorGetTableEntryIndex(UIFormFrame));
  curBackColor = WinSetBackColor(UIColorGetTableEntryIndex(UIFormFill));
  curTextColor = WinSetTextColor(UIColorGetTableEntryIndex(UIFormFrame));
  curFont = FntSetFont (boldFont);

  // Erase the Form's title area and draw the NoteView's.
  WinEraseRectangle (&eraseRect, 0);
  WinDrawRectangle (&drawRect, 3);
  
  if (FntWidthToOffset (desc, descLen, maxWidth, NULL, &descWidth) != descLen) {
    ellipsisWidth = FntCharWidth(chrEllipsis);
    descLen = FntWidthToOffset (desc, descLen, maxWidth - ellipsisWidth, NULL, &descWidth);
  }

  x = (formWidth - descWidth - ellipsisWidth + 1) / 2;
  
  WinDrawInvertedChars(desc, descLen, x, 1);
  if (ellipsisWidth != 0) {
    Char buf[maxCharBytes + sizeOf7BitChar(chrNull)];
    buf[TxtSetNextChar(buf, 0, chrEllipsis)] = chrNull;
    WinDrawInvertedChars (buf, StrLen(buf), x + descWidth, 1);
  }
  
  // Now that we've drawn everything, blast it all back on the screen at once.
  if (lockedWinP)  WinScreenUnlock();
  
  // Unlock the record that ApptGetRecord() implicitly locked.
  //MemHandleUnlock (recordH);
  
  // Restore window colors and font.
  WinSetForeColor(curForeColor);
  WinSetBackColor(curBackColor);
  WinSetTextColor(curTextColor);
  FntSetFont(curFont);
}


static void NoteViewUpdateScrollBar (void) {
  UInt16 scrollPos;
  UInt16 textHeight;
  UInt16 fieldHeight;
  Int16 maxValue;
  FieldPtr fld;
  ScrollBarPtr bar;
  
  fld = GetObjectPtr(NoteField);
  bar = GetObjectPtr(NoteScrollBar);
  
  FldGetScrollValues(fld, &scrollPos, &textHeight,  &fieldHeight);
  
  if (textHeight > fieldHeight) {
    // On occasion, such as after deleting a multi-line selection of text,
    // the display might be the last few lines of a field followed by some
    // blank lines.  To keep the current position in place and allow the user
    // to "gracefully" scroll out of the blank area, the number of blank lines
    // visible needs to be added to max value.  Otherwise the scroll position
    // may be greater than maxValue, get pinned to maxvalue in SclSetScrollBar
    // resulting in the scroll bar and the display being out of sync.
    maxValue = (textHeight - fieldHeight) + FldGetNumberOfBlankLines (fld);
  } else if (scrollPos)
    maxValue = scrollPos;
  else
    maxValue = 0;
  
  SclSetScrollBar (bar, scrollPos, 0, maxValue, fieldHeight-1);
}

static void NoteViewScroll (Int16 linesToScroll, Boolean updateScrollbar) {
  UInt16			blankLines;
  FieldPtr			fld;
  
  fld = GetObjectPtr (NoteField);
  blankLines = FldGetNumberOfBlankLines (fld);
  
  if (linesToScroll < 0)
    FldScrollField (fld, -linesToScroll, winUp);
  else if (linesToScroll > 0)
    FldScrollField (fld, linesToScroll, winDown);
  
  // If there were blank lines visible at the end of the field
  // then we need to update the scroll bar.
  if ((blankLines && (linesToScroll < 0)) || updateScrollbar) {
    NoteViewUpdateScrollBar();
  }
}


static void NoteViewPageScroll (WinDirectionType direction) {
  UInt16 linesToScroll;
  FieldPtr fld;
  
  fld = GetObjectPtr(NoteField);
  
  if (FldScrollable(fld, direction))	{
    linesToScroll = FldGetVisibleLines(fld) - 1;
    
    if (direction == winUp)
      linesToScroll = -linesToScroll;
      
    NoteViewScroll(linesToScroll, true);
  }
}


Boolean NoteViewHandleEvent (EventType *event) {
	FormPtr frm;
	Boolean handled = false;

  if (event->eType == keyDownEvent) {
		if (TxtCharIsHardKey(event->data.keyDown.modifiers, event->data.keyDown.chr)) {
      if (FldGetTextHandle(GetObjectPtr(NoteField))) NoteViewSave();
      FrmReturnToForm(gNoteReturnForm);
      FrmUpdateForm(gNoteReturnForm, frmRedrawUpdateCode);
      handled = true;
    } else if (EvtKeydownIsVirtual(event)) {
      if (event->data.keyDown.chr == vchrPageUp) {
        NoteViewPageScroll(winUp);
        handled = true;
      } else if (event->data.keyDown.chr == vchrPageDown) {
        NoteViewPageScroll(winDown);
        handled = true;
      }
    }
  }	else if (event->eType == ctlSelectEvent) {
    switch (event->data.ctlSelect.controlID) {
      case NoteDoneButton:
        if (FldGetTextHandle(GetObjectPtr(NoteField))) NoteViewSave();
        FrmReturnToForm(gNoteReturnForm);
        FrmUpdateForm(gNoteReturnForm, frmRedrawUpdateCode);
        handled = true;
        break;

      case NoteDeleteButton:
        NoteDeleteLocal();
        FrmReturnToForm(gNoteReturnForm);
        FrmUpdateForm(gNoteReturnForm, frmRedrawUpdateCode);
        handled = true;
        break;
    }
  }	else if (event->eType == menuEvent) {
    handled = HandleMenuEvent(event->data.menu.itemID);
  }	else if (event->eType == frmOpenEvent) {
    frm = FrmGetActiveForm();
    NoteViewInit(frm);
    NoteViewDrawTitleAndForm(frm);
    NoteViewUpdateScrollBar();
    FrmSetFocus(frm, FrmGetObjectIndex (frm, NoteField));
    handled = true;
  } else if (event->eType == fldChangedEvent) {
    frm = FrmGetActiveForm();
    NoteViewUpdateScrollBar();
    handled = true;
  } else if (event->eType == frmUpdateEvent) {
    frm = FrmGetActiveForm ();
    NoteViewDrawTitleAndForm (frm);
    handled = true;

  } else if (event->eType == frmCloseEvent) {
    // Is not send for modal forms
    //if (FldGetTextHandle(GetObjectPtr(NoteField))) NoteViewSave();

  } else if (event->eType == sclRepeatEvent) {
    NoteViewScroll(event->data.sclRepeat.newValue - event->data.sclRepeat.value, false);
  }
  
  return (handled);
}
