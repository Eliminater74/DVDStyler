/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessSlideshow.h
// Author:      Alex Thuering
// Created:     26.09.2014 (refactored)
// RCS-ID:      $Id: ProcessSlideshow.cpp,v 1.6 2016/12/11 10:06:31 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "ProcessSlideshow.h"
#include "DVD.h"
#include "Config.h"
#include "mediaenc_ffmpeg.h"
#include "mediatrc_ffmpeg.h"
#include <wxVillaLib/utils.h>
#include <wxSVG/SVGDocument.h>
#include <wxSVG/SVGImageElement.h>
#include <wxSVG/SVGAnimateElement.h>
#include <wxSVG/mediadec_ffmpeg.h>

#define TRANSITION_FILE(fname) wxFindDataFile(wxT("transitions") + wxString(wxFILE_SEP_PATH) + fname)
#define TRANSITIONS_DIR wxFindDataDirectory(wxT("transitions"))

ProcessSlideshow::ProcessSlideshow(ProgressDlg* progressDlg, DVD* dvd, wxString dvdTmpDir): ProcessTranscode(progressDlg) {
	this->dvd = dvd;
	slideshowSubSteps = 0;
	for (int tsi = 0; tsi < (int) dvd->GetTitlesets().Count(); tsi++) {
		Titleset* ts = dvd->GetTitlesets()[tsi];
		for (int pgci = 0; pgci < (int) ts->GetTitles().Count(); pgci++) {
			Pgc* pgc = ts->GetTitles()[pgci];
			for (int vobi = 0; vobi < (int) pgc->GetVobs().Count(); vobi++) {
				Vob* vob = pgc->GetVobs()[vobi];
				if (vob->GetSlideshow()) {
					slideshowSubSteps += 10 * vob->GetSlideshow()->size();
					if (vob->GetAudioFilenames().size() > 0)
						slideshowSubSteps += 200; // transcode
					vob->SetTmpFilename(dvdTmpDir
							+ wxString::Format(_T("title%d-%d-%d.vob"), tsi, pgci, vobi));
					slideshowVobs.Add(vob);
				}
			}
		}
	}
}

/** Returns true, if process need be executed */
bool ProcessSlideshow::IsNeedExecute() {
	return slideshowVobs.Count() > 0;
}

/** Executes process */
bool ProcessSlideshow::Execute() {
	if (slideshowVobs.Count() == 0)
		return true;
	if (progressDlg->WasCanceled())
		return false;
	progressDlg->SetSubSteps(slideshowSubSteps);
	for (unsigned int i = 0; i < slideshowVobs.Count(); i++) {
		wxString msg = wxString::Format(_("Generating slideshow %u of %lu"), i + 1, slideshowVobs.Count());
		if (i == 0) {
			progressDlg->AddSummaryMsg(msg);
		} else {
			progressDlg->ReplaceSummaryMsg(msg);
			progressDlg->AddDetailMsg(msg);
		}
		
		Vob* vob = (Vob*) slideshowVobs[i];
		AudioFormat audioFormat = dvd->GetAudioFormat();
		if (vob->GetStreams().size() > 0 && vob->GetStreams()[0]->GetType() == stAUDIO) {
			Stream* stream = vob->GetStreams()[0];
			audioFormat = stream->GetAudioFormat();
			if (audioFormat == afCOPY) {
				// set destination format if it set to COPY and codec is not mp2/ac3 or sample rate != 48 kHz
				if (stream->GetSourceCodecName() != wxT("mp2")
						&& stream->GetSourceCodecName() != wxT("ac3")
						&& stream->GetSourceCodecName() != wxT("liba52")) {
					audioFormat = dvd->GetAudioFormat();
				} else if (stream->GetSourceSampleRate() != 48000) {
					audioFormat = stream->GetSourceCodecName() == wxT("mp2") ? afMP2 : afAC3;
				}
			}
		}
		if (!GenerateSlideshow(vob->GetSlideshow(), vob->GetTmpFilename(), audioFormat,
				vob->GetAudioFilenames().size() > 0 ? vob->GetAudioFilenames()[0] : wxString(), dvd->GetAudioBitrate()))
			return false;
	}
	progressDlg->IncStep();
	return true;
}

