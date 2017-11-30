/////////////////////////////////////////////////////////////////////////////
// Name:        MenuObject.cpp
// Purpose:     The class to store a DVD Menu Object
// Author:      Alex Thuering
// Created:     04.11.2006
// RCS-ID:      $Id: MenuObject.cpp,v 1.64 2016/06/20 10:41:04 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////

#include "DVD.h"
#include "Menu.h"
#include <wxVillaLib/utils.h>
#include <wxSVG/svg.h>
#include <wxSVGXML/svgxmlhelpr.h>
#include <wx/mstream.h>
#include <wx/filename.h>
#include <wx/regex.h>
#include <wx/tokenzr.h>
#include <wx/log.h>

#define BUTTONS_DIR wxFindDataDirectory(_T("buttons"))
#define OBJECTS_DIR wxFindDataDirectory(_T("objects"))

const wxString TRANS_ELEM_ID = wxT("s_trans");
wxRegEx s_jumpTitlesetMenu(wxT("jump titleset ([[:digit:]]+) menu;"));

///////////////////////////// MenuObject /////////////////////////////////////

MenuObject::MenuObject(Menu* menu, bool vmg, wxString fileName, int x, int y, wxString param):
		m_action(vmg), m_textOffset(0), m_svg(NULL), m_use(NULL), m_symbol(NULL) {
	m_menu = menu;
	if (menu == NULL) {
		m_deleteSVG = true;
		m_svg = Menu::CreateSVG(400, 400);
		wxSVGRectElement* bgRect = new wxSVGRectElement;
		bgRect->SetWidth(400);
		bgRect->SetHeight(400);
		bgRect->SetFill(wxSVGPaint(*wxBLACK));
		m_svg->GetRootElement()->InsertChild(bgRect, m_svg->GetRootElement()->GetChildren());
	} else {
		m_deleteSVG = false;
		m_svg = menu->GetSVG();
	}
	
	m_fileName = fileName;
	m_previewHighlighted = false;

	m_button = false;
	m_autoExecute = false;
	m_displayVideoFrame = true;
	m_customVideoFrame = false;
	m_displayVobId = DVD::MakeId(0, 0, 0);

	m_defaultSize = true;
	m_defaultWidth.value = m_defaultHeight.value = 0;
	m_defaultWidth.valueInc = m_defaultHeight.valueInc = 0;
	m_defaultWidth.valuePercent = m_defaultHeight.valuePercent = 0;
	m_minWidth.value = m_minHeight.value = 0;
	m_minWidth.valueInc = m_minHeight.valueInc = 0;
	m_minWidth.valuePercent = m_minHeight.valuePercent = 0;
	m_keepAspectRatio = false;
	m_aspectRatioElem = NULL;

	if (fileName.length())
		Init(fileName, x, y, param);
}

MenuObject::~MenuObject() {
	WX_CLEAR_ARRAY(m_params)
	if (m_use)
		m_use->GetParent()->RemoveChild(m_use);
	if (m_symbol)
		m_symbol->GetParent()->RemoveChild(m_symbol);
	if (m_deleteSVG)
		delete m_svg;
}

bool MenuObject::Init(wxString fileName, int x, int y, wxString param) {
	m_fileName = fileName;
	wxSvgXmlDocument xml;
	xml.Load(fileName);
	if (!xml.GetRoot())
		return false;
	wxSvgXmlNode* root = xml.GetRoot();

	m_button = root->GetName() == wxT("button");
	m_displayVideoFrame = m_button; // display video frame is default for button but not for objects
	m_previewHighlighted = root->GetAttribute(wxT("previewHighlighted")) == wxT("true");
	m_title = XmlReadValue(root, wxT("title"));
	
	if (m_id.length()) { // load button
		m_symbol = (wxSVGSVGElement*) m_svg->GetElementById(wxT("s_") + m_id);
		m_use = (wxSVGUseElement*) m_svg->GetElementById(m_id);
	} else {
		// create new button
		m_id = GenerateId(m_button ? wxT("button") : wxT("obj"));
		wxSvgXmlNode* node = XmlFindNode(root, wxT("default-size"));
		wxString val;
		if (node && node->GetPropVal(wxT("keepAspectRatio"), &val) && (val == wxT("true") || val == wxT("1")))
			m_keepAspectRatio = true;
	}

	wxSvgXmlNode* svgNode = XmlFindNode(root, wxT("svg"));
	if (svgNode) {
		wxSVGDocument svg;
		LoadSVG(svg, svgNode);
		wxSVGSVGElement* root = svg.GetRootElement();
		if (root) {
			m_defaultWidth.value = root->GetWidth().GetBaseVal();
			m_defaultHeight.value = root->GetHeight().GetBaseVal();
			if (!m_symbol)
				AddSymol(m_id, root);
			if (!m_use)
				AddUse(m_id, x, y, m_defaultWidth.value, m_defaultHeight.value);
			wxSvgXmlNode* imageNode = XmlFindNode(svgNode, wxT("image"));
			if (imageNode != NULL) {
				wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(imageNode->GetAttribute(wxT("id")));
				if (elem && elem->GetDtd() == wxSVG_IMAGE_ELEMENT)
					m_aspectRatioElem = (wxSVGImageElement*) elem;
			}
		}
	}

	// load parameters 
	wxSvgXmlNode* paramsNode = XmlFindNode(root, wxT("parameters"));
	if (paramsNode) {
		wxSvgXmlNode* child = paramsNode->GetChildren();
		while (child) {
			if (child->GetType() == wxSVGXML_ELEMENT_NODE && child->GetName() == wxT("parameter")) {
				MenuObjectParam* param = new MenuObjectParam;
				param->title = XmlReadValue(child, wxT("title"));
				param->name = XmlReadValue(child, wxT("name"));
				param->type = XmlReadValue(child, wxT("type"));
				wxStringTokenizer tokenizer(XmlReadValue(child, wxT("element")), wxT(","), wxTOKEN_RET_EMPTY_ALL);
				while (tokenizer.HasMoreTokens()) {
					param->element.push_back(tokenizer.GetNextToken());
				}
				param->attribute = XmlReadValue(child, wxT("attribute"));
				param->changeable = XmlFindNode(child, wxT("changeable")) != NULL && param->type == wxT("colour");
				m_params.Add(param);
				if (param->changeable) {
					param->changeable = false;
					param->normalColour = GetParamColour(param->name);
					wxCSSStyleDeclaration style;
					style.SetProperty(wxCSS_PROPERTY_STROKE, XmlReadValue(child, wxT("default-value/highlighted")));
					param->highlightedColour = style.GetStroke().GetRGBColor();
					style.SetProperty(wxCSS_PROPERTY_STROKE, XmlReadValue(child, wxT("default-value/selected")));
					param->selectedColour = style.GetStroke().GetRGBColor();
					param->changeable = true;
				}
			}
			child = child->GetNext();
		}
	}

	m_initParameter = XmlReadValue(root, wxT("init-parameter"));
	// set initial parameter value
	if (m_initParameter.length() && param.length())
		SetParam(m_initParameter, param);
	
	m_textOffset.SetValueAsString(XmlReadValue(root, wxT("text-offset")));

	// load default size
	wxSvgXmlNode* defaultSizeNode = XmlFindNode(root, wxT("default-size"));
	if (defaultSizeNode && defaultSizeNode->GetFirstChild()) {
		wxString sizeElem = XmlReadValue(defaultSizeNode, wxT("element"));
		if (sizeElem.length() > 0) {
			m_defaultWidth.elements.Add(sizeElem);
			m_defaultHeight.elements.Add(sizeElem);
		}
		InitSize(XmlReadValue(defaultSizeNode, wxT("width")), m_defaultWidth);
		InitSize(XmlReadValue(defaultSizeNode, wxT("height")), m_defaultHeight);
	}

	// load min size
	wxSvgXmlNode* minSizeNode = XmlFindNode(root, wxT("min-size"));
	if (minSizeNode) {
		wxString sizeElem = XmlReadValue(minSizeNode, wxT("element"));
		if (sizeElem.length() > 0) {
			m_minWidth.elements.Add(sizeElem);
			m_minHeight.elements.Add(sizeElem);
		}
		InitSize(XmlReadValue(minSizeNode, wxT("width")), m_minWidth);
		InitSize(XmlReadValue(minSizeNode, wxT("height")), m_minHeight);
	}

	UpdateSize();

	return true;
}

