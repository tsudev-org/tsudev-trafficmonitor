**Tiếng Việt | [English](./Help_en-us.md)**

# tsudev-trafficmonitor - Câu hỏi thường gặp (FAQ)

Trang này tổng hợp các câu hỏi thường gặp khi dùng tsudev-trafficmonitor. Phần giới thiệu tính năng và cách dùng có tại [README](./README.md).

> Tên menu/nút trong tài liệu này dùng đúng nhãn hiển thị trong giao diện tiếng Việt. Nếu bạn dùng ngôn ngữ khác, hãy đối chiếu với [bản tiếng Anh](./Help_en-us.md).

---

### Trên Windows 11, cửa sổ Taskbar bị đè lên mục Widgets (Tiện ích)

Mở **Cài đặt...** → thẻ **Cửa sổ Taskbar** → nhấn **Thiết lập riêng cho Windows 11**, rồi đánh dấu **Tránh bị mục Widgets đè lên**.

Nếu Widgets không hiển thị trên thanh tác vụ thì tùy chọn này sẽ bị mờ.

### Trên Windows 11, tùy chọn “Đẩy sang mép bên TRÁI của Taskbar” bị mờ

Trên Windows 11, tùy chọn này chỉ dùng được khi thanh tác vụ **căn giữa**. Khi thanh tác vụ căn trái, phía bên trái không còn chỗ trống để tsudev-trafficmonitor hiển thị.

### Trên Windows 11, cửa sổ Taskbar của tsudev-trafficmonitor đè lên biểu tượng khi thanh tác vụ đầy

Hiện chưa khắc phục được. Bạn nên hạn chế ghim quá nhiều ứng dụng lên thanh tác vụ, hoặc trong Cài đặt Windows đổi **Gộp các nút trên thanh tác vụ và ẩn nhãn** thành **Luôn luôn**.

### Làm sao hiển thị mức sử dụng CPU và bộ nhớ (RAM)?

Nhấp chuột phải vào cửa sổ nổi và chọn **Hiện thêm thông số**. Nếu muốn hiển thị cả trên cửa sổ Taskbar, nhấp chuột phải vào cửa sổ Taskbar → **Sắp xếp hiển thị...**, rồi đánh dấu mục CPU và bộ nhớ trong hộp thoại hiện ra.

### Làm sao đổi chữ “CPU”, “MEM”... trên thanh tác vụ?

Nhãn trên cửa sổ Taskbar có thể tùy biến: nhấp chuột phải vào cửa sổ Taskbar → **Cài đặt...** → thẻ **Cửa sổ Taskbar** → nhấn **Đổi chữ hiển thị...**. Nhấp đúp vào cột chữ bên phải để sửa, sau đó nhấn **Đồng ý** hai lần.

Vì nhãn là do người dùng tùy biến nên sẽ **không tự đổi** khi bạn chuyển ngôn ngữ. Sau khi chuyển ngôn ngữ, hãy nhấn **Khôi phục Mặc định** trong hộp thoại **Đổi chữ hiển thị**.

### Tốc độ mạng luôn hiển thị 0KB

Thường do máy tính vừa chuyển sang mạng khác (ví dụ đổi từ Wi‑Fi sang mạng dây, bật/tắt VPN).

1. Nhấp chuột phải → **Chọn Card Mạng** → **Làm mới danh sách mạng**.

2. Nếu vẫn lỗi, hãy chọn đúng card mạng cần theo dõi trong menu **Chọn Card Mạng** thay vì **Tự động phát hiện**.
3. Nếu vẫn lỗi, vào **Cài đặt...** → **Cài đặt Chung** → **Nâng cao** → **Chọn card mạng thủ công...**, chỉ đánh dấu mạng cần theo dõi rồi nhấn **Đồng ý** hai lần.

### Làm sao đặt màu riêng cho từng mục trên cửa sổ Taskbar?

Vào **Cài đặt...** → thẻ **Cửa sổ Taskbar**, đánh dấu **Mỗi thông số một màu riêng biệt**, rồi nhấn vào ô màu bên phải **Màu chữ** để mở hộp thoại **Đổi màu Cửa sổ Taskbar**. Nếu không đánh dấu tùy chọn này, bạn chỉ đặt được một màu chung cho toàn bộ chữ.

### “Khởi động cùng Windows” không hoạt động

Có hai cơ chế tự khởi động, chọn trong **Cài đặt Chung** → **Phương thức tự khởi động**:

* **Registry (Sổ đăng ký)**: tạo khóa `tsudev-trafficmonitor` trong `HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run`. Không cần quyền Quản trị viên, nhưng không dùng được nếu bạn đặt chương trình luôn **chạy với quyền Quản trị viên**.
* **Task Scheduler (cần quyền Quản trị viên)**: tạo một tác vụ trong Task Scheduler (mở qua `Control Panel\System and Security\Administrative Tools`). Bản có giám sát nhiệt độ cần cơ chế này vì thư viện đo phần cứng yêu cầu quyền cao.

