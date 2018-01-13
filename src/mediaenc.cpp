/////////////////////////////////////////////////////////////////////////////
// Name:        mediaenc.cpp
// Purpose:     Media Encoder Interface
// Author:      Alex Thuering
// Created:     2.05.2010
// RCS-ID:      $Id: mediaenc.cpp,v 1.6 2016/12/17 17:27:38 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "mediaenc.h"

/** Returns frame size of given video format */
wxSize GetFrameSize(VideoFormat format) {
	switch (format) {
	case vfNONE:
	case vfCOPY:
		break;
	case vfPAL:
		return wxSize(720, 576);
	case vfNTSC:
		return wxSize(720, 480);
	case vfPAL_CROPPED:
		return wxSize(704, 576);
	case vfNTSC_CROPPED:
		return wxSize(704, 480);
	case vfPAL_HALF_D1:
		return wxSize(352, 576);
	case vfNTSC_HALF_D1:
		return wxSize(352, 480);
	case vfPAL_VCD:
		return wxSize(352, 288);
	case vfNTSC_VCD:
		return wxSize(352, 240);
	case vfPAL_HALF_HD:
	case vfNTSC_HALF_HD:
		return wxSize(1280, 720);
	case vfPAL_HDV:
	case vfNTSC_HDV:
		return wxSize(1440, 1080);
	case vfPAL_FULL_HD:
	case vfNTSC_FULL_HD:
		return wxSize(1920, 1080);
	}
	return wxSize(0, 0);
}

/** Returns frame aspect ratio */
double GetFrameAspectRatio(AspectRatio aspect) {
	return aspect == ar4_3 ? 4.0/3 : 16.0/9;
}

/** Returns true if video format is NTSC */
bool isNTSC(VideoFormat format) {
	return format == vfNTSC || format == vfNTSC_CROPPED || format == vfNTSC_HALF_D1 || format == vfNTSC_VCD
			|| format == vfNTSC_HALF_HD || format == vfNTSC_HDV || format == vfNTSC_FULL_HD; 
}

/** Returns count of frames per second */
double GetFps(VideoFormat videoFormat, bool ntscFilm) {
	return isNTSC(videoFormat) ? (ntscFilm ? 24 : 30000.0/1001) : 25;
}
