#include "stdafx.h"
#include "DrawCommonEx.h"
#include "DrawCommonHelper.h"

CDrawCommonEx::CDrawCommonEx(CDC* pDC)
{
    Create(pDC);
}

CDrawCommonEx::CDrawCommonEx()
{
}


CDrawCommonEx::~CDrawCommonEx()
{
    SAFE_DELETE(m_pGraphics);
}

void CDrawCommonEx::Create(CDC* pDC)
{
    ASSERT(pDC != nullptr);
    m_pDC = pDC;
    SAFE_DELETE(m_pGraphics);
    m_pGraphics = new Gdiplus::Graphics(pDC->GetSafeHdc());
}

void CDrawCommonEx::SetFont(CFont * pFont)
{
    //Gán phông chữ vào CDC, khi vẽ sẽ tạo phông GDI+ từ CDC
    m_pDC->SelectObject(pFont);
}

void CDrawCommonEx::DrawImage(Gdiplus::Image* pImage, CPoint start_point, CSize size, StretchMode stretch_mode)
{
    m_pGraphics->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);
    DrawCommonHelper::ImageDrawAreaConvert(CSize(pImage->GetWidth(), pImage->GetHeight()), start_point, size, stretch_mode);
    m_pGraphics->DrawImage(pImage, INT(start_point.x), INT(start_point.y), INT(size.cx), INT(size.cy));
}

void CDrawCommonEx::SetBackColor(COLORREF back_color, BYTE alpha)
{
    m_back_color = CGdiPlusHelper::COLORREFToGdiplusColor(back_color, alpha);
}

void CDrawCommonEx::DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align, bool draw_back_ground, bool multi_line, BYTE alpha)
{
    //Vùng hình chữ nhật
    Gdiplus::RectF rect_gdiplus = CGdiPlusHelper::CRectToGdiplusRect(rect);

    //Vẽ nền
    if (draw_back_ground)
    {
        Gdiplus::SolidBrush brush(m_back_color);
        m_pGraphics->FillRectangle(&brush, rect_gdiplus);
    }
    //Thiết lập phông chữ
    Gdiplus::Font font(m_pDC->GetSafeHdc());
    //Thiết lập màu chữ
    Gdiplus::SolidBrush brush(CGdiPlusHelper::COLORREFToGdiplusColor(color, alpha));
    //Thiết lập kiểu căn chỉnh
    Gdiplus::StringFormat format;
    Gdiplus::StringAlignment alignment = Gdiplus::StringAlignmentNear;
    if (align == Alignment::CENTER)
        alignment = Gdiplus::StringAlignmentCenter;
    else if (align == Alignment::RIGHT)
        alignment = Gdiplus::StringAlignmentFar;
    format.SetAlignment(alignment);    //Kiểu căn chỉnh ngang
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);    //Kiểu căn chỉnh dọc
    UINT flags = Gdiplus::StringFormatFlagsNoFitBlackBox;
    if (!multi_line)
        flags |= Gdiplus::StringFormatFlagsNoWrap;      //Không tự động xuống dòng
    format.SetTrimming(Gdiplus::StringTrimmingNone);    //Cấm cắt bớt văn bản
    format.SetFormatFlags(flags);

    //Vẽ văn bản
    m_pGraphics->DrawString(lpszString, -1, &font, rect_gdiplus, &format, &brush);
}

void CDrawCommonEx::SetDrawRect(CRect rect)
{
    m_pGraphics->SetClip(CGdiPlusHelper::CRectToGdiplusRect(rect));
}

void CDrawCommonEx::FillRect(CRect rect, COLORREF color, BYTE alpha)
{
    Gdiplus::RectF rect_gdiplus = CGdiPlusHelper::CRectToGdiplusRect(rect);
    Gdiplus::SolidBrush brush(CGdiPlusHelper::COLORREFToGdiplusColor(color, alpha));
    m_pGraphics->FillRectangle(&brush, rect_gdiplus);
}

static void CreateRoundRectPath(Gdiplus::GraphicsPath* pPath, const Gdiplus::RectF& rect, float radius)
{
    // Giới hạn bán kính không vượt quá một nửa kích thước hình chữ nhật
    float half_w = rect.Width / 2.0f;
    float half_h = rect.Height / 2.0f;
    if (radius > half_w) radius = half_w;
    if (radius > half_h) radius = half_h;

    // Nếu bán kính bằng 0 hoặc âm thì thêm trực tiếp hình chữ nhật thường
    if (radius < 0.1f)
    {
        pPath->AddRectangle(rect);
        return;
    }

    float x = rect.X;
    float y = rect.Y;
    float width = rect.Width;
    float height = rect.Height;
    
    pPath->AddArc(x, y, radius * 2, radius * 2, 180, 90); // Góc trên bên trái
    pPath->AddArc(x + width - radius * 2, y, radius * 2, radius * 2, 270, 90); // Góc trên bên phải
    pPath->AddArc(x + width - radius * 2, y + height - radius * 2, radius * 2, radius * 2, 0, 90); // Góc dưới bên phải
    pPath->AddArc(x, y + height - radius * 2, radius * 2, radius * 2, 90, 90); // Góc dưới bên trái
    pPath->CloseFigure();

}

