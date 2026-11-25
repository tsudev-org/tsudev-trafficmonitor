# PHIẾU BÀN GIAO - Sau UI2 và P9 phần 1-2 (nền đăng nhập)

- **Mã phiếu**: 20261125-03
- **Từ**: phiên tsudev-trafficmonitor-61 (SES-20261125-0927-4f83) - **Đến**: phiên kế tiếp của repo này
- **Thời điểm**: 25/11/2026 - **Trạng thái**: MỞ
- **Nhánh git**: không có nhánh dang dở sau PR bàn giao này. Không file nào khoá; không trình duyệt/monitor nào đang giữ.

## 1. Trạng thái hiện tại

- Bản Latest vẫn **Tsu Tramon `v26.11.2501`** (không phát hành trong phiên này).
- Website (deploy 25/11 sau #96): quy ước **v6.0.0** (nền Sáng giấy `#EEF1F5`), preload font màn đầu + font dự phòng có
  số đo (`website/src/styles/fonts-fallback.css`), hoạ tiết hình học tĩnh ở thẻ + chân trang. Phiếu UI2 ĐÓNG.
- **P9 mở khoá**: client OIDC `tsutramon` có trên tsudev.com (phiên tsudev-c4 báo, tự kiểm `/oidc/auth` trả 303 với
  `redirect_uri=http://127.0.0.1:<cổng>/callback`, PKCE S256). Discovery: `https://tsudev.com/oidc/.well-known/openid-configuration`
  (issuer `https://tsudev.com/oidc`, chỉ ES256, scope `tsudev.entitlements` có, `authorization_response_iss_parameter_supported`).
- **P9 phần 1-2 đã gộp (#97)** - CHƯA nối vào app (không có trong `.vcxproj`, không giao diện):
  - `tsudev-trafficmonitor/ProAuth.h` (thuần, test cả g++ lẫn MSVC): PKCE, state/nonce, URL đăng nhập (`prompt=consent`
    để giữ `offline_access`), đọc callback loopback (iss bắt buộc - RFC 9207), JWKS P-256, JWS chỉ ES256 (từ chối
    crit/jku/jwk/x5u/x5c), quyền lợi PRODUCT_POLICY 7.4, id_token OIDC Core 3.1.3.7, JSON chặt có giới hạn, mã cài đặt.
  - `tsudev-trafficmonitor/ProNet.h` (Windows): WinHTTP chỉ `https://tsudev.com/...` (không theo chuyển hướng, TLS ≥ 1.2,
    ≤ 64 KB), `LoopbackReceiver` 127.0.0.1 cổng ngẫu nhiên, refresh token trong Credential Manager
    (`tsudev/tsutramon/refresh_token`).
  - URL ở `ForkConfig.h` (`FORK_TSUDEV_*`). Test: `tests/ProAuthTests.cpp` (142 + PKCE RFC 7636 + ES256 thật bằng BCrypt
    trên Windows), `tests/ProNetTests.cpp`, fuzz `tests/fuzz/ProAuthFuzz.cpp` (120 giây trong CI).

## 2. Việc còn lại (từ trên xuống)

1. **Theo dõi (đầu phiên, ~2 phút)**: VT arm64ec 2401 + 2501 từ **27/11** (`docs/store/antivirus.md` mục 3; khoá
   `~/.config/tsudev/virustotal.env` là KHOÁ TRẦN - đọc `head -c 200 … | tr -d '[:space:]'`, KHÔNG `source`); winget #477193;
   phiếu TM1 scoop-bucket (bucket vẫn chỉ có `tsudev-trafficmonitor.json` ngày 25/11).
2. **P9 phần 3 - nối vào app** (một PR, build CI + thử thật Windows trên máy chủ dự án, memory `thu-giao-dien-tren-windows`):
   - Thêm `ProAuth.h`/`ProNet.h` vào `.vcxproj` (+ `_Lite`), lớp `CProAccount` chạy luồng nền: `NewAuthorizationRequest` →
     `ShellExecute` URL → `LoopbackReceiver::Wait(180 s, &huỷ)` → `StateMatches` + `IssuerOk` → POST token
     (`TokenRequestBody`) → JWKS → `ValidateIdToken` → lưu refresh token → GET quyền lợi với `Authorization: Bearer` +
     `X-Tsu-Installation` → `VerifyEntitlement` → đệm JWS (≤ 7 ngày, tệp trong `%AppData%\tsutramon`) → `ProRenewal::Entitlement`.
   - Mã cài đặt: `NewInstallationId()` lần chạy đầu, lưu cùng cấu hình; chỉ gửi khi đã đăng nhập (AUTH 14.3).
   - BẪY include: `ProNet.h` cần `winsock2.h` TRƯỚC `windows.h`; trong MFC (`stdafx/pch`) `afxwin.h` đã kéo `windows.h`
     ⇒ thêm `#include <winsock2.h>` vào pch trước afx hoặc tách `ProNet.cpp` không dùng pch.
   - Giao diện: mục "Tài khoản tsudev" trong Cài đặt (nút ghi rõ "Đăng nhập tài khoản tsudev để ..." - AUTH 14.1), trạng
     thái, **Đăng xuất và xóa dữ liệu cục bộ** (xoá Credential Manager + JWS đệm + mã cài đặt), công tắc **Chế độ ngoại tuyến
     hoàn toàn** (AUTH 14.1, chặn cả kiểm tra cập nhật). Chuỗi vi/en trong `.rc` (UTF-16 LE BOM) + `language/*.ini`.
   - Hộp thoại 7.6 (nội dung nguyên văn PRODUCT_POLICY 7.6) nối `ProRenewal::Tracker`; nút Gia hạn mở `ProAuth::ProPageUrl()`.
   - Đăng nhập QR (QR1, `CROSS_DEVICE_LOGIN.md` v1.1.0) - có thể tách PR sau.
   - ADR-0002 mục 2.4: chỉ nối hộp thoại 7.6 khi thể hiện được (đã có nguồn JWS sau phần 3).
3. **P10** (đồng bộ TsuDEV Sync, xuất CSV miễn phí, thông báo nhiều thiết bị 7.3 - 5 test) - cần hợp đồng tsusync cho C++.
4. Phát hành kế tiếp khi có P9 phần 3: bỏ `*_Lite.zip` CHỈ khi scoop-bucket chuyển gói; thử thật Windows 2501 → bản mới.

## 3. Lưu ý / bẫy

- **MSVC dịch `\uXXXX` cả trong raw string `R"(...)"`** (g++ không) ⇒ lỗi C3850; test JSON có thoát `\u` dùng chuỗi thường.
- Chuỗi cổng `cmd | tail -1 && git commit` che mã thoát ⇒ commit lọt cổng; dùng `set -o pipefail` (AKB lesson mới).
- Gitleaks chặn hạt giống fuzz là JWT giả ⇒ sinh JWS lúc chạy, KHÔNG thêm allowlist.
- `npm run check:theme` cục bộ cần `npx playwright install chromium` đúng phiên bản của `website/` (CI tự cài).
- Ghi đè token bản sắc phải dùng bộ chọn `:root[data-product="trafficmonitor"]` (products.css thắng `:root`).

## 4. File liên quan / đang khóa

| Đường dẫn | Lý do | Còn khóa? |
| --- | --- | --- |
| (không) | | Không |

## 5. Kết quả xử lý (mỗi phiên ghi một dòng)

-