void MenuObject::InitSize(wxString value, MenuObjectSize& size) {
	long lval = 0;
	if (value.Index(wxT('|'))) {
		if (value.BeforeFirst(wxT('|')).ToLong(&lval))
			size.value = lval;
		value = value.AfterFirst(wxT('|'));
	}
	while (value.Length() > 0) {
		wxString val = value.BeforeFirst(wxT('+'));
		if (val.Last() == wxT('%') && val.SubString(0, val.length()-2).ToLong(&lval))
			size.valuePercent += lval;
		else if (val.ToLong(&lval))
			size.valueInc += lval;
		else
			size.elements.Add(val);
		value = value.AfterFirst(wxT('+'));
	}
}

bool MenuObject::LoadSVG(wxSVGDocument& svg, wxSvgXmlNode* node) {
	bool res;
	wxSvgXmlDocument xml;
	xml.SetRoot(node->CloneNode());
	wxMemoryOutputStream output;
	xml.Save(output);
	wxMemoryInputStream input(output);
	res = svg.Load(input);
	return res;
}

wxString MenuObject::GenerateId(wxString prefix) {
	int i = 1;
	while (1) {
		wxString id = prefix + wxString::Format(wxT("%02d"), i);
		if (m_svg->GetElementById(id) == NULL)
			return id;
		i++;
	}
	return wxT("");
}

MenuObjectParam* MenuObject::GetObjectParam(wxString name) const {
	for (unsigned int i = 0; i < m_params.size(); i++) {
		if (m_params[i]->name == name)
			return m_params[i];
	}
	return NULL;
}

MenuObjectParam* MenuObject::GetInitParam() {
	if (!m_initParameter.length())
		return NULL;
	return GetObjectParam(m_initParameter);
}

MenuObjectParam* MenuObject::GetImageParam() {
	for (unsigned int i = 0; i < m_params.size(); i++) {
		if (m_params[i]->type == wxT("image"))
			return m_params[i];
	}
	return NULL;
}

int MenuObject::GetChangebaleColourCount(bool drawButtonsOnBackground) {
	int count = 0;
	for (int i = 0; i < (int) m_params.Count(); i++) {
		if (m_params[i]->isChangeable() && ((m_params[i]->normalColour.IsOk() && !drawButtonsOnBackground)
				|| m_params[i]->highlightedColour.IsOk() || m_params[i]->selectedColour.IsOk()))
			count++;
	}
	return count;
}

wxSVGSVGElement* MenuObject::AddSymol(wxString id, wxSVGElement* content) {
	m_symbol = new wxSVGSVGElement;
	m_symbol->SetId(wxT("s_") + id);
	m_svg->GetElementById(wxT("defs"))->AppendChild(m_symbol);
	if (content->GetDtd() == wxSVG_SVG_ELEMENT) {
		m_symbol->SetViewBox(((wxSVGSVGElement*) content)->GetViewBox());
		m_symbol->SetPreserveAspectRatio(((wxSVGSVGElement*) content)->GetPreserveAspectRatio());
	} else if (content->GetDtd() == wxSVG_SYMBOL_ELEMENT) {
		m_symbol->SetViewBox(((wxSVGSymbolElement*) content)->GetViewBox());
		m_symbol->SetPreserveAspectRatio(((wxSVGSymbolElement*) content)->GetPreserveAspectRatio());
	}
	wxSvgXmlElement* child = content->GetChildren();
	while (child) {
		m_symbol->AppendChild(((wxSVGSVGElement*) child)->CloneNode());
		child = child->GetNext();
	}
	return m_symbol;
}

void MenuObject::AddUse(wxString id, int x, int y, int width, int height, const wxSVGTransformList* transforms) {
	m_use = new wxSVGUseElement;
	m_use->SetId(id);
	m_use->SetHref(wxT("#s_") + id);
	m_use->SetX(x);
	m_use->SetY(y);
	m_use->SetWidth(width);
	m_use->SetHeight(height);
	if (transforms)
		m_use->SetTransform(*transforms);
	if (IsButton())
		m_svg->GetElementById(wxT("buttons"))->AppendChild(m_use);
	else
		m_svg->GetElementById(wxT("objects"))->AppendChild(m_use);
}

void MenuObject::SetScale(double scaleX, double scaleY) {
	wxSVGGElement* transElem = (wxSVGGElement*) m_symbol->GetElementById(TRANS_ELEM_ID);
	if (transElem == NULL) {
		if (scaleX == 1 && scaleY == 1)
			return;
		// add transElem
		transElem = new wxSVGGElement();
		transElem->SetId(TRANS_ELEM_ID);
		m_symbol->AddChild(transElem);
		// move all children elements in gElem
		while (m_symbol->GetFirstChild() != transElem) {
			wxSvgXmlNode* elem = m_symbol->GetFirstChild();
			m_symbol->RemoveChild(elem);
			transElem->AppendChild(elem);
		}
	}
	wxSVGTransformList transList;
	if (scaleX != 1 || scaleY != 1) {
		wxSVGTransform transform;
		transform.SetScale(scaleX, scaleY);
		transList.Add(transform);
	}
	// copy old values
	const wxSVGTransformList& oldTransList = transElem->GetTransform().GetBaseVal();
	for (unsigned int i = 0; i < oldTransList.size(); i++)
		if (i > 0 || oldTransList[i].GetType() != wxSVG_TRANSFORM_SCALE)
			transList.Add(oldTransList[i]);
	transElem->SetTransform(transList);
}

wxString MenuObject::GetId(bool translate) {
	if (!translate)
		return m_id;
	long l = 0;
	m_id.Mid(IsButton() ? 6 : 3).ToLong(&l);
	return (IsButton() ? _("button") : _("object")) + wxString::Format(wxT(" %d"), (int) l);
}

int MenuObject::GetX() const {
	return m_use->GetX().GetBaseVal();
}
void MenuObject::SetX(int value) {
	m_use->SetX(value);
	UpdateTransform();
}

int MenuObject::GetY() const {
	return m_use->GetY().GetBaseVal();
}
void MenuObject::SetY(int value) {
	m_use->SetY(value);
	UpdateTransform();
}

