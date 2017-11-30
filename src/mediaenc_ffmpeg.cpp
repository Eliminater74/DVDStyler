/////////////////////////////////////////////////////////////////////////////
// Name:        mediaenc_ffmpeg.cpp
// Purpose:     FFMPEG Media Encoder
// Author:      Alex Thuering
// Created:     04.08.2007
// RCS-ID:      $Id: mediaenc_ffmpeg.cpp,v 1.56 2016/10/23 19:03:35 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "mediaenc_ffmpeg.h"
#include <wx/wx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef INT64_C
#define INT64_C(val) val##LL
#define UINT64_C(val) val##ULL
#endif
extern "C" {
#define __STDC_CONSTANT_MACROS
#define __STDC_LIMIT_MACROS
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
#include <libavutil/avstring.h>
}

#define AUDIO_BUF_SIZE 524288
#define VIDEO_BUF_SIZE 1835008

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54, 25, 0)
#define AVCodecID CodecID
#define AV_CODEC_ID_NONE CODEC_ID_NONE
#define AV_CODEC_ID_MPEG2VIDEO CODEC_ID_MPEG2VIDEO
#define AV_CODEC_ID_MP2 CODEC_ID_MP2
#define AV_CODEC_ID_AC3 CODEC_ID_AC3
#endif

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif


wxFfmpegMediaEncoder::wxFfmpegMediaEncoder(int threadCount) {
	m_threadCount = threadCount;
	m_gopSize = -1;
	m_outputCtx = NULL;
	m_videoStm = NULL;
	m_audioStm = NULL;
    m_nextVideoPts = 0;
    m_nextAudioPts = 0;
	m_samples = NULL;
	m_audioFrame = NULL;
	m_picture = NULL;
	m_imgConvertCtx = NULL;
	m_videoOutbuf = NULL;
}

wxFfmpegMediaEncoder::~wxFfmpegMediaEncoder() {
	CloseEncoder();
}

wxString wxFfmpegMediaEncoder::GetBackendVersion() {
	return wxString::Format(wxT("libavformat %d.%d.%d, libavcodec %d.%d.%d, libavutil %d.%d.%d"),
			LIBAVFORMAT_VERSION_INT >> 16, LIBAVFORMAT_VERSION_INT >> 8 & 0xFF, LIBAVFORMAT_VERSION_INT & 0xFF,
			LIBAVCODEC_VERSION_INT >> 16, LIBAVCODEC_VERSION_INT >> 8 & 0xFF, LIBAVCODEC_VERSION_INT & 0xFF,
			LIBAVUTIL_VERSION_INT >> 16, LIBAVUTIL_VERSION_INT >> 8 & 0xFF, LIBAVUTIL_VERSION_INT & 0xFF);
}

#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(52, 45, 0)
#define av_guess_format guess_format
#endif

void print_error(const char *filename, int err) {
	char errbuf[128];
	const char *errbuf_ptr = errbuf;
	if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
		errbuf_ptr = strerror(AVUNERROR(err));
	wxLogError(wxT("%s: %s\n"), filename, errbuf_ptr);
}

