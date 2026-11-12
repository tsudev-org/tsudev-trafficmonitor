// EdgeDock.h - logic thuần cho tính năng "thu cửa sổ nổi vào mép màn hình".
//
// Kéo cửa sổ nổi sát mép màn hình thì cửa sổ trượt ra ngoài mép và mờ dần, ở mép còn một tay nắm hình mũi tên;
// bấm tay nắm thì cửa sổ trượt trở lại. Header này chỉ chứa phần tính toán (phát hiện mép, vị trí, đường cong
// chuyển động) và KHÔNG phụ thuộc MFC/Win32, để kiểm thử tự động được (tests/EdgeDockTests.cpp).
// Phần giao diện: CEdgeHandleWnd (EdgeHandleWnd.h) và CTsudevTrafficMonitorDlg.

#pragma once
#include <algorithm>
#include <cmath>

namespace EdgeDock
{
    enum class Edge : int
    {
        None = 0,
        Left = 1,
        Top = 2,
        Right = 3,
        Bottom = 4,
    };

    // Đọc từ tệp cấu hình: giá trị lạ => None (không thu gọn).
    inline Edge EdgeFromInt(int value)
    {
        return (value >= 1 && value <= 4) ? static_cast<Edge>(value) : Edge::None;
    }

    inline bool IsVertical(Edge edge)       // tay nắm dựng đứng (mép trái/phải)
    {
        return edge == Edge::Left || edge == Edge::Right;
    }

    struct Point
    {
        int x{};
        int y{};
    };

    struct Rect
    {
        int left{};
        int top{};
        int right{};
        int bottom{};

        int Width() const { return right - left; }
        int Height() const { return bottom - top; }
        bool IsEmpty() const { return right <= left || bottom <= top; }
        Rect Offset(int dx, int dy) const { return Rect{ left + dx, top + dy, right + dx, bottom + dy }; }
        bool operator==(const Rect& o) const { return left == o.left && top == o.top && right == o.right && bottom == o.bottom; }
        bool operator!=(const Rect& o) const { return !(*this == o); }
    };

    struct DetectParams
    {
        int cursor_threshold{ 4 };      // con trỏ cách mép màn hình <= số px này thì coi là "kéo sát mép"
        double overflow_ratio{ 0.5 };   // hoặc phần cửa sổ vượt ra ngoài vùng làm việc >= tỉ lệ này
        // Cạnh nào có màn hình khác liền kề (kéo sang màn hình bên cạnh, không phải thu gọn). Chỉ số = (int)Edge.
        bool blocked[5]{};
    };

    // Phát hiện mép cần thu gọn khi thả cửa sổ.
    //   window : vị trí cửa sổ lúc thả; cursor: vị trí con trỏ
    //   monitor: toàn bộ màn hình chứa cửa sổ; work: vùng làm việc (trừ thanh tác vụ) của màn hình đó
    // Trả về None nếu không thu gọn. Nhiều mép cùng thỏa (góc màn hình) thì chọn mép con trỏ sát nhất,
    // hòa nhau thì ưu tiên trái/phải (tay nắm dọc gọn hơn) rồi trên/dưới.
    inline Edge DetectEdge(const Rect& window, const Point& cursor, const Rect& monitor, const Rect& work, const DetectParams& params)
    {
        if (window.IsEmpty() || monitor.IsEmpty() || work.IsEmpty())
            return Edge::None;
        const bool cursor_inside = cursor.x >= monitor.left && cursor.x < monitor.right && cursor.y >= monitor.top && cursor.y < monitor.bottom;

        struct Candidate
        {
            Edge edge;
            int cursor_distance;        // khoảng cách con trỏ tới mép màn hình
            double overflow;            // phần cửa sổ vượt ra ngoài vùng làm việc (0..1)
        };
        const Candidate all[4] = {
            { Edge::Left,   cursor.x - monitor.left,       window.Width() > 0 ? double(work.left - window.left) / window.Width() : 0.0 },
            { Edge::Right,  monitor.right - 1 - cursor.x,  window.Width() > 0 ? double(window.right - work.right) / window.Width() : 0.0 },
            { Edge::Top,    cursor.y - monitor.top,        window.Height() > 0 ? double(work.top - window.top) / window.Height() : 0.0 },
            { Edge::Bottom, monitor.bottom - 1 - cursor.y, window.Height() > 0 ? double(window.bottom - work.bottom) / window.Height() : 0.0 },
        };

        Edge best = Edge::None;
        double best_score = 0;
        for (const Candidate& c : all)
        {
            if (params.blocked[static_cast<int>(c.edge)])
                continue;
            const bool by_cursor = cursor_inside && c.cursor_distance >= 0 && c.cursor_distance <= params.cursor_threshold;
            const bool by_overflow = c.overflow >= params.overflow_ratio;
            if (!by_cursor && !by_overflow)
                continue;
            // Điểm: con trỏ càng sát càng cao; phần vượt ra ngoài cộng thêm. Dùng > để giữ thứ tự ưu tiên khi hòa.
            double score = (by_cursor ? 2.0 + double(params.cursor_threshold - c.cursor_distance) / (params.cursor_threshold + 1) : 0.0)
                + (by_overflow ? (std::min)(c.overflow, 1.0) : 0.0);
            if (score > best_score)
            {
                best_score = score;
                best = c.edge;
            }
        }
        return best;
    }

    // Đặt khung `r` vào trong `area` (giữ kích thước; khung lớn hơn vùng thì canh theo cạnh trái/trên).
    inline Rect ClampInto(const Rect& r, const Rect& area)
    {
        int dx = 0, dy = 0;
        if (r.right > area.right)
            dx = area.right - r.right;
        if (r.left + dx < area.left)
            dx = area.left - r.left;
        if (r.bottom > area.bottom)
            dy = area.bottom - r.bottom;
        if (r.top + dy < area.top)
            dy = area.top - r.top;
        return r.Offset(dx, dy);
    }

