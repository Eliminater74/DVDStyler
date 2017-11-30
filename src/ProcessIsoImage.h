/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessIsoImage.h
// Author:      Alex Thuering
// Created:     3.10.2014 (refactored)
// RCS-ID:      $Id: ProcessIsoImage.h,v 1.2 2014/10/20 06:08:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_ISO_IMAGE_H
#define DS_PROCESS_ISO_IMAGE_H

#include "Process.h"

class BurnDlg;

#define TMP_ISO wxT("dvd-out.iso")

/**
 * Implements process of ISO image generation
 */
class ProcessIsoImage: public Process {
public:
	/** Constructor */
	ProcessIsoImage(ProgressDlg* progressDlg, BurnDlg* burnDlg, DVD* dvd, Cache* cache, const wxString& dvdOutDir,
			const wxString& tmpDir);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    BurnDlg* burnDlg;
    DVD* dvd;
    Cache* cache;
    wxString dvdOutDir;
    wxString tmpDir;
};

#endif // DS_PROCESS_ISO_IMAGE_H
