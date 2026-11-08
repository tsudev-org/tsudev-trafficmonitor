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

// Các trang tài liệu
#define FORK_HELP_URL               L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/blob/main/Help.md"
#define FORK_UPDATE_LOG_URL         L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/blob/main/UpdateLog/update_log_vi.md"
#define FORK_WIKI_URL               L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor#readme"
#define FORK_SKIN_TUTORIAL_URL      L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/blob/main/HuongDanTaoSkin.md"
#define FORK_SKIN_DOWNLOAD_URL      L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/blob/main/KhoGiaoDien.md"
#define FORK_PLUGIN_DOWNLOAD_URL    L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/blob/main/Plugin.md"
#define FORK_PLUGIN_DEV_GUIDE_URL   L"https://github.com/tsudev-tsudev/tsudev-trafficmonitor/blob/main/PhatTrienPlugin.md"

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
