/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessDvdFilesystem.h
// Author:      Alex Thuering
// Created:     26.09.2014 (refactored)
// RCS-ID:      $Id: ProcessDvdFilesystem.h,v 1.2 2014/10/20 06:08:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_DVD_FILESYSTEM_H
#define DS_PROCESS_DVD_FILESYSTEM_H

#include "Process.h"

/**
 * Implements process of DVD file system generation
 */
class ProcessDvdFilesystem: public Process {
public:
	/** Constructor */
	ProcessDvdFilesystem(ProgressDlg* progressDlg, DVD* dvd, const wxString& dvdTmpDir, const wxString& dvdOutDir);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    DVD* dvd;
    wxString dvdTmpDir;
    wxString dvdOutDir;
	void PrintProjectTranscodedInfo();
	void PrintInfoLine(const wxString& info, int transcodedSize);
};

#endif // DS_PROCESS_DVD_FILESYSTEM_H