bool ProcessSlideshow::GenerateSlideshow(Slideshow* slideshow, const wxString& vobFile, AudioFormat audioFormat,
		wxString audioFile, int audioBitrate) {
	if (progressDlg->WasCanceled())
		return false;
	
	wxString m2vFile = vobFile + wxT("_video.m2v");

	wxFfmpegMediaEncoder ffmpeg(s_config.GetThreadCount());
	if (!ffmpeg.BeginEncode(audioFile.length() ? m2vFile : vobFile, slideshow->GetVideoFormat(),
			audioFile.length() ? afNONE : audioFormat, slideshow->GetAspectRatio(),
			s_config.GetSlideshowVideoBitrate(), s_config.GetSlideshowVideoCBR())) {
		progressDlg->Failed(_("Error creation of slideshow"));
		return false;
	}
	
	int width = GetFrameSize(slideshow->GetVideoFormat()).GetWidth();
	int height = GetFrameSize(slideshow->GetVideoFormat()).GetHeight();
	
	wxArrayString transitions;
	wxString fname = wxFindFirstFile(TRANSITIONS_DIR + _T("*.xml"));
	while (!fname.IsEmpty()) {
		transitions.Add(wxFileName(fname).GetFullName());
		fname = wxFindNextFile();
	}
	
	int tLast = -1;
	srand(time(NULL));
	for (unsigned i = 0; i < slideshow->size(); i++) {
		if (progressDlg->WasCanceled())
			return false;
		wxString msg = wxString::Format(_("Converting slide %u image to video"), i + 1);
		if (i == 0) {
			progressDlg->AddDetailMsg(msg);
		} else {
			progressDlg->ReplaceLastDetailMsg(msg);
		}
		wxYield();
		wxImage img = slideshow->GetImage(i);
		if (!ffmpeg.EncodeImage(img, (int)(slideshow->GetDuration()*slideshow->GetFPS()), progressDlg)) {
			progressDlg->Failed(_("Error creation of slideshow"));
			return false;
		}
		// Transition
		wxString transition = slideshow->GetSlide(i)->GetTransition();
		if (transition.length() == 0)
			transition = slideshow->GetTransition();
		if (transition.length() && transition != wxT("none") && i != slideshow->size() - 1) {
			if (transition == wxT("random")) {
				int t;
				do {
					t = rand() % transitions.size();
				} while (t == tLast);
				tLast = t;
				transition = transitions[t];
			}
			wxSVGDocument svg;
			if (!LoadTransition(svg, transition, slideshow, slideshow->GetSlide(i), slideshow->GetSlide(i + 1)))
				return false;
			int frameCount = slideshow->GetFPS();
			for (int f = 0; f < frameCount; f++) {
				wxImage img = svg.Render(width, height, NULL, false);
				if (!ffmpeg.EncodeImage(img, 1, progressDlg)) {
					progressDlg->Failed(_("Error creation of slideshow"));
					return false;
				}
				svg.SetCurrentTime(((double) f) / slideshow->GetFPS());
			}
		}
		progressDlg->IncSubStep(10);
	}
	ffmpeg.EndEncode();
	
	if (audioFile.length()) {
		// mplex (and optionally transcode audio)
		if (progressDlg->WasCanceled())
			return false;
		
		wxFfmpegMediaDecoder ffmpeg;
		if (!ffmpeg.Load(audioFile))
			return false;
		double audioDuration = ffmpeg.GetDuration();
		progressDlg->AddDetailMsg(_("Audio file duration: ") + Time2String(audioDuration * 1000));
		
		int resultDuration = slideshow->GetResultDuration();
		wxString concatFile;
		if (audioDuration > 0 && audioDuration < resultDuration) {
			// concate
			progressDlg->AddDetailMsg(_("Concat audio files"));
			int n = resultDuration / audioDuration + 1;
//			concatFile = vobFile + wxT("_audio.txt");
//			wxTextFile file(concatFile);
//			file.Create();
//			file.AddLine(wxT("ffconcat version 1.0"));
//			for (int i = 0; i < n; i++)
//				file.AddLine(wxT("file ") + audioFile);
//			file.Write();
			//ffmpeg -i 1.mp2 -i 1.mp2 -filter_complex "[0:0] [1:0] concat=n=2:v=0:a=1 [a]" -map "[a]"
			wxFfmpegMediaTranscoder transcoder;
			concatFile = vobFile + wxString(wxT("_audio.")) + (audioFormat  == afMP2 ? wxT("mp2") : wxT("ac3"));
			for (int i = 0; i < n; i++) {
				transcoder.AddInputFile(audioFile);
			}
			transcoder.ConcatAudio(concatFile, n);
			AVConvExecute exec(progressDlg, 0);
			if (!exec.Execute(transcoder.GetCmd())) {
				if (wxFileExists(concatFile))
					wxRemoveFile(concatFile);
				progressDlg->Failed(_("Error transcoding of ") + audioFile);
				return false;
			}
		}
		
		progressDlg->AddDetailMsg(_("Multiplexing audio and video"));
		
		Vob slideShowVob;
		slideShowVob.SetFilename(m2vFile);
		slideShowVob.AddAudioFile(concatFile.length() ? concatFile : audioFile);
		for (unsigned int i=0; i<slideShowVob.GetStreams().size(); i++) {
			Stream* stream = slideShowVob.GetStreams()[i];
			if (stream->GetType() == stAUDIO)
				stream->SetAudioFormat(audioFormat);
		}
		slideShowVob.SetTmpFilename(vobFile);
		slideShowVob.SetRecordingTime(resultDuration);
		if (!Transcode(&slideShowVob, slideshow->GetAspectRatio(), s_config.GetSlideshowVideoBitrate(),
				audioBitrate, s_config.GetUseMplexForMenus()))
			return false;
		if (s_config.GetRemoveTempFiles()) {
			DeleteFile(m2vFile);
			if (concatFile.length())
				DeleteFile(concatFile);
		}
	}

	wxYield();
	return true;
}

