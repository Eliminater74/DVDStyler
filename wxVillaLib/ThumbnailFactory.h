/////////////////////////////////////////////////////////////////////////////
// Name:        ThumbnailFactory.h
// Purpose:     wxThumbnailFactory class
// Author:      Alex Thuering
// Created:		15.02.2003
// RCS-ID:      $Id: ThumbnailFactory.h,v 1.10 2014/01/08 19:36:20 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WXVILLALIB_THUMBNAILFACTORY_H
#define WXVILLALIB_THUMBNAILFACTORY_H

#include <wx/wx.h>
#include <wx/image.h>
#include <wx/thread.h>
#include <wx/dynarray.h>

enum ThumbType {
  THUMBNAIL_NORMAL,
  THUMBNAIL_FAILED // cannot be generated 
};

class ThumbInfo; // type of entry  in a queue

WX_DEFINE_ARRAY(ThumbInfo*, ThumbInfoArray); // type of queue

class wxThumbnailFactory: public wxThread {
public:
    static void Init();
	/**
	 * It must be called by starting of application
	 */
	static void InitGnome(const char* appName, const char* appVersion, int argc, char** argv);
	/** 
	 * Returns thumbnail image if it is already generated or "wait" icon and put an entry to the queue
	 */
	static wxImage GetThumbnail(wxWindow* parent, wxString filename, int width, int height, bool* noOutline = NULL);
	/**
	 * Empty queue (we don't need to generate last thumbnails)
	 */
	static void ClearQueue(wxWindow* parent);
	/**
	 * Removes all thumbnails files from cache directory
	 */
	static void ClearCacheDir();
	/**
	 * Generates thumbnail image from video file
	 */
	static wxImage GenerateThumbnailFromVideo(const wxString& filename);

protected:
	static ThumbInfoArray m_queue;
	static wxMutex m_queueMutex;
	static wxThumbnailFactory* thread;
	static int maxFileSize;
	/** here we get entry from queue and generate thumbnails */
	wxThread::ExitCode Entry();
	/** add an entry to queue */
	static wxImage AddToQueue(ThumbInfo* info);
	/** creates a ThumbInfo structure and fill it
	 *  it test also if it's an image or a video
	 */
	static ThumbInfo* GetThumbInfo(wxString filename, wxWindow* parent, int width, int height);
	/** loads thumbnail, that was already generated */
	static wxImage LoadThumbnail(ThumbInfo& info, ThumbType type = THUMBNAIL_NORMAL);
	/** returns true if thumbnail can be generated */
	static bool CanThumbnail(ThumbInfo& info);
	/** returns path, where thumbnail is saved */
	static wxString GetThumbPath(ThumbInfo& info, ThumbType type = THUMBNAIL_NORMAL);
	/** generated a thumbnail */
	static wxImage GenerateThumbnail(ThumbInfo& info, bool save = true);
};

#endif //WXVILLALIB_THUMBNAILFACTORY_H
