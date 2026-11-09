# Kiến trúc - tsudev-trafficmonitor

## Thành phần

| Thành phần | Thư mục | Công nghệ |
| --- | --- | --- |
| Ứng dụng giám sát mạng/CPU/RAM | `TrafficMonitor/` | C++ (MFC, Win32, Direct2D), MSBuild VS 2022 v143 |
| Giao diện plugin (API v8) | `include/PluginInterface.h` | C++ thuần ảo, DLL nạp từ thư mục `plugins` |
| Plugin mẫu | `PluginDemo/` | C++/MFC DLL |
| Đo nhiệt độ (chỉ bản đầy đủ, không phát hành) | `OpenHardwareMonitorApi/` | C++/CLI + LibreHardwareMonitor (MPL-2.0) |
| Website sản phẩm | `website/` | Astro (tĩnh), Cloudflare Pages |
| Công cụ kiểm và sinh tài sản | `tools/`, `scripts/` | Python 3, Bash, PowerShell |

## Mạng

Ứng dụng chỉ gọi mạng ở hai việc, mọi URL khai trong `TrafficMonitor/ForkConfig.h`:

1. Kiểm tra cập nhật: tải `version_signed.xml` + chữ ký từ Releases của repo, xác minh ECDSA P-256 (Windows CNG) bằng khóa công khai nhúng sẵn.
2. Tra IP công cộng khi người dùng mở Chi tiết mạng: `api.ipify.org`, `api6.ipify.org`.

`GetURL` chỉ nhận HTTPS, có timeout, giới hạn 1 MB, URL cuối sau chuyển hướng phải thuộc `FORK_TRUSTED_HOSTS`.

## Phân loại dữ liệu (SECURITY_BASELINE mục 2)

| Dữ liệu | Mức | Nơi lưu | Thời hạn |
| --- | --- | --- | --- |
| Cấu hình (`config.ini`), lịch sử lưu lượng | D1 | Máy người dùng (thư mục chương trình hoặc `%AppData%\TrafficMonitor`) | Do người dùng xóa |
| Tên card mạng, IP, MAC (hiển thị) | D1 | Chỉ trong bộ nhớ, không ghi ra ngoài | Theo phiên chạy |

Không có tài khoản, không thu thập, không gửi dữ liệu người dùng đi đâu.

## Chuỗi tin cậy bản phát hành

Tag có chữ ký SSH → `release.yml` (duyệt environment `release`) build → `SHA256SUMS.txt` + chứng thực build (attest-build-provenance) + `version_signed.xml` ký bằng secret `UPDATE_SIGNING_KEY`. Chi tiết: [SECURITY.md](../SECURITY.md).
