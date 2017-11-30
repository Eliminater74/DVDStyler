/////////////////////////////////////////////////////////////////////////////
// Name:        DVDPropDlg.h
// Purpose:     DVD properties dialog
// Author:      Alex Thuering
// Created:     7.03.2005
// RCS-ID:      $Id: DVDPropDlg.h,v 1.4 2010/05/28 19:46:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
 
#ifndef DVD_PROP_DLG_H
#define DVD_PROP_DLG_H

#include <wx/wx.h>
#include <wxVillaLib/PropDlg.h>
#include "NewProjectDlg.h"

class DVDPropDlg: public NewProjectDlg {
public:
    DVDPropDlg(wxWindow *parent, DVD* dvd);
	virtual ~DVDPropDlg() {}

protected:
	void CreatePropPanel(wxSizer* sizer);
	bool SetValues();

private:
	DVD* m_dvd;
	/** Returns list of possible first play commands */
	wxArrayString GetFPCommands();
};

#endif // DVD_PROP_DLG_H
