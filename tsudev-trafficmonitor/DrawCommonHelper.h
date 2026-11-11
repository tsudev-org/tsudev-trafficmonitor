#pragma once
#include "IDrawCommon.h"

//Lớp dùng để vẽ với bộ đệm kép
class CDrawDoubleBuffer final : public IDrawBuffer
{
public:
    CDrawDoubleBuffer(CDC* pDC, CRect rect)
        : m_pDC(pDC), m_rect(rect)
    {
        if (m_pDC != nullptr)
        {
            m_memDC.CreateCompatibleDC(NULL);
            m_memBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
            m_pOldBit = m_memDC.SelectObject(&m_memBitmap);
        }
    }

    ~CDrawDoubleBuffer()
    {
        if (m_pDC != nullptr)
        {
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), &m_memDC, 0, 0, SRCCOPY);
            m_memDC.SelectObject(m_pOldBit);
            m_memBitmap.DeleteObject();
            m_memDC.DeleteDC();
        }
    }

    CDC* GetMemDC()
    {
        return &m_memDC;
    }

private:
    CDC* m_pDC{};
    CDC m_memDC;
    CBitmap m_memBitmap;
    CBitmap* m_pOldBit{};
    CRect m_rect;
};

namespace DrawCommonHelper
{
    UINT ProccessTextFormat(CRect rect, CSize text_length, IDrawCommon::Alignment align, bool multi_line) noexcept;

    //Tính vị trí thực tế để vẽ ảnh theo chế độ co giãn ảnh
    //image_size[int]: kích thước gốc của ảnh
    //start_point[int][out]: vị trí bắt đầu của vùng vẽ
    //size[int][out]: kích thước vùng vẽ
    //stretch_mode[int]: chế độ co giãn
    void ImageDrawAreaConvert(CSize image_size, CPoint& start_point, CSize& size, IDrawCommon::StretchMode stretch_mode);

    struct Point
    {
    public:
        Point() {}
        Point(int x, int y)
            : m_x(x), m_y(y)
        {}
        bool operator==(const Point& a) const
        {
            return m_x == a.m_x && m_y == a.m_y;
        }
        bool operator<(const Point& a) const
        {
            if (m_x == a.m_x)
                return m_y < a.m_y;
            else
                return m_x < a.m_x;
        }

    private:
        int m_x{};
        int m_y{};
    };

    //Lấy các điểm hoàn toàn trong suốt trong một vùng và lưu vào points
    void GetBitmapAlphaPixel(HBITMAP hBitmap, std::set<Point>& points);

    //Sửa kênh Alpha của phần văn bản trong bitmap
    //Sau khi dùng UpdateLayeredWindow, văn bản vẽ bằng GDI cũng bị trong suốt; hàm này duyệt các điểm ảnh trong bitmap có alpha bằng 0 nhưng không nằm trong alpha_points và sửa lại thành giá trị alpha đúng
    void FixBitmapTextAlpha(HBITMAP hBitmap, BYTE alpha, std::set<Point> alpha_points);
};

