// CMFCColorDialogEx.cpp: tệp triển khai
//

#include "stdafx.h"
#include "CMFCColorDialogEx.h"
#include "Common.h"
#include "TsudevTrafficMonitor.h"

// CMFCColorDialogEx

//IMPLEMENT_DYNAMIC(CMFCColorDialogEx, CMFCColorDialog)

CMFCColorDialogEx::CMFCColorDialogEx(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd, HPALETTE hPal)
	:CMFCColorDialog(clrInit, dwFlags, pParentWnd, hPal)
{

}

CMFCColorDialogEx::~CMFCColorDialogEx()
{
}


BEGIN_MESSAGE_MAP(CMFCColorDialogEx, CMFCColorDialog)
END_MESSAGE_MAP()



// Trình xử lý thông điệp của CMFCColorDialogEx

BOOL CMFCColorDialogEx::OnInitDialog()
{
	CMFCColorDialog::OnInitDialog();

	// TODO:  Thêm khởi tạo bổ sung tại đây

	//Khởi tạo phông chữ
	CRect rect;
	GetWindowRect(rect);
	FontInfo font_info;
	font_info.name = theApp.m_str_table.GetLanguageInfo().default_font_name.c_str();
	font_info.size = 9;
	UINT dpi_x{}, dpi_y{};
	int dpi = theApp.GetDpi();
	if (theApp.DPIFromRect(rect, &dpi_x, &dpi_y))
		dpi = dpi_x;
	font_info.Create(m_dlg_font, dpi);

	CCommon::SetDialogFont(this, &m_dlg_font);

	CWnd* pPropSheet = m_pColourSheetOne->GetParent();
	if (pPropSheet != nullptr)
		CCommon::SetDialogFont(pPropSheet, &m_dlg_font);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // Ngoại lệ: trang thuộc tính OCX phải trả về FALSE
}

