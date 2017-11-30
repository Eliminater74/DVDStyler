/////////////////////////////////////////////////////////////////////////////
// Name:        TitlePropDlg.h
// Purpose:     DVD title properties dialog
// Author:      Alex Thuering
// Created:     31.01.2004
// RCS-ID:      $Id: TitlePropDlg.h,v 1.23 2016/03/03 16:23:22 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
 
#ifndef TITLE_PROP_DLG_H
#define TITLE_PROP_DLG_H

#include <wx/wx.h>
#include <wxVillaLib/PropDlg.h>
#include <wxVillaLib/Thumbnails.h>
#include "DVD.h"
#include "VobListBox.h"

class TitlePropDlg: public wxPropDlg {
public:
	TitlePropDlg(wxWindow *parent, DVD* dvd, int tsi, int pgci, int vobi);
	virtual ~TitlePropDlg();

	/** Returns list of commands for choice */
	static wxArrayString GetCommandList(DVD* dvd, int tsi);
	/** Updates "Do not remultiplex"-checkbox */
	void UpdateDoNotTranscodeCheck();
	/** Updates chapters control */
	void UpdateChaptersCtrl();
	/** Update chapters from control */
	bool ApplyChaptersCtrl();

private:
	DVD* m_dvd;
	int m_tsi;
	int m_pgci;
	PgcArray* m_pgcs;
	Pgc* m_pgc;
	Vob* m_vob;
	int m_vobi;
	wxArrayString* m_langCodes;
	VobListBox* m_vobListBox;
	wxCheckBox* m_doNotTranscodeCheckBox;
	wxBitmapButton* m_addBt;
	wxBitmapButton* m_delBt;
	wxBitmapButton* m_propBt;
	wxTextCtrl* m_chaptersCtrl;
	wxButton* m_chaptersBt;
	wxSpinCtrl* m_pause;
	wxThumbnails* m_slideShowBox;
	wxArrayString m_transitions;
	wxChoice* m_audioFormat;
	void CreatePropPanel(wxSizer* sizer);
	void CreateVobGroup(wxSizer* sizer);
	void CreateSlideshowGroup(wxSizer* sizer);
	void CreateAudioGroup(wxSizer* sizer);
	void CreateTitleGroup(wxSizer* sizer);
	bool SetValues();
	void OnSelectItem(wxCommandEvent& event);
	void OnAddBt(wxCommandEvent& event);
	void OnRemoveBt(wxCommandEvent& event);
	void OnPropBt(wxCommandEvent& event);
    void OnThumbDoubleClick(wxCommandEvent& event);
	void OnChaptersBt(wxCommandEvent& event);
	void OnChangeAudio(wxCommandEvent& event);
	void OnDoNotTranscodeCheck(wxCommandEvent& event);
	DECLARE_EVENT_TABLE()
};

#endif // TITLE_PROP_DLG_H