Nguyên nhân phổ biến nhất là bạn **đã di chuyển thư mục chương trình** sau khi bật tự khởi động. Cách khắc phục: mở **Cài đặt Chung**, bỏ đánh dấu **Khởi động cùng Windows**, nhấn **Đồng ý**, rồi mở lại và đánh dấu lại; hoặc nhấn **Sửa lỗi Khởi động**.

Một số phần mềm diệt virus/bảo mật có thể chặn tự khởi động - hãy cho phép tsudev-trafficmonitor trong phần mềm đó.

Khi chuyển giữa hai cơ chế, chương trình sẽ tự xóa mục tự khởi động của cơ chế cũ.

### Chương trình báo “Không thể lưu cài đặt”

Nghĩa là chương trình không có quyền ghi vào thư mục của nó. Cách xử lý:

1. Di chuyển chương trình sang thư mục có quyền ghi (ví dụ `D:\Tools\tsudev-trafficmonitor`), **không** đặt trong `C:\Program Files` nếu không chạy với quyền Quản trị viên; hoặc
2. Vào **Cài đặt Chung** → **Nơi lưu tệp cấu hình** → chọn **Lưu trong ổ C (Thư mục Appdata)**. Cấu hình sẽ được lưu tại `C:\Users\<tên người dùng>\AppData\Roaming\tsudev-trafficmonitor`.

Nếu vẫn báo lỗi, tạo (hoặc sửa) tệp `global_cfg.ini` cạnh `tsudev-trafficmonitor.exe` với nội dung:

```ini
[config]
portable_mode = false
```

Nếu không tạo được tệp tại chỗ, hãy tạo ở Desktop rồi chép vào thư mục chương trình. Trường hợp cuối cùng, xóa tệp `C:\Users\<tên người dùng>\AppData\Roaming\tsudev-trafficmonitor\config.ini` (tệp sẽ được tạo lại).

Khi thư mục chương trình không ghi được, chương trình tự động lưu cấu hình vào AppData và tùy chọn **Lưu ngay trong thư mục phần mềm** sẽ bị vô hiệu hóa.

### Cấu hình bị mất sau khi khởi động lại máy

Nếu không thấy thông báo “Không thể lưu cài đặt” thì cấu hình vẫn được lưu bình thường. Nguyên nhân thường là trên máy có **nhiều bản tsudev-trafficmonitor** và cấu hình được lưu cạnh từng tệp exe. Ví dụ bạn chỉnh cấu hình ở `D:\software\tsudev-trafficmonitor\tsudev-trafficmonitor.exe` nhưng lúc khởi động máy lại chạy `D:\software1\tsudev-trafficmonitor\tsudev-trafficmonitor.exe`.

Cách xử lý:

1. Chuyển nơi lưu cấu hình sang **Lưu trong ổ C (Thư mục Appdata)**.
2. Xóa các bản tsudev-trafficmonitor không dùng, mở bản cần dùng → **Cài đặt Chung** → nhấn **Sửa lỗi Khởi động**.

### Giá trị tốc độ mạng bị hiển thị thiếu

Mỗi phông chữ có độ rộng ký tự khác nhau nên đôi khi giá trị bị cắt. Vào **Cài đặt...** → **Cửa sổ Taskbar** và chọn giá trị lớn hơn ở danh sách **số chữ số**.

### Đã bật “Click xuyên qua” thì tắt bằng cách nào?

Nhấp chuột phải vào biểu tượng tsudev-trafficmonitor ở **khay hệ thống** (góc phải thanh tác vụ), bỏ đánh dấu **Click xuyên qua (Bỏ qua chuột)**.

Khi bật tính năng này, cửa sổ nổi không nhận bất kỳ thao tác chuột nào; menu chuột phải của biểu tượng khay hệ thống giống hệt menu của cửa sổ nổi. Để tránh “mất” menu, biểu tượng khay hệ thống sẽ **tự động hiện** trong các trường hợp:

* Sau khi bật **Click xuyên qua**;
* Ẩn cửa sổ nổi trong khi không hiển thị cửa sổ Taskbar;
* Đóng cửa sổ Taskbar khi cửa sổ nổi đang ẩn;
* Đóng cửa sổ Taskbar khi đang bật **Click xuyên qua**.

### Màu chữ không phù hợp khi Windows dùng thanh tác vụ màu sáng

Trong **Cửa sổ Taskbar**, nhấn **Mẫu màu** và chọn mẫu cho chế độ sáng.

Bạn cũng có thể đánh dấu **Tự đổi chữ Trắng/Đen theo màu Windows** để chương trình tự đổi mẫu màu khi Windows chuyển giữa chế độ sáng/tối; nhấn **Tùy chỉnh đổi màu...** để chọn mẫu màu cho từng chế độ.

### Cửa sổ Taskbar không hiển thị khi bật HDR

Hãy thử tắt **Xóa màu nền (Trong suốt)** trong thẻ **Cửa sổ Taskbar**, hoặc trong **Cài đặt hình ảnh (Rendering)** chuyển sang kết xuất **Direct2D**.

### Mức sử dụng CPU khác với Task Manager

