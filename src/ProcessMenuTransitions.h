/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessMenuTransitions.h
// Author:      Alex Thuering
// Created:     27.09.2014
// RCS-ID:      $Id: ProcessMenuTransitions.h,v 1.3 2016/12/11 10:06:31 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_MENU_TRANSITIONS_H
#define DS_PROCESS_MENU_TRANSITIONS_H

#include "ProcessTranscode.h"
class Menu;
class MenuObject;

/**
 * Implements process of menu transitions generation
 */
class ProcessMenuTransitions: public ProcessTranscode {
public:
	/** Constructor */
	ProcessMenuTransitions(ProgressDlg* progressDlg, DVD* dvd, wxString dvdTmpDir);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    DVD* dvd;
    wxString dvdTmpDir;
    Menu* menu;
	/** Generates a transition from menu to title and join it with title video*/
	bool GenerateMenuTransition(Menu* menu, Vob* vob, DVD* dvd, const wxString& dvdTmpDir);
	/** Generates a transition from menu to title */
	bool GenerateMenuTransitionVideo(Menu* menu, MenuObject* menuObj, const wxString& videoFile, double transitionDur,
			const wxString& transitionFile);
};

#endif // DS_PROCESS_MENU_TRANSITIONS_H
