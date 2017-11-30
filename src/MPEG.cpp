/////////////////////////////////////////////////////////////////////////////
// Name:        MPEG.cpp
// Purpose:     MPEG utils
// Author:      Alex Thuering
// Created:	04.04.2003
// RCS-ID:      $Id: MPEG.cpp,v 1.6 2012/06/24 23:00:09 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "MPEG.h"
#include <wx/wfstream.h>

bool MPEG::IsValid(const wxString& fileName) {
	wxFileInputStream s(fileName);
	if (!s.Ok()) {
		wxLogError(_("Can't open file %s"), fileName.c_str());
		return false;
	}

	// skip zeroes at start of file
	int zeroCnt = 0;
	char c;
	while ((c = s.GetC()) == 0)
		zeroCnt++;
	
	return zeroCnt >= 2 && c == 1;
}

bool MPEG::HasNavPacks(const wxString& fileName) {
	if (fileName.StartsWith(wxT("concat:")))
		return false;
	
	/*
	Navigation packets are PES packets with a stream id 0xbf, i.e.
	private stream 2.  It's made up of PCI, Presentation Control Information
	and DSI, Data Search Information.
	
	details: www.mpucoder.com
	*/
	
	unsigned char PCI_PACK[] = { 0x00, 0x00, 0x01, 0xBF, 0x03, 0xD4 };
	unsigned char DSI_PACK[] = { 0x00, 0x00, 0x01, 0xBF, 0x03, 0xFA };
	
	wxFileInputStream s(fileName);
	if (!s.Ok()) {
		wxLogError(_("Can't open file %s"), fileName.c_str());
		return false;
	}

	const long MAX_LENGTH = 16384; // only looking at 1st 16KB - enough to contain nav packs
	unsigned char buffer[MAX_LENGTH];
	
	// skip zeroes at start of file
	int zeroCnt = 0;
	char c;
	while ((c = s.GetC()) == 0)
		zeroCnt++;
	
	if (zeroCnt < 2 || c != 1)
		return false;

	buffer[0] = buffer[1] = 0;
	buffer[2] = 1;
	int readed = s.Read(buffer + 3, MAX_LENGTH - 3).LastRead() + 3;
	
	bool valid_pci = false;
	bool valid_dsi = false;
	for (int i = 0; i < readed - (int) sizeof(PCI_PACK); i++) {
		int j;
		for (j = 0; j < (int) sizeof(PCI_PACK); j++) {
			if (buffer[i + j] != PCI_PACK[j])
				break;
			else if (j == sizeof(PCI_PACK) - 1)
				valid_pci = true;
		}
		for (j = 0; j < (int) sizeof(DSI_PACK); j++) {
			if (buffer[i + j] != DSI_PACK[j])
				break;
			else if (j == sizeof(DSI_PACK) - 1)
				valid_dsi = true;
		}
		
		if (valid_pci && valid_dsi)
			return true;
	}
	
	return false;
}
