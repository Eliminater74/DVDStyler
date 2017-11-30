/////////////////////////////////////////////////////////////////////////////
// Name:        StatusBar.cpp
// Purpose:     Status bar for main window
// Author:      Alex Thuering
// Created:     28.12.2008
// RCS-ID:      $Id: StatusBar.cpp,v 1.9 2017/11/25 15:56:34 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
#include "StatusBar.h"
#include "DVD.h"

class StatSideText: public wxControl {
public:
	StatSideText(wxWindow *parent): wxControl(parent, wxID_ANY) {}
	~StatSideText() {};
	virtual void OnPaint(wxPaintEvent& event);
	virtual void OnEraseBackground(wxEraseEvent &event) {}
private:
	DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(StatSideText, wxControl)
	EVT_PAINT(StatSideText::OnPaint)
	EVT_ERASE_BACKGROUND(StatSideText::OnEraseBackground)
END_EVENT_TABLE()

void StatSideText::OnPaint(wxPaintEvent &event) {
	wxPaintDC dc(this);
	wxCoord w, h, cw, ch;
	const wxString& label = wxControl::GetLabel();
	dc.GetTextExtent(label, &w, &h);
	GetClientSize(&cw, &ch);
	dc.DrawText(label, (cw-w)/2, 0);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// StatusBar ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(StatusBar, wxStatusBar)
	EVT_SIZE(StatusBar::OnSize)
END_EVENT_TABLE()

StatusBar::StatusBar(wxWindow* parent): wxStatusBar(parent, wxID_ANY) {
	static const int widths[] = { -1, 150, 136, 80 };
	SetFieldsCount(4, widths);
	m_gauge = new wxGauge(this, wxID_ANY, 100);
	m_fillText = new wxStaticText(this, wxID_ANY, wxT(""));
	m_bitrate = new wxStaticText(this, wxID_ANY, wxT(""));
}

StatusBar::~StatusBar() {
	// nothing to do
}

/** Sets fill status */
void StatusBar::SetFillStatus(int dur, int durTotal, int resultBitrate) {
	if (durTotal > 0)
		m_fillText->SetLabel(wxString::Format(_("%d/%d Minutes"), dur, durTotal));
	else
		m_fillText->SetLabel(wxString::Format(_("%d Minutes"), dur));
	m_fillText->SetForegroundColour(durTotal > 0 && dur > durTotal ? *wxRED : *wxBLACK);
	wxString text = resultBitrate % 1000 == 0
			? wxString::Format(_("%d Mb/s"), resultBitrate/1000)
			: wxString::Format(_("%.1f Mb/s"), ((double)resultBitrate)/1000);
	m_bitrate->SetLabel(text);
	m_bitrate->SetForegroundColour(resultBitrate >= 4500 ? *wxBLACK :
			(resultBitrate >= 3000 ? wxColour(200, 100, 0) : *wxRED));
	m_gauge->SetValue(durTotal > 0 ? dur * 100 / durTotal : 0);
	Update();
}

void StatusBar::OnSize(wxSizeEvent& event) {
	wxRect rect;
	GetFieldRect(1, rect);
	m_gauge->SetSize(rect.x + 1, rect.y + 1, rect.width - 2, rect.height - 2);
	GetFieldRect(2, rect);
	m_fillText->SetSize(rect.x + 3, rect.y + 3, rect.width - 3, rect.height - 3);
	GetFieldRect(3, rect);
	m_bitrate->SetSize(rect.x + 3, rect.y + 3, rect.width - 3, rect.height - 3);
	event.Skip();
}
