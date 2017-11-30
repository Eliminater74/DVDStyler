/////////////////////////////////////////////////////////////////////////////
// Name:        Cell.h
// Purpose:     The class to store a DVD cell data
// Author:      Alex Thuering
// Created:	    29.01.2003
// RCS-ID:      $Id: Cell.h,v 1.3 2016/03/03 16:23:22 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef CELL_H
#define CELL_H

#include "DVDAction.h"

class wxSvgXmlNode;
class wxSvgXmlDocument;
class DVD;

/** The cell. It defines a marker point in a title. */
class Cell {
public:
	/** Constructor */
	Cell();
	/** Constructor */
	Cell(long start, bool chapter = true);
	
	/** Returns the start time (ms) */
	long GetStart() const { return m_start; }
	/** Returns the start time as string */
	wxString GetStartStr() const;
	/** Sets the start time (ms) */
	void SetStart(long start) { m_start = start; }
	/** Sets the start time */
	void SetStart(const wxString& startStr);
	
	/** Returns the end time (ms) */
	long GetEnd() const { return m_end; }
	/** Returns the end time as string */
	wxString GetEndStr() const;
	/** Sets the end time (ms) */
	void SetEnd(long end) { m_end = end; }
	/** Sets the end time */
	void SetEnd(const wxString& endStr);
	
	/** Returns true if cell is a chapter point */
	bool IsChapter() const { return m_chapter; }
	/** Sets cell as chapter point */
	void SetChapter(bool chapter) { m_chapter = chapter; }
	
	/** Returns true if cell is a program point */
	bool IsProgram() const { return m_program; }
	/** Sets cell as program point */
	void SetProgram(bool program) { m_program = program; }
	
	/** Returns the pause */
	int GetPause() const { return m_pause; }
	/** Sets the pause */
	void SetPause(int pause) { m_pause = pause; }
	
	/** Returns VM commands that will be executed when the cell plays. */
	wxString GetCommands() const { return m_commands; }
	/** Sets the VM commands that will be executed when the cell plays. */
	void SetCommands(const wxString& commands) { m_commands = commands; }
		
	wxSvgXmlNode* GetXML(DVDFileType type, long endTime, DVD* dvd, int nextTitle);
	bool PutXML(wxSvgXmlNode* node);
	
private:
	long m_start;
	long m_end;
	bool m_chapter;
	bool m_program;
	int m_pause;
	wxString m_commands;
};

void ReplaceJumpNextTitle(wxString& commands, DVD* dvd, int nextTitle);

#endif // CELL_H
