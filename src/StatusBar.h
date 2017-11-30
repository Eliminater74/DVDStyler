/////////////////////////////////////////////////////////////////////////////
// Name:        StatusBar.h
// Purpose:     Status bar for main window
// Author:      Alex Thuering
// Created:     28.12.2008
// RCS-ID:      $Id: StatusBar.h,v 1.4 2014/09/27 06:01:55 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
#ifndef STATUSBAR_H_
#define STATUSBAR_H_

#include <wx/statusbr.h>
#include <wx/gauge.h>
#include <wx/choice.h>
#include <wx/stattext.h>

class StatusBar: public wxStatusBar {
public:
	StatusBar(wxWindow* parent);
	virtual ~StatusBar();

	/** Sets fill status of DVD */
	void SetFillStatus(int dur, int durTotal, int resultBitrate);

	void OnSize(wxSizeEvent& event);

private:
	wxGauge* m_gauge;
	wxStaticText* m_fillText;
	wxStaticText* m_bitrate;
	DECLARE_EVENT_TABLE()
};

#endif // STATUSBAR_H_
