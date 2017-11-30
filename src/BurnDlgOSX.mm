/////////////////////////////////////////////////////////////////////////////
// Name:        BurnDlgOSX.mm
// Purpose:     Burn dialog (OSX)
// Author:      Alex Thuering
// Created:		18.06.2010
// RCS-ID:      $Id: BurnDlgOSX.mm,v 1.5 2014/02/25 10:58:28 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <DiscRecording/DRDevice.h>

extern "C++" {
	void EnumDevices(wxArrayString& devices, wxComboBox* deviceChoice);
	wxString GetDeviceName(const wxString& ioRegistryEntryPath);
	wxString GetDeviceNode(const wxString& ioRegistryEntryPath);
	bool IsMediaPresent(const wxString& ioRegistryEntryPath);
	bool IsMediaErasable(const wxString& ioRegistryEntryPath);
	bool IsMediaBlank(const wxString& ioRegistryEntryPath);
	long GetMediaSpaceFree(const wxString& ioRegistryEntryPath);
}

inline NSString* nsStr(const wxString& str) {
	return [NSString stringWithUTF8String: str.mb_str(wxConvUTF8)];
}

inline wxString wxStr(NSString* str) {
	return wxString([str UTF8String], wxConvUTF8);
}

void EnumDevices(wxArrayString& devices, wxComboBox* deviceChoice) {
	NSArray* devs = [DRDevice devices];
	for (int i = 0; i < [devs count]; i++) {
		DRDevice* device = [devs objectAtIndex:i];
		if (![device writesDVD])
			continue;
		devices.Add(wxStr([device ioRegistryEntryPath]));
		deviceChoice->Append(wxStr([device displayName]));
	}
}

wxString GetDeviceName(const wxString& ioRegistryEntryPath) {
	DRDevice* device = [DRDevice deviceForIORegistryEntryPath: nsStr(ioRegistryEntryPath)];
	return device != nil && [device displayName] != nil ? wxStr([device displayName]) : ioRegistryEntryPath;
}

wxString GetDeviceNode(const wxString& ioRegistryEntryPath) {
	DRDevice* device = [DRDevice deviceForIORegistryEntryPath: nsStr(ioRegistryEntryPath)];
	return device != nil && [device bsdName] != nil ? wxStr([device bsdName]) : wxT("");
}

bool IsMediaPresent(const wxString& ioRegistryEntryPath) {
	DRDevice* device = [DRDevice deviceForIORegistryEntryPath: nsStr(ioRegistryEntryPath)];
	return device != nil ? [device mediaIsPresent] : false;
}

bool IsMediaErasable(const wxString& ioRegistryEntryPath) {
	DRDevice* device = [DRDevice deviceForIORegistryEntryPath: nsStr(ioRegistryEntryPath)];
	return device != nil ? [device mediaIsErasable] : false;
}

bool IsMediaBlank(const wxString& ioRegistryEntryPath) {
	DRDevice* device = [DRDevice deviceForIORegistryEntryPath: nsStr(ioRegistryEntryPath)];
	return device != nil ? [device mediaIsBlank] : false;
}

// media free space in 2048 blocks
long GetMediaSpaceFree(const wxString& ioRegistryEntryPath) {
	DRDevice* device = [DRDevice deviceForIORegistryEntryPath: nsStr(ioRegistryEntryPath)];
	return device != nil ? [[device mediaSpaceFree] floatValue] : 0;
} 