bool wxFfmpegMediaEncoder::BeginEncode(const wxString& fileName, VideoFormat videoFormat, AudioFormat audioFormat,
		AspectRatio aspectRatio, int videoBitrate, bool cbr) {
	EndEncode();
	AVOutputFormat* outputFormat = NULL;
	if (videoFormat == vfNONE || audioFormat == afNONE)
		outputFormat = av_guess_format(NULL, (const char*) fileName.ToUTF8(), NULL);
	else
		outputFormat = av_guess_format("dvd", NULL, NULL);
	if (!outputFormat) {
		wxLogError(wxT("Cannot open output format"));
		return false;
	}
	outputFormat->video_codec = videoFormat == vfNONE ? AV_CODEC_ID_NONE : AV_CODEC_ID_MPEG2VIDEO;
	if (audioFormat == afNONE)
		outputFormat->audio_codec = AV_CODEC_ID_NONE;
	else if (audioFormat == afAC3)
		outputFormat->audio_codec = AV_CODEC_ID_AC3;
	else
		outputFormat->audio_codec = AV_CODEC_ID_MP2;
	
	m_outputCtx = avformat_alloc_context();
	m_outputCtx->pb = NULL;
	if (!m_outputCtx) {
		wxLogError(wxT("memory allocation error"));
		return false;
	}

	m_outputCtx->oformat = outputFormat;
	av_strlcpy(m_outputCtx->filename, (const char*) fileName.ToUTF8(), sizeof(m_outputCtx->filename));
	
	// add video and audio streams
	if (!addVideoStream(outputFormat->video_codec, videoFormat, aspectRatio, videoBitrate, cbr))
		return false;
	if (!addAudioStream(outputFormat->audio_codec))
		return false;

	av_dump_format(m_outputCtx, 0, (const char*) fileName.ToUTF8(), 1);
	m_outputCtx->packet_size = 2048;
	
	
	// open the audio and video codecs
	if (m_videoStm && !OpenVideoEncoder())
		return false;
	if (m_audioStm && !OpenAudioEncoder())
		return false;

	// open the output file
	if (avio_open2(&m_outputCtx->pb, (const char*) fileName.ToUTF8(), AVIO_FLAG_WRITE, NULL, NULL) < 0) {
		wxLogError(wxT("Could not open '%s'"), fileName.c_str());
		return false;
	}
	
	// write the stream header
	AVDictionary *opts = NULL;
	av_dict_set(&opts, "packet_size", "2048", 0);
#ifdef USE_FFMPEG
	av_dict_set(&opts, "muxrate", "10080000", 0); // from mplex project: data_rate = 1260000. mux_rate = data_rate * 8
#endif
	char tmpstr[48];
	snprintf(tmpstr, sizeof(tmpstr), "%i", (int)(0.5 * AV_TIME_BASE));
	av_dict_set(&opts, "preload", tmpstr, 0);
	m_outputCtx->packet_size = 2048;
	m_outputCtx->max_delay = (int) (0.7 * AV_TIME_BASE);
	int ret = avformat_write_header(m_outputCtx, &opts);
    if (ret < 0) {
    	wxLogError(wxT("Error occurred when opening output file"));
        return false;
    } 
	av_dict_free(&opts);
	return true;
}

bool wxFfmpegMediaEncoder::addVideoStream(int codecId, VideoFormat videoFormat, AspectRatio aspectRatio,
		int videoBitrate, bool cbr) {
	if (codecId == AV_CODEC_ID_NONE) {
		m_videoStm = NULL;
		return true;
	}
	m_videoStm = avformat_new_stream(m_outputCtx, NULL);
	if (!m_videoStm) {
		wxLogError(wxT("Could not alloc stream"));
		return false;
	}
	m_videoStm->id = 0;

	AVCodecContext* c = m_videoStm->codec;
	c->thread_count = m_threadCount;
	c->codec_id = (AVCodecID) codecId;
	c->codec_type = AVMEDIA_TYPE_VIDEO;
	c->bit_rate = videoBitrate * 1000;
	wxSize frameSize = GetFrameSize(videoFormat);
	c->width = frameSize.GetWidth();
	c->height = frameSize.GetHeight();
	c->time_base.den = isNTSC(videoFormat) ? 30000 : 25;
	c->time_base.num = isNTSC(videoFormat) ? 1001 : 1;
	c->gop_size = m_gopSize > 0 ? m_gopSize : (isNTSC(videoFormat) ? 15 : 12);
	c->pix_fmt = AV_PIX_FMT_YUV420P;
	c->rc_buffer_size = VIDEO_BUF_SIZE;
	c->rc_max_rate = cbr ? videoBitrate * 1000 : 9000000;
	c->rc_min_rate = cbr ? videoBitrate * 1000 : 0;
	
//	int qscale = 0;
//	if (qscale >= 0) {
//		c->flags |= AV_CODEC_FLAG_QSCALE;
//		c->global_quality = FF_QP2LAMBDA * qscale;
//	} 
	
	double ar = aspectRatio == ar16_9 ? (double) 16 / 9 : (double) 4 / 3;
	c->sample_aspect_ratio = av_d2q(ar * c->height / c->width, 255);
	m_videoStm->sample_aspect_ratio = c->sample_aspect_ratio;
	return true;
}

