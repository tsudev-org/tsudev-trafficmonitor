#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////
// ForkConfig.h - Cấu hình tập trung cho bản Việt hóa tsudev-trafficmonitor
//
// Mọi URL, tên repo và giá trị mặc định riêng của bản Việt hóa được khai báo tại đây,
// để khi cần đổi repo/tổ chức chỉ phải sửa một chỗ duy nhất.
//
// Giấy phép: "Anti 996" License - xem tệp LICENSE ở thư mục gốc.
////////////////////////////////////////////////////////////////////////////////////////////////////

// Repo GitHub của bản Việt hóa (từ V1.87-vn chuyển sang repo tsudev-trafficmonitor; repo cũ tsudev-trafficmonitor lưu trữ)
#define FORK_GITHUB_OWNER           L"tsudev-org"
#define FORK_GITHUB_REPO            L"tsutramon"
#define FORK_GITHUB_BRANCH          L"main"
#define FORK_REPO_URL               L"https://github.com/tsudev-org/tsutramon"

// Tệp thông tin phiên bản dùng để kiểm tra cập nhật (định dạng XML, xem version_utf8.info)
#define FORK_VERSION_INFO_RAW_URL   L"https://raw.githubusercontent.com/tsudev-org/tsutramon/main/version_utf8.info"

// Tiền tố bắt buộc của mọi liên kết tải bản cập nhật.
// Bản cập nhật chỉ được chấp nhận khi liên kết tải trỏ đúng vào trang Releases của repo này,
// nhằm ngăn tệp thông tin phiên bản bị giả mạo chuyển hướng người dùng tới tệp độc hại.
#define FORK_RELEASE_DOWNLOAD_PREFIX L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/"
// Repo chuyển sang Organization tsudev-org (11/2026); GitHub chuyển hướng URL cũ sang repo mới.
// Chấp nhận cả hai tiền tố. release.yml vẫn ghi liên kết theo owner cũ (tsudev-tsudev) vào version_signed.xml
// để các bản đã cài (chỉ tin tiền tố cũ) vẫn cập nhật được. KHÔNG tạo lại repo tsudev-tsudev/tsudev-trafficmonitor:
// GitHub sẽ bỏ chuyển hướng và mọi bản đã cài mất kênh cập nhật.
#define FORK_RELEASE_DOWNLOAD_PREFIX_ORG L"https://github.com/tsudev-org/tsudev-trafficmonitor/releases/download/"
//Tên repo sau khi đổi thành tsutramon (ADR-0001 mục 3.1, 3.8): bản từ 26.11.2401 tin thêm tiền tố này để manifest sau
//này có thể trỏ thẳng tên mới. Manifest vẫn PHẢI ghi tiền tố tsudev-tsudev/tsudev-trafficmonitor cho bản <= 26.11.1202.
#define FORK_RELEASE_DOWNLOAD_PREFIX_NEW L"https://github.com/tsudev-org/tsutramon/releases/download/"

// Thông tin phiên bản CÓ CHỮ KÝ (từ V1.88-vn): release.yml tạo version_signed.xml (phiên bản, liên kết tải,
// SHA-256 từng gói) và ký bằng ECDSA P-256/SHA-256; khóa bí mật chỉ nằm trong secret UPDATE_SIGNING_KEY
// của environment "release". Ứng dụng chỉ tin thông tin phiên bản khi chữ ký hợp lệ với khóa công khai dưới đây.
// "releases/latest/download" luôn trỏ bản phát hành mới nhất (không tính bản thử nghiệm).
// Từ 26.11.2501 trỏ thẳng tên repo mới (ADR-0001 mục 3.1); bản đã cài vẫn gọi tên cũ, chạy nhờ chuyển hướng của GitHub.
#define FORK_SIGNED_VERSION_URL     L"https://github.com/tsudev-org/tsutramon/releases/latest/download/version_signed.xml"
#define FORK_SIGNED_VERSION_SIG_URL L"https://github.com/tsudev-org/tsutramon/releases/latest/download/version_signed.xml.sig"
// Khóa công khai ECDSA P-256 (X||Y, 64 byte, dạng hex); bản PEM: tools/keys/update_signing_pub.pem
#define FORK_UPDATE_PUBLIC_KEY_HEX  "d8725c096fbcac2420427ef8b8d40dee09bd75ea0edd33cf6cbdee96d4f69752045a880fdebdcc42076b01b2d8acb82ab65457807fadc241a6802774e561c685"

// Máy chủ được phép trả dữ liệu cho GetURL sau khi theo chuyển hướng (redirect).
// URL cuối cùng phải là HTTPS và có host nằm trong danh sách này, phân tách bằng dấu "|".
#define FORK_TRUSTED_HOSTS          L"github.com|objects.githubusercontent.com|release-assets.githubusercontent.com|raw.githubusercontent.com|tramon.tsudev.com|trafficmonitor.tsudev.com|api.ipify.org|api6.ipify.org"
// Máy chủ được phép ở MỌI bước chuyển hướng khi ứng dụng tự tải gói cập nhật (CUpdateHelper::DownloadPackage).
// Hẹp hơn FORK_TRUSTED_HOSTS: chỉ trang Releases của GitHub và máy chủ lưu tệp đính kèm của GitHub.
#define FORK_UPDATE_DOWNLOAD_HOSTS  L"github.com|objects.githubusercontent.com|release-assets.githubusercontent.com"

