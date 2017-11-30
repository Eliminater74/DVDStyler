/////////////////////////////////////////////////////////////////////////////
// Name:        DVDAction.h
// Purpose:     Stores a DVD Action
// Author:      Alex Thuering
// Created:     03.09.2009
// RCS-ID:      $Id: DVDAction.cpp,v 1.34 2016/05/08 17:06:16 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "DVDAction.h"
#include "DVD.h"
#include "Config.h"
#include <wxSVGXML/svgxmlhelpr.h>
#include <wx/sstream.h>
#include <wx/regex.h>

extern "C" {
#include "dvdvm.h"
}

/* entry menu types */
const wxString s_entriesArr[6] = {wxT("title"), wxT("root"), wxT("subtitle"), wxT("audio"), wxT("angle"), wxT("ptt")};
const wxArrayString s_entries(6, s_entriesArr);

DVDAction::DVDAction(bool vmg, wxString id) {
	m_vmg = vmg;
	m_id = id;
	m_menu = false;
	m_tsi = -2; // current titleset
	m_pgci = 0; // title 1
	m_chapter = 0; // chapter 1
	m_playAll = false;
	m_playAllTitlesets = s_config.GetDefPlayAllTitlesets();
	m_audio = -1;
	m_subtitle = -1;
}

wxString DVDAction::AsString(DVD* dvd, bool translate) {
	wxString action;
	wxString actionPost;
	if (m_audio >= 0)
		action += translate ? wxString::Format(_("select audio track %d and "), m_audio + 1)
			: wxString::Format(wxT("audio=%d; "), m_audio);
	if (m_subtitle >= 0)
		action += translate ? wxString::Format(_("select subtitle track %d and "), m_subtitle + 1)
			: wxString::Format(wxT("subtitle=%d; "), m_subtitle > 0 ? m_subtitle + 63 : 0);
	if (m_custom.length()) // custom action selected
		return action + m_custom;
	// jump to selected
	if (!translate)
		action += wxT("jump ");
	if (GetPgci() == -1 && GetEntry().length() == 0) // jump to cell 1
		return action + (translate ? _("jump to start of current menu") : wxT("cell 1;"));
	int tsi = GetTsi();
	bool menu = IsMenu();
	wxString entry = GetEntry();
	int pgci = GetPgci();
	if (tsi >= 0 && (!m_vmg || menu || translate)) { // don't use titleset for jumping to title from vmg
		if (!translate)
			action += wxString::Format(wxT("titleset %d "), tsi + 1);
		else if (dvd->GetTitlesets().size() > 1)
			actionPost = wxT(" ") + wxString::Format(_("from titleset %d"), tsi + 1);
	}
	if (menu && entry.length() > 0) {
		if (translate) {
			if (s_entries.Index(entry) >= 0) {
				const wxString jumpToEntry[6] = {
						_("jump to title menu"), _("jump to root menu"), _("jump to subtitle menu"),
						_("jump to audio menu"), _("jump to angle menu"), _("jump to chapter menu")};
				action += jumpToEntry[s_entries.Index(entry)];
				Menus& menus = (Menus&) dvd->GetPgcArray(tsi, true);
				int menuIdx = menus.GetPgciByEntry(entry);
				if (menuIdx >= 0)
					action += wxT(" (") + wxString(_("menu")) + wxString::Format(wxT(" %d)"), menuIdx + 1);
			} else {
				action += _("jump to menu") + wxString(wxT(" entry ")) + entry;
			}
		} else if (tsi == -1) {
			action += wxT("vmgm menu entry ") + entry;
		} else {
			action += wxT("menu entry ") + entry;
		}
	} else {
		int n = pgci + 1;
		if (m_vmg && !menu && !translate) // use sequence number of title for jumping to title from vmg
			for (int i = 0; i < tsi; i++)
				n += i < (int) dvd->GetTitlesets().size() ? dvd->GetTitlesets()[i]->GetTitles().size() : 1;
		
		if (menu) {
			if (tsi == -1)
				action += translate ? _("jump to VMGM menu") : wxT("vmgm menu");
			else
				action += translate ? _("jump to menu") : wxT("menu");
			if (tsi < 0 || n > 1 || translate)
				action += wxString::Format(wxT(" %d"), n);
		} else {
			if (GetChapter() > 0) {
				if (translate) {
					action += wxString::Format(_("jump to title %d chapter %d"), n, GetChapter() + 1);
				} else {
					action += wxString::Format(wxT("title %d chapter %d"), n, GetChapter() + 1);
				}
			} else {
				action += translate ? _("jump to title") : wxT("title");
				action += wxString::Format(wxT(" %d"), n);
			}
		}
	}
	if (translate)
		action += actionPost;
	else
		action += wxT(";");
	return action;
}