bool wxFfmpegMediaEncoder::addAudioStream(int codecId) {
	if (codecId == AV_CODEC_ID_NONE) {
		m_audioStm = NULL;
		return true;
	}
	m_audioStm = avformat_new_stream(m_outputCtx, NULL);
	if (!m_audioStm) {
		wxLogError(wxT("Could not alloc stream"));
		return false;
	}
	m_audioStm->id = 1;

	AVCodecContext* c = m_audioStm->codec;
	c->thread_count = m_threadCount;
	c->codec_id = (AVCodecID) codecId;
	c->codec_type = AVMEDIA_TYPE_AUDIO;
	c->bit_rate = 64000;
	c->sample_rate = 48000;
	c->sample_fmt = AV_SAMPLE_FMT_S16;
	c->channels = 2;
	c->time_base = (AVRational){ 1, c->sample_rate };
	// some formats want stream headers to be separate
	if(m_outputCtx->oformat->flags & AVFMT_GLOBALHEADER)
	    c->flags |= CODEC_FLAG_GLOBAL_HEADER;
	
	return true;
}

bool hasSampleFmt(AVCodec* codec, AVSampleFormat sample_fmt) {
	if (codec != NULL && codec->sample_fmts != NULL) {
		int fIdx = 0;
		while (codec->sample_fmts[fIdx] >= 0) {
			if (codec->sample_fmts[fIdx] == sample_fmt)
				return true;
			fIdx++;
		}
	}
	return false;
}

bool wxFfmpegMediaEncoder::OpenAudioEncoder() {
	AVCodecContext* c = m_audioStm->codec;
	
	// find the audio encoder and open it
	AVCodec* codec = NULL;
	if (c->codec_id == AV_CODEC_ID_AC3) {
		// There are 2 ac3 encoders (float and integer). Depending on libav implementation/version/fork,
		// one or the other may work. So we try both.
		codec = avcodec_find_encoder_by_name("ac3_fixed");
		if (!hasSampleFmt(codec, c->sample_fmt)) {
			// Try the encoding from float format
			c->sample_fmt = AV_SAMPLE_FMT_FLTP;
			codec = avcodec_find_encoder(c->codec_id);
		}
	} else {
 		c->sample_fmt = AV_SAMPLE_FMT_S16;
 		codec = avcodec_find_encoder(c->codec_id);
	}

	if (!codec) {
		wxLogError(wxT("Audio codec not found"));
		return false;
	}
	if (avcodec_open2(c, codec, NULL) < 0) {
		wxLogError(wxT("Could not open audio codec"));
		return false;
	}

    m_samples = (int16_t*) av_malloc(c->frame_size * av_get_bytes_per_sample(c->sample_fmt) * c->channels);
	memset(m_samples, 0, c->frame_size * av_get_bytes_per_sample(c->sample_fmt) * c->channels);
	
	m_audioFrame = av_frame_alloc();
	m_audioFrame->nb_samples = c->frame_size;
	avcodec_fill_audio_frame(m_audioFrame, c->channels, c->sample_fmt, (uint8_t *) m_samples, c->frame_size
			* av_get_bytes_per_sample(c->sample_fmt) * c->channels, 1);
	return true;
}

void wxFfmpegMediaEncoder::CloseAudioEncoder() {
	if (!m_audioStm)
		return;
	if (m_samples) {
		av_freep(&m_samples);
	}
	if (m_audioFrame) {
		av_frame_free(&m_audioFrame);
	}
	avcodec_close(m_audioStm->codec);
	m_audioStm = NULL;
}

AVFrame* allocPicture(AVPixelFormat pix_fmt, int width, int height) {
	AVFrame* frame = av_frame_alloc();
	if (!frame)
		return NULL;
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(53, 0, 0)
	frame->width = width;
	frame->height = height;
	frame->format = pix_fmt;
	if (av_frame_get_buffer(frame, 32) < 0) {
		av_free(frame);
		return NULL;
	}
#else
	int size = avpicture_get_size(pix_fmt, width, height);
	uint8_t* picture_buf = (uint8_t*) av_malloc(size);
	if (!picture_buf) {
		av_free(frame);
		return NULL;
	}
	avpicture_fill((AVPicture *) frame, picture_buf, pix_fmt, width, height);
#endif
	return frame;
}

