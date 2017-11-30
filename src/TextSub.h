/////////////////////////////////////////////////////////////////////////////
// Name:        TextSub.h
// Purpose:     The class to store a DVD TextSub parameters
// Author:      Alex Thuering
// Created:	    09.04.2011
// RCS-ID:      $Id: TextSub.h,v 1.4 2016/04/19 23:09:52 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef TEXTSUB_H
#define TEXTSUB_H

#include "mediaenc.h"
#include "DVDAction.h"
#include <wx/wx.h>
#include <wx/dynarray.h>
#include <map>

using namespace std;
typedef map<wxString, map<wxString, wxString> > FontMap;

class wxSvgXmlNode;

class TextSub {
public:
	TextSub(wxString filename = wxT(""));

	wxString GetFilename() { return m_filename; }
	void SetFilename(wxString value) { m_filename = value; }
	
	bool GetForce() { return m_force; }
	void SetForce(bool value) { m_force = value; }
	
	wxString GetCharacterSet() { return m_characterSet; }
	void SetCharacterSet(const wxString& value) { m_characterSet = value; }
	
	wxString GetFontFile();
	
	wxString GetFontFamily() { return m_fontFamily; }
	void SetFontFamily(const wxString& value) { m_fontFamily = value; }
	
	wxString GetFontStyle() { return m_fontStyle; }
	void SetFontStyle(const wxString& value) { m_fontStyle = value; }

	double GetFontSize() { return m_fontSize; }
	void SetFontSize(double value) { m_fontSize = value; }
	
    wxColour GetFillColour() { return m_fillColour; }
    void SetFillColour(const wxColour& value) { m_fillColour = value; }
    
    wxColour GetOutlineColour() { return m_outlineColour; }
    void SetOutlineColour(const wxColour& value) { m_outlineColour = value; }
    
    double GetOutlineThickness() { return m_outlineThickness; }
    void SetOutlineThickness(double value) { m_outlineThickness = value; }
    
    wxColour GetShadowColour() { return m_shadowColour; }
    void SetShadowColour(const wxColour& value) { m_shadowColour = value; }
    
    wxPoint GetShadowOffset() { return m_shadowOffset; }
    void SetShadowOffset(const wxPoint& value) { m_shadowOffset = value; }
	
	wxAlignment GetAlignment() { return m_alignment; }
	void SetAlignment(wxAlignment value) { m_alignment = value; }
	
	int GetLeftMargin() { return m_leftMargin; }
	void SetLeftMargin(int value) { m_leftMargin = value; }
	
	int GetRightMargin() { return m_rightMargin; }
	void SetRightMargin(int value) { m_rightMargin = value; }
	
	int GetTopMargin() { return m_topMargin; }
	void SetTopMargin(int value) { m_topMargin = value; }
	
	int GetBottomMargin() { return m_bottomMargin; }
	void SetBottomMargin(int value) { m_bottomMargin = value; }
	
	double GetSubtitleFps() { return m_subtitleFps; }
	void SetSubtitleFps(double value) { m_subtitleFps = value; }
	
	double GetMovieFps() { return m_movieFps; }
	void SetMovieFps(double value) { m_movieFps = value; }
	
	wxSize GetMovieSize() { return m_movieSize; }
	void SetMovieSize(wxSize value) { m_movieSize = value; }
	
	AspectRatio GetAspectRatio() { return m_aspectRatio; }
	void SetAspectRatio(AspectRatio value) { m_aspectRatio = value; }
	
	wxSvgXmlNode* GetXML(DVDFileType type);
	bool PutXML(wxSvgXmlNode* node);
	
	/** saves a configuration for spumux */
	bool SaveSpumux(const wxString& filename, VideoFormat videoFormat);

	/** Returns subtitle attributes as sting */
	wxString AsString();
    
    /** Returns font map (font family -> font style -> font filename) */
    static FontMap& GetFontMap();
    /** Returns true, if font map is initialized */
	static bool IsFontMapInitialized();
	
private:
    wxString m_filename;
    bool m_force;
    wxString m_characterSet;
    wxString m_fontFamily;
    wxString m_fontStyle;
    double m_fontSize;
    wxColour m_fillColour;
    wxColour m_outlineColour;
    double m_outlineThickness;
    wxColour m_shadowColour;
    wxPoint m_shadowOffset;
    wxAlignment m_alignment;
    int m_leftMargin;
    int m_rightMargin;
    int m_topMargin;
    int m_bottomMargin;
    double m_subtitleFps;
    double m_movieFps;
    wxSize m_movieSize;
    AspectRatio m_aspectRatio;
    static FontMap s_fonts;
};

WX_DEFINE_ARRAY(TextSub*, TextSubArray);

#endif // TEXTSUB_H
