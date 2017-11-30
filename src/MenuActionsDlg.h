/////////////////////////////////////////////////////////////////////////////
// Name:        MenuActionsDlg.h
// Author:      Alex Thuering
// Created:     13.01.2012
// RCS-ID:      $Id: MenuActionsDlg.h,v 1.4 2016/01/06 21:10:04 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
#ifndef MENUACTIONSDLG_H
#define MENUACTIONSDLG_H

#include "DVD.h"

//(*Headers(MenuActionsDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/grid.h>
#include <wx/bmpbuttn.h>
#include <wx/dialog.h>
//*)
class MenuPropDlg;

/**
 * Menu actions dialog
 */
class MenuActionsDlg: public wxDialog {
public:
	/** Constructor */
	MenuActionsDlg(MenuPropDlg* parent, DVD* dvd, int tsi, int pgci, vector<DVDAction*> actions);
	virtual ~MenuActionsDlg();
	
	vector<DVDAction*>& GetActions() { return m_actions; }

private:
	MenuPropDlg* parentDlg;
	DVD* m_dvd;
	int m_tsi;
	int m_pgci;
	vector<DVDAction*> m_actions;
	
	//(*Declarations(MenuActionsDlg)
	wxBitmapButton* m_delBt;
	wxGrid* m_grid;
	wxBitmapButton* m_addBt;
	//*)

	//(*Identifiers(MenuActionsDlg)
	static const long ID_GRID;
	static const long ID_ADD_BT;
	static const long ID_DEL_BT;
	//*)

	//(*Handlers(MenuActionsDlg)
	void OnAddBt(wxCommandEvent& event);
	void OnDelBt(wxCommandEvent& event);
	//*)
	void OnOkBt(wxCommandEvent& event);
	void SetCellEditors(int row);

	DECLARE_EVENT_TABLE()
};

#endif
