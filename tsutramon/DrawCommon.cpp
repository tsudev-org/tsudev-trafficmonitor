#include "stdafx.h"
#include "DrawCommon.h"
#include "DrawCommonHelper.h"

CDrawCommon::CDrawCommon()
{
}

CDrawCommon::~CDrawCommon()
{
}

void CDrawCommon::Create(CDC* pDC, CWnd* pMainWnd)
{
    m_pDC = pDC;
    m_pMainWnd = pMainWnd;
    if (pMainWnd != nullptr)
        m_pfont = m_pMainWnd->GetFont();
    m_gdi_plus_drawer.Create(pDC);
}

void CDrawCommon::SetFont(CFont* pfont)
{
    m_pfont = pfont;
    m_pDC->SelectObject(m_pfont);
}

void CDrawCommon::SetDC(CDC* pDC)
{
    m_pDC = pDC;
    m_gdi_plus_drawer.Create(pDC);
}

void CDrawCommon::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align, bool draw_back_ground, bool multi_line, BYTE alpha)
{
    m_pDC->SetTextColor(color);
    if (!draw_back_ground)
        m_pDC->SetBkMode(TRANSPARENT);
    m_pDC->SelectObject(m_pfont);
    CSize text_size = m_pDC->GetTextExtent(lpszString);

    auto format = DrawCommonHelper::ProccessTextFormat(rect, text_size, align, multi_line);

    if (draw_back_ground)
        m_pDC->FillSolidRect(rect, m_back_color);
    m_pDC->DrawText(lpszString, rect, format);
}

void CDrawCommon::SetDrawRect(CRect rect)
{
    CRgn rgn;
    rgn.CreateRectRgnIndirect(rect);
    m_pDC->SelectClipRgn(&rgn);
}

void CDrawCommon::SetDrawRect(CDC* pDC, CRect rect)
{
    CRgn rgn;
    rgn.CreateRectRgnIndirect(rect);
    pDC->SelectClipRgn(&rgn);
}

void CDrawCommon::DrawBitmap(CBitmap& bitmap, CPoint start_point, CSize size, StretchMode stretch_mode)
{
    CDC memDC;

    //Lấy kích thước thực tế của ảnh
    BITMAP bm;
    GetObject(bitmap, sizeof(BITMAP), &bm);

    memDC.CreateCompatibleDC(m_pDC);
    memDC.SelectObject(&bitmap);
    // Hai dòng dưới đây tránh ảnh bị méo
    m_pDC->SetStretchBltMode(HALFTONE);
    m_pDC->SetBrushOrg(0, 0);

    DrawCommonHelper::ImageDrawAreaConvert(CSize(bm.bmWidth, bm.bmHeight), start_point, size, stretch_mode);

    m_pDC->StretchBlt(start_point.x, start_point.y, size.cx, size.cy, &memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    memDC.DeleteDC();
}

void CDrawCommon::DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode)
{
    CBitmap bitmap;
    bitmap.LoadBitmap(bitmap_id);
    DrawBitmap(bitmap, start_point, size, stretch_mode);
}

void CDrawCommon::DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode, BYTE)
{
    CBitmap bitmap;
    if (!bitmap.Attach(hbitmap))
        return;
    DrawBitmap(bitmap, start_point, size, stretch_mode);
    bitmap.Detach();
}

void CDrawCommon::DrawIcon(HICON hIcon, CPoint start_point, CSize size)
{
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    if (size.cx == 0 || size.cy == 0)
        ::DrawIconEx(m_pDC->GetSafeHdc(), start_point.x, start_point.y, hIcon, 0, 0, 0, NULL, DI_NORMAL | DI_DEFAULTSIZE);
    else
        ::DrawIconEx(m_pDC->GetSafeHdc(), start_point.x, start_point.y, hIcon, size.cx, size.cy, 0, NULL, DI_NORMAL);
}

void CDrawCommon::BitmapStretch(CImage* pImage, CImage* ResultImage, CSize size)
{
    if (pImage->IsDIBSection())
    {
        // Lấy DC của pImage
        CDC* pImageDC1 = CDC::FromHandle(pImage->GetDC()); // Vì Image có DC riêng nên phải dùng FromHandle để lấy DC tương ứng

        CBitmap* bitmap1 = pImageDC1->GetCurrentBitmap();
        BITMAP bmpInfo;
        bitmap1->GetBitmap(&bmpInfo);

        // Tạo CImage mới
        ResultImage->Create(size.cx, size.cy, bmpInfo.bmBitsPixel);
        CDC* ResultImageDC = CDC::FromHandle(ResultImage->GetDC());

        // Khi Destination nhỏ hơn, chế độ Stretch Blt của DC đích sẽ quyết định có giữ thông tin của các điểm bị loại bỏ hay không
        ResultImageDC->SetStretchBltMode(HALFTONE);        // Dùng chất lượng cao nhất
        ::SetBrushOrgEx(ResultImageDC->m_hDC, 0, 0, NULL); // Điều chỉnh điểm bắt đầu của Brush

        // Vẽ pImage lên ResultImage
        StretchBlt(*ResultImageDC, 0, 0, size.cx, size.cy, *pImageDC1, 0, 0, pImage->GetWidth(), pImage->GetHeight(), SRCCOPY);
        // pImage->Draw(*ResultImageDC,0,0,StretchWidth,StretchHeight,0,0,pImage->GetWidth(),pImage->GetHeight());

        pImage->ReleaseDC();
        ResultImage->ReleaseDC();
    }
}

