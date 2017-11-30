/////////////////////////////////////////////////////////////////////////////
// Name:        ChaptersDlg.h
// Purpose:     The chapters dialog
// Author:      Alex Thuering
// Created:     19.04.2010
// RCS-ID:      $Id: ChaptersDlg.h,v 1.6 2015/10/03 13:29:57 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
#ifndef CHAPTERS_DLG_H
#define CHAPTERS_DLG_H

#include "Pgc.h"
#include "mediactrl_ffmpeg.h"
#include <wxSVG/mediadec_ffmpeg.h>

//(*Headers(ChaptersDlg)
#include "wxVillaLib/Thumbnails.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/splitter.h>
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/bmpbuttn.h>
#include <wx/dialog.h>
#include <wx/spinbutt.h>
#include <wx/combobox.h>
//*)

class ChaptersDlg: public wxDialog {
public:
	/** Constructor */
	ChaptersDlg(wxWindow* parent, DVD* dvd, int tsi, int pgci, int vobi, Vob* vob);
	/** Destructor */
	virtual ~ChaptersDlg();

	//(*Declarations(ChaptersDlg)
	wxSpinCtrl* m_pauseSpin;
	wxThumbnails* m_thumbnails;
	wxRadioButton* m_regularBt;
	wxSlider* m_slider;
	wxStaticText* StaticText2;
	wxSpinButton* m_frameSpinBt;
	wxBitmapButton* m_delBt;
	wxComboBox* m_commandsCtrl;
	MediaCtrlFF* m_mediaCtrl;
	wxTextCtrl* m_timeCtrl;
	wxCheckBox* m_addCellAtBegin;
	wxBitmapButton* m_addBt;
	wxSpinButton* m_endSpinBt;
	wxTextCtrl* m_endCtrl;
	wxRadioButton* m_programBt;
	wxRadioButton* m_chapterBt;
	wxSpinButton* m_endFrameSpinBt;
	wxChoice* m_endChoice;
	wxSpinButton* m_timeSpinBt;
	//*)

private:
	//(*Identifiers(ChaptersDlg)
	static const long ID_THUMBNAILS;
	static const long ID_ADD_BT;
	static const long ID_DEL_BT;
	static const long ID_PANEL1;
	static const long ID_MEDIA_CTRL;
	static const long ID_SLIDER;
	static const long ID_TIME_CTRL;
	static const long ID_TIME_SPINB;
	static const long ID_FRAME_SPINBT;
	static const long ID_RADIOBUTTON1;
	static const long ID_RADIOBUTTON2;
	static const long ID_RADIOBUTTON3;
	static const long ID_STATICTEXT1;
	static const long ID_CHOICE1;
	static const long ID_END_CTRL;
	static const long ID_END_SPIN;
	static const long ID_END_FRAME_SPINBT;
	static const long ID_DURATION_CTRL;
	static const long ID_COMMANDS_CTRL;
	static const long ID_PANEL2;
	static const long ID_SPLITTERWINDOW;
	static const long ID_ADD_CELL_CHECK;
	//*)

	DVD* m_dvd;
	int m_tsi;
	int m_pgci;
	int m_vobi;
	Vob* m_vob;
	vector<Cell*> m_cells;
	Cell* m_selectedCell;
	wxImage LoadFrame(long pos);
	void AddThumbnail(Cell* cell, int pos = -1);
	void SeekVideo(long pos, bool updateTimeCtrl = true);
	/** Checks if cell values are valid and update model */
	bool CheckCellValues();

	//(*Handlers(ChaptersDlg)
	void OnAddBt(wxCommandEvent& event);
	void OnDelBt(wxCommandEvent& event);
	void OnChangeTime(wxCommandEvent& event);
	void OnTimeSpin(wxSpinEvent& event);
	void OnSliderScroll(wxScrollEvent& event);
	void OnEndChoice(wxCommandEvent& event);
	void OnChangeEnd(wxCommandEvent& event);
	void OnEndSpin(wxSpinEvent& event);
	void OnFrameSpin(wxSpinEvent& event);
	void OnEndFrameSpin(wxSpinEvent& event);
	void OnFrameSpinDown(wxSpinEvent& event);
	void OnEndFrameSpinDown(wxSpinEvent& event);
	//*)
	void OnSelectionChanged(wxCommandEvent& event);
	void OnKeyDown(wxKeyEvent& event) ;
	void OnOkBt(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif // CHAPTERS_DLG_H