void DVDAction::SetCustom(wxString action) {
	// remove multiple spaces
	action = action.Strip(wxString::both);
	wxRegEx reSpace(wxT("[[:space:]]+"));
	reSpace.ReplaceAll(&action, wxT(" "));
	// check jump command
	wxString ptJump = wxT("^jump ((titleset ([1-9][0-9]?) )|(vmgm ))?((menu ([1-9][0-9]?))|");
	ptJump += wxT("(menu entry (title|root|subtitle|audio|angle|ptt))|(title ([1-9][0-9]?)( chapter ([1-9][0-9]*))?));$");
	wxRegEx reJump(ptJump);
	if (reJump.Matches(action)) {
		long lval;
		m_tsi = -2;
		// titleset
		int i = 3;
		wxString val = reJump.GetMatch(action, i++);
		if (val.length() > 0 && val.ToLong(&lval))
			m_tsi = lval - 1;
		// vmgm
		val = reJump.GetMatch(action, i++);
		if (val.length())
			m_tsi = -1;
		i += 2;
		// menu x
		val = reJump.GetMatch(action, i++);
		if (val.length() && val.ToLong(&lval)) {
			m_menu = true;
			m_pgci = lval - 1;
			m_entry = wxT("");
		} else {
			// menu entry x
			i++;
			val = reJump.GetMatch(action, i++);
			if (val.length()) {
				m_menu = true;
				m_pgci = -1;
				m_entry = val;
			} else {
				// title x
				i++;
				val = reJump.GetMatch(action, i++);
				if (val.length() && val.ToLong(&lval)) {
					m_menu = false;
					m_pgci = lval - 1;
				}
				// chapter x
				i++;
				val = reJump.GetMatch(action, i++);
				if (val.length() && val.ToLong(&lval)) {
					m_chapter = lval - 1;
				}
			}
		}
		if (m_tsi == -1 && !m_menu) // not valid action => set it as custom action
			m_custom = action;
		else
			m_custom = wxT("");
	} else {
		m_custom = action;
	}
}

/** Returns source of action as string */
wxString DVDAction::GetSourceStr(DVD* dvd, int tsi, int pgci, bool menu, wxString buttonId) {
	wxString source;
	if (tsi >= 0) {
		if (dvd->GetTitlesets().GetCount() > 1)
			source = _("titleset") + wxString::Format(wxT(" %d "), tsi + 1);
		source += (menu ? _("menu") : _("title")) + wxString::Format(wxT(" %d"), pgci + 1);
	} else
		source = _("VMGM menu") + wxString::Format(wxT(" %d"), pgci + 1);
	if (buttonId.Length())
		source += wxT(" ") + buttonId;
	return source;
}

