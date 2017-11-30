/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessFormatDvd.cpp
// Author:      Alex Thuering
// Created:     3.10.2014 (refactored)
// RCS-ID:      $Id: ProcessFormatDvd.cpp,v 1.4 2015/11/29 17:29:29 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "ProcessFormatDvd.h"
#include "BurnDlg.h"
#include "ProgressDlg.h"
#include "Config.h"
#include "SysUtils.h"
#include <wx/msgdlg.h>

/** Constructor */
ProcessFormatDvd::ProcessFormatDvd(ProgressDlg* progressDlg, BurnDlg* burnDlg): Process(progressDlg) {
	this->burnDlg = burnDlg;
}

/** Returns true, if process need be executed */
bool ProcessFormatDvd::IsNeedExecute() {
	return burnDlg->DoBurn() && burnDlg->DoFormat();
}

/** Executes process */
bool ProcessFormatDvd::Execute() {
	if (!burnDlg->DoBurn() || !burnDlg->DoFormat())
		return true;
	if (progressDlg->WasCanceled())
		return false;
	progressDlg->AddSummaryMsg(_("Formatting DVD-RW"));
	while (1) {
		progressDlg->SetSubSteps(100);
		wxString cmd = s_config.GetFormatCmd();
		wxString device = burnDlg->GetDevice();
#ifdef __WXMAC__
		if (device.Mid(0,5) != wxT("/dev/")) {
			while (!IsMediaPresent(device) || !IsMediaErasable(device)) {
				if (IsMediaBlank(device))
					return true;
				if (wxMessageBox(wxString::Format(_("Please insert a rewritable DVD into the device %s."),
						GetDeviceName(device).c_str()), _("Burn"), wxOK|wxCANCEL, progressDlg) == wxCANCEL) {
					progressDlg->AddSummaryMsg(_("Aborted"), wxEmptyString, *wxRED);
					return false;
				}
			}
		}
#endif		
		cmd.Replace(wxT("$DEV"), device);
		if (!Exec(cmd)) {
			int repeat = wxMessageBox(_("Formatting DVD-RW failed. Try again?"), _("Burn"),
					wxYES_NO|wxCANCEL | wxICON_QUESTION, progressDlg);
			if (repeat == wxYES) {
				continue;
			} else if (repeat == wxNO) {
				progressDlg->AddSummaryMsg(_("-> skipped <-"), wxEmptyString, wxColour(128, 64, 64));
				break;
			} else {
				progressDlg->Failed();
				return false;
			}
		}
		break;
	}
	progressDlg->IncStep();
	return true;
}
