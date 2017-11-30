/////////////////////////////////////////////////////////////////////////////
// Name:        Languages.h
// Purpose:     Languages dialog
// Author:      Alex Thuering
// Created:	23.05.2003
// RCS-ID:      $Id: Languages.h,v 1.8 2012/07/22 09:31:23 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DVDSTYLER_LANGUAGES_H
#define DVDSTYLER_LANGUAGES_H

#include <wx/wx.h>
#include <wxVillaLib/PropDlg.h>

int ChooseLanguage(int defLanguage);
const wxArrayString& GetLangNames();
wxString GetLangName(wxString langCode);
wxString GetLangCode(int index);
const BitmapArray& GetLangBitmaps();

#endif // DVDSTYLER_LANGUAGES_H
