/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessBurn.h
// Author:      Alex Thuering
// Created:     3.10.2014 (refactored)
// RCS-ID:      $Id: ProcessBurn.h,v 1.2 2014/10/20 06:08:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_BURN_H
#define DS_PROCESS_BURN_H

#include "Process.h"

class BurnDlg;

/**
 * Implements burn process
 */
class ProcessBurn: public Process {
public:
	/** Constructor */
	ProcessBurn(ProgressDlg* progressDlg, BurnDlg* burnDlg, DVD* dvd, const wxString& dvdOutDir, const wxString& tmpDir);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    BurnDlg* burnDlg;
    DVD* dvd;
    wxString dvdOutDir;
    wxString tmpDir;
};

#endif // DS_PROCESS_BURN_H
