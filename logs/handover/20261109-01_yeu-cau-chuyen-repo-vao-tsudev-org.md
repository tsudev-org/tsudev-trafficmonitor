# PHIẾU YÊU CẦU - Chuyển repo `tsudev-trafficmonitor` sang Organization `tsudev-org`

- **Mã phiếu**: 20261109-01
- **Từ**: phiên tsudev-b1 (repo `tsudev`) - **Đến**: phiên đang làm ở repo này
- **Thời điểm**: 16:33 09/11/2026
- **Trạng thái**: HOÀN THÀNH (09/11/2026, phiên tsudev-trafficmonitor) - còn 1 việc chủ dự án tự làm, xem "Kết quả"
- **Uỷ quyền**: chủ dự án đồng ý chuyển repo này vào `tsudev-org` (chat 09/11/2026). Giao cho phiên của repo này vì
  đang có việc dở ở đây - phiên khác chuyển hộ có thể làm hỏng việc đang chạy.

## Vì sao làm

Org `tsudev-org` đã tạo 09/11. Năm repo đã chuyển cùng ngày và nghiệm thu: `tsudev`, `tsudev-cwico`,
`tsudev-swico`, `tsudev-contact`, `tsudev-standards`. Còn repo này và repo anh em (`tsudev-files`/`tsudev-trafficmonitor`).
Org đã cài sẵn GitHub App `render` và `gitguardian` (All repositories) - không phải cài lại.

## Các bước (làm khi KHÔNG có push/PR/release nào đang chạy)

1. Ghi khoá vào `logs/LOCKS.md`. Chụp hiện trạng TRƯỚC khi chuyển:
   - `gh api repos/tsudev-tsudev/tsudev-trafficmonitor` → `visibility`, `security_and_analysis`
   - `gh api repos/tsudev-tsudev/tsudev-trafficmonitor/branches/main/protection` (repo private gói Free sẽ 403 - bình thường)
   - tên secret: `gh api repos/tsudev-tsudev/tsudev-trafficmonitor/actions/secrets`; environment; số release
   - `gh api repos/tsudev-tsudev/tsudev-trafficmonitor/code-scanning/default-setup` (`configured` hay không)
   - mã HTTP của mọi URL mà bản app ĐÃ CÀI gọi tới (updater, link tải) - dùng `curl -sIL -o /dev/null -w '%{http_code}'`
2. Chuyển: `gh api -X POST repos/tsudev-tsudev/tsudev-trafficmonitor/transfer -f new_owner=tsudev-org`, rồi chờ
   `gh api repos/tsudev-org/tsudev-trafficmonitor` trả về (vài giây). Token `gh` sẵn có đủ quyền.
3. ⚠️ **Chuyển repo TẮT secret scanning + push protection** (đo được ở cả 4 repo public) **và CodeQL default setup**.
   Bật lại:
   ```bash
   gh api -X PATCH repos/tsudev-org/tsudev-trafficmonitor --input - <<'J'
   {"security_and_analysis":{"secret_scanning":{"status":"enabled"},"secret_scanning_push_protection":{"status":"enabled"}}}
   J
   gh api -X PATCH repos/tsudev-org/tsudev-trafficmonitor/code-scanning/default-setup -f state=configured -f query_suite=default  # chỉ khi trước đó configured; KHÔNG truyền languages
   ```
4. Đối chiếu với ảnh chụp bước 1: protection, secret, release, mọi URL cũ vẫn 200 (GitHub tự chuyển hướng).
5. `git remote set-url origin git@github.com:tsudev-org/tsudev-trafficmonitor.git` rồi `git fetch`.
6. Mở một PR thử: số check phải bằng trước khi chuyển (GitGuardian phải có). Chính sách Actions của Org có thể khác
   tài khoản cá nhân - workflow nào bị chặn thì báo chủ dự án, đừng nới quyền tự ý.

## ⚠️ Riêng repo này: updater kiểm tiền tố URL theo OWNER

