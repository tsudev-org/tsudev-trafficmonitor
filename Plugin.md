**Tiếng Việt | [English](./Plugin_en-us.md)** · [Trang chủ](./README.md) · [Hỏi đáp](./Help.md) · [Phát triển plugin](./PhatTrienPlugin.md)

# Plugin — TrafficMonitor-VN

Trang này được mở từ liên kết **Tải thêm Plugin** trong hộp thoại **Quản lý Plugin**.

Plugin là tệp `.dll` bổ sung mục hiển thị mới (ngày giờ, thời tiết, pin, nhiệt độ...) cho cửa sổ nổi và cửa sổ trên thanh tác vụ.

> ⚠️ **Plugin chạy cùng quyền với TrafficMonitor** và có thể làm mọi việc chương trình làm được. Chỉ cài plugin từ nguồn bạn tin cậy, kiểm tra chữ ký hoặc mã băm do tác giả plugin công bố nếu có.

## Cài plugin

1. Tải tệp plugin `.dll` **đúng kiến trúc** với bản TrafficMonitor-VN bạn dùng (x86, x64 hoặc ARM64EC — xem trong **Giới thiệu**).
2. Thoát TrafficMonitor.
3. Chép tệp `.dll` vào thư mục `plugins` cạnh `TrafficMonitor.exe` (tạo thư mục nếu chưa có).
4. Mở lại TrafficMonitor. Plugin được nạp khi khởi động.
5. Bật mục hiển thị của plugin: menu chuột phải → **Hiện thêm thông số**, hoặc **Cài đặt** → **Cửa sổ Taskbar** → **Sắp xếp các mục...**.

Quản lý plugin tại **Cài đặt** → **Cài đặt Chung** → **Nâng cao** → **Quản lý Plugin...**: xem thông tin, tùy chỉnh, tắt plugin, mở thư mục plugin. Tệp cấu hình của plugin nằm trong thư mục `plugins` của thư mục cấu hình (mặc định `%AppData%\TrafficMonitor\plugins`).

## Nguồn plugin

* **Plugin mẫu** `PluginDemo` (ngày, giờ, mục tự vẽ) được dựng cùng mã nguồn của TrafficMonitor-VN trong thư mục [`PluginDemo`](./PluginDemo); dùng để thử nghiệm và làm mẫu khi viết plugin.
* **Plugin của dự án gốc**: tác giả TrafficMonitor (zhongyang219) phát hành bộ plugin riêng tại kho [TrafficMonitorPlugins](https://github.com/zhongyang219/TrafficMonitorPlugins). Các plugin này **không do TrafficMonitor-VN phát triển hay kiểm duyệt**, nhưng dùng chung giao diện plugin nên chạy được trên TrafficMonitor-VN. Hộp thoại **Quản lý Plugin** đánh dấu khi có phiên bản mới hơn của các plugin này.
* **Plugin của bạn**: xem [Phát triển plugin](./PhatTrienPlugin.md). Muốn giới thiệu plugin trên trang này, hãy mở [Issue](https://github.com/tsudev-tsudev/tsudev-trafficmonitor/issues) kèm liên kết mã nguồn.

## Khắc phục sự cố

* **Plugin không hiện trong danh sách hoặc báo không tải được:** sai kiến trúc (ví dụ plugin x86 cho bản x64), thiếu thư viện đi kèm, hoặc plugin dùng phiên bản giao diện mới hơn chương trình hỗ trợ.
* **Chương trình bị crash sau khi cài plugin:** xóa (hoặc chuyển ra ngoài) tệp `.dll` vừa cài khỏi thư mục `plugins` rồi mở lại; báo lỗi cho tác giả plugin.
* Xem thêm [Hỏi đáp](./Help.md).
