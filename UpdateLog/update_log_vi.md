**Tiếng Việt | [English](./update_log_en-us.md)**

# Nhật ký cập nhật - tsudev-trafficmonitor

Thay đổi của từng bản phát hành tsudev-trafficmonitor do tsudev thực hiện, mới nhất trên cùng. Tóm tắt mỗi bản một dòng: [CHANGELOG.md](../CHANGELOG.md).
Các bản trước khi đổi tên mang tên **TrafficMonitor-VN**; nội dung của chúng bên dưới giữ nguyên như lúc phát hành.

## Chưa phát hành

### Đổi tên thành tsudev-trafficmonitor
* Phần mềm, tệp chương trình (`tsudev-trafficmonitor.exe`), thư mục trong gói zip, tài liệu và website thống nhất tên **tsudev-trafficmonitor** theo hệ sinh thái tsudev.
* **Tự chuyển dữ liệu:** lần chạy đầu, cấu hình và lịch sử lưu lượng ở `%AppData%\TrafficMonitor` được chép sang `%AppData%\tsudev-trafficmonitor` (thư mục cũ giữ nguyên để quay lại bản cũ vẫn dùng được).
* **Tự chuyển tự khởi động cùng Windows:** mục Registry/Task Scheduler tên cũ của chính phần mềm này được thay bằng tên mới; mục thuộc bản TrafficMonitor gốc cài riêng (nếu có) không bị đụng tới.
* Bản cũ và bản mới không chạy cùng lúc: chạy bản mới khi bản cũ đang mở sẽ báo phần mềm đang chạy.
* Plugin có sẵn vẫn dùng được (giao diện plugin không đổi; tên `ITrafficMonitor` giữ làm tên khác của `ITsudevTrafficMonitor`).

### Thương hiệu
* Logo mới cho icon chương trình, icon khay, hộp thoại Giới thiệu và website; thêm skin `tsudev_Logo`.

## 26.11.1001 (10/11/2026)

### Sửa lỗi
* Sau khi tự tải gói cập nhật và kiểm SHA-256 khớp, bản 26.11.0905 không mở được thư mục Tải về; nay mở đúng thư mục và chọn sẵn tệp.

