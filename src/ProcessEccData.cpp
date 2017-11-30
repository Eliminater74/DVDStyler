/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessEccData.cpp
// Author:      Alex Thuering
// Created:     3.10.2014 (refactored)
// RCS-ID:      $Id: ProcessEccData.cpp,v 1.2 2014/10/20 06:08:12 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "ProcessEccData.h"
#include "ProcessExecute.h"
#include "ProcessIsoImage.h"
#include "BurnDlg.h"
#include "Config.h"

/** Constructor */
ProcessEccData::ProcessEccData(ProgressDlg* progressDlg, BurnDlg* burnDlg, const wxString& tmpDir): Process(progressDlg) {
	this->burnDlg = burnDlg;
	this->tmpDir = tmpDir;
}

/** Executes process */
bool ProcessEccData::Execute() {
	if (progressDlg->WasCanceled())
		return false;
	progressDlg->AddSummaryMsg(_("Adding ECC data"));
	progressDlg->SetSubSteps(200);
	wxString cmd = s_config.GetAddECCCmd();
	cmd.Replace(_T("$FILE"), burnDlg->DoCreateIso() ? burnDlg->GetIsoFile() : tmpDir + TMP_ISO);
	ProgressExecute exec(progressDlg, wxT("(Preparing|Ecc).*"));
	if (!exec.Execute(cmd)) {
		progressDlg->Failed();
		return false;
	}
	progressDlg->IncStep();
	return true;
}

/** Returns true, if process need be executed */
bool ProcessEccData::IsNeedExecute() {
	return burnDlg->DoAddECC() && (burnDlg->DoCreateIso() || burnDlg->DoBurn());
}

