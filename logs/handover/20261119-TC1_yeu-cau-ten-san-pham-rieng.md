# PHIẾU YÊU CẦU - TC1 (tsudev-trafficmonitor): tên sản phẩm riêng, bỏ tên dự án gốc khỏi nội dung hiển thị

- **Mã phiếu**: 20261119-TC1 (phiếu con; phiếu tổng `~/projects/logs/handover/20261119-TC1_ten-goi-rieng-toan-he.md`)
- **Từ**: phiên tsufiles-3c, 19/11/2026, theo yêu cầu trực tiếp của chủ dự án
- **Đến**: phiên tsudev-trafficmonitor kế tiếp
- **Trạng thái**: MỞ - bước 1 CHỜ chủ dự án

Chủ dự án 19/11: mọi sản phẩm hệ tsudev chỉ dùng tên gọi/nội dung/giao thức của chính nó; nội dung hiển thị không đề
cập tên gọi cũ (như tsufiles đã bỏ "Filee", lotus-share đã bỏ "LocalSend"). Ngoại lệ pháp lý: phiếu tổng mục 2 -
`LICENSE`, `LICENSE_CN`, `NOTICE.md` GIỮ thông báo bản quyền "zhongyang219" (Anti-996 buộc giữ).

## 1. Hiện trạng (đo `python3 -I ~/projects/logs/tools/ra_ten_cu.py --dong tsudev-trafficmonitor`, 19/11/2026)

Khác tsufiles/lotus-share: **chính tên sản phẩm** vẫn là tên dự án gốc - "tsudev-trafficmonitor", `TrafficMonitor.exe`,
tên miền `trafficmonitor.tsudev.com`, winget `tsudev.TrafficMonitor` (PR winget-pkgs #477193 chờ duyệt). Câu hiển thị:
README "bản Việt hóa và gia cố bảo mật của TrafficMonitor", `UpdateLog/update_log_*.md:169` "Dựa trên TrafficMonitor 1.86",
`include/PluginInterface.h`, `ForkConfig.h` (11 chỗ), `TsudevTrafficMonitor.cpp` (7 chỗ).

## 2. Việc

1. 👤 **Chủ dự án chọn tên sản phẩm** theo khuôn hệ (TsuFiles, Tsu Swico, Tsu Cwico, Tsu Contact…), ví dụ gợi ý
   "Tsu Traffic" / repo `tsutraffic` - agent KHÔNG tự đặt. Kèm: tên miền mới hay giữ `trafficmonitor.tsudev.com` (đổi
   thì cần chuyển hướng 301 vĩnh viễn).
2. Sau khi có tên: ADR đổi tên theo mẫu tsufiles ADR-0003 (đọc mục 5-8 ở `tsudev-org/tsufiles`), bắt buộc:
   - Updater đã cài (`version_utf8.info`, `ForkConfig.h`): bản mới phải vẫn được bản cũ nhận; giữ tên tệp phát hành cũ
     song song ít nhất một bản (cùng bài học phiếu V1 mục 6 của tsufiles).
   - Di trú dữ liệu `%AppData%\tsudev-trafficmonitor` + mục tự khởi động (đã có khuôn từ lần đổi `TrafficMonitor` →
     `tsudev-trafficmonitor`, `UpdateLog` dòng 24-25).
   - Đổi tên repo GitHub trong `tsudev-org`, rồi tên cũ `tsudev-trafficmonitor` là BẤT KHẢ XÂM PHẠM: ghi vào
     `~/.claude/CLAUDE.md` như các repo đã đổi (không tạo/fork/đổi tên repo nào thành tên cũ).
   - winget/Scoop/kho gói: phiếu cho `linux-packages`, `scoop-bucket`, PR winget mới.
3. Nội dung hiển thị: bỏ câu "bản Việt hóa của TrafficMonitor", "Dựa trên TrafficMonitor 1.86" (VERSIONING v3 mục bản
   fork - chủ dự án đã bỏ dòng "Dựa trên" ở Lotus Share 12/11; áp tương tự). Plugin API (`PluginInterface.h`) đổi tên
   tiền tố có thể phá plugin bên thứ ba - xét riêng, ghi quyết định.
4. Nghiệm thu: `ra_ten_cu.py` 0 dòng hiển thị ngoài mục 2 phiếu tổng; bản cũ cập nhật lên bản mới chạy được (thử thật).

## 3. Trả lời

Ghi tên đã chọn, số PR, bảng `ra_ten_cu.py` trước/sau vào mục 6 phiếu tổng.
