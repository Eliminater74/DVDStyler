/////////////////////////////////////////////////////////////////////////////
// Name:        MenuCellsDlg.cpp
// Purpose:     The menu cells dialog
// Author:      Alex Thuering
// Created:     10.01.2012
// RCS-ID:      $Id: MenuCellsDlg.cpp,v 1.7 2016/01/06 21:10:04 ntalex Exp $
// Copyright:   (c) Alex Thuering
// Licence:     GPL
/////////////////////////////////////////////////////////////////////////////
#include "MenuCellsDlg.h"
#include "MenuPropDlg.h"
#include "Utils.h"
#include <wxVillaLib/utils.h>
#include <algorithm>
#include "rc/add.png.h"
#include "rc/remove.png.h"

//(*InternalHeaders(MenuCellsDlg)
#include <wx/intl.h>
#include <wx/button.h>
#include <wx/string.h>
//*)

class CellsTable: public wxGridTableBase {
public:
	CellsTable(vector<Cell*>& cells): wxGridTableBase(), m_cells(cells) { }

	int GetNumberRows() {
		return m_cells.size();
	}

	int GetNumberCols() {
		return 5;
	}

	bool IsEmptyCell(int row, int col) {
		return false;
	}

	wxString GetValue(int row, int col) {
		Cell* cell = m_cells[row];
		switch (col) {
		case 0:
			return cell->IsChapter() ? _("Chapter") : cell->IsProgram() ? _("Program") : _("Regular");
		case 1:
			return cell->GetStartStr();
		case 2:
			return cell->GetEndStr();
		case 3:
			return wxString::Format(wxT("%d"), cell->GetPause());
		case 4:
			return cell->GetCommands();
		default:
			break;
		}
		return wxT("");
	}

	void SetValue(int row, int col, const wxString& value) {
		Cell* cell = m_cells[row];
		long lval;
		switch (col) {
		case 0:
			cell->SetChapter(value == _("Chapter"));
			cell->SetProgram(value == _("Program"));
			break;
		case 1:
			cell->SetStart(value);
			break;
		case 2:
			cell->SetEnd(value);
			break;
		case 3:
			if (value.ToLong(&lval))
				cell->SetPause((int) lval);
			break;
		case 4:
			cell->SetCommands(value);
			break;
		default:
			break;
		}
	}

	bool AppendRows(size_t numRows = 1) {
		m_cells.push_back(new Cell(0, false));
		if (GetView()) {
			wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_APPENDED, numRows);
			GetView()->ProcessTableMessage(msg);
		}
		return true;
	}

	bool DeleteRows(size_t pos = 0, size_t numRows = 1) {
		m_cells.erase(m_cells.begin() + pos);
		if (GetView()) {
			wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, pos, 1);
			GetView()->ProcessTableMessage(msg);
		}
		return true;
	}

	wxString GetRowLabelValue( int row ) {
		return wxT("");
	}

	wxString GetColLabelValue( int col ) {
		wxString cellCols[] = { _("Type"), _("Start"), _("End"), _("Pause"), _("Commands") };
		return cellCols[col];
	}

private:
	vector<Cell*>& m_cells;
};

//(*IdInit(MenuCellsDlg)
const long MenuCellsDlg::ID_STATICTEXT1 = wxNewId();
const long MenuCellsDlg::ID_GRID = wxNewId();
const long MenuCellsDlg::ID_ADD_BT = wxNewId();
const long MenuCellsDlg::ID_DEL_BT = wxNewId();
//*)

BEGIN_EVENT_TABLE(MenuCellsDlg ,wxDialog)
	//(*EventTable(MenuCellsDlg)
	//*)
	EVT_BUTTON(wxID_OK, MenuCellsDlg::OnOkBt)
END_EVENT_TABLE()

