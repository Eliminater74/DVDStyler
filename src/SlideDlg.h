/////////////////////////////////////////////////////////////////////////////
// Name:        SlideDlg.h
// Purpose:     Slide properties dialog
// Author:      Alex Thuering
// Created:     23.12.2013
// RCS-ID:      $Id: SlideDlg.h,v 1.1 2013/12/29 21:08:21 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
#ifndef SLIDEDLG_H
#define SLIDEDLG_H

//(*Headers(SlideDlg)
#include "wxVillaLib/Thumbnails.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/dialog.h>
//*)
#include "Slideshow.h"

class SlideDlg: public wxDialog {
public:
	/** Constructor */
	SlideDlg(wxWindow* parent, Slide* slide, int defaultTranstion);
	/** Destructor */
	virtual ~SlideDlg();
	/** Returns thumbnail image for given transition */
	static wxImage getTransitionImage(const wxString& transition);

	//(*Declarations(SlideDlg)
	wxThumbnails* m_thumbnails;
	wxStaticText* m_fileNameText;
	//*)

protected:
	//(*Identifiers(SlideDlg)
	static const long ID_FILENAME_TEXT;
	static const long ID_THUMBNAILS;
	//*)

private:
	Slide* m_slide;
	wxArrayString m_transitions;
	//(*Handlers(SlideDlg)
	//*)
	void OnOkBt(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif
