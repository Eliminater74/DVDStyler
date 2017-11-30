/////////////////////////////////////////////////////////////////////////////
// Name:        MenuPropDlg.h
// Purpose:     DVD menu properties dialog
// Author:      Alex Thuering
// Created:     25.04.2004
// RCS-ID:      $Id: MenuPropDlg.h,v 1.15 2016/01/07 17:29:09 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
 
#ifndef MENU_PROP_DLG_H
#define MENU_PROP_DLG_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/combobox.h>
#include <wxVillaLib/PropDlg.h>
#include "DVD.h"
#include "Menu.h"

class MenuPropDlg: public wxPropDlg {
public:
    MenuPropDlg(wxWindow *parent, DVD* dvd, int tsi, int pgci);
	virtual ~MenuPropDlg();
  	
  	/** Returns list of commands for choice */
	wxArrayString GetCommandList();
	
protected:
	/** Creates properties panel */
	void CreatePropPanel(wxSizer* sizer);
	
	/** Creates panel with buttons */
	void CreateButtonPane(wxSizer* sizer, bool resetButton = false, bool dontShowCheckbox = false);
	
	/** Populate values on the model */
  	bool SetValues();
  	
private:
	DVD* m_dvd;
	Video* m_video;
	wxArrayString* m_langCodes;
	int m_tsi;
	int m_pgci;
    Pgc* m_pgc;
	Vob* m_vob;
	Menu* m_menu;
	vector<Cell*> m_cells;
	vector<DVDAction*> m_actions;
	wxChoice* m_aspect;
	wxChoice* m_wsFormat;
	wxChoice* m_audioFormat;
	wxSpinCtrl* m_pauseCtrl;
	wxComboBox* m_postCommandsCtrl;
	int m_subStartCtrlIdx;
	int m_audioCtrlIdx;
	int m_preCommandsCtrlIdx;
	void CreateButtonsGroup(wxSizer* sizer);
	void CreateVobGroup(wxSizer* sizer, bool loop);
	void CreateMenuGroup(wxSizer* sizer, bool loop);
	void OnChangeAspect(wxCommandEvent& event);
	void OnChangeAudio(wxCommandEvent& event);
	void OnLoopCheck(wxCommandEvent& event);
	void OnCellsBt(wxCommandEvent& event);
	void OnActionsBt(wxCommandEvent& event);
	void CreateAspectCtrls(wxSizer* sizer, wxSVG_PRESERVEASPECTRATIO align, wxSVG_MEETORSLICE meetOrSlice);

    DECLARE_EVENT_TABLE()
};

#endif // MENU_PROP_DLG_H
