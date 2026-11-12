#include "stdafx.h"
#include "TsudevTrafficMonitor.h"
#include "EdgeHandleWnd.h"
#include <gdiplus.h>

namespace
{
    constexpr UINT_PTR ANIM_TIMER_ID = 1;
    constexpr double APPEAR_SHOW_MS = 280.0;    // trượt ra (vượt nhẹ rồi về)
    constexpr double APPEAR_HIDE_MS = 170.0;    // thu vào
    constexpr double HOVER_MS = 140.0;
    constexpr double PREVIEW_ALPHA = 0.72;

    // Màu lấy từ logo tsudev (xanh lơ) trên nền xanh than đậm của mặt cú.
    const Gdiplus::Color ACCENT(255, 63, 182, 245);
    const Gdiplus::Color BODY_TOP(240, 27, 38, 56);
    const Gdiplus::Color BODY_BOTTOM(240, 12, 18, 29);

    BYTE Mix(BYTE a, BYTE b, double t)
    {
        return static_cast<BYTE>(a + (b - a) * t + 0.5);
    }

    Gdiplus::Color MixColor(const Gdiplus::Color& a, const Gdiplus::Color& b, double t)
    {
        return Gdiplus::Color(Mix(a.GetA(), b.GetA(), t), Mix(a.GetR(), b.GetR(), t), Mix(a.GetG(), b.GetG(), t), Mix(a.GetB(), b.GetB(), t));
    }

    // Hình chữ nhật bo góc (GDI+ không có sẵn).
    void AddRoundRect(Gdiplus::GraphicsPath& path, Gdiplus::REAL x, Gdiplus::REAL y, Gdiplus::REAL w, Gdiplus::REAL h, Gdiplus::REAL r)
    {
        Gdiplus::REAL d = r * 2;
        if (d > w) d = w;
        if (d > h) d = h;
        path.AddArc(x, y, d, d, 180, 90);
        path.AddArc(x + w - d, y, d, d, 270, 90);
        path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
        path.AddArc(x, y + h - d, d, d, 90, 90);
        path.CloseFigure();
    }
}

BEGIN_MESSAGE_MAP(CEdgeHandleWnd, CWnd)
    ON_WM_TIMER()
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, &CEdgeHandleWnd::OnMouseLeave)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONUP()
    ON_WM_MOUSEACTIVATE()
    ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

bool CEdgeHandleWnd::Create(HWND notify_wnd)
{
    m_notify_wnd = notify_wnd;
    LPCTSTR cls = AfxRegisterWndClass(0, ::LoadCursor(nullptr, IDC_HAND), nullptr, nullptr);
    // Không có cửa sổ cha/chủ: cửa sổ nổi bị ẩn thì tay nắm vẫn phải hiện.
    if (!CreateEx(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, cls, _T("tsudev-trafficmonitor edge handle"),
        WS_POPUP, 0, 0, 1, 1, nullptr, nullptr))
        return false;
    if (m_tool_tip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX))
    {
        m_tool_tip.SetMaxTipWidth(theApp.DPI(400));
        m_tool_tip.AddTool(this, _T(""));
    }
    return true;
}

int CEdgeHandleWnd::Thickness()
{
    return theApp.DPI(23);
}

int CEdgeHandleWnd::Length()
{
    return theApp.DPI(68);
}

double CEdgeHandleWnd::NowMs()
{
    static LARGE_INTEGER freq{};
    if (freq.QuadPart == 0)
        QueryPerformanceFrequency(&freq);
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    return static_cast<double>(now.QuadPart) * 1000.0 / static_cast<double>(freq.QuadPart);
}

void CEdgeHandleWnd::SetToolTipText(const CString& text)
{
    if (m_tool_tip.GetSafeHwnd() != NULL)
        m_tool_tip.UpdateTipText(text, this);
}

