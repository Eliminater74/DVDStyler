/////////////////////////////////////////////////////////////////////////////
// Name:        Stream.h
// Purpose:     The class to store stream information
// Author:      Alex Thuering
// Created:	    25.10.2013
// RCS-ID:      $Id: Stream.h,v 1.6 2016/12/17 17:27:38 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef STREAM_H_
#define STREAM_H_

#include "mediaenc.h"
#include "DVDAction.h"
#include <wxSVG/mediadec_ffmpeg.h>
class Vob;

/** Audio adjustment type */
enum AudioAdjustmentType {
	aatChangePercent = 0,
	aatChangeDB,
	aatNormalize
};

class Stream {
public:
	Stream(StreamType type, wxString codecName);
	
	StreamType GetType() { return m_type; }
	void SetType(StreamType type) { m_type = type; }
	
	wxString GetSourceCodecName() { return m_sourceCodecName; }
	void SetSourceCodecName(wxString sourceCodecName) { m_sourceCodecName = sourceCodecName; }
	
	wxSize GetSourceVideoSize() { return m_sourceVideoSize; }
	void SetSourceVideoSize(wxSize sourceVideoSize) { m_sourceVideoSize = sourceVideoSize; }
	
	double GetSourceAspectRatio() { return m_sourceAspectRatio; }
	void SetSourceAspectRatio(double sourceAspectRatio) { m_sourceAspectRatio = sourceAspectRatio; }
	
	double GetSourceFps() { return m_sourceFps; }
	void SetSourceFps(double sourceFps) { m_sourceFps = sourceFps; }
		
	VideoFormat GetSourceVideoFormat();
	AudioFormat GetSourceAudioFormat();
	
	int GetSourceBitrate() { return m_sourceBitrate; }
	void SetSourceBitrate(int sourceBitrate) { m_sourceBitrate = sourceBitrate; }
	
	int GetSourceChannelNumber() { return m_sourceChannelNumber; }
	void SetSourceChannelNumber(int sourceChannelNumber) { m_sourceChannelNumber = sourceChannelNumber; }
	
	int GetSourceSampleRate() { return m_sourceSampleRate; }
	void SetSourceSampleRate(int sourceSampleRate) { m_sourceSampleRate = sourceSampleRate; }
	
	/** Returns the stream format string */
	wxString GetSourceFormat();
	
	/** Returns the stream language (as stored in metadata) */
	wxString GetLanguage() { return m_language; }
	/** Sets the stream language (as stored in metadata) */
	void SetLanguage(wxString language) { m_language = language; }
	/** Returns the stream language code (ISO 639) */
	wxString GetLanguageCode();
	
	/** Returns destination audio, video or subtitle format */
	int GetDestinationFormat() { return m_destinationFormat; }
	/** Sets destination audio, video or subtitle format */
	void SetDestinationFormat(int destinationFormat) { m_destinationFormat = destinationFormat; }
	
	/** Returns destination video format */
	VideoFormat GetVideoFormat() { return (VideoFormat) m_destinationFormat; }
	/** Sets destination video format */
	void SetVideoFormat(VideoFormat videoFormat) { m_destinationFormat = videoFormat; }
	
	/** Returns destination audio format */
	AudioFormat GetAudioFormat() { return (AudioFormat) m_destinationFormat; }
	/** Sets destination audio format */
	void SetAudioFormat(AudioFormat audioFormat) { m_destinationFormat = audioFormat; }
	
	/** Returns destination subtitle format */
	SubtitleFormat GetSubtitleFormat() { return (SubtitleFormat) m_destinationFormat; }
	/** Sets destination subtitle format */
	void SetSubtitleFormat(SubtitleFormat subtitleFormat) { m_destinationFormat = subtitleFormat; }
	
	/** Returns destination audio adjustment type */
	AudioAdjustmentType GetAudioAdjustType() { return m_audioAdjustType; }
	/** Sets destination audio adjustment type */
	void SetAudioAdjustType(AudioAdjustmentType audioAdjustType) { m_audioAdjustType = audioAdjustType; }
	
	/** Returns destination audio volume (1.0 = normal) */
	double GetAudioVolume() { return m_audioVolume; }
	/** Sets destination audio volume (1.0 = normal) */
	void SetAudioVolume(double audioVolume) { m_audioVolume = audioVolume; }
	
	/** Returns true, if replay gain is calculated */
	bool IsReplayGainCalculated() { return m_replayGainCalculated; }
	/** Sets, if replay gain is calculated */
	void SetReplayGainCalculated(bool replayGainCalculated) { m_replayGainCalculated = replayGainCalculated; }
	
	/** Returns track replay gain */
	double GetTrackGain() { return m_trackGain; }
	/** Sets track replay gain */
	void SetTrackGain(double trackGain) { m_trackGain = trackGain; }
	
	/** Returns destination audio channel number */
	int GetChannelNumber() const { return m_channelNumber; }
	/** Sets destination audio channel number */
	void SetChannelNumber(int channelNumber) { m_channelNumber = channelNumber; }
	
	/** Returns ts offset */
	long GetTsOffset() { return m_tsOffset; }
	/** Sets ts offset */
	void SetTsOffset(long tsOffset) { m_tsOffset = tsOffset; }
	
	/** Returns filters */
	const wxString&  GetFilters() { return m_filters; }
	/** Sets filters */
	void SetFilters(const wxString& filters) { m_filters = filters; }
	
	/** Returns all audio filters (incl. volume, up- and downmix) */
	wxString GetAllAudioFilters(bool forCacheEntry = false);
	
	/** Returns true if copy of video stream is possible */
	bool IsCopyPossible();
	
	/** Returns true if stream video is DVD compliant */
	bool IsDvdCompliant(bool hd);
	
	wxSvgXmlNode* GetXML(DVDFileType type, Vob* vob);
	bool PutXML(wxSvgXmlNode* node, Vob* vob);
	
private:
	StreamType m_type; // audio/video/subtitle
	wxString m_sourceCodecName; // source codec name
	int m_sourceBitrate; // source bitrate
	int m_sourceChannelNumber; // number of audio channels
	int m_sourceSampleRate; // sample rate of audio
	wxSize m_sourceVideoSize; // frame size of source video
	double m_sourceAspectRatio; // aspect ratio of source video
	double m_sourceFps; // FPS of source video
	wxString m_language; // language of stream  (as stored in metadata)
	int m_destinationFormat; // destination audio, video or subtitle format
	AudioAdjustmentType m_audioAdjustType; // destination audio adjustment type
	double m_audioVolume; // destination audio volume
	bool m_replayGainCalculated; // replay gain is calculated
	double m_trackGain; // track replay gain
	int m_channelNumber; // destination audio channel number
	long m_tsOffset; // input ts offset
	wxString m_filters; // filters
};

#endif // STREAM_H_
