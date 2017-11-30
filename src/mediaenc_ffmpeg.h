/////////////////////////////////////////////////////////////////////////////
// Name:        mediaenc_ffmpeg.h
// Purpose:     FFMPEG Media Encoder
// Author:      Alex Thuering
// Created:     04.08.2007
// RCS-ID:      $Id: mediaenc_ffmpeg.h,v 1.15 2016/10/05 19:51:30 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_FFMPEG_MEDIA_ENCODER_H
#define WX_FFMPEG_MEDIA_ENCODER_H

#include "mediaenc.h"
#include <stdint.h>

struct AVFormatContext;
struct AVStream;
struct AVFrame;
struct SwsContext;

class wxFfmpegMediaEncoder {
public:
    wxFfmpegMediaEncoder(int threadCount = 1);
    ~wxFfmpegMediaEncoder();
    
    bool BeginEncode(const wxString& fileName, VideoFormat videoFormat, AudioFormat audioFormat,
    		AspectRatio aspectRatio = ar4_3, int videoBitrate = 6000, bool cbr = false);
    bool EncodeImage(wxImage image, int frames, AbstractProgressDialog* progressDialog);
    bool EncodeAudio(double duration, AbstractProgressDialog* progressDialog);
    void EndEncode();
    
    void SetGopSize(int gopSize) { m_gopSize = gopSize; }
    
    static wxString GetBackendVersion();
    
private:
	int m_threadCount;
	int m_gopSize;
    AVFormatContext* m_outputCtx;
    AVStream* m_videoStm;
    AVStream* m_audioStm;
    int64_t m_nextVideoPts;
    int64_t m_nextAudioPts;
    bool addVideoStream(int codecId, VideoFormat videoFormat, AspectRatio aspectRatio, int videoBitrate, bool cbr);
    bool addAudioStream(int codecId);
    
    int16_t* m_samples;
    AVFrame* m_audioFrame;
    bool OpenAudioEncoder();
    void CloseAudioEncoder();
    
    AVFrame* m_picture;
    SwsContext* m_imgConvertCtx;
    uint8_t* m_videoOutbuf;
    bool OpenVideoEncoder();
    void CloseVideoEncoder();
    
    void getAudioFrame(int nbChannels);
    bool writeAudioFrame();
    /** writes m_picture */
    bool writeVideoFrame();
    
    void CloseEncoder();
};

#endif // WX_FFMPEG_MEDIA_ENCODER_H
