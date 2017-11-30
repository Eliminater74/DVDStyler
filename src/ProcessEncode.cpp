/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessEncode.h
// Author:      Alex Thuering
// Created:     26.09.2014 (refactored)
// RCS-ID:      $Id: ProcessEncode.cpp,v 1.7 2016/12/29 12:38:52 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "ProcessEncode.h"
#include "DVD.h"
#include "Cache.h"
#include "Config.h"
#include <algorithm>

/** Constructor */
ProcessEncode::ProcessEncode(ProgressDlg* progressDlg, DVD* dvd, Cache* cache, wxString dvdTmpDir):
		ProcessTranscode(progressDlg) {
	this->dvd = dvd;
	this->cache = cache;
	CacheSet preCacheSet;
	titleSubSteps = 0;
	for (int tsi = 0; tsi < (int) dvd->GetTitlesets().Count(); tsi++) {
		Titleset* ts = dvd->GetTitlesets()[tsi];
		for (int pgci = 0; pgci < (int) ts->GetTitles().Count(); pgci++) {
			Pgc* pgc = ts->GetTitles()[pgci];
			for (int vobi = 0; vobi < (int) pgc->GetVobs().Count(); vobi++) {
				Vob* vob = pgc->GetVobs()[vobi];
				vob->SetTmpFilename(_T(""));
				if (vob->GetSlideshow() || (vob->GetDoNotTranscode() && vob->GetAudioFilenames().Count() == 0))
					continue;
				if (vob->GetKeepAspectRatio()) {
					vob->UpdatePadCrop(ts->GetTitles().GetVideo().GetAspect());
				}
				
				// check if file is already in the cache
				wxString cacheFile = cache->Find(vob, dvd);
				if (cacheFile.length() > 0 && wxFileExists(cacheFile)) {
					progressDlg->AddDetailMsg(wxString::Format(_("Found transcoded file in cache for '%s'"),
							vob->GetFilename().c_str()));
					vob->SetTmpFilename(cacheFile);
					continue; 
				}
				
				// check if file is already noticed to transcode (pre-cache)
				CacheEntry entry(vob, dvd, -1);
				auto it = std::find(preCacheSet.begin(), preCacheSet.end(), entry);
				if (it != preCacheSet.end()) {
					titleVobs2.Add(vob);
					continue;
				}
				
				titleSubSteps += 200;
				titleVobs.Add(vob);
				titleAspects.Add(ts->GetTitles().GetVideo().GetAspect());
				preCacheSet.push_back(entry);
			}
		}
	}
}
    
/** Returns true, if process need be executed */
bool ProcessEncode::IsNeedExecute() {
	return titleVobs.Count() > 0;
}
	
/** Executes process */
bool ProcessEncode::Execute() {
	if (titleVobs.Count() == 0)
		return true;
	if (progressDlg->WasCanceled())
		return false;
	progressDlg->SetSubSteps(titleSubSteps);
	int videoBitrate = dvd->GetVideoBitrate();
	if (dvd->GetCapacity() != dcUNLIMITED && dvd->GetSize() > dvd->GetCapacityValue()*0.965)
		videoBitrate *= 0.965;
	for (int i = 0; i < (int) titleVobs.Count(); i++) {
		wxString msg = wxString::Format(_("Transcode video %u of %lu"), i + 1, titleVobs.Count());
		if (i == 0) {
			progressDlg->AddSummaryMsg(msg);
		} else {
			progressDlg->ReplaceSummaryMsg(msg);
			progressDlg->AddDetailMsg(msg);
		}
		
		Vob* vob = (Vob*) titleVobs[i];
		AspectRatio aspect = (AspectRatio) titleAspects[i];
		wxString cacheFile = cache->Add(vob, dvd);
		progressDlg->AddDetailMsg(wxT("Add file to cache:") + cacheFile);
		vob->SetTmpFilename(cacheFile);
		
		if (!Transcode(vob, aspect, videoBitrate, dvd->GetAudioBitrate(), s_config.GetUseMplex(), dvd->GetAudioFormat())) {
			cache->Remove(vob, dvd);
			return false;
		}
	}
	// set temporary vob files
	for (int i = 0; i < (int) titleVobs2.Count(); i++) {
		Vob* vob = (Vob*) titleVobs2[i];
		vob->SetTmpFilename(cache->Find(vob, dvd));
	}
	progressDlg->IncStep();
	return true;
}
