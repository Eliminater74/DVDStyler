/////////////////////////////////////////////////////////////////////////////
// Name:        NewProjectDlg.h
// Purpose:     New project dialog
// Author:      Alex Thuering
// Created:     29.10.2006
// RCS-ID:      $Id: NewProjectDlg.h,v 1.9 2013/01/10 15:29:57 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef NEW_PROJECT_DLG_H
#define NEW_PROJECT_DLG_H

#include <wx/wx.h>
#include <wxVillaLib/PropDlg.h>
#include "DVD.h"

class NewProjectDlg: public wxPropDlg {
public:
	/**
	 * Constructor. 
	 */
	NewProjectDlg(wxWindow *parent, bool create = true);
	virtual ~NewProjectDlg() {}
	
	wxString GetLabel();
	DiscCapacity GetCapacity();
	int GetVideoBitrate();
	int GetAudioBitrate();
	VideoFormat GetVideoFormat();
	AudioFormat GetAudioFormat();
	AspectRatio GetAspectRatio();
	DefaultPostCommand GetDefPostCommand();
	
protected:
	int propIndex;
	virtual void CreatePropPanel(wxSizer* sizer);
	virtual void CreateDVDPropPanel(wxSizer* sizer, DVD* dvd);
	virtual bool SetValues();
	virtual bool Validate();
};

#endif // NEW_PROJECT_DLG_H
