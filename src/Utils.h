/////////////////////////////////////////////////////////////////////////////
// Name:        Utils.h
// Purpose:     Miscellaneous utilities
// Author:      Alex Thuering
// Created:		06.04.2008
// RCS-ID:      $Id: Utils.h,v 1.10 2015/11/29 17:27:53 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_H
#define UTILS_H

#include <wxSVGXML/svgxml.h>
#include <wx/arrstr.h>
#include <wx/regex.h>
#include <wx/colour.h>
#include <vector>

using namespace std;

#define VECTOR_CLEAR(obj, elem_type) \
	for (vector<elem_type*>::iterator vectorIt = obj.begin(); vectorIt != obj.end(); vectorIt++) \
		delete *vectorIt; \
	obj.clear();

#define VECTOR_COPY(src, dst, elem_type) \
	for (vector<elem_type*>::const_iterator vectorIt = src.begin(); vectorIt != src.end(); vectorIt++) \
		dst.push_back(new elem_type(**vectorIt));

const wxString DATAFORMAT_ACTION =  wxT("application/dvdstyler-action");
const wxString DATAFORMAT_MENU =  wxT("application/dvdstyler-menu");
const wxString DATAFORMAT_MENU_OBJECT =  wxT("application/dvdstyler-menu-object");

/**
 * Copies given xml node to the clipboard.
 * Data Format: application/dvdstyler
 */
void CopyXmlToClipboard(wxSvgXmlNode* node, wxString dataFormatId);
/**
 * Gets xml node from clipboard.
 * Data Format: application/dvdstyler
 */
bool GetXmlFromClipboard(wxString dataFormatId, wxSvgXmlDocument& doc);
/**
 * Gets text from clipboard.
 */
wxString GetTextFromClipboard();

/**
 * Sorts string arrays by value
 */
int Sort(wxArrayString& keys, wxArrayString& values, const wxString& selectedKey = wxEmptyString);

/**
 * Converts time span value (milliseconds) in string
 */
wxString Time2String(long value, bool full = false);
/**
 * Converts string in time span value (milliseconds)
 */
long String2Time(const wxString& value, float fps = 25);

/**
 * Converts color value in string
 */
wxString Colour2String(const wxColour& colour);
/**
 * Converts string in color value
 */
wxColour String2Colour(const wxString& value);

/** Regular expression for time values */
extern wxRegEx s_timeRE;

#endif // UTILS_H