void CDrawCommon::FillRect(CRect rect, COLORREF color, BYTE alpha)
{
    m_pDC->FillSolidRect(rect, color);
}

void CDrawCommon::FillRectWithBackColor(CRect rect)
{
    m_pDC->FillSolidRect(rect, m_back_color);
}

void CDrawCommon::DrawRectOutLine(CRect rect, COLORREF color, int width, bool dot_line, BYTE alpha, int radius)
{
    if (radius > 0)
    {
        //Khi bán kính lớn hơn 0 thì vẽ hình chữ nhật bo góc bằng GDI+
        m_gdi_plus_drawer.DrawRectOutLine(rect, color, width, dot_line, alpha, radius);
    }
    else
    {
        CPen aPen, * pOldPen;
        aPen.CreatePen((dot_line ? PS_DOT : PS_SOLID), width, color);
        pOldPen = m_pDC->SelectObject(&aPen);
        CBrush* pOldBrush{ dynamic_cast<CBrush*>(m_pDC->SelectStockObject(NULL_BRUSH)) };

        rect.DeflateRect(width / 2, width / 2);
        m_pDC->Rectangle(rect);
        m_pDC->SelectObject(pOldPen);
        m_pDC->SelectObject(pOldBrush); // Restore the old brush
        aPen.DeleteObject();
    }
}

void CDrawCommon::GetRegionFromImage(CRgn& rgn, CBitmap& cBitmap, int threshold)
{
    CDC memDC;

    memDC.CreateCompatibleDC(NULL);
    CBitmap* pOldMemBmp = NULL;
    pOldMemBmp = memDC.SelectObject(&cBitmap);

    //Tạo vùng tổng của cửa sổ, region ban đầu là 0
    rgn.CreateRectRgn(0, 0, 0, 0);

    BITMAP bit;
    cBitmap.GetBitmap(&bit); //Lấy thông số bitmap, ở đây cần dùng chiều dài và chiều rộng của bitmap
    int y;
    for (y = 0; y < bit.bmHeight; y++)
    {
        CRgn rgnTemp; //Lưu region tạm thời
        int iX = 0;
        do
        {
            //Bỏ qua màu trong suốt để tìm điểm không trong suốt tiếp theo.
            while (iX < bit.bmWidth && GetColorBritness(memDC.GetPixel(iX, y)) <= threshold)
                iX++;
            int iLeftX = iX; //Ghi nhớ điểm bắt đầu này

            //Tìm điểm có màu trong suốt tiếp theo
            while (iX < bit.bmWidth && GetColorBritness(memDC.GetPixel(iX, y)) > threshold)
                ++iX;

            //Tạo một “region” tạm cao 1 pixel bao giữa điểm đầu và điểm cuối
            rgnTemp.CreateRectRgn(iLeftX, y, iX, y + 1);
            rgn.CombineRgn(&rgn, &rgnTemp, RGN_OR);

            //Xóa "region" tạm, nếu không lần tạo sau sẽ bị lỗi
            rgnTemp.DeleteObject();
        } while (iX < bit.bmWidth);
    }
    memDC.DeleteDC();
}

int CDrawCommon::GetColorBritness(COLORREF color)
{
    return (GetRValue(color) + GetGValue(color) + GetBValue(color)) / 3;
}

void CDrawCommon::DrawLine(CPoint start_point, CPoint end_point, COLORREF color, BYTE alpha)
{
    CPen aPen, *pOldPen;
    aPen.CreatePen(PS_SOLID, 1, color);
    pOldPen = m_pDC->SelectObject(&aPen);
    CBrush* pOldBrush{dynamic_cast<CBrush*>(m_pDC->SelectStockObject(NULL_BRUSH))};

    m_pDC->MoveTo(start_point);
    m_pDC->LineTo(end_point);
    m_pDC->SelectObject(pOldPen);
    m_pDC->SelectObject(pOldBrush); // Restore the old brush
    aPen.DeleteObject();
}

int CDrawCommon::GetTextWidth(LPCTSTR lpszString)
{
    return m_pDC->GetTextExtent(lpszString).cx;
}

void CDrawCommon::GetTextExtent(const wchar_t* lpszString, int& w, int& h)
{
    CSize size = m_pDC->GetTextExtent(lpszString);
    w = size.cx;
    h = size.cy;
}
