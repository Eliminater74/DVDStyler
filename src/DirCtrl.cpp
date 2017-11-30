/////////////////////////////////////////////////////////////////////////////
// Name:        DirCtrl.h
// Purpose:     The control to display a directory listing as tree
// Author:      Alex Thuering
// Created:     25.10.2008
// RCS-ID:      $Id: DirCtrl.cpp,v 1.1 2008/10/25 21:00:20 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "DirCtrl.h"

enum {
	REFRESH_ID = 2300
};

BEGIN_EVENT_TABLE(DirCtrl, wxGenericDirCtrl)
	EVT_MENU(REFRESH_ID, DirCtrl::OnRefresh)
END_EVENT_TABLE()

DirCtrl::DirCtrl(wxWindow* parent, const wxWindowID id): wxGenericDirCtrl(parent, id, 
		wxDirDialogDefaultFolderStr, wxDefaultPosition, wxDefaultSize, wxDIRCTRL_DIR_ONLY) {
	m_contextMenu = new wxMenu;
	m_contextMenu->Append(REFRESH_ID, _("&Refresh") + wxString(wxT("\tF5")));
	GetTreeCtrl()->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(DirCtrl::OnMouseUp), NULL, this); 
	GetTreeCtrl()->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(DirCtrl::OnKeyDown), NULL, this);
}

DirCtrl::~DirCtrl() {
	// nothing to do
}

void DirCtrl::OnMouseUp(wxMouseEvent &event) {
	GetTreeCtrl()->PopupMenu(m_contextMenu, event.GetPosition());
}

void DirCtrl::OnKeyDown(wxKeyEvent& event) {
	wxCommandEvent evt;
	switch (event.GetKeyCode()) {
		case WXK_F5:
			OnRefresh(evt);
			break;
		default:
			event.Skip();
			break;
	}
}

void DirCtrl::OnRefresh(wxCommandEvent& event) {
	SetDefaultPath(GetPath());
	ReCreateTree();
}
