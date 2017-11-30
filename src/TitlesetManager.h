/////////////////////////////////////////////////////////////////////////////
// Name:        TitlesetManager.h
// Purpose:     Titleset Manager
// Author:      Alex Thuering
// Created:	27.01.2003
// RCS-ID:      $Id: TitlesetManager.h,v 1.26 2016/05/01 12:45:18 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef TITLE_THUMBNAILS_H
#define TITLE_THUMBNAILS_H

#include <wxVillaLib/Thumbnails.h>
#include "DVD.h"
class wxProgressDialog;

class TitlesetManager: public wxThumbnails {
  public:
    TitlesetManager(wxWindow* parent, int id);
    ~TitlesetManager();
	
	void SetDVD(DVD* dvd);
	inline DVD* GetDVD() { return m_dvd; }
	inline int GetSelectedMenu() { return m_selectedMenu; }
	int GetPointedId() { return GetItem(GetPointed())->GetId(); }
	int GetSelectedId() { return GetItem(GetSelected())->GetId(); }
	int GetSelectedMenuId() { return GetSelectedMenu() >= 0 ? GetItem(GetSelectedMenu())->GetId() : -1; }
	
	bool AddVideo(const wxString& fname, bool createTitle = true, int tsi = -1, bool allowMenuDuplication = true);
    bool AddAudio(const wxString& fname);
    bool AddSubtitles(const wxString& fname);
    void AddImage(const wxString& fname, bool createTitle = true, bool update = true);
	void SelectFirstMenu();
	void SelectMenu(int tsi, int pgci);
	
	void AddMenu();
	void AddVmMenu();
	void AddTitleset();
	bool DuplicateMenu(int tsi, int pgci, int targetTsi);
	bool CreateChapterMenu(int targetTsi, int chapterCount);
	void MoveSelectedTo(int idx);
    
    /** updates items to display current content of DVD */
    void UpdateItems(wxProgressDialog* progressDlg = NULL);
    /** Checks if video fit on DVD without re-encoding */
	bool AskForReencoding(VideoFormat videoFormat);

  private:
	DVD* m_dvd;
	int m_selectedMenu;
	bool m_reEncodingByDefault;
	wxProgressDialog* m_progressDlg;
	wxBitmap GetThumbImage(wxThumb& thumb);
    void DrawThumbnail(wxBitmap& bmp, wxThumb& thumb, int index);
    void Paint(wxPaintDC& dc);
    int GetRowHeight(unsigned int begRow, unsigned int count);
	/** returns index of thumbnail for specified vob */
	int GetIndex(int tsi, int pgci, int vobi, bool menu);
	int GetIndex(int id);
	bool IsBeginOfTitleset(int tsi, int pgci, int vobi, bool isMenu);
	bool IsEndOfTitleset(int tsi, int pgci, int vobi, bool isMenu);
    void SendMenuSelectedEvent();
	void SendDvdChangedEvent();
	
	wxMenu* m_mainMenu;
	wxMenu* m_titleMenu;
	void OnMouseUp(wxMouseEvent& event);
	void OnMouseDClick(wxMouseEvent& event);
	void OnAddTitlesetUpdateUI(wxUpdateUIEvent& event);
	void OnMoveTitleLeft(wxCommandEvent& WXUNUSED(event));
	void OnMoveTitleRight(wxCommandEvent& WXUNUSED(event));
	void OnMoveTitleLeftUpdateUI(wxUpdateUIEvent& event);
	void OnMoveTitleRightUpdateUI(wxUpdateUIEvent& event);
	void OnDelete(wxCommandEvent& WXUNUSED(event));
	void OnProps(wxCommandEvent& WXUNUSED(event));
	void OnCopy(wxCommandEvent& WXUNUSED(event));
	void OnPaste(wxCommandEvent& WXUNUSED(event));
	void OnUpdateUIObjectPaste(wxUpdateUIEvent& event);
	void OnMouseMove(wxMouseEvent &event);
	void FixVobChapters(Pgc* pgc, int vobi, int vobi2);

    DECLARE_EVENT_TABLE()
};

BEGIN_DECLARE_EVENT_TYPES()
  DECLARE_EVENT_TYPE(EVT_COMMAND_MENU_SELECTED, 3300)
  DECLARE_EVENT_TYPE(EVT_COMMAND_DVD_CHANGED, 3301)
END_DECLARE_EVENT_TYPES()

#define EVT_MENU_SELECTED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_COMMAND_MENU_SELECTED, id, wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),
#define EVT_DVD_CHANGED(id, fn)\
 DECLARE_EVENT_TABLE_ENTRY(EVT_COMMAND_DVD_CHANGED, id, wxID_ANY,\
 (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)& fn, NULL),

#endif // TITLE_THUMBNAILS_H