int MenuObject::GetWidth() const {
	return m_use->GetWidth().GetBaseVal();
}
void MenuObject::SetWidth(int value) {
	m_use->SetWidth(value);
	UpdateTransform();
}

int MenuObject::GetHeight() const {
	return m_use->GetHeight().GetBaseVal();
}
void MenuObject::SetHeight(int value) {
	m_use->SetHeight(value);
	UpdateTransform();
}

void MenuObject::SetRect(wxRect rect) {
	m_use->SetX(rect.x);
	m_use->SetY(rect.y);
	m_use->SetWidth(rect.width);
	m_use->SetHeight(rect.height);
	UpdateTransform();
}

void MenuObject::UpdateTransform() {
	if (m_use->GetTransform().GetBaseVal().size() > 0
			&& m_use->GetTransform().GetBaseVal()[0].GetType() == wxSVG_TRANSFORM_ROTATE) {
		wxSVGTransform& transform = m_use->GetTransform().GetBaseVal()[0];
		transform.SetRotate(transform.GetAngle(), GetX() + GetWidth()/2, GetY() + GetHeight()/2);
	}
}

double MenuObject::GetAngle() const {
	MenuObjectParam* param = GetObjectParam(wxT("rotation"));
	if (param != NULL) {
		return GetParamDouble(wxT("rotation"));
	}
	return m_use->GetTransform().GetBaseVal().size() > 0
			&& m_use->GetTransform().GetBaseVal()[0].GetType() == wxSVG_TRANSFORM_ROTATE
			? m_use->GetTransform().GetBaseVal()[0].GetAngle() : 0;
}

void MenuObject::SetAngle(double angle) {
	if (angle > 360)
		angle -= 360;
	if (angle < 0)
		angle += 360;
	MenuObjectParam* param = GetObjectParam(wxT("rotation"));
	if (param != NULL) {
		SetParamDouble(wxT("rotation"), angle);
		return;
	}
	if (m_use->GetTransform().GetBaseVal().size() > 0
			&& m_use->GetTransform().GetBaseVal()[0].GetType() == wxSVG_TRANSFORM_ROTATE) {
		if (angle != 0) {
			m_use->GetTransform().GetBaseVal()[0].SetRotate(angle, GetX() + GetWidth()/2, GetY() + GetHeight()/2);
		} else {
			m_use->SetTransform(wxSVGTransformList());
		}
	} else if (angle != 0) {
		m_use->SetTransform(wxSVGTransformList());
		m_use->Rotate(angle, GetX() + GetWidth()/2, GetY() + GetHeight()/2);
	}
}

void MenuObject::UpdateMatrix(wxSVGMatrix& matrix) const {
	m_use->UpdateMatrix(matrix);
}

wxRect MenuObject::GetBBox() const {
	return wxRect(GetX(), GetY(), GetWidth(), GetHeight());
}

wxRect MenuObject::GetResultBBox() const {
	wxRect bbox = wxRect(GetX(), GetY(), GetWidth(), GetHeight());
//	wxSVGElement* cElem = (wxSVGElement*) m_symbol->GetElementById(wxT("circle"));
//	if (cElem != NULL) {
//		wxSVGSVGElement* viewportElem = new wxSVGSVGElement();
//		viewportElem->SetWidth(m_use->GetWidth().GetBaseVal());
//		viewportElem->SetHeight(m_use->GetHeight().GetBaseVal());
//		cElem->SetViewportElement(viewportElem);
//		wxSVGRect elemBox = wxSVGLocatable::GetElementResultBBox(cElem);
//		cElem->SetViewportElement(NULL);
//		delete viewportElem;
//		bbox = wxRect(GetX() + elemBox.GetX(), GetY() + elemBox.GetY(), elemBox.GetWidth(), elemBox.GetHeight());
//	}
	if (m_use->GetTransform().GetBaseVal().size() > 0
			&& m_use->GetTransform().GetBaseVal()[0].GetType() == wxSVG_TRANSFORM_ROTATE) {
		wxSVGTransform& transform = m_use->GetTransform().GetBaseVal()[0];
		double angle = transform.GetAngle();
		if (angle != 0) {
			wxSVGPoint point[4] = {
					wxSVGPoint(bbox.GetX(), bbox.GetY()).MatrixTransform(transform.GetMatrix()),
					wxSVGPoint(bbox.GetX() + bbox.GetWidth(), bbox.GetY()).MatrixTransform(transform.GetMatrix()),
					wxSVGPoint(bbox.GetX(), bbox.GetY() + bbox.GetHeight()).MatrixTransform(transform.GetMatrix()),
					wxSVGPoint(bbox.GetX() + bbox.GetWidth(), bbox.GetY() + bbox.GetHeight()).MatrixTransform(transform.GetMatrix())
			};
			wxSVGPoint p1 = point[0];
			wxSVGPoint p2 = point[0];
			for (int i = 1; i < 4; i++) {
				if (p1.GetX() > point[i].GetX())
					p1.SetX(point[i].GetX());
				if (p1.GetY() > point[i].GetY())
					p1.SetY(point[i].GetY());
				if (p2.GetX() < point[i].GetX())
					p2.SetX(point[i].GetX());
				if (p2.GetY() < point[i].GetY())
					p2.SetY(point[i].GetY());
			}
			return wxRect(p1.GetX(), p1.GetY(), p2.GetX() - p1.GetX(), p2.GetY() - p1.GetY());
		}
	}
	return bbox;
}

wxRect MenuObject::GetFrameBBox(SubStreamMode mode, bool ignorePadding) {
	wxRect bbox = GetResultBBox();
	double fx = mode == ssmPANSCAN ? 4.0/3 : 1.0;
	double fy = mode == ssmLETTERBOX ? 0.75 : 1.0;
	if (m_menu != NULL)
		fy *= (double) m_menu->GetFrameResolution().GetHeight() / m_menu->GetResolution().GetHeight();
	int width = round(bbox.GetWidth()*fx);
	int height = round(bbox.GetHeight()*fy);
	int padY = mode == ssmLETTERBOX && !ignorePadding ? m_menu->GetFrameResolution().GetHeight()*0.125: 0;
	int cropX = mode == ssmPANSCAN ? m_menu->GetFrameResolution().GetWidth()*0.125 : 0;
	int x = round((bbox.GetX() - cropX)*fx);
	int y = round(bbox.GetY()*fy) + padY;
	if (height % 2 == 1)
		height += y % 2 == 0 ? 1 : -1;
	if (y % 2 == 1)
		y++;
	return wxRect(x, y, width, height);
}

unsigned int MenuObject::CalcSize(MenuObjectSize& size, bool width) {
	unsigned int result = 0;
	for (unsigned int idx = 0; idx < size.elements.GetCount(); idx++) {
		wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(size.elements[idx]);
		if (elem) {
			if (elem->GetDtd() == wxSVG_IMAGE_ELEMENT) {
				wxSVGImageElement* imgElem = (wxSVGImageElement*) elem;
				result += width ? imgElem->GetDefaultWidth() : imgElem->GetDefaultHeight();
			} else {
				wxSVGRect bbox = wxSVGLocatable::GetElementResultBBox(elem);
				result += width ? (unsigned int) bbox.GetWidth() : (unsigned int) bbox.GetHeight();
			}
		}
	}
	if (size.valuePercent > 0)
		result += lround(((double) result * size.valuePercent) / 100);
	result += size.valueInc;
	return wxMax(result, size.value);
}

