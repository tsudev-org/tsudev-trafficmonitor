#pragma once
// Logic thuần nhắc gia hạn TsuDEV Pro và báo hết kỳ (tsudev-standards PRODUCT_POLICY mục 7.6, phiếu 20261120-PRO2,
// ADR-0002). KHÔNG phụ thuộc MFC/Win32 để kiểm thử được trên mọi nền tảng (tests/ProRenewalTests.cpp).
//
// Bản hiện tại CHƯA có đăng nhập/JWS quyền lợi (ADR-0002): ứng dụng luôn ở bản miễn phí nên Evaluate() luôn trả
// None. Khi có nguồn JWS (đã xác minh chữ ký ES256), chỉ cần nạp Entitlement + nối hộp thoại; điều kiện đã có test.
//
// Cách dùng:
//   ProRenewal::Tracker t(persisted_notified_until);         // đọc khoá tsu.pro_expired.notified_until từ cấu hình
//   t.OnStart(now) / t.OnBackground(now) / t.OnForeground(now) // vòng đời cửa sổ
//   auto d = t.Evaluate(entitlement, now);                   // gọi khi rảnh (không giữa lúc nhập liệu)
//   if (d.kind != Kind::None) { hiện hộp thoại; lưu t.NotifiedUntil() nếu d.kind == Kind::Expired }

#include <cstdint>

namespace ProRenewal
{
    constexpr int64_t kDay = 24 * 60 * 60;
    constexpr int64_t kRemindWindow = 7 * kDay;        // nhắc khi 0 < pro_until - now <= 7 ngày
    constexpr int64_t kReopenAfter = 30 * 60;          // quay lại sau >= 30 phút ở nền = một lần mở mới
    constexpr int64_t kCloudReadDays = 90;             // bản sao đám mây tải về được tới pro_until + 90 ngày

    // Quyền lợi đọc từ JWS (đã xác minh chữ ký ở nơi khác). present = false khi chưa từng có JWS.
    struct Entitlement
    {
        bool present = false;
        int64_t exp = 0;        // hạn của chính JWS (giây Unix, <= 7 ngày sau khi cấp)
        int64_t pro_until = 0;  // hạn Pro (giây Unix); 0 = chưa từng có Pro
    };

    enum class Kind
    {
        None,
        Renew,      // "Gia hạn TsuDEV Pro" - days_left, until
        Expired,    // "TsuDEV Pro đã hết hạn" - delete_at
    };

    struct Decision
    {
        Kind kind = Kind::None;
        int64_t days_left = 0;  // {n}: số ngày còn lại làm tròn lên, >= 1
        int64_t until = 0;      // {ngày}: pro_until
        int64_t delete_at = 0;  // {ngày_xoá}: pro_until + 90 ngày
    };

    // Số ngày còn lại làm tròn lên, tối thiểu 1 (chỉ dùng khi pro_until > now).
    inline int64_t DaysLeftCeil(int64_t pro_until, int64_t now)
    {
        const int64_t secs = pro_until - now;
        if (secs <= 0)
            return 1;
        const int64_t d = (secs + kDay - 1) / kDay;
        return d < 1 ? 1 : d;
    }

    class Tracker
    {
    public:
        explicit Tracker(int64_t notified_until = 0) : m_notified_until(notified_until) {}

        // Khởi động mới = một lần mở.
        void OnStart(int64_t /*now*/) { m_shown_this_open = false; m_background_since = -1; }
        // Cửa sổ ẩn/thu nhỏ vào khay.
        void OnBackground(int64_t now)
        {
            if (m_background_since < 0)
                m_background_since = now;
        }
        // Cửa sổ hiện lại: chỉ tính là lần mở mới khi đã ở nền >= 30 phút.
        void OnForeground(int64_t now)
        {
            if (m_background_since >= 0 && now - m_background_since >= kReopenAfter)
                m_shown_this_open = false;
            m_background_since = -1;
        }

        // Quyết định hiện gì (tối đa một thông báo mỗi lần mở). Gọi nhiều lần không hiện lặp.
        Decision Evaluate(const Entitlement& e, int64_t now)
        {
            Decision d;
            // Không có JWS, hoặc JWS hết hạn (ngoại tuyến lâu): coi như bản miễn phí, không hiện gì, không lỗi.
            if (!e.present || e.exp <= now || e.pro_until <= 0)
                return d;
            if (e.pro_until > now)
            {
                if (e.pro_until - now <= kRemindWindow && !m_shown_this_open)
                {
                    m_shown_this_open = true;
                    d.kind = Kind::Renew;
                    d.days_left = DaysLeftCeil(e.pro_until, now);
                    d.until = e.pro_until;
                }
                return d;
            }
            // Đã hết kỳ: báo đúng một lần cho mỗi giá trị pro_until.
            if (m_notified_until != e.pro_until && !m_shown_this_open)
            {
                m_shown_this_open = true;
                m_notified_until = e.pro_until;
                d.kind = Kind::Expired;
                d.until = e.pro_until;
                d.delete_at = e.pro_until + kCloudReadDays * kDay;
            }
            return d;
        }

        // Giá trị cần lưu vào khoá tsu.pro_expired.notified_until (giây Unix).
        int64_t NotifiedUntil() const { return m_notified_until; }

    private:
        int64_t m_notified_until;
        int64_t m_background_since = -1;
        bool m_shown_this_open = false;
    };
}