bool wxFfmpegMediaEncoder::OpenVideoEncoder() {
	AVCodecContext* c = m_videoStm->codec;
	
	
	// find the video encoder and open it
	AVCodec* codec = avcodec_find_encoder(c->codec_id);
	if (!codec) {
		wxLogError(wxT("Video codec not found"));
		return false;
	}
	if (avcodec_open2(c, codec, NULL) < 0) {
		wxLogError(wxT("Could not open video codec"));
		return false;
	}

	m_videoOutbuf = (uint8_t*) av_malloc(VIDEO_BUF_SIZE);
	
	
	// allocate the encoded raw picture
	m_picture = allocPicture(c->pix_fmt, c->width, c->height);
	if (!m_picture) {
		wxLogError(wxT("Could not allocate frame"));
		return false;
	}

	m_imgConvertCtx = sws_getContext(c->width, c->height, AV_PIX_FMT_RGB24, c->width, c->height, c->pix_fmt, SWS_BICUBIC,
			NULL, NULL, NULL);
	if (!m_imgConvertCtx) {
		wxLogError(wxT("Cannot initialize the conversion context"));
		return false;
	}
	return true;
}

void wxFfmpegMediaEncoder::CloseVideoEncoder() {
	if (!m_videoStm)
		return;
	avcodec_close(m_videoStm->codec);
	if (m_imgConvertCtx)
		sws_freeContext(m_imgConvertCtx);
	if (m_picture) {
		av_freep(&m_picture->data[0]);
		av_freep(&m_picture);
	}
	av_freep(&m_videoOutbuf);
	m_videoStm = NULL;
}

bool wxFfmpegMediaEncoder::EncodeImage(wxImage image, int frames, AbstractProgressDialog* progressDialog) {
	if (!image.Ok())
		return false;
	AVCodecContext *c = m_videoStm->codec;
	if (image.GetWidth() != c->width || image.GetHeight() != c->height) {
		wxLogError(wxT("Image size (%dx%d) doesn't match frame size (%dx%d)."),
				image.GetWidth(), image.GetHeight(), c->width, c->height);
		return false;
	}
	// convert wxImage to the codec pixel format
	uint8_t *rgbSrc[3] = { image.GetData(), NULL, NULL };
	int rgbStride[3] = { 3 * image.GetWidth(), 0, 0 };
	sws_scale(m_imgConvertCtx, rgbSrc, rgbStride, 0, c->height, m_picture->data, m_picture->linesize);
	// encode audio and video
	double duration = ((double) m_nextVideoPts + frames) * m_videoStm->codec->time_base.num / m_videoStm->codec->time_base.den;
	while (true) {
		double audioPts = m_audioStm ? ((double) m_nextAudioPts) * m_audioStm->codec->time_base.num
				/ m_audioStm->codec->time_base.den : 0.0;
		double videoPts = ((double) m_nextVideoPts) * m_videoStm->codec->time_base.num / m_videoStm->codec->time_base.den;
		if (progressDialog->WasCanceled())
			return false;

		if ((!m_audioStm || audioPts >= duration) && (!m_videoStm || videoPts >= duration))
			break;
		
		// write interleaved audio and video frames
		if (m_audioStm && audioPts < videoPts) {
			if (!writeAudioFrame())
				return false;
		} else {
			if (!writeVideoFrame())
				return false;
			if (frames == 1)
				return true;
		}
	}
	return true;
}

bool wxFfmpegMediaEncoder::EncodeAudio(double duration, AbstractProgressDialog* progressDialog) {
	// encode audio
	while (true) {
		double audioPts = m_audioStm ? ((double) m_nextAudioPts) * m_audioStm->codec->time_base.num
				/ m_audioStm->codec->time_base.den : 0.0;
		if (progressDialog->WasCanceled())
			return false;

		if (!m_audioStm || audioPts >= duration)
			break;
		
		// write interleaved audio and video frames
		if (!writeAudioFrame())
			return false;
	}
	return true;
}

