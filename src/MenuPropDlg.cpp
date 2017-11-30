/////////////////////////////////////////////////////////////////////////////
// Name:        MenuPropDlg.cpp
// Purpose:     DVD menu properties dialog
// Author:      Alex Thuering
// Created:     25.04.2004
// RCS-ID:      $Id: MenuPropDlg.cpp,v 1.52 2017/01/02 10:46:47 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "MenuPropDlg.h"
#include "TitlesetManager.h"
#include "Config.h"
#include "MenuCellsDlg.h"
#include "MenuActionsDlg.h"
#include <wxVillaLib/utils.h>
#include <wx/file.h>
#include "rc/preferences.png.h"

enum {
	ASPECT_CHOICE_ID = 7850,
	AUDIO_CTRL_ID,
	LOOP_CB_ID,
	CELLS_BT_ID,
	ACTIONS_BT_ID
};

BEGIN_EVENT_TABLE(MenuPropDlg, wxPropDlg)
	EVT_CHOICE(ASPECT_CHOICE_ID, MenuPropDlg::OnChangeAspect)
	EVT_TEXT(AUDIO_CTRL_ID, MenuPropDlg::OnChangeAudio)
	EVT_CHECKBOX(LOOP_CB_ID, MenuPropDlg::OnLoopCheck)
	EVT_BUTTON(CELLS_BT_ID, MenuPropDlg::OnCellsBt)
	EVT_BUTTON(ACTIONS_BT_ID, MenuPropDlg::OnActionsBt)
END_EVENT_TABLE()

MenuPropDlg::MenuPropDlg(wxWindow *parent, DVD* dvd, int tsi, int pgci):
		wxPropDlg(parent), m_dvd(dvd), m_tsi(tsi), m_pgci(pgci), m_aspect(NULL), m_wsFormat(NULL), m_audioFormat(NULL),
		m_pauseCtrl(NULL), m_postCommandsCtrl(NULL), m_subStartCtrlIdx(0), m_audioCtrlIdx(0), m_preCommandsCtrlIdx(0) {
	PgcArray& pgcs = m_dvd->GetPgcArray(tsi, true);
	m_video = &pgcs.GetVideo();
	m_langCodes = &pgcs.GetAudioLangCodes();
	m_pgc = pgcs[pgci];
	m_vob = m_dvd->GetMenuVob(tsi, pgci);
	m_menu = m_vob->GetMenu();
	VECTOR_COPY(m_vob->GetCells(), m_cells, Cell)
	VECTOR_COPY(m_menu->GetActions(), m_actions, DVDAction)
	m_audioFormat = NULL;
	// set title
	wxString title = _("Properties") + wxString(wxT(" - "));
	if (tsi >= 0) {
		if (dvd->GetTitlesets().Count() > 1)
			title += _("Titleset") + wxString::Format(wxT(" %d "), tsi+1);
		title += _("Menu") + wxString::Format(wxT(" %d"), pgci+1);
	} else
		title += _("VMGM menu") + wxString::Format(wxT(" %d"), pgci+1);
	SetTitle(title);
	// create
	Create();
	SetSize(400,-1);
}


MenuPropDlg::~MenuPropDlg() {
	VECTOR_CLEAR(m_cells, Cell)
	VECTOR_CLEAR(m_actions, DVDAction)
}

/**
 * Creates properties panel.
 */
void MenuPropDlg::CreatePropPanel(wxSizer* sizer) {
	bool loop = m_vob->GetPause() == 0 && m_pgc->GetPostCommands() == wxT("jump cell 1;");
	CreateVobGroup(sizer, loop);
	CreateButtonsGroup(sizer);
	CreateMenuGroup(sizer, loop);
}

