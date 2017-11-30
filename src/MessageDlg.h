/////////////////////////////////////////////////////////////////////////////
// Name:        MessageDlg.h
// Purpose:     Message dialog with "Don't show again" checkbox
// Author:      Alex Thuering
// Created:     26.03.2009
// RCS-ID:      $Id: MessageDlg.h,v 1.2 2013/10/25 21:19:16 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
 
#ifndef DS_MESSAGE_DLG_H
#define DS_MESSAGE_DLG_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>

class MessageDlg: public wxDialog {
public:
	MessageDlg(wxWindow *parent, const wxString& message, const wxString& caption, long style = wxOK|wxCENTRE);
	int ShowModal();
	bool IsShowAgain();
	
private:
	wxWindow* m_parent;
	wxString m_message;
	wxString m_caption;
	long m_dialogStyle;
	wxCheckBox* m_checkBox;
	void DoCreateMsgdialog();
	void OnYes(wxCommandEvent& event);
	void OnNo(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

int MessageBox(const wxString& message, const wxString& caption, long style = wxOK | wxCENTRE,
		wxWindow *parent = NULL);


#endif // DS_MESSAGE_DLG_H