void CEdgeHandleWnd::ShowAt(EdgeDock::Edge edge, const EdgeDock::Rect& rect, bool preview)
{
    if (GetSafeHwnd() == NULL || edge == EdgeDock::Edge::None || rect.IsEmpty())
        return;
    bool edge_changed = (edge != m_edge);
    m_edge = edge;
    m_rect = rect;
    if (preview != m_preview || !m_target_visible)
    {
        // gợi ý khi kéo: chuột xuyên qua; tay nắm thật: nhận chuột
        LONG ex = GetWindowLong(m_hWnd, GWL_EXSTYLE);
        ex = preview ? (ex | WS_EX_TRANSPARENT) : (ex & ~WS_EX_TRANSPARENT);
        SetWindowLong(m_hWnd, GWL_EXSTYLE, ex);
    }
    m_preview = preview;
    double now = NowMs();
    if (!m_target_visible || edge_changed)
    {
        if (edge_changed)
            m_appear = 0;
        m_appear_from = m_appear;
        m_appear_start = now;
    }
    m_target_visible = true;
    if (preview)
    {
        m_hover_target = 0;
        m_hover = 0;
    }
    Render();
    if (!IsWindowVisible())
        ShowWindow(SW_SHOWNOACTIVATE);
    ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    StartTimer();
}

void CEdgeHandleWnd::MoveTo(const EdgeDock::Rect& rect)
{
    if (rect == m_rect || rect.IsEmpty())
        return;
    m_rect = rect;
    if (m_target_visible)
        Render();
}

void CEdgeHandleWnd::HideAnimated()
{
    if (GetSafeHwnd() == NULL || !m_target_visible)
        return;
    m_target_visible = false;
    m_appear_from = m_appear;
    m_appear_start = NowMs();
    m_tool_tip.Pop();
    StartTimer();
}

void CEdgeHandleWnd::HideNow()
{
    m_target_visible = false;
    m_appear = 0;
    m_hover = 0;
    m_hover_target = 0;
    if (GetSafeHwnd() != NULL)
    {
        KillTimer(ANIM_TIMER_ID);
        m_timer_running = false;
        ShowWindow(SW_HIDE);
    }
}

void CEdgeHandleWnd::StartTimer()
{
    if (!m_timer_running && GetSafeHwnd() != NULL)
    {
        SetTimer(ANIM_TIMER_ID, USER_TIMER_MINIMUM, nullptr);
        m_timer_running = true;
    }
}

void CEdgeHandleWnd::Animate(double& value, double& from, double target, double& start_ms, double duration_ms, double now)
{
    double t = EdgeDock::Progress(now - start_ms, duration_ms);
    value = from + (target - from) * t;     // tuyến tính; đường cong áp dụng khi vẽ
    if (t >= 1.0)
    {
        value = target;
        from = target;
    }
}

void CEdgeHandleWnd::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent != ANIM_TIMER_ID)
    {
        CWnd::OnTimer(nIDEvent);
        return;
    }
    double now = NowMs();
    double appear_target = m_target_visible ? 1.0 : 0.0;
    Animate(m_appear, m_appear_from, appear_target, m_appear_start, m_target_visible ? APPEAR_SHOW_MS : APPEAR_HIDE_MS, now);
    Animate(m_hover, m_hover_from, m_hover_target, m_hover_start, HOVER_MS, now);

    if (!m_target_visible && m_appear <= 0.0)
    {
        HideNow();
        return;
    }
    Render();
    if (m_appear == appear_target && m_hover == m_hover_target)
    {
        KillTimer(ANIM_TIMER_ID);
        m_timer_running = false;
    }
}

