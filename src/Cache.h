/////////////////////////////////////////////////////////////////////////////
// Name:        Cache.h
// Purpose:     Manage cache of transcoded files
// Author:      Alex Thuering
// Created:     29.08.2008
// RCS-ID:      $Id: Cache.h,v 1.8 2016/12/13 20:17:07 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DVDSTYLER_CACHE_H
#define DVDSTYLER_CACHE_H

#include "DVD.h"
#include "mediaenc.h"
#include <wx/hashmap.h>
#include <wx/window.h>
#include <vector>

/**
 * Stores cache entry information
 */
class CacheEntry {
public:
	CacheEntry(int idx);
	CacheEntry(Vob* vob, DVD* dvd, int idx);
	
	bool operator==(const CacheEntry& entry) const;
	
	inline int GetIdx() const { return m_idx; }
	inline const wxString& GetVideoFile() const { return m_videoFile; }
	inline bool IsUsed() const { return m_used; }
	inline void SetUsed(bool used) { m_used = used; }
	wxSvgXmlNode* GetXML();
	bool PutXML(wxSvgXmlNode* node);
	
private:
	wxString m_videoFile;
	wxArrayString m_audioFiles;
	wxArrayString m_subtitleFiles;
	wxArrayInt m_formats;
    int m_videoBitrate;
    int m_audioBitrate;
	double m_startTime;
	double m_recordingTime;
	wxString m_videoFilters;
	wxString m_audioFilters;
    int m_idx;
    bool m_used;
};

typedef vector<CacheEntry> CacheSet;

/**
 * Manage cache of transcoded files
 */
class Cache {
public:
	/**
	 * Constructor
	 */
	Cache();
	
	/**
	 * Destructor
	 */
	~Cache();
	
	/**
	 * Returns true if the cache is already initialized
	 */
	bool IsInitialized();
	
	/**
	 * Sets the temporally directory for cache files
	 */
	bool SetTempDir(wxString tempDir);
	
	/**
	 * Returns temporally directory
	 */
	wxString GetTempDir();
	
	/**
	 * Return count of entries in cache
	 */
	int GetCount();
	
	/**
	 * Return size of files in cache (in KB)
	 */
	long GetSize();
	
	/**
	 * Checks if there is a file for given VOB in the cache
	 * @return The file name in cache or empty string if no match entry in cache
	 */
	wxString Find(Vob* vob, DVD* dvd);
	
	/**
	 * Adds an entry for given VOB to the cache
	 * @return The file name in cache
	 */
	wxString Add(Vob* vob, DVD* dvd);
	
	/**
	 * Removes an entry for given VOB from the cache
	 */
	void Remove(Vob* vob, DVD* dvd);
	
	/**
	 * Removes all files from cache
	 */
	bool Clear();
	
	/**
	 * Begins checking for unused entries
	 */
	void BeginClean();
	
	/**
	 * Removes all unused entries
	 */
	void EndClean();
	
	/**
	 * Saves list of cache entries
	 */
	void Save();
	
	/**
	 * Shows prompt dialog and clears or saves cache 
	 */
	void ShowClearPrompt(wxWindow* parent);

private:
	wxString m_dir;
	CacheSet m_cacheSet;
	
	/**
	 * @return The file name of file in cache with given index
	 */
	wxString GetFileName(int index);
	
	/**
	 * @return The index for new cache entry
	 */
	int GenerateIndex();
	
	/**
	 * Loads list of cache entries
	 */
	bool Open();
};

#endif //DVDSTYLER_CACHE_H