bool MenuObject::IsAlignRight(MenuObjectSize& size) {
	if (size.elements.size() == 0) {
		return false;
	}
	wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(size.elements[0]);
	if (!elem || elem->GetDtd() != wxSVG_TEXT_ELEMENT) {
		return false;
	}
	return ((wxSVGTextElement*) elem)->GetTextAnchor() == wxCSS_VALUE_END;
}

#include <wxSVG/SVGCanvasItem.h>

void MenuObject::FixSize(int& width, int& height) {
	// calculate aspect ratio
	double aspectRatio = -1;
	if (m_keepAspectRatio && m_aspectRatioElem != NULL && m_aspectRatioElem->GetDefaultWidth() > 0)
		aspectRatio = ((double) m_aspectRatioElem->GetDefaultWidth()) / m_aspectRatioElem->GetDefaultHeight();
	
	if (m_defaultSize) {
		width = CalcSize(m_defaultWidth, true);
		if (aspectRatio > 0)
			height = width / aspectRatio;
		else
			height = CalcSize(m_defaultHeight, false);
	} else {
		width = wxMax(width, (int) CalcSize(m_minWidth, true));
		if (m_keepAspectRatio) {
			if (aspectRatio > 0)
				height = width / aspectRatio;
			else
				height = width * CalcSize(m_defaultHeight, false) / CalcSize(m_defaultWidth, true);
		} else
			height = wxMax(height, (int) CalcSize(m_minHeight, false));
	}
}

void MenuObject::UpdateSize() {
	int width = GetWidth();
	int height = GetHeight();
	FixSize(width, height);
	if (IsAlignRight(m_defaultSize ? m_defaultWidth : m_minWidth) && width != GetWidth()) {
		SetX(GetX() + GetWidth() - width);
	}
	SetWidth(width);
	SetHeight(height);
	if (GetX() + GetWidth() <= 0)
		SetX(0);
	if (GetY() + GetHeight() <= 0)
		SetY(0);
}

wxString MenuObject::GetParam(wxString name, wxString attribute) const {
	MenuObjectParam* param = GetObjectParam(name);
	if (!param)
		return wxT("");
	wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(param->element.front());
	if (!elem)
		return wxT("");
	if (param->attribute.length()) {
		// return attribute value
		if (param->attribute.Find(wxT('#')) > 0) {
			wxString value = elem->GetAttribute(param->attribute.BeforeFirst(wxT('#')));
			long n = 0;
			param->attribute.AfterFirst(wxT('#')).ToLong(&n);
			if (n > 0 && value.length() > 0 && value.Index(wxT('(')) > 0) {
				// return n-parameter of the function (e.g. if you want to get rotation angle of transform)
				value = value.AfterFirst(wxT('(')).BeforeFirst(wxT(')'));
				for (int i = 1; i < n; i++)
					value = value.AfterFirst(wxT(','));
				value = value.BeforeFirst(wxT(','));
				return value.Trim().Trim(false);
			}
		}
		return elem->GetAttribute(attribute.length() && attribute[0] != wxT('-') ?
				attribute : param->attribute + attribute);
	} else if (elem->GetDtd() == wxSVG_TEXT_ELEMENT) {
		if (attribute.length())
			return elem->GetAttribute(attribute);
		wxString value;
		wxSvgXmlNode* child = elem->GetChildren();
		while (child != NULL) {
			if (child->GetType() == wxSVGXML_TEXT_NODE)
				value += child->GetContent().Strip(wxString::both);
			else if (child->GetType() == wxSVGXML_ELEMENT_NODE
					&& ((wxSVGElement*) child)->GetDtd() == wxSVG_TBREAK_ELEMENT)
				value += wxT("\n");
			child = child->GetNextSibling();
		}
		return value;
	}
	return wxT("");
}

void MenuObject::SetParam(wxString name, wxString value, wxString attribute) {
	MenuObjectParam* param = GetObjectParam(name);
	if (!param)
		return;
	for (vector<wxString>::const_iterator elemIt = param->element.begin(); elemIt != param->element.end(); elemIt++) {
		wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(*elemIt);
		if (!elem)
			continue;
		if (param->attribute.length()) {
			if (param->attribute.Find(wxT('#')) > 0) {
				// sets n-parameter of the function (e.g. if you want to set rotation angle of transform)
				wxString oldValue = elem->GetAttribute(param->attribute.BeforeFirst(wxT('#')));
				long n = 0;
				param->attribute.AfterFirst(wxT('#')).ToLong(&n);
				if (n > 0 && oldValue.length() > 0 && oldValue.Index(wxT('(')) > 0) {
					wxString newValue = oldValue.BeforeFirst(wxT('(')) + wxT('(');
					for (int i = 1; i < n; i++)
						newValue += oldValue.BeforeFirst(wxT(',')) + wxT(',');
					newValue += value;
					if (oldValue.Index(wxT(')')) < oldValue.Index(wxT(',')) && oldValue.Index(wxT(')')) > 0)
						newValue += wxT(')') + oldValue.AfterFirst(wxT(')'));
					else
						newValue += wxT(',') + oldValue.AfterFirst(wxT(','));
					value = newValue;
				}
				elem->SetAttribute(param->attribute.BeforeFirst(wxT('#')), value);
			} else {
				elem->SetAttribute(attribute.length() && attribute[0] != wxT('-') ?
						attribute : param->attribute + attribute, value);
			}
			if (elem->GetDtd() == wxSVG_IMAGE_ELEMENT && param->attribute == wxT("xlink:href"))
				((wxSVGImageElement*) elem)->SetCanvasItem(NULL);
		} else if (elem->GetDtd() == wxSVG_TEXT_ELEMENT) {
			if (attribute.length()) {
				elem->SetAttribute(attribute, value);
				return;
			}
			wxStringTokenizer tokenizer(value, wxT("\r\n"), wxTOKEN_RET_EMPTY_ALL);
			wxSvgXmlNode* child = elem->GetChildren();
			while (tokenizer.HasMoreTokens()) {
				wxString token = tokenizer.GetNextToken();
				// insert text node (token)
				if (child != NULL && child->GetType() == wxSVGXML_TEXT_NODE) {
					child->SetContent(token);
					child = child->GetNextSibling();
				} else
					elem->InsertChild(new wxSvgXmlNode(wxSVGXML_TEXT_NODE, wxEmptyString, token), child);
				// insert t-break
				if (tokenizer.HasMoreTokens()) {
					if (child != NULL && child->GetType() == wxSVGXML_ELEMENT_NODE
							&& ((wxSVGElement*) child)->GetDtd() != wxSVG_TBREAK_ELEMENT)
						child = child->GetNextSibling();
					else
						elem->InsertChild(new wxSVGTBreakElement(), child);
				}
				((wxSVGTextElement*) elem)->SetXmlspace(wxT("preserve"));
			}
			while (child != NULL) {
				wxSvgXmlNode* nextChild = child->GetNextSibling();
				elem->RemoveChild(child);
				child = nextChild;
			}
			((wxSVGTextElement*) elem)->SetCanvasItem(NULL);
		}
	}
}

