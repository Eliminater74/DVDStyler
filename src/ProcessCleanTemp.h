/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessCleanTemp.h
// Author:      Alex Thuering
// Created:     4.10.2014 (refactored)
// RCS-ID:      $Id: ProcessCleanTemp.h,v 1.2 2014/10/20 06:08:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_CLEAN_TEMP_H
#define DS_PROCESS_CLEAN_TEMP_H

#include "Process.h"

/**
 * Implements process of temporary directory cleaning
 */
class ProcessCleanTemp: public Process {
public:
	/** Constructor */
	ProcessCleanTemp(ProgressDlg* progressDlg, const wxString& tmpDir, const wxString& dvdTmpDir, const wxString& dvdOutDir);
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();
    
    /** Returns true, if gauge need be updated */
    bool IsUpdateGauge();
	
	/** Executes process */
	virtual bool Execute();
	
	/** Removes given temporary directory and all temporary files in it */
	bool DeleteTempFiles(bool delOutDir);

private:
    wxString tmpDir;
    wxString dvdTmpDir;
    wxString dvdOutDir;
	
	/** Removes given directory and all files in it */
	bool DeleteDir(wxString dir);
};

#endif // DS_PROCESS_CLEAN_TEMP_H
