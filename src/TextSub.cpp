/////////////////////////////////////////////////////////////////////////////
// Name:        TextSub.cpp
// Purpose:     The class to store a DVD TextSub parameters
// Author:      Alex Thuering
// Created:	    09.04.2011
// RCS-ID:      $Id: TextSub.cpp,v 1.7 2016/04/19 23:09:52 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "TextSub.h"
#include "Utils.h"
#include "Config.h"
#include <wxVillaLib/utils.h>
#include <wxSVGXML/svgxmlhelpr.h>
#include <wx/sstream.h>

#include <fontconfig/fontconfig.h>
#define FONTS_CONF wxFindDataFile(wxT("fonts.conf"))

FontMap TextSub::s_fonts;

TextSub::TextSub(wxString filename) {
	m_filename = filename;
	m_force = false;
	m_characterSet = s_config.GetSubtitlesCharacterSet();
	m_fontFamily = s_config.GetSubtitlesFontFamily();
	m_fontStyle = s_config.GetSubtitlesFontStyle();
	m_fontSize = s_config.GetSubtitlesFontSize();
	m_fillColour = s_config.GetSubtitlesFillColour();
	m_outlineColour = s_config.GetSubtitlesOutlineColour();
    m_outlineThickness = s_config.GetSubtitlesOutlineThickness();
    m_shadowColour = s_config.GetSubtitlesShadowColour();
    m_shadowOffset = wxPoint(s_config.GetSubtitlesShadowOffsetX(), s_config.GetSubtitlesShadowOffsetY());
	m_alignment = (wxAlignment) s_config.GetSubtitlesAlignment();
	m_leftMargin = s_config.GetSubtitlesLeftMargin();
	m_rightMargin = s_config.GetSubtitlesRightMargin();
	m_topMargin = s_config.GetSubtitlesTopMargin();
	m_bottomMargin = s_config.GetSubtitlesBottomMargin();
	m_subtitleFps = 25;
	m_movieFps = 25;
	m_movieSize = wxSize(720, 574);
	m_aspectRatio = arAUTO;
}

wxString TextSub::GetFontFile() {
	map<wxString, wxString>& fonts = GetFontMap()[m_fontFamily];
	if (fonts.find(m_fontStyle) != fonts.end())
		return fonts[m_fontStyle];
	if (fonts.find(wxT("Normal")) != fonts.end())
		return fonts[wxT("Normal")];
	if (fonts.find(wxT("Standard")) != fonts.end())
		return fonts[wxT("Standard")];
	return fonts.begin() != fonts.end() ? fonts.begin()->second : wxT("");
}

wxString ToString(const wxColour& colour, DVDFileType type) {
	if (!colour.IsOk())
		return type == DVDAUTHOR_XML ? wxT("rgba(0,0,0,0)") : wxT("#00000000");
	return wxString::Format(type == DVDAUTHOR_XML ? wxT("rgba(%d,%d,%d,%d)") : wxT("#%02x%02x%02x%02x"),
			colour.Red(), colour.Green(), colour.Blue(), colour.Alpha());
}

