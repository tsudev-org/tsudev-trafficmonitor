// Kiểm thử tự động cho logic thuần của tính năng "thu cửa sổ nổi vào mép màn hình" (tsudev-trafficmonitor/EdgeDock.h).
// Build và chạy trong CI (build.yml, job "Kiểm thử C++"); chạy tay:
//   cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /sdl /I tsudev-trafficmonitor tests\EdgeDockTests.cpp /Fe:EdgeDockTests.exe
//   g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -I tsudev-trafficmonitor tests/EdgeDockTests.cpp

#include "EdgeDock.h"
#include <cmath>
#include <iostream>

using namespace EdgeDock;

static int g_failed = 0;
static int g_passed = 0;

static void Check(bool cond, const char* expr, int line)
{
    if (cond)
    {
        g_passed++;
    }
    else
    {
        g_failed++;
        std::cerr << "THAT BAI (dong " << line << "): " << expr << "\n";
    }
}
#define CHECK(x) Check((x), #x, __LINE__)

static bool Near(double a, double b, double eps = 1e-9)
{
    return std::fabs(a - b) <= eps;
}

// Màn hình 1920x1080, thanh tác vụ 48 px ở dưới.
static const Rect kMonitor{ 0, 0, 1920, 1080 };
static const Rect kWork{ 0, 0, 1920, 1032 };

static void TestEdgeFromInt()
{
    CHECK(EdgeFromInt(0) == Edge::None);
    CHECK(EdgeFromInt(1) == Edge::Left);
    CHECK(EdgeFromInt(2) == Edge::Top);
    CHECK(EdgeFromInt(3) == Edge::Right);
    CHECK(EdgeFromInt(4) == Edge::Bottom);
    CHECK(EdgeFromInt(5) == Edge::None);         // tệp cấu hình hỏng
    CHECK(EdgeFromInt(-1) == Edge::None);
    CHECK(IsVertical(Edge::Left) && IsVertical(Edge::Right));
    CHECK(!IsVertical(Edge::Top) && !IsVertical(Edge::Bottom) && !IsVertical(Edge::None));
}

static void TestDetect()
{
    DetectParams p;
    p.cursor_threshold = 4;
    p.overflow_ratio = 0.5;
    const Rect win{ 500, 400, 720, 460 };       // 220x60, giữa màn hình

    // giữa màn hình: không thu gọn
    CHECK(DetectEdge(win, Point{ 600, 430 }, kMonitor, kWork, p) == Edge::None);
    // con trỏ chạm mép trái / phải / trên / dưới
    CHECK(DetectEdge(win.Offset(-480, 0), Point{ 0, 430 }, kMonitor, kWork, p) == Edge::Left);
    CHECK(DetectEdge(win.Offset(-480, 0), Point{ 3, 430 }, kMonitor, kWork, p) == Edge::Left);
    CHECK(DetectEdge(win.Offset(-480, 0), Point{ 5, 430 }, kMonitor, kWork, p) == Edge::None);    // ngoài ngưỡng 4 px
    CHECK(DetectEdge(win.Offset(1100, 0), Point{ 1919, 430 }, kMonitor, kWork, p) == Edge::Right);
    CHECK(DetectEdge(win.Offset(0, -400), Point{ 600, 0 }, kMonitor, kWork, p) == Edge::Top);
    CHECK(DetectEdge(win.Offset(0, 600), Point{ 600, 1079 }, kMonitor, kWork, p) == Edge::Bottom); // con trỏ trên thanh tác vụ
    // cửa sổ vượt quá nửa ra ngoài vùng làm việc (con trỏ chưa sát mép)
    CHECK(DetectEdge(Rect{ -120, 400, 100, 460 }, Point{ 50, 430 }, kMonitor, kWork, p) == Edge::Left);
    CHECK(DetectEdge(Rect{ -100, 400, 120, 460 }, Point{ 50, 430 }, kMonitor, kWork, p) == Edge::None); // mới 45%
    CHECK(DetectEdge(Rect{ 1800, 400, 2020, 460 }, Point{ 1850, 430 }, kMonitor, kWork, p) == Edge::None);
    CHECK(DetectEdge(Rect{ 1780, 1000, 2000, 1060 }, Point{ 1850, 1010 }, kMonitor, kWork, p) == Edge::None);
    CHECK(DetectEdge(Rect{ 1800, 1000, 2020, 1060 }, Point{ 1850, 1010 }, kMonitor, kWork, p) == Edge::None);
    CHECK(DetectEdge(Rect{ 1700, 400, 1920 + 120, 460 }, Point{ 1850, 430 }, kMonitor, kWork, p) == Edge::None); // 54%? 120/340
    CHECK(DetectEdge(Rect{ 1800, 400, 2020, 460 }.Offset(20, 0), Point{ 1850, 430 }, kMonitor, kWork, p) == Edge::Right); // 120/220 = 54%
    // góc trên-trái: con trỏ sát mép trái hơn => Left; sát mép trên hơn => Top
    CHECK(DetectEdge(Rect{ 0, 0, 220, 60 }, Point{ 0, 3 }, kMonitor, kWork, p) == Edge::Left);
    CHECK(DetectEdge(Rect{ 0, 0, 220, 60 }, Point{ 3, 0 }, kMonitor, kWork, p) == Edge::Top);
    CHECK(DetectEdge(Rect{ 0, 0, 220, 60 }, Point{ 0, 0 }, kMonitor, kWork, p) == Edge::Left);     // hòa: ưu tiên trái/phải
    // cạnh có màn hình liền kề: không thu gọn ở cạnh đó
    DetectParams blocked = p;
    blocked.blocked[static_cast<int>(Edge::Right)] = true;
    CHECK(DetectEdge(win.Offset(1100, 0), Point{ 1919, 430 }, kMonitor, kWork, blocked) == Edge::None);
    CHECK(DetectEdge(win.Offset(-480, 0), Point{ 0, 430 }, kMonitor, kWork, blocked) == Edge::Left);
    // con trỏ ngoài màn hình này (đang ở màn hình khác) thì chỉ xét phần vượt ra ngoài
    CHECK(DetectEdge(win, Point{ -5, 430 }, kMonitor, kWork, p) == Edge::None);
    // dữ liệu rỗng
    CHECK(DetectEdge(Rect{}, Point{ 0, 0 }, kMonitor, kWork, p) == Edge::None);
    CHECK(DetectEdge(win, Point{ 0, 0 }, Rect{}, kWork, p) == Edge::None);
    // màn hình phụ ở tọa độ âm (bên trái màn hình chính)
    const Rect mon2{ -1280, 0, 0, 1024 };
    CHECK(DetectEdge(Rect{ -1280, 300, -1060, 360 }, Point{ -1280, 330 }, mon2, mon2, p) == Edge::Left);
    CHECK(DetectEdge(Rect{ -220, 300, 0, 360 }, Point{ -1, 330 }, mon2, mon2, p) == Edge::Right);
}

