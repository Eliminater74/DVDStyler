/////////////////////////////////////////////////////////////////////////////
// Name:        mediaenc.h
// Purpose:     Media Encoder Interface
// Author:      Alex Thuering
// Created:     24.02.2007
// RCS-ID:      $Id: mediaenc.h,v 1.13 2016/12/17 17:27:38 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_MEDIA_ENCODER_H
#define WX_MEDIA_ENCODER_H

#include <wx/image.h>

enum VideoFormat {
	vfNONE = 0,
	vfCOPY,
	vfPAL,
	vfNTSC,
	vfPAL_CROPPED,
	vfNTSC_CROPPED,
	vfPAL_HALF_D1,
	vfNTSC_HALF_D1,
	vfPAL_VCD,
	vfNTSC_VCD,
	vfPAL_HALF_HD,
	vfNTSC_HALF_HD,
	vfPAL_HDV,
	vfNTSC_HDV,
	vfPAL_FULL_HD,
	vfNTSC_FULL_HD
};

enum AudioFormat {
	afNONE = 0,
	afCOPY,
	afMP2,
	afAC3,
	afPCM
};

enum SubtitleFormat {
	sfNONE = 0,
	sfCOPY,
	sfDVD
};

enum AspectRatio {
	arAUTO = 0,
	ar4_3,
	ar16_9
};

enum FirstField {
	ffAUTO = -1,
	ffBOTTOM = 0,
	ffTOP = 1
};

enum WidescreenType {
	wtAUTO = 0,
	wtNOPANSCAN,
	wtNOLETTERBOX
};

/** Returns frame size of given video format */
wxSize GetFrameSize(VideoFormat format);
/** Returns frame aspect ratio */
double GetFrameAspectRatio(AspectRatio aspect);
/** Returns true if video format is NTSC */
bool isNTSC(VideoFormat format);
/** Returns count of frames per second */
double GetFps(VideoFormat format, bool ntscFilm);

/**
 * Interface that have to be implemented with progress dialog.
 */
class AbstractProgressDialog {
public:
	virtual ~AbstractProgressDialog() {}
	/** Return whether "Cancel" button was pressed */
	virtual bool WasCanceled() = 0;
};

#endif // WX_MEDIA_ENCODER_H