// Các trang tài liệu (website trafficmonitor.tsudev.com, sinh từ các tệp .md trong repo;
// bản .md trên GitHub vẫn giữ làm dự phòng). Ngôn ngữ khác tiếng Việt dùng tài liệu tiếng Anh (*_EN_URL) trên GitHub,
// chọn qua CTsudevTrafficMonitorApp::DocUrl().
#define FORK_HELP_URL               L"https://tramon.tsudev.com/huong-dan/hoi-dap/"
#define FORK_HELP_EN_URL            L"https://github.com/tsudev-org/tsutramon/blob/main/Help_en-us.md"
#define FORK_UPDATE_LOG_URL         L"https://tramon.tsudev.com/nhat-ky/"
#define FORK_UPDATE_LOG_EN_URL      L"https://github.com/tsudev-org/tsutramon/blob/main/UpdateLog/update_log_en-us.md"
#define FORK_WIKI_URL               L"https://tramon.tsudev.com/huong-dan/"
#define FORK_WIKI_EN_URL            L"https://github.com/tsudev-org/tsutramon/blob/main/README_en-us.md"
#define FORK_SKIN_TUTORIAL_URL      L"https://tramon.tsudev.com/huong-dan/tu-lam-giao-dien/"
#define FORK_SKIN_TUTORIAL_EN_URL   L"https://github.com/tsudev-org/tsutramon/blob/main/SkinTutorial_en-us.md"
#define FORK_SKIN_DOWNLOAD_URL      L"https://tramon.tsudev.com/huong-dan/kho-giao-dien/"
#define FORK_SKIN_DOWNLOAD_EN_URL   L"https://github.com/tsudev-org/tsutramon/blob/main/SkinGallery_en-us.md"
#define FORK_PLUGIN_DOWNLOAD_URL    L"https://tramon.tsudev.com/huong-dan/plugin/"
#define FORK_PLUGIN_DOWNLOAD_EN_URL L"https://github.com/tsudev-org/tsutramon/blob/main/Plugin_en-us.md"
#define FORK_PLUGIN_DEV_GUIDE_URL   L"https://tramon.tsudev.com/huong-dan/phat-trien-plugin/"
#define FORK_PLUGIN_DEV_GUIDE_EN_URL L"https://github.com/tsudev-org/tsutramon/blob/main/PluginDev_en-us.md"

// Hệ sinh thái tsudev (ECOSYSTEM_IDENTITY mục 2.3: hộp thoại Giới thiệu có liên kết tới tsudev.com)
#define TSUDEV_HOME_URL             L"https://tsudev.com"
// Trang Ủng hộ của sản phẩm (phiếu 20261112-01; dữ liệu website/src/data/ung-ho.json): ứng dụng chỉ mở trang,
// không nhúng thông tin chuyển khoản - đổi kênh không cần phát hành lại ứng dụng.
#define TSUDEV_DONATE_URL           L"https://tramon.tsudev.com/ung-ho/"
#define FORK_PRODUCT_URL            L"https://tramon.tsudev.com"

// Ghi công thư viện bên thứ ba (hộp thoại Giới thiệu)
#define THIRD_PARTY_LIBREHARDWAREMONITOR_URL L"https://github.com/LibreHardwareMonitor/LibreHardwareMonitor"
#define THIRD_PARTY_TINYXML2_URL    L"https://github.com/leethomason/tinyxml2"

// Dịch vụ tra cứu IP công cộng (chỉ trả về IP, chỉ gọi khi người dùng yêu cầu)
#define PUBLIC_IP_V4_URL            L"https://api.ipify.org/?format=json"
#define PUBLIC_IP_V6_URL            L"https://api6.ipify.org/?format=json"

// Ngôn ngữ mặc định khi chưa có cấu hình (lần chạy đầu tiên).
// Định dạng: "<BCP-47>|<DISPLAY_NAME>|<TRANSLATOR>" - phải khớp với phần [general] của language\Vietnamese.ini
#define DEFAULT_LANGUAGE_CONFIG     L"vi-VN|Tiếng Việt|tsudev"

// Đăng nhập tài khoản tsudev và quyền lợi TsuDEV Pro (P9, ProAuth.h; AUTH_AND_ACCOUNT mục 4, PRODUCT_POLICY mục 7.4).
// Chép từ discovery https://tsudev.com/oidc/.well-known/openid-configuration (đo 25/11/2026) và GHI CỨNG: discovery
// bị giả mạo không đổi được điểm cuối token/JWKS. Chuỗi hẹp (UTF-8) vì dùng trong URL/JSON, không qua giao diện.
#define FORK_TSUDEV_OIDC_ISSUER         "https://tsudev.com/oidc"
#define FORK_TSUDEV_OIDC_AUTHORIZE_URL  "https://tsudev.com/oidc/auth"
#define FORK_TSUDEV_OIDC_TOKEN_URL      "https://tsudev.com/oidc/token"
#define FORK_TSUDEV_OIDC_REVOKE_URL     "https://tsudev.com/oidc/token/revocation"
#define FORK_TSUDEV_OIDC_JWKS_URL       "https://tsudev.com/oidc/jwks"
#define FORK_TSUDEV_ENTITLEMENTS_URL    "https://tsudev.com/api/me/entitlements"
// Trang mua/gia hạn Pro cho bản ngoài cửa hàng (PRODUCT_POLICY 7.3 "Kênh"), nối thêm client_id
#define FORK_TSUDEV_PRO_URL_PREFIX      "https://tsudev.com/pro?app="
// Cổng quay về loopback của app desktop (AUTH mục 4): http://127.0.0.1:<cổng ngẫu nhiên>/callback
#define FORK_TSUDEV_LOOPBACK_PREFIX     "http://127.0.0.1:"
