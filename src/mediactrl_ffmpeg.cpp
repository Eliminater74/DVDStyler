/////////////////////////////////////////////////////////////////////////////
// Name:        mediactrl_ffmpeg.cpp
// Purpose:     Displays video
// Author:      Alex Thuering
// Created:     21.01.2011
// RCS-ID:      $Id: mediactrl_ffmpeg.cpp,v 1.12 2015/08/02 17:31:18 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "mediactrl_ffmpeg.h"
#include <wx/wx.h>
#include <wx/dcclient.h>
#include <wxSVG/mediadec_ffmpeg.h>

using namespace std;

class FFEvtHandler: public wxEvtHandler {
public:
	FFEvtHandler(wxMediaCtrl* ctrl): m_ctrl(ctrl) {
		m_videoFormat = vfPAL;
		m_aspectRatio = ar4_3;
		m_frameAspectRatio = 1;
	}
	
	virtual ~FFEvtHandler() {
		// nothing to do
	}

	inline wxImage GetImage() {
		return m_image;
	}

	inline void SetImage(wxImage image) {
		m_image = image;
		m_bitmap = wxBitmap();
		m_ctrl->Refresh();
	}
	
	inline void SetFrameAspectRatio(float frameAspectRatio) {
		m_frameAspectRatio = frameAspectRatio;
	}
	
	inline void SetVideoFormat(VideoFormat videoFormat, AspectRatio aspectRatio,
			const vector<int>& pad, const vector<int>& crop) {
		m_videoFormat = videoFormat;
		m_aspectRatio = aspectRatio;
		m_pad = pad;
		m_crop = crop;
		m_bitmap = wxBitmap();
		m_ctrl->Refresh();
	}

protected:
	void OnPaint(wxPaintEvent &event) {
		wxPaintDC dc(m_ctrl);
		if (!m_bitmap.Ok()) {
			if (!m_image.Ok())
				return;
			wxImage img = m_image;
			double aspect = m_frameAspectRatio;
			if (m_aspectRatio != arAUTO)
				aspect = m_aspectRatio == ar4_3 ? 4.0/3 : 16.0/9;
			wxSize frameSize = m_videoFormat != vfCOPY ? GetFrameSize(m_videoFormat)
					: wxSize(img.GetWidth(), img.GetHeight());
			if (m_crop.size() == 4 && m_crop[0] + m_crop[1] + m_crop[2] + m_crop[3] > 0) {
				int w = img.GetWidth() - m_crop[0] - m_crop[1];
				int h = img.GetHeight() - m_crop[2] - m_crop[3];
				img = img.GetSubImage(wxRect(m_crop[0], m_crop[2], w > 0 ? w : 0, h > 0 ? h : 0));
			}
			wxSize size = m_ctrl->GetClientSize();
			m_dspSize = size;
			m_dspPos = wxPoint();
			if (size.y >= size.x/aspect) {
				m_dspSize.y = size.x/aspect;
				m_dspPos.y = (size.y - m_dspSize.y)/2;
			} else {
				m_dspSize.x = size.y*aspect;
				m_dspPos.x = (size.x - m_dspSize.x)/2;
			}
			wxSize bmpSize = m_dspSize;
			m_bmpPos = m_dspPos;
			if (m_pad.size() == 4 && m_pad[0] + m_pad[1] + m_pad[2] + m_pad[3] > 0) {
				int padX = m_pad[0] + m_pad[1];
				int padY = m_pad[2] + m_pad[3];
				bmpSize.x -= padX*m_dspSize.x/frameSize.x;
				bmpSize.y -= padY*m_dspSize.y/frameSize.y;
				m_bmpPos.x += m_pad[0]*m_dspSize.x/frameSize.x;
				m_bmpPos.y += m_pad[2]*m_dspSize.y/frameSize.y;
			}
			m_bitmap = wxBitmap(img.Scale(bmpSize.x, bmpSize.y));
		}
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.SetBrush(*wxBLACK_BRUSH);
		dc.DrawRectangle(m_dspPos, m_dspSize);
		dc.DrawBitmap(m_bitmap, m_bmpPos);
	}

	void OnResize(wxSizeEvent &event) {
		m_bitmap = wxBitmap();
		m_ctrl->Refresh();
	}

private:
	wxMediaCtrl* m_ctrl;
	wxImage m_image;
	float m_frameAspectRatio;
	VideoFormat m_videoFormat;
	AspectRatio m_aspectRatio;
	vector<int> m_pad;
	vector<int> m_crop;
	wxSize m_dspSize;
	wxPoint m_dspPos;
	wxBitmap m_bitmap;
	wxPoint m_bmpPos;
	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(FFEvtHandler, wxEvtHandler)
  EVT_PAINT(FFEvtHandler::OnPaint)
  EVT_SIZE(FFEvtHandler::OnResize)
END_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////////////
/////////////////////////// wxFFMediaBackend ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
class wxFFMediaBackend: public wxMediaBackendCommonBase {
public:
	wxFFMediaBackend(): m_loaded(false), m_duration(0), m_evtHandler(NULL) {}
	virtual ~wxFFMediaBackend() {
		if (m_evtHandler != NULL) {
			m_ctrl->PopEventHandler(true);
			m_evtHandler = NULL;
		}
	}
	
