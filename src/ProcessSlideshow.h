/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessSlideshow.h
// Author:      Alex Thuering
// Created:     26.09.2014 (refactored)
// RCS-ID:      $Id: ProcessSlideshow.h,v 1.3 2016/12/11 10:06:31 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_PROCESS_SLIDESHOW_H
#define DS_PROCESS_SLIDESHOW_H

#include "ProcessTranscode.h"
class Slideshow;
class Slide;
class wxSVGDocument;
class wxSVGElement;

/**
 * Implements process of slideshow generation
 */
class ProcessSlideshow: public ProcessTranscode {
public:
	/** Constructor */
	ProcessSlideshow(ProgressDlg* progressDlg, DVD* dvd, wxString dvdTmpDir);
	
	/** Executes process */
	virtual bool Execute();
    
	/** Returns true, if process need be executed */
    virtual bool IsNeedExecute();

private:
    DVD* dvd;
    int slideshowSubSteps;
    wxArrayPtrVoid slideshowVobs;
    bool GenerateSlideshow(Slideshow* slideshow, const wxString& vobFile, AudioFormat audioFormat, wxString audioFile,
    		int audioBitrate);
    bool LoadTransition(wxSVGDocument& svg, const wxString& name, Slideshow* slideshow, Slide* slide1, Slide* slide2);
    void SetAnimationDur(wxSVGElement* parent, double dur);
};

#endif // DS_PROCESS_SLIDESHOW_H