void MenuPropDlg::CreateAspectCtrls(wxSizer* sizer, wxSVG_PRESERVEASPECTRATIO align,
		wxSVG_MEETORSLICE meetOrSlice) {
	bool keepAspectRatio = align > wxSVG_PRESERVEASPECTRATIO_NONE;
	BeginCheckGroup(sizer, _("Keep Aspect Ratio"), keepAspectRatio);
	sizer->AddSpacer(4);
	wxArrayString alignXStrings;
	alignXStrings.Add(_("X Min"));
	alignXStrings.Add(_("X Mid"));
	alignXStrings.Add(_("X Max"));
	int alignX = keepAspectRatio ? (align - wxSVG_PRESERVEASPECTRATIO_XMINYMIN) % 3 : 1;
	sizer->Add(AddChoiceProp(alignXStrings[alignX], alignXStrings));
	wxArrayString alignYStrings;
	alignYStrings.Add(_("Y Min"));
	alignYStrings.Add(_("Y Mid"));
	alignYStrings.Add(_("Y Max"));
	int alignY = keepAspectRatio ? (align - wxSVG_PRESERVEASPECTRATIO_XMINYMIN) / 3 : 1;
	sizer->Add(AddChoiceProp(alignYStrings[alignY], alignYStrings), 0, wxLEFT, 4);
	wxArrayString meetStrings;
	meetStrings.Add(_("Meet"));
	meetStrings.Add(_("Slice"));
	int meet = meetOrSlice == wxSVG_MEETORSLICE_SLICE ? 1 : 0;
	sizer->Add(AddChoiceProp(meetStrings[meet], meetStrings), 0, wxLEFT, 4);
	sizer->AddStretchSpacer(1);
	EndGroup();
}

