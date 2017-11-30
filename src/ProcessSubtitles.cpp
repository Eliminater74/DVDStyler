/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessSubtitles.h
// Author:      Alex Thuering
// Created:     26.09.2014 (refactored)
// RCS-ID:      $Id: ProcessSubtitles.cpp,v 1.4 2016/12/11 10:06:31 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "ProcessSubtitles.h"
#include "DVD.h"
#include "Cache.h"
#include "Config.h"
#include <wx/filename.h>

/** Constructor */
ProcessSubtitles::ProcessSubtitles(ProgressDlg* progressDlg, DVD* dvd, Cache* cache, wxString dvdTmpDir):
		ProcessTranscode(progressDlg) {
	this->dvd = dvd;
	this->cache = cache;
	subtitleSubSteps = 0;
	for (int tsi = 0; tsi < (int) dvd->GetTitlesets().Count(); tsi++) {
		Titleset* ts = dvd->GetTitlesets()[tsi];
		for (int pgci = 0; pgci < (int) ts->GetTitles().Count(); pgci++) {
			Pgc* pgc = ts->GetTitles()[pgci];
			for (int vobi = 0; vobi < (int) pgc->GetVobs().Count(); vobi++) {
				Vob* vob = pgc->GetVobs()[vobi];
				if (vob->GetSubtitles().Count()) {
					subtitleSubSteps += vob->GetSubtitles().Count() * 50;
					wxString cacheFile = cache->Find(vob, dvd);
					if (cacheFile.length() > 0 && wxFileExists(cacheFile)) {
						progressDlg->AddDetailMsg(wxString::Format(_("Found transcoded file in cache for '%s'"),
								vob->GetFilename().c_str()));
						vob->SetTmpFilename(cacheFile);
						continue; // file in cache do not need to transcode
					}
					subtitleVobs.Add(vob);
					subtitleTsi.Add(tsi);
				}
			}
		}
	}
}

/** Returns true, if process need be executed */
bool ProcessSubtitles::IsNeedExecute() {
	return subtitleVobs.Count() > 0;
}
	
/** Executes process */
bool ProcessSubtitles::Execute() {
	if (subtitleVobs.Count() == 0)
		return true;
	if (progressDlg->WasCanceled())
		return false;
	progressDlg->SetSubSteps(subtitleSubSteps);
	for (unsigned int i = 0; i < subtitleVobs.Count(); i++) {
		wxString msg = wxString::Format(_("Multiplexing subtitles %u of %lu"), i + 1, subtitleVobs.Count());
		if (i == 0) {
			progressDlg->AddSummaryMsg(msg);
		} else {
			progressDlg->ReplaceSummaryMsg(msg);
			progressDlg->AddDetailMsg(msg);
		}
		Vob* vob = (Vob*) subtitleVobs[i];
		AspectRatio aspectRatio = dvd->GetTitlesets()[subtitleTsi[i]]->GetTitles().GetVideo().GetAspect();
		if (vob->GetTmpFilename().length() == 0) {
			wxString cacheFile = cache->Add(vob, dvd);
			progressDlg->AddDetailMsg(wxT("Add file to cache:") + cacheFile);
			vob->SetTmpFilename(cacheFile);
		}
		
		// Extract Video stream geometry
		wxSize movieSize;
		VideoFormat videoFormat = vfPAL;
		Stream* stream = vob->GetVideoStream();

		if (stream != NULL) {
			if (stream->GetVideoFormat() == vfCOPY) {
				movieSize = stream->GetSourceVideoSize();
				videoFormat = movieSize.GetHeight() == 480 || movieSize.GetHeight() == 240 ? vfNTSC : vfPAL;
			} else {
				videoFormat = stream->GetVideoFormat();
				movieSize = GetFrameSize(videoFormat);
			}
		}
		
		int subCount = vob->GetSubtitleStreamsCount() - vob->GetSubtitles().Count();
		for (unsigned int s = 0; s < vob->GetSubtitles().Count(); s++) {
			wxString vobFile = vob->GetFilename();
			if (wxFileExists(vob->GetTmpFilename())) {
				if (!wxRenameFile(vob->GetTmpFilename(), vob->GetTmpFilename() + wxT(".old"))) {
					progressDlg->Failed(wxString::Format(_("Can't rename temporary file '%s'"),vob->GetTmpFilename().c_str()));
					return false;
				}
				vobFile = vob->GetTmpFilename() + wxT(".old");
			}
			
			// Force geometry of subtitle
			TextSub* textSub = vob->GetSubtitles()[s];
			textSub->SetMovieSize(movieSize);
			textSub->SetAspectRatio(aspectRatio);

			if (!MultiplexSubtitles(vobFile, vob->GetSubtitles()[s], s + subCount, videoFormat, vob->GetTmpFilename()))
				return false;
			progressDlg->IncSubStep(vob->GetSize(dvd));
		}
	}
	progressDlg->IncStep();
	return true;
}

bool ProcessSubtitles::MultiplexSubtitles(const wxString& vobFile, TextSub* textSub, unsigned int streamIdx,
		VideoFormat videoFormat, const wxString& resultFile) {
	if (progressDlg->WasCanceled())
		return false;
	//spumux
	wxString cmd = s_config.GetSpumuxCmd();
	wxString spuFile = resultFile + wxT("_spumux.xml");
	textSub->SaveSpumux(spuFile, videoFormat);
	cmd.Replace(wxT("$FILE_CONF"), spuFile);
	cmd.Replace(wxT("$STREAM"), wxString::Format(wxT("%u"), streamIdx));
	wxULongLong fileSize = wxFileName::GetSize(vobFile);
	SpumuxExecute exec(progressDlg, fileSize != wxInvalidSize ? fileSize.ToDouble() : -1);
	if (!exec.Execute(cmd, vobFile, resultFile)) {
		if (wxFileExists(resultFile))
			wxRemoveFile(resultFile);
		progressDlg->Failed();
		return false;
	}
	if (s_config.GetRemoveTempFiles()) {
		if (vobFile == resultFile + _T(".old"))
			DeleteFile(vobFile);
		DeleteFile(spuFile);
	}

	wxYield();
	return true;
}
