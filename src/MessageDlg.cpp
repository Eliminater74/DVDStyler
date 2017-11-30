/////////////////////////////////////////////////////////////////////////////
// Name:        MessageDlg.h
// Purpose:     Message dialog with "Don't show again" checkbox
// Author:      Alex Thuering
// Created:     26.03.2009
// RCS-ID:      $Id: MessageDlg.cpp,v 1.3 2013/10/25 21:19:16 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
 
#ifndef MESSAGE_DLG_H
#define MESSAGE_DLG_H

#include "MessageDlg.h"
#include <wx/artprov.h>

BEGIN_EVENT_TABLE(MessageDlg, wxDialog)
	EVT_BUTTON(wxID_YES, MessageDlg::OnYes)
	EVT_BUTTON(wxID_NO, MessageDlg::OnNo)
	EVT_BUTTON(wxID_CANCEL, MessageDlg::OnCancel)
END_EVENT_TABLE()

MessageDlg::MessageDlg(wxWindow *parent, const wxString& message, const wxString& caption, long style) {
	m_parent = parent ? wxGetTopLevelParent(parent) : wxTheApp->GetTopWindow();
	m_message = message;
	m_caption = caption;
	m_dialogStyle = style;
	if ((style & wxYES_NO) && !(style & wxCANCEL))
		SetEscapeId(wxID_NO);
	DoCreateMsgdialog();
}

void MessageDlg::DoCreateMsgdialog() {
	wxDialog::Create(m_parent, wxID_ANY, m_caption, wxDefaultPosition,
			wxDefaultSize, wxDEFAULT_DIALOG_STYLE);
	bool is_pda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);
	wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *icon_text = new wxBoxSizer(wxHORIZONTAL);

	// 1) icon
	if (m_dialogStyle & wxICON_MASK) {
		wxBitmap bitmap;
		switch (m_dialogStyle & wxICON_MASK) {
		default:
			wxFAIL_MSG(_T("incorrect log style"));
			bitmap = wxArtProvider::GetIcon(wxART_ERROR, wxART_MESSAGE_BOX);
			break;

		case wxICON_ERROR:
			bitmap = wxArtProvider::GetIcon(wxART_ERROR, wxART_MESSAGE_BOX);
			break;

		case wxICON_INFORMATION:
			bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_MESSAGE_BOX);
			break;

		case wxICON_WARNING:
			bitmap = wxArtProvider::GetIcon(wxART_WARNING, wxART_MESSAGE_BOX);
			break;

		case wxICON_QUESTION:
			bitmap = wxArtProvider::GetIcon(wxART_QUESTION, wxART_MESSAGE_BOX);
			break;
		}
		wxStaticBitmap *icon = new wxStaticBitmap(this, wxID_ANY, bitmap);
		if (is_pda)
		topsizer->Add( icon, 0, wxTOP|wxLEFT|wxRIGHT | wxALIGN_LEFT, 10 );
		else
		icon_text->Add( icon, 0, wxCENTER );
	}

	// 2) text
	icon_text->Add( CreateTextSizer(m_message), 0, wxALIGN_CENTER | wxLEFT, 10 );
	topsizer->Add( icon_text, 1, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
	
	m_checkBox = new wxCheckBox(this, wxID_ANY, _("&Don't show this message again"));
	topsizer->Add(m_checkBox, 0, wxTOP|wxLEFT, 12);
	
	// 3) buttons
	int center_flag = wxEXPAND;
	wxSizer *sizerBtn = CreateButtonSizer(m_dialogStyle & (wxOK|wxCANCEL|wxYES_NO|wxNO_DEFAULT));
	if ( sizerBtn )
		topsizer->Add(sizerBtn, 0, center_flag | wxALL, 10 );
	
	SetAutoLayout( true );
	SetSizer( topsizer );
	
	topsizer->SetSizeHints( this );
	topsizer->Fit( this );
	wxSize size( GetSize() );
	if (size.x < size.y*3/2) {
		size.x = size.y*3/2;
		SetSize( size );
	}
	
	Centre(wxBOTH | wxCENTER_FRAME);
}


int MessageDlg::ShowModal() {
	int ret = wxDialog::ShowModal();
	switch (ret) {
	case wxID_OK:
		return wxOK;
	case wxID_YES:
		return wxYES;
	case wxID_NO:
		return wxNO;
	default:
		return wxCANCEL;
	}
}

bool MessageDlg::IsShowAgain() {
	return !m_checkBox->GetValue();
}

void MessageDlg::OnYes(wxCommandEvent& event) {
	EndModal(wxID_YES);
}

void MessageDlg::OnNo(wxCommandEvent& event) {
	EndModal(wxID_NO);
}

void MessageDlg::OnCancel(wxCommandEvent& event) {
	EndModal(wxID_CANCEL);
}

#endif // MESSAGE_DLG_H
