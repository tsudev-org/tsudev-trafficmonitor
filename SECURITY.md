# Chính sách bảo mật - TrafficMonitor-VN

## Phiên bản được hỗ trợ

Chỉ **bản phát hành mới nhất** trên trang [Releases](https://github.com/tsudev-org/tsudev-trafficmonitor/releases/latest) nhận bản vá bảo mật.

## Báo cáo lỗ hổng

**Không** báo lỗ hổng bảo mật qua Issues công khai.

Hãy dùng tính năng **[Private vulnerability reporting](https://github.com/tsudev-org/tsudev-trafficmonitor/security/advisories/new)** của GitHub (tab *Security* → *Report a vulnerability*). Vui lòng cung cấp:

* Phiên bản, kiến trúc (x86/x64/ARM64EC), phiên bản Windows.
* Mô tả lỗ hổng, mức ảnh hưởng và các bước tái hiện (PoC tối thiểu nếu có).

Cam kết phản hồi: xác nhận đã nhận trong **5 ngày làm việc**; đánh giá và kế hoạch khắc phục trong **30 ngày**. Người báo cáo sẽ được ghi công (nếu đồng ý) khi công bố bản vá.

## Kiểm tra tính toàn vẹn tệp tải về

Mỗi bản phát hành kèm tệp `SHA256SUMS.txt` do quy trình CI tự tạo. Trước khi chạy, hãy kiểm tra mã băm trong PowerShell:

```powershell
Get-FileHash .\tsudev-trafficmonitor_26.11.0901_x64_Lite.zip -Algorithm SHA256
```

So sánh kết quả với dòng tương ứng trong `SHA256SUMS.txt`. Nếu không khớp, **không chạy** tệp và báo cho chúng tôi.

Bản phát hành còn kèm **chứng thực nguồn gốc build** (build provenance attestation) của GitHub. Có thể xác minh bằng GitHub CLI:

```bash
gh attestation verify tsudev-trafficmonitor_26.11.0901_x64_Lite.zip --repo tsudev-org/tsudev-trafficmonitor
```

## Chuỗi tin cậy của bản cập nhật

1. **Chữ ký thông tin phiên bản** - ứng dụng chỉ tin `version_signed.xml` có chữ ký hợp lệ (xem mục 1 bên dưới).
2. **SHA-256** - từ 26.11.0905, khi bạn đồng ý cập nhật, ứng dụng **tự tải gói** đúng kiến trúc, tính SHA-256 trong lúc tải và so với SHA-256 trong thông tin phiên bản có chữ ký; chỉ khi khớp mới lưu gói vào thư mục Tải về (Downloads) và mở thư mục chọn sẵn tệp. Lệch thì xóa tệp và báo lỗi. Mã băm này cũng có trong `SHA256SUMS.txt` của bản phát hành; nếu tải bằng trình duyệt (bản cũ hơn, hoặc khi tự tải thất bại), đối chiếu bằng `Get-FileHash <tệp.zip> -Algorithm SHA256` (PowerShell).
3. **Chứng thực nguồn gốc build** - mỗi gói được GitHub Actions chứng thực (`gh attestation verify <tệp.zip> -R tsudev-org/tsudev-trafficmonitor`), chứng minh gói được build từ mã nguồn của repo này tại đúng tag phát hành.

Tự kiểm tra chữ ký thông tin phiên bản (cần OpenSSL và Python):

```bash
python3 -c "import base64,sys;r=base64.b64decode(open('version_signed.xml.sig').read());i=lambda b:(b'\0'+b.lstrip(b'\0') if b.lstrip(b'\0')[0]&0x80 else b.lstrip(b'\0'));x,y=i(r[:32]),i(r[32:]);s=b'\2'+bytes([len(x)])+x+b'\2'+bytes([len(y)])+y;sys.stdout.buffer.write(b'\x30'+bytes([len(s)])+s)" > sig.der
openssl dgst -sha256 -verify update_signing_pub.pem -signature sig.der version_signed.xml
```

## Mô hình mối đe dọa và biện pháp đã áp dụng

TrafficMonitor là ứng dụng desktop chạy với quyền người dùng (bản Lite) hoặc quyền Quản trị viên (bản đầy đủ có đo nhiệt độ). Các bề mặt tấn công chính và biện pháp tương ứng:

### 1. Cơ chế kiểm tra cập nhật
*Rủi ro:* tệp thông tin phiên bản bị giả mạo (tấn công chuỗi cung ứng, MITM) dẫn người dùng tới tệp độc hại.

* **Thông tin phiên bản có chữ ký (từ V1.88-vn).** Mỗi bản phát hành kèm `version_signed.xml` (phiên bản, liên kết tải, **SHA-256 của từng gói**) và chữ ký `version_signed.xml.sig` (ECDSA P-256/SHA-256). Khóa bí mật chỉ nằm trong secret của environment `release` trên GitHub (job phát hành cần chủ repo duyệt); khóa công khai nhúng trong `ForkConfig.h` và có bản PEM tại [`tools/keys/update_signing_pub.pem`](./tools/keys/update_signing_pub.pem). Ứng dụng xác minh chữ ký bằng Windows CNG; chữ ký sai thì bỏ toàn bộ thông tin cập nhật. Vì vậy kể cả khi GitHub, DNS hay kết nối mạng bị giả mạo, kẻ tấn công không thể đưa phiên bản/liên kết/mã băm giả vào ứng dụng.
* **Chống hạ cấp:** chỉ báo cập nhật khi phiên bản trong thông tin có chữ ký **lớn hơn** phiên bản đang chạy (so sánh theo số).
* Bản V1.87-vn (trước khi có chữ ký) đọc `version_utf8.info` không chữ ký qua **HTTPS**; tệp này vẫn được duy trì cho bản đó. Bản V1.86-vn đọc thông tin cập nhật từ repo cũ `TrafficMonitor-VN` đã gỡ (11/2026) nên không còn nhận thông báo; hãy tải bản mới tại trang Tải về.
* Liên kết tải phải bắt đầu **chính xác** bằng `https://github.com/tsudev-org/tsudev-trafficmonitor/releases/download/` hoặc địa chỉ cũ trước khi chuyển repo `https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/` (GitHub chuyển hướng; bản phát hành ghi liên kết theo địa chỉ cũ để bản đã cài vẫn cập nhật được), không chứa `..`, `@`, `?`, `#`, `%`, `\`, khoảng trắng hay ký tự điều khiển, và chỉ trỏ tới `.zip`/`.exe`. Liên kết được kiểm tra **hai lần** (khi phân tích và ngay trước khi mở).
* **Tự tải và kiểm SHA-256 (từ 26.11.0905).** Chỉ sau khi người dùng đồng ý, ứng dụng tải gói ở luồng nền (hộp thoại tiến độ có nút Hủy):
  * chỉ tải liên kết đã qua bộ lọc trên và trỏ tới gói `.zip`; tên tệp lưu lấy từ liên kết, chỉ gồm `A-Z a-z 0-9 . _ -`, không phải tên thiết bị DOS;
  * tự theo chuyển hướng (tối đa 5 bước) nhưng **mọi bước** phải là HTTPS tới `github.com`, `objects.githubusercontent.com` hoặc `release-assets.githubusercontent.com` (`FORK_UPDATE_DOWNLOAD_HOSTS`); luôn kiểm chứng chỉ TLS, không cookie, không bộ nhớ đệm;
  * giới hạn 50 MB và 10 phút; ghi ra tệp tạm `<tên>.part` rồi mới đổi tên khi SHA-256 khớp; **không ghi đè** tệp đang có (lưu thành `<tên> (2).zip`...); lệch, lỗi hay hủy thì xóa tệp tạm;
  * tệp lưu được gắn *Mark of the Web* (`Zone.Identifier`) như khi tải bằng trình duyệt, để Windows SmartScreen vẫn kiểm chương trình giải nén từ gói;
  * chương trình **không tự giải nén và không tự chạy** gói. Thông tin phiên bản không có SHA-256 hợp lệ, hoặc tải thất bại (lỗi mạng, không ghi được tệp): người dùng được hỏi có mở liên kết trong trình duyệt không (đường cũ). SHA-256 lệch thì **không** mở gì.
  * Kiểm thử: `tests/UpdateSecurityTests.cpp` (logic thuần) và `tests/UpdateDownloadTests.cpp` (tải gói phát hành thật trên GitHub: SHA khớp, SHA lệch, không ghi đè, quá kích thước, hủy, chuyển hướng tới máy chủ không được phép) chạy trong CI; harness fuzz `tests/fuzz/UpdateSecurityFuzz.cpp` kiểm bất biến của bộ lọc tên tệp và so khớp SHA-256.
* Chuỗi phiên bản được kiểm tra định dạng và so sánh theo số.

### 2. Truy cập mạng
* `CCommon::GetURL` chỉ chấp nhận URL `https://`, đặt timeout 15 giây, giới hạn phản hồi 1 MB, không dùng cache và cookie.
* Sau khi theo chuyển hướng (redirect), URL cuối cùng phải là HTTPS và thuộc danh sách máy chủ cho phép (`FORK_TRUSTED_HOSTS` trong `ForkConfig.h`: GitHub, máy chủ tệp của GitHub, `trafficmonitor.tsudev.com`, ipify); nếu không, dữ liệu bị bỏ.
* Mọi URL ứng dụng dùng chỉ khai báo trong `ForkConfig.h`; `tools/check_urls.py` chạy trong CI chặn `http://`, URL ngoài danh sách cho phép và URL viết thẳng trong mã C++.
* Tra cứu IP công cộng chỉ chạy khi người dùng bấm trong hộp thoại **Chi tiết kết nối mạng**, dùng [ipify](https://www.ipify.org/) (mã nguồn mở, chỉ trả về địa chỉ IP); kết quả được kiểm tra định dạng trước khi hiển thị.
* Chương trình **không gửi dữ liệu thống kê/telemetry** nào.

### 3. Nạp DLL và plugin
* Khi khởi động: `SetDllDirectory("")` và `SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)` - loại thư mục hiện hành và biến `PATH` khỏi thứ tự tìm DLL (chống DLL hijacking/preloading).
* Plugin được nạp bằng đường dẫn tuyệt đối với `LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS`.
* **Giới hạn đã biết:** plugin là mã gốc chạy cùng tiến trình và cùng quyền với TrafficMonitor; chương trình **không** xác minh chữ ký số của plugin. Chỉ cài plugin từ nguồn tin cậy. Nên đặt TrafficMonitor ở thư mục mà người dùng thường không ghi được (ví dụ `C:\Program Files\TrafficMonitor-VN`) khi chạy bản đầy đủ với quyền Quản trị viên, để tránh phần mềm khác cài plugin trái phép.

### 3b. Đọc tệp skin (`skin.xml`)
*Rủi ro:* skin tải từ nguồn khác chứa XML độc hại (làm ứng dụng dừng, tốn bộ nhớ, giá trị kích thước bất thường).

* Phần phân tích thuần nằm ở [`TrafficMonitor/XmlParsing.h`](./TrafficMonitor/XmlParsing.h) (không phụ thuộc MFC), dùng tinyxml2 11.0.0 (không hỗ trợ DTD/thực thể ngoài, nên không có XXE).
* Giới hạn: tệp tối đa **1 MB** (kiểm kích thước trước khi đọc), tối đa 4096 phần tử, sâu tối đa 16 cấp; tối đa 256 màu/văn bản hiển thị/mục bố cục/ánh xạ plugin, chuỗi tối đa 256 byte (cắt đúng ranh giới UTF-8), tên phông quá 128 byte thì dùng phông mặc định. Vượt giới hạn hoặc sai cú pháp: skin bị bỏ, ứng dụng dùng giao diện mặc định.
* Số đọc từ thuộc tính không tràn (thay `atoi`) và bị kẹp: kích thước 0-4096, tọa độ -4096-4096, chiều cao chữ 0-512, cỡ phông 0-72, màu 0-0xFFFFFF, kiểu căn lề lạ coi là căn trái.
* Thông tin phiên bản (`version_signed.xml`) chỉ được phân tích **sau** khi xác minh chữ ký; hàm tìm nút đơn giản cũng nằm trong `XmlParsing.h` (đã sửa trường hợp thẻ đóng đứng trước thẻ mở).
* Kiểm thử: `tests/XmlParsingTests.cpp` (kể cả mọi `skin.xml` thật trong `TrafficMonitor/skins`, chạy thường và dưới ASan) và harness fuzz `tests/fuzz/SkinXmlFuzz.cpp` (libFuzzer + ASan 120 giây mỗi lần CI, hạt giống là các `skin.xml` thật) kiểm mọi giá trị trả về nằm trong khoảng hợp lệ.

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
