/////////////////////////////////////////////////////////////////////////////
// Name:        Utils.h
// Purpose:     System utilities
// Author:      Alex Thuering
// Created:		05.10.2015
// RCS-ID:      $Id: SysUtils.h,v 1.1 2015/11/29 17:27:05 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef SYS_UTILS_H
#define SYS_UTILS_H

#include <wx/combobox.h>

#ifdef __WXMAC__
extern "C++" {
	void EnumDevices(wxArrayString& devices, wxComboBox* deviceChoice);
	wxString GetDeviceName(const wxString& ioRegistryEntryPath);
	wxString GetDeviceNode(const wxString& ioRegistryEntryPath);
	bool IsMediaPresent(const wxString& ioRegistryEntryPath);
	bool IsMediaErasable(const wxString& ioRegistryEntryPath);
	bool IsMediaBlank(const wxString& ioRegistryEntryPath);
	long GetMediaSpaceFree(const wxString& ioRegistryEntryPath);
}
#else
	void EnumDevices(wxArrayString& devices, wxComboBox* deviceChoice);
#endif

/** System Manager interface */
class SystemManager {
public:
	/** Checks if user has a privileges to stop (ie. shutdown) the computer system. */
	static bool CanStop();

	/** Initiates a request to stop (ie. shutdown) the computer system. */
	static void Stop();
};

#endif // UTILS_H
