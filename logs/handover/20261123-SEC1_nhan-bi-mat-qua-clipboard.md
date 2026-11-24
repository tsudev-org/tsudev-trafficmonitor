# PHIẾU YÊU CẦU SEC1 - Nhận token/secret qua clipboard (tsudev-trafficmonitor)

- **Mã phiếu**: 20261123-SEC1
- **Thời điểm**: 23/11/2026
- **Người gửi**: agent-tsuswico (phiên tsuswico-6e) theo yêu cầu của chủ dự án
- **Phiếu điều phối (ĐỌC TRƯỚC)**: `~/projects/logs/handover/20261123-SEC1_nhan-bi-mat-qua-clipboard-toan-he.md`
- **Trạng thái**: ĐÓNG - PR chore/standards-v5.1.0-phieu-ton (24/11/2026); còn 👤 thay khoá VirusTotal (mục 4)

## 1. Quy tắc áp dụng ngay cho agent của repo này

- KHÔNG đề nghị chủ dự án dán token/secret vào chat, tệp hay lệnh `!`. Lỡ dán vào chat ⇒ coi là đã lộ, đề nghị thay.
- Nhận bí mật: chạy `tsu-nhan-bi-mat ...` bằng `run_in_background`, nhắn "Bấm Copy trên trang <dịch vụ> - không dán vào chat".
  Ưu tiên `--stdout | <lệnh đích>`; cần giữ ⇒ `--luu ~/.config/tsudev/<ten>.env` (chmod 600, ngoài repo).
- KHÔNG in giá trị bí mật (`cat .env`, `sed`…); chỉ in tên biến/độ dài/kết quả kiểm tra.
- Cần người dán vào dashboard ⇒ `tsu-nhan-bi-mat --dua-vao-clipboard <tệp>` rồi `--xoa-lich-su <tệp>`.
- Lịch sử Win+V trên máy đang BẬT ⇒ luôn để công cụ xoá mục khỏi lịch sử.

## 2. Việc cần làm

1. Rà tài liệu/hướng dẫn của repo (README thiết lập, `docs/`, `logs/STATE.md`, phiếu cũ còn mở) - chỗ nào bảo dán token
   vào chat/.env bằng tay ⇒ sửa theo phiếu điều phối mục 2-3.
2. Kiểm `.gitignore` có `.env*` (trừ `.env.example`); gitleaks/push protection theo SECURITY_BASELINE.
3. Liệt kê TÊN các bí mật repo dùng + nơi lưu (GitHub Actions secret / Cloudflare / Vercel / Render / `~/.config/tsudev/`)
   + phạm vi quyền vào mục 3 dưới (không ghi giá trị). Cái nào từng lộ trong chat/log ⇒ báo chủ dự án thay.
4. Ghi AKB của repo "áp dụng SEC1", đổi Trạng thái → ĐÓNG.

## 3. Bí mật của repo (agent điền - CHỈ TÊN)

| Tên | Nơi lưu | Phạm vi | Ghi chú |
| --- | --- | --- | --- |
| `UPDATE_SIGNING_KEY` | GitHub Actions secret của environment `release` (chỉ nhận tag `V*`) | Khoá bí mật ECDSA P-256 ký `version_signed.xml` | Khoá công khai ở `tools/keys/update_signing_pub.pem`; chưa từng lộ |
| `VT_API_KEY` | GitHub Actions secret cấp Organization `tsudev-org` (mọi repo) | Khoá VirusTotal cho job quét tệp phát hành (#76) | Bản sao cục bộ `~/.config/tsudev/virustotal.env` (dùng chung) - xem mục 4 |
| `GITHUB_TOKEN` | Tự cấp mỗi lần chạy workflow | Theo khối `permissions:` từng workflow (mặc định `contents: read`) | Không lưu |
| Đăng nhập wrangler (OAuth) | `~/.config/wrangler-tsudev/` (ngoài repo, chủ máy đăng nhập) | Deploy Cloudflare Pages `tsudev-trafficmonitor` | Không phải tệp trong repo |

## 4. Kết quả xử lý

- 24/11/2026 - phiên tsudev-trafficmonitor (SES-20261124-1215-6a21): rà README*, `docs/`, `website/README.md`,
  `logs/STATE.md`, phiếu còn mở - không chỗ nào bảo dán token vào chat/.env bằng tay. `.gitignore` có `.env`, `.env.*`,
  `.envrc`; gitleaks chạy trong CI (toàn bộ lịch sử) + hook `.githooks`. Repo không có secret cấp repo.
- ⚠️ **Lộ khoá**: agent đọc `~/.config/tsudev/virustotal.env` để đo lại VirusTotal (phiếu AV1); tệp chứa khoá trần nên
  lệnh che bằng `sed 's/=.*/.../'` không che được và **giá trị khoá đã hiện trong nhật ký phiên**. Theo mục 1 ⇒ coi là
  đã lộ. 👤 Chủ dự án: tạo khoá VirusTotal mới, cập nhật secret org `VT_API_KEY` và tệp cục bộ bằng
  `tsu-nhan-bi-mat --luu ~/.config/tsudev/virustotal.env`, rồi thu hồi khoá cũ (khoá dùng chung - báo điều phối).
  Bài học: tệp bí mật một dòng không có `TEN=` ⇒ chỉ in `wc -c`, không dùng `sed` để che.