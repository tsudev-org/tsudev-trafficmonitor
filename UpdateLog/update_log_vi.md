**Tiếng Việt | [English (dự án gốc)](./update_log_en-us.md)**

# Nhật ký cập nhật — TrafficMonitor-VN

Lịch sử thay đổi của dự án gốc (đến phiên bản 1.86) xem tại [update_log_en-us.md](./update_log_en-us.md).

## Chưa phát hành

### Bảo mật
* Mọi URL ứng dụng dùng (kể cả ghi công thư viện bên thứ ba và dịch vụ tra IP công cộng) khai báo một chỗ duy nhất trong `ForkConfig.h`; bỏ hàm thử nghiệm không dùng gọi tới một trang bên ngoài.
* Thêm `tools/check_urls.py` chạy trong CI: chặn `http://`, URL ngoài danh sách cho phép và URL viết thẳng trong mã C++ ngoài `ForkConfig.h`; kiểm tra liên kết còn sống hằng tuần.
* GitHub Actions ghim theo SHA commit; tạo Release cần duyệt qua environment `release`.

## V1.87-vn (2026-11-08) — Bản cầu nối sang kho mã mới

### Chuyển kho mã
* Kho mã chính chuyển sang [tsudev-tsudev/tsudev-trafficmonitor](https://github.com/tsudev-tsudev/tsudev-trafficmonitor). Từ bản này, kiểm tra cập nhật, tải bản mới, trang hướng dẫn và báo lỗi đều dùng kho mới. Kho cũ `TrafficMonitor-VN` được lưu trữ (chỉ đọc) và vẫn giữ toàn bộ lịch sử commit.
* Người dùng V1.86-vn cập nhật như bình thường lên V1.87-vn; các bản sau tự lấy từ kho mới.

### Sửa lỗi
* Kiểm tra phiên bản plugin: bảng phiên bản mới nhất được ghi từ luồng kiểm tra cập nhật và đọc từ hộp thoại Quản lý plugin mà không khóa (có thể crash khi mở hộp thoại đúng lúc đang kiểm tra); nay phân tích vào bảng tạm rồi thay dưới khóa, hàm đọc trả về bản sao.
* Menu con plugin và menu "Sửa mẫu" ép handle menu `HMENU` về `UINT` 32-bit trên bản 64-bit; nay dùng `UINT_PTR`.
* Đọc tệp (`GetFileContent`): khi `tellg()` lỗi trả về -1, giá trị bị ép thành kích thước cực lớn rồi cấp phát; nay kiểm tra lỗi và chỉ giữ số byte thực đọc được.
* Sửa toàn bộ cảnh báo ép kiểu số (C4244, C4267, C4018), cắt con trỏ (C4302, C4311) và chuỗi tiếng Việt trong chuỗi `char` (C4566) của bản Lite; CI coi các cảnh báo này là lỗi để không phát sinh lại.
* Thời điểm build trong hộp thoại Giới thiệu mất năm ("Sun 11/08/") khi build trên máy locale en-US; nay ghi dạng `yyyy-MM-dd HH:mm:ss UTC`.
* Kho giao diện: thiếu dấu cách sau "Người thiết kế:".
* Thông báo trong các gói ngôn ngữ có dấu `=` thừa trước cụm trong ngoặc kép (ví dụ `tệp= "..."`, `"Tùy chọn" >= "Cài đặt chung"`), lỗi có sẵn từ bản gốc; đã bỏ ở mọi gói ngôn ngữ.

### Chuyển liên kết về TrafficMonitor-VN
* "Tải thêm mẫu mạng" (Kho giao diện), "Tải thêm Plugin" và "Hướng dẫn làm Plugin" (Quản lý Plugin) mở trang tiếng Việt của TrafficMonitor-VN: [KhoGiaoDien.md](../KhoGiaoDien.md), [Plugin.md](../Plugin.md), [PhatTrienPlugin.md](../PhatTrienPlugin.md) thay vì trang tiếng Trung/Anh của dự án gốc.
* Thông báo crash ở 14 gói ngôn ngữ còn lại (Pháp, Đức, Nhật...) hướng dẫn báo lỗi tại trang Issues của TrafficMonitor-VN thay vì gửi email cho tác giả gốc.
* Mọi URL của bản Việt hóa gom về `ForkConfig.h` (gồm cả nguồn tra phiên bản plugin của dự án gốc).

### Giao diện (bổ sung sau khi kiểm tra bằng ảnh chụp thật)
* Rút gọn "Đổi vị trí hàng trên/dưới cho Upload/Download", "Mỗi thông số một màu riêng biệt" (bị khung nhóm che dù điều khiển tự giãn khi chạy) và tên ngôn ngữ "Tiếng Việt (Chuẩn)" → "Tiếng Việt" (bị cắt trong ô chọn ngôn ngữ).
* Cấu hình ngôn ngữ cũ (lưu kèm tên hiển thị "Tiếng Việt (Chuẩn)") được tự khớp theo mã BCP-47 sang gói hiện tại, tránh ô chọn ngôn ngữ nhảy về "Theo hệ thống" sau khi đổi tên hiển thị.

### Sửa sau đợt kiểm thử giao diện tự động (V1.87-vn)
* Tự động chọn card mạng: trên máy có card ảo (Hyper-V/WSL, VirtualBox...) phần mềm có thể chọn card ảo vì nó có nhiều byte nhất, nên cửa sổ nổi hiện tốc độ gần bằng 0 dù đang tải qua Wi-Fi/LAN. Nay ưu tiên card có cổng mặc định (default gateway); nếu không card nào có thì chọn như cũ.
* Nhãn mặc định của cửa sổ nổi đổi từ "Up:"/"Down:" (tiếng Anh, chữ đè lên hình trang trí của skin mặc định) thành "↑:"/"↓:". Nhãn người dùng tự đặt không bị ảnh hưởng.
* Hộp thoại Giới thiệu hiện lặp "Tiếng Việt Bản dịch tiếng Việt: …"; nay là "Bản dịch Tiếng Việt: …".
* Quản lý Plugin: tiêu đề cột "Phiên bản" bị cắt thành "Phiên b..."; chia lại độ rộng cột.

## V1.86-vn (2026-11-07) — Bản Việt hóa đầu tiên

Dựa trên TrafficMonitor 1.86 (commit `930f175` của dự án gốc).

### Việt hóa
* Giao diện tiếng Việt mặc định ở lần chạy đầu; gói ngôn ngữ tiếng Việt đầy đủ 438/438 chuỗi và được nhúng trong tệp chương trình.
* Bổ sung bản tiếng Việt cho hộp thoại chọn màu và plugin mẫu (PluginDemo).
* Dịch toàn bộ chú thích mã nguồn (≈2.800 đoạn, 186 tệp), tên nhóm thư mục trong Visual Studio, tài liệu README, Hỏi đáp, hướng dẫn tạo skin, mẫu báo lỗi sang tiếng Việt.
* Chuyển 27 tệp nguồn mã hóa GBK/UTF-16 sang UTF-8 (BOM); đổi tên thư mục skin tiếng Trung sang tên không dấu.

### Giao diện
* Sửa 13 chỗ chữ tiếng Việt bị cắt trong các hộp thoại Cài đặt (đo bằng phông Segoe UI 9pt so với kích thước điều khiển): rút gọn câu dịch, nới rộng nút "Khôi phục mặc định" trong Cài đặt Windows 11; sửa 2 chỗ tương tự ở bản tiếng Anh.

### Gỡ tiếng Trung và phân định bản quyền
* Bỏ gói ngôn ngữ tiếng Trung giản thể/phồn thể, hộp thoại chọn màu tiếng Trung, lời cảm ơn tiếng Trung (thay bằng bản tiếng Việt) và phần nội dung cập nhật tiếng Trung.
* Thông tin phiên bản của tệp chương trình dùng locale tiếng Việt, tên sản phẩm "TrafficMonitor-VN", giữ ghi công tác giả gốc.
* Hộp thoại Giới thiệu: bỏ mục quảng bá phần mềm khác của tác giả gốc, thêm dòng bản quyền của bản Việt hóa; báo lỗi khi crash chuyển về trang Issues của TrafficMonitor-VN thay vì email tác giả gốc.
* Bỏ 2 skin dùng hình nền/logo Windows của Microsoft (GiaoDien04, GiaoDien07); skin GiaoDien11 đổi chữ tiếng Trung "bộ nhớ" in trong ảnh nền thành "RAM".
* Bỏ ảnh chụp màn hình giao diện tiếng Trung khỏi tài liệu (sẽ thay bằng ảnh giao diện tiếng Việt).
* Thêm NOTICE.md phân định bản quyền: phần thay đổi của TrafficMonitor-VN © 2026 tsudev-tsudev; mã kế thừa © zhongyang219 (bắt buộc giữ theo giấy phép Anti 996).
* `tools/check_i18n.py` quét chữ Trung trong cả tài liệu, tệp `.rc` UTF-16, `.ini`, `.xml`... (trừ LICENSE_CN và gói tiếng Nhật).

### Bảo mật
* Kiểm tra cập nhật chỉ dùng repo TrafficMonitor-VN; liên kết tải bị ràng buộc chặt vào trang Releases của repo.
* So sánh phiên bản theo số (sửa lỗi so sánh chuỗi).
* `GetURL` chỉ chấp nhận HTTPS, có timeout và giới hạn kích thước.
* Tra cứu IP công cộng chuyển sang ipify; kiểm tra định dạng IP.
* Chống DLL hijacking khi khởi động và khi nạp plugin.
* Bật Control Flow Guard, CET, ASLR, DEP, `/sdl` cho bản Release.
* CI: build mỗi lần push/PR, CodeQL, Dependabot, mã băm SHA-256 và chứng thực nguồn gốc cho bản phát hành.

### Sửa lỗi
* Bản ARM64EC chọn nhầm liên kết tải bản x64 khi kiểm tra cập nhật.
* `GetJsonValueSimple` có thể gây lỗi `out_of_range` với dữ liệu không hợp lệ.
* Mẫu báo lỗi tiếng Anh trỏ nhầm sang repo không liên quan.
* Không lấy được IP công cộng ("Lỗi lấy dữ liệu"): URL ipify thiếu "/" trước "?" khiến WinINet bỏ mất tham số `format=json`; phân tích phản hồi chấp nhận cả JSON lẫn văn bản thuần.
* `GetURL` đọc phản hồi dạng byte thô thay vì gói vào `CString` (tránh byte rác cuối chuỗi khi phản hồi có số byte lẻ); ghi nhật ký gỡ lỗi khi mã HTTP khác 200 hoặc phản hồi quá lớn.
* IPv6: khi máy không có địa chỉ IPv6 toàn cầu (nhà mạng/router không cấp IPv6), hiển thị "Không có kết nối IPv6" thay vì "Lỗi lấy dữ liệu".
* Hộp thoại Chi tiết mạng: luồng lấy IP công cộng không còn thao tác trực tiếp lên điều khiển và đối tượng hộp thoại (có thể đã bị hủy); nhấp đúp liên tục không tạo nhiều luồng; có thể nhấp đúp lại để làm mới; IPv4 hiển thị ngay không chờ IPv6.
* Hộp thoại Chi tiết mạng dùng bản sao dữ liệu thay vì con trỏ vào bảng giao diện mạng mà luồng giám sát có thể cấp phát lại (nguy cơ crash); số byte đã nhận/gửi cập nhật mỗi giây.
* Bộ đếm lưu lượng 64-bit (`GetIfEntry2`): bộ đếm 32-bit cũ quay vòng sau mỗi 4 GiB làm mất lưu lượng thống kê và hiển thị sai tổng dữ liệu; tốc độ card mạng trên 4 Gbps (10 GbE) hiển thị đúng.
* An toàn luồng: khởi tạo lại kết nối mạng chỉ chạy trên luồng giao diện, có khóa bảo vệ bảng giao diện mạng giữa luồng giám sát và luồng giao diện.
* Phát hiện thay đổi số kết nối mạng (bật/tắt VPN, cắm cáp...) không bao giờ hoạt động do biến đếm luôn bằng -1.
* Cấp phát bảng giao diện mạng an toàn khi số card thay đổi giữa hai lần gọi (trước đây có thể đọc bảng chưa khởi tạo); bỏ qua bảng rỗng.
* Tính tốc độ mạng theo đồng hồ đơn điệu thay vì giờ hệ thống: chỉnh giờ/đồng bộ NTP có thể gây chia cho 0 (crash) hoặc tốc độ sai.
* Lưu lịch sử lưu lượng qua tệp tạm rồi thay thế, tránh mất toàn bộ lịch sử khi mất điện/đầy đĩa giữa lúc ghi; không ghi đè lịch sử khi tệp tạm thời không đọc được.
* Hộp thoại Chi tiết mạng mở trang của kết nối đang hoạt động có lưu lượng lớn nhất khi đang "Chọn tất cả" hoặc kết nối đang chọn không hoạt động (trước đây có thể mở trang card bị tắt, toàn số 0 và IP 0.0.0.0); trang của kết nối không hoạt động được ghi chú "Không hoạt động" và làm mờ các giá trị.
