# ADR-0001: Đổi tên sản phẩm thành Tsu Tramon, giữ đường cập nhật của mọi bản đã cài

- **Trạng thái**: ĐÃ CHẤP NHẬN (tên do chủ dự án chốt 19/11/2026; phần kỹ thuật do agent chốt theo uỷ quyền phiếu 20261114-01 mục 1)
- **Ngày**: 24/11/2026
- **Người quyết định**: chủ dự án (tên, repo, tên miền); phiên tsudev-trafficmonitor (cách chuyển đổi)
- **Liên quan**: phiếu `logs/handover/20261119-TC1_yeu-cau-ten-san-pham-rieng.md` (mục 3), `20261117-V1_quy-uoc-phien-ban-thong-nhat.md`,
  `.standards/docs/VERSIONING.md` mục 2.2-2.4, `.standards/docs/PACKAGING.md` mục 2.2, khuôn tsufiles ADR-0003 mục 5-8

## 1. Bối cảnh

Chủ dự án chốt 19/11/2026: tên hiển thị **Tsu Tramon**, repo `tsudev-org/tsudev-trafficmonitor` → **`tsudev-org/tsutramon`**,
tên miền **`tramon.tsudev.com`** (tên miền cũ chuyển hướng 301 vĩnh viễn), đổi hết trong app có di trú. Phiếu V1 đổi thẻ
phát hành từ `V26.11.DDNN-vn` sang `v26.11.DDNN` và tên tệp sang `{gói}_{phiên bản}_windows-{arch}.zip`.

Ràng buộc: các bản đã cài ngoài máy người dùng (26.11.0901 → 26.11.1202) chỉ cập nhật được qua trình cập nhật trong app.
Khảo sát mã của chính các bản đó (24/11/2026, `UpdateSecurity.h`, `ForkConfig.h`, `version_utf8.info`):

| Điều bản đã cài đòi | Chi tiết | Hệ quả |
| --- | --- | --- |
| Manifest ký | Tải `https://github.com/tsudev-org/tsudev-trafficmonitor/releases/latest/download/version_signed.xml` (+ `.sig`, ECDSA P-256, khoá `FORK_UPDATE_PUBLIC_KEY_HEX`) | Sau đổi tên repo phải còn tới được qua chuyển hướng của GitHub; khoá ký giữ nguyên |
| So phiên bản | `CompareVersion` so **chuỗi `<version>`** từng nhóm số; `IsValidVersionString` chỉ nhận chữ số và dấu chấm | Thẻ git KHÔNG được đọc ⇒ đổi khuôn thẻ an toàn; `<version>` phải giữ `YY.M.DDNN`, không có `v` |
| Link tải | `IsTrustedDownloadUrl`: tiền tố `https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/` hoặc `https://github.com/tsudev-org/tsudev-trafficmonitor/releases/download/`, đuôi `.zip`/`.exe`, cấm `..`, `@?#\%` | Link trong manifest phải GIỮ tiền tố tên cũ mãi mãi (GitHub chuyển hướng); tên thẻ và tên tệp sau tiền tố tuỳ ý |
| Sau khi tải | Chỉ lưu zip vào Downloads, kiểm SHA-256, gắn Mark of the Web; người dùng tự giải nén và chạy | Không có bộ cài tự thay exe ⇒ đổi tên exe không phá đường cập nhật; bản mới tự di trú dữ liệu khi chạy lần đầu |

Chứng minh bằng test: `tests/UpdateSecurityTests.cpp` `TestKhuonV1` (tiền tố của bản đã cài ghi cứng): phiên bản kế tiếp
được coi là MỚI HƠN, link `v26.11.DDNN/tsutramon_..._windows-{x64,x86,arm64ec}.zip` và tên tệp cũ đều được nhận dưới tiền
tố cũ; link theo tên repo mới bị bản đã cài TỪ CHỐI.

Dữ liệu: D1 (cấu hình + lịch sử lưu lượng cục bộ). Chi phí: 0 đồng (GitHub, Cloudflare gói miễn phí).

## 2. Các phương án đã cân nhắc

| Phương án | Ưu | Nhược | Chi phí |
| --- | --- | --- | --- |
| A. Đổi tên toàn bộ, manifest giữ link theo tên cũ, bản mới tự di trú | Một bước, người dùng cũ tự lên bản mới | Phụ thuộc chuyển hướng của GitHub (tên cũ phải bất khả xâm phạm) | 0 |
| B. Bản cầu nối: phát hành một bản tên cũ chỉ để thêm tiền tố mới, rồi mới đổi tên | Không phụ thuộc chuyển hướng lâu dài | Thêm một vòng phát hành; bản cũ không tự cập nhật vẫn kẹt | 0 |
| C. Không đổi tên | Không rủi ro | Trái quyết định chủ dự án | 0 |

## 3. Quyết định

Chọn **A**, theo đúng thứ tự (mỗi bước một PR, phase P5-P7 trong `logs/STATE.md`):

1. **Manifest** (`version_utf8.info`, `version_signed.xml`): `<version>` khuôn `YY.M.DDNN`; mọi `link*` ghi theo
   `https://github.com/tsudev-tsudev/tsudev-trafficmonitor/releases/download/<thẻ>/<tệp>` - KHÔNG BAO GIỜ đổi sang tên repo
   mới (bản ≤ 26.11.1202 sẽ từ chối). Bản mới thêm tiền tố `tsudev-org/tsutramon` vào danh sách tin cậy của chính nó.