void CEdgeHandleWnd::Render()
{
    if (GetSafeHwnd() == NULL || m_edge == EdgeDock::Edge::None || m_rect.IsEmpty())
        return;
    const bool vertical = EdgeDock::IsVertical(m_edge);
    const int cw = vertical ? m_rect.Width() : m_rect.Height();      // bề dày (hướng vuông góc với mép)
    const int ch = vertical ? m_rect.Height() : m_rect.Width();      // chiều dài dọc theo mép
    const int bw = m_rect.Width(), bh = m_rect.Height();

    // Tiến độ xuất hiện đã qua đường cong: hiện = vượt nhẹ kiểu lò xo, ẩn = tăng tốc dần
    // (khi ẩn m_appear giảm 1 -> 0: chậm lúc đầu, nhanh dần về cuối)
    double appear = m_target_visible ? EdgeDock::EaseOutBack(m_appear, 1.1) : 1.0 - EdgeDock::EaseInCubic(1.0 - m_appear);
    double alpha = EdgeDock::Clamp01(m_target_visible ? EdgeDock::EaseOutCubic(m_appear) : m_appear) * (m_preview ? PREVIEW_ALPHA : 1.0);
    int slide = static_cast<int>(std::lround((1.0 - appear) * cw));
    POINT dst{ m_rect.left, m_rect.top };
    switch (m_edge)
    {
    case EdgeDock::Edge::Left: dst.x -= slide; break;
    case EdgeDock::Edge::Right: dst.x += slide; break;
    case EdgeDock::Edge::Top: dst.y -= slide; break;
    case EdgeDock::Edge::Bottom: dst.y += slide; break;
    default: break;
    }

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bw;
    bmi.bmiHeader.biHeight = -bh;       // từ trên xuống
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits{};
    HDC screen_dc = ::GetDC(nullptr);
    HDC mem_dc = ::CreateCompatibleDC(screen_dc);
    HBITMAP dib = ::CreateDIBSection(screen_dc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dib != NULL && bits != nullptr && mem_dc != NULL)
    {
        HGDIOBJ old_bmp = ::SelectObject(mem_dc, dib);
        {
            // GDI+ vẽ thẳng vào bộ đệm DIB ở dạng ARGB nhân trước (đúng định dạng UpdateLayeredWindow cần)
            Gdiplus::Bitmap canvas(bw, bh, bw * 4, PixelFormat32bppPARGB, static_cast<BYTE*>(bits));
            Gdiplus::Graphics g(&canvas);
            g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
            g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
            g.Clear(Gdiplus::Color(0, 0, 0, 0));
            // Vẽ theo hướng chuẩn (mép trái), rồi xoay/lật cho mép thực tế
            Gdiplus::REAL fcw = static_cast<Gdiplus::REAL>(cw);
            switch (m_edge)
            {
            case EdgeDock::Edge::Right: { Gdiplus::Matrix m(-1, 0, 0, 1, fcw, 0); g.SetTransform(&m); } break;
            case EdgeDock::Edge::Top: { Gdiplus::Matrix m(0, 1, 1, 0, 0, 0); g.SetTransform(&m); } break;
            case EdgeDock::Edge::Bottom: { Gdiplus::Matrix m(0, -1, 1, 0, 0, fcw); g.SetTransform(&m); } break;
            default: break;
            }
            Paint(g, cw, ch);
        }
        POINT src{ 0, 0 };
        SIZE size{ bw, bh };
        BLENDFUNCTION bf{};
        bf.BlendOp = AC_SRC_OVER;
        bf.SourceConstantAlpha = static_cast<BYTE>(std::lround(EdgeDock::Clamp01(alpha) * 255.0));
        bf.AlphaFormat = AC_SRC_ALPHA;
        ::UpdateLayeredWindow(m_hWnd, screen_dc, &dst, &size, mem_dc, &src, 0, &bf, ULW_ALPHA);
        ::SelectObject(mem_dc, old_bmp);
    }
    if (dib != NULL)
        ::DeleteObject(dib);
    if (mem_dc != NULL)
        ::DeleteDC(mem_dc);
    ::ReleaseDC(nullptr, screen_dc);
}

