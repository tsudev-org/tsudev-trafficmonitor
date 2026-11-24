# PHIẾU BÀN GIAO - Uỷ quyền tự động làm hết P5-P8 + gửi báo nhầm Malwarebytes

- **Mã phiếu**: 20261124-UQ1
- **Từ**: phiên tsudev-trafficmonitor SES-20261124-1215-6a21 (ghi theo lời chủ dự án trong chat 24/11/2026) - **Đến**: các phiên sau của `tsudev-trafficmonitor`
- **Thời điểm**: 24/11/2026
- **Trạng thái**: MỞ - còn hiệu lực tới khi P5-P8 trong `logs/STATE.md` xong hết và phiếu AV1 đã gửi Malwarebytes
- **Nhánh git**: không có nhánh dang dở (`main` = `9eeab05`, #82)

## 0. Uỷ quyền của chủ dự án (nguyên ý, 24/11/2026)

Chủ dự án **đồng ý uỷ quyền cho agent tự động làm mọi bước cần thiết tiếp theo cho tới khi xong toàn bộ task/phase**
(P5 → P8 ở "Hàng đợi task" của `logs/STATE.md`), **gồm cả việc agent tự mở trình duyệt, soạn, nhập và điền đầy đủ
thông tin/biểu mẫu để gửi báo nhầm cho Malwarebytes với bản arm64ec**.

Phạm vi được làm KHÔNG cần hỏi lại:

- Tạo nhánh, commit, mở PR, chờ CI, squash merge (quy trình thường của repo; không push thẳng `main`).
- Deploy website (`XDG_CONFIG_HOME=$HOME/.config/wrangler-tsudev npm run deploy`), thêm custom domain
  `tramon.tsudev.com` + chuyển hướng 301 từ `trafficmonitor.tsudev.com` (P6).
- Thử thật trên máy Windows của chủ dự án bằng build CI (memory `thu-giao-dien-tren-windows`): sao lưu
  `%AppData%\tsudev-trafficmonitor` trước, khôi phục/mở lại bản của chủ máy sau khi thử.
- Phát hành bản đầu tiên tên Tsu Tramon: sửa `release.yml` + environment `release` nhận thẻ `v*`, đẩy thẻ đã ký
  `v26.11.DDNN`, tự duyệt environment bằng `gh api .../pending_deployments` (đã làm được từ 26.11.0905), kiểm
  SHA256SUMS/attestation/chữ ký/VirusTotal (P6).
- Đổi tên repo `tsudev-org/tsudev-trafficmonitor` → `tsudev-org/tsutramon` SAU khi phát hành, kiểm 4 đường chuyển hướng
  theo ADR-0001 mục 3.8; gãy thì đổi ngược ngay (P7). Sau đó báo điều phối thêm tên cũ vào danh sách BẤT KHẢ XÂM PHẠM.
- Viết phiếu + nhắn (ListAgents → SendMessage) cho `scoop-bucket`, `linux-packages`, `tsudev` (P7); nộp manifest winget
  mới khi #477193 đã gộp.
- **Gửi báo nhầm Malwarebytes** (mục 2 dưới đây).

## 1. Giới hạn vẫn giữ (uỷ quyền không gỡ các ràng buộc này)

- ⛔ Không tạo/fork/đổi tên repo nào thành tên cũ (`~/.claude/CLAUDE.md`); không sửa `LICENSE*`, `.standards/`.
- Bí mật (SEC1): KHÔNG nhận mật khẩu/token qua chat, KHÔNG in giá trị bí mật. Cần đăng nhập Malwarebytes (hoặc dịch vụ
  khác) mà trình duyệt chưa có phiên ⇒ để chủ dự án tự gõ mật khẩu/2FA trong cửa sổ trình duyệt, agent làm tiếp phần
  còn lại. Không tự tạo tài khoản mới đứng tên chủ dự án nếu chưa có - hỏi trước.
- Khoá VirusTotal đã lộ (phiếu SEC1 mục 4): việc thay khoá vẫn là của chủ dự án (khoá dùng chung cả hệ, cần đăng nhập
  VirusTotal). Agent chỉ nhắc, và sau khi khoá mới có thì dùng theo `tsu-nhan-bi-mat`.
- Nội dung gửi đi đứng tên chủ dự án/TsuDEV, không ghi "do AI thực hiện" (`~/.claude/CLAUDE.md`).
- Việc khó đảo ngược ngoài phạm vi mục 0 (xoá repo/thẻ/Release, đổi khoá ký cập nhật, đổi giấy phép) vẫn phải hỏi.

## 2. Gửi báo nhầm Malwarebytes (phiếu AV1) - các bước cụ thể

1. Đo lại trước khi gửi (khoá đọc thẳng từ tệp, không in - bài học LES-20261124-1313-d01a):
   `curl -fsS -H "x-apikey: $(head -1 ~/.config/tsudev/virustotal.env)" https://www.virustotal.com/api/v3/files/11ecdaa0bdd646cd89b6295ba2660722efa0f3e8921b5ab6eb81f2581d429009`
   Malwarebytes đã hết cờ ⇒ không gửi, ghi ngày gỡ vào `docs/store/antivirus.md` và ĐÓNG AV1.
2. Mở trình duyệt tới https://forums.malwarebytes.com/forum/122-false-positives/ → "Start new topic". Chưa đăng nhập ⇒
   làm theo mục 1 (chủ dự án tự đăng nhập).
3. Tiêu đề: `False positive report: TsuDEV TrafficMonitor (1 file)`. Nội dung: thư mục 3.8 của
   `~/projects/logs/handover/20261122-AV1_bao-nham-antivirus-toan-he.md`, **đã sửa 2 câu** theo `docs/store/antivirus.md`
   mục 2 (thay câu SignPath bằng câu SHA256SUMS + SLSA + build tái lập; bỏ chữ "PRIVACY"). Đính link VirusTotal và Release
   `V26.11.1202-vn`. Nếu biểu mẫu cho đính tệp: đính zip arm64ec tải từ Release (kiểm SHA-256 trước).
4. Chụp màn hình trước khi bấm gửi và sau khi gửi (lưu ngoài repo, vd scratchpad); ghi ngày gửi + link chủ đề vào
   `docs/store/antivirus.md` mục 2 và phiếu `20261122-AV1` mục 4 (qua PR).
5. Đo lại VT sau 3-7 ngày; hết cờ ⇒ ghi ngày gỡ, đổi AV1 → ĐÓNG.

## 2b. Việc dang dở còn lại

- [ ] P5 → P8 theo `logs/STATE.md` (P5 có ghi chú khảo sát mã sẵn).
- [ ] Mục 2 ở trên (Malwarebytes).
- [ ] PR dependabot #70 (website): kiểm và gộp nếu CI xanh. (#70 đã ĐÓNG, thay bằng #79)

## 3. File liên quan / đang khóa

| Đường dẫn | Lý do | Còn khóa? |
| --- | --- | --- |
| (không) | | Không |

## 4. Kết quả xử lý (mỗi phiên ghi một dòng)

- 24/11/2026 (SES-20261124-2010-27e7): P5 xong (#84), thử thật Windows ĐẠT và đã khôi phục máy chủ dự án về 1202. #70 đã đóng, thay bằng dependabot #79 (CI xanh).
