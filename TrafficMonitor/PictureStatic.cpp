#include "stdafx.h"
#include "PictureStatic.h"


CPictureStatic::CPictureStatic()
{
}


CPictureStatic::~CPictureStatic()
{
	m_memDC.DeleteDC();
	m_bitmap.Detach();
}

void CPictureStatic::SetPicture(UINT pic_id)
{
	m_memDC.DeleteDC();
	if (!m_bitmap.LoadBitmap(pic_id))		//Nạp bitmap
		return;
	//Lấy kích thước thực tế của ảnh
	GetObject(m_bitmap, sizeof(BITMAP), &m_bm);
	CDC* pDC = GetDC();
	m_memDC.CreateCompatibleDC(pDC);
	m_memDC.SelectObject(&m_bitmap);
	//Lấy kích thước điều khiển
	GetClientRect(m_rect);
	//Vẽ lại thủ công
	Invalidate();
}

void CPictureStatic::SetPicture(HBITMAP hBitmap)
{
	m_memDC.DeleteDC();
	m_bitmap.Detach();
	if (!m_bitmap.Attach(hBitmap))
		return;
	//Lấy kích thước thực tế của ảnh
	GetObject(m_bitmap, sizeof(BITMAP), &m_bm);
	CDC* pDC = GetDC();
	m_memDC.CreateCompatibleDC(pDC);
	m_memDC.SelectObject(&m_bitmap);
	//Lấy kích thước điều khiển
	GetClientRect(m_rect);
	//Vẽ lại thủ công
	Invalidate();
}

BEGIN_MESSAGE_MAP(CPictureStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CPictureStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Thêm mã xử lý thông điệp tại đây
					   // Không gọi CStatic::OnPaint() cho thông điệp vẽ
	if (m_bitmap.m_hObject != NULL)
	{
		// Hai dòng dưới đây tránh ảnh bị méo
		dc.SetStretchBltMode(HALFTONE);
		dc.SetBrushOrg(0, 0);
		//Vẽ ảnh trong DC bộ nhớ
		dc.StretchBlt(0, 0, m_rect.Width(), m_rect.Height(), &m_memDC, 0, 0, m_bm.bmWidth, m_bm.bmHeight, SRCCOPY);

		//Gửi thông điệp vẽ lại tới cửa sổ cha
		CWnd* pParent{ GetParent() };
		if (pParent != nullptr)
			pParent->SendMessage(WM_CONTROL_REPAINT, (WPARAM)this, LPARAM(&dc));
	}
}