wxSvgXmlNode* TextSub::GetXML(DVDFileType type) {
	wxSvgXmlNode* textsubNode = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, _T("textsub"));
	TextSub* defaults = new TextSub(wxT(""));
	textsubNode->AddProperty(_T("filename"), m_filename);
	if (type == DVDAUTHOR_XML || m_characterSet != defaults->m_characterSet)
		textsubNode->AddProperty(_T("characterset"), m_characterSet);
	if (m_force)
		textsubNode->AddProperty(_T("force"), _T("yes"));
	
	// font
	if (type == DVDAUTHOR_XML) {
		if (GetFontFile().length() > 0)
			textsubNode->AddProperty(wxT("font"), GetFontFile());
	} else {
		if (m_fontFamily != defaults->m_fontFamily)
			textsubNode->AddProperty(wxT("fontFamily"), m_fontFamily);
		if (m_fontStyle != defaults->m_fontStyle)
			textsubNode->AddProperty(wxT("fontStyle"), m_fontStyle);
	}
	if (m_fontSize != defaults->m_fontSize)
		textsubNode->AddProperty(_T("fontsize"), wxString::Format(wxT("%g"), m_fontSize));
	
	// colours
	textsubNode->AddProperty(wxT("fill-color"), ToString(m_fillColour, type));
	textsubNode->AddProperty(wxT("outline-color"), ToString(m_outlineColour, type));
	textsubNode->AddProperty(wxT("outline-thickness"), wxString::Format(wxT("%f"), m_outlineThickness));
	textsubNode->AddProperty(wxT("shadow-color"), ToString(m_shadowColour, type));
	if (m_shadowOffset != defaults->m_shadowOffset)
		textsubNode->AddProperty(wxT("shadow-offset"),
				wxString::Format(wxT("%d,%d"), m_shadowOffset.x, m_shadowOffset.y));
	
	// alignment
	wxString hAlignment = m_alignment & wxALIGN_CENTER_HORIZONTAL ? wxT("center")
			: m_alignment & wxALIGN_RIGHT ? wxT("right") : wxT("left");
	if ((type == DVDAUTHOR_XML && hAlignment != wxT("left"))
			|| (type == DVDSTYLER_XML && hAlignment != wxT("center")))
		textsubNode->AddProperty(wxT("horizontal-alignment"), hAlignment);
	wxString vAlignment = m_alignment & wxALIGN_CENTER_VERTICAL ? wxT("center")
				: m_alignment & wxALIGN_BOTTOM ? wxT("bottom") : wxT("top");
	if (vAlignment != wxT("bottom"))
		textsubNode->AddProperty(wxT("vertical-alignment"), vAlignment);
	
	// margin
	if (m_leftMargin != defaults->m_leftMargin)
		textsubNode->AddProperty(wxT("left-margin"), wxString::Format(wxT("%d"), m_leftMargin));
	if (m_rightMargin != defaults->m_rightMargin)
		textsubNode->AddProperty(wxT("right-margin"), wxString::Format(wxT("%d"), m_rightMargin));
	if (m_topMargin != defaults->m_topMargin)
		textsubNode->AddProperty(wxT("top-margin"), wxString::Format(wxT("%d"), m_topMargin));
	if (m_bottomMargin != defaults->m_bottomMargin)
		textsubNode->AddProperty(wxT("bottom-margin"), wxString::Format(wxT("%d"), m_bottomMargin));
	
	// fps
	if (m_subtitleFps != defaults->m_subtitleFps)
		textsubNode->AddProperty(wxT("subtitle-fps"), wxString::Format(wxT("%g"), m_subtitleFps));
	
	// movie values
	if (type == DVDAUTHOR_XML) {
		if (m_movieFps != defaults->m_movieFps)
			textsubNode->AddProperty(wxT("movie-fps"), wxString::Format(wxT("%g"), m_movieFps));
		if (m_movieSize.x != defaults->m_movieSize.x)
			textsubNode->AddProperty(wxT("movie-width"), wxString::Format(wxT("%d"), m_movieSize.x));
		if (m_movieSize.y != defaults->m_movieSize.y)
			textsubNode->AddProperty(wxT("movie-height"), wxString::Format(wxT("%d"), m_movieSize.y));
		if (m_aspectRatio != arAUTO)
			textsubNode->AddProperty(wxT("aspect"), m_aspectRatio == ar4_3 ? wxT("4:3") : wxT("16:9"));
	}
	delete defaults;
	return textsubNode;
}

wxString TextSub::AsString() {
	wxStringOutputStream stream;
	wxSvgXmlDocument doc;
	doc.SetRoot(GetXML(DVDSTYLER_XML));
	doc.Save(stream);
	return stream.GetString();
}

bool TextSub::PutXML(wxSvgXmlNode* node) {
	wxString val;
	long lval;
	long lval2;
	double dval;

	if (!node->GetPropVal(wxT("filename"), &m_filename))
		return false;
	node->GetPropVal(wxT("characterset"), &m_characterSet);

	m_force = node->GetPropVal(wxT("force"), &val) && val == wxT("yes");
	
	// font
	node->GetPropVal(wxT("fontFamily"), &m_fontFamily);
	node->GetPropVal(wxT("fontStyle"), &m_fontStyle);
	if (node->GetPropVal(wxT("fontsize"), &val) && val.ToDouble(&dval))
		m_fontSize = dval;
	
	// colours
	if (node->GetPropVal(wxT("fill-color"), &val) && val.length() >= 7 && val.GetChar(0) == wxT('#'))
		m_fillColour = String2Colour(val);
	if (node->GetPropVal(wxT("outline-color"), &val) && val.length() >= 7 && val.GetChar(0) == wxT('#'))
		m_outlineColour = String2Colour(val);
	if (node->GetPropVal(wxT("outline-thickness"), &val) && val.ToDouble(&dval))
		m_outlineThickness = dval;
	if (node->GetPropVal(wxT("shadow-color"), &val) && val.length() >= 7 && val.GetChar(0) == wxT('#'))
		m_shadowColour = String2Colour(val);
	if (node->GetPropVal(wxT("shadow-offset"), &val) && val.Find(wxT(',')) > 0
			&& val.Mid(0, val.Find(wxT(','))).ToLong(&lval) && val.Mid(val.Find(wxT(',')) + 1).ToLong(&lval2))
		m_shadowOffset = wxPoint(lval, lval2);
	
	// alignment
	m_alignment = wxALIGN_CENTER_HORIZONTAL;
	if (node->GetPropVal(wxT("horizontal-alignment"), &val)) {
		if (val == wxT("left"))
			m_alignment = wxALIGN_LEFT;
		else if (val == wxT("right"))
			m_alignment = wxALIGN_RIGHT;
	}
	if (node->GetPropVal(wxT("vertical-alignment"), &val)) {
		if (val == wxT("center"))
			m_alignment = (wxAlignment) (m_alignment | wxALIGN_CENTER_VERTICAL);
		else if (val == wxT("top"))
			m_alignment = (wxAlignment) (m_alignment | wxALIGN_TOP);
		else
			m_alignment = (wxAlignment) (m_alignment | wxALIGN_BOTTOM);
	} else
		m_alignment = (wxAlignment) (m_alignment | wxALIGN_BOTTOM);
	
	// margin
	if (node->GetPropVal(wxT("left-margin"), &val) && val.ToLong(&lval))
		m_leftMargin = lval;
	if (node->GetPropVal(wxT("right-margin"), &val) && val.ToLong(&lval))
		m_rightMargin = lval;
	if (node->GetPropVal(wxT("top-margin"), &val) && val.ToLong(&lval))
		m_topMargin = lval;
	if (node->GetPropVal(wxT("bottom-margin"), &val) && val.ToLong(&lval))
		m_bottomMargin = lval;
	
	// fps
	if (node->GetPropVal(wxT("subtitle-fps"), &val) && val.ToDouble(&dval))
		m_subtitleFps = dval;

	return true;
}