void MenuPropDlg::CreateVobGroup(wxSizer* sizer, bool loop) {
	wxSizer* grpSizer = BeginGroup(sizer, _("Video object"));
	wxFlexGridSizer* grid = new wxFlexGridSizer(2, 4, 6);
	grid->AddGrowableCol(1);
	
	// format
	AddText(grid, _("Format:"));
	wxSizer* formatSizer = new wxBoxSizer(wxHORIZONTAL);
	wxArrayString formats = DVD::GetVideoFormatLabels(false, false, true);
	int vf = m_menu->GetVideoFormat() >= 2 ? m_menu->GetVideoFormat() - 2 : 0;
	formatSizer->Add(AddChoiceProp(formats[vf], formats), 0, wxEXPAND);
	formatSizer->AddSpacer(4);
	formats = DVD::GetAspectRatioLabels();
	int ar = m_menu->GetAspectRatio() >= 1 ? m_menu->GetAspectRatio() - 1 : 0;
	formatSizer->Add(AddChoiceProp(formats[ar], formats, ASPECT_CHOICE_ID), 0, wxEXPAND);
	m_aspect = (wxChoice*) GetLastControl();
	formatSizer->AddSpacer(4);
	wxArrayString wsFormats = m_video->GetWidescreenStrings(_("auto")); 
	formatSizer->Add(AddChoiceProp(wsFormats[m_video->GetWidescreen()], wsFormats), 0, wxEXPAND);
	m_wsFormat = (wxChoice*) GetLastControl();
	formatSizer->AddStretchSpacer(1);
	grid->Add(formatSizer, 0, wxEXPAND);
	wxCommandEvent evt;
	OnChangeAspect(evt);
	
	// Background
	wxString video = wxThumbnails::GetVideoExtWildcard();
	wxString imageExt = wxImage::GetImageExtWildcard();
	AddFileProp(grid, _("Background:"), m_menu->GetBackground(), wxFD_OPEN, wxT("..."),
			_("Image and Video Files") + wxString::Format(wxT("|%s%s|"), video.c_str(), imageExt.AfterLast(wxT('|')).c_str())
			+ _("Video Files") + wxString::Format(wxT(" (%s)|%s|"), video.c_str(), video.c_str())
			+ _("Image Files ") + imageExt
			+ wxT("|") + wxString(_("All Files")) + wxT(" (*.*)|*.*"));
	// Keep aspect ratio
	grid->AddSpacer(8);
	wxSizer* cbSizer = new wxBoxSizer(wxHORIZONTAL);
	CreateAspectCtrls(cbSizer, m_menu->GetBackgroundAlign(), m_menu->GetBackgroundMeetOrSlice());
	grid->Add(cbSizer, 0, wxEXPAND|wxALL, 4);
	// Colour
	AddColourProp(grid, wxT(" "), m_menu->GetBackgroundColour());
	
	// audio file
	wxString audioFile = m_vob->GetAudioFilenames().size() ? m_vob->GetAudioFilenames()[0] : wxT("");
	wxString audio = wxThumbnails::GetAudioExtWildcard();
	AddFileProp(grid, _("Audio:"), audioFile, wxFD_OPEN, wxT("..."),
			_("Audio Files") + wxString::Format(wxT(" (%s)|%s|"), audio.c_str(), audio.c_str())
			+ _("All Files") + wxString(wxT(" (*.*)|*.*")), AUDIO_CTRL_ID);
	m_audioCtrlIdx = GetLastControlIndex();
	// audio format
	grid->AddSpacer(8);
	Stream* stream = audioFile.length() > 0 ? m_vob->GetStreams()[m_vob->GetStreams().size() - 1] : NULL;
	int af = stream != NULL ? stream->GetAudioFormat() - 1 : afCOPY - 1;
	formats = DVD::GetAudioFormatLabels(true);
	wxSizer* audioFormatSizer = AddChoiceProp(grid, wxT(""), formats[af], formats, 0, false);
	m_audioFormat = (wxChoice*) GetLastControl();
	m_audioFormat->Enable(audioFile.length() > 0);
	wxArrayString labels = DVD::GetAudioLanguageCodes();
	if (m_langCodes->GetCount() < 1)
		m_langCodes->Add(s_config.GetDefAudioLanguage());
	audioFormatSizer->AddSpacer(4);
	AddChoiceProp(audioFormatSizer, wxT(""), (*m_langCodes)[0], labels, 0, false);
	// pause and loop
	wxSizer* pSizer = AddSpinProp(grid, _("Pause:"), m_vob->GetPause(), -1, 254, 100, _("sec"),false);
	m_pauseCtrl = (wxSpinCtrl*) GetLastControl();
	m_pauseCtrl->Enable(!loop);
	pSizer->AddSpacer(8);
	AddCheckProp(pSizer, _("Loop"), loop, false, LOOP_CB_ID);
	
	grpSizer->Add(grid, 0, wxEXPAND|wxALL, 4);
	sizer->AddSpacer(6);
}

void MenuPropDlg::CreateButtonsGroup(wxSizer* sizer) {
	wxSizer* grpSizer = BeginGroup(sizer, _("Buttons"));
	wxFlexGridSizer* grid = new wxFlexGridSizer(2, 4, 6);
	grid->AddGrowableCol(1);
	// Start/End
	wxSizer* timeSizer = AddTextProp(grid, _("Subpicture start/end:"), m_menu->GetStartTime(), false, 100, false);
	m_subStartCtrlIdx = GetLastControlIndex();
	AddTextProp(timeSizer, wxT(" - "), m_menu->GetEndTime(), false, 100);
	grpSizer->Add(grid, 0, wxEXPAND|wxALL, 4);
	// Remember last selected button
	wxSizer* cbSizer = new wxBoxSizer(wxHORIZONTAL);
	BeginCheckGroup(cbSizer, _("Remember last selected button"), m_menu->GetRememberLastButton());
	cbSizer->AddSpacer(4);
	wxArrayString registers;
	registers.Add(_("Auto"));
	for (int i=0; i<13; i++)
		registers.Add(wxString::Format(wxT("g%d"), i));
	cbSizer->Add(AddChoiceProp(registers[m_menu->GetRememberLastButtonRegister() + 1], registers));
	cbSizer->AddStretchSpacer(1);
	grpSizer->Add(cbSizer, 0, wxEXPAND|wxALL, 4);
	sizer->AddSpacer(6);
	EndGroup();
}

