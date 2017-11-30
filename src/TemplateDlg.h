/////////////////////////////////////////////////////////////////////////////
// Name:        TemplateDlg.h
// Purpose:     Select DVD menu template dialog
// Author:      Alex Thuering
// Created:     20.10.2009
// RCS-ID:      $Id: TemplateDlg.h,v 1.8 2016/01/04 19:03:43 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef TEMPLATE_DLG_H
#define TEMPLATE_DLG_H

#include "DVD.h"
#include "wxVillaLib/Thumbnails.h"
#include <wx/wx.h>
#include <wx/image.h>
#include <vector>

using namespace std;

//(*Headers(TemplateDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/statline.h>
#include <wx/radiobut.h>
#include <wxVillaLib/Thumbnails.h>
#include <wx/dialog.h>
//*)

class TemplateDlg: public wxDialog {
public:
	/** Constructor */
	TemplateDlg(wxWindow* parent, AspectRatio aspectRatio);
	virtual ~TemplateDlg();
	
	/** Returns template file name */
	wxString GetTemplate();
	/** Returns title */
	wxString GetTitle();
	/** Returns true if chapter menu need to be created */
	bool IsChapterMenu();

	//(*Declarations(TemplateDlg)
	wxThumbnails* m_thumbnails;
	wxRadioButton* m_titleMenu;
	wxTextCtrl* m_titleText;
	wxListBox* m_categoryListBox;
	wxRadioButton* m_chapterMenu;
	//*)

protected:
	//(*Identifiers(TemplateDlg)
	static const long ID_TEXTCTRL1;
	static const long ID_RADIOBUTTON1;
	static const long ID_RADIOBUTTON2;
	static const long CATEGORY_LIST_BOX_ID;
	static const long THUMBNAILS_ID;
	//*)

private:
    AspectRatio m_aspectRatio;
    wxArrayString m_dirs;
    void LoadThumbnails(wxString dir);
    void OnThumbDoubleClick(wxCommandEvent& event);

	//(*Handlers(TemplateDlg)
	void OnCategorySelect(wxCommandEvent& event);
	//*)

	DECLARE_EVENT_TABLE()
};

#endif // TEMPLATE_DLG_H
