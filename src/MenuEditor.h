/////////////////////////////////////////////////////////////////////////////
// Name:        MenuEditor.h
// Purpose:     The widget to edit a DVD Menu
// Author:      Alex Thuering
// Created:	11.10.2003
// RCS-ID:      $Id: MenuEditor.h,v 1.28 2014/08/27 17:11:14 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
 
#ifndef MENUEDITOR_H
#define MENUEDITOR_H

#include <wxSVG/svgctrl.h>
#include "DVD.h"
class TitlesetManager;
class MenuObject;

enum EditorAction {
	eaNONE, eaMOVE, eaRESIZE_TL, eaRESIZE_TR, eaRESIZE_BL, eaRESIZE_BR, eaROTATE, eaSELECT, eaSELECT_RECT
};

class MenuEditor: public wxSVGCtrl {
public:
    MenuEditor(wxWindow *parent, wxWindowID id = -1);
    
    void SetTitlesetManager(TitlesetManager* tm) { m_titlesetManager = tm; }
    TitlesetManager* GetTitlesetManager() { return m_titlesetManager; }
	
	void  SetMenu(DVD* dvd, Menu* menu, int tsi, int pgci);
	Menu* GetMenu() { return m_menu; }
	DVD*  GetDVD()	{ return m_dvd;	}
	int   GetTsi() { return m_tsi; }
	
	bool SetBackground(wxString fname);
	void SetBackgroundColour(wxColour colour);
	bool AddImage(wxString fname, int x = 0, int y = 0);
	bool AddText(wxString text, int x = 0, int y = 0);
    bool AddObject(wxString fname, int x = 0, int y = 0);
	bool AddButton(wxString fname, int x, int y, int pgci = 0, int tsi = -2, bool isMenu = false, int chapter = 0);
	
	wxString GetSelected() { return m_selectedIds.size() > 0 ? m_selectedIds[0] : wxT(""); }
	const wxArrayString& GetSelectedIds() { return m_selectedIds; }
    void Select(wxString id) { DoSelect(id, true, true); }
	
	wxString GetPointed() { return m_pointed; }
	void SetPointed(wxString id) { m_pointed = id; }
	
	int  GetGrid() { return m_grid; }
	void SetGrid(int value) { m_grid = value; }
	
	void ShowSafeTV();
	void ShowGrid();
    
    /** Causes a Menu-changed-event to be generated and repaints window */
    void Update(const wxRect* rect = NULL) { SendChangedEvent(); Refresh(true, rect); }
    void Update(const wxSVGRect* rect) { SendChangedEvent(); Refresh(rect); }
    /** Causes a Menu-changed-event to be generated and
      * redraws the contents of the given rectangle */
    void UpdateRect(const wxRect& rect) { Update(&rect); }
    void UpdateRect(const wxSVGRect& rect) { Update(&rect); }
    
    wxString HitTest(int x, int y);
    
private:
    TitlesetManager* m_titlesetManager;
	DVD*  m_dvd;
	Menu* m_menu;
	int m_tsi;
	int m_pgci;
	wxMenu* m_pmenu;
	wxMenu* m_objMenu;
	wxMenu* m_gridMenu;
	wxPoint m_menuPos;
	wxArrayString m_selectedIds;
	wxString m_pointed;
	bool m_safeTV;
	bool m_showGrid;
	bool m_gridOverObejcts;
	int m_grid;
	wxColour m_gridColour;
	wxArrayString m_addMenuObjects;
	
	EditorAction m_action;
	bool m_doAction;
	wxString m_actionObject;
	int m_oldx, m_oldy;
	bool m_doCopy;
	bool m_copied;
	EditorAction GetEditorAction(int x, int y);
	void SetMouseCursor(EditorAction tt);
	int SnapToGrid(int pos);
	bool MoveObject(int& x, int& y);
	bool MoveObjectInt(int& x, int& y);
	bool RotateObject(int x, int y);
	bool ResizeObject(int x, int y, EditorAction action = eaRESIZE_BR);
	bool ResizeObjectInt(int x, int y, EditorAction action = eaRESIZE_BR);
    
    void DoSelect(wxString id = wxT(""), bool refresh = false, bool sendEvent = false, bool ctrlDown = false);
    wxRect CreateSelection(wxString id);
    void RefreshSelected();
    
    wxRect UpdateSelectionRect(int x, int y);
    wxRect PerformSelection();
	
	void OnMouseLeftButton(wxMouseEvent &event);
    void OnMouseMove(wxMouseEvent &event);
	void OnMouseRightButton(wxMouseEvent &event);
	void OnMouseDClick(wxMouseEvent& event);
    
	void OnKeyDown(wxKeyEvent& event);
	
	void OnObjectCut(wxCommandEvent& event);
	void OnObjectCopy(wxCommandEvent& event);
	void OnObjectPaste(wxCommandEvent& event);
	void OnUpdateUIObjectPaste(wxUpdateUIEvent& event);
	void OnFirstButton(wxCommandEvent& event);
	void OnUpdateUIFirstButton(wxUpdateUIEvent& event);
	void OnObjectDelete(wxCommandEvent& event);
    void OnObjectToFront(wxCommandEvent& event);
    void OnObjectForward(wxCommandEvent& event);
    void OnObjectBackward(wxCommandEvent& event);
    void OnObjectToBack(wxCommandEvent& event);
    void OnUpdateUIObjectForward(wxUpdateUIEvent& event);
    void OnUpdateUIObjectBackward(wxUpdateUIEvent& event);
    void OnAlignToCenterMH(wxCommandEvent& event);
    void OnAlignToCenterMV(wxCommandEvent& event);
    void OnAlignToCenterH(wxCommandEvent& event);
    void OnAlignToCenterV(wxCommandEvent& event);
    void OnAlignToLeft(wxCommandEvent& event);
    void OnAlignToRight(wxCommandEvent& event);
    void OnAlignToTop(wxCommandEvent& event);
    void OnAlignToBottom(wxCommandEvent& event);
    void OnUpdateUIAlign(wxUpdateUIEvent& event);
	void OnProperties(wxCommandEvent& event);
	void OnUpdateUIProperties(wxUpdateUIEvent& event);
	void OnSafeTV(wxCommandEvent& event);
	void OnShowGrid(wxCommandEvent& event);
	void OnShowGridOverObjects(wxCommandEvent& event);
	void OnGridColour(wxCommandEvent& event);
	void OnGridMenu(wxCommandEvent& event);
	void OnAddObject(wxCommandEvent& event);
    
    void SendChangedEvent(bool updateAllItems = false);
    DECLARE_EVENT_TABLE()
};

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(EVT_COMMAND_MENUEDITOR_MENU_CHANGED, 3305)
  DECLARE_EVENT_TYPE(EVT_COMMAND_MENUEDITOR_OBJECT_POINTED, 3306)
END_DECLARE_EVENT_TYPES()

#define EVT_MENUEDITOR_MENU_CHANGED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_COMMAND_MENUEDITOR_MENU_CHANGED, id, wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),

#define EVT_MENUEDITOR_OBJECT_POINTED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_COMMAND_MENUEDITOR_OBJECT_POINTED, id,\
 wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),

#endif // MENUEDITOR_H