// Vẽ tay nắm theo hướng chuẩn: mép màn hình ở x = 0, mũi tên chỉ sang phải (vào trong màn hình).
void CEdgeHandleWnd::Paint(Gdiplus::Graphics& g, int width, int height) const
{
    using Gdiplus::REAL;
    const double hover = EdgeDock::EaseOutCubic(m_hover);
    const REAL pad = static_cast<REAL>(theApp.DPI(5));          // chừa chỗ cho bóng đổ
    const REAL body_w_min = static_cast<REAL>(width) - pad - static_cast<REAL>(theApp.DPI(4));
    const REAL body_w = body_w_min + static_cast<REAL>(theApp.DPI(4) * hover);   // trỏ chuột vào thì nở ra
    const REAL radius = static_cast<REAL>(theApp.DPI(8));
    const REAL top = pad, body_h = static_cast<REAL>(height) - pad * 2;
    // Thân kéo dài qua mép trái (x < 0) để chỉ bo góc ở phía trong màn hình
    const REAL left = -radius - 2;
    const REAL full_w = body_w - left;

    // Bóng đổ mềm: vài lớp bo góc nở dần, mờ dần
    for (int i = 4; i >= 1; i--)
    {
        REAL grow = static_cast<REAL>(theApp.DPI(i));
        Gdiplus::GraphicsPath shadow;
        AddRoundRect(shadow, left, top - grow + 1, full_w + grow, body_h + grow * 2, radius + grow);
        Gdiplus::SolidBrush brush(Gdiplus::Color(static_cast<BYTE>(14 + (4 - i) * 6), 0, 0, 0));
        g.FillPath(&brush, &shadow);
    }

    Gdiplus::GraphicsPath body;
    AddRoundRect(body, left, top, full_w, body_h, radius);
    Gdiplus::LinearGradientBrush fill(Gdiplus::PointF(0, top), Gdiplus::PointF(0, top + body_h),
        MixColor(BODY_TOP, Gdiplus::Color(250, 90, 200, 250), hover), MixColor(BODY_BOTTOM, Gdiplus::Color(250, 38, 150, 225), hover));
    g.FillPath(&fill, &body);

    // Ánh sáng nhẹ ở nửa trên (cảm giác nổi khối)
    Gdiplus::GraphicsPath gloss;
    AddRoundRect(gloss, left, top, full_w, body_h * 0.48f, radius);
    Gdiplus::LinearGradientBrush gloss_brush(Gdiplus::PointF(0, top), Gdiplus::PointF(0, top + body_h * 0.48f),
        Gdiplus::Color(static_cast<BYTE>(34 + 20 * hover), 255, 255, 255), Gdiplus::Color(0, 255, 255, 255));
    g.FillPath(&gloss_brush, &gloss);

    // Viền màu nhấn
    Gdiplus::Pen border(MixColor(Gdiplus::Color(150, ACCENT.GetR(), ACCENT.GetG(), ACCENT.GetB()), ACCENT, hover), static_cast<REAL>(theApp.DPI(1)));
    g.DrawPath(&border, &body);

    // Vạch sáng sát mép màn hình
    Gdiplus::SolidBrush strip(Gdiplus::Color(static_cast<BYTE>(200 + 55 * hover), ACCENT.GetR(), ACCENT.GetG(), ACCENT.GetB()));
    REAL strip_w = static_cast<REAL>(theApp.DPI(2));
    g.FillRectangle(&strip, 0.0f, top + radius * 0.6f, strip_w, body_h - radius * 1.2f);

    // Mũi tên (chevron) chỉ vào trong; trỏ chuột vào thì nhích về phía trước
    const REAL cx = (strip_w + body_w) / 2 + static_cast<REAL>(theApp.DPI(1) * hover);
    const REAL cy = top + body_h / 2;
    const REAL half_h = static_cast<REAL>(theApp.DPI(6));
    const REAL depth = static_cast<REAL>(theApp.DPI(4));
    Gdiplus::Color arrow_color = MixColor(Gdiplus::Color(245, 236, 246, 255), Gdiplus::Color(255, 8, 20, 34), hover);
    Gdiplus::Pen arrow(arrow_color, static_cast<REAL>(theApp.DPI(2)) + 0.4f);
    arrow.SetStartCap(Gdiplus::LineCapRound);
    arrow.SetEndCap(Gdiplus::LineCapRound);
    arrow.SetLineJoin(Gdiplus::LineJoinRound);
    Gdiplus::PointF pts[3] = {
        Gdiplus::PointF(cx - depth / 2, cy - half_h),
        Gdiplus::PointF(cx + depth / 2, cy),
        Gdiplus::PointF(cx - depth / 2, cy + half_h),
    };
    g.DrawLines(&arrow, pts, 3);
}

EdgeDock::Rect CEdgeHandleWnd::ClampToWorkArea(const EdgeDock::Rect& rect) const
{
    RECT r{ rect.left, rect.top, rect.right, rect.bottom };
    MONITORINFO mi{ sizeof(MONITORINFO) };
    if (!::GetMonitorInfo(::MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST), &mi))
        return rect;
    EdgeDock::Rect work{ mi.rcWork.left, mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom };
    return EdgeDock::ClampInto(rect, work);
}

