# PHIẾU YÊU CẦU - Đồng bộ mục Ủng hộ (hộp thoại Donate)

- **Mã phiếu**: 20261112-01
- **Từ**: phiên tsudev-files-01 (12/11/2026), theo yêu cầu trực tiếp của chủ dự án
- **Đến**: phiên làm việc trên `tsudev-trafficmonitor`
- **Trạng thái**: ĐÃ XỬ LÝ (PR gỡ Gitee + Ủng hộ tsudev, phiên tsudev-trafficmonitor-d8, 12/11/2026)

## 1. Đọc trước

Phiếu gốc `~/projects/logs/handover/20261112-01_yeu-cau-dong-bo-ung-ho-he-sinh-thai.md`: thông tin Ủng hộ chuẩn (VIB `056485506`, NGUYEN TRANG TINH SU, chuỗi VietQR, ảnh
`vietqr-vib.svg` + SHA-256) và kiến trúc bắt buộc (một nguồn dữ liệu, app chỉ trỏ về trang, không dịch vụ QR bên thứ
ba, ghim SHA-256 + kiểm VietQR lúc build, CODEOWNERS, chính sách kho di động, không lưu bí mật).
Bản tham chiếu: repo `tsudev-files` (`website/src/data/ung-ho.json`, `website/src/lib/ung-ho.ts`,
`website/public/ung-ho/vietqr-vib.svg`, `tools/ung-ho/tao-vietqr.py`).

## 2. Việc riêng của repo này (khảo sát nhanh 12/11/2026 - cần rà lại)

- `TrafficMonitor/AboutDlg.cpp`: lối vào `CDonateDlg` đang bị chú thích; rà `DonateDlg.*` và tài nguyên ảnh (mã
  WeChat/Alipay của tác giả gốc nếu còn trong `.rc`/`res/`) - gỡ hẳn tài nguyên cũ.
- Chuỗi `IDS_DONATE_ATHOUR` ("Donate the author"), `TXT_ABOUT_DONATE` trong `language/*.ini`: đổi thành mở trang ủng
  hộ tsudev (khuyến nghị: nút trong hộp Giới thiệu mở `https://tsudev.com/ung-ho/`, không nhúng QR vào exe).
- `.github/FUNDING.yml` trỏ về trang ủng hộ tsudev. Giữ ghi công và giấy phép của tác giả gốc.

## 3. Việc chung

Theo phiếu gốc mục 4: gỡ kênh cũ, `FUNDING.yml`, trang/nút Ủng hộ, cổng kiểm + test, ghi `logs/STATE.md`.

## 4. Kết quả xử lý (phiên nhận điền, và ghi một dòng vào phiếu gốc mục 6)

- Không còn `DonateDlg`/ảnh QR của tác giả gốc trong mã (chỉ còn ID tài nguyên thừa và mã chú thích - đã gỡ: `IDB_DONATE_*`,
  `IDC_DONATE_PIC`, `OnStnClickedStaticDonate`). Gỡ chuỗi `IDS_DONATE_ATHOUR` ("Donate the author") khỏi 15 gói ngôn ngữ.
- Hộp Giới thiệu: thêm liên kết "Ủng hộ tsudev" (`TXT_ABOUT_DONATE`, `IDC_STATIC_DONATE`) mở `TSUDEV_DONATE_URL` (xem dòng CẬP NHẬT) (khai trong `ForkConfig.h`, `check_urls.py` canh). Exe KHÔNG chứa số tài khoản/QR.
- `.github/FUNDING.yml` và chân trang website trỏ về trang Ủng hộ (xem dòng CẬP NHẬT).
- CẬP NHẬT 12/11/2026: trang trung tâm `tsudev.com/ung-ho/` chưa có (404, phiếu của repo `tsudev` còn CHỜ NHẬN) nên
  repo này dựng trang riêng `https://trafficmonitor.tsudev.com/ung-ho/` theo bản tham chiếu tsudev-files: dữ liệu
  `website/src/data/ung-ho.json`, kiểm lúc build `website/src/lib/ung-ho.ts` (CRC VietQR, NAPAS, VND, BIN + số tài khoản,
  SHA-256 ảnh QR - đã thử tráo ảnh: build dừng), ảnh `website/public/ung-ho/vietqr-vib.svg` (SHA-256 `7209698f…`),
  `.github/CODEOWNERS`. Ứng dụng (`TSUDEV_DONATE_URL`), FUNDING.yml, chân trang đều trỏ về trang này.

## 5. Bổ sung 12/11/2026 - phương án theo nền tảng (đã chốt)

Áp dụng phiếu `~/projects/logs/handover/20261112-02_quyet-dinh-ung-ho-theo-nen-tang.md` (thay mục 3.5 của phiếu gốc):
web hiện đầy đủ; desktop/Android chỉ nút mở trang web kèm câu "tự nguyện, 100% tới nhà phát triển, không mở khóa gì";
iOS App Store ẩn hoàn toàn. Ghi một dòng kết quả vào mục 5 của phiếu đó.
