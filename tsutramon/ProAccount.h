#pragma once
// Tài khoản tsudev trong Tsu Tramon (P9 phần 3): nối logic thuần ProAuth.h và tầng Windows ProNet.h thành luồng
// đăng nhập / làm mới quyền lợi / đăng xuất chạy trên MỘT luồng nền. KHÔNG phụ thuộc MFC (ProAccount.cpp biên dịch
// không dùng PCH vì ProNet.h cần winsock2.h trước windows.h). Giao diện chỉ đọc Status() và gửi lệnh.
//
// Quy tắc (AUTH_AND_ACCOUNT 14.1-14.3, PRODUCT_POLICY 7.4, ADR-0002):
//  - Chưa đăng nhập => KHÔNG gọi mạng. Chế độ ngoại tuyến hoàn toàn => không gọi mạng kể cả khi đã đăng nhập.
//  - Refresh token chỉ trong Credential Manager; JWS quyền lợi + JWKS đệm thành tệp trong thư mục cấu hình
//    (kiểm lại chữ ký khi nạp; JWS quá exp <= 7 ngày => coi như miễn phí).
//  - Mã cài đặt chỉ gửi trong header X-Tsu-Installation khi đã đăng nhập.
//  - Các phụ thuộc ra ngoài (HTTPS, mở trình duyệt, kiểm ES256, kho bí mật, đồng hồ) tiêm được để kiểm thử cả luồng
//    với máy chủ giả (tests/ProAccountTests.cpp).

#include "ProAuth.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

struct ProHttpResult
{
    bool ok = false;        // đọc trọn phản hồi HTTP (mọi mã trạng thái)
    unsigned status = 0;
    std::string body;
};

struct ProAccountDeps
{
    // method L"GET"/L"POST"; headers: các dòng "Tên: giá trị\r\n"; body rỗng = không có thân
    std::function<ProHttpResult(const wchar_t* method, const std::string& url, const std::wstring& headers, const std::string& body)> http;
    std::function<bool(const std::string& url)> open_browser;     // mở trình duyệt hệ thống (KHÔNG WebView)
    ProAuth::Es256Verifier verify;
    std::function<bool(const std::string&)> save_secret;
    std::function<bool(std::string&)> load_secret;
    std::function<bool()> delete_secret;
    std::function<int64_t()> now;                                 // giây Unix
    int login_timeout_ms = 180 * 1000;                            // chờ trình duyệt quay về
    int first_refresh_delay_ms = 15 * 1000;                       // làm mới lần đầu sau khi khởi động
    int refresh_interval_ms = 12 * 60 * 60 * 1000;                // làm mới định kỳ khi đã đăng nhập
    int retry_interval_ms = 60 * 60 * 1000;                       // thử lại sau lỗi mạng

    static ProAccountDeps Default();                              // Windows thật: ProNet + ShellExecute + BCrypt
};

class CProAccount
{
public:
    enum class Op { None, SigningIn, Refreshing, SigningOut };
    enum class Error
    {
        None,
        Offline,          // đang bật Chế độ ngoại tuyến hoàn toàn
        Listener,         // không mở được cổng 127.0.0.1
        Browser,          // không mở được trình duyệt
        Cancelled,        // người dùng huỷ / hết thời gian chờ
        Denied,           // người dùng từ chối trên tsudev.com (error=access_denied)
        BadCallback,      // state/iss sai - có thể là tiến trình khác gọi cổng loopback
        Network,          // không kết nối được tsudev.com
        Server,           // mã HTTP lạ hoặc thân phản hồi hỏng
        InvalidToken,     // id_token/JWS sai chữ ký, iss, aud, nonce, hạn...
        SessionExpired,   // refresh token bị thu hồi/hết hạn => đã về chế độ khách
        Storage,          // không ghi được Credential Manager
    };

    struct Status
    {
        bool signed_in = false;              // có refresh token trong Credential Manager
        Op op = Op::None;
        Error last_error = Error::None;
        bool has_entitlement = false;        // có JWS đã kiểm chữ ký, chưa quá exp
        ProAuth::Entitlement entitlement;
        int64_t last_online_ok = 0;          // lần cuối lấy JWS trực tuyến thành công (giây Unix, 0 = chưa)
        uint64_t revision = 0;               // tăng mỗi lần trạng thái đổi
    };

    // config_dir kết thúc bằng '\'; notify_wnd nhận PostMessage(notify_msg) mỗi khi trạng thái đổi
    CProAccount(ProAccountDeps deps = ProAccountDeps::Default());
    ~CProAccount();
    CProAccount(const CProAccount&) = delete;
    CProAccount& operator=(const CProAccount&) = delete;

    void Init(const std::wstring& config_dir, const std::string& installation_id, bool offline, void* notify_wnd = nullptr, unsigned notify_msg = 0);
    void SetOffline(bool offline);
    void SetInstallationId(const std::string& installation_id);

    void StartSignIn();         // mở trình duyệt, chờ callback (huỷ được)
    void CancelSignIn();
    void RefreshNow();          // chỉ khi đã đăng nhập và không ngoại tuyến
    // Đăng xuất và xoá dữ liệu cục bộ: huỷ lượt đăng nhập đang chờ, thu hồi refresh token (nếu được phép gọi mạng),
    // xoá Credential Manager + JWS/JWKS đệm. Trạng thái cục bộ về "chưa đăng nhập" NGAY khi gọi.
    void SignOutAndClear();
    // Chờ luồng nền dừng tối đa wait_ms; quá hạn thì tách luồng (nó chỉ giữ dữ liệu chung qua shared_ptr).
    void Shutdown(int wait_ms = 2000);

    Status GetStatus() const;
    // Đầu vào cho ProRenewal::Tracker
    bool RenewalInput(int64_t& exp, int64_t& pro_until) const;

    // Tên tệp đệm trong thư mục cấu hình (để thử nghiệm/kiểm thử)
    static constexpr const wchar_t* kEntitlementFile = L"tsudev_entitlement.jws";
    static constexpr const wchar_t* kJwksFile = L"tsudev_jwks.json";

    struct Impl;
private:
    std::shared_ptr<Impl> m_impl;
};
