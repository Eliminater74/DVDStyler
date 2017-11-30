/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessFormatDvd.h
// Author:      Alex Thuering
// Created:     3.10.2014 (refactored)
// RCS-ID:      $Id: ProcessFormatDvd.h,v 1.2 2014/10/20 06:08:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_FORMAT_DVD_H
#define DS_PROCESS_FORMAT_DVD_H

#include "Process.h"

class BurnDlg;

/**
 * Implements format DVD process
 */
class ProcessFormatDvd: public Process {
public:
	/** Constructor */
	ProcessFormatDvd(ProgressDlg* progressDlg, BurnDlg* burnDlg);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    BurnDlg* burnDlg;
};

#endif // DS_PROCESS_FORMAT_DVD_H
