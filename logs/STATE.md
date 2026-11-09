# STATE.md - Trạng thái project

> Agent đọc file này ở đầu mỗi phiên và cập nhật ở cuối mỗi phiên.
> Quy trình: `.standards/docs/AGENT_PROTOCOL.md`.

## Hàng đợi task (làm từ trên xuống)

- (trống) - việc tùy chọn còn lại xem `AGENTS.md` mục B.4

## Đang thực hiện

| Task | Agent | Bắt đầu |
| --- | --- | --- |

## Đã hoàn thành (mới nhất trên cùng)

- 09/11/2026 - Kiểm tra khả năng truy cập website bằng axe-core trong CI (WCAG 2.2 AA, sáng + tối); sửa tương phản menu và khối mã cuộn (PR #26)
- 09/11/2026 - Kiểm thử tự động C++ cho kênh cập nhật: `TrafficMonitor/UpdateSecurity.h` + `tests/UpdateSecurityTests.cpp` (73 phép kiểm, job "Kiểm thử C++") (PR #25)
- 09/11/2026 - CI "Chuẩn tsudev": chạy gitleaks bằng bản phát hành kiểm SHA-256 (gitleaks-action bị chính sách Actions chặn) (PR #24)
- 09/11/2026 - Phát hành 26.11.0902 (PR #20-#22): icon cú tsudev, skin đồ thị mạng sáng/tối, gỡ tên dự án gốc; đã kiểm sha256/attestation/chữ ký, thử cập nhật 26.11.0901 -> 26.11.0902 trên Windows, icon khay sáng/tối; website đã deploy
- 09/11/2026 - Tài liệu tiếng Anh, CHANGELOG.md (PR #18); gỡ tên/liên kết dự án gốc, bỏ kiểm tra phiên bản plugin bên thứ ba (PR #19); icon cú tsudev, skin đồ thị mạng (PR #20); áp dụng bộ quy ước v3.1.1
- 09/11/2026 - Phát hành 26.11.0901 (khuôn phiên bản mới)

## Quyết định quan trọng

> Quyết định kiến trúc lớn thì viết ADR riêng theo `docs/templates/ADR.md` và chỉ
> ghi một dòng tham chiếu ở đây.

- 09/11/2026 - Gỡ tên tác giả dự án gốc khỏi repo ở mức tối đa giấy phép Anti 996 cho phép: chỉ giữ trong LICENSE, LICENSE_CN, LICENSE_VI.md, NOTICE.md
- 09/11/2026 - Không kiểm tra phiên bản plugin qua mạng (bỏ nguồn bên thứ ba); giữ API plugin v8
- 09/11/2026 - Người dịch gói vi/en ghi tsudev; gói ngôn ngữ khác để trống người dịch

## Sự cố bảo mật

> Ghi theo `.standards/docs/SECURITY_BASELINE.md` mục 9. Để trống nếu chưa có.

- (chưa có)