int MenuObject::GetParamInt(wxString name, wxString attribute) const {
	long lval = 0;
	GetParam(name, attribute).ToLong(&lval);
	return lval;
}

void MenuObject::SetParamInt(wxString name, int value, wxString attribute) {
	SetParam(name, wxString::Format(wxT("%d"), value), attribute);
}

double MenuObject::GetParamDouble(wxString name) const {
	double dval = 0;
	GetParam(name).ToDouble(&dval);
	return dval;
}

void MenuObject::SetParamDouble(wxString name, double value) {
	SetParam(name, wxString::Format(wxT("%g"), value));
}

wxFont MenuObject::GetParamFont(wxString name) const {
	MenuObjectParam* param = GetObjectParam(name);
	if (!param)
		return wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
	wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(param->element.front());
	if (!elem) {
		return wxFont(20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false);
	}

	int size = 20;
	double dval;
	if (elem->GetAttribute(wxT("font-size")).ToDouble(&dval))
		size = (int) dval;

	wxFontStyle style = wxFONTSTYLE_NORMAL;
	wxString styleStr = elem->GetAttribute(wxT("font-style"));
	if (styleStr == wxT("italic"))
		style = wxFONTSTYLE_ITALIC;
	else if (styleStr == wxT("oblique"))
		style = wxFONTSTYLE_SLANT;

	wxFontWeight weight = wxFONTWEIGHT_NORMAL;
	wxString weightStr = elem->GetAttribute(wxT("font-weight"));
	if (weightStr == wxT("bold"))
		weight = wxFONTWEIGHT_BOLD;
	if (weightStr == wxT("bolder"))
		weight = wxFONTWEIGHT_MAX;
	else if (weightStr == wxT("lighter"))
		weight = wxFONTWEIGHT_LIGHT;

	wxString faceName = elem->GetAttribute(wxT("font-family"));

	return wxFont(size, wxFONTFAMILY_DEFAULT, style, weight, false, faceName);
}

void MenuObject::SetParamFont(wxString name, wxFont value) {
	MenuObjectParam* param = GetObjectParam(name);
	if (!param)
		return;
	for (vector<wxString>::const_iterator elemIt = param->element.begin(); elemIt != param->element.end(); elemIt++) {
		wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(*elemIt);
		if (!elem)
			continue;
	
		elem->SetAttribute(wxT("font-size"), wxString::Format(wxT("%d"), value.GetPointSize()));
	
		wxString styleStr = wxT("normal");
		if (value.GetStyle() == wxFONTSTYLE_ITALIC)
			styleStr = wxT("italic");
		else if (value.GetStyle() == wxFONTSTYLE_SLANT)
			styleStr = wxT("oblique");
		elem->SetAttribute(wxT("font-style"), styleStr);
	
		wxString weightStr = wxT("normal");
		if (value.GetWeight() == wxFONTWEIGHT_BOLD)
			weightStr = wxT("bold");
		if (value.GetWeight() == wxFONTWEIGHT_MAX)
			weightStr = wxT("bolder");
		else if (value.GetWeight() == wxFONTWEIGHT_LIGHT)
			weightStr = wxT("lighter");
		elem->SetAttribute(wxT("font-weight"), weightStr);
	
		elem->SetAttribute(wxT("font-family"), value.GetFaceName());
		
		if (elem->GetDtd() == wxSVG_TEXT_ELEMENT)
			((wxSVGTextElement*) elem)->SetCanvasItem(NULL);
	}
}

wxColour MenuObject::GetParamColour(wxString name, MenuButtonState state) const {
	MenuObjectParam* param = GetObjectParam(name);
	if (!param)
		return wxColour();
	if (param->changeable) {
		switch (state) {
		case mbsNORMAL:
			return param->normalColour;
		case mbsHIGHLIGHTED:
			return param->highlightedColour;
		case mbsSELECTED:
			return param->selectedColour;
		}
	}
	wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(param->element.front());
	if (!elem)
		return wxColour();
	if (param->attribute.length()) {
		const wxCSSStyleDeclaration& style = wxSVGStylable::GetElementStyle(*elem);
		const wxCSSValue& value = style.GetPropertyCSSValue(param->attribute);
		switch (value.GetCSSValueType()) {
		case wxCSS_PRIMITIVE_VALUE:
			return ((wxCSSPrimitiveValue&) value).GetRGBColorValue();
		case wxCSS_SVG_COLOR:
		case wxCSS_SVG_PAINT:
			return ((wxSVGColor&) value).GetRGBColor();
		default:
			break;
		}
	}
	return wxColour();
}

void MenuObject::SetParamColour(wxString name, wxColour value, MenuButtonState state) {
	MenuObjectParam* param = GetObjectParam(name);
	if (!param)
		return;
	if (param->changeable) {
		switch (state) {
		case mbsNORMAL:
			param->normalColour = value;
			break;
		case mbsHIGHLIGHTED:
			param->highlightedColour = value;
			break;
		case mbsSELECTED:
			param->selectedColour = value;
			break;
		}
	}
	if (state != mbsNORMAL)
		return;
	wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(param->element.front());
	if (!elem)
		return;
	if (param->attribute.length()) {
		wxSVGPaint paint(value);
		elem->SetAttribute(param->attribute, paint.GetCSSText());
	}
}

double MenuObject::GetParamVideoClipBegin(const wxString& name) {
	MenuObjectParam* param = GetObjectParam(name);
	if (!param)
		return 0;
	wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(param->element.front());
	if (!elem || elem->GetDtd() != wxSVG_VIDEO_ELEMENT)
		return 0;
	return ((wxSVGVideoElement*) elem)->GetClipBegin();
}

double MenuObject::GetParamVideoDuration(const wxString& name) {
	MenuObjectParam* param = GetObjectParam(name);
	if (!param)
		return 0;
	wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(param->element.front());
	if (!elem || elem->GetDtd() != wxSVG_VIDEO_ELEMENT)
		return 0;
	return ((wxSVGVideoElement*) elem)->GetDur();
}

void MenuObject::SetParamImageVideo(const wxString& name, const wxString& filename, long pos, int duration) {
	MenuObjectParam* param = GetObjectParam(name);
	if (!param)
		return;
	for (vector<wxString>::const_iterator elemIt = param->element.begin(); elemIt != param->element.end(); elemIt++) {
		MenuObject::SetImageVideoParams(m_symbol, *elemIt, filename, pos, duration);
	}
}

