# PHIẾU NHẮC - Theo dõi PR winget `tsudev.TrafficMonitor` 26.11.1001 chờ Microsoft duyệt

- **Mã phiếu**: 20261112-03
- **Từ**: phiên tsudev-trafficmonitor-d8 (12/11/2026) - **Đến**: các phiên sau của `tsudev-trafficmonitor`
- **Trạng thái**: MỞ - đóng khi gói đã lên winget VÀ bản 26.11.1201 (hoặc mới hơn) đã được nộp

## 1. Hiện trạng (kiểm 12/11/2026)

- PR: https://github.com/microsoft/winget-pkgs/pull/477193 - "New package: tsudev.TrafficMonitor version 26.11.1001",
  tạo 11/11/2026 03:39 UTC từ fork `tsudev-tsudev/winget-pkgs`, nhánh `tsudev.TrafficMonitor-26.11.1001`.
- Trạng thái: OPEN, `REVIEW_REQUIRED`. Nhãn: `Azure-Pipeline-Passed`, `New-Package`, **`Validation-No-Executables`**.
- Manifest (`manifests/t/tsudev/TrafficMonitor/26.11.1001/`): zip + `NestedInstallerType: portable`,
  `RelativeFilePath: TrafficMonitor\TrafficMonitor.exe`, `PortableCommandAlias: tsudev-trafficmonitor`; URL tải
  `tsudev-org/...V26.11.1001-vn/...`, SHA-256 khớp bản 1001. Đúng với gói 1001.

## 2. Việc mỗi phiên làm (chỉ ĐỌC, không sửa PR nếu chủ dự án chưa yêu cầu)

```bash
gh pr view 477193 --repo microsoft/winget-pkgs --json state,reviewDecision,labels,mergedAt \
  -q '"\(.state) \(.reviewDecision) merged=\(.mergedAt) \([.labels[].name]|join(","))"'
gh pr view 477193 --repo microsoft/winget-pkgs --json comments -q '.comments[-3:][] | "\(.author.login): \(.body[0:300])"'
```

Ghi một dòng kết quả vào mục 4 và báo chủ dự án nếu có thay đổi.

## 3. Xử lý theo kết quả

| Kết quả | Việc |
| --- | --- |
| Còn OPEN, không có yêu cầu mới | Không làm gì; ghi dòng kiểm ở mục 4 |
| Người duyệt/bot yêu cầu sửa (vd. nhãn `Needs-Author-Feedback`, `Validation-*-Error`) | Báo chủ dự án; sửa trên nhánh fork khi được đồng ý. `Validation-No-Executables` thường do bộ quét không thấy exe trong zip - kiểm `RelativeFilePath` khớp cấu trúc zip |
| MERGED | Nộp bản mới 26.11.1201 (hoặc bản mới nhất): giữ `PackageIdentifier: tsudev.TrafficMonitor`, **đổi `RelativeFilePath` thành `tsudev-trafficmonitor\tsudev-trafficmonitor.exe`** (đổi tên thư mục/exe từ 26.11.1201), URL + SHA-256 lấy từ `SHA256SUMS.txt` của bản phát hành; dùng `packaging/package.json` của repo này |
| CLOSED không gộp | Đọc lý do, báo chủ dự án; nộp lại thẳng bản mới nhất với `RelativeFilePath` tên mới |
| Chờ quá lâu (vd. > 3 tuần) | Hỏi chủ dự án có muốn cập nhật PR sang bản mới nhất (thêm thư mục phiên bản mới, đường dẫn exe tên mới) |

Lưu ý: KHÔNG tạo lại/đổi tên repo dưới `tsudev-tsudev` trùng tên repo đã chuyển sang `tsudev-org`
(`~/.claude/CLAUDE.md`). Fork `tsudev-tsudev/winget-pkgs` là fork của microsoft/winget-pkgs, được phép dùng.
Trang Tải về (`website/src/pages/tai-ve.astro`) đang ghi "Gói winget đang chờ Microsoft duyệt" - sửa thành lệnh
`winget install tsudev.TrafficMonitor` khi gói đã lên.

## 4. Nhật ký kiểm (mỗi lần một dòng)

- 12/11/2026 - OPEN, REVIEW_REQUIRED; nhãn Azure-Pipeline-Passed, New-Package, Validation-No-Executables; chưa có bình luận của người duyệt.
- 12/11/2026 (phiên sau) - Không đổi: OPEN, REVIEW_REQUIRED; cùng 3 nhãn; chỉ có bình luận của bot policy-service.
