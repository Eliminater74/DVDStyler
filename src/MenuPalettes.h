/////////////////////////////////////////////////////////////////////////////
// Name:        MenuPalettes.h
// Purpose:     The class to create and store menu palettes 
// Author:      Alex Thuering
// Created:	04.11.2006
// RCS-ID:      $Id: MenuPalettes.h,v 1.5 2014/02/25 11:08:27 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef MENU_PALETTES_H
#define MENU_PALETTES_H

#include "Menu.h"
#include <wx/palette.h>
#include <wx/hashmap.h>
#include <wx/hashset.h>

WX_DECLARE_HASH_SET(int, wxIntegerHash, wxIntegerEqual, IntHashSet);

class Palette: public wxPalette {
private:
    int coloursCount;
    bool m_hasTranspColour;
    
public:
    Palette() { coloursCount = 0; m_hasTranspColour = false; }
    virtual ~Palette() {}
    bool Create(IntHashSet& colours);    
    bool Apply(const unsigned char* pixel, unsigned char *dest, unsigned char *destAlpha) const;
};

WX_DECLARE_HASH_MAP(int, Palette, wxIntegerHash, wxIntegerEqual, PaletteMap);
WX_DECLARE_HASH_MAP(int, PaletteMap, wxIntegerHash, wxIntegerEqual, Palette2DMap);

class MenuPalettes {
private:
    Palette m_palette1;
    PaletteMap m_palette2;
    Palette2DMap m_palette3;
    bool m_drawButtonsOnBackground;
    int GetKey(const unsigned char* pixel, unsigned char alpha);
    
public:
    /** Constructor. Creates palettes */
	MenuPalettes(MenuObject& obj, bool drawButtonsOnBackground);
	
	/** Returns rgb value for first image (buttons normal) */
	bool Apply(const unsigned char* pixel1, unsigned char *dest, unsigned char *destAlpha);
	/** Returns rgb value for second image (buttons highlighted) */
	bool Apply(const unsigned char* pixel1, unsigned char pixel1Alpha, const unsigned char* pixel2,
			unsigned char *dest, unsigned char *destAlpha);
	/** Returns rgb value for third image (buttons selected) */
	bool Apply(const unsigned char* pixel1, unsigned char pixel1Alpha, const unsigned char* pixel2,
			unsigned char pixel2Alpha, const unsigned char* pixel3, unsigned char *dest, unsigned char *destAlpha);
};

#endif // MENU_PALETTES_H