/** Validates jump target */
bool validateJumpTarget(wxString msgPrefix, DVD* dvd, int tsi, int pgci, bool menu, int chapter, wxString entry) {
	int titlesetCount = dvd->GetTitlesets().GetCount();
	if (tsi >= titlesetCount) {
		wxLogError(msgPrefix + _("cannot jump to titleset %d, only %d exist"), tsi + 1, titlesetCount);
		return false;
	}
	if (menu) {
		Menus& menus = tsi < 0 ? dvd->GetVmgm() : dvd->GetTitlesets()[tsi]->GetMenus();
		int menuCount = menus.Count() > 0 ? menus.Count() : (dvd->IsEmptyMenuEnabled() && dvd->HasMenus() ? 1 : 0);
		if (menuCount == 0) {
			if (tsi == -1)
				wxLogError(msgPrefix + _("cannot jump to VMGM menu; none exist"));
			else
				wxLogError(msgPrefix + _("cannot jump to menu; none exist"));
			return false;
		} else if (entry.length() > 0) { // menu entry
			const StringSet& entrySet = menus.GetEntries();
			if (entry != wxT("default") && entry != wxT("root") && entrySet.find(entry) == entrySet.end()){
				const wxString entries[6] = {_("title menu"), _("root menu"), _("subtitle menu"), _("audio menu"),
						_("angle menu"), _("chapter menu")};
				int entryIdx = s_entries.Index(entry);
				wxString entryTran = entryIdx >= 0 ? entries[entryIdx] : wxT("menu entry ") + entry;
				wxLogError(msgPrefix + _("cannot find %s"), entryTran.c_str());
				return false;
			}
		} else if (pgci >= menuCount) {
			if (tsi == -1)
				wxLogError(msgPrefix + _("cannot jump to VMGM menu %d, only %d exist"), pgci + 1, menuCount);
			else
				wxLogError(msgPrefix + _("cannot jump to menu %d, only %d exist"), pgci + 1, menuCount);
			return false;
		}
	} else {
		int titleCount = tsi >= 0 ? dvd->GetTitlesets()[tsi]->GetTitles().Count() : 0;
		if (tsi < 0) {
			for (unsigned int i = 0; i < dvd->GetTitlesets().Count(); i++) {
				titleCount += dvd->GetTitlesets()[i]->GetTitles().Count();
			}
		}
		if (titleCount == 0) {
			wxLogError(msgPrefix + _("cannot jump to title; none exist"));
			return false;
		} else if (pgci >= titleCount) {
			wxLogError(msgPrefix + _("cannot jump to title %d, only %d exist"), pgci + 1, titleCount);
			return false;
		} else if (tsi >= 0 && chapter > 0 && chapter < 65535) {
			int chapterCount = dvd->GetTitlesets()[tsi]->GetTitles()[pgci]->GetChapterCount();
			if (chapter >= chapterCount) {
				wxLogError(msgPrefix + _("cannot jump to chapter %d of title %d, only %d exist"),
						chapter + 1, pgci + 1, chapterCount);
				return false;
			}
		}
	}
	return true;
}

wxString GetJumpFormError(bool toMenu, bool fromMenu) {
	wxString msg;
	if (toMenu) { // jump to menu from another titleset
		msg = _("cannot jump to a specific menu in another titleset, only to root/chapter/audio/subtitle/angle menu");
	} else { // jump to title from another titleset
		if (fromMenu)
			msg = _("cannot jump from regular menu to a title in another titleset, use VMGM menu instead");
		else
			msg = _("cannot jump to a title in another titleset");
	}
	return msg;
}