void MenuObject::SetImageVideoParams(wxSVGSVGElement* svgElem, const wxString& id, const wxString& filename,
		long pos, int duration) {
	wxSVGElement* elem = (wxSVGElement*) svgElem->GetElementById(id);
	if (!elem || (elem->GetDtd() != wxSVG_IMAGE_ELEMENT && elem->GetDtd() != wxSVG_VIDEO_ELEMENT))
		return;
	if (duration <= 0) {
		if (elem->GetDtd() != wxSVG_IMAGE_ELEMENT) {
			wxSVGVideoElement* oldElem = (wxSVGVideoElement*) elem;
			wxSVGImageElement* newElem = new wxSVGImageElement;
			newElem->SetId(oldElem->GetId());
			newElem->SetX(oldElem->GetX().GetBaseVal());
			newElem->SetY(oldElem->GetY().GetBaseVal());
			newElem->SetWidth(oldElem->GetWidth().GetBaseVal());
			newElem->SetHeight(oldElem->GetHeight().GetBaseVal());
			newElem->SetStyle(oldElem->GetStyle());
			newElem->SetPreserveAspectRatio(oldElem->GetPreserveAspectRatio());
			oldElem->GetParent()->InsertBefore(newElem, oldElem);
			oldElem->GetParent()->RemoveChild(oldElem);
			elem = newElem;
		}
		wxString href = filename + (filename.length() && pos >= 0 ? wxString::Format(wxT("#%ld"), pos) : wxT(""));
		if (href != ((wxSVGImageElement*) elem)->GetHref()) {
			((wxSVGImageElement*) elem)->SetCanvasItem(NULL);
			((wxSVGImageElement*) elem)->SetHref(href);
		}
	} else {
		if (elem->GetDtd() != wxSVG_VIDEO_ELEMENT) {
			wxSVGImageElement* oldElem = (wxSVGImageElement*) elem;
			wxSVGVideoElement* newElem = new wxSVGVideoElement;
			newElem->SetId(oldElem->GetId());
			newElem->SetX(oldElem->GetX().GetBaseVal());
			newElem->SetY(oldElem->GetY().GetBaseVal());
			newElem->SetWidth(oldElem->GetWidth().GetBaseVal());
			newElem->SetHeight(oldElem->GetHeight().GetBaseVal());
			newElem->SetStyle(oldElem->GetStyle());
			newElem->SetPreserveAspectRatio(oldElem->GetPreserveAspectRatio());
			oldElem->GetParent()->InsertBefore(newElem, oldElem);
			oldElem->GetParent()->RemoveChild(oldElem);
			elem = newElem;
		}
		if (filename != ((wxSVGVideoElement*) elem)->GetHref()) {
			((wxSVGVideoElement*) elem)->SetCanvasItem(NULL);
			((wxSVGVideoElement*) elem)->SetHref(filename);
		}
		((wxSVGVideoElement*) elem)->SetClipBegin(((double)pos)/1000);
		((wxSVGVideoElement*) elem)->SetDur(duration);
	}
}

/** Returns object svg element by element id */
wxSVGElement* MenuObject::GetElementById(wxString id) {
	return (wxSVGElement*) m_symbol->GetElementById(id);
}

void GetAnimationElements(wxSVGElement* child, vector<wxSVGAnimateElement*>& animations) {
	while (child) {
		if (child->GetType() == wxSVGXML_ELEMENT_NODE && child->GetDtd() == wxSVG_ANIMATE_ELEMENT) {
			animations.push_back((wxSVGAnimateElement*) child);
		}
		child = (wxSVGElement*) child->GetNextSibling();
	}
}

/** Returns animation elements */
vector<wxSVGAnimateElement*> MenuObject::GetAnimations() {
	vector<wxSVGAnimateElement*> animations;
	GetAnimationElements((wxSVGElement*) m_use->GetFirstChild(), animations);
	GetAnimationElements((wxSVGElement*) m_symbol->GetFirstChild(), animations);
	return animations;
}

void RemoveAnimationElements(wxSVGElement* child) {
	while (child != NULL) {
		wxSVGElement* nextChild = (wxSVGElement*) child->GetNextSibling();
		if (child->GetType() == wxSVGXML_ELEMENT_NODE && child->GetDtd() == wxSVG_ANIMATE_ELEMENT) {
			child->GetParent()->RemoveChild(child);
			delete child;
		}
		child = nextChild;
	}
}

/** Sets animation elements */
void MenuObject::SetAnimations(vector<wxSVGAnimateElement*>& animations) {
	RemoveAnimationElements((wxSVGElement*) m_symbol->GetFirstChild());
	RemoveAnimationElements((wxSVGElement*) m_use->GetFirstChild());
	for (auto anim : animations) {
		if (anim->GetHref().length() >0)
			m_symbol->AddChild(anim);
		else
			m_use->AddChild(anim);
	}
}

void MenuObject::ToFront() {
	wxSVGElement* parent = (wxSVGElement*) m_use->GetParent();
	parent->RemoveChild(m_use);
	parent->AppendChild(m_use);
}

void MenuObject::Forward() {
	wxSVGElement* parent = (wxSVGElement*) m_use->GetParent();
	wxSVGElement* next = (wxSVGElement*) m_use->GetNextSibling();
	if (next && next->GetType() == wxSVGXML_ELEMENT_NODE && next->GetDtd() == wxSVG_USE_ELEMENT) {
		parent->RemoveChild(m_use);
		parent->InsertChild(m_use, next->GetNextSibling());
	}
}

void MenuObject::Backward() {
	wxSVGElement* parent = (wxSVGElement*) m_use->GetParent();
	wxSVGElement* prev = (wxSVGElement*) m_use->GetPreviousSibling();
	if (prev && prev->GetType() == wxSVGXML_ELEMENT_NODE && prev->GetDtd() == wxSVG_USE_ELEMENT) {
		parent->RemoveChild(m_use);
		parent->InsertChild(m_use, prev);
	}
}

void MenuObject::ToBack() {
	wxSVGElement* parent = (wxSVGElement*) m_use->GetParent();
	wxSVGElement* first = (wxSVGElement*) parent->GetFirstChild();
	while (first && (first->GetType() != wxSVGXML_ELEMENT_NODE
			|| first->GetDtd() != wxSVG_USE_ELEMENT))
		first = (wxSVGElement*) first->GetNextSibling();
	if (first && first != m_use) {
		parent->RemoveChild(m_use);
		parent->InsertChild(m_use, first);
	}
}

bool MenuObject::IsFirst() {
	wxSVGElement* prev = (wxSVGElement*) m_use->GetPreviousSibling();
	return !prev || prev->GetType() != wxSVGXML_ELEMENT_NODE;
}

bool MenuObject::IsLast() {
	wxSVGElement* next = (wxSVGElement*) m_use->GetNextSibling();
	return !next || next->GetType() != wxSVGXML_ELEMENT_NODE;
}

wxString MenuObject::GetDefaultFocusDest(NavigationButton navButton) {
	map<int, wxArrayString> btMap; // dist -> buttons
	map<int, wxArrayString> btMapAll; // dist -> buttons
	int minDist = 99999;
	int minDistAll = 99999;
	wxRect rect = GetBBox();
	for (unsigned int i = 0; i < m_menu->GetObjectsCount(); i++) {
		MenuObject* obj = m_menu->GetObject(i);
		if (!obj->IsButton())
			continue;
		wxRect objRect = obj->GetBBox();
		int dist = -1;
		if (navButton == nbLEFT)
			dist = rect.GetX() - objRect.GetRight() - 1;
		else if (navButton == nbRIGHT)
			dist = objRect.GetX() - rect.GetRight() - 1;
		else if (navButton == nbUP)
			dist = rect.GetY() - objRect.GetBottom() - 1;
		else // nbDOWN
			dist = objRect.GetY() - rect.GetBottom() - 1;
		if (dist < 0)
			continue;
		if (navButton == nbLEFT || navButton == nbRIGHT)
			objRect.SetX(rect.GetX());
		else // nbUP/nbDOWN
			objRect.SetY(rect.GetY());
		if (objRect.Intersects(rect)) {
			btMap[dist].Add(obj->GetId());
			if (dist < minDist)
				minDist = dist;
		}
		btMapAll[dist].Add(obj->GetId());
		if (dist < minDistAll)
			minDistAll = dist;
	}
	// filter by minDist
	wxArrayString buttons;
	for (map<int, wxArrayString>::iterator it = btMap.begin(); it != btMap.end(); it++)
		if (it->first <= minDist + 4)
			buttons.insert(buttons.begin(), it->second.begin(), it->second.end());
	if (buttons.IsEmpty()) {
		if (btMapAll.empty())
			return GetId(); // 'none'
		buttons = btMapAll[minDistAll];
	}
	// filter by distance to left/up
	wxString result;
	int resultDist = 0;
	for (unsigned int i = 0; i < buttons.size(); i++) {
		MenuObject* obj = m_menu->GetObject(buttons[i]);
		wxRect objRect = obj->GetBBox();
		int dist = navButton == nbLEFT || navButton == nbRIGHT
				? objRect.GetY() - rect.GetY() : objRect.GetX() - rect.GetX();
		if (result.length() == 0) {
			result = obj->GetId();
			resultDist = dist;
		} else if (dist < resultDist) {
			result = obj->GetId();
			resultDist = dist;
		}
	}
	return result.length() ? result : GetId(); // GetID() -> 'none'
}

