///////////////////////////////////////////////////////////////////////////////
// Name:        Palettes3D.h
// Purpose:     The class to create and store palette with 3 colours per entry 
// Author:      Alex Thuering
// Created:     05.11.2006
// RCS-ID:      $Id: Palette3D.h,v 1.3 2011/07/16 18:56:33 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
///////////////////////////////////////////////////////////////////////////////

#ifndef PALETTE_3D_H
#define PALETTE_3D_H

#include "MenuObject.h"
#include <wx/dynarray.h>
#include <wx/colour.h>

class Palette3D {
private:
	wxArrayInt m_colours1; // normal colour
	wxArrayInt m_colours2; // highlighted colour
	wxArrayInt m_colours3; // selected colour
	
public:
	Palette3D() {}
	virtual ~Palette3D() {}
	
	void Add(const wxColour& colour1, const wxColour& colour2, const wxColour& colour3);
	
	/**
	 * Searches for matched colours in palette and replace given with them.
	 * Returns true if colour was changed.
	 **/
	bool Apply(MenuObjectParam* param, bool drawButtonsOnBackground);
	
	/** Reduces the number of colours in palette to given value. */
	void ReduceColours(int maxColours);
	
	/** Returns the number of colours in palette */
	inline int GetColoursCount() { return m_colours1.GetCount(); }
};

#endif // PALETTE_3D_H
