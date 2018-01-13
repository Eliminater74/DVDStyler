//////////////////////////////////////////////////////////////////////////////
// Name:        Slideshow.cpp
// Purpose:     The class to store information for slideshow
// Author:      Alex Thuering
// Created:	26.06.2005
// RCS-ID:      $Id: Slideshow.cpp,v 1.16 2016/10/23 19:04:58 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
//////////////////////////////////////////////////////////////////////////////

#include "Slideshow.h"
#include "Config.h"
#include "Utils.h"
#include <wx/filename.h>
#include <wxVillaLib/utils.h>
#include <wxSVGXML/svgxmlhelpr.h>
#include <wxSVG/SVGDocument.h>
#include <wxSVG/ExifHandler.h>

#define TRANSITIONS_DIR wxFindDataDirectory(wxT("transitions"))

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Slide ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

wxSvgXmlNode* Slide::GetXML() {
	wxSvgXmlNode* node = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, _T("slide"));
	node->AddProperty(_T("filename"), m_filename);
	if (m_transition.length())
		node->AddProperty(_T("transition"), m_transition);
	return node;
}

bool Slide::PutXML(wxSvgXmlNode* node) {
	if (node == NULL || node->GetName() != _T("slide"))
		return false;
	wxString val;

	node->GetPropVal(wxT("filename"), &m_filename);
	if (node->GetPropVal(wxT("transition"), &val))
		m_transition = val;
	
	return true;
}

//////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Slideshow //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Slideshow::Slideshow(VideoFormat videoFormat, AspectRatio aspectRatio) {
	m_duration = s_config.GetDefSlideDuration();
	m_videoFormat = videoFormat;
	m_aspectRatio = aspectRatio;
	m_transition = s_config.GetDefTransition();
}

Slideshow::Slideshow(wxSvgXmlNode* node) {
	m_duration = s_config.GetDefSlideDuration();
	m_videoFormat = vfPAL;
	m_aspectRatio = ar4_3;
	PutXML(node);
}

Slideshow::Slideshow(const Slideshow& slideshow) {
	m_videoFormat = slideshow.m_videoFormat;
	m_aspectRatio = slideshow.m_aspectRatio;
	m_transition = slideshow.m_transition;
	m_duration = slideshow.m_duration;
	VECTOR_COPY(slideshow.m_slides, m_slides, Slide)
}

Slideshow::~Slideshow() {
	Clear();
}

wxSize Slideshow::GetResolution() {
	if (m_aspectRatio == ar16_9)
		return wxSize(720, 405);
	return wxSize(720, 540); // 4:3
}

float Slideshow::GetFPS() {
	if (m_videoFormat == vfNTSC)
		return 29.97;
	return 25; // vfPAL 
}

void Slideshow::Clear() {
	VECTOR_CLEAR(m_slides, Slide)
}

wxImage Slideshow::GetThumbImage(int width, int height) {
	wxBitmap thumbImg(width, height);
	wxMemoryDC dc;
	dc.SelectObject(thumbImg);
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxBrush(*wxBLACK, wxBRUSHSTYLE_SOLID));
	dc.DrawRectangle(0, 0, width, height);
	wxImage img[3];
	for (unsigned int i = 0; i < 3; i++) {
		if (m_slides.size() > i) {
#if wxCHECK_VERSION(2,9,0)
			img[i].SetOption(wxIMAGE_OPTION_MAX_WIDTH, 3 * width / 4);
			img[i].SetOption(wxIMAGE_OPTION_MAX_HEIGHT, 3 * height / 4);
#endif
			img[i].LoadFile(m_slides[i]->GetFilename());
			ExifHandler::rotateImage(m_slides[i]->GetFilename(), img[i]);
			float scale = (float) 3 * width / 4 / img[i].GetWidth();
			if (scale > (float) 3 * height / 4 / img[i].GetHeight())
				scale = (float) 3 * height / 4 / img[i].GetHeight();
			img[i].Rescale((int) (img[i].GetWidth() * scale), (int) (img[i].GetHeight() * scale));
		}
	}
	for (int i = 2; i >= 0; i--) {
		dc.SetPen(wxPen(*wxWHITE, 0, wxPENSTYLE_SOLID));
		if (img[i].Ok()) {
			int w = img[i].GetWidth();
			int h = img[i].GetHeight();
			int x = 2;
			int y = 2;
			if (i == 1) {
				int w0 = img[0].GetWidth();
				int h0 = img[0].GetHeight();
				int w2 = img[2].Ok() ? img[2].GetWidth() : img[0].GetWidth();
				int h2 = img[2].Ok() ? img[2].GetHeight() : img[0].GetHeight();
				x = (width + w0 - w2) / 2 - w / 2;
				y = (height + h0 - h2) / 2 - h / 2;
			} else if (i == 2) {
				x = width - w - 2;
				y = height - h - 2;
			}
			dc.DrawBitmap(wxBitmap(img[i]), x, y);
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.DrawRectangle(x, y, w, h);
		} else {
			int w = img[0].GetWidth();
			int h = img[0].GetHeight();
			int x = width / 2 - w / 2;
			int y = height / 2 - h / 2;
			if (i == 2) {
				x = width - w - 2;
				y = height - h - 2;
			}
			dc.SetBrush(wxBrush(*wxBLACK, wxBRUSHSTYLE_SOLID));
			dc.DrawRectangle(x, y, w, h);
		}
	}
	dc.SelectObject(wxNullBitmap);
	return thumbImg.ConvertToImage();
}