void CEdgeHandleWnd::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_tracking_leave)
    {
        TRACKMOUSEEVENT tme{ sizeof(TRACKMOUSEEVENT) };
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        m_tracking_leave = (::TrackMouseEvent(&tme) != FALSE);
    }
    if (m_hover_target != 1.0 && !m_preview)
    {
        m_hover_from = m_hover;
        m_hover_target = 1.0;
        m_hover_start = NowMs();
        StartTimer();
    }
    if (m_pressed)
    {
        CPoint cursor;
        ::GetCursorPos(&cursor);
        int delta = EdgeDock::IsVertical(m_edge) ? cursor.y - m_press_point.y : cursor.x - m_press_point.x;
        if (!m_dragging && abs(delta) >= theApp.DPI(5))
        {
            m_dragging = true;
            m_tool_tip.Pop();
        }
        if (m_dragging)
        {
            EdgeDock::Rect r = EdgeDock::IsVertical(m_edge) ? m_press_rect.Offset(0, delta) : m_press_rect.Offset(delta, 0);
            m_rect = ClampToWorkArea(r);
            Render();
        }
    }
    CWnd::OnMouseMove(nFlags, point);
}

LRESULT CEdgeHandleWnd::OnMouseLeave(WPARAM, LPARAM)
{
    m_tracking_leave = false;
    if (!m_pressed && m_hover_target != 0.0)
    {
        m_hover_from = m_hover;
        m_hover_target = 0.0;
        m_hover_start = NowMs();
        StartTimer();
    }
    return 0;
}

void CEdgeHandleWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_pressed = true;
    m_dragging = false;
    ::GetCursorPos(&m_press_point);
    m_press_rect = m_rect;
    SetCapture();
    CWnd::OnLButtonDown(nFlags, point);
}

void CEdgeHandleWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
    bool was_pressed = m_pressed;
    bool was_dragging = m_dragging;
    m_pressed = false;
    m_dragging = false;
    if (GetCapture() == this)
        ReleaseCapture();
    if (was_pressed && m_notify_wnd != NULL)
    {
        if (was_dragging)
        {
            int delta = EdgeDock::IsVertical(m_edge) ? m_rect.top - m_press_rect.top : m_rect.left - m_press_rect.left;
            if (delta != 0)
                ::PostMessage(m_notify_wnd, WM_EDGE_HANDLE_ACTION, static_cast<WPARAM>(EdgeHandleAction::DragAlong), static_cast<LPARAM>(delta));
        }
        else
        {
            ::PostMessage(m_notify_wnd, WM_EDGE_HANDLE_ACTION, static_cast<WPARAM>(EdgeHandleAction::Click), 0);
        }
    }
    CWnd::OnLButtonUp(nFlags, point);
}

void CEdgeHandleWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
    if (m_notify_wnd != NULL)
        ::PostMessage(m_notify_wnd, WM_EDGE_HANDLE_ACTION, static_cast<WPARAM>(EdgeHandleAction::ContextMenu), 0);
    CWnd::OnRButtonUp(nFlags, point);
}

int CEdgeHandleWnd::OnMouseActivate(CWnd*, UINT, UINT)
{
    return MA_NOACTIVATE;       // bấm tay nắm không lấy tiêu điểm của cửa sổ đang dùng
}

void CEdgeHandleWnd::OnCaptureChanged(CWnd* pWnd)
{
    // mất capture giữa chừng (vd. Alt+Tab): hủy kéo, trả tay nắm về chỗ cũ
    if (m_pressed && pWnd != this)
    {
        m_pressed = false;
        if (m_dragging)
        {
            m_dragging = false;
            m_rect = m_press_rect;
            Render();
        }
    }
    CWnd::OnCaptureChanged(pWnd);
}

BOOL CEdgeHandleWnd::PreTranslateMessage(MSG* pMsg)
{
    if (m_tool_tip.GetSafeHwnd() != NULL && !m_dragging && !m_preview)
        m_tool_tip.RelayEvent(pMsg);
    return CWnd::PreTranslateMessage(pMsg);
}