void MenuPropDlg::CreateMenuGroup(wxSizer* sizer, bool loop) {	
	wxSizer* grpSizer = BeginGroup(sizer, _("Menu"));
	wxFlexGridSizer* grid = new wxFlexGridSizer(2, 4, 6);
	grid->AddGrowableCol(1);
	
	const StringSet& entries = m_pgc->GetEntries();
	AddText(grid, _("Entry"), m_tsi >= 0 ? wxALIGN_TOP : wxALIGN_CENTER_VERTICAL);
	if (m_tsi >= 0) {
		wxFlexGridSizer* entryGrid = new wxFlexGridSizer(3, 4, 16);
		AddCheckProp(entryGrid, _("Root"), entries.find(wxT("root")) != entries.end());
		AddCheckProp(entryGrid, _("Subtitle"), entries.find(wxT("subtitle")) != entries.end());
		AddCheckProp(entryGrid, _("Chapter"), entries.find(wxT("ptt")) != entries.end());
		AddCheckProp(entryGrid, _("Audio"), entries.find(wxT("audio")) != entries.end());
		AddCheckProp(entryGrid, _("Angle"), entries.find(wxT("angle")) != entries.end());
		grid->Add(entryGrid);
	} else
		AddCheckProp(grid, _("Title"), entries.find(wxT("title")) != entries.end());
	
	// Pre and post commans
	wxArrayString commands = GetCommandList();
	AddComboProp(grid, _("Pre commands:"), m_pgc->GetPreCommands(), commands);
	m_preCommandsCtrlIdx = GetLastControlIndex();
	AddComboProp(grid, _("Post commands:"), m_pgc->GetPostCommands(), commands);
	m_postCommandsCtrl = (wxComboBox*) GetLastControl();
	m_postCommandsCtrl->Enable(!loop);
	
	grpSizer->Add(grid, 0, wxEXPAND|wxALL, 4);
}

/**
 * Creates panel with buttons
 */
void MenuPropDlg::CreateButtonPane(wxSizer* sizer, bool resetButton, bool dontShowCheckbox) {
	wxStdDialogButtonSizer* buttonPane = new wxStdDialogButtonSizer();
	
	// cells & actions buttons
	buttonPane->Add(new wxButton(this, CELLS_BT_ID, _("Cells...")));
	buttonPane->Add(new wxButton(this, ACTIONS_BT_ID, _("Actions...")));
	buttonPane->Add(10, 10, 1, wxEXPAND);
	
	// ok & cancel buttons
	buttonPane->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	buttonPane->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	
	buttonPane->Realize();
	buttonPane->GetAffirmativeButton()->SetDefault();
	sizer->Add(buttonPane, 0, wxEXPAND|wxLEFT|wxRIGHT|wxBOTTOM, 10);
}

wxArrayString MenuPropDlg::GetCommandList() {
	wxArrayString commands;
#ifdef __WXMAC__
	commands.Add(wxT(""));
#endif
	commands.Add(wxT("jump cell 1;"));
	for (int pgci=0; pgci<(int)m_dvd->GetVmgm().Count(); pgci++)
		commands.Add(wxString::Format(_T("jump vmgm menu %d;"),pgci+1));
	for (int tsi=0; tsi<(int)m_dvd->GetTitlesets().Count(); tsi++) {
		Titleset* ts = m_dvd->GetTitlesets()[tsi];
		for (int pgci=0; pgci<(int)ts->GetMenus().Count(); pgci++)
			commands.Add(wxString::Format(_T("jump menu %d;"),pgci+1));
		for (int pgci=0; pgci<(int)ts->GetTitles().Count(); pgci++)
			commands.Add(wxString::Format(_T("jump title %d;"),pgci+1));
	}
	return commands;
}