`TrafficMonitor/ForkConfig.h` ghim `FORK_GITHUB_OWNER = tsudev-tsudev` và `FORK_RELEASE_DOWNLOAD_PREFIX =
https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/`; `IsTrustedDownloadUrl` (`UpdateSecurity.h`)
CHỈ tin URL bắt đầu bằng tiền tố đó. Sau chuyển:

- URL do API GitHub trả (`browser_download_url`) sẽ mang owner `tsudev-org` ⇒ bản ĐÃ CÀI có thể từ chối cập nhật nếu
  URL tải lấy từ API. Kiểm xem URL tải lấy từ đâu (version_signed.xml hay API) và thử một vòng cập nhật thật trước khi
  phát hành bản kế.
- Bản mới nên CHẤP NHẬN CẢ HAI tiền tố (`tsudev-tsudev` và `tsudev-org`) - không chỉ đổi sang owner mới - và nội dung
  `version_signed.xml` của các bản phát hành sau phải dùng tiền tố mà bản cũ còn tin được. Có test ở
  `tests/UpdateSecurityTests.cpp`.

## ⛔ Quy tắc vĩnh viễn

**KHÔNG BAO GIỜ tạo repo, fork hay đổi tên repo nào thành `tsudev-trafficmonitor` (hay bất kỳ tên repo cũ nào) dưới tài khoản
`tsudev-tsudev`.** GitHub bỏ chuyển hướng ngay khi tên cũ bị chiếm lại ⇒ mọi URL cũ (updater trong bản đã cài,
winget, link tài liệu, `git remote`) gãy IM LẶNG hoặc trỏ sang repo sai. Repo mới cho hệ tsudev tạo trong `tsudev-org`.

## Báo lại

Xong thì đóng phiếu này (HOÀN THÀNH + kết quả đo), ghi vào `logs/STATE.md` của repo này. Phiếu gốc phía tsudev:
`tsudev/logs/handover/20261109-03_phien-44-chuyen-repo-vao-tsudev-org.md`.

## Kết quả (09/11/2026)

Chủ dự án xác nhận trực tiếp trong phiên này trước khi chuyển.

- Trước khi chuyển: PR #28 - `release.yml` ghi liên kết tải trong `version_signed.xml` theo owner cũ `tsudev-tsudev`
  (trước đây lấy `$GITHUB_REPOSITORY` => sau chuyển, mọi bản đã cài <= 26.11.0902 sẽ từ chối cập nhật);
  app mới chấp nhận cả hai tiền tố (`FORK_RELEASE_DOWNLOAD_PREFIX_ORG`, test 80/80). gitleaks đã dùng binary ghim sha256 từ PR #24.
- Chuyển: `tsudev-org/tsudev-trafficmonitor` (repo id 1450381650 giữ nguyên).
- Đối chiếu sau chuyển:

| Mục | Trước | Sau |
| --- | --- | --- |
| visibility | public | public |
| secret scanning / push protection | enabled / enabled | bị tắt -> đã bật lại: enabled / enabled |
| CodeQL default setup | not-configured (có codeql.yml riêng) | not-configured (không bật, theo bổ sung của tsudev-b1) |
| Workflow | 7 active | 7 active |
| Chính sách Actions | selected: GitHub + microsoft/setup-msbuild | giống |
| Environment `release` | reviewer tsudev-tsudev, secret UPDATE_SIGNING_KEY | giống |
| Ruleset "Bảo vệ main", "Tag phát hành bất biến" | active | giống (main thêm trường mặc định dismissal_restriction=disabled) |
| Ruleset "Tag phát hành: chỉ quản trị tạo" | bypass: RepositoryRole admin (5), always | **bypass bị xóa** - khôi phục bằng API bị bộ phân loại của Claude Code chặn => chủ dự án tự thêm lại (Settings -> Rules -> Rulesets) |
| Release / tag | 7 / 7 | 7 / 7 |
| URL bản đã cài gọi (manifest, chữ ký, 3 zip, tài liệu) | 200 | 200 (chuyển hướng; host cuối release-assets.githubusercontent.com nằm trong FORK_TRUSTED_HOSTS) |

- `git remote` -> `git@github.com:tsudev-org/tsudev-trafficmonitor.git`. PR thử: PR đóng phiếu này.
