/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessPreview.h
// Author:      Alex Thuering
// Created:     3.10.2014 (refactored)
// RCS-ID:      $Id: ProcessPreview.h,v 1.2 2014/10/20 06:08:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_PREVIEW_H
#define DS_PROCESS_PREVIEW_H

#include "Process.h"

class BurnDlg;

/**
 * Implements preview process
 */
class ProcessPreview: public Process {
public:
	/** Constructor */
	ProcessPreview(ProgressDlg* progressDlg, BurnDlg* burnDlg, const wxString& dvdOutDir);
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();
    
    /** Returns true, if gauge need be updated */
    virtual bool IsUpdateGauge();
	
	/** Executes process */
	virtual bool Execute();

private:
    BurnDlg* burnDlg;
    wxString dvdOutDir;
};

#endif // DS_PROCESS_PREVIEW_H
