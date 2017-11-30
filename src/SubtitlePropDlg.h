/////////////////////////////////////////////////////////////////////////////
// Name:        SubtitlePropDlg.h
// Purpose:     The subtitle properties dialog
// Author:      Alex Thuering
// Created:     24.02.2010
// RCS-ID:      $Id: SubtitlePropDlg.h,v 1.10 2016/04/19 23:09:52 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef SUBTITLEPROPDLG_H
#define SUBTITLEPROPDLG_H

//(*Headers(SubtitlePropDlg)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/listbox.h>
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

#include "TextSub.h"
#include <wxVillaLib/PropDlg.h>

class SubtitlePropDlg: public wxDialog {
public:
    /** Constructor */
	SubtitlePropDlg(wxWindow* parent,TextSub* textsub, const wxString& langCode);
	virtual ~SubtitlePropDlg();

    /** Displays dialog */
    virtual int ShowModal();

    /** Returns selected language code */
    wxString GetLangCode();

	//(*Declarations(SubtitlePropDlg)
	wxStaticText* StaticText9;
	wxTextCtrl* m_previewText;
	wxChoice* m_langChoice;
	wxSpinCtrl* m_marginLeft;
	wxSpinCtrl* m_shadowOpacityCtrl;
	ColourPanel* m_shadowPanel;
	wxListBox* m_fontStyleBox;
	wxRadioButton* m_alignRadioTL;
	wxSpinCtrl* m_marginBottom;
	wxCheckBox* m_force;
	wxRadioButton* m_alignRadioBC;
	wxButton* m_useAsDefaultBt;
	wxButton* m_outlineBt;
	wxSpinCtrl* m_offsetYCtrl;
	ColourPanel* m_outlinePanel;
	wxListBox* m_fontSizeBox;
	wxRadioButton* m_alignRadioBL;
	ColourPanel* m_fillPanel;
	wxRadioButton* m_alignRadioCL;
	wxRadioButton* m_alignRadioBR;
	wxSpinCtrl* m_fillOpacityCtrl;
	wxSpinCtrl* m_fontSizeSpin;
	wxSpinCtrl* m_marginTop;
	wxSpinCtrl* m_outlineOpacityCtrl;
	wxRadioButton* m_alignRadioTC;
	wxButton* m_fillBt;
	wxSpinCtrl* m_marginRight;
	wxRadioButton* m_alignRadioCR;
	wxListBox* m_fontFamilyBox;
	wxTextCtrl* m_thicknessCtrl;
	wxChoice* m_charsetChoice;
	wxRadioButton* m_alignRadioTR;
	wxSpinCtrl* m_offsetXCtrl;
	wxButton* m_shadowBt;
	//*)

protected:

	//(*Identifiers(SubtitlePropDlg)
	static const long ID_CHOICE1;
	static const long ID_FORCE_CB;
	static const long ID_CHOICE2;
	static const long FONT_FAMILY_BOX_ID;
	static const long FONT_STYLE_BOX_ID;
	static const long FONT_SIZE_SPIN_ID;
	static const long FONT_SIZE_BOX_ID;
	static const long ID_STATICTEXT4;
	static const long ID_TEXTCTRL1;
	static const long ID_PANEL1;
	static const long FILL_BT_ID;
	static const long ID_SPINCTRL7;
	static const long ID_STATICTEXT10;
	static const long ID_PANEL2;
	static const long OUTLINE_BT_ID;
	static const long ID_SPINCTRL8;
	static const long ID_TEXTCTRL2;
	static const long ID_PANEL3;
	static const long SHADOW_BT_ID;
	static const long ID_SPINCTRL9;
	static const long ID_SPINCTRL1;
	static const long ID_SPINCTRL2;
	static const long ID_RADIOBUTTON1;
	static const long ID_RADIOBUTTON2;
	static const long ID_RADIOBUTTON3;
	static const long ID_RADIOBUTTON4;
	static const long ID_RADIOBUTTON6;
	static const long ID_RADIOBUTTON7;
	static const long ID_RADIOBUTTON8;
	static const long ID_RADIOBUTTON9;
	static const long ID_SPINCTRL3;
	static const long ID_SPINCTRL4;
	static const long ID_SPINCTRL5;
	static const long ID_SPINCTRL6;
	static const long ID_DEF_BT;
	static const long ID_BUTTON1;
	//*)

private:
    TextSub* m_textsub;
    wxString m_langCode;

	//(*Handlers(SubtitlePropDlg)
	void OnFontFamilySelected(wxCommandEvent& event);
	void OnFontStyleSelected(wxCommandEvent& event);
	void OnFontSizeChanged(wxSpinEvent& event);
	void OnFontSizeSelected(wxCommandEvent& event);
	void OnSelectFillColor(wxCommandEvent& event);
	void OnSelectOutlineColor(wxCommandEvent& event);
	void OnSelectShadowColor(wxCommandEvent& event);
	void OnUseAsDefaults(wxCommandEvent& event);
	void OnResetBt(wxCommandEvent& event);
	//*)
    virtual void OnFontChanged();

    wxColour GetFillColour();
    void SetFillColour(const wxColour& value);

    wxColour GetOutlineColour();
    void SetOutlineColour(const wxColour& value);

    wxColour GetShadowColour();
    void SetShadowColour(const wxColour& value);

	DECLARE_EVENT_TABLE()
};

#endif
