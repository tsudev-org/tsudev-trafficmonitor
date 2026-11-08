#pragma once
#include "CommonData.h"
#include "PluginInterface.h"

class IDrawBuffer
{
public:
    virtual ~IDrawBuffer() = default;
};

class IDrawCommon : public IPluginDrawer
{
public:

    virtual void SetBackColor(COLORREF back_color, BYTE alpha = 255) = 0;
    // Thiết lập phông chữ để vẽ văn bản
    virtual void SetFont(CFont* pfont) = 0;
    // Vẽ văn bản trong vùng hình chữ nhật chỉ định
    virtual void DrawWindowText(CRect rect, LPCTSTR lpszString, COLORREF color, Alignment align = Alignment::LEFT, bool draw_back_ground = false, bool multi_line = false, BYTE alpha = 255) = 0;
    // Thiết lập vùng cắt (clip) khi vẽ
    virtual void SetDrawRect(CRect rect) = 0;
    // Tô hình chữ nhật bằng màu đơn sắc
    virtual void FillRect(CRect rect, COLORREF color, BYTE alpha = 255) = 0;
    // Vẽ viền hình chữ nhật. Nếu dot_line là true thì dùng nét đứt. Khi radius lớn hơn 0 thì vẽ hình chữ nhật bo góc
    virtual void DrawRectOutLine(CRect rect, COLORREF color, int width = 1, bool dot_line = false, BYTE alpha = 255, int radius = 0) = 0;
    // Vẽ đường thẳng bằng bút vẽ hiện tại
    virtual void DrawLine(CPoint start_point, CPoint end_point, COLORREF color, BYTE alpha = 255) = 0;
    // Vẽ đường thẳng bằng bút vẽ hiện tại, bắt đầu từ start_point, vẽ lên trên đoạn có độ dài height
    virtual void DrawLine(CPoint start_point, int height, COLORREF color, BYTE alpha = 255);
    virtual void SetTextColor(const COLORREF color, BYTE alpha = 255) = 0;
    // Vẽ một bitmap
    virtual void DrawBitmap(HBITMAP hbitmap, CPoint start_point, CSize size, StretchMode stretch_mode = StretchMode::STRETCH, BYTE alpha = 255) = 0;
    virtual void DrawIcon(HICON hIcon, CPoint start_point, CSize size) = 0;
    virtual ~IDrawCommon() = default;

    //Lấy handle ngữ cảnh vẽ.
    virtual CDC* GetDC() = 0;
    //Lấy chiều rộng văn bản
    virtual int GetTextWidth(LPCTSTR lpszString) { return 0; }

    // Kế thừa qua IPluginDrawer
    virtual int GetAPIVersion() override;
    virtual void DrawWindowText(int x, int y, int w, int h, const wchar_t* lpszString, unsigned long color, Alignment align, bool multi_line, unsigned char alpha) override;
    virtual void SetDrawRect(int x, int y, int w, int h) override;
    virtual void FillRect(int x, int y, int w, int h, unsigned long color, unsigned char alpha) override;
    virtual void DrawRectOutLine(int x, int y, int w, int h, unsigned long color, int width, bool dot_line, unsigned char alpha, int radius) override;
    virtual void DrawBitmap(void* hbitmap, int x, int y, int w, int h, StretchMode stretch_mode, unsigned char alpha) override;
    virtual void DrawIcon(void* hIcon, int x, int y, int w, int h) override;
    virtual void DrawLine(int x1, int y1, int x2, int y2, unsigned long color, int width = 1, bool dot_line = false, unsigned char alpha = 255) override;
    virtual void* GetHDC() override;
};

namespace DrawCommonHelper
{
    /**
     * @brief Enum kiểu bộ kết xuất; khi thêm kiểu bộ kết xuất mới cần thêm vào enum này
     *
     */
    enum class RenderType
    {
        // Dùng GDI
        DEFAULT,
        // Nếu được hỗ trợ thì dùng D2D1
        D2D1,
        // Nếu hệ thống mới hơn Win8.1 thì khi dùng D2D1 sẽ hiển thị kết quả qua DirectComposition
        D2D1_WITH_DCOMPOSITION
    };

    constexpr BYTE GDI_NO_MODIFIED_FLAG = 0x01;
    constexpr BYTE OPAQUE_ALPHA_VALUE = 0xFF;
    constexpr BYTE TRANSPARENT_ALPHA_VALUE = 0x00;
    constexpr BYTE AVAILABLE_MINIMUM_ALPHA = 0x02;
    constexpr BYTE GDI_MODIFIED_FLAG = 0x00;
}