wxImage MenuObject::GetImage(int maxWidth, int maxHeight) {
	if (!m_use)
		return wxImage();
	if (m_previewHighlighted) {
		for (int i = 0; i < GetObjectParamsCount(); i++) {
			MenuObjectParam* param = GetObjectParam(i);
			if (param->changeable) {
				wxSVGElement* elem = (wxSVGElement*) m_symbol->GetElementById(param->element.front());
				if (elem && param->attribute.length()) {
					wxSVGPaint paint(param->highlightedColour);
					elem->SetAttribute(param->attribute, paint.GetCSSText());
				}
			}
		}
	}
	m_svg->GetRootElement()->SetWidth(GetWidth());
	m_svg->GetRootElement()->SetHeight(GetHeight());
	return m_svg->Render(maxWidth, maxHeight);
}

wxSvgXmlNode* MenuObject::GetXML(DVDFileType type, DVD* dvd, SubStreamMode mode, bool withSVG) {
	wxString rootName = wxT("object");
	if (IsButton())
		rootName = IsAutoExecute() && type == SPUMUX_XML ? wxT("action") : wxT("button");
	wxSvgXmlNode* rootNode = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, rootName);
	switch (type) {
	case DVDSTYLER_XML:
		if (IsButton())
			rootNode->AddChild(m_action.GetXML(type, dvd));
		if (GetFocusDest(nbLEFT).length() || GetFocusDest(nbRIGHT).length()
				|| GetFocusDest(nbUP).length() || GetFocusDest(nbDOWN).length()) {
			wxSvgXmlNode* directionNode = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, _T("direction"));
			if (GetFocusDest(nbLEFT).length())
				directionNode->AddProperty(_T("left"), GetFocusDest(nbLEFT));
			if (GetFocusDest(nbRIGHT).length())
				directionNode->AddProperty(_T("right"), GetFocusDest(nbRIGHT));
			if (GetFocusDest(nbUP).length())
				directionNode->AddProperty(_T("up"), GetFocusDest(nbUP));
			if (GetFocusDest(nbDOWN).length())
				directionNode->AddProperty(_T("down"), GetFocusDest(nbDOWN));
			rootNode->AddChild(directionNode);
		}
		XmlWriteValue(rootNode, _T("filename"), GetFileName().AfterLast(wxFILE_SEP_PATH));
		for (int i = 0; i < GetObjectParamsCount(); i++) {
			MenuObjectParam* param = GetObjectParam(i);
			if (param->changeable) {
				wxSvgXmlNode* paramNode = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, _T("parameter"));
				paramNode->AddProperty(_T("name"), param->name);
				paramNode->AddProperty(_T("normal"), wxSVGPaint(param->normalColour).GetCSSText());
				paramNode->AddProperty(_T("highlighted"), wxSVGPaint(param->highlightedColour).GetCSSText());
				paramNode->AddProperty(_T("selected"), wxSVGPaint(param->selectedColour).GetCSSText());
				rootNode->AddChild(paramNode);
			}
		}
		rootNode->AddProperty(wxT("id"), GetId());
		if (m_defaultSize)
			rootNode->AddProperty(wxT("defSize"), wxT("true"));
		if (IsAutoExecute())
			rootNode->AddProperty(wxT("autoExecute"), wxT("true"));
		if (m_keepAspectRatio)
			rootNode->AddProperty(wxT("keepAspectRatio"), wxT("true"));
		if (!m_displayVideoFrame)
			rootNode->AddProperty(wxT("displayVideoFrame"), wxT("false"));
		if (m_customVideoFrame)
			rootNode->AddProperty(wxT("customVideoFrame"), wxT("true"));
		if (m_displayVobId != DVD::MakeId(0, 0, 0))
			rootNode->AddProperty(wxT("vobId"), wxString::Format(_T("%d"), m_displayVobId));
		break;
	case SPUMUX_XML: {
		int mwidth = (int) m_menu->GetFrameResolution().GetWidth() - 1;
		int mheight = (int) m_menu->GetFrameResolution().GetHeight() - 1;
		rootNode->AddProperty(_T("name"), GetId());
		wxRect rect = GetFrameBBox(mode);
		rootNode->AddProperty(_T("x0"), wxString::Format(_T("%d"), rect.GetX() > 0 ? rect.GetX() : 0));
		rootNode->AddProperty(_T("y0"), wxString::Format(_T("%d"), rect.GetY() > 0 ? rect.GetY() : 0));
		rootNode->AddProperty(_T("x1"), wxString::Format(_T("%d"),
				rect.GetX() + rect.GetWidth() < mwidth ? rect.GetX() + rect.GetWidth() : mwidth));
		rootNode->AddProperty(_T("y1"), wxString::Format(_T("%d"),
				rect.GetY() + rect.GetHeight() < mheight ? rect.GetY() + rect.GetHeight() : mheight));
		wxString focusDest = GetFocusDest(nbLEFT).length() ? GetFocusDest(nbLEFT) : GetDefaultFocusDest(nbLEFT);
		if (focusDest.length())
			rootNode->AddProperty(_T("left"), focusDest);
		focusDest = GetFocusDest(nbRIGHT).length() ? GetFocusDest(nbRIGHT) : GetDefaultFocusDest(nbRIGHT);
		if (focusDest.length())
			rootNode->AddProperty(_T("right"), focusDest);
		focusDest = GetFocusDest(nbUP).length() ? GetFocusDest(nbUP) : GetDefaultFocusDest(nbUP);
		if (focusDest.length())
			rootNode->AddProperty(_T("up"), focusDest);
		focusDest = GetFocusDest(nbDOWN).length() ? GetFocusDest(nbDOWN) : GetDefaultFocusDest(nbDOWN);
		if (focusDest.length())
			rootNode->AddProperty(_T("down"), focusDest);
		break;
	}
	case DVDAUTHOR_XML: {
		rootNode->AddProperty(_T("name"), GetId());
		wxString action = GetAction().AsString(dvd);
		action.Replace(wxT("vmMenu"), wxT("vmgm menu"));
		if (dvd->GetPlayAllRegister() != -1)
			action = wxString::Format(wxT("g%d=%d;"), dvd->GetPlayAllRegister(),
					GetAction().IsPlayAll() ? (GetAction().IsPlayAllTitlesets() ? 2 : 1): 0) + action;
		if (m_menu->GetRememberLastButton()) {
			int reg = m_menu->GetRememberLastButtonRegister();
			if (reg == -1)
				reg = dvd->GetRememberLastButtonRegister();
			action = wxString::Format(wxT("g%d=button;"), reg) + action;
		}
		rootNode->AddChild(new wxSvgXmlNode(wxSVGXML_TEXT_NODE, wxEmptyString, action));
		break;
	}
	default:
		break;
	}
	if (withSVG) {  // used by copy & paste
		rootNode->DeleteProperty(wxT("id"));
		wxSvgXmlNode* svgNode = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, wxT("svg"));
		wxSvgXmlNode* defsNode = new wxSvgXmlNode(wxSVGXML_ELEMENT_NODE, wxT("defs"));
		wxSVGElement* symbol = (wxSVGElement*) m_symbol->CloneNode();
		symbol->SetId(wxT(""));
		defsNode->AddChild(symbol);
		svgNode->AddChild(defsNode);
		svgNode->AddChild(m_use->CloneNode());
		rootNode->AddChild(svgNode);
	}
	return rootNode;
}