/** Returns true if action is valid */
bool DVDAction::IsValid(DVD* dvd, int tsi, int pgci, bool menu, wxString buttonId, bool showMessage, bool showSource,
		bool skipInvalidTarget) {
	bool log = wxLog::IsEnabled();
	if (!showMessage)
		wxLog::EnableLogging(false);
	wxString source = showSource ? GetSourceStr(dvd, tsi, pgci, menu, buttonId) + wxT(": ") : wxT("");
	wxString msgPrefix = source + wxString(_("invalid action")) + wxT(": ");
	bool res = true;
	if (IsCustom()) {
		res = IsCustomValid(msgPrefix, source, dvd, tsi, pgci, menu, buttonId);
	} else {
		if (m_tsi == tsi)
			m_tsi = -2; // same titleset
		if (m_tsi == -2 && m_pgci == pgci && m_menu == menu)
			m_pgci = -1; // same pgci
		if (!menu && ((m_menu && m_entry.length() == 0) || tsi == -1)) {
			wxLogError(msgPrefix + _("cannot jump to a menu from a title, use 'call' instead"));
			res = false;
		} else if (m_tsi >= 0 && ((m_menu && m_pgci > 0 && m_entry.length() == 0) || (!m_menu && tsi != -1))
				&& !dvd->IsJumppadsEnabled()) { // jump to menu/title from another titleset and jumppad disabled
			wxLogError(msgPrefix + GetJumpFormError(m_menu, menu));
			res = false;
		} else if (tsi == -1 && m_chapter > 0 && !dvd->IsJumppadsEnabled()) {
			wxString target = m_chapter < 65536 ? _("chapter") : (m_chapter < 131072 ? _("program") : _("cell"));
			wxLogError(msgPrefix + _("jumping from VMGM menu to %s is not allowed"), target.c_str());
			return false;
		} else {
			if (!skipInvalidTarget)
				res = validateJumpTarget(msgPrefix, dvd, m_tsi != -2 ? m_tsi : tsi, m_pgci, m_menu, m_chapter, m_entry);
		}
	}
	wxLog::EnableLogging(log);
	return res;
}

bool allowallreg = false;
struct vm_statement* dvd_vm_parsed_cmd = NULL;

wxString s_vmlErrorMsg;
wxString s_parseErrorMsg;

extern "C" {

/* reports a vml error. */
void dvdvmlerror(const char *errorMesage, const char *dvdvmtext) {
	wxString errorMsg(errorMesage, wxConvISO8859_1);
	wxString dvdvmText(dvdvmtext, wxConvISO8859_1);
	s_vmlErrorMsg = wxString::Format(errorMsg, dvdvmText.c_str()) + wxT(".");
}

/* reports a parse error. */
void dvdvmerror(const char *errorMesage) {
    extern char *dvdvmtext;
    s_parseErrorMsg = wxString(errorMesage, wxConvISO8859_1);
    wxString token(dvdvmtext, wxConvISO8859_1);
    if (token.length() > 0)
    	s_parseErrorMsg += wxT(" (") + wxString::Format(_("on token '%s'"), token.c_str()) + wxT(")");
    if (s_parseErrorMsg.length())
    	s_parseErrorMsg = s_parseErrorMsg.Mid(0, 1).Upper() + s_parseErrorMsg.Mid(1) + wxT(".");
}

} // extern "C"

/* type of menu/title */
typedef enum {
	VTYPE_VTS = 0, /* title in titleset */
	VTYPE_VTSM = 1, /* menu in titleset */
	VTYPE_VMGM = 2 /* menu in VMG */
} vtypes;

bool validateTargetNumbers(wxString msgPrefix, int i1, int i2, int i3) {
	if (!i1 && !i2 && !i3) { //  same VMGM/titleset and same PGC and no chapter/cell/program
		wxLogError(msgPrefix + _("0 is not valid menu/chapter number"));
		return false;
	}
	if (i2 == 128) {
		wxLogError(msgPrefix + _("0 is not valid title number"));
		return false;
	}
	if (i3 == 65536) {
		wxLogError(msgPrefix + _("0 is not valid program number"));
		return false;
	}
	if (i3 == 131072) {
		wxLogError(msgPrefix + _("0 is not valid cell number"));
		return false;
	}
	return true;
}