bool MenuPropDlg::SetValues() {
	// validate sub end
	if (GetString(m_subStartCtrlIdx).size() > 0 && GetString(m_subStartCtrlIdx+1).size() > 0
			&& String2Time(GetString(m_subStartCtrlIdx)) >= String2Time(GetString(m_subStartCtrlIdx + 1))) {
		wxLogError(_("Subpicture end must be greater than start or empty"));
		return false;
	}
	// validate audio file
	wxString audioFile = GetString(m_audioCtrlIdx);
	if (audioFile.length() > 0) {
		if (!wxFile::Exists(audioFile)) {
			wxLogError(_("File %s doesn't exist."), audioFile.c_str());
			return false;
		}
		wxFfmpegMediaDecoder ffmpeg;
		if (!ffmpeg.Load(audioFile))
			return false;
		if (ffmpeg.GetStreamCount() == 0 || ffmpeg.GetStreamType(0) != stAUDIO) {
			wxLogError(_("File %s isn't audio file."), audioFile.c_str());
			return false;
		}
	}
	
	// validate commands
	wxString preCommands = GetString(m_preCommandsCtrlIdx);
	wxString postCommands = GetString(m_preCommandsCtrlIdx + 1);
	DVDAction action;
	action.SetCustom(preCommands);
	if (preCommands.length() && !action.IsValid(m_dvd, m_tsi, m_pgci, true, wxT(""), true, false)) {
		return false;
	}
	action.SetCustom(postCommands);
	if (postCommands.length() && !action.IsValid(m_dvd, m_tsi, m_pgci, true, wxT(""), true, false)) {
		return false;
	}
	
	if (postCommands.length() && GetInt(m_audioCtrlIdx + 3) == -1) {
		wxLogError(_("Post commands will not be executed if pause is set to -1"));
		return false;
	}

	int n = 0;
	
	// set video format
	VideoFormat videoFormat = (VideoFormat) (GetInt(n++) + 2);
	if (videoFormat != m_menu->GetVideoFormat()) {
		m_dvd->GetPgcArray(m_tsi, true).SetVideoFormat(videoFormat);
	}
	// set aspect ratio
	AspectRatio aspectRatio = (AspectRatio) (GetInt(n++) + 1);
	if (aspectRatio != m_menu->GetAspectRatio()) {
		m_dvd->GetPgcArray(m_tsi, true).SetAspectRatio(aspectRatio);
	}
	// set WS format
	m_video->SetWidescreen((WidescreenType) GetInt(n++));
	
	m_menu->SetBackground(GetString(n++));
	if (m_menu->GetBackground().length()) {
		if (GetBool(n++)) {
			int alignX = GetInt(n++);
			int alignY = GetInt(n++);
			m_menu->SetBackgroundAlign((wxSVG_PRESERVEASPECTRATIO) (wxSVG_PRESERVEASPECTRATIO_XMINYMIN + alignY * 3 + alignX));
			int meet = GetInt(n++);
			m_menu->SetBackgroundMeetOrSlice(meet == 0 ? wxSVG_MEETORSLICE_MEET : wxSVG_MEETORSLICE_SLICE);
		} else {
			n += 3;
			m_menu->SetBackgroundAlign(wxSVG_PRESERVEASPECTRATIO_NONE);
		}
	} else
		n += 4;
	m_menu->SetBackgroundColour(GetColour(n++));
	
	// VOB properties
	audioFile = GetString(n++).Strip(wxString::both);
	while (m_vob->GetAudioFilenames().GetCount() > 0)
		m_vob->RemoveAudioFile(0);
	if (audioFile.length() > 0) {
		m_vob->AddAudioFile(audioFile);
		if (m_vob->GetStreams().size() > 0) {
			Stream* stream = m_vob->GetStreams()[m_vob->GetStreams().size() - 1];
			if (stream->GetType() == stAUDIO)
				stream->SetAudioFormat((AudioFormat) (GetInt(n) + 1));
		}
	}
	n++;
	m_langCodes->RemoveAt(0);
	m_langCodes->Insert(GetString(n++), 0);
	m_vob->GetCells().swap(m_cells);
	m_menu->GetActions().swap(m_actions);
	m_vob->SetPause(GetInt(n++));
	n++; // loop
	
	// subpictures/buttons
	m_menu->SetStartTime(GetString(n++));
	m_menu->SetEndTime(GetString(n++));
	m_menu->SetRememberLastButton(GetBool(n++));
	m_menu->SetRememberLastButtonRegister(m_menu->GetRememberLastButton() ? GetInt(n) - 1 : -1);
	n++; // Last button register
	
	// set entries
	StringSet& entries = m_pgc->GetEntries();
	entries.clear();
	if (m_tsi >= 0) {
		if (GetBool(n++))
			entries.insert(wxT("root"));
		if (GetBool(n++))
			entries.insert(wxT("subtitle"));
		if (GetBool(n++))
			entries.insert(wxT("ptt"));
		if (GetBool(n++))
			entries.insert(wxT("audio"));
		if (GetBool(n++))
			entries.insert(wxT("angle"));
	} else if (GetBool(n++))
		entries.insert(wxT("title"));
	// remove same entries from other menus in the same titleset
	PgcArray& pgcs = m_dvd->GetPgcArray(m_tsi, true);
	for (int pgci = 0; pgci<(int)pgcs.size(); pgci++) {
		if (pgci != m_pgci) {
			pgcs[pgci]->RemoveEntries(entries);
		}
	}
	
	m_pgc->SetPreCommands(GetString(n++));
	m_pgc->SetPostCommands(GetString(n++));
	// fix coordinates of buttons if they are out of range
	m_menu->FixButtonCoordinates();
	return true;
}

