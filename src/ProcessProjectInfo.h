/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessProjectInfo.h
// Author:      Alex Thuering
// Created:     4.10.2014 (refactored)
// RCS-ID:      $Id: ProcessProjectInfo.h,v 1.2 2014/10/20 06:08:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_PROJECT_INFO_H
#define DS_PROCESS_PROJECT_INFO_H

#include "Process.h"

/**
 * Prints project info
 */
class ProcessProjectInfo: public Process {
public:
	/** Constructor */
	ProcessProjectInfo(ProgressDlg* progressDlg, DVD* dvd);
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();
    
    /** Returns true, if gauge need be updated */
    virtual bool IsUpdateGauge();
	
	/** Executes process */
	virtual bool Execute();

private:
    DVD* dvd;
    void PrintInfoLine(const wxString& info, unsigned int inputSize, unsigned int outputSize, double outputDuration,
    		int bitrate);
};

#endif // DS_PROCESS_PROJECT_INFO_H
