// Sinh khối "Cài bằng lệnh" của trang /tai-ve/ LÚC BUILD từ catalog chính thức của các kho gói (phiếu 20261114-02).
//
//   node scripts/cai-bang-lenh.mjs          # tải catalog, ghi src/data/cai-bang-lenh.json
//
// Vì sao sinh lúc build: trang là tệp tĩnh, không gọi mạng lúc chạy; tên gói không chép tay - kho đổi tên gói thì lần
// build sau trang tự đúng. Tải hoặc kiểm catalog lỗi ⇒ build THẤT BẠI có thông báo, KHÔNG lặng lẽ ra khối rỗng.
// Lọc theo `homepage` = SITE (cách ghép giống `scripts/pkg-catalog-sync.js` của repo tsudev). Gói chuyển tiếp
// (`replacedBy`) bị bỏ: trang chỉ giới thiệu tên gói hiện hành.
// Mã winget không có catalog (manifest nằm ở microsoft/winget-pkgs) nên đọc từ packaging/package.json của repo.
import { mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const WEB = join(dirname(fileURLToPath(import.meta.url)), '..');
const GOC = join(WEB, '..');
const OUT = join(WEB, 'src', 'data', 'cai-bang-lenh.json');
const SITE = 'https://tramon.tsudev.com';
// Tên miền cũ (301 sang SITE, ADR-0001 mục 3.6): catalog kho gói ghi homepage cũ cho tới khi kho cập nhật - vẫn nhận.
const SITE_CU = 'https://trafficmonitor.tsudev.com';
// Khớp FINGERPRINT trong tsudev/scripts/pkg-catalog-sync.js - không tin vân tay tải về.
const VAN_TAY_PKG = '388310F47FA8224D76C0A9C79745B6AF3EA33C74';
const NGUON = {
  pkg: 'https://pkg.tsudev.com/catalog.json',
  scoop: 'https://raw.githubusercontent.com/tsudev-org/scoop-bucket/main/catalog.json',
  brew: 'https://raw.githubusercontent.com/tsudev-org/homebrew-tap/main/catalog.json',
};

function loi(msg) {
  console.error(`[cai-bang-lenh] LỖI: ${msg}`);
  process.exit(1);
}

const chuan = (u) => String(u || '').replace(/\/+$/, '');

async function tai(kho, url) {
  let res;
  try {
    res = await fetch(url, { signal: AbortSignal.timeout(20000), headers: { 'User-Agent': 'tsudev-trafficmonitor-website' } });
  } catch (e) {
    loi(`không tải được catalog ${kho} (${url}): ${e.message}`);
  }
  if (!res.ok) loi(`catalog ${kho} trả HTTP ${res.status} (${url})`);
  const cat = await res.json().catch(() => loi(`catalog ${kho} không phải JSON (${url})`));
  if (cat.schema !== 1) loi(`catalog ${kho}: schema ${cat.schema}, script chỉ hiểu schema 1`);
  if (kho === 'pkg' && cat.key?.fingerprint !== VAN_TAY_PKG) loi(`vân tay khoá kho APT/RPM ${cat.key?.fingerprint} khác giá trị cứng`);
  if (!Array.isArray(cat.packages)) loi(`catalog ${kho}: thiếu mảng packages`);
  for (const p of cat.packages) {
    if (!/^[a-z][a-z0-9-]+$/.test(p.name || '')) loi(`catalog ${kho}: tên gói không hợp lệ "${p.name}"`);
  }
  return { cat, goi: cat.packages.filter((p) => [SITE, SITE_CU].includes(chuan(p.homepage)) && !p.replacedBy) };
}

const goiCauHinh = JSON.parse(readFileSync(join(GOC, 'packaging', 'package.json'), 'utf8'));
if (chuan(goiCauHinh.homepage) !== SITE) loi(`packaging/package.json homepage ${goiCauHinh.homepage} khác ${SITE}`);

const [pkg, scoop, brew] = await Promise.all([tai('pkg', NGUON.pkg), tai('scoop', NGUON.scoop), tai('brew', NGUON.brew)]);

const nhom = [];
const windows = [];
// Mã winget khai trước khi Microsoft duyệt: chỉ hiện lệnh khi manifest ĐÃ có trong microsoft/winget-pkgs (đo lúc build),
// không thì người dùng chép một lệnh báo "No package found".
let wingetCo = false;
// Kiểm mã winget TRƯỚC khi đưa vào URL gọi mạng: mỗi đoạn giữa dấu chấm khác rỗng (không `..`), chỉ chữ/số/gạch.
const WINGET_ID = /^[A-Za-z0-9-]+(\.[A-Za-z0-9-]+)+$/;
let duong = '';
if (goiCauHinh.winget) {
  if (typeof goiCauHinh.winget !== 'string' || !WINGET_ID.test(goiCauHinh.winget)) loi(`mã winget không hợp lệ: ${goiCauHinh.winget}`);
  duong = ['manifests', goiCauHinh.winget[0].toLowerCase(), ...goiCauHinh.winget.split('.')].map(encodeURIComponent).join('/');
  const r = await fetch(`https://api.github.com/repos/microsoft/winget-pkgs/contents/${duong}`, {
    signal: AbortSignal.timeout(20000),
    headers: { 'User-Agent': 'tsudev-trafficmonitor-website', ...(process.env.GITHUB_TOKEN && { authorization: `Bearer ${process.env.GITHUB_TOKEN}` }) },
  }).catch((e) => loi(`không hỏi được winget-pkgs: ${e.message}`));
  if (r.status === 200) wingetCo = true;
  else if (r.status === 404) console.log(`[cai-bang-lenh] winget ${goiCauHinh.winget} chưa có trên winget-pkgs - bỏ qua`);
  else loi(`winget-pkgs trả HTTP ${r.status}`);
}
if (wingetCo) {
  windows.push({
    kenh: 'winget',
    ghiChu: 'bộ cài, có sẵn trên Windows 10/11',
    lenh: [`winget install --id ${goiCauHinh.winget} -e`],
    nguon: `https://github.com/microsoft/winget-pkgs/tree/master/${duong}`,
  });
}
for (const p of scoop.goi) {
  windows.push({
    kenh: 'Scoop',
    ghiChu: 'không cần quyền quản trị, tự chọn bản x64/x86/ARM64',
    lenh: [...(scoop.cat.setup || []), p.install],
    lenhTao: p.commands || [],
    nguon: scoop.cat.bucket?.url || 'https://github.com/tsudev-org/scoop-bucket',
  });
}
if (windows.length) nhom.push({ heDieuHanh: 'Windows', kenh: windows });

const mac = brew.goi.map((p) => ({ kenh: 'Homebrew', ghiChu: 'macOS', lenh: [p.install], nguon: brew.cat.tap?.url }));
if (mac.length) nhom.push({ heDieuHanh: 'macOS', kenh: mac });

const linux = [];
for (const p of pkg.goi) {
  for (const k of ['apt', 'dnf']) {
    if (!p.install?.[k]) continue;
    linux.push({
      kenh: k === 'apt' ? 'APT (Debian, Ubuntu)' : 'DNF (Fedora, RHEL)',
      lenh: [...(pkg.cat.setup?.[k] || []), p.install[k]],
      nguon: 'https://pkg.tsudev.com',
    });
  }
}
if (linux.length) nhom.push({ heDieuHanh: 'Linux', kenh: linux });

if (!nhom.length) loi(`không kênh nào khớp homepage ${SITE} - kiểm packaging/package.json và catalog các kho`);

mkdirSync(dirname(OUT), { recursive: true });
writeFileSync(OUT, JSON.stringify({ nguon: NGUON, nhom }, null, 2) + '\n');
console.log(`[cai-bang-lenh] ${nhom.map((n) => `${n.heDieuHanh}: ${n.kenh.map((k) => k.kenh).join(', ')}`).join(' | ')}`);
