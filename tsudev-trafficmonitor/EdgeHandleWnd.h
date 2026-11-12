// EdgeHandleWnd.h - tay nắm hình mũi tên ở mép màn hình khi cửa sổ nổi đã thu gọn vào mép.
//
// Cửa sổ popup layered (32-bit, alpha từng điểm ảnh), vẽ bằng GDI+ khử răng cưa; không lấy tiêu điểm, không hiện trên
// thanh tác vụ. Có hoạt ảnh trượt ra/vào mép và sáng lên khi trỏ chuột. Bấm: hiện lại cửa sổ nổi; kéo dọc mép: dời vị
// trí; chuột phải: menu chính. Mọi thao tác báo về cửa sổ chính bằng WM_EDGE_HANDLE_ACTION.

#pragma once
#include "EdgeDock.h"
#include <gdiplus.h>

// wParam của WM_EDGE_HANDLE_ACTION
enum class EdgeHandleAction
{
    Click = 0,          // bấm: hiện lại cửa sổ nổi
    DragAlong = 1,      // kéo dọc mép xong: lParam = độ dịch (px, có dấu) dọc theo mép
    ContextMenu = 2,    // chuột phải: hiện menu chính
};

class CEdgeHandleWnd : public CWnd
{
public:
    CEdgeHandleWnd() = default;

    bool Create(HWND notify_wnd);

    // Bề dày (vuông góc với mép, gồm cả bóng đổ) và chiều dài (dọc theo mép) của tay nắm theo DPI hiện tại.
    static int Thickness();
    static int Length();

    // Hiện tay nắm (có hoạt ảnh trượt ra từ mép). preview = true: chỉ là gợi ý mờ khi đang kéo cửa sổ nổi tới mép,
    // chuột xuyên qua được.
    void ShowAt(EdgeDock::Edge edge, const EdgeDock::Rect& rect, bool preview);
    void MoveTo(const EdgeDock::Rect& rect);
    void HideAnimated();
    void HideNow();
    void SetToolTipText(const CString& text);

    bool IsShown() const { return m_target_visible; }
    bool IsPreview() const { return m_preview; }
    EdgeDock::Edge GetEdge() const { return m_edge; }
    const EdgeDock::Rect& GetRect() const { return m_rect; }

protected:
    void Render();
    void Paint(Gdiplus::Graphics& g, int width, int height) const;
    void StartTimer();
    void Animate(double& value, double& from, double target, double& start_ms, double duration_ms, double now);
    EdgeDock::Rect ClampToWorkArea(const EdgeDock::Rect& rect) const;

public:
    static double NowMs();      // đồng hồ độ phân giải cao (ms) cho hoạt ảnh

protected:
    HWND m_notify_wnd{};
    EdgeDock::Edge m_edge{ EdgeDock::Edge::None };
    EdgeDock::Rect m_rect{};            // vị trí cuối (đã trượt ra hẳn), tọa độ màn hình
    bool m_preview{};
    bool m_target_visible{};

    // hoạt ảnh: giá trị hiện tại, giá trị lúc bắt đầu, thời điểm bắt đầu (ms)
    double m_appear{}, m_appear_from{}, m_appear_start{};
    double m_hover{}, m_hover_from{}, m_hover_target{}, m_hover_start{};
    bool m_timer_running{};

    // chuột
    bool m_tracking_leave{};
    bool m_pressed{};
    bool m_dragging{};
    CPoint m_press_point{};
    EdgeDock::Rect m_press_rect{};
    CToolTipCtrl m_tool_tip;

    DECLARE_MESSAGE_MAP()
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
    afx_msg void OnCaptureChanged(CWnd* pWnd);
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