bool TextSub::SaveSpumux(const wxString& filename, VideoFormat videoFormat) {
	wxSvgXmlDocument xml;
	wxSvgXmlNode* root = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, wxT("subpictures"));
	root->AddProperty(wxT("format"), isNTSC(videoFormat) ? wxT("NTSC") : wxT("PAL"));
	wxSvgXmlNode* streamNode = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, wxT("stream"));
	streamNode->AddChild(GetXML(DVDAUTHOR_XML));
	root->AddChild(streamNode);
	xml.SetRoot(root);
	return xml.Save(filename);
}

/**
 * Returns font map (font family -> font style -> font filename)
 */
FontMap& TextSub::GetFontMap() {
	if (s_fonts.size() > 0)
		return s_fonts;
#ifdef __WXMSW__
	wxString fontConfigFile;
	if (!wxGetEnv(wxT("FONTCONFIG_FILE"), &fontConfigFile)) {
		// load config and fonts
		FcConfig* fc = FcConfigCreate ();
		if (fc == NULL) {
			wxLogError(wxString(wxT("SubtitlePropDlg::UpdateFontList(): ")) + wxString(wxT("FcConfigCreate failed.")));
			return s_fonts;
		}
		FcConfigParseAndLoad(fc, (const FcChar8*) (const char*) FONTS_CONF.ToUTF8(), FcTrue);
		FcConfigBuildFonts(fc);
		FcConfigSetCurrent(fc);
	}
#endif
	FcPattern* pattern = FcPatternBuild(NULL, FC_OUTLINE, FcTypeBool, 1, FC_SCALABLE, FcTypeBool, 1, NULL);
	FcObjectSet* objectSet = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_FULLNAME, FC_FILE, FC_INDEX, NULL);
	FcFontSet* fontSet = FcFontList(0, pattern, objectSet);
	FcObjectSetDestroy(objectSet);
	FcPatternDestroy(pattern);
	if (fontSet == NULL)
		return s_fonts;
	for (int fi = 0; fi < fontSet->nfont; fi++) {
		// font file
		FcChar8* str;
		if (FcPatternGetString(fontSet->fonts[fi], FC_FILE, 0, &str) != FcResultMatch)
			continue;
		wxString fontFilename = wxString::FromUTF8((char*) str);
		
		// font file index
		int fontFileIndex = 0;
		FcPatternGetInteger(fontSet->fonts[fi], FC_INDEX, 0, &fontFileIndex);
		if (fontFileIndex > 0)
			continue;
		// family name
		if (FcPatternGetString(fontSet->fonts[fi], FC_FAMILY, 0, &str) != FcResultMatch)
			continue;
		wxString fontFamily = wxString::FromUTF8((char*) str);
		// style name
		if (FcPatternGetString(fontSet->fonts[fi], FC_STYLE, 0, &str) != FcResultMatch)
			continue;
		wxString fontStyle = wxString::FromUTF8((char*) str);
		s_fonts[fontFamily][fontStyle] = fontFilename;
	}
	FcFontSetDestroy(fontSet);
	return s_fonts;
}

/** Returns true, if font map is initialized */
bool TextSub::IsFontMapInitialized() {
	return s_fonts.size() > 0;
}
