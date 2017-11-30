/////////////////////////////////////////////////////////////////////////////
// Name:        MenuObject.h
// Purpose:     The class to store a DVD Menu Object
// Author:      Alex Thuering
// Created:	04.11.2006
// RCS-ID:      $Id: MenuObject.h,v 1.34 2016/01/29 16:43:00 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#ifndef MENU_OBJECT_H
#define MENU_OBJECT_H

#include "DVDAction.h"
#include <wx/image.h>
#include <wx/dynarray.h>
#include <wxSVG/SVGLength.h>
#include <vector>
using namespace std;

class wxSvgXmlNode;
class wxSVGDocument;
class wxSVGElement;
class wxSVGUseElement;
class wxSVGSVGElement;
class wxSVGImageElement;
class wxSVGRect;
class wxSVGMatrix;
class wxSVGTransformList;
class wxSVGAnimateElement;
class Menu;
class DVD;

enum MenuButtonState { mbsNORMAL, mbsHIGHLIGHTED, mbsSELECTED };

enum SubStreamMode {
	ssmNORMAL = 0,
	ssmWIDESCREEN,
	ssmLETTERBOX,
	ssmPANSCAN
};

struct MenuObjectParam {
  wxString name;
  wxString title;
  wxString type;
  vector<wxString> element; // id of element
  wxString attribute; // attribute name
  bool changeable; // changed if select the button
  wxColour normalColour;
  wxColour highlightedColour;
  wxColour selectedColour;
  inline bool isChangeable() {
	  return changeable && (normalColour != highlightedColour || normalColour != selectedColour);
  }
};

WX_DEFINE_ARRAY(MenuObjectParam*, MenuObjectParams);

struct MenuObjectSize {
	unsigned int value;
	unsigned int valueInc;
	unsigned int valuePercent;
	wxArrayString elements;
};

enum NavigationButton {
  nbLEFT = 0,
  nbRIGHT,
  nbUP,
  nbDOWN
};

class MenuObject {
public:
	/** Constructor */
	MenuObject(Menu* menu, bool vmg, wxString fileName = wxT(""), int x = 0, int y = 0, wxString param = wxT(""));
	/** Destructor */
	virtual ~MenuObject();
	
	/** Returns id of menu object */
	virtual wxString GetId(bool translate = false);
	/** Returns true id this menu object is a button */
	inline bool IsButton() { return m_button; }
	/** Returns button action */
	inline DVDAction& GetAction() { return m_action; }
	
	/** Returns true, if it's an auto-execute button. */
	inline bool IsAutoExecute() { return m_autoExecute; }
	/** Sets, if it's an auto-execute button. */
	inline void SetAutoExecute(bool autoExecute) { m_autoExecute = autoExecute; }
    
    inline wxString GetFileName() { return m_fileName; }
    inline wxString GetTitle() { return m_title; }
    
    int GetX() const;
    void SetX(int value);
    int GetY() const;
    void SetY(int value);
    int GetWidth() const;
    void SetWidth(int value);
    int GetHeight() const;
    void SetHeight(int value);
    void SetRect(wxRect rect);
    double GetAngle() const;
    void SetAngle(double value);
    wxRect GetBBox() const;
    wxRect GetResultBBox() const;
    wxRect GetFrameBBox(SubStreamMode mode, bool ignorePadding = false);
	
	inline bool IsDefaultSize() { return m_defaultSize; }
	inline void SetDefaultSize(bool value = true) { m_defaultSize = value; }
	inline bool IsKeepAspectRatio() { return m_keepAspectRatio; }
	inline void SetKeepAspectRatio(bool value = true) { m_keepAspectRatio = value; }
    void FixSize(int& width, int& height);
    void UpdateSize();
    void UpdateMatrix(wxSVGMatrix& matrix) const;
    
    int GetObjectParamsCount() const { return m_params.Count(); }
    MenuObjectParam* GetObjectParam(int index) const { return m_params[index]; }
    MenuObjectParam* GetObjectParam(wxString name) const;
    MenuObjectParam* GetInitParam();
    MenuObjectParam* GetImageParam();
    int GetChangebaleColourCount(bool drawButtonsOnBackground);
    const wxSVGLength& GetTextOffset() const { return m_textOffset; }
    