static void TestRects()
{
    const Rect win{ 500, 400, 720, 460 };
    // ClampInto
    CHECK(ClampInto(Rect{ -50, -10, 170, 50 }, kWork) == (Rect{ 0, 0, 220, 60 }));
    CHECK(ClampInto(Rect{ 1800, 1000, 2020, 1060 }, kWork) == (Rect{ 1700, 972, 1920, 1032 }));
    CHECK(ClampInto(win, kWork) == win);
    CHECK(ClampInto(Rect{ 100, 100, 3000, 200 }, kWork).left == 0);     // rộng hơn vùng: canh trái

    // ShownRect: sát mép đã thu gọn, cách margin
    CHECK(ShownRect(Rect{ -300, 400, -80, 460 }, Edge::Left, kWork, 8) == (Rect{ 8, 400, 228, 460 }));
    CHECK(ShownRect(Rect{ 1900, 400, 2120, 460 }, Edge::Right, kWork, 8) == (Rect{ 1692, 400, 1912, 460 }));
    CHECK(ShownRect(Rect{ 500, -90, 720, -30 }, Edge::Top, kWork, 8) == (Rect{ 500, 8, 720, 68 }));
    CHECK(ShownRect(Rect{ 500, 1050, 720, 1110 }, Edge::Bottom, kWork, 8) == (Rect{ 500, 964, 720, 1024 }));
    CHECK(ShownRect(Rect{ -300, 1050, -80, 1110 }, Edge::Left, kWork, 8) == (Rect{ 8, 972, 228, 1032 })); // góc: kẹp cả phương kia
    CHECK(ShownRect(win, Edge::None, kWork, 8) == win);

    // HiddenRect: trượt hẳn ra ngoài mép, phương kia giữ nguyên
    const Rect shownL{ 8, 400, 228, 460 };
    CHECK(HiddenRect(shownL, Edge::Left, kWork) == (Rect{ -220, 400, 0, 460 }));
    CHECK(HiddenRect(Rect{ 1692, 400, 1912, 460 }, Edge::Right, kWork) == (Rect{ 1920, 400, 2140, 460 }));
    CHECK(HiddenRect(Rect{ 500, 8, 720, 68 }, Edge::Top, kWork) == (Rect{ 500, -60, 720, 0 }));
    CHECK(HiddenRect(Rect{ 500, 964, 720, 1024 }, Edge::Bottom, kWork) == (Rect{ 500, 1032, 720, 1092 }));
    CHECK(HiddenRect(shownL, Edge::None, kWork) == shownL);

    // HandleRect: áp sát mép, giữa tầm cửa sổ, không lọt ra ngoài vùng làm việc
    CHECK(HandleRect(shownL, Edge::Left, kWork, 24, 64) == (Rect{ 0, 398, 24, 462 }));
    CHECK(HandleRect(Rect{ 1692, 400, 1912, 460 }, Edge::Right, kWork, 24, 64) == (Rect{ 1896, 398, 1920, 462 }));
    CHECK(HandleRect(Rect{ 8, 0, 228, 20 }, Edge::Left, kWork, 24, 64).top == 0);                 // sát trên
    CHECK(HandleRect(Rect{ 8, 1012, 228, 1032 }, Edge::Left, kWork, 24, 64).bottom == 1032);      // sát dưới
    CHECK(HandleRect(Rect{ 500, 8, 720, 68 }, Edge::Top, kWork, 24, 64) == (Rect{ 578, 0, 642, 24 }));
    CHECK(HandleRect(Rect{ 500, 964, 720, 1024 }, Edge::Bottom, kWork, 24, 64) == (Rect{ 578, 1008, 642, 1032 }));
    CHECK(HandleRect(Rect{ 1890, 8, 1919, 68 }, Edge::Top, kWork, 24, 64).right == 1920);
    CHECK(HandleRect(shownL, Edge::None, kWork, 24, 64).IsEmpty());

    // SlideAlongEdge
    CHECK(SlideAlongEdge(shownL, Edge::Left, kWork, 100) == shownL.Offset(0, 100));
    CHECK(SlideAlongEdge(shownL, Edge::Left, kWork, -1000).top == 0);
    CHECK(SlideAlongEdge(shownL, Edge::Left, kWork, 5000).bottom == 1032);
    CHECK(SlideAlongEdge(shownL, Edge::Left, kWork, 5000).left == shownL.left);   // phương vuông góc giữ nguyên
    CHECK(SlideAlongEdge(Rect{ 500, 8, 720, 68 }, Edge::Top, kWork, 5000).right == 1920);

    // mũi tên chỉ vào trong
    CHECK(ArrowDirection(Edge::Left).x == 1 && ArrowDirection(Edge::Left).y == 0);
    CHECK(ArrowDirection(Edge::Right).x == -1);
    CHECK(ArrowDirection(Edge::Top).y == 1);
    CHECK(ArrowDirection(Edge::Bottom).y == -1);
    CHECK(ArrowDirection(Edge::None).x == 0 && ArrowDirection(Edge::None).y == 0);
}