bool validateJump(wxString msgPrefix, DVD* dvd, const struct vm_statement *cs, int tsi, int pgci, vtypes ismenu) {
	int i1 = cs->i1; // if nonzero, 1 for VMGM, or titleset nr + 1
	int i2 = cs->i2; // menu number or menu entry ID + 120 or title number + 128
	int i3 = cs->i3; // chapter number if nonzero and less than 65536 or program number + 65536 or cell number + 131072
	
	if (!validateTargetNumbers(msgPrefix, i1, i2, i3)) {
		return false;
	}
		
	// check for various disallowed combinations
	if (i1 == 1 && ismenu == VTYPE_VMGM) {
		i1 = 0; // no need to explicitly specify VMGM
	}
	
	if (((i2 > 0 && i2 < 128) // jump to non-entry menu
			|| (i2 == 0 && i1 == 1)) // jump to VMGM
			&& ismenu == VTYPE_VTS) {
		wxLogError(msgPrefix + _("cannot jump to a menu from a title, use 'call' instead"));
		return false;
	}
	if (i2 > 0 && i2 < 128 // jump to non-entry menu
			&& i3 // chapter/cell/program specified
			&& ismenu != VTYPE_VTS) {
		wxLogError(msgPrefix + _("cannot specify chapter when jumping to another menu"));
		return false;
	}
	if (i1 && !i2) { // VMGM/titleset and no PGC
		wxLogError(msgPrefix + _("cannot omit menu/title if specifying vmgm/titleset"));
		return false;
	}
	if (i2 == 121 && (i1 >= 2 || (i1 == 0 && ismenu != VTYPE_VMGM))) { //  jump to FPC
		wxLogError(msgPrefix + _("VMGM must be specified with FPC"));
		return false;
	}

	if (i2 == 121) {  // jump to FPC
		// ok
	} else if (i2 >= 120 && i2 < 128) { // jump to menu entry
		wxString entry = i2 >= 122 && i2 < 128 ? s_entriesArr[i2-122] : wxT("default");
		if (!validateJumpTarget(msgPrefix, dvd, i1 == 0 ? tsi : i1 - 2, -1, true, i3 - 1, entry))
			return false;
	} else if (i1 == 1 && i2 >= 128) { // jump to vmgm and title
		wxLogError(msgPrefix + _("VMGM does not have titles"));
		return false;
	} else if (i1 >= 2 && !dvd->IsJumppadsEnabled()) { // jump to titleset and jumppad disabled
		wxLogError(msgPrefix + GetJumpFormError(i2 < 128, ismenu != VTYPE_VTS));
		return false;
	} else if (ismenu == VTYPE_VMGM && i2 >= 128 && i3) { // jump from vmgm to title chapter/program/cell
		if (!dvd->IsJumppadsEnabled()) {
			wxString target = i3 < 65536 ? _("chapter") : (i3 < 131072 ? _("program") : _("cell"));
			wxLogError(msgPrefix + _("jumping from VMGM menu to %s is not allowed"), target.c_str());
			return false;
		}
	} else if (!i1 && !i2 && i3) {
		if (pgci < 0) {
			wxLogError(msgPrefix + _("cannot jump to a chapter from a FPC"));
			return false;
		}
		// check chapters
		if (i3 < 65536) {
			if (ismenu != VTYPE_VTS) {
				wxLogError(msgPrefix + _("menus do not have chapters"));
				return false;
			}
			int chapterCount = dvd->GetTitlesets()[tsi]->GetTitles()[pgci]->GetChapterCount();
			if (i3 >> 16 == 0 && (i3 & 65535) > chapterCount) {
				wxLogError(msgPrefix + _("cannot jump to chapter %d, only %d exist"), i3 & 65535, chapterCount);
				return false;
			}
		}
	} else {
		if (!validateJumpTarget(msgPrefix, dvd, i1 == 0 ? tsi : i1 - 2, (i2 & 127) - 1, i2 < 128, i3 - 1, wxT(""))) {
			return false;
		}
	}
	return true;
}