bool wxFfmpegMediaEncoder::writeAudioFrame() {
	AVPacket pkt = { 0 }; // data and size must be 0;
	int got_packet;

	av_init_packet(&pkt);
	AVCodecContext *c = m_audioStm->codec;
	
	m_audioFrame->pts = m_nextAudioPts;
	m_nextAudioPts += m_audioFrame->nb_samples;
	avcodec_encode_audio2(c, &pkt, m_audioFrame, &got_packet);
	if (!got_packet) {
		av_packet_unref(&pkt);
		return true;
	}

	pkt.stream_index = m_audioStm->index; 
	
	// write the compressed frame in the media file
	int ret = av_interleaved_write_frame(m_outputCtx, &pkt);
	if (ret < 0) {
		av_packet_unref(&pkt);
		print_error("Error while writing audio frame", ret);
		return false;
	}
	av_packet_unref(&pkt);
	return true;
}

bool wxFfmpegMediaEncoder::writeVideoFrame() {
	AVCodecContext *c = m_videoStm->codec;
	
	// encode the image
	m_picture->pts = m_nextVideoPts++;
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(54, 0, 0)
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = m_videoOutbuf;
	pkt.size = VIDEO_BUF_SIZE;
	
	int got_packet = 0;
	int ret = avcodec_encode_video2(c, &pkt, m_picture, &got_packet);
	if (ret < 0) {
		av_packet_unref(&pkt);
		print_error("Error while writing video frame", ret);
		return false;
	}
	if (got_packet) {
		if (pkt.pts != (int64_t) AV_NOPTS_VALUE)
			pkt.pts = av_rescale_q(pkt.pts, c->time_base, m_videoStm->time_base);
		if (pkt.dts != (int64_t) AV_NOPTS_VALUE)
			pkt.dts = av_rescale_q(pkt.dts, c->time_base, m_videoStm->time_base);
		pkt.stream_index = m_videoStm->index;
		
		// write the compressed frame in the media file
		ret = av_interleaved_write_frame(m_outputCtx, &pkt);
		if (ret < 0) {
			av_packet_unref(&pkt);
			print_error("Error while writing video frame", ret);
			return false;
		}
	}
#else
	int out_size = avcodec_encode_video(c, m_videoOutbuf, VIDEO_BUF_SIZE, m_picture);
	if (out_size < 0) {
		wxLogError(wxT("Video encoding failed"));
		return false;
	}
	// if zero size, it means the image was buffered
	if (out_size == 0)
		return true;
	AVPacket pkt;
	av_init_packet(&pkt);
	
	pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base, m_videoStm->time_base);
	if (c->coded_frame->key_frame)
		pkt.flags |= AV_PKT_FLAG_KEY;
	pkt.stream_index = m_videoStm->index;
	pkt.data = m_videoOutbuf;
	pkt.size = out_size;
	
	// write the compressed frame in the media file
	if (av_interleaved_write_frame(m_outputCtx, &pkt) != 0) {
		wxLogError(wxT("Error while writing video frame"));
		return false;
	}
#endif
	av_packet_unref(&pkt);
	return true;
}

void wxFfmpegMediaEncoder::EndEncode() {
	if (!m_outputCtx)
		return;

	// write the trailer
	if (m_outputCtx->nb_streams)
		av_write_trailer(m_outputCtx);
	
	CloseEncoder();
}

void wxFfmpegMediaEncoder::CloseEncoder() {
	if (!m_outputCtx)
		return;

	CloseVideoEncoder();
	CloseAudioEncoder();
	
	// free the streams
	for (unsigned int i = 0; i < m_outputCtx->nb_streams; i++) {
		av_freep(&m_outputCtx->streams[i]->codec);
		av_freep(&m_outputCtx->streams[i]);
	}

	// close the output file
	if (m_outputCtx->pb != NULL)
		avio_close(m_outputCtx->pb);
	
	// free the stream
	av_freep(&m_outputCtx);
}
