# Hướng dẫn tạo skin (giao diện) cho TrafficMonitor-VN

Skin quyết định hình nền, phông chữ, màu chữ và vị trí các mục trên **cửa sổ nổi** (cửa sổ chính) của TrafficMonitor. Cửa sổ Taskbar không dùng skin.

> Tài liệu này được viết dựa trên mã nguồn đọc skin (`TrafficMonitor/SkinFile.cpp`). Hướng dẫn gốc (tiếng Trung) của tác giả zhongyang219 nằm trong Wiki của [dự án gốc](https://github.com/zhongyang219/TrafficMonitor).

## 1. Cấu trúc thư mục

Mỗi skin là một thư mục con trong thư mục `skins` (nằm cạnh `TrafficMonitor.exe`). **Tên thư mục chính là tên skin.**

```
TrafficMonitor.exe
skins\
  0_MacDinh\
    background.png      ← nền khi "Hiện thêm thông số" TẮT (cửa sổ nhỏ)
    background_l.png    ← nền khi "Hiện thêm thông số" BẬT (cửa sổ lớn)
    skin.xml            ← tệp cấu hình skin (khuyến nghị)
```

| Tệp | Bắt buộc | Ghi chú |
|---|---|---|
| `background.png` / `background.bmp` | Có | Ảnh nền nhỏ. Ưu tiên PNG; nếu nạp PNG thất bại sẽ dùng BMP. |
| `background_l.png` / `background_l.bmp` | Có | Ảnh nền lớn. |
| `skin.xml` | Khuyến nghị | Định dạng mới, hỗ trợ đủ mọi mục (nhiệt độ, GPU, ổ cứng, plugin...). |
| `skin.ini` | Không | Định dạng cũ, chỉ để tương thích skin phiên bản trước 1.80. |

* **PNG** hỗ trợ nền **trong suốt** (kênh alpha). Khi dùng PNG, cửa sổ được vẽ bằng `UpdateLayeredWindow` + GDI+.
* **BMP** không hỗ trợ trong suốt; có thể dùng độ trong suốt chung của cửa sổ (**Độ trong suốt...** trong menu chuột phải).
* Nên đặt tên thư mục bằng **chữ không dấu, không khoảng trắng** (ví dụ `XanhNhat_DoiMau`) để tránh lỗi đường dẫn.

## 2. Tệp `skin.xml`

Tệp phải lưu bằng **UTF-8**. Ví dụ đầy đủ:

```xml
<root>
  <skin>
    <text_color>16777215,16777215,16777215,16777215</text_color>
    <specify_each_item_color>0</specify_each_item_color>
    <skin_author>Tên của bạn</skin_author>
    <font name="Segoe UI" size="9" style="0"/>
    <display_text>
      <up>Lên: </up>
      <down>Xuống: </down>
      <cpu>CPU: </cpu>
      <memory>RAM: </memory>
    </display_text>
  </skin>
  <layout text_height="20" no_label="0">
    <layout_l width="220" height="60">
      <up     x="6"   y="4"  width="100" align="0" show="1"/>
      <down   x="114" y="4"  width="100" align="0" show="1"/>
      <cpu    x="6"   y="32" width="100" align="0" show="1"/>
      <memory x="114" y="32" width="100" align="0" show="1"/>
    </layout_l>
    <layout_s width="220" height="28">
      <up   x="6"   y="4" width="100" align="0" show="1"/>
      <down x="114" y="4" width="100" align="0" show="1"/>
    </layout_s>
  </layout>
  <preview width="230" height="100">
    <l x="0" y="0"/>
    <s x="0" y="66"/>
  </preview>
</root>
```

### 2.1. Nút `<skin>` — thông tin chung

| Nút | Ý nghĩa |
|---|---|
| `<text_color>` | Danh sách màu chữ, phân cách bằng dấu phẩy, theo thứ tự các mục hiển thị. Màu ghi dạng **số thập phân của COLORREF** (`0xBBGGRR`): ví dụ trắng = `16777215`, đỏ (`RGB(255,0,0)`) = `255`, xanh dương (`RGB(0,0,255)`) = `16711680`. Nếu thiếu, các mục còn lại dùng màu đầu tiên. |
| `<specify_each_item_color>` | `1`: mỗi mục một màu theo `text_color`; `0`: mọi mục dùng màu đầu tiên. |
| `<skin_author>` | Tên tác giả, hiển thị trong hộp thoại **Đổi giao diện...**. |
| `<font name=".." size=".." style=".."/>` | Phông chữ mặc định của skin. `style` là tổng các bit: `1` đậm, `2` nghiêng, `4` gạch chân, `8` gạch ngang. Với tiếng Việt nên dùng **Segoe UI**, **Arial** hoặc **Tahoma**. |
| `<display_text>` | Nhãn mặc định cho từng mục (tên nút con xem bảng ở mục 2.3). Người dùng vẫn có thể sửa trong **Đổi chữ hiển thị...**. |

### 2.2. Nút `<layout>` — bố cục

* Thuộc tính `text_height`: chiều cao dòng chữ (pixel ở 96 DPI). `no_label="1"`: ẩn toàn bộ nhãn, chỉ hiện giá trị.
* `<layout_l>`: bố cục khi **Hiện thêm thông số** BẬT; `<layout_s>`: khi TẮT. Thuộc tính `width`/`height` là kích thước cửa sổ, nên khớp kích thước ảnh nền tương ứng.
* Mỗi mục con có các thuộc tính:

| Thuộc tính | Ý nghĩa |
|---|---|
| `x`, `y` | Vị trí góc trên bên trái (pixel ở 96 DPI; chương trình tự co giãn theo DPI màn hình). |
| `width` | Chiều rộng vùng hiển thị. |
| `align` | `0` căn trái, `1` căn phải, `2` căn giữa, `3` căn đều hai bên (nhãn bên trái, giá trị bên phải). |
| `show` | `1` hiển thị, `0` ẩn. |

### 2.3. Tên nút của các mục hiển thị

| Tên nút | Mục |
|---|---|
| `up` / `down` / `total_speed` | Tốc độ tải lên / tải xuống / tổng |
| `today_traffic` | Lưu lượng đã dùng hôm nay |
| `cpu` / `cpu_freq` / `memory` | Mức dùng CPU / tần số CPU / mức dùng bộ nhớ |
| `gpu` / `hdd` | Mức dùng GPU / ổ cứng |
| `cpu_temperature` / `gpu_temperature` / `hdd_temperature` / `main_board_temperature` | Nhiệt độ CPU / GPU / ổ cứng / bo mạch chủ (chỉ bản có giám sát nhiệt độ) |

### 2.4. Hiển thị mục của plugin — nút `<plugin_map>`

Để đặt mục của plugin lên skin, khai báo một tên nút tùy ý ánh xạ tới **ID của mục plugin**, rồi dùng tên đó trong `<layout_l>`/`<layout_s>`:

```xml
<plugin_map>
  <dong_ho>ra1YX2g1</dong_ho>
</plugin_map>
...
<dong_ho x="6" y="60" width="100" align="0" show="1"/>
```

ID của mục plugin xem trong **Quản lý Plugin...** → **Chi tiết** của plugin đó.

### 2.5. Nút `<preview>` — ảnh xem trước

Quy định cách ghép ảnh xem trước trong hộp thoại **Đổi giao diện...**: `width`/`height` là kích thước vùng xem trước; `<l>` và `<s>` là vị trí đặt cửa sổ lớn và cửa sổ nhỏ trong vùng đó.

## 3. Quy trình tạo skin nhanh

1. Sao chép một skin có sẵn (ví dụ `skins\0_MacDinh`) thành thư mục mới.
2. Thay `background.png` và `background_l.png` bằng ảnh của bạn (giữ nguyên kích thước hoặc sửa `width`/`height` trong `skin.xml` cho khớp).
3. Sửa `skin.xml`: tác giả, phông chữ, màu, vị trí các mục.
4. Mở TrafficMonitor → menu chuột phải → **Chức năng khác** → **Đổi giao diện...**, chọn skin mới để xem trước và áp dụng. Sau mỗi lần sửa, chọn sang skin khác rồi chọn lại để nạp lại.

## 4. Lưu ý an toàn

* Skin chỉ gồm ảnh và tệp cấu hình, **không chứa mã thực thi**. Nếu một gói “skin” tải về có kèm tệp `.exe`, `.dll`, `.bat`, `.ps1`, `.vbs`... thì **không được chạy** các tệp đó.
* Chỉ tải skin từ nguồn tin cậy. Danh sách skin có sẵn và cách cài thêm: [Kho giao diện](./KhoGiaoDien.md).
