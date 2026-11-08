// CLinkStatic.cpp: tệp triển khai

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "LinkStatic.h"


// CLinkStatic

IMPLEMENT_DYNAMIC(CLinkStatic, CStatic)

CLinkStatic::CLinkStatic()
{

}

CLinkStatic::~CLinkStatic()
{
}


void CLinkStatic::SetBackgroundColor(COLORREF background_color)
{
    m_back_color = background_color;
}

void CLinkStatic::SetURL(CString strURL)
{
	m_strURL = strURL;
}

CString CLinkStatic::GetURL() const
{
	return m_strURL;
}


BEGIN_MESSAGE_MAP(CLinkStatic, CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// Trình xử lý thông điệp của CLinkStatic

bool CLinkStatic::IsLinkValid() const
{
    return (!m_link_is_url || !m_strURL.IsEmpty());
}

void CLinkStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.dwHoverTime = 1;
	_TrackMouseEvent(&tme);
}


void CLinkStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
	if (m_link_is_url)
	{
		if (!m_strURL.IsEmpty())
			ShellExecute(NULL, _T("open"), m_strURL, NULL, NULL, SW_SHOW);	//Mở siêu liên kết
	}
	else
	{
		CWnd* pParent{ GetParent() };
		if (pParent != nullptr)
			pParent->SendMessage(WM_LINK_CLICKED, (WPARAM)this);
	}
}


void CLinkStatic::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
	if (!m_bHot)
	{
		m_bHot = true;
		Invalidate();
	}
	else
	{
		CStatic::OnMouseHover(nFlags, point);
	}
}


void CLinkStatic::OnMouseLeave()
{
	// TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
	m_bHot = false;
	Invalidate();
}


BOOL CLinkStatic::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Thêm mã xử lý thông điệp và/hoặc gọi giá trị mặc định tại đây
	if (IsLinkValid() && m_bHot)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		return TRUE;
	}

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}



void CLinkStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Thêm mã xử lý thông điệp tại đây
					   // Không gọi CStatic::OnPaint() cho thông điệp vẽ
	//Công việc chuẩn bị
    if (m_hover_font.GetSafeHandle() == NULL)
    {
        LOGFONT lf;
        GetFont()->GetLogFont(&lf);
        lf.lfUnderline = true;
        m_hover_font.CreateFontIndirect(&lf);
    }

    if (IsLinkValid())
    {
        if (m_bHot)
            dc.SelectObject(&m_hover_font);
        else
            dc.SelectObject(GetFont());
        dc.SetTextColor(GetSysColor(COLOR_HOTLIGHT));
    }
    else
    {
        dc.SelectObject(GetFont());
    }
	dc.SetBkMode(TRANSPARENT);
	CRect rect;
	this->GetClientRect(&rect);
	//Vẽ nền
	dc.FillSolidRect(rect, m_back_color);
	///Xuất văn bản
    CString text;
    GetWindowText(text);
	if (text.GetLength()>0)
	{
		dc.DrawText(text, rect, DT_VCENTER | DT_SINGLELINE);
	}
}


void CLinkStatic::PreSubclassWindow()
{
	// TODO: Thêm mã chuyên biệt và/hoặc gọi lớp cơ sở tại đây
	DWORD dwStyle = GetStyle();
	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);

	CStatic::PreSubclassWindow();
}