    wxString GetParam(wxString name, wxString attribute = wxT("")) const;
	void SetParam(wxString name, wxString value, wxString attribute = wxT(""));
    int GetParamInt(wxString name, wxString attribute = wxT("")) const;
	void SetParamInt(wxString name, int value, wxString attribute = wxT(""));
	double GetParamDouble(wxString name) const;
	void SetParamDouble(wxString name, double value);
	wxFont GetParamFont(wxString name) const;
	void SetParamFont(wxString name, wxFont value);
	wxColour GetParamColour(wxString name, MenuButtonState state = mbsNORMAL) const;
	void SetParamColour(wxString name, wxColour value, MenuButtonState state = mbsNORMAL);
	double GetParamVideoClipBegin(const wxString& name);
	double GetParamVideoDuration(const wxString& name);
	void SetParamImageVideo(const wxString& name, const wxString& filename, long pos, int duration);
	/** Returns object svg element by element id */
	wxSVGElement* GetElementById(wxString id);
	/** Returns animation elements */
	vector<wxSVGAnimateElement*> GetAnimations();
	/** Sets animation elements */
	void SetAnimations(vector<wxSVGAnimateElement*>& animations);
	
	/** Returns true, if video frame must be displayed (if button has image parameter and "jump to title" action) */
	inline bool IsDisplayVideoFrame() { return m_displayVideoFrame; }
	/** Sets, if video frame must be displayed */
	inline void SetDisplayVideoFrame(bool displayVideoFrame) { m_displayVideoFrame = displayVideoFrame; }
	/** Returns true, if custom video frame is selected */
	inline bool IsCustomVideoFrame() { return m_customVideoFrame; }
	/** Sets, if custom video frame is selected */
	inline void SetCustomVideoFrame(bool customVideoFrame) { m_customVideoFrame = customVideoFrame; }
	/** Returns VOB-ID of displayed frame */
	inline int GetDisplayVobId() { return m_displayVobId; }
	/** Sets VOB-ID of displayed frame */
	inline void SetDisplayVobId(int displayVobId) { m_displayVobId = displayVobId; }
	
    void ToFront();
    void Forward();
    void Backward();
    void ToBack();
    bool IsFirst();
    bool IsLast();
	
	wxString GetFocusDest(NavigationButton navButton) { return m_direction[navButton]; }
	void SetFocusDest(NavigationButton navButton, wxString value) { m_direction[navButton] = value; }
	wxString GetDefaultFocusDest(NavigationButton navButton);
	
	wxImage GetImage(int maxWidth, int maxHeight);
	
	virtual wxSvgXmlNode* GetXML(DVDFileType type, DVD* dvd, SubStreamMode mode = ssmNORMAL, bool withSVG = false);
	virtual bool PutXML(wxSvgXmlNode* node);
	
	static void SetImageVideoParams(wxSVGSVGElement* svgElem, const wxString& id, const wxString& filename, long pos,
			int duration);
private:
	Menu* m_menu; // can be null
	wxString m_id;
	bool m_button;
	bool m_autoExecute;
	DVDAction m_action;
	wxString m_fileName;
	wxString m_title;
	bool m_previewHighlighted;
	
	wxString m_direction[4]; // left, right, up, down button names
	
	MenuObjectParams m_params;
	wxString m_initParameter;
	wxSVGLength m_textOffset;
	bool m_displayVideoFrame; // sets if video frame must be displayed
	bool m_customVideoFrame; // shows if custom video frame is selected
	int m_displayVobId; // VOB-ID of displayed frame
	
	bool m_keepAspectRatio;
	wxSVGImageElement* m_aspectRatioElem;
		
	bool m_defaultSize;
	MenuObjectSize m_defaultWidth;
	MenuObjectSize m_defaultHeight;
	
	MenuObjectSize m_minWidth;
	MenuObjectSize m_minHeight;
	
	bool m_deleteSVG;
	wxSVGDocument* m_svg;
	wxSVGUseElement* m_use;
	wxSVGSVGElement* m_symbol;
	wxSVGSVGElement* AddSymol(wxString id, wxSVGElement* content);
	void AddUse(wxString id, int x, int y, int width, int height, const wxSVGTransformList* transforms = NULL);
	
	void SetScale(double scaleX, double scaleY);
	
	bool Init(wxString filename, int x = 0, int y = 0, wxString param = wxT(""));
	bool LoadSVG(wxSVGDocument& svg, wxSvgXmlNode* node);
	wxString GenerateId(wxString prefix);
	void InitSize(wxString value, MenuObjectSize& size);
	unsigned int CalcSize(MenuObjectSize& size, bool width);
	bool IsAlignRight(MenuObjectSize& size);
	void UpdateTransform();
};

#endif // MENU_OBJECT_H
