/////////////////////////////////////////////////////////////////////////////
// Name:        WelcomeDlg.h
// Purpose:     Welcome dialog
// Author:      Alex Thuering
// Created:     23.03.2009
// RCS-ID:      $Id: WelcomeDlg.h,v 1.1 2009/03/29 19:33:17 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef WELCOME_DLG_H
#define WELCOME_DLG_H

#include "NewProjectDlg.h"

class WelcomeDlg: public NewProjectDlg {
public:
	WelcomeDlg(wxWindow *parent);
	virtual ~WelcomeDlg() {}
	
	bool IsNewProject();
	wxString GetOpenFilename();
	
protected:
	virtual void CreatePropPanel(wxSizer* sizer);
	virtual bool SetValues();
	
private:
	wxRadioButton* m_newProjectBt;
	wxRadioButton* m_openProjectBt;
	wxListBox* m_listBox;
	void OnChooseNew(wxCommandEvent& event);
	void OnChooseOpen(wxCommandEvent& event);
	void OnSetFocus(wxChildFocusEvent& event);
	void OnListBoxDClick(wxCommandEvent& event);
	
    DECLARE_EVENT_TABLE()
};

#endif // WELCOME_DLG_H
