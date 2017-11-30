/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessEncode.h
// Author:      Alex Thuering
// Created:     26.09.2014 (refactored)
// RCS-ID:      $Id: ProcessEncode.h,v 1.3 2016/12/15 20:46:28 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_ENCODE_H
#define DS_PROCESS_ENCODE_H

#include "ProcessTranscode.h"

/**
 * Implements encoding process
 */
class ProcessEncode: public ProcessTranscode {
public:
	/** Constructor */
	ProcessEncode(ProgressDlg* progressDlg, DVD* dvd, Cache* cache, wxString dvdTmpDir);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    DVD* dvd;
    Cache* cache;
    int titleSubSteps;
	wxArrayPtrVoid titleVobs;
	wxArrayPtrVoid titleVobs2;
	wxArrayInt titleAspects;
};

#endif // DS_PROCESS_ENCODE_H