    // Vị trí cửa sổ khi hiện lại: nằm trọn trong vùng làm việc, cách mép đã thu gọn `margin` px.
    inline Rect ShownRect(const Rect& window, Edge edge, const Rect& work, int margin)
    {
        Rect r = ClampInto(window, work);
        switch (edge)
        {
        case Edge::Left: r = r.Offset(work.left + margin - r.left, 0); break;
        case Edge::Right: r = r.Offset(work.right - margin - r.right, 0); break;
        case Edge::Top: r = r.Offset(0, work.top + margin - r.top); break;
        case Edge::Bottom: r = r.Offset(0, work.bottom - margin - r.bottom); break;
        default: break;
        }
        return ClampInto(r, work);
    }

    // Vị trí cửa sổ khi đã trượt hẳn ra ngoài mép (điểm cuối hoạt ảnh ẩn, điểm đầu hoạt ảnh hiện).
    inline Rect HiddenRect(const Rect& shown, Edge edge, const Rect& work)
    {
        switch (edge)
        {
        case Edge::Left: return shown.Offset(work.left - shown.right, 0);
        case Edge::Right: return shown.Offset(work.right - shown.left, 0);
        case Edge::Top: return shown.Offset(0, work.top - shown.bottom);
        case Edge::Bottom: return shown.Offset(0, work.bottom - shown.top);
        default: return shown;
        }
    }

    // Khung tay nắm mũi tên: áp sát mép, ở ngang tầm cửa sổ, luôn nằm trong vùng làm việc.
    //   thickness: bề dày (phương vuông góc với mép); length: chiều dài dọc theo mép.
    inline Rect HandleRect(const Rect& shown, Edge edge, const Rect& work, int thickness, int length)
    {
        Rect r{};
        if (IsVertical(edge))
        {
            int center = (shown.top + shown.bottom) / 2;
            int top = (std::max)(work.top, (std::min)(center - length / 2, work.bottom - length));
            int left = (edge == Edge::Left) ? work.left : work.right - thickness;
            r = Rect{ left, top, left + thickness, top + length };
        }
        else if (edge == Edge::Top || edge == Edge::Bottom)
        {
            int center = (shown.left + shown.right) / 2;
            int left = (std::max)(work.left, (std::min)(center - length / 2, work.right - length));
            int top = (edge == Edge::Top) ? work.top : work.bottom - thickness;
            r = Rect{ left, top, left + length, top + thickness };
        }
        return r;
    }

    // Dịch cửa sổ dọc theo mép (khi kéo tay nắm): giữ cửa sổ trong vùng làm việc theo phương dọc mép.
    inline Rect SlideAlongEdge(const Rect& shown, Edge edge, const Rect& work, int delta)
    {
        Rect r = IsVertical(edge) ? shown.Offset(0, delta) : shown.Offset(delta, 0);
        Rect clamped = ClampInto(r, work);
        // chỉ kẹp theo phương dọc mép; phương vuông góc giữ nguyên
        return IsVertical(edge) ? Rect{ r.left, clamped.top, r.right, clamped.bottom } : Rect{ clamped.left, r.top, clamped.right, r.bottom };
    }

    // Hướng mũi tên trên tay nắm: chỉ vào trong màn hình (hướng cửa sổ sẽ trượt ra).
    inline Point ArrowDirection(Edge edge)
    {
        switch (edge)
        {
        case Edge::Left: return Point{ 1, 0 };
        case Edge::Right: return Point{ -1, 0 };
        case Edge::Top: return Point{ 0, 1 };
        case Edge::Bottom: return Point{ 0, -1 };
        default: return Point{ 0, 0 };
        }
    }

    // ------------------------------------------------------------------ đường cong chuyển động
    inline double Clamp01(double t)
    {
        return t < 0.0 ? 0.0 : (t > 1.0 ? 1.0 : t);
    }

    // Tiến độ 0..1 theo thời gian thực đã trôi qua (không theo số khung hình => mượt cả khi bộ định thời trễ).
    inline double Progress(double elapsed_ms, double duration_ms)
    {
        if (duration_ms <= 0.0)
            return 1.0;
        return Clamp01(elapsed_ms / duration_ms);
    }

    inline double EaseOutCubic(double t)
    {
        t = Clamp01(t);
        double u = 1.0 - t;
        return 1.0 - u * u * u;
    }

    inline double EaseInCubic(double t)
    {
        t = Clamp01(t);
        return t * t * t;
    }

    inline double EaseInOutCubic(double t)
    {
        t = Clamp01(t);
        if (t < 0.5)
            return 4.0 * t * t * t;
        double u = -2.0 * t + 2.0;
        return 1.0 - u * u * u / 2.0;
    }

    // Vượt nhẹ qua đích rồi về (kiểu lò xo). overshoot ~1.0 => vượt khoảng 4-5%.
    inline double EaseOutBack(double t, double overshoot = 1.0)
    {
        t = Clamp01(t);
        double c3 = overshoot + 1.0;
        double u = t - 1.0;
        return 1.0 + c3 * u * u * u + overshoot * u * u;
    }

    inline int Lerp(int from, int to, double t)
    {
        return static_cast<int>(std::lround(from + (to - from) * t));
    }

    // Nội suy vị trí, giữ nguyên kích thước của `to`.
    inline Rect LerpRect(const Rect& from, const Rect& to, double t)
    {
        int left = Lerp(from.left, to.left, t);
        int top = Lerp(from.top, to.top, t);
        return Rect{ left, top, left + to.Width(), top + to.Height() };
    }
}