	virtual bool CreateControl(wxControl* ctrl, wxWindow* parent, wxWindowID id, const wxPoint& pos,
			const wxSize& size, long style, const wxValidator& validator, const wxString& name) {
		if (!ctrl->wxControl::Create(parent, id, pos, size, style, validator, name))
			return false;
		m_ctrl = wxStaticCast(ctrl, wxMediaCtrl);
		m_evtHandler = new FFEvtHandler(m_ctrl);
		m_ctrl->PushEventHandler(m_evtHandler);
		return true;
	}
	
	virtual bool Load(const wxString& fileName) {
		if (fileName.length() && m_decoder.Load(fileName)) {
			m_loaded = true;
			m_duration = m_decoder.GetDuration();
			if (m_duration < 0) {
				if (m_decoder.SetPosition(360000, false))
					m_duration = m_decoder.GetPosition();
				m_decoder.SetPosition(0, false);
			}
			NotifyMovieLoaded();
			m_evtHandler->SetImage(m_decoder.GetNextFrame());
			m_evtHandler->SetFrameAspectRatio(m_decoder.GetFrameAspectRatio());
			return true;
		}
		m_loaded = false;
		m_evtHandler->SetImage(wxImage());
		return false;
	}
	
	virtual bool SetPosition(wxLongLong where) {
		if (!m_loaded)
			return false;
		double dpos = where.ToDouble()/1000;
		if (dpos == 0 && m_decoder.GetPosition() != 0) {
			m_decoder.SetPosition(0, false);
			for (int i = 0; i < 8; i++) {
				m_decoder.GetNextFrame();
			}
		}
		m_decoder.SetPosition(dpos > 1.0 ? dpos - 1.0 : 0.0, dpos != 0);
		wxImage image;
		for (int i = 0; i < 60; i++) {
			image = m_decoder.GetNextFrame();
			if (m_decoder.GetPosition() >= dpos)
				break;
		}
		m_evtHandler->SetImage(image);
		return true;
	}
	
	virtual wxLongLong GetPosition() {
		wxLongLong res;
		if (m_loaded)
			res.Assign(m_decoder.GetPosition()*1000);
		return res;
	}
	
	virtual wxLongLong GetDuration() {
		wxLongLong res;
		if (m_loaded)
			res.Assign(m_duration*1000);
		return res;
	}
	
	void SetVideoFormat(VideoFormat videoFormat, AspectRatio aspectRatio, const vector<int>& pad, const vector<int>& crop) {
		m_evtHandler->SetVideoFormat(videoFormat, aspectRatio, pad, crop);
	}
	
	inline double GetFps() {
		return m_decoder.GetFps();
	}
	
	inline wxFfmpegMediaDecoder& GetDecoder() {
		return m_decoder;
	}

	inline wxImage GetImage() {
		return m_evtHandler->GetImage();
	}
	
private:
	bool m_loaded;
	wxFfmpegMediaDecoder m_decoder;
	double m_duration;
	FFEvtHandler* m_evtHandler;
	DECLARE_DYNAMIC_CLASS(wxFFMediaBackend)
};

IMPLEMENT_DYNAMIC_CLASS(wxFFMediaBackend, wxMediaBackend)

/////////////////////////////////////////////////////////////////////////////
////////////////////////////// MediaCtrlFF //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
MediaCtrlFF::MediaCtrlFF(wxWindow* parent, wxWindowID id, const wxString& fileName, const wxPoint& pos,
		const wxSize& size, long style, const wxValidator& validator, const wxString& name):
        wxMediaCtrl(parent, id, fileName, pos, size, style, wxT("wxFFMediaBackend"), validator, name) {
	// nothing to do
	if (m_imp == NULL) {
		m_imp = new wxFFMediaBackend();
		if (!m_imp->CreateControl(this, parent, id, pos, size, style, validator, name)) {
		    delete m_imp;
		    m_imp = NULL;
		}
	}
}

MediaCtrlFF::~MediaCtrlFF() {
	// nothing to do
}

void MediaCtrlFF::SetVideoFormat(VideoFormat videoFormat, AspectRatio aspectRatio,
		const vector<int>& pad, const vector<int>& crop) {
	((wxFFMediaBackend*) m_imp)->SetVideoFormat(videoFormat, aspectRatio, pad, crop);
}

double MediaCtrlFF::GetFps() {
	return ((wxFFMediaBackend*) m_imp)->GetFps();
}

wxFfmpegMediaDecoder& MediaCtrlFF::GetDecoder() {
	return ((wxFFMediaBackend*) m_imp)->GetDecoder();
}

wxImage MediaCtrlFF::GetImage() {
	return ((wxFFMediaBackend*) m_imp)->GetImage();
}