void MenuPropDlg::OnChangeAspect(wxCommandEvent& event) {
	AspectRatio aspect = (AspectRatio) (m_aspect->GetSelection() + 1);
	m_wsFormat->Enable(aspect == ar16_9);
}

void MenuPropDlg::OnChangeAudio(wxCommandEvent& event) {
	if (!m_audioFormat)
		return;
	wxString audioFile = event.GetString();
	m_audioFormat->Enable(audioFile.length() > 0);
	// set audio format
	wxFfmpegMediaDecoder ffmpeg;
	if (wxFile::Exists(audioFile) && ffmpeg.Load(audioFile) && ffmpeg.GetStreamCount() > 0
			&& ffmpeg.GetStreamType(0) == stAUDIO) {
		AudioFormat af = afCOPY;
		wxString codecName = ffmpeg.GetCodecName(0);
		int sampleRate = ffmpeg.GetSampleRate(0);
		if (codecName != wxT("mp2") && codecName != wxT("ac3") && codecName != wxT("liba52"))
			af = m_dvd->GetAudioFormat();
		else if (sampleRate != 48000)
			af = codecName == wxT("mp2") ? afMP2 : afAC3;
		m_audioFormat->SetSelection(af - 1);
	}
}

void MenuPropDlg::OnLoopCheck(wxCommandEvent& event) {
	m_pauseCtrl->SetValue(event.IsChecked() ? 0 : -1);
	m_pauseCtrl->Enable(!event.IsChecked());
	m_postCommandsCtrl->SetValue(event.IsChecked() ? wxT("jump cell 1;"): wxT(""));
	m_postCommandsCtrl->Enable(!event.IsChecked());
}

void MenuPropDlg::OnCellsBt(wxCommandEvent& event) {
	MenuCellsDlg dlg(this, m_dvd, m_tsi, m_pgci, m_cells);
	if (dlg.ShowModal() == wxID_OK)
		m_cells.swap(dlg.GetCells());
}

void MenuPropDlg::OnActionsBt(wxCommandEvent& event) {
	MenuActionsDlg dlg(this, m_dvd, m_tsi, m_pgci, m_actions);
	if (dlg.ShowModal() == wxID_OK)
		m_actions.swap(dlg.GetActions());
}