### Bảo mật
* Đọc tệp skin (`skin.xml`) an toàn hơn: tệp tối đa 1 MB, giới hạn số phần tử và độ sâu; kích thước, tọa độ, cỡ phông, màu ngoài khoảng hợp lệ được đưa về khoảng hợp lệ; chuỗi quá dài bị cắt. Skin không hợp lệ thì dùng giao diện mặc định. Có kiểm thử tự động và fuzzing với các skin thật.
* **Build tái lập:** cùng mã nguồn và cùng bộ công cụ cho ra cùng `TrafficMonitor.exe` và cùng gói zip từng byte; CI tự build hai lần để kiểm. "Ngày biên dịch" trong Giới thiệu nay là thời điểm commit của mã nguồn. Cách tự kiểm: [SECURITY.md](../SECURITY.md#build-tái-lập-reproducible-build).

## 26.11.0905 (09/11/2026)

### Bảo mật
* Khi bạn đồng ý cập nhật, ứng dụng **tự tải gói** đúng kiến trúc (có hộp thoại tiến độ và nút Hủy), tính SHA-256 trong lúc tải và so với SHA-256 trong thông tin phiên bản có chữ ký số. Chỉ khi khớp mới lưu vào thư mục Tải về và mở thư mục chọn sẵn tệp; không khớp thì xóa tệp và báo lỗi. Bạn không còn phải tự đối chiếu mã băm.
* Chỉ tải từ trang Releases của repo; mọi bước chuyển hướng phải tới máy chủ của GitHub qua HTTPS; giới hạn 50 MB; không ghi đè tệp có sẵn; tệp lưu được đánh dấu "tải từ Internet" như khi tải bằng trình duyệt. Ứng dụng không tự giải nén hay tự chạy gói.
* Tải thất bại (lỗi mạng): ứng dụng hỏi có mở liên kết trong trình duyệt như trước không.
* Kiểm thử tích hợp trong CI tải gói phát hành thật để thử các trường hợp khớp, lệch SHA-256, quá kích thước, hủy và chuyển hướng tới máy chủ lạ.

### Khác
* Repo cũ `TrafficMonitor-VN` được gỡ; mọi mã nguồn, bản phát hành và kênh cập nhật nằm ở repo `tsudev-org/tsudev-trafficmonitor`.

## 26.11.0904 (09/11/2026)

> Số 26.11.0903 không phát hành (bản build lỗi ở bước sinh SBOM trước khi tạo Release); 26.11.0904 có cùng nội dung.

### Bảo mật
* Nâng thư viện đọc XML tinyxml2 lên 11.0.0 (trước là bản 9.0 chưa phát hành chính thức), vá CVE-2024-50614 và CVE-2024-50615: tệp `skin.xml` độc hại có thể làm ứng dụng dừng đột ngột.
* Kiểm tra cập nhật chấp nhận liên kết tải từ cả địa chỉ cũ (`tsudev-tsudev`) và địa chỉ mới (`tsudev-org`) của kho mã; vẫn chỉ tin thông tin phiên bản có chữ ký hợp lệ.
* Mỗi gói phát hành kèm SBOM chuẩn SPDX 2.3 (`*.spdx.json`) có chứng thực của GitHub (`gh attestation verify <tệp.zip> -R tsudev-org/tsudev-trafficmonitor --predicate-type https://spdx.dev/Document/v2.3`).
* Logic kiểm tra cập nhật có kiểm thử tự động, chạy thêm AddressSanitizer và fuzzing trong CI; CodeQL quét cả website, script và workflow.

### Kho mã
* Kho mã chuyển sang tổ chức [tsudev-org](https://github.com/tsudev-org/tsudev-trafficmonitor); liên kết trong ứng dụng, tài liệu và website trỏ địa chỉ mới. Địa chỉ cũ tự chuyển hướng, bản đã cài vẫn nhận cập nhật.

### Website
* Sửa độ tương phản của liên kết đang chọn trong menu và cho phép cuộn khối mã bằng bàn phím (kiểm bằng axe-core, chuẩn WCAG 2.2 AA).

## 26.11.0902 (09/11/2026)

### Nhận diện tsudev
* Icon chương trình (tệp exe, cửa sổ, thanh tác vụ, kiểu biểu tượng khay thứ tư) là chú cú tsudev; các kiểu biểu tượng khay dạng đồ thị mạng giữ nguyên.
* Hộp thoại Giới thiệu: ảnh đầu mới; dòng bản quyền "Bản quyền (c) 2026 tsudev"; bỏ liên kết "Dự án gốc" và "Lời cảm ơn". Giấy phép Anti 996 và thông báo bản quyền bắt buộc xem tại liên kết "Giấy phép sử dụng".
* Người dịch gói tiếng Việt và tiếng Anh ghi tsudev; gói ngôn ngữ khác không hiện dòng người dịch.

### Giao diện (skin)
* Thêm skin `tsudev_DoThi_Sang` (nền sáng) và `tsudev_DoThi_Toi` (nền tối): nền đồ thị tốc độ mạng, hiện thêm GPU và ổ cứng khi bật "Hiện thêm thông số".

### Bảo mật
* Bỏ kiểm tra phiên bản plugin qua mạng tới kho của bên thứ ba: ứng dụng chỉ còn gọi mạng để kiểm tra cập nhật có chữ ký và tra IP công cộng khi người dùng mở Chi tiết mạng. Giao diện plugin giữ nguyên (API phiên bản 8).

### Tài liệu
* Tài liệu tiếng Anh viết lại (README, Hỏi đáp, nhật ký cập nhật); thêm CHANGELOG.md.

## 26.11.0901 (09/11/2026)

### Cách đánh số phiên bản mới
* Từ bản này, phiên bản theo khuôn **`{YY}.{M}.{DD}{NN}`** của tsudev (năm.tháng.ngày + số thứ tự trong ngày): `26.11.0901` là bản đầu tiên ngày 09/11/2026. Bản ngay trước là V1.91-vn; so sánh theo số nên các bản cũ vẫn nhận ra bản cập nhật.
* Tag phát hành dạng `V26.11.0901-vn`; gói tải về đổi tên thành `tsudev-trafficmonitor_26.11.0901_<kiến trúc>_Lite.zip`.

### Tài liệu tiếng Anh
* Thêm tài liệu tiếng Anh: Plugin (`Plugin_en-us.md`), Kho giao diện (`SkinGallery_en-us.md`), Hướng dẫn tạo skin (`SkinTutorial_en-us.md`), Phát triển plugin (`PluginDev_en-us.md`).
* Khi giao diện dùng ngôn ngữ khác tiếng Việt, các liên kết Trợ giúp, Tải thêm mẫu mạng, Hướng dẫn tạo skin, Tải thêm Plugin, Hướng dẫn làm Plugin mở tài liệu tiếng Anh (Trợ giúp mở `README_en-us.md`); tiếng Việt vẫn mở website.

## V1.91-vn (09/11/2026)

### Liên kết
* Website sản phẩm **https://trafficmonitor.tsudev.com** đã chạy trên tên miền chính thức (Cloudflare Pages, HSTS, CSP chặt, DNSSEC, CAA).
* Các liên kết trong ứng dụng mở trang hướng dẫn trên website thay cho tệp `.md` trên GitHub: Hỏi đáp (`/huong-dan/hoi-dap/`), Nhật ký cập nhật (`/nhat-ky/`), Trợ giúp (`/huong-dan/`), Kho giao diện, Tự làm giao diện, Plugin, Phát triển plugin. Bản `.md` trên GitHub vẫn giữ làm dự phòng.
* Ngôn ngữ khác tiếng Việt: Hỏi đáp và Nhật ký cập nhật vẫn mở tài liệu tiếng Anh trên GitHub (`FORK_HELP_EN_URL`, `FORK_UPDATE_LOG_EN_URL`).

## V1.90-vn (09/11/2026)

### Giao diện (skin)
* Skin mặc định mới **`0_tsudev_Cu`** (chú cú tsudev, nền sáng) và skin **`tsudev_VongMang`** (biểu tượng tsudev, nền tối), sinh bằng `tools/skin_assets/make_skins.py` từ ảnh nguồn của tsudev.
* Gỡ 4 skin có hình nhân vật hoạt hình của bên thứ ba kế thừa từ bản gốc: `0_MacDinh`, `MacDinh2`, `GiaoDien01`, `GiaoDien02`. Máy đang dùng các skin này tự chuyển sang `0_tsudev_Cu` (skin đầu tiên theo thứ tự tên).

## V1.89-vn (09/11/2026)

### Sửa lỗi
* Hộp thoại Giới thiệu: chữ "Lời cảm ơn" đè lên liên kết "tsudev.com" (V1.88-vn) vì được xếp vào cột thứ ba khi tự giãn theo độ dài chữ; nay các liên kết xếp đúng hai cột.

## V1.88-vn (09/11/2026) - Bản cập nhật có chữ ký số

### Nhận diện tsudev
* Thông tin tệp chương trình: nhà phát hành `tsudev`, `Copyright (c) 2026 tsudev`, kèm `https://tsudev.com`.
* Hộp thoại Giới thiệu thêm liên kết tới [tsudev.com](https://tsudev.com).
* Trang sản phẩm [trafficmonitor.tsudev.com](https://trafficmonitor.tsudev.com): tải về kèm SHA-256, hướng dẫn, nhật ký, bảo mật.

### Bảo mật
* Mọi URL ứng dụng dùng (kể cả ghi công thư viện bên thứ ba và dịch vụ tra IP công cộng) khai báo một chỗ duy nhất trong `ForkConfig.h`; bỏ hàm thử nghiệm không dùng gọi tới một trang bên ngoài.
* Thêm `tools/check_urls.py` chạy trong CI: chặn `http://`, URL ngoài danh sách cho phép và URL viết thẳng trong mã C++ ngoài `ForkConfig.h`; kiểm tra liên kết còn sống hằng tuần.
* GitHub Actions ghim theo SHA commit; tạo Release cần duyệt qua environment `release`.
* **Thông tin phiên bản có chữ ký:** kiểm tra cập nhật đọc `version_signed.xml` của bản phát hành mới nhất và chỉ tin khi chữ ký ECDSA P-256 hợp lệ (xác minh bằng Windows CNG, khóa công khai nhúng trong ứng dụng); chỉ báo cập nhật khi phiên bản lớn hơn bản đang chạy. Hộp thoại cập nhật hiện SHA-256 của gói để đối chiếu sau khi tải.
* Truy cập mạng: sau khi theo chuyển hướng, URL cuối phải là HTTPS và thuộc danh sách máy chủ cho phép.
* SECURITY.md: mô tả chuỗi tin cậy (chữ ký → SHA-256 → chứng thực build) và cách tự kiểm tra.

### Bản đầy đủ (đo nhiệt độ)
* Đọc giá trị cảm biến dạng `float` thay vì `double` rồi ép ngầm; ép kiểu tường minh mức dùng GPU/ổ cứng. CI build thêm bản đầy đủ x64 (không phát hành) với cùng mức kiểm tra cảnh báo như bản Lite.

## V1.87-vn (08/11/2026) - Bản cầu nối sang kho mã mới

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
* "Tải thêm mẫu mạng" (Kho giao diện), "Tải thêm Plugin" và "Hướng dẫn làm Plugin" (Quản lý Plugin) mở trang tiếng Việt của TrafficMonitor-VN: [KhoGiaoDien.md](../KhoGiaoDien.md), [Plugin.md](../Plugin.md), [PhatTrienPlugin.md](../PhatTrienPlugin.md)
* Thông báo crash ở 14 gói ngôn ngữ còn lại (Pháp, Đức, Nhật...) hướng dẫn báo lỗi tại trang Issues của TrafficMonitor-VN thay vì gửi email.
* Mọi URL của bản Việt hóa gom về `ForkConfig.h`.

### Giao diện (bổ sung sau khi kiểm tra bằng ảnh chụp thật)
* Rút gọn "Đổi vị trí hàng trên/dưới cho Upload/Download", "Mỗi thông số một màu riêng biệt" (bị khung nhóm che dù điều khiển tự giãn khi chạy) và tên ngôn ngữ "Tiếng Việt (Chuẩn)" → "Tiếng Việt" (bị cắt trong ô chọn ngôn ngữ).
* Cấu hình ngôn ngữ cũ (lưu kèm tên hiển thị "Tiếng Việt (Chuẩn)") được tự khớp theo mã BCP-47 sang gói hiện tại, tránh ô chọn ngôn ngữ nhảy về "Theo hệ thống" sau khi đổi tên hiển thị.

### Sửa sau đợt kiểm thử giao diện tự động (V1.87-vn)
* Tự động chọn card mạng: trên máy có card ảo (Hyper-V/WSL, VirtualBox...) phần mềm có thể chọn card ảo vì nó có nhiều byte nhất, nên cửa sổ nổi hiện tốc độ gần bằng 0 dù đang tải qua Wi-Fi/LAN. Nay ưu tiên card có cổng mặc định (default gateway); nếu không card nào có thì chọn như cũ.
* Nhãn mặc định của cửa sổ nổi đổi từ "Up:"/"Down:" (tiếng Anh, chữ đè lên hình trang trí của skin mặc định) thành "↑:"/"↓:". Nhãn người dùng tự đặt không bị ảnh hưởng.
* Hộp thoại Giới thiệu hiện lặp "Tiếng Việt Bản dịch tiếng Việt: …"; nay là "Bản dịch Tiếng Việt: …".
* Quản lý Plugin: tiêu đề cột "Phiên bản" bị cắt thành "Phiên b..."; chia lại độ rộng cột.

## V1.86-vn (07/11/2026) - Bản Việt hóa đầu tiên

Dựa trên TrafficMonitor 1.86.

### Việt hóa
* Giao diện tiếng Việt mặc định ở lần chạy đầu; gói ngôn ngữ tiếng Việt đầy đủ 438/438 chuỗi và được nhúng trong tệp chương trình.
* Bổ sung bản tiếng Việt cho hộp thoại chọn màu và plugin mẫu (PluginDemo).
* Dịch toàn bộ chú thích mã nguồn (≈2.800 đoạn, 186 tệp), tên nhóm thư mục trong Visual Studio, tài liệu README, Hỏi đáp, hướng dẫn tạo skin, mẫu báo lỗi sang tiếng Việt.
* Chuyển 27 tệp nguồn mã hóa GBK/UTF-16 sang UTF-8 (BOM); đổi tên thư mục skin tiếng Trung sang tên không dấu.

### Giao diện
* Sửa 13 chỗ chữ tiếng Việt bị cắt trong các hộp thoại Cài đặt (đo bằng phông Segoe UI 9pt so với kích thước điều khiển): rút gọn câu dịch, nới rộng nút "Khôi phục mặc định" trong Cài đặt Windows 11; sửa 2 chỗ tương tự ở bản tiếng Anh.

### Gỡ tiếng Trung và phân định bản quyền
* Bỏ gói ngôn ngữ tiếng Trung giản thể/phồn thể, hộp thoại chọn màu tiếng Trung, lời cảm ơn tiếng Trung (thay bằng bản tiếng Việt) và phần nội dung cập nhật tiếng Trung.
* Thông tin phiên bản của tệp chương trình dùng locale tiếng Việt, tên sản phẩm "TrafficMonitor-VN".
* Hộp thoại Giới thiệu: bỏ mục quảng bá phần mềm khác, thêm dòng bản quyền của bản Việt hóa; báo lỗi khi crash chuyển về trang Issues của TrafficMonitor-VN thay vì email.
* Bỏ 2 skin dùng hình nền/logo Windows của Microsoft (GiaoDien04, GiaoDien07); skin GiaoDien11 đổi chữ tiếng Trung "bộ nhớ" in trong ảnh nền thành "RAM".
* Bỏ ảnh chụp màn hình giao diện tiếng Trung khỏi tài liệu (sẽ thay bằng ảnh giao diện tiếng Việt).
* Thêm NOTICE.md phân định bản quyền; giữ nguyên thông báo giấy phép gốc theo yêu cầu của giấy phép Anti 996.
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
