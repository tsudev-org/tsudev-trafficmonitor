// Kiểm tra bản build: mọi trang có title đúng khuôn, og:site_name = tsudev, canonical, description 120-160 ký tự,
// không có <script> nội tuyến hay style nội tuyến (CSP), liên kết ra ngoài mở tab mới có rel đủ.
import { readdirSync, readFileSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const dist = join(dirname(fileURLToPath(import.meta.url)), '../dist');
const loi = [];
function duyet(thuMuc) {
  for (const e of readdirSync(thuMuc, { withFileTypes: true })) {
    const p = join(thuMuc, e.name);
    if (e.isDirectory()) duyet(p);
    else if (e.isFile() && e.name.endsWith('.html')) kiemTra(p, readFileSync(p, 'utf8'));
  }
}
function kiemTra(tep, html) {
  const ten = tep.slice(dist.length);
  const title = html.match(/<title>([^<]*)<\/title>/)?.[1] ?? '';
  if (!/(^| - )TrafficMonitor-VN \| tsudev$/.test(title)) loi.push(`${ten}: title sai khuôn: ${title}`);
  if (!html.includes('<meta property="og:site_name" content="tsudev">')) loi.push(`${ten}: thiếu og:site_name tsudev`);
  if (!/<link rel="canonical" href="https:\/\/trafficmonitor\.tsudev\.com\//.test(html)) loi.push(`${ten}: thiếu canonical`);
  const mota = html.match(/<meta name="description" content="([^"]*)"/)?.[1] ?? '';
  const dai = [...mota.replace(/&[a-z#0-9]+;/g, 'x')].length;
  if (dai < 120 || dai > 160) loi.push(`${ten}: description ${dai} ký tự (cần 120-160)`);
  if (/<script(?![^>]*type="application\/ld\+json")[^>]*>/.test(html)) loi.push(`${ten}: có <script>`);
  if (/<style[\s>]/.test(html) || / style="/.test(html)) loi.push(`${ten}: có CSS nội tuyến (CSP chặn)`);
  for (const a of html.matchAll(/<a [^>]*target="_blank"[^>]*>/g)) {
    if (!a[0].includes('rel="noopener noreferrer"')) loi.push(`${ten}: thiếu rel noopener noreferrer: ${a[0]}`);
  }
}
duyet(dist);
for (const l of loi) console.error('[check-dist] LỖI', l);
console.log(`[check-dist] ${loi.length} lỗi`);
process.exit(loi.length ? 1 : 0);
