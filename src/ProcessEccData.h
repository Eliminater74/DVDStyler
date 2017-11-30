/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessEccData.h
// Author:      Alex Thuering
// Created:     3.10.2014 (refactored)
// RCS-ID:      $Id: ProcessEccData.h,v 1.2 2014/10/20 06:08:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_ECC_DATA_H
#define DS_PROCESS_ECC_DATA_H

#include "Process.h"

class BurnDlg;

/**
 * Implements process of adding the ECC data
 */
class ProcessEccData: public Process {
public:
	/** Constructor */
	ProcessEccData(ProgressDlg* progressDlg, BurnDlg* burnDlg, const wxString& tmpDir);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    BurnDlg* burnDlg;
    wxString tmpDir;
};

#endif // DS_PROCESS_ECC_DATA_H