bool validateCall(wxString msgPrefix, DVD* dvd, const struct vm_statement *cs, int tsi, int pgci, vtypes ismenu)  {
	int i1 = cs->i1; // if nonzero is 1 for VMGM, or titleset nr + 1
	int i2 = cs->i2; // menu number or menu entry ID + 120 or title number + 128
	int i3 = cs->i3; // chapter number if nonzero and less than 65536 or program number + 65536 or cell number + 131072
	int i4 = cs->i4; // resume cell if specified, else zero
	int i5 = cs->i5; // last menu if specified, else zero
	
	if (i5) {
		if (!dvd->HasMenus()) {
			wxLogError(msgPrefix + _("cannot jump to menu; none exist"));
			return false;
		}
		return true;
	}
	
	if (!validateTargetNumbers(msgPrefix, i1, i2, i3)) {
		return false;
	}
	
	// CALL's from <post> MUST have a resume cell
	if (!i4)
		i4 = 1;
	if (ismenu != VTYPE_VTS) {
		wxLogError(msgPrefix + _("cannot 'call' a menu from another menu, use 'jump' instead"));
		return false;
	}
	if (i2 == 0 || i2 >= 128) { // title nr or no menu/title
		wxLogError(msgPrefix + _("cannot 'call' another title, use 'jump' instead"));
		return false;
	}
	if (i3 != 0) { // chapter/cell/program
		wxLogError(msgPrefix + _("cannot 'call' a chapter within a menu"));
		return false;
	}
	if (i2 == 121 // FPC
			&& (i1 >= 2 /*titleset*/|| (i1 == 0 /*no VMGM/titleset*/&& ismenu != VTYPE_VMGM))) {
		wxLogError(msgPrefix + _("VMGM must be specified with FPC"));
		return false;
	}

	if (i1 >= 2 && !dvd->IsJumppadsEnabled()) { // titleset
		wxLogError(msgPrefix + _("cannot call to a menu in another titleset"));
		return false;
	} else if (i1 == 0 && i2 < 120 && !dvd->IsJumppadsEnabled()) { // non-entry menu in current VMGM/titleset
		wxLogError(msgPrefix + _("cannot call to a specific menu PGC, only an entry"));
		return false;
	} else if (i2 != 121) {
		wxString entry = i2 >= 122 && i2 < 128 ? s_entriesArr[i2-122] : (i2 == 120 ? wxString(wxT("default")) : wxT(""));
		if (!validateJumpTarget(msgPrefix, dvd, i1 == 0 ? tsi : i1 - 2, i2 - 1, true, i3 - 1, entry)) {
			return false;
		}
	}
	return true;
}

bool validateVm(DVD* dvd, const struct vm_statement *cs, int tsi, int pgci, vtypes ismenu, wxString msgPrefix) {
	while (cs) {
		switch (cs->op) {
		case VM_SET: // cs->i1 is destination, cs->param is source
			if (cs->i1 <= 15 // set GPRM
					|| (cs->i1 >= 32 && cs->i1 <= 32 + 15) // set GPRM, counter mode
					|| (cs->i1 >= 128 && cs->i1 <= 128 + 3)) { // audio, subtitle, angle
				// ok
			} else if (cs->i1 == 128 + 8) { // button
				if (cs->param->op == VM_VAL && !(cs->param->i1 >= -128 && cs->param->i1 < 0)) {
					const int v = cs->param->i1;
					if (v > 0 && (v & 1023) != 0) // TODO
						fprintf(stderr,
								"WARN: Button value is %d, but it should be a multiple of 1024\n"
									"WARN: (button #1=1024, #2=2048, etc)\n", v);
				}
			} else {
				wxLogError(msgPrefix + _("cannot set SPRM %d"), cs->i1 - 128);
				return false;
			}
			break;

		case VM_IF: /* if-statement */
			if (!validateVm(dvd, cs->param->next->param, tsi, pgci, ismenu, msgPrefix)
					|| (cs->param->next->next && !validateVm(dvd, cs->param->next->next, tsi, pgci, ismenu, msgPrefix))) {
				return false;
			}
			break;

		case VM_LABEL: {
			// TODO
//			for (int i = 0; i < numlabels; i++)
//				if (!strcasecmp(labels[i].lname, cs->s1)) {
//					wxLogError(msgPrefix + _("Duplicate label '%s'"), cs->s1);
//					return false;
//				} /*if; for*/
//			if (numlabels == MAXLABELS) {
//				wxLogError(msgPrefix + _("Too many labels"));
//				return false;
//			}
//			labels[numlabels].lname = cs->s1;
//			numlabels++;
		}
			break;

		case VM_GOTO:
			// TODO
//			if (numgotos == MAXGOTOS) {
//				wxLogError(msgPrefix + _("Too many gotos"));
//				return false;
//			}
//			numgotos++;
			break;

		case VM_JUMP:
			if (!validateJump(msgPrefix, dvd, cs, tsi, pgci, ismenu))
				return false;
			break;

		case VM_CALL:
			if (!validateCall(msgPrefix, dvd, cs, tsi, pgci, ismenu))
				return false;
			break;

		case VM_BREAK:
		case VM_EXIT:
		case VM_LINK:
		case VM_NOP:
			break;

		default:
			wxLogError(msgPrefix + _("unsupported VM opcode %d"), cs->op);
			return false;
		}
		cs = cs->next;
	}
	return true;
}

