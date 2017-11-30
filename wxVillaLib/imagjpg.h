#if !wxCHECK_VERSION(2,9,0)

#ifndef _WX_IMAGJPG_H_
#define _WX_IMAGJPG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "imagjpg.h"
#endif

#include "wx/image.h"

#ifndef wxIMAGE_OPTION_MAX_WIDTH
	#define wxIMAGE_OPTION_MAX_WIDTH wxT("max_width")
	#define wxIMAGE_OPTION_MAX_HEIGHT wxT("max_height")
#endif

//-----------------------------------------------------------------------------
// wxJPGHandler
//-----------------------------------------------------------------------------

#if wxUSE_LIBJPEG
class WXDLLEXPORT wxJPGHandler: public wxImageHandler
{
public:
    inline wxJPGHandler()
    {
        m_name = wxT("JPEG file");
        m_extension = wxT("jpg");
        m_type = wxBITMAP_TYPE_JPEG;
        m_mime = wxT("image/jpeg");
    }

#if wxUSE_STREAMS
    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 );
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true );
protected:
    virtual bool DoCanRead( wxInputStream& stream );
#endif

private:
    DECLARE_DYNAMIC_CLASS(wxJPGHandler)
};

#endif // wxUSE_LIBJPEG

bool LoadImageFile(wxImage& img, wxInputStream& stream,
  long type = wxBITMAP_TYPE_ANY, int index = -1);
bool LoadImageFile(wxImage& img, const wxString& filename,
 long type = wxBITMAP_TYPE_ANY, int index = -1);

#endif // _WX_IMAGJPG_H_

#endif // !wxCHECK_VERSION(2,9,0)