2. **Tệp phát hành** bản đầu tiên tên mới: `tsutramon_<v>_windows-{x64,x86,arm64ec}.zip` (khuôn V1) **và song song** tên
   cũ `tsudev-trafficmonitor_<v>_{x64,x86,arm64ec}_Lite.zip` (cùng nội dung, có trong `SHA256SUMS`) cho Scoop/winget/
   liên kết cũ. Bỏ tên cũ từ bản kế tiếp sau khi kênh gói đã chuyển. Manifest trỏ tệp tên mới.
3. **Thẻ**: `v<YY.M.DDNN>` (thẻ cũ `V*-vn` giữ nguyên, ruleset bất biến). `release.yml` và environment `release` phải nhận
   cả `v*` trước khi đẩy thẻ đầu tiên; `website` đọc cả `V`/`v` (đã làm ở #80).
4. **App**: tên hiển thị "Tsu Tramon"; exe `TsuTramon.exe`; `%AppData%\tsutramon`; giá trị Run `TsuTramon`. Lần chạy đầu
   chép `%AppData%\tsudev-trafficmonitor` (hoặc `TrafficMonitor`) nếu thư mục mới chưa có, chuyển Run key/tác vụ tự khởi
   động trỏ exe mới (mở rộng khuôn `LEGACY_*` đã chạy thật ở 26.11.1201); chặn chạy song song với mọi tên cũ. Test C++
   cho logic chọn nguồn di trú (logic thuần, header riêng).
5. **Plugin API**: giữ ABI v8 và tên `ITsudevTrafficMonitor` + alias `ITrafficMonitor` (đổi tên lớp phá biên dịch plugin
   bên thứ ba; tên lớp không hiển thị cho người dùng). Chỉ đổi chú thích/tài liệu.
6. **Website**: `tramon.tsudev.com` (custom domain Cloudflare Pages); `trafficmonitor.tsudev.com` chuyển hướng 301 giữ
   đường dẫn + truy vấn; bản mới thêm `tramon.tsudev.com` vào `FORK_TRUSTED_HOSTS`, giữ tên miền cũ.
7. **winget**: giữ mã `tsudev.TrafficMonitor` (PACKAGING 2.2: mã đã nộp không đổi), đổi `PackageName` thành "Tsu Tramon".
8. **Đổi tên repo** chỉ SAU khi bản đầu tiên tên mới đã phát hành, rồi kiểm bằng `curl -sIL` cả 4 đường: manifest
   `releases/latest/download/version_signed.xml` theo tên cũ, link tải theo `tsudev-tsudev/tsudev-trafficmonitor`, raw
   `version_utf8.info`, API releases. Đường nào gãy ⇒ đổi tên repo ngược lại ngay (tên chưa bị chiếm) và chuyển sang B.

## 4. Hệ quả

**Tích cực:** người dùng mọi bản 26.11.09xx-12xx tự lên Tsu Tramon; tên sản phẩm riêng theo khuôn hệ (TsuFiles, Tsu Swico...).

**Tiêu cực và chi phí phải trả:** đường cập nhật của bản cũ phụ thuộc vĩnh viễn vào chuyển hướng của GitHub ⇒ tên
`tsudev-trafficmonitor` (trong `tsudev-org` lẫn `tsudev-tsudev`) trở thành BẤT KHẢ XÂM PHẠM; bản phát hành đầu tiên gánh
gấp đôi tệp.

**Việc phát sinh cần làm:**
- [x] P5 (#84): đổi tên trong app + di trú + test (`LegacyNames.h`); thử thật Windows bằng build CI: chặn song song, chuyển dữ liệu, Run key ĐẠT (tự tải + SHA thử ở P6 khi có bản phát hành)
- [x] P6 (#85): website tên mới + tên miền + 301; `release.yml` nhận `v*`, tên tệp mới + song song tên cũ; phát hành `v26.11.2401` 24/11/2026; SHA256SUMS/attestation 6 gói/chữ ký ĐẠT; VirusTotal: arm64ec 1/73 Malwarebytes (chủ dự án chấp nhận, ticket báo nhầm 8706403)
- [x] P7 (24/11/2026): đổi tên repo → `tsudev-org/tsutramon`, 4 đường chuyển hướng ĐẠT; phiếu TM1 cho điều phối (`~/.claude/CLAUDE.md`, tsudev.com, tsudev-standards) và scoop-bucket; linux-packages không đóng gói sản phẩm này. Chứng thực build ghi tên repo lúc build ⇒ bản ≤ 26.11.2401 kiểm bằng `-R tsudev-org/tsudev-trafficmonitor`
- [ ] winget: đổi `PackageName` "Tsu Tramon" sau khi #477193 gộp

## 5. Đường lùi

Trước bước 8, mọi thay đổi đảo ngược được bằng bản phát hành kế tiếp. Sau bước 8: đổi tên repo ngược lại (GitHub cho phép
khi tên chưa bị chiếm) khôi phục mọi URL; dữ liệu cũ ở `%AppData%\tsudev-trafficmonitor` không bị xoá khi di trú nên bản cũ
vẫn chạy được nếu người dùng quay lại.