/** Checks custom action */
bool DVDAction::IsCustomValid(wxString msgPrefix, wxString source, DVD* dvd, int tsi, int pgci, bool menu,
		wxString buttonId) {
	// parse command
	char* cmd = (char*) malloc(m_custom.length() + 1);
	strcpy(cmd, (const char*)m_custom.ToAscii());
	dvdvm_buffer_state buf = dvdvm_scan_string(cmd);
	dvd_vm_parsed_cmd = 0;
	s_parseErrorMsg = wxT("");
	s_vmlErrorMsg = wxT("");
	if (dvdvmparse()) {
		wxString msg = source + _("invalid action '%s'");
		if (s_vmlErrorMsg.length())
			msg += wxT(": ") + s_vmlErrorMsg;
		else if (s_parseErrorMsg.length())
			msg += wxT(": ") + s_parseErrorMsg;
		wxLogError(msg, m_custom.c_str());
		return false;
	}
	dvdvm_delete_buffer(buf);
	free((void *) cmd);

	// validate command
	vtypes ismenu = menu ? (m_vmg ? VTYPE_VMGM : VTYPE_VTSM) : VTYPE_VTS;
	return validateVm(dvd, dvd_vm_parsed_cmd, tsi, pgci, ismenu, msgPrefix);
}

int findFirstJump(const struct vm_statement *cs, int tsi) {
	while (cs) {
		switch (cs->op) {
		case VM_IF: {
			int result = findFirstJump(cs->param->next->param, tsi);
			if (result != -1)
				return result;
			if (cs->param->next->next)
				result = findFirstJump(cs->param->next->next, tsi); 
			if (result != -1)
				return result;
			break;
		}
		case VM_JUMP: {
			int i1 = cs->i1; // if nonzero, 1 for VMGM, or titleset nr + 1
			int i2 = cs->i2; // menu number or menu entry ID + 120 or title number + 128
			int i3 = cs->i3; // chapter number if nonzero and less than 65536 or program number + 65536 or cell number + 131072
			if (i1 == 0)
				i1 = tsi + 2;
			if (i1 > 1 && i2 > 128)
				return DVD::MakeId(i1 - 2, i2 - 129, i3 < 256 ? i3 : 0, false);
			break;
		}
		default:
			break;
		}
		cs = cs->next;
	}
	return -1;
}

/** Returns first jump target or -1 if no jump title action found */
int DVDAction::FindFirstJump(int tsi) {
	// parse command
	char* cmd = (char*) malloc(m_custom.length() + 1);
	strcpy(cmd, (const char*)m_custom.ToAscii());
	dvdvm_buffer_state buf = dvdvm_scan_string(cmd);
	dvd_vm_parsed_cmd = 0;
	s_parseErrorMsg = wxT("");
	s_vmlErrorMsg = wxT("");
	int result = dvdvmparse();
	dvdvm_delete_buffer(buf);
	free((void *) cmd);
	if (result)
		return -1;
	return findFirstJump(dvd_vm_parsed_cmd, tsi);
}