bool ProcessSlideshow::LoadTransition(wxSVGDocument& svg, const wxString& fileName, Slideshow* slideshow, Slide* slide1,
		Slide* slide2) {
	if (!svg.Load(TRANSITION_FILE(fileName))) {
		progressDlg->Failed(_("Error loading transition definition"));
		return false;
	}
	svg.GetRootElement()->SetWidth(slideshow->GetResolution().GetWidth());
	svg.GetRootElement()->SetHeight(slideshow->GetResolution().GetHeight());
	
	wxSVGImageElement* img1 = (wxSVGImageElement*) svg.GetElementById(wxT("image1"));
	wxSVGImageElement* img2 = (wxSVGImageElement*) svg.GetElementById(wxT("image2"));
	if (!img1 || !img2) {
		progressDlg->Failed(wxT("Transition: image element with id 'image1' and/or 'image2' is not defined"));
		return false;
	}
	
	img1->SetHref(slide1->GetFilename());
	img2->SetHref(slide2->GetFilename());
	
	SetAnimationDur(svg.GetRootElement(), s_config.GetDefTransitionDuration());
	return true;
}

void ProcessSlideshow::SetAnimationDur(wxSVGElement* parent, double dur) {
	wxSVGElement* elem = (wxSVGElement*) parent->GetChildren();
	while (elem) {
		if (elem->GetType() == wxSVGXML_ELEMENT_NODE) {
			if (elem->GetDtd() == wxSVG_ANIMATE_ELEMENT) {
				wxSVGAnimateElement* animateElem = (wxSVGAnimateElement*) elem;
				animateElem->SetDur(dur);
			} else if (elem->GetChildren()) {
				SetAnimationDur(elem, dur);
			}
		}
		elem = (wxSVGElement*) elem->GetNext();
	}
}