static void TestEasing()
{
    CHECK(Near(Progress(0, 200), 0.0));
    CHECK(Near(Progress(100, 200), 0.5));
    CHECK(Near(Progress(500, 200), 1.0));
    CHECK(Near(Progress(-5, 200), 0.0));
    CHECK(Near(Progress(5, 0), 1.0));

    double (*curves[])(double) = { EaseOutCubic, EaseInCubic, EaseInOutCubic };
    for (auto f : curves)
    {
        CHECK(Near(f(0.0), 0.0));
        CHECK(Near(f(1.0), 1.0));
        CHECK(Near(f(-1.0), 0.0));
        CHECK(Near(f(2.0), 1.0));
        double last = -1;
        bool monotonic = true;
        for (int i = 0; i <= 100; i++)
        {
            double v = f(i / 100.0);
            if (v < last - 1e-12)
                monotonic = false;
            last = v;
        }
        CHECK(monotonic);
    }
    CHECK(EaseOutCubic(0.5) > 0.5);         // nhanh lúc đầu
    CHECK(EaseInCubic(0.5) < 0.5);          // chậm lúc đầu
    CHECK(Near(EaseInOutCubic(0.5), 0.5));
    CHECK(Near(EaseOutBack(0.0), 0.0));
    CHECK(Near(EaseOutBack(1.0), 1.0));
    double peak = 0;
    for (int i = 0; i <= 1000; i++)
        peak = (std::max)(peak, EaseOutBack(i / 1000.0));
    CHECK(peak > 1.0 && peak < 1.08);       // vượt nhẹ, không quá 8%

    CHECK(Lerp(0, 100, 0.0) == 0);
    CHECK(Lerp(0, 100, 1.0) == 100);
    CHECK(Lerp(0, 100, 0.255) == 26);       // làm tròn
    CHECK(Lerp(-220, 8, 0.5) == -106);
    const Rect from{ -220, 400, 0, 460 }, to{ 8, 400, 228, 460 };
    CHECK(LerpRect(from, to, 0.0) == from);
    CHECK(LerpRect(from, to, 1.0) == to);
    CHECK(LerpRect(from, to, 0.5).Width() == 220 && LerpRect(from, to, 0.5).Height() == 60);
}

int main()
{
    TestEdgeFromInt();
    TestDetect();
    TestRects();
    TestEasing();
    std::cout << "EdgeDockTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed == 0 ? 0 : 1;
}