void CDrawCommonEx::DrawRectOutLine(CRect rect, COLORREF color, int width, bool dot_line, BYTE alpha, int radius)
{
    // Chuyển thành hình chữ nhật GDI+
    Gdiplus::RectF rect_gdiplus = CGdiPlusHelper::CRectToGdiplusRect(rect);

    // Tạo bút vẽ
    Gdiplus::Color gdi_color = CGdiPlusHelper::COLORREFToGdiplusColor(color, alpha);
    Gdiplus::Pen pen(gdi_color, (Gdiplus::REAL)width);

    // Thiết lập kiểu nét đứt
    if (dot_line)
    {
        // Dùng nét chấm gạch
        pen.SetDashStyle(Gdiplus::DashStyleDash);
    }

    // Vẽ
    auto gdiplus_states = m_pGraphics->Save();
    if (radius > 0)
    {
        // Vẽ hình chữ nhật bo góc bằng path
        Gdiplus::GraphicsPath path;
        rect_gdiplus.Width -= width;
        rect_gdiplus.Height -= width;
        CreateRoundRectPath(&path, rect_gdiplus, (float)radius);
        m_pGraphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias); // Bật khử răng cưa (anti-aliasing)
        m_pGraphics->DrawPath(&pen, &path);
    }
    else
    {
        // Hình chữ nhật thường
        m_pGraphics->DrawRectangle(&pen, rect_gdiplus);
    }
    m_pGraphics->Restore(gdiplus_states);
}

void CDrawCommonEx::DrawLine(CPoint start_point, CPoint end_point, COLORREF color, BYTE alpha)
{
    //Vẽ đường thẳng bằng GDI
    CPen aPen, * pOldPen;
    aPen.CreatePen(PS_SOLID, 1, color);
    pOldPen = m_pDC->SelectObject(&aPen);
    CBrush* pOldBrush{ dynamic_cast<CBrush*>(m_pDC->SelectStockObject(NULL_BRUSH)) };

    m_pDC->MoveTo(start_point);
    m_pDC->LineTo(end_point);
    m_pDC->SelectObject(pOldPen);
    m_pDC->SelectObject(pOldBrush);
    aPen.DeleteObject();
}

void CDrawCommonEx::SetTextColor(const COLORREF color, BYTE alpha)
{
    m_text_color = CGdiPlusHelper::COLORREFToGdiplusColor(color, alpha);
}

CDC* CDrawCommonEx::GetDC()
{
    return m_pDC;
}

int CDrawCommonEx::GetTextWidth(LPCTSTR lpszString)
{
    int w{}, h{};
    GetTextExtent(lpszString, w, h);
    return w;
}

void CDrawCommonEx::GetTextExtent(const wchar_t* lpszString, int& w, int& h)
{
    Gdiplus::Font font(m_pDC->GetSafeHdc());
    Gdiplus::RectF textSize;
    m_pGraphics->MeasureString(lpszString, -1, &font, Gdiplus::PointF(0, 0), &textSize);
    w = static_cast<int>(textSize.Width + 0.5f);
    h = static_cast<int>(textSize.Height + 0.5f);
}

void CDrawCommonEx::DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode, BYTE alpha)
{
    Gdiplus::Bitmap bm(hbitmap, NULL);
    m_pGraphics->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);
    DrawCommonHelper::ImageDrawAreaConvert(CSize(bm.GetWidth(), bm.GetHeight()), start_point, size, stretch_mode);
    m_pGraphics->DrawImage(&bm, INT(start_point.x), INT(start_point.y), INT(size.cx), INT(size.cy));
}

void CDrawCommonEx::DrawIcon(HICON hIcon, CPoint start_point, CSize size)
{
    //Vẽ biểu tượng trực tiếp bằng GDI
    if (m_pDC->GetSafeHdc() == NULL)
        return;
    if (size.cx == 0 || size.cy == 0)
        ::DrawIconEx(m_pDC->GetSafeHdc(), start_point.x, start_point.y, hIcon, 0, 0, 0, NULL, DI_NORMAL | DI_DEFAULTSIZE);
    else
        ::DrawIconEx(m_pDC->GetSafeHdc(), start_point.x, start_point.y, hIcon, size.cx, size.cy, 0, NULL, DI_NORMAL);
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
Gdiplus::Color CGdiPlusHelper::COLORREFToGdiplusColor(COLORREF color, BYTE alpha /*= 255*/)
{
    return Gdiplus::Color(alpha, GetRValue(color), GetGValue(color), GetBValue(color));
}

COLORREF CGdiPlusHelper::GdiplusColorToCOLORREF(Gdiplus::Color color)
{
    return RGB(color.GetR(), color.GetG(), color.GetB());
}

CRect CGdiPlusHelper::GdiplusRectToCRect(Gdiplus::RectF rect)
{
    return CRect(static_cast<int>(rect.GetLeft()), static_cast<int>(rect.GetTop()), static_cast<int>(rect.GetRight()), static_cast<int>(rect.GetBottom()));
}

Gdiplus::RectF CGdiPlusHelper::CRectToGdiplusRect(CRect rect)
{
    return Gdiplus::RectF(static_cast<Gdiplus::REAL>(rect.left), static_cast<Gdiplus::REAL>(rect.top), static_cast<Gdiplus::REAL>(rect.Width()), static_cast<Gdiplus::REAL>(rect.Height()));
}
