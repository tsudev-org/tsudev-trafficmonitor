// Kiểm thử logic nhắc gia hạn TsuDEV Pro (tsudev-trafficmonitor/ProRenewal.h, PRODUCT_POLICY mục 7.6 - 6 ca bắt buộc
// + ca biên). Build và chạy trong CI (build.yml, job "Kiểm thử C++"); chạy tay:
//   cl /nologo /std:c++17 /EHsc /W4 /WX /utf-8 /sdl /I tsudev-trafficmonitor tests\ProRenewalTests.cpp /Fe:ProRenewalTests.exe
//   g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -I tsudev-trafficmonitor tests/ProRenewalTests.cpp

#include "ProRenewal.h"
#include <iostream>

using namespace ProRenewal;

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

static const int64_t T0 = 1'800'000'000;   // mốc giờ bất kỳ

static Entitlement Jws(int64_t pro_until, int64_t issued_at)
{
    Entitlement e;
    e.present = true;
    e.exp = issued_at + 7 * kDay;
    e.pro_until = pro_until;
    return e;
}

// Ca 1: còn 8 ngày => không nhắc.
static void TestCon8Ngay()
{
    Tracker t;
    t.OnStart(T0);
    CHECK(t.Evaluate(Jws(T0 + 8 * kDay, T0), T0).kind == Kind::None);
    // Biên: 7 ngày + 1 giây vẫn chưa nhắc.
    Tracker t2;
    t2.OnStart(T0);
    CHECK(t2.Evaluate(Jws(T0 + 7 * kDay + 1, T0), T0).kind == Kind::None);
}

// Ca 2: còn 7 ngày => nhắc, {n} = 7; còn 1 giờ => {n} = 1 (làm tròn lên).
static void TestCon7Ngay()
{
    Tracker t;
    t.OnStart(T0);
    Decision d = t.Evaluate(Jws(T0 + 7 * kDay, T0), T0);
    CHECK(d.kind == Kind::Renew);
    CHECK(d.days_left == 7);
    CHECK(d.until == T0 + 7 * kDay);

    Tracker t2;
    t2.OnStart(T0);
    Decision d2 = t2.Evaluate(Jws(T0 + 3600, T0), T0);
    CHECK(d2.kind == Kind::Renew);
    CHECK(d2.days_left == 1);
    // 6 ngày 1 giây => 7 (làm tròn lên).
    CHECK(DaysLeftCeil(T0 + 6 * kDay + 1, T0) == 7);
}

// Ca 3: mở app 2 lần trong 7 ngày cuối => nhắc 2 lần; quay lại sau < 30 phút => không nhắc lại.
static void TestMoHaiLan()
{
    const Entitlement e = Jws(T0 + 5 * kDay, T0);
    Tracker t;
    t.OnStart(T0);
    CHECK(t.Evaluate(e, T0).kind == Kind::Renew);
    CHECK(t.Evaluate(e, T0 + 10).kind == Kind::None);           // cùng một lần mở: tối đa một lần

    t.OnBackground(T0 + 60);
    t.OnForeground(T0 + 60 + 29 * 60);                          // < 30 phút
    CHECK(t.Evaluate(e, T0 + 60 + 29 * 60).kind == Kind::None);

    t.OnBackground(T0 + 2 * 3600);
    t.OnForeground(T0 + 2 * 3600 + 30 * 60);                    // >= 30 phút = lần mở mới
    CHECK(t.Evaluate(e, T0 + 2 * 3600 + 30 * 60).kind == Kind::Renew);

    Tracker khoiDongLai;                                        // khởi động mới = lần mở mới
    khoiDongLai.OnStart(T0 + kDay);
    CHECK(khoiDongLai.Evaluate(e, T0 + kDay).kind == Kind::Renew);
}

// Ca 4: hết hạn => báo hết kỳ đúng một lần (kèm ngày xoá = +90 ngày), mở lại => không báo nữa.
static void TestHetHan()
{
    const int64_t until = T0 - 3600;
    const Entitlement e = Jws(until, T0);                       // JWS mới (còn hạn) nhưng Pro đã hết
    Tracker t(0);
    t.OnStart(T0);
    Decision d = t.Evaluate(e, T0);
    CHECK(d.kind == Kind::Expired);
    CHECK(d.delete_at == until + 90 * kDay);
    CHECK(t.NotifiedUntil() == until);

    Tracker moLai(t.NotifiedUntil());                           // giá trị đã lưu cấu hình
    moLai.OnStart(T0 + kDay);
    CHECK(moLai.Evaluate(Jws(until, T0 + kDay), T0 + kDay).kind == Kind::None);
}

// Ca 5: gia hạn (JWS mới, pro_until xa hơn) => thôi nhắc; kỳ sau hết hạn => báo lại.
static void TestGiaHan()
{
    const int64_t cu = T0 + 3 * kDay;
    Tracker t;
    t.OnStart(T0);
    CHECK(t.Evaluate(Jws(cu, T0), T0).kind == Kind::Renew);

    const int64_t moi = cu + 365 * kDay;
    Tracker sauGiaHan(t.NotifiedUntil());
    sauGiaHan.OnStart(T0 + kDay);
    CHECK(sauGiaHan.Evaluate(Jws(moi, T0 + kDay), T0 + kDay).kind == Kind::None);

    // Kỳ trước đã từng báo hết hạn (notified_until = cu); kỳ mới hết hạn => báo lại cho giá trị mới.
    Tracker kySau(cu);
    kySau.OnStart(moi + 60);
    Decision d = kySau.Evaluate(Jws(moi, moi), moi + 60);
    CHECK(d.kind == Kind::Expired);
    CHECK(kySau.NotifiedUntil() == moi);
}

// Ca 6: ngoại tuyến với JWS đã hết hạn => không nhắc, không lỗi; chưa từng có JWS/Pro => không gì.
static void TestNgoaiTuyen()
{
    Tracker t;
    t.OnStart(T0);
    Entitlement cu = Jws(T0 + 2 * kDay, T0 - 8 * kDay);         // JWS cấp 8 ngày trước => exp đã qua
    CHECK(cu.exp <= T0);
    CHECK(t.Evaluate(cu, T0).kind == Kind::None);
    Entitlement hetCa = Jws(T0 - kDay, T0 - 8 * kDay);           // cả Pro lẫn JWS đều hết: vẫn không báo
    CHECK(t.Evaluate(hetCa, T0).kind == Kind::None);
    CHECK(t.NotifiedUntil() == 0);

    CHECK(t.Evaluate(Entitlement{}, T0).kind == Kind::None);    // bản miễn phí hiện tại (chưa có JWS)
    CHECK(t.Evaluate(Jws(0, T0), T0).kind == Kind::None);       // JWS của người chưa từng có Pro
}

int main()
{
    TestCon8Ngay();
    TestCon7Ngay();
    TestMoHaiLan();
    TestHetHan();
    TestGiaHan();
    TestNgoaiTuyen();
    std::cout << "ProRenewalTests: " << g_passed << " dat, " << g_failed << " that bai\n";
    return g_failed == 0 ? 0 : 1;
}
