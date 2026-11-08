//Lớp vẽ đóng gói
#pragma once
#include "IDrawCommon.h"
#include "D2D1Support.h"
#include "CommonData.h"
#include "Nullable.hpp"
#include "DrawCommonEx.h"

class CDrawCommon final : public IDrawCommon
{
public:

    CDrawCommon();
    ~CDrawCommon();

    void Create(CDC* pDC, CWnd* pMainWnd);
    void SetFont(CFont* pfont) override;		//Thiết lập phông chữ để vẽ văn bản
    void SetDC(CDC* pDC);		//Thiết lập DC để vẽ
    virtual CDC* GetDC() override { return m_pDC; }
    void SetBackColor(COLORREF back_color, BYTE alpha = 255) override { m_back_color = back_color; }
    void SetTextColor(const COLORREF text_color, BYTE alpha = 255) override
    {
        m_pDC->SetTextColor(text_color);
    }

    void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false, BYTE alpha = 255) override; //Vẽ văn bản trong vùng hình chữ nhật chỉ định

    void SetDrawRect(CRect rect) override;		//Thiết lập vùng cắt (clip) khi vẽ
    static void SetDrawRect(CDC* pDC, CRect rect);

    //Vẽ một bitmap
    void DrawBitmap(CBitmap& bitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
    void DrawBitmap(UINT bitmap_id, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH);
    void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH, BYTE alpha = 255) override;

    void DrawIcon(HICON hIcon, CPoint start_point, CSize size) override;

    //Co giãn ảnh về kích thước chỉ định
    static void BitmapStretch(CImage* pImage, CImage* ResultImage, CSize size);

    void FillRect(CRect rect, COLORREF color, BYTE alpha = 255) override; //Tô hình chữ nhật bằng màu đơn sắc
    void FillRectWithBackColor(CRect rect);			//Tô hình chữ nhật bằng màu nền
    void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false, BYTE alpha = 255, int radius = 0) override; //Vẽ viền hình chữ nhật. Nếu dot_line là true thì dùng nét đứt

    //Tạo region từ ảnh; nếu độ sáng của điểm ảnh nhỏ hơn threshold (giá trị 0~255, 0 là đen, 255 là trắng) thì điểm ảnh đó nằm ngoài region
    static void GetRegionFromImage(CRgn& rgn, CBitmap& cBitmap, int threshold);

    void DrawLine(CPoint start_point, CPoint end_point, COLORREF color, BYTE alpha = 255) override; //Vẽ đường thẳng bằng bút vẽ hiện tại

    virtual int GetTextWidth(LPCTSTR lpszString) override;

    virtual void GetTextExtent(const wchar_t* lpszString, int& w, int& h) override;

private:
    CDC* m_pDC{};		//Con trỏ tới lớp CDC dùng để vẽ
    CWnd* m_pMainWnd{};	//Handle của cửa sổ vẽ
    CFont* m_pfont{};
    COLORREF m_back_color{};
    CDrawCommonEx m_gdi_plus_drawer;

    static int GetColorBritness(COLORREF color);
};

