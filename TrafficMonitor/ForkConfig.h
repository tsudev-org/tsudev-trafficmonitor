#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////
// ForkConfig.h — Cấu hình tập trung cho bản Việt hóa TrafficMonitor-VN
//
// Mọi URL, tên repo và giá trị mặc định riêng của bản Việt hóa được khai báo tại đây,
// để khi cần đổi repo/tổ chức chỉ phải sửa một chỗ duy nhất.
//
// Dự án gốc: https://github.com/zhongyang219/TrafficMonitor (tác giả zhongyang219)
// Giấy phép: "Anti 996" License — xem tệp LICENSE ở thư mục gốc.
////////////////////////////////////////////////////////////////////////////////////////////////////

// Repo GitHub của bản Việt hóa (từ V1.87-vn chuyển sang repo tsudev-trafficmonitor; repo cũ TrafficMonitor-VN lưu trữ)
#define FORK_GITHUB_OWNER           L"tsudev-tsudev"
#define FORK_GITHUB_REPO            L"tsudev-trafficmonitor"
#define FORK_GITHUB_BRANCH          L"main"
#define FORK_REPO_URL               L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor"

// Tệp thông tin phiên bản dùng để kiểm tra cập nhật (định dạng XML, xem version_utf8.info)
#define FORK_VERSION_INFO_RAW_URL   L"https://raw.githubusercontent.com/tsudev-tsudev/tsudev-trafficmonitor/main/version_utf8.info"

// Tiền tố bắt buộc của mọi liên kết tải bản cập nhật.
// Bản cập nhật chỉ được chấp nhận khi liên kết tải trỏ đúng vào trang Releases của repo này,
// nhằm ngăn tệp thông tin phiên bản bị giả mạo chuyển hướng người dùng tới tệp độc hại.
#define FORK_RELEASE_DOWNLOAD_PREFIX L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/"

// Thông tin phiên bản CÓ CHỮ KÝ (từ V1.88-vn): release.yml tạo version_signed.xml (phiên bản, liên kết tải,
// SHA-256 từng gói) và ký bằng ECDSA P-256/SHA-256; khóa bí mật chỉ nằm trong secret UPDATE_SIGNING_KEY
// của environment "release". Ứng dụng chỉ tin thông tin phiên bản khi chữ ký hợp lệ với khóa công khai dưới đây.
// "releases/latest/download" luôn trỏ bản phát hành mới nhất (không tính bản thử nghiệm).
#define FORK_SIGNED_VERSION_URL     L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/latest/download/version_signed.xml"
#define FORK_SIGNED_VERSION_SIG_URL L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/latest/download/version_signed.xml.sig"
// Khóa công khai ECDSA P-256 (X||Y, 64 byte, dạng hex); bản PEM: tools/keys/update_signing_pub.pem
#define FORK_UPDATE_PUBLIC_KEY_HEX  "d8725c096fbcac2420427ef8b8d40dee09bd75ea0edd33cf6cbdee96d4f69752045a880fdebdcc42076b01b2d8acb82ab65457807fadc241a6802774e561c685"

// Máy chủ được phép trả dữ liệu cho GetURL sau khi theo chuyển hướng (redirect).
// URL cuối cùng phải là HTTPS và có host nằm trong danh sách này, phân tách bằng dấu "|".
#define FORK_TRUSTED_HOSTS          L"github.com|objects.githubusercontent.com|release-assets.githubusercontent.com|raw.githubusercontent.com|trafficmonitor.tsudev.com|api.ipify.org|api6.ipify.org"

// Các trang tài liệu (website trafficmonitor.tsudev.com, sinh từ các tệp .md trong repo;
// bản .md trên GitHub vẫn giữ làm dự phòng). Ngôn ngữ khác tiếng Việt dùng tài liệu tiếng Anh trên GitHub.
#define FORK_HELP_URL               L"https://trafficmonitor.tsudev.com/huong-dan/hoi-dap/"
#define FORK_HELP_EN_URL            L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/blob/main/Help_en-us.md"
#define FORK_UPDATE_LOG_URL         L"https://trafficmonitor.tsudev.com/nhat-ky/"
#define FORK_UPDATE_LOG_EN_URL      L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/blob/main/UpdateLog/update_log_en-us.md"
#define FORK_WIKI_URL               L"https://trafficmonitor.tsudev.com/huong-dan/"
#define FORK_SKIN_TUTORIAL_URL      L"https://trafficmonitor.tsudev.com/huong-dan/tu-lam-giao-dien/"
#define FORK_SKIN_DOWNLOAD_URL      L"https://trafficmonitor.tsudev.com/huong-dan/kho-giao-dien/"
#define FORK_PLUGIN_DOWNLOAD_URL    L"https://trafficmonitor.tsudev.com/huong-dan/plugin/"
#define FORK_PLUGIN_DEV_GUIDE_URL   L"https://trafficmonitor.tsudev.com/huong-dan/phat-trien-plugin/"

// Hệ sinh thái tsudev (ECOSYSTEM_IDENTITY mục 2.3: hộp thoại Giới thiệu có liên kết tới tsudev.com)
#define TSUDEV_HOME_URL             L"https://tsudev.com"
#define FORK_PRODUCT_URL            L"https://trafficmonitor.tsudev.com"

// Repo gốc (giữ để ghi công tác giả theo yêu cầu giấy phép)
#define UPSTREAM_REPO_URL           L"https://github.com/zhongyang219/TrafficMonitor"

// Thông tin phiên bản mới nhất của các plugin do tác giả gốc phát hành (TrafficMonitorPlugins).
// Các plugin này do dự án gốc phát triển nên phiên bản được tra trực tiếp từ nguồn phát hành của chúng (HTTPS).
#define UPSTREAM_PLUGINS_VERSION_URL L"https://raw.githubusercontent.com/zhongyang219/TrafficMonitorPlugins/main/plugins_version.xml"

// Ghi công thư viện bên thứ ba (hộp thoại Giới thiệu)
#define THIRD_PARTY_LIBREHARDWAREMONITOR_URL L"https://github.com/LibreHardwareMonitor/LibreHardwareMonitor"
#define THIRD_PARTY_TINYXML2_URL    L"https://github.com/leethomason/tinyxml2"

// Dịch vụ tra cứu IP công cộng (chỉ trả về IP, chỉ gọi khi người dùng yêu cầu)
#define PUBLIC_IP_V4_URL            L"https://api.ipify.org/?format=json"
#define PUBLIC_IP_V6_URL            L"https://api6.ipify.org/?format=json"

// Ngôn ngữ mặc định khi chưa có cấu hình (lần chạy đầu tiên).
// Định dạng: "<BCP-47>|<DISPLAY_NAME>|<TRANSLATOR>" — phải khớp với phần [general] của language\Vietnamese.ini
#define DEFAULT_LANGUAGE_CONFIG     L"vi-VN|Tiếng Việt|Lieutrung"
