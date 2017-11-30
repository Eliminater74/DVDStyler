/////////////////////////////////////////////////////////////////////////////
// Name:        DirCtrl.cpp
// Purpose:     The control to display a directory listing as tree
// Author:      Alex Thuering
// Created:     25.10.2008
// RCS-ID:      $Id: DirCtrl.h,v 1.1 2008/10/25 21:00:20 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DVDSTYLER_DIRCTRL_H
#define DVDSTYLER_DIRCTRL_H

#include <wx/wx.h>
#include <wx/dirctrl.h>

class DirCtrl: public wxGenericDirCtrl {
public:
	DirCtrl(wxWindow* parent, const wxWindowID id = wxID_ANY);
	virtual ~DirCtrl();
private:
	wxMenu* m_contextMenu;
	void OnMouseUp(wxMouseEvent &event);
	void OnKeyDown(wxKeyEvent& event);
	void OnRefresh(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // DVDSTYLER_DIRCTRL_H
