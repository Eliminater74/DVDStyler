/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessSubtitles.h
// Author:      Alex Thuering
// Created:     26.09.2014 (refactored)
// RCS-ID:      $Id: ProcessSubtitles.h,v 1.3 2016/12/11 10:06:31 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_SUBTITLES_H
#define DS_PROCESS_SUBTITLES_H

#include "ProcessTranscode.h"
class TextSub;

/**
 * Implements process of subtitles multiplexing
 */
class ProcessSubtitles: public ProcessTranscode {
public:
	/** Constructor */
	ProcessSubtitles(ProgressDlg* progressDlg, DVD* dvd, Cache* cache, wxString dvdTmpDir);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    DVD* dvd;
    Cache* cache;
    int subtitleSubSteps;
	wxArrayPtrVoid subtitleVobs;
	wxArrayInt subtitleTsi;
	bool MultiplexSubtitles(const wxString& vobFile, TextSub* textSub, unsigned int streamIdx, VideoFormat videoFormat,
			const wxString& resultFile);
};

#endif // DS_PROCESS_SUBTITLES_H
