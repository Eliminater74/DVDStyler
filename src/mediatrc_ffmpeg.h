/////////////////////////////////////////////////////////////////////////////
// Name:        mediatrc_ffmpeg.h
// Purpose:     FFMPEG Media Transcoder
// Author:      Alex Thuering
// Created:     26.04.2008
// RCS-ID:      $Id: mediatrc_ffmpeg.h,v 1.43 2016/06/04 22:04:07 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_FFMPEG_MEDIA_TRANSCODER_H
#define WX_FFMPEG_MEDIA_TRANSCODER_H

#include "mediaenc.h"
#include <wxSVG/mediadec_ffmpeg.h>
#include "wx/dynarray.h"
#include <stdint.h>
#include <math.h>
#include <map>
#include <vector>

using namespace std;

class wxFfmpegMediaTranscoder {
public:
	wxFfmpegMediaTranscoder();
	~wxFfmpegMediaTranscoder();
	
	/** Initializes transcoder command */
	void Init();
	
	/** Adds input file. */
	bool AddInputFile(const wxString& fileName, const wxString& format = wxT(""), long tsOffset = 0);
	
	/** Sets output file and video/audio/subtitle formats. */
	bool SetOutputFile(const wxString& fileName, VideoFormat videoFormat, bool ntscFilm, AspectRatio aspectRatio,
			AudioFormat audioFormat, SubtitleFormat subtitleFormat, int videoBitrate = 6000, bool vbr = false,
			int audioBitrate = 224, int mapFileIdx = -1, int mapStreamIdx = -1, StreamType mapStreamType = stUNKNOWN);
	/** Sets output file and video/audio/subtitle formats. */
	bool SetOutputFile(const wxString& fileName, VideoFormat videoFormat, bool ntscFilm, AspectRatio aspectRatio, 
			wxArrayInt audioFormats, wxArrayInt subtitleFormats, int videoBitrate = 6000, bool vbr = false,
			int audioBitrate = 224, int mapFileIdx = -1, int mapStreamIdx = -1, StreamType mapStreamType = stUNKNOWN);
	/** Sets output format (optional). Call it before SetOutputFile() */
	void SetOutputFormat(wxString outputFormat) { m_outputFormat = outputFormat; }
	/** Sets interlaced encoding flag */
	void SetInterlaced(bool value);
	/** Sets first field flag (Auto, TFF, BFF) */
	void SetFirstField(FirstField firstField);
	/** Sets start time (in sec) */
	void SetStartTime(double startTime);
	/** Sets recording time (in sec) */
	void SetRecordingTime(double recordingTime);
	/** Sets list of chapters to force key frames */
	void SetChapterList(const wxString& chapterList) { m_chapterList = chapterList; }
	/** Sets video filters */
	void SetVideoFilters(const wxString& videoFilters) { m_videoFilters = videoFilters; }
	/** Sets audio filters */
	void SetAudioFilters(int streamIndex, const wxString& audioFilters) { m_audioFilters[streamIndex] = audioFilters; }
	/** Returns audio filters */
	map<int, wxString>& GetAudioFilters() { return m_audioFilters; }
	/** Set filter to concat video segments (with reencoding) */
	void ConcatVideo(const wxString& resultFile, int videoBitrate = 6000, bool copyAudio = true, bool copySubtitle = false);
	/** Set filter to concat audio segments */
	void ConcatAudio(const wxString& resultFile, int segments);
	/** Set filter and output to calculate replay gain values */
	void ReplayGain(int audioStreamIdx);
	/** Set filter and output to calculate volume peak value */
	void VolumeDetect(int audioStreamIdx);
	
	/** Returns transcoding command */
	wxString GetCmd(bool forceInternalEncoder = false) const;
	
private:
	unsigned int m_inputFileCount;
	wxString m_cmd;
	wxString m_outputFormat;
	wxString m_chapterList; // list of chapters to force key frames
	wxString m_videoFilters;
    map<int, wxString> m_audioFilters; // stream index -> audio filters
    void AddOption(const wxString& name, const wxString& value);
    void AddAudioOption(const wxString& name, int streamIdx, const wxString& value);
    void AddVideoOption(const wxString& name, int streamIdx, const wxString& value);
    void AddSubtitleOption(const wxString& name, int streamIdx, const wxString& value);
};

#endif // WX_FFMPEG_MEDIA_TRANSCODER_H
