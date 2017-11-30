/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessMenu.h
// Author:      Alex Thuering
// Created:     26.09.2014 (refactored)
// RCS-ID:      $Id: ProcessMenu.h,v 1.3 2016/12/11 10:06:31 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_MENU_H
#define DS_PROCESS_MENU_H

#include "ProcessTranscode.h"
class Menu;

/**
 * Implements process of menu generation
 */
class ProcessMenu: public ProcessTranscode {
public:
	/** Constructor */
	ProcessMenu(ProgressDlg* progressDlg, DVD* dvd, wxString dvdTmpDir);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    DVD* dvd;
    int menuSubSteps;
	wxArrayPtrVoid menuVobs;
	wxArrayInt menuWSTypes;
	bool GenerateMenu(Menu* menu, WidescreenType widescreenType, const wxString& menuFile, AudioFormat audioFormat,
			wxString audioFile, int audioBitrate);
};

#endif // DS_PROCESS_MENU_H
