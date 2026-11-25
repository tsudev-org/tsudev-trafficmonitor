#include "stdafx.h"
#include "StaticEx.h"


CStaticEx::CStaticEx()
{
}


CStaticEx::~CStaticEx()
{
}

void CStaticEx::SetWindowTextEx(LPCTSTR lpszString, Alignment align)
{
	m_text = lpszString;
	m_align = align;
	m_color_text = true;
	Invalidate();
}

void CStaticEx::SetTextColor(COLORREF textColor)
{
	m_text_color = textColor;
	Invalidate();
}

void CStaticEx::SetBackColor(COLORREF back_color)
{
	m_back_color = back_color;
	m_draw_background_color = true;
}

CString CStaticEx::GetString() const
{
	return m_text;
}


LRESULT CStaticEx::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
	if (message == WM_SETTEXT)
	{
		CRect rect;
		CDC* pDC = GetDC();
		GetClientRect(rect);
		DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);
		ReleaseDC(pDC);
	}
	return CStatic::DefWindowProc(message, wParam, lParam);
}
BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()



void CStaticEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Thêm mã xử lý thông điệp tại đây
					   // Không gọi CStatic::OnPaint() cho thông điệp vẽ
	dc.SetTextColor(m_text_color);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(this->GetFont());
	CRect rect;
	this->GetClientRect(&rect);
	if (m_draw_background_color)
		dc.FillSolidRect(rect, m_back_color);
	else
		DrawThemeParentBackground(m_hWnd, dc.GetSafeHdc(), &rect);	//Vẽ lại vùng điều khiển để khắc phục chữ bị chồng
	CSize text_size = dc.GetTextExtent(m_text);
	UINT format{ DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX };		//Định dạng văn bản của hàm CDC::DrawText()
	if (text_size.cx > rect.Width())		//Nếu chiều rộng văn bản vượt quá chiều rộng vùng hình chữ nhật thì khi đặt căn giữa sẽ chuyển sang căn trái
	{
		if (m_align == Alignment::RIGHT)
			format |= DT_RIGHT;
	}
	else
	{
		switch (m_align)
		{
		case Alignment::RIGHT: format |= DT_RIGHT; break;
		case Alignment::CENTER: format |= DT_CENTER; break;
		}
	}
	dc.DrawText(m_text, rect, format);
}


void CStaticEx::PreSubclassWindow()
{
	// TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
	GetWindowText(m_text);

	CStatic::PreSubclassWindow();
}
