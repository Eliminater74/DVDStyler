/////////////////////////////////////////////////////////////////////////////
// Name:        DVDAction.h
// Purpose:     Stores a DVD Action
// Author:      Alex Thuering
// Created:     03.09.2009
// RCS-ID:      $Id: DVDAction.h,v 1.17 2014/04/06 10:24:39 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DVDACTION_H
#define DVDACTION_H

#include <wx/wx.h>

enum DVDFileType { DVDSTYLER_XML, DVDAUTHOR_XML, SPUMUX_XML };
class DVD;
class wxSvgXmlNode;

class DVDAction {
public:
	/** Constructor */
	DVDAction(bool vmg = false, wxString id = wxT(""));
	/** Sets if it's a vmgm menu */
	inline void SetVmg(bool vmg) { m_vmg = vmg; }
	
	wxString GetId() { return m_id; }
	void SetId(const wxString& id) { m_id = id; }
	wxString AsString(DVD* dvd, bool translate = false);
	
	inline bool IsCustom() { return m_custom.length()>0; }
	inline wxString GetCustom() { return m_custom; }
	void SetCustom(wxString action);
	
	inline int  GetTsi() { return m_tsi; }
	inline void	SetTsi(int tsi) { m_tsi = tsi; }
	inline int  GetPgci() { return m_pgci; }
	inline void	SetPgci(int pgci) { m_pgci = pgci; }
	inline bool IsMenu() { return m_menu; }
	inline void	SetMenu(int menu) { m_menu = menu; }
	inline wxString GetEntry() { return m_entry; }
	inline void SetEntry(wxString entry) { m_entry = entry; }
	inline int  GetChapter() { return m_chapter; }
	inline void	SetChapter(int chapter) { m_chapter = chapter; }
	
	inline bool IsPlayAll() { return m_playAll; }
	inline void SetPlayAll(bool value) { m_playAll = value; }
	
	inline bool IsPlayAllTitlesets() { return m_playAllTitlesets; }
	inline void SetPlayAllTitlesets(bool value) { m_playAllTitlesets = value; }
	
	inline int  GetAudio() { return m_audio; }
	inline void	SetAudio(int audio) { m_audio = audio; }
	inline int  GetSubtitle() { return m_subtitle; }
	inline void	SetSubtitle(int subtitle) { m_subtitle = subtitle; }
	
	/** Returns true if action is valid */
	bool IsValid(DVD* dvd, int tsi, int pgci, bool menu, wxString buttonId = wxT(""),
			bool showMessage = false, bool showSource = true, bool skipInvalidTarget = false);
	/** Returns first jump target or -1 if no jump title action found */
	int FindFirstJump(int tsi);

	wxSvgXmlNode* GetXML(DVDFileType type, DVD* dvd);
	bool PutXML(wxSvgXmlNode* node);
	/** Stores object data to string */
	wxString Serialize();
	/** Restores object from data */
	void Deserialize(const wxString& data);
	
	/** Returns source of action as string */
	static wxString GetSourceStr(DVD* dvd, int tsi, int pgci, bool menu, wxString buttonId);
private:
	bool m_vmg;
	wxString m_id;
	wxString m_custom;
	int m_tsi;
	int m_pgci;
	bool m_menu;
	wxString m_entry;
	int m_chapter;
	bool m_playAll;
	bool m_playAllTitlesets;
	int m_audio;
	int m_subtitle;
	/** Returns true if custom action is valid */
	bool IsCustomValid(wxString msgPrefix, wxString source, DVD* dvd, int tsi, int pgci, bool menu, wxString buttonId);
};

extern const wxArrayString s_entries;

#endif /* DVDACTION_H_ */
