# Chính sách bảo mật — TrafficMonitor-VN

## Phiên bản được hỗ trợ

Chỉ **bản phát hành mới nhất** trên trang [Releases](https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/latest) nhận bản vá bảo mật.

## Báo cáo lỗ hổng

**Không** báo lỗ hổng bảo mật qua Issues công khai.

Hãy dùng tính năng **[Private vulnerability reporting](https://github.com/tsudev-tsudev/tsudev-trafficmonitor/security/advisories/new)** của GitHub (tab *Security* → *Report a vulnerability*). Vui lòng cung cấp:

* Phiên bản, kiến trúc (x86/x64/ARM64EC), phiên bản Windows.
* Mô tả lỗ hổng, mức ảnh hưởng và các bước tái hiện (PoC tối thiểu nếu có).

Cam kết phản hồi: xác nhận đã nhận trong **5 ngày làm việc**; đánh giá và kế hoạch khắc phục trong **30 ngày**. Người báo cáo sẽ được ghi công (nếu đồng ý) khi công bố bản vá.

Lỗ hổng thuộc phần mã kế thừa từ dự án gốc cũng sẽ được thông báo cho [tác giả gốc](https://github.com/zhongyang219/TrafficMonitor).

## Kiểm tra tính toàn vẹn tệp tải về

Mỗi bản phát hành kèm tệp `SHA256SUMS.txt` do quy trình CI tự tạo. Trước khi chạy, hãy kiểm tra mã băm trong PowerShell:

```powershell
Get-FileHash .\TrafficMonitor-VN_V1.86_x64_Lite.zip -Algorithm SHA256
```

So sánh kết quả với dòng tương ứng trong `SHA256SUMS.txt`. Nếu không khớp, **không chạy** tệp và báo cho chúng tôi.

Bản phát hành còn kèm **chứng thực nguồn gốc build** (build provenance attestation) của GitHub. Có thể xác minh bằng GitHub CLI:

```bash
gh attestation verify TrafficMonitor-VN_V1.86_x64_Lite.zip --repo tsudev-tsudev/tsudev-trafficmonitor
```

## Mô hình mối đe dọa và biện pháp đã áp dụng

TrafficMonitor là ứng dụng desktop chạy với quyền người dùng (bản Lite) hoặc quyền Quản trị viên (bản đầy đủ có đo nhiệt độ). Các bề mặt tấn công chính và biện pháp tương ứng:

### 1. Cơ chế kiểm tra cập nhật
*Rủi ro:* tệp thông tin phiên bản bị giả mạo (tấn công chuỗi cung ứng, MITM) dẫn người dùng tới tệp độc hại.

* Chỉ tải `version_utf8.info` từ repo này qua **HTTPS** (chứng chỉ TLS luôn được kiểm tra).
* Liên kết tải phải bắt đầu **chính xác** bằng `https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/`, không chứa `..`, `@`, `?`, `#`, `%`, `\`, khoảng trắng hay ký tự điều khiển, và chỉ trỏ tới `.zip`/`.exe`. Liên kết được kiểm tra **hai lần** (khi phân tích và ngay trước khi mở).
* Chương trình **không tự tải và không tự chạy** bản cập nhật; chỉ mở trang tải trong trình duyệt sau khi người dùng đồng ý.
* Chuỗi phiên bản được kiểm tra định dạng và so sánh theo số.

### 2. Truy cập mạng
* `CCommon::GetURL` chỉ chấp nhận URL `https://`, đặt timeout 15 giây, giới hạn phản hồi 1 MB, không dùng cache và cookie.
* Tra cứu IP công cộng chỉ chạy khi người dùng bấm trong hộp thoại **Chi tiết kết nối mạng**, dùng [ipify](https://www.ipify.org/) (mã nguồn mở, chỉ trả về địa chỉ IP); kết quả được kiểm tra định dạng trước khi hiển thị.
* Chương trình **không gửi dữ liệu thống kê/telemetry** nào.

### 3. Nạp DLL và plugin
* Khi khởi động: `SetDllDirectory("")` và `SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)` — loại thư mục hiện hành và biến `PATH` khỏi thứ tự tìm DLL (chống DLL hijacking/preloading).
* Plugin được nạp bằng đường dẫn tuyệt đối với `LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS`.
* **Giới hạn đã biết:** plugin là mã gốc chạy cùng tiến trình và cùng quyền với TrafficMonitor; chương trình **không** xác minh chữ ký số của plugin. Chỉ cài plugin từ nguồn tin cậy. Nên đặt TrafficMonitor ở thư mục mà người dùng thường không ghi được (ví dụ `C:\Program Files\TrafficMonitor-VN`) khi chạy bản đầy đủ với quyền Quản trị viên, để tránh phần mềm khác cài plugin trái phép.

### 4. Gia cố tệp thực thi (cấu hình Release)
Thiết lập tại [`Directory.Build.props`](./Directory.Build.props):

| Cờ | Tác dụng |
|---|---|
| `/sdl`, `/GS` | Kiểm tra bảo mật bổ sung lúc biên dịch, chống tràn bộ đệm stack |
| `/guard:cf` | Control Flow Guard |
| `/DYNAMICBASE`, `/HIGHENTROPYVA` | ASLR (entropy cao trên x64) |
| `/NXCOMPAT` | DEP |
| `/CETCOMPAT` | Tương thích Intel CET Shadow Stack (x86/x64) |

Project C++/CLI `OpenHardwareMonitorApi` không hỗ trợ `/guard:cf` và `/CETCOMPAT` nên được loại trừ khỏi hai cờ này.

### 5. Thành phần bên thứ ba
| Thành phần | Phiên bản | Ghi chú |
|---|---|---|
| LibreHardwareMonitorLib.dll | 0.9.4 (net472) | SHA-256 `a0f2728f1734c236a9d02d9e25a88bc4f8cb7bd1faff1770726beb7af06bf8dc`, trùng khớp gói NuGet chính thức |
| TinyXML-2 | nhúng mã nguồn | Phân tích skin XML |

Khi cập nhật DLL bên thứ ba, chỉ lấy từ nguồn chính thức (NuGet/GitHub Releases của dự án) và ghi lại mã băm vào bảng trên.

### 6. Quy trình phát triển
* **CodeQL** quét mã C/C++ mỗi lần push/PR và hằng tuần.
* **Dependabot** theo dõi phiên bản GitHub Actions.
* Workflow CI dùng quyền tối thiểu (`permissions: contents: read`) và ghim action theo phiên bản.
* Khuyến nghị bật cho repo: *Branch protection* cho `main` (yêu cầu PR + CI xanh), *Secret scanning*, *Push protection*, *Private vulnerability reporting*.

## Phạm vi

Ngoài phạm vi: lỗi chỉ tái hiện được khi kẻ tấn công **đã có** quyền Quản trị viên hoặc quyền ghi vào thư mục chương trình; plugin/skin của bên thứ ba; dịch vụ ipify và GitHub.
