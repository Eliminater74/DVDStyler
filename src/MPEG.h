/////////////////////////////////////////////////////////////////////////////
// Name:        MPEG.h
// Purpose:     MPEG utils
// Author:      Alex Thuering
// Created:	04.04.2003
// RCS-ID:      $Id: MPEG.h,v 1.3 2012/06/24 23:00:09 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef DS_MPEG_H
#define DS_MPEG_H

#include <wx/wx.h>

class MPEG {
  public:
    static bool IsValid(const wxString& fileName);
    static bool HasNavPacks(const wxString& fileName);
};

#endif //DS_MPEG_H