wxSvgXmlNode* DVDAction::GetXML(DVDFileType type, DVD* dvd) {
	wxSvgXmlNode* rootNode = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, type == DVDAUTHOR_XML ? wxT("button") : wxT("action"));
	switch (type) {
	case DVDSTYLER_XML:
		if (m_id.length())
			rootNode->AddProperty(wxT("id"), m_id);
		if (!GetCustom().length()) {
			if (GetTsi() >= -1)
				rootNode->AddProperty(wxT("tsi"), wxString::Format(wxT("%d"), GetTsi()));
			if (GetEntry().length() > 0)
				rootNode->AddProperty(wxT("entry"), GetEntry());
			else if (GetPgci() >= 0)
				rootNode->AddProperty(wxT("pgci"), wxString::Format(wxT("%d"), (GetPgci()*2 + (IsMenu() ? 0 : 1))));
			if (GetChapter() > 0)
				rootNode->AddProperty(wxT("chapter"), wxString::Format(wxT("%d"), GetChapter()));
		} else
			rootNode->AddChild(new wxSvgXmlNode(wxSVGXML_TEXT_NODE, wxEmptyString, GetCustom()));
		if (IsPlayAll())
			rootNode->AddProperty(wxT("playAll"), wxT("true"));
		if (IsPlayAllTitlesets())
			rootNode->AddProperty(wxT("playAllTitlesets"), wxT("true"));
		if (GetAudio() >= 0)
			rootNode->AddProperty(wxT("audio"), wxString::Format(wxT("%d"), GetAudio()));
		if (GetSubtitle() >= 0)
			rootNode->AddProperty(wxT("subtitle"), wxString::Format(wxT("%d"), GetSubtitle()));
		break;
	case SPUMUX_XML:
		rootNode->AddProperty(wxT("name"), m_id);
		break;
	case DVDAUTHOR_XML:
		rootNode->AddProperty(wxT("name"), m_id);
		rootNode->AddChild(new wxSvgXmlNode(wxSVGXML_TEXT_NODE, wxEmptyString, AsString(dvd)));
		break;
	default:
		break;
	}
	return rootNode;
}

bool DVDAction::PutXML(wxSvgXmlNode* node) {
	node->GetPropVal(wxT("id"), &m_id);
	long lval;
	wxString val;
	if (node->GetPropVal(wxT("tsi"), &val) && val.ToLong(&lval))
		SetTsi(lval);
	if (node->GetPropVal(wxT("pgci"), &val) && val.ToLong(&lval)) {
		SetPgci(lval / 2);
		SetMenu(lval % 2 == 0);
	} else {
		SetPgci(-1);
		SetMenu(true);
	}
	if (node->GetPropVal(wxT("entry"), &val))
		SetEntry(val);
	if (node->GetPropVal(wxT("chapter"), &val) && val.ToLong(&lval))
		SetChapter(lval);
	if (node->GetPropVal(wxT("playAll"), &val))
		SetPlayAll(val == wxT("true") || val == wxT("1"));
	SetPlayAllTitlesets(node->GetPropVal(wxT("playAllTitlesets"), &val) && (val == wxT("true") || val == wxT("1")));
	if (node->GetPropVal(wxT("audio"), &val) && val.ToLong(&lval))
		SetAudio(lval);
	if (node->GetPropVal(wxT("subtitle"), &val) && val.ToLong(&lval))
		SetSubtitle(lval);
	if (node->GetChildren() && (node->GetChildren()->GetType() == wxSVGXML_TEXT_NODE
			|| node->GetChildren()->GetType() == wxSVGXML_CDATA_SECTION_NODE))
		SetCustom(node->GetChildren()->GetContent().Strip(wxString::both));
	return true;
}

/** Stores object data to string */
wxString DVDAction::Serialize() {
	wxSvgXmlDocument xml;
	xml.SetRoot(GetXML(DVDSTYLER_XML, NULL));
	wxStringOutputStream stream;
	xml.Save(stream);
	return stream.GetString();
}

/** Restores object from data */
void DVDAction::Deserialize(const wxString& data) {
	wxStringInputStream stream(data);
	wxSvgXmlDocument xml;
	xml.Load(stream);
	PutXML(xml.GetRoot());
}
