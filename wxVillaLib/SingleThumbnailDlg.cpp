/////////////////////////////////////////////////////////////////////////////
// Name:        SingleThumbnailDlg.cpp
// Purpose:     Single thumbnail dialog
// Author:      Alex Thuering
// Created:     07.01.2014
// RCS-ID:      $Id: SingleThumbnailDlg.cpp,v 1.2 2014/01/09 17:49:19 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
#include "SingleThumbnailDlg.h"
#include <wx/file.h>
#include <wx/button.h>
#include <wx/string.h>
#include <wx/intl.h>
#include "Thumbnails.h"
#include "utils.h"
#include "rc/loading.png.h"

enum {
	THUMBNAILS_ID = 9100,
};

BEGIN_EVENT_TABLE(SingleThumbnailDlg, wxDialog)
	EVT_THUMBNAILS_DCLICK(THUMBNAILS_ID, SingleThumbnailDlg::OnThumbDoubleClick)
END_EVENT_TABLE()

SingleThumbnailDlg::SingleThumbnailDlg(wxWindow *parent, const wxString& message, const wxString& caption,
		const wxArrayString& fileNames, const wxArrayString& labels) {
	Create(parent, caption, message);

	for (unsigned int i = 0; i < fileNames.size(); i++) {
		wxThumb* thumb = new wxThumb(fileNames[i], labels[i]);
		m_thumbnails->InsertItem(thumb);
	}
	if (m_thumbnails->GetItemCount() > 0)
		m_thumbnails->SetSelected(0);
	m_thumbnails->Update();
	m_thumbnails->SetFocus();
}

SingleThumbnailDlg::SingleThumbnailDlg(wxWindow *parent, const wxString& message, const wxString& caption,
		const wxArrayString& choices, std::vector<wxImage> images) {
	Create(parent, caption, message);

	for (unsigned int i = 0; i < choices.size(); i++) {
		wxThumb* thumb = new wxThumb(images[i], choices[i],  choices[i]);
		m_thumbnails->InsertItem(thumb);
	}
	if (m_thumbnails->GetItemCount() > 0)
		m_thumbnails->SetSelected(0);
	m_thumbnails->Update();
	m_thumbnails->SetFocus();
}

void SingleThumbnailDlg::Create(wxWindow* parent, const wxString& caption, const wxString& message) {
	wxDialog::Create(parent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	SetClientSize(wxSize(500, 380));
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	mainSizer->Add(new wxStaticText(this, wxID_ANY, message), 0, wxALL | wxALIGN_CENTER_VERTICAL, 8);
	m_thumbnails = new wxThumbnails(this, THUMBNAILS_ID, wxScrolledWindowStyle | wxDOUBLE_BORDER);
	mainSizer->Add(m_thumbnails, 1, wxLEFT | wxRIGHT | wxEXPAND, 8);
	wxStdDialogButtonSizer* stdDialogButtonSizer = new wxStdDialogButtonSizer();
	stdDialogButtonSizer->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	stdDialogButtonSizer->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	stdDialogButtonSizer->Realize();
	stdDialogButtonSizer->GetAffirmativeButton()->SetDefault();
	mainSizer->Add(stdDialogButtonSizer, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(mainSizer);
	Layout();
	Center();
}

SingleThumbnailDlg::~SingleThumbnailDlg() {
	// nothing to do
}

void SingleThumbnailDlg::SetSelection(int sel){
	m_thumbnails->SetSelected(sel);
}

int SingleThumbnailDlg::GetSelection() const{
	return m_thumbnails->GetSelected();
}

void SingleThumbnailDlg::OnThumbDoubleClick(wxCommandEvent& event) {
	EndModal(wxID_OK);
}

