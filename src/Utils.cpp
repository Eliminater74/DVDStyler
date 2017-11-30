/////////////////////////////////////////////////////////////////////////////
// Name:        Utils.h
// Purpose:     Miscellaneous utilities
// Author:      Alex Thuering
// Created:		06.04.2008
// RCS-ID:      $Id: Utils.cpp,v 1.15 2015/11/29 17:27:53 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "Utils.h"
#include <wx/clipbrd.h>
#include <wx/mstream.h>
#include <wx/log.h>
#include <wx/dataobj.h>
#include <wx/datetime.h>

// Time format: 0:00:00.000 or 0:00:00:00 (SMPTE)
wxRegEx s_timeRE(wxT("^(([[:digit:]]+:)?[[:digit:]][[:digit:]]?:)?[[:digit:]][[:digit:]]?((\\.[[:digit:]]+)|(:[[:digit:]][[:digit:]]?))?$"));

void CopyXmlToClipboard(wxSvgXmlNode* node, wxString dataFormatId) {
	wxMemoryOutputStream stream;
	wxSvgXmlDocument doc;
	doc.SetRoot(node);
	doc.Save(stream);
	int bufferSize = stream.GetSize();
	char* buffer = new char[bufferSize];
	stream.CopyTo(buffer, bufferSize);
	wxDataFormat format(dataFormatId);
	wxCustomDataObject* dataObj = new wxCustomDataObject(format);
	dataObj->TakeData(bufferSize, buffer);
	wxClipboardLocker locker;
	if (!locker) {
		wxLogError(wxT("Can't open clipboard"));
	} else if (!wxTheClipboard->SetData(dataObj)) {
		wxLogError(wxT("Can't copy xml node to the clipboard"));
	}
}

bool GetXmlFromClipboard(wxString dataFormatId, wxSvgXmlDocument& doc) {
	if (!wxTheClipboard->Open()) {
		wxLogError(wxT("Can't open clipboard"));
		return false;
	}
	wxDataFormat format(dataFormatId);
	wxCustomDataObject dataObj(format);
	if (!wxTheClipboard->GetData(dataObj)) {
	  wxLogError(wxT("Can't paste data from the clipboard"));
	  wxTheClipboard->Close();
	  return false;
	}
	wxTheClipboard->Close();
	wxMemoryInputStream stream(dataObj.GetData(), dataObj.GetSize());
	if (!doc.Load(stream)) {
	  wxLogError(wxT("Can't paste object: error parsing"));
	  return false;
	}
	return true;
}

wxString GetTextFromClipboard() {
	if (!wxTheClipboard->Open()) {
		wxLogError(wxT("Can't open clipboard"));
		return wxT("");
	}
	wxTextDataObject textObj;
	if (!wxTheClipboard->GetData(textObj)) {
		wxLogError(wxT("Can't paste data from the clipboard"));
		wxTheClipboard->Close();
		return wxT("");
	}
	wxTheClipboard->Close();
	return textObj.GetText();
}

int Sort(wxArrayString& keys, wxArrayString& values, const wxString& selectedKey) {
	int idx = -1;
	wxArrayString tmpArray;
	for (int i = 0; i<(int)keys.GetCount(); i++) {
		tmpArray.Add(values[i] + wxT("|") + keys[i]);
	}
	tmpArray.Sort();
	keys.Clear();
	values.Clear();
	for (int i = 0; i<(int)tmpArray.GetCount(); i++) {
		values.Add(tmpArray[i].BeforeLast('|'));
		wxString key = tmpArray[i].AfterLast('|');
		keys.Add(key);
		if (key == selectedKey)
			idx = keys.GetCount() - 1;
	}
	return idx;
}

/**
 * Converts time span value (milliseconds) in string
 */
wxString Time2String(long value, bool full) {
	int t = value / 1000;
	int ms = value % 1000;
	if (full || (t >= 3600 && ms > 0))
		return wxString::Format(wxT("%d:%02d:%02d.%03d"), t/3600, (t/60) % 60, t % 60, ms);
	else if (t >= 3600)
		return wxString::Format(wxT("%d:%02d:%02d"), t/3600, (t/60) % 60, t % 60);
	else if (ms > 0)
		return wxString::Format(wxT("%d:%02d.%03d"), (t/60) % 60, t % 60, ms);
	return wxString::Format(wxT("%d:%02d"), (t/60) % 60, t % 60);
}

/**
 * Converts string in time span value (milliseconds)
 */
long String2Time(const wxString& value, float fps) {
	long result = 0;
	wxString val = value;
	for (int i = 0; i <= 3; i++) {
		if (i < 3)
			result *= 60;
		if (val.Find(wxT(':')) == wxNOT_FOUND) {
			result *= 1000;
			if (i == 3) { // SMPTE format 00:00:00:00
				long t = 0;
				if (val.ToLong(&t))
					result += lround(t*1000/fps);
			} else {
				double t = 0;
				if (val.ToDouble(&t))
					result += lround(t*1000);
			}
			break;
		} else {
			long t = 0;
			if (val.BeforeFirst(wxT(':')).ToLong(&t))
				result += t;
			val = val.AfterFirst(wxT(':'));
		}
	}
	return result;
}


/**
 * Converts color value in string
 */
wxString Colour2String(const wxColour& colour) {
	if (!colour.IsOk()) {
		return wxT("");
	}
	return colour.Alpha() != 255 ? wxString::Format(wxT("#%02x%02x%02x%02x"),
			colour.Red(), colour.Green(), colour.Blue(), colour.Alpha()) :
			wxString::Format(wxT("#%02x%02x%02x"), colour.Red(), colour.Green(), colour.Blue());
}

/**
 * Converts string in color value
 */
wxColour String2Colour(const wxString& value) {
	if (value.length() < 7 || value.GetChar(0) != wxT('#')) {
		return wxColour();
	}
	long r = 0, g = 0, b = 0, a = 255;
	value.Mid(1, 2).ToLong(&r, 16);
	value.Mid(3, 2).ToLong(&g, 16);
	value.Mid(5, 2).ToLong(&b, 16);
	if (value.length() >= 9)
		value.Mid(7, 2).ToLong(&a, 16);
	return wxColour(r, g, b, a);
}