bool MenuObject::PutXML(wxSvgXmlNode* node) {
	m_button = node->GetName() == wxT("button");

	long lval;
	wxString val;

	if (IsButton()) {
		wxSvgXmlNode* actionNode = XmlFindNodeSimple(node, _T("action"));
		if (actionNode)
			m_action.PutXML(actionNode);

		wxSvgXmlNode* dirNode = XmlFindNodeSimple(node, _T("direction"));
		if (dirNode) {
			if (dirNode->GetPropVal(_T("left"), &val))
				SetFocusDest(nbLEFT, val);
			if (dirNode->GetPropVal(_T("right"), &val))
				SetFocusDest(nbRIGHT, val);
			if (dirNode->GetPropVal(_T("up"), &val))
				SetFocusDest(nbUP, val);
			if (dirNode->GetPropVal(_T("down"), &val))
				SetFocusDest(nbDOWN, val);
		}
	}

	// deprecated
	int x = 0, y = 0;
	if (node->GetPropVal(_T("x"), &val) && val.ToLong(&lval))
		x = (int) (lval / m_svg->GetScale());
	if (node->GetPropVal(_T("y"), &val) && val.ToLong(&lval))
		y = (int) (lval / m_svg->GetScale());
	wxString fileName = XmlReadValue(node, _T("type"));
	if (fileName.length()) {
		wxString text = XmlReadValue(node, _T("text"));
		return Init(BUTTONS_DIR + wxT("text.xml"), x, y, text);
	}

	fileName = XmlReadValue(node, _T("filename"));
	if (!fileName.length())
		return false;

	wxFileName fn(fileName);
	wxString dir = IsButton() ? BUTTONS_DIR : OBJECTS_DIR;
	if (fn.IsRelative()) {
		if (wxFileName::FileExists(dir + fileName))
			fileName = dir + fileName;
		else if (wxFileName::FileExists(dir + wxT("deprecated") + wxFILE_SEP_PATH + fileName))
			fileName = dir + wxT("deprecated") + wxFILE_SEP_PATH + fileName;
		else if (wxFileName::FileExists(dir + fn.GetFullName()))
			fileName = dir + fn.GetFullName();
		else {
			wxLogError(_("can't open file '%s'"), fileName.c_str());
			return false;
		}
	} else if (!wxFileExists(fileName)) {
		if (wxFileName::FileExists(dir + fn.GetFullName()))
			fileName = dir + fn.GetFullName();
		else if (wxFileName::FileExists(dir + wxT("deprecated") + wxFILE_SEP_PATH + fn.GetFullName()))
			fileName = dir + wxT("deprecated") + wxFILE_SEP_PATH + fn.GetFullName();
		else {
			wxLogError(_("can't open file '%s'"), fileName.c_str());
			return false;
		}
	}

	node->GetPropVal(wxT("id"), &m_id);
	m_autoExecute = node->GetPropVal(wxT("autoExecute"), &val) && (val == wxT("true") || val == wxT("1"));
	if (!node->GetPropVal(wxT("defSize"), &val) || (val != wxT("true") && val != wxT("1")))
		m_defaultSize = false;

	// paste button
	wxSvgXmlNode* svgNode = XmlFindNode(node, wxT("svg"));
	if (svgNode) {
		m_id = GenerateId(m_button ? wxT("button") : wxT("obj"));
		wxSVGDocument svg;
		LoadSVG(svg, svgNode);
		wxSVGSVGElement* root = svg.GetRootElement();
		if (root && XmlFindNode(root, wxT("defs")) && XmlFindNode(root, wxT("use"))) {
			wxSVGElement* defsNode = (wxSVGElement*) XmlFindNode(root, wxT("defs"));
			wxSvgXmlElement* child = defsNode->GetChildren();
			while (child) {
				AddSymol(m_id, (wxSVGElement*) child);
				child = child->GetNext();
			}
			wxSVGUseElement* useElem = (wxSVGUseElement*) XmlFindNode(root, wxT("use"));
			AddUse(m_id, useElem->GetX().GetBaseVal(), useElem->GetY().GetBaseVal(), useElem->GetWidth().GetBaseVal(),
					useElem->GetHeight().GetBaseVal(), &useElem->GetTransform().GetBaseVal());
		}
	}

	if (!Init(fileName, x, y))
		return false;
	
	m_keepAspectRatio = node->GetPropVal(wxT("keepAspectRatio"), &val) && (val == wxT("true") || val == wxT("1"));
	m_displayVideoFrame = !node->GetPropVal(wxT("displayVideoFrame"), &val) || (val != wxT("false") && val != wxT("0"));
	m_customVideoFrame = node->GetPropVal(wxT("customVideoFrame"), &val) && (val == wxT("true") || val == wxT("1"));
	if (node->GetPropVal(wxT("vobId"), &val) && val.ToLong(&lval))
		m_displayVobId = lval;
	
	// read changeable parameters
	wxSvgXmlNode* child = node->GetChildren();
	while (child) {
		if (child->GetName() == wxT("parameter")) {
			wxString name;
			wxString normal;
			wxString selected;
			wxString highlighted;
			if (child->GetPropVal(wxT("name"), &name)
					&& child->GetPropVal(wxT("normal"), &normal)
					&& child->GetPropVal(wxT("highlighted"), &highlighted)
					&& child->GetPropVal(wxT("selected"), &selected)) {
				wxCSSStyleDeclaration style;
				style.SetProperty(wxT("fill"), normal);
				SetParamColour(name, style.GetFill().GetRGBColor(), mbsNORMAL);
				style.SetProperty(wxT("fill"), highlighted);
				SetParamColour(name, style.GetFill().GetRGBColor(), mbsHIGHLIGHTED);
				style.SetProperty(wxT("fill"), selected);
				SetParamColour(name, style.GetFill().GetRGBColor(), mbsSELECTED);
			}
		}
		child = child->GetNext();
	}
	
	return true;
}
