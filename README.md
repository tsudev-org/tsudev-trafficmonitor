**Tiếng Việt | [English](./README_en-us.md)**

[![Build](https://img.shields.io/github/actions/workflow/status/tsudev-tsudev/TrafficMonitor-VN/build.yml?branch=main&label=Build&logo=github&style=flat-square)](https://github.com/tsudev-tsudev/TrafficMonitor-VN/actions/workflows/build.yml)
[![CodeQL](https://img.shields.io/github/actions/workflow/status/tsudev-tsudev/TrafficMonitor-VN/codeql.yml?branch=main&label=CodeQL&logo=github&style=flat-square)](https://github.com/tsudev-tsudev/TrafficMonitor-VN/actions/workflows/codeql.yml)
[![Release](https://img.shields.io/github/v/release/tsudev-tsudev/TrafficMonitor-VN?style=flat-square)](https://github.com/tsudev-tsudev/TrafficMonitor-VN/releases/latest)
[![License](https://img.shields.io/badge/license-Anti%20996-blue.svg?style=flat-square)](./LICENSE)

# TrafficMonitor-VN

**TrafficMonitor-VN** là bản Việt hóa và gia cố bảo mật của [TrafficMonitor](https://github.com/zhongyang219/TrafficMonitor) — phần mềm giám sát tốc độ mạng, mức sử dụng CPU/RAM/GPU/ổ cứng trên Windows với cửa sổ nổi và hiển thị trực tiếp trên thanh tác vụ (Taskbar).

> Dự án gốc do **zhongyang219** phát triển, phát hành theo giấy phép **Anti 996**. TrafficMonitor-VN giữ nguyên giấy phép và thông báo bản quyền của tác giả gốc.

## Điểm khác biệt so với bản gốc

### Việt hóa
* **Giao diện tiếng Việt mặc định** ngay lần chạy đầu tiên (vẫn chọn được ngôn ngữ khác hoặc “Theo hệ thống” trong **Cài đặt Chung**).
* Gói ngôn ngữ tiếng Việt **đầy đủ 100%** chuỗi giao diện, được **nhúng sẵn** trong tệp chương trình (không cần thư mục `language`).
* Hộp thoại chọn màu, plugin mẫu, tài liệu Hỏi đáp, hướng dẫn tạo skin, mẫu báo lỗi... đều bằng tiếng Việt.
* Toàn bộ chú thích mã nguồn (≈2.800 đoạn trong 186 tệp) được dịch sang tiếng Việt, thuận tiện cho lập trình viên Việt Nam phát triển tiếp.

### Bảo mật
| Hạng mục | Bản gốc | TrafficMonitor-VN |
|---|---|---|
| Nguồn cập nhật | GitHub hoặc Gitee | Chỉ repo TrafficMonitor-VN trên GitHub |
| Kiểm tra liên kết tải bản cập nhật | Chấp nhận mọi đường dẫn `github.com/`, `gitee.com/` | Chỉ chấp nhận trang Releases của repo này, chặn `..`, `@`, `?`, `#`, `%`, chỉ `.zip`/`.exe` |
| So sánh phiên bản | So sánh chuỗi (sai khi `1.100` vs `1.86`) | So sánh theo số |
| Giao thức tải dữ liệu | Mọi giao thức `OpenURL` hỗ trợ (kể cả `file://`, `ftp://`) | **Chỉ HTTPS**, có timeout, giới hạn 1 MB, không cache/cookie |
| Tra cứu IP công cộng | Dịch vụ bên thứ ba ở nước ngoài, có gửi vị trí | [ipify](https://www.ipify.org/) (mã nguồn mở, chỉ trả IP), chỉ khi người dùng bấm |
| Nạp DLL / plugin | Thứ tự tìm DLL mặc định | Chống DLL hijacking (`SetDefaultDllDirectories`, `LoadLibraryEx` an toàn) |
| Cờ biên dịch | Mặc định | `/guard:cf`, `/sdl`, `/GS`, ASLR, DEP, CET Shadow Stack |
| CI/CD | Build khi gắn tag | Build mỗi lần push/PR, CodeQL quét lỗ hổng, Dependabot, SHA-256 cho bản phát hành |

Chi tiết xem [SECURITY.md](./SECURITY.md).

## Tải về

* Bản phát hành: [Releases](https://github.com/tsudev-tsudev/TrafficMonitor-VN/releases/latest). Mỗi bản phát hành kèm tệp `SHA256SUMS.txt` — hãy đối chiếu mã băm trước khi chạy (xem [SECURITY.md](./SECURITY.md#kiểm-tra-tính-toàn-vẹn-tệp-tải-về)).
* Bản build thử nghiệm: tab [Actions](https://github.com/tsudev-tsudev/TrafficMonitor-VN/actions/workflows/build.yml) (cần đăng nhập GitHub).

TrafficMonitor-VN **chỉ** được phát hành qua repo GitHub trên. Mọi trang web khác tự xưng là “trang chính thức” đều không liên quan.

Chương trình cần **Microsoft Visual C++ Redistributable**. Nếu khi chạy báo thiếu `MSVC*.dll`/`mfc*.dll`, hãy cài từ trang của Microsoft: [Latest supported Visual C++ Redistributable](https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist).

## Phiên bản

TrafficMonitor-VN phát hành bản **Lite** (không cần quyền Quản trị viên). Từ bản gốc 1.86, bản Lite đã có giám sát GPU và ổ cứng; chỉ thiếu đo nhiệt độ. Nhu cầu đo nhiệt độ/quạt/công suất nên dùng plugin [Hardware Monitor](https://github.com/zhongyang219/TrafficMonitorPlugins/blob/main/download/plugin_download_en.md) của tác giả gốc.

| Tính năng | Bản đầy đủ (tự build) | Bản Lite |
|---|---|---|
| Tốc độ mạng | ✔ | ✔ |
| Mức dùng CPU, RAM | ✔ | ✔ |
| Tần số CPU, mức dùng GPU, ổ cứng | ✔ | ✔ |
| Nhiệt độ CPU/GPU/ổ cứng/bo mạch chủ | ✔ | ❌ |
| Chi tiết kết nối mạng, thống kê lưu lượng | ✔ | ✔ |
| Skin, plugin | ✔ | ✔ |
| Cần quyền Quản trị viên | Có | Không |

## Tính năng chính

* Hiển thị tốc độ tải lên/tải xuống, mức sử dụng CPU và bộ nhớ.
* Tự động hoặc chọn thủ công card mạng khi máy có nhiều kết nối.
* Xem chi tiết kết nối mạng.
* Hiển thị nhúng trên thanh tác vụ (hỗ trợ Windows 11, nhiều màn hình).
* Đổi skin ([kho giao diện](./KhoGiaoDien.md)), tự tạo skin ([hướng dẫn](./HuongDanTaoSkin.md)).
* Thống kê lưu lượng theo ngày/tuần/tháng/quý/năm.
* Hệ thống plugin.

## Cách sử dụng

Khi khởi động, một **cửa sổ nổi** hiển thị tốc độ mạng xuất hiện trên màn hình. Nhấp chuột phải vào cửa sổ để mở menu.

* **Hiển thị trên thanh tác vụ:** menu chuột phải → **Hiện ở dưới Taskbar**.
* **Chọn mục hiển thị trên thanh tác vụ:** nhấp chuột phải vào cửa sổ Taskbar → **Sắp xếp hiển thị...**.
* **Hiện CPU/RAM trên cửa sổ nổi:** menu chuột phải → **Hiện thêm thông số**.
* **Đổi skin:** menu chuột phải → **Chức năng khác** → **Đổi giao diện...**.
* **Tùy chọn:** menu chuột phải → **Cài đặt...** (màu chữ, phông chữ, đơn vị tốc độ, cảnh báo lưu lượng, tự khởi động...).

Gặp sự cố? Xem [Câu hỏi thường gặp](./Help.md).

## Ảnh chụp màn hình

Cửa sổ nổi (skin mặc định):

![Cửa sổ nổi hiện tốc độ tải lên và tải xuống](./Screenshots/vi/main.png)

Cửa sổ trên thanh tác vụ (Windows 11):

![Tốc độ mạng trên thanh tác vụ](./Screenshots/vi/taskbar.png)

Cài đặt:

![Hộp thoại Cài đặt, tab Cửa sổ Chính](./Screenshots/vi/options.png)

Chi tiết mạng (nhấp đúp vào cửa sổ nổi; địa chỉ MAC, IP và tên card trong ảnh đã được che):

![Hộp thoại Chi tiết mạng](./Screenshots/vi/network_info.png)

## Plugin

Plugin là tệp `.dll` đặt trong thư mục `plugins` cạnh `TrafficMonitor.exe`, được nạp khi chương trình khởi động; quản lý tại **Cài đặt Chung** → **Nâng cao** → **Quản lý Plugin...**.

* Cài plugin và nguồn plugin: [Plugin](./Plugin.md).
* Phát triển plugin: [Phát triển plugin](./PhatTrienPlugin.md); giao diện lập trình (đã chú thích tiếng Việt) ở [`include/PluginInterface.h`](./include/PluginInterface.h), ví dụ ở [`PluginDemo`](./PluginDemo).

> ⚠️ Plugin chạy **cùng quyền với TrafficMonitor**. Chỉ cài plugin từ nguồn tin cậy.

## Giám sát phần cứng

Tính năng đo nhiệt độ (chỉ có ở bản đầy đủ) dựa trên thư viện [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor) và **tắt theo mặc định**. Bật tại **Cài đặt Chung** → **Theo dõi Phần cứng**.

**Lưu ý:** tính năng này có thể làm tăng mức dùng CPU/RAM; một số máy gặp lỗi đóng chương trình đột ngột hoặc treo máy. Chỉ bật khi bạn chấp nhận các rủi ro này.

## Build từ mã nguồn

Yêu cầu: Windows 10/11, **Visual Studio 2022** với workload *Desktop development with C++*, thành phần **C++ MFC for latest v143 build tools** (x86 & x64, và ARM64EC nếu cần), cùng **.NET Framework 4.7.2 targeting pack** (cho bản đầy đủ).

```bat
:: Bản Lite (khuyến nghị)
msbuild TrafficMonitor_Lite.sln -p:Configuration="Release (lite)" -p:Platform=x64

:: Bản đầy đủ (có đo nhiệt độ)
msbuild TrafficMonitor.sln -p:Configuration=Release -p:Platform=x64
```

Tệp kết quả nằm trong `Bin\x64\Release (lite)\` (hoặc `Bin\x64\Release\`).

Cấu hình riêng của bản Việt hóa (repo, URL cập nhật, ngôn ngữ mặc định) tập trung tại [`TrafficMonitor/ForkConfig.h`](./TrafficMonitor/ForkConfig.h). Cờ biên dịch bảo mật nằm ở [`Directory.Build.props`](./Directory.Build.props).

### Đồng bộ với dự án gốc

Lịch sử commit của TrafficMonitor-VN đã được viết lại (tiêu đề commit dịch sang tiếng Việt), nên không còn chung lịch sử với dự án gốc và không thể `git merge` trực tiếp. Để lấy bản sửa mới từ dự án gốc, hãy áp dụng từng thay đổi bằng bản vá:

```bash
git remote add upstream https://github.com/zhongyang219/TrafficMonitor.git   # chỉ cần một lần
git fetch upstream
git diff <commit-cũ-đã-đồng-bộ> upstream/master > upstream.patch
git apply --3way upstream.patch
```

Commit dự án gốc đã đồng bộ gần nhất: `930f175` (v1.86). Vì chú thích mã nguồn đã được dịch, khi áp dụng có thể phát sinh xung đột ở các dòng chú thích: hãy giữ phần **mã** của bản gốc và phần **chú thích** tiếng Việt.

## Đóng góp

* Báo lỗi / đề xuất: [Issues](https://github.com/tsudev-tsudev/TrafficMonitor-VN/issues/new/choose).
* Lỗ hổng bảo mật: **không** đăng công khai, xem [SECURITY.md](./SECURITY.md).
* Nhật ký thay đổi: [UpdateLog/update_log_vi.md](./UpdateLog/update_log_vi.md).

## Giấy phép

Phát hành theo giấy phép **"Anti 996" License 1.0** — xem [LICENSE](./LICENSE) (bản tiếng Anh) và [LICENSE_CN](./LICENSE_CN) (bản tiếng Trung). Bản dịch tiếng Việt chỉ để tham khảo: [LICENSE_VI.md](./LICENSE_VI.md).

Copyright (c) 2017-2026 zhongyang219 (dự án gốc — bắt buộc giữ theo điều 1 của giấy phép).
Phần Việt hóa, gia cố bảo mật và các thay đổi của TrafficMonitor-VN: Copyright (c) 2026 tsudev-tsudev. Chi tiết phân định bản quyền xem [NOTICE.md](./NOTICE.md).