wxImage Slideshow::GetImage(int index) {
	int width = GetResolution().GetWidth();
	int height = GetResolution().GetHeight();
	wxString filename = m_slides[index]->GetFilename();
	wxImage img;
	if (index < (int) m_slides.size())
		img.LoadFile(filename);
	if (img.GetWidth() == 0 || img.GetHeight() == 0)
		return wxImage(GetFrameSize(m_videoFormat).GetWidth(), GetFrameSize(m_videoFormat).GetHeight());
	ExifHandler::rotateImage(filename, img);
	float scale = (float) width / img.GetWidth();
	if (scale > (float) height / img.GetHeight())
		scale = (float) height / img.GetHeight();
	img.Rescale((int) (img.GetWidth() * scale),
			(int) (img.GetHeight() * scale * GetFrameSize(m_videoFormat).GetHeight() / height));
	
	width = GetFrameSize(m_videoFormat).GetWidth();
	height = GetFrameSize(m_videoFormat).GetHeight();
	wxImage resImg(width, height);
	int x_offset1 = (width - img.GetWidth()) / 2;
	int x_offset2 = width - img.GetWidth() - x_offset1;
	int y_offset = (height - img.GetHeight()) / 2;
	unsigned char* src = img.GetData();
	unsigned char* dst = resImg.GetData() + y_offset * 3 * width;
	for (int y = 0; y < img.GetHeight(); y++) {
		dst += x_offset1 * 3;
		for (int x = 0; x < img.GetWidth(); x++) {
			dst[0] = src[0];
			dst[1] = src[1];
			dst[2] = src[2];
			src += 3;
			dst += 3;
		}
		dst += x_offset2 * 3;
	}
	return resImg;
}

/** Returns total duration of slideshow (sec) */
int Slideshow::GetResultDuration() {
	bool transition = GetTransition().length() && GetTransition() != wxT("none");
	return GetDuration() * GetCount() + (transition ? GetCount() - 1 : 0);
}

wxSvgXmlNode* Slideshow::GetXML(DVDFileType type, wxSvgXmlNode* node) {
	if (type != DVDSTYLER_XML)
		return node;

	node = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, _T("slideshow"));
	node->AddProperty(wxT("videoFormat"), (m_videoFormat == vfPAL) ? _T("PAL") : _T("NTSC"));
	node->AddProperty(wxT("aspectRatio"), wxString::Format(wxT("%d"), m_aspectRatio));
	node->AddProperty(wxT("duration"), wxString::Format(_T("%d"), m_duration));
	node->AddProperty(wxT("transition"), m_transition);
	
	for (int i = 0; i < (int) m_slides.size(); i++)
		node->AddChild(m_slides[i]->GetXML());
	return node;
}

bool Slideshow::PutXML(wxSvgXmlNode* node) {
	if (node == NULL || node->GetName() != _T("slideshow"))
		return false;

	wxString val;
	long lval;
	
	m_videoFormat = vfPAL;
	if (node->GetPropVal(_T("videoFormat"), &val) && val == _T("NTSC"))
		m_videoFormat = vfNTSC;
	if (node->GetPropVal(wxT("aspectRatio"), &val) && val.ToLong(&lval))
		m_aspectRatio = AspectRatio(lval);
	
	if (node->GetPropVal(_T("duration"), &val) && val.ToLong(&lval))
		m_duration = lval;
	if (node->GetPropVal(_T("transition"), &val))
		m_transition = val;
	
	wxSvgXmlNode* child = node->GetChildren();
	while (child) {
		if (child->GetName() == _T("slide"))
			m_slides.push_back(new Slide(child));
		child = child->GetNext();
	}
	
	return true;
}

void Slideshow::GetTransitions(wxArrayString& transitions, wxArrayString& labels) {
	wxArrayString tmpArray;
	labels.Clear();
	transitions.Clear();
	labels.Add(_("None"));
	labels.Add(_("Random"));
	transitions.Add(wxT("none"));
	transitions.Add(wxT("random"));
	wxString fname = wxFindFirstFile(TRANSITIONS_DIR + _T("*.xml"));
	while (!fname.IsEmpty()) {
		wxSVGDocument doc;
		doc.Load(fname);
		tmpArray.Add(wxGetTranslation((const wxChar*)doc.GetTitle().GetData()) + wxString(wxT("|"))
				+ wxFileName(fname).GetFullName());
		fname = wxFindNextFile();
	}
	tmpArray.Sort();
	for (int i = 0; i<(int)tmpArray.GetCount(); i++) {
		labels.Add(tmpArray[i].BeforeLast('|'));
		wxString fname = tmpArray[i].AfterLast('|');
		transitions.Add(fname);
	}
}
