/////////////////////////////////////////////////////////////////////////////
// Name:        SingleThumbnailDlg.h
// Purpose:     Single thumbnail dialog
// Author:      Alex Thuering
// Created:     07.01.2014
// RCS-ID:      $Id: SingleThumbnailDlg.h,v 1.1 2014/01/08 19:35:36 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
#ifndef SINGLETHUMBNAILDLG_H
#define SINGLETHUMBNAILDLG_H

#include <wx/dialog.h>
#include <wx/stattext.h>
#include <vector>
class wxThumbnails;

class SingleThumbnailDlg: public wxDialog {
public:
	/** Constructor */
	SingleThumbnailDlg(wxWindow *parent, const wxString& message, const wxString& caption,
			const wxArrayString& fileNames, const wxArrayString& labels);
	/** Constructor */
	SingleThumbnailDlg(wxWindow *parent, const wxString& message, const wxString& caption,
			const wxArrayString& choices, std::vector<wxImage> images);
	/** Destructor */
	virtual ~SingleThumbnailDlg();

    void SetSelection(int sel);
    int GetSelection() const;

private:
	wxThumbnails* m_thumbnails;
	void OnThumbDoubleClick(wxCommandEvent& event);
	void Create(wxWindow* parent, const wxString& caption,
			const wxString& message);

	DECLARE_EVENT_TABLE()
};

#endif
