/////////////////////////////////////////////////////////////////////////////
// Name:        Cell.cpp
// Purpose:     The class to store a DVD cell data
// Author:      Alex Thuering
// Created:     29.01.2003
// RCS-ID:      $Id: Cell.cpp,v 1.3 2015/09/25 13:36:50 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "Cell.h"
#include "Utils.h"
#include "DVD.h"
#include <wxSVGXML/svgxmlhelpr.h>

void ReplaceJumpNextTitle(wxString& commands, DVD* dvd, int nextTitle) {
	wxString cmd = wxT("jump next title;");
	int stPos = commands.Find(cmd);
	if (stPos >= 0) {
		wxString jmp;
		if (nextTitle > 0) {
			jmp = wxString::Format(wxT("jump title %d;"), nextTitle);
		} else if (dvd->HasMenus()) {
			jmp = dvd->GetVmgm().size() > 0 ? wxT("call vmgm menu entry title;") : wxT("call menu entry root;");
		} else {
			jmp = wxT("exit;");
		}
		commands.replace(stPos, cmd.length(), jmp);
	}
}

/** Constructor */
Cell::Cell() {
	m_start = -1;
	m_end = -1;
	m_chapter = false;
	m_program = false;
	m_pause = 0;
}

/** Constructor */
Cell::Cell(long start, bool chapter) {
	m_start = start;
	m_end = -1;
	m_chapter = chapter;
	m_program = false;
	m_pause = 0;
}

/** Returns the start time as string */
wxString Cell::GetStartStr() const {
	return m_start >= 0 ? Time2String(m_start) : wxT("");
}

/** Sets the start time */
void Cell::SetStart(const wxString& startStr) {
	m_start = startStr.length() ? String2Time(startStr) : -1;
}

/** Returns the end time as string */
wxString Cell::GetEndStr() const {
	return m_end >= 0 ? Time2String(m_end) : wxT("");
}

/** Sets the end time */
void Cell::SetEnd(const wxString& endStr) {
	m_end = endStr.length() ? String2Time(endStr) : -1;
}

wxSvgXmlNode* Cell::GetXML(DVDFileType type, long endTime, DVD* dvd, int nextTitle) {
	wxSvgXmlNode* node = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, wxT("cell"));
	if (m_start >= 0)
		node->AddProperty(wxT("start"), GetStartStr());
	if (endTime >= 0)
		node->AddProperty(wxT("end"), Time2String(endTime));
	if (m_chapter)
		node->AddProperty(wxT("chapter"), wxT("1"));
	if (m_program)
		node->AddProperty(wxT("program"), wxT("1"));
	if (m_pause != 0) {
		wxString pauseStr = m_pause == -1 ? wxT("inf") : wxString::Format(wxT("%d"), m_pause);
		node->AddProperty(wxT("pause"), pauseStr);
	}
	if (m_commands.length()) {
		wxString commands = m_commands;
		if (type == DVDAUTHOR_XML) {
			// replace call last menu
			wxString cmd = wxT("call last menu");
			int stPos = commands.Find(cmd);
			if (stPos >= 0)
				commands.replace(stPos, cmd.length(), wxT("call menu entry root"));
			// replace jump next title
			ReplaceJumpNextTitle(commands, dvd, nextTitle);
		}
		// add content
		node->AddChild(new wxSvgXmlNode(wxSVGXML_TEXT_NODE, wxEmptyString, commands));
	}
	return node;
}

bool Cell::PutXML(wxSvgXmlNode* node) {
	if (node == NULL || node->GetName() != wxT("cell"))
		return false;
	long lval;
	wxString val;
	if (node->GetPropVal(wxT("start"), &val))
		SetStart(val);
	if (node->GetPropVal(wxT("end"), &val))
		SetEnd(val);
	if (node->GetPropVal(wxT("chapter"), &val))
		SetChapter(val == wxT("1") || val == wxT("on") || val == wxT("yes"));
	if (node->GetPropVal(wxT("program"), &val))
		SetProgram(val == wxT("1") || val == wxT("on") || val == wxT("yes"));
	if (node->GetPropVal(wxT("pause"), &val) && val == wxT("inf"))
		SetPause(-1);
	else if (node->GetPropVal(wxT("pause"), &val) && val.ToLong(&lval))
		SetPause(lval);
	if (node->GetChildren() && (node->GetChildren()->GetType() == wxSVGXML_TEXT_NODE
				|| node->GetChildren()->GetType() == wxSVGXML_CDATA_SECTION_NODE))
		SetCommands(node->GetChildren()->GetContent().Strip(wxString::both));
	return true;
}