MenuCellsDlg::MenuCellsDlg(MenuPropDlg* parent, DVD* dvd, int tsi, int pgci, vector<Cell*>& cells):
		parentDlg(parent), m_dvd(dvd), m_tsi(tsi), m_pgci(pgci) {
	VECTOR_COPY(cells, m_cells, Cell)

	//(*Initialize(MenuCellsDlg)
	wxBoxSizer* boxSizer3;
	wxBoxSizer* boxSizer2;
	wxStdDialogButtonSizer* stdDialogButtonSizer;
	wxStaticText* staticText1;
	wxBoxSizer* mainSizer;

	Create(parent, wxID_ANY, _("Menu cells"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	mainSizer = new wxBoxSizer(wxVERTICAL);
	staticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Cells:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	mainSizer->Add(staticText1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	boxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	m_grid = new wxGrid(this, ID_GRID, wxDefaultPosition, wxSize(520,300), 0, _T("ID_GRID"));
	boxSizer2->Add(m_grid, 1, wxRIGHT|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 6);
	boxSizer3 = new wxBoxSizer(wxVERTICAL);
	m_addBt = new wxBitmapButton(this, ID_ADD_BT, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_ADD_BT"));
	m_addBt->SetDefault();
	boxSizer3->Add(m_addBt, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_delBt = new wxBitmapButton(this, ID_DEL_BT, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_DEL_BT"));
	m_delBt->SetDefault();
	boxSizer3->Add(m_delBt, 0, wxTOP|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 2);
	boxSizer2->Add(boxSizer3, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL, 4);
	mainSizer->Add(boxSizer2, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	stdDialogButtonSizer = new wxStdDialogButtonSizer();
	stdDialogButtonSizer->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	stdDialogButtonSizer->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	stdDialogButtonSizer->Realize();
	mainSizer->Add(stdDialogButtonSizer, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);
	Center();

	Connect(ID_ADD_BT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MenuCellsDlg::OnAddBt);
	Connect(ID_DEL_BT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&MenuCellsDlg::OnDelBtClick);
	//*)
	stdDialogButtonSizer->GetAffirmativeButton()->SetDefault();
	
	CellsTable* table = new CellsTable(m_cells);
	m_grid->SetTable(table);
	m_grid->SetRowLabelSize(10);
	m_grid->SetColSize(4, 190);
	m_grid->SetColLabelSize(m_grid->GetCharHeight() + 8);
	m_grid->SetDefaultRowSize(m_grid->GetCharHeight() + 8);
	m_grid->SetSelectionMode(wxGrid::wxGridSelectRows);
	for (int i = 0; i < m_grid->GetNumberRows(); i++)
		SetCellEditors(i);
	
	m_addBt->SetBitmapLabel(wxBITMAP_FROM_MEMORY(add));
	m_delBt->SetBitmapLabel(wxBITMAP_FROM_MEMORY(remove));
	
	m_addBt->SetFocus();
	mainSizer->Fit(this);
}

MenuCellsDlg::~MenuCellsDlg() {
	//(*Destroy(MenuCellsDlg)
	//*)
	VECTOR_CLEAR(m_cells, Cell)
}

void MenuCellsDlg::SetCellEditors(int row) {
	const wxString cellTypes[] = { _("Regular"), _("Program"), _("Chapter") };
	m_grid->SetCellEditor(row, 0, new wxGridCellChoiceEditor(WXSIZEOF(cellTypes), cellTypes));
	m_grid->SetCellEditor(row, 3, new wxGridCellNumberEditor(-1, 255));
	m_grid->SetCellEditor(row, 4, new wxGridCellChoiceEditor(parentDlg->GetCommandList(), true));
}

void MenuCellsDlg::OnAddBt(wxCommandEvent& event) {
	m_grid->AppendRows(1);
	SetCellEditors(m_grid->GetNumberRows() - 1);
	m_grid->SelectRow(m_grid->GetGridCursorRow(), false);
	m_grid->SetFocus();
	m_delBt->Enable(true);
}

void MenuCellsDlg::OnDelBtClick(wxCommandEvent& event) {
	if (!m_grid->GetSelectedRows().IsEmpty())
		while (m_grid->GetSelectedRows().size() > 0)
			m_grid->DeleteRows(m_grid->GetSelectedRows()[0]);
	else if (m_grid->GetGridCursorRow() >= 0)
		m_grid->DeleteRows(m_grid->GetGridCursorRow());
	if (m_grid->GetNumberRows() > 0)
		m_grid->SelectRow(m_grid->GetGridCursorRow(), false);
	m_grid->SetFocus();
	m_delBt->Enable(m_grid->GetNumberRows() > 0);
}

bool cellCompare(Cell* c1, Cell* c2) { return c1->GetStart() < c2->GetStart(); }

void MenuCellsDlg::OnOkBt(wxCommandEvent& event) {
	// check commands
	for (unsigned int i = 0; i < m_cells.size(); i++) {
		DVDAction action;
		action.SetCustom(m_cells[i]->GetCommands());
		if (m_cells[i]->GetCommands().length() && !action.IsValid(m_dvd, m_tsi, m_pgci, true, wxT(""), true, false)) {
			return;
		}
	}
	// sort cells
	std::sort(m_cells.begin(), m_cells.end(), cellCompare);
	m_grid->Refresh();
	// check overlapping cells
	for (unsigned int i = 0; i < m_cells.size(); i++) {
		if (i > 0 && m_cells[i]->GetStart() == -1) {
			wxLogError(_("The cells %d and %d are overlapping"), i, i + 1);
			return;
		} else if (m_cells[i]->GetEnd() >= 0 && i + 1 < m_cells.size()
				&& m_cells[i]->GetEnd() > m_cells[i+1]->GetStart()) {
			wxLogError(_("The cells %d and %d are overlapping"), i + 1, i + 2);
			return;
		}
	}
	this->EndModal(wxID_OK);
}
