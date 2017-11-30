/////////////////////////////////////////////////////////////////////////////
// Name:        AudioPropDlg.h
// Author:      Alex Thuering
// Created:     23.07.2011
// RCS-ID:      $Id: AudioPropDlg.h,v 1.10 2016/06/04 22:05:36 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef AUDIO_PROP_DLG_H
#define AUDIO_PROP_DLG_H

#include "DVD.h"

//(*Headers(AudioPropDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/choice.h>
#include <wx/bmpbuttn.h>
#include <wx/gbsizer.h>
#include <wx/dialog.h>
#include <wx/spinbutt.h>
//*)

/**
 * Audio properties dialog
 */
class AudioPropDlg: public wxDialog {
public:
	AudioPropDlg(wxWindow* parent,Vob* vob, const wxString& audioFile, const wxString& langCode, int streamIdx);
	virtual ~AudioPropDlg();
	
    /** Returns selected audio format */
    AudioFormat GetAudioFormat();
    
    /** Returns selected language code */
    wxString GetLangCode();
	
private:
	//(*Declarations(AudioPropDlg)
	wxTextCtrl* m_filtersCtrl;
	wxChoice* m_langChoice;
	wxStaticText* m_durText;
	wxSpinCtrl* m_volume1Ctrl;
	wxRadioButton* m_volChange1Bt;
	wxCheckBox* m_audio51;
	wxStaticText* m_gainText;
	wxTextCtrl* m_volume2Ctrl;
	wxSpinButton* m_offsetSpin;
	wxBitmapButton* m_calcGainBt;
	wxRadioButton* m_volNormalizeBt;
	wxTextCtrl* m_offsetCtrl;
	wxChoice* m_dstChoice;
	wxRadioButton* m_volChange2Bt;
	wxStaticText* m_fileNameText;
	wxSpinButton* m_volume2Spin;
	wxStaticText* m_srcText;
	wxRadioButton* m_volNoBt;
	//*)
	
	//(*Identifiers(AudioPropDlg)
	static const long ID_STATICTEXT2;
	static const long ID_STATICTEXT4;
	static const long ID_STATICTEXT6;
	static const long ID_DST_CHOICE;
	static const long ID_A51_CHECK;
	static const long ID_LANG_CHOICE;
	static const long ID_TEXTCTRL1;
	static const long ID_SPINBUTTON2;
	static const long ID_TEXTCTRL2;
	static const long ID_RADIOBUTTON1;
	static const long ID_RADIOBUTTON2;
	static const long ID_SPINCTRL1;
	static const long ID_RADIOBUTTON4;
	static const long ID_RADIOBUTTON3;
	static const long ID_TEXTCTRL3;
	static const long ID_SPINBUTTON3;
	static const long ID_CALC_GAIN_BT;
	static const long ID_STATICTEXT_GAIN;
	//*)
	
	//(*Handlers(AudioPropDlg)
	void OnChangeFormat(wxCommandEvent& event);
	void OnChangeVolume(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	void OnChangeOffset(wxCommandEvent& event);
	void OnOffsetSpin(wxSpinEvent& event);
	void OnSelectVolumeBt(wxCommandEvent& event);
	void OnVolume2SpinUp(wxSpinEvent& event);
	void OnVolume2SpinDown(wxSpinEvent& event);
	void OnCalculateGain(wxCommandEvent& event);
	//*)
	
    Vob* m_vob;
    wxString m_audioFile;
    wxString m_langCode;
    int m_streamIdx;
    Stream* m_stream;

	DECLARE_EVENT_TABLE()
};

#endif