Từ Windows 10 trở đi, để số liệu khớp với Task Manager, vào **Cài đặt Chung** → **Nâng cao** → **Cách đo CPU:** và chọn **Theo Performance Counter**.

### Về tính năng đo nhiệt độ

Bản Lite được phát hành **không đo nhiệt độ**. Tính năng đo nhiệt độ chỉ có ở bản đầy đủ do bạn tự build (xem [README](./README.md#build-từ-mã-nguồn)).

Ở bản đầy đủ, tính năng này **tắt theo mặc định** vì có thể gây lỗi trên một số máy. Để bật, vào **Cài đặt Chung** → **Theo dõi Phần cứng**. Sau khi bật, các mục nhiệt độ sẽ xuất hiện trong **Sắp xếp hiển thị...** của cửa sổ Taskbar.

Tính năng này dựa trên thư viện mã nguồn mở [LibreHardwareMonitor](https://github.com/LibreHardwareMonitor/LibreHardwareMonitor). Nếu không hiển thị được nhiệt độ hoặc nhiệt độ sai, hãy tải LibreHardwareMonitor về chạy thử:

* Nếu LibreHardwareMonitor cũng không hiển thị được → vấn đề nằm ở thư viện/phần cứng, hãy báo cho dự án LibreHardwareMonitor.
* Nếu bản mới nhất của LibreHardwareMonitor hiển thị đúng → có thể thay tệp `LibreHardwareMonitorLib.dll` trong thư mục tsudev-trafficmonitor bằng tệp mới. **Chỉ tải tệp từ trang chính thức** của LibreHardwareMonitor và kiểm tra chữ ký/mã băm trước khi thay.

> **Cảnh báo:** tính năng giám sát phần cứng (nhiệt độ, mức sử dụng GPU) có thể làm tăng mức dùng CPU/RAM; một số người dùng gặp lỗi chương trình bị đóng đột ngột hoặc treo máy khi bật. Chỉ bật khi bạn chấp nhận các rủi ro này.

### Khi khởi động báo thiếu “MSVC\*.dll” hoặc “mfc\*.dll”

Cài đặt Microsoft Visual C++ Redistributable bản mới nhất từ trang chính thức của Microsoft:
[Latest supported Visual C++ Redistributable downloads | Microsoft Learn](https://learn.microsoft.com/cpp/windows/latest-supported-vc-redist)

### Plugin

Cách cài plugin và khắc phục sự cố: trang [Plugin](./Plugin.md); tự viết plugin: [Phát triển plugin](./PhatTrienPlugin.md).

> **Lưu ý bảo mật:** plugin là tệp DLL chạy **cùng quyền với tsudev-trafficmonitor**. Chỉ cài plugin từ nguồn bạn tin cậy; plugin không rõ nguồn gốc có thể chứa mã độc.

### Làm sao biết tệp tải về là bản thật?

Chỉ tải từ [trafficmonitor.tsudev.com](https://trafficmonitor.tsudev.com/tai-ve/) hoặc [Releases](https://github.com/tsudev-org/tsudev-trafficmonitor/releases/latest) của repo. Đối chiếu SHA-256 của tệp zip với `SHA256SUMS.txt` (PowerShell: `Get-FileHash <tệp>.zip`); khi cập nhật trong ứng dụng (từ 26.11.0905), ứng dụng tự tải gói và chỉ lưu vào thư mục Tải về khi SHA-256 khớp thông tin phiên bản có chữ ký; nếu báo SHA-256 không khớp, đừng tải lại bằng cách khác mà hãy báo qua GitHub Issues. Các bước đầy đủ, gồm chứng thực build: [SECURITY.md](./SECURITY.md).

### Chương trình bị đóng đột ngột (crash)

1. Khởi động lại chương trình và **tắt toàn bộ** mục trong **Cài đặt Chung** → **Theo dõi Phần cứng** - phần lớn sự cố liên quan đến tính năng này.
2. Nếu vẫn lỗi, vào **Nâng cao** → **Quản lý Plugin...** và vô hiệu hóa mọi plugin rồi khởi động lại. (Nếu chương trình không mở được, xóa các tệp `.dll` trong thư mục `plugins` cạnh `tsudev-trafficmonitor.exe`.)
3. Nếu đã loại trừ hai nguyên nhân trên, hãy [tạo issue báo lỗi](https://github.com/tsudev-org/tsudev-trafficmonitor/issues/new/choose) kèm nội dung trong cửa sổ báo lỗi.

### Góp ý, báo lỗi

Vui lòng gửi qua [GitHub Issues của tsudev-trafficmonitor](https://github.com/tsudev-org/tsudev-trafficmonitor/issues). Hãy mô tả chi tiết lỗi, thông báo lỗi, các bước đã thử, kèm ảnh chụp màn hình và tệp cấu hình (**Cài đặt Chung** → **Mở thư mục**) nếu có thể. **Không** đính kèm thông tin nhạy cảm.

Lỗ hổng bảo mật vui lòng báo riêng theo hướng dẫn trong [SECURITY.md](./SECURITY.md), không đăng công khai.
