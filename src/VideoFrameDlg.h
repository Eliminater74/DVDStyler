/////////////////////////////////////////////////////////////////////////////
// Name:        VideoFrameDlg.h
// Purpose:     The video frame dialog
// Author:      Alex Thuering
// Created:     23.12.2011
// RCS-ID:      $Id: VideoFrameDlg.h,v 1.6 2013/06/30 13:24:47 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef VIDEOFRAME_DLG_H
#define VIDEOFRAME_DLG_H

//(*Headers(VideoFrameDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/slider.h>
#include <wx/bmpbuttn.h>
#include <wx/dialog.h>
#include <wx/spinbutt.h>
//*)
#include "mediactrl_ffmpeg.h"

class VideoFrameDlg: public wxDialog {
public:
	VideoFrameDlg(wxWindow* parent, wxString fileName, bool custom, long defaultPos, long pos, int duration);
	virtual ~VideoFrameDlg();
	long GetPos() { return m_pos; }
	int GetDuration() { return m_durationCtrl->GetValue(); }

private:
	//(*Declarations(VideoFrameDlg)
	wxBitmapButton* m_resetTimeBt;
	wxSlider* m_slider;
	wxSpinButton* m_frameSpinBt;
	wxSpinCtrl* m_durationCtrl;
	MediaCtrlFF* m_mediaCtrl;
	wxTextCtrl* m_timeCtrl;
	wxSpinButton* m_timeSpinBt;
	//*)

	//(*Identifiers(VideoFrameDlg)
	static const long ID_MEDIA_CTRL;
	static const long ID_SLIDER;
	static const long ID_TIME_CTRL;
	static const long ID_TIME_SPINB;
	static const long ID_FRAME_SPINBT;
	static const long ID_RESET_TIME_BT;
	static const long ID_DURATION_CTRL;
	//*)

	//(*Handlers(VideoFrameDlg)
	void OnSliderScroll(wxScrollEvent& event);
	void OnTimeSpin(wxSpinEvent& event);
	void OnChangeTime(wxCommandEvent& event);
	void OnResetBt(wxCommandEvent& event);
	void OnFrameSpin(wxSpinEvent& event);
	void OnFrameSpinDown(wxSpinEvent& event);
	//*)
	
	long m_pos;
	long m_defaultPos;
	void SeekVideo(long pos, bool updateTimeCtrl = true);
	DECLARE_EVENT_TABLE()
};

#endif
