/////////////////////////////////////////////////////////////////////////////
// Name:        MenuCellsDlg.h
// Purpose:     The menu cells dialog
// Author:      Alex Thuering
// Created:     10.01.2012
// RCS-ID:      $Id: MenuCellsDlg.h,v 1.5 2016/01/06 21:10:04 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef MENUCELLS_DLG_H
#define MENUCELLS_DLG_H

#include "DVD.h"

//(*Headers(MenuCellsDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/grid.h>
#include <wx/bmpbuttn.h>
#include <wx/dialog.h>
//*)
class MenuPropDlg;

class MenuCellsDlg: public wxDialog {
public:
	/** Costructor */
	MenuCellsDlg(MenuPropDlg* parent, DVD* dvd, int tsi, int pgci, vector<Cell*>& cells);
	virtual ~MenuCellsDlg();

	vector<Cell*>& GetCells() { return m_cells; }
	
private:
	MenuPropDlg* parentDlg;
	DVD* m_dvd;
	int m_tsi;
	int m_pgci;
    vector<Cell*> m_cells;
	
	//(*Declarations(MenuCellsDlg)
	wxBitmapButton* m_delBt;
	wxGrid* m_grid;
	wxBitmapButton* m_addBt;
	//*)

	//(*Identifiers(MenuCellsDlg)
	static const long ID_STATICTEXT1;
	static const long ID_GRID;
	static const long ID_ADD_BT;
	static const long ID_DEL_BT;
	//*)

	//(*Handlers(MenuCellsDlg)
	void OnAddBt(wxCommandEvent& event);
	void OnDelBtClick(wxCommandEvent& event);
	//*)
	void SetCellEditors(int row);
	void OnOkBt(wxCommandEvent& event);
	
	DECLARE_EVENT_TABLE()
};

#endif
