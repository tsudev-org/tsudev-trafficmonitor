// Nội dung trang lấy trực tiếp từ các tệp .md của repo (một nguồn duy nhất cho tài liệu).
import { readFileSync } from 'node:fs';
import { dirname, join, normalize } from 'node:path';
import { Marked } from 'marked';

export const REPO = 'tsudev-org/tsudev-trafficmonitor';
export const REPO_URL = `https://github.com/${REPO}`;
export const SITE = 'https://tramon.tsudev.com';

// Gốc repo: build chạy trong thư mục website/ (npm run build), nên gốc là thư mục cha.
const GOC = normalize(join(process.cwd(), '..'));

/** Tệp .md của repo -> đường dẫn trên website. Tệp khác của repo trỏ về GitHub. */
const TRANG_MD: Record<string, string> = {
  'Help.md': '/huong-dan/hoi-dap/',
  'KhoGiaoDien.md': '/huong-dan/kho-giao-dien/',
  'HuongDanTaoSkin.md': '/huong-dan/tu-lam-giao-dien/',
  'Plugin.md': '/huong-dan/plugin/',
  'PhatTrienPlugin.md': '/huong-dan/phat-trien-plugin/',
  'UpdateLog/update_log_vi.md': '/nhat-ky/',
  'SECURITY.md': '/bao-mat/',
  'NOTICE.md': '/ban-quyen/',
  'LICENSE_VI.md': '/ban-quyen/#ban-dich-giay-phep',
  'LICENSE': '/ban-quyen/#giay-phep-nguyen-van',
  'README.md': '/',
};

export function docTep(duongDan: string): string {
  return readFileSync(join(GOC, duongDan), 'utf8').replace(/^﻿/, '');
}

/** Slug kiểu GitHub: chữ thường, bỏ dấu câu, khoảng trắng -> "-" (giữ chữ có dấu tiếng Việt). */
function slug(text: string): string {
  // Bỏ thẻ HTML lặp tới khi hết (một lượt có thể để lại thẻ ghép lồng như "<scr<b>ipt>")
  let s = text.toLowerCase().trim();
  for (let truoc = ''; truoc !== s; ) {
    truoc = s;
    s = s.replace(/<[^<>]*>/g, '');
  }
  return s.replace(/[^\p{L}\p{N}\s_-]/gu, '').replace(/\s/g, '-');
}

function viTriLienKet(href: string, tepMd: string): string {
  if (/^(https?:|mailto:|#)/.test(href)) return href;
  const [duong, neo] = href.split('#', 2);
  const tuyetDoi = normalize(join(dirname(tepMd), decodeURI(duong))).replace(/\\/g, '/').replace(/\/$/, '');
  if (tuyetDoi.startsWith('Screenshots/')) return '/anh/' + tuyetDoi.slice('Screenshots/'.length);
  const trang = TRANG_MD[tuyetDoi];
  if (trang) return neo ? `${trang.split('#')[0]}#${neo}` : trang;
  const loai = /\.[A-Za-z0-9_-]+$/.test(tuyetDoi) ? 'blob' : 'tree';
  return `${REPO_URL}/${loai}/main/${encodeURI(tuyetDoi)}${neo ? '#' + neo : ''}`;
}

/** Liên kết ngoài = http(s) tới host khác trang này (so hostname, không so chuỗi đầu: "tramon.tsudev.com.evil.example" là ngoài). */
function laNgoai(href: string): boolean {
  if (!/^https?:/i.test(href)) return false;
  try {
    return new URL(href).hostname.toLowerCase() !== new URL(SITE).hostname;
  } catch {
    return true;   // URL hỏng: coi là ngoài (thêm rel="noopener noreferrer")
  }
}

/** Chuyển Markdown của repo sang HTML: sửa liên kết tương đối, thêm id cho tiêu đề, rel cho liên kết ngoài. */
export function renderMd(tepMd: string, opts: { boTieuDeDau?: boolean; boDongNgonNgu?: boolean } = {}): { html: string; tieuDe: string } {
  let md = docTep(tepMd);
  if (opts.boDongNgonNgu) md = md.replace(/^\*\*[^\n]*\|[^\n]*\*\*\s*\n/, '');
  const tieuDe = md.match(/^#\s+(.+)$/m)?.[1]?.trim() ?? '';
  if (opts.boTieuDeDau) md = md.replace(/^#\s+.+\n/m, '');
  const marked = new Marked({ gfm: true });
  const daDung = new Map<string, number>();   // id tiêu đề phải duy nhất trong trang (nhật ký lặp "Sửa lỗi", "Bảo mật"... mỗi bản)
  const idDuyNhat = (goc: string): string => {
    const n = (daDung.get(goc) ?? 0) + 1;
    daDung.set(goc, n);
    return n === 1 ? goc : `${goc}-${n}`;
  };
  marked.use({
    renderer: {
      heading({ tokens, depth }) {
        const text = this.parser.parseInline(tokens);
        return `<h${depth} id="${idDuyNhat(slug(text))}">${text}</h${depth}>\n`;
      },
      link({ href, title, tokens }) {
        const text = this.parser.parseInline(tokens);
        const dich = viTriLienKet(href, tepMd);
        const ngoai = laNgoai(dich) ? ' target="_blank" rel="noopener noreferrer"' : '';
        return `<a href="${dich}"${title ? ` title="${title}"` : ''}${ngoai}>${text}</a>`;
      },
      image({ href, title, text }) {
        return `<img src="${viTriLienKet(href, tepMd)}" alt="${text}"${title ? ` title="${title}"` : ''} loading="lazy">`;
      },
    },
  });
  const html = (marked.parse(md, { async: false }) as string)
    .replace(/<table>/g, '<div class="bang"><table>')
    .replace(/<\/table>/g, '</table></div>')
    // Khối mã có thể cuộn ngang: cho nhận focus để cuộn bằng bàn phím (WCAG 2.1.1, axe scrollable-region-focusable)
    .replace(/<pre>/g, '<pre tabindex="0">');
  return { html, tieuDe };
}

interface GoiTai { kienTruc: string; ten: string; url: string; sha256: string; kichThuoc: number }
export interface BanPhatHanh { tag: string; phienBan: string; ngay: string; url: string; goi: GoiTai[] }

let boNho: Promise<BanPhatHanh> | undefined;

/** Bản phát hành mới nhất, lấy lúc build từ GitHub (SHA-256 từ SHA256SUMS.txt). */
export function banMoiNhat(): Promise<BanPhatHanh> {
  boNho ??= (async () => {
    const res = await fetch(`https://api.github.com/repos/${REPO}/releases/latest`, {
      headers: { Accept: 'application/vnd.github+json', 'User-Agent': 'tsudev-trafficmonitor-website' },
    });
    if (!res.ok) throw new Error(`GitHub API ${res.status}`);
    const r = await res.json() as { tag_name: string; published_at: string; html_url: string; assets: { name: string; browser_download_url: string; size: number }[] };
    const sums = r.assets.find((a) => a.name === 'SHA256SUMS.txt');
    const bang = new Map<string, string>();
    if (sums) {
      const txt = await (await fetch(sums.browser_download_url)).text();
      for (const dong of txt.split('\n')) {
        const m = dong.trim().match(/^([0-9a-f]{64})\s+\*?(.+)$/);
        if (m) bang.set(m[2], m[1]);
      }
    }
    const thuTu = ['x64', 'x86', 'arm64ec'];
    // Tên tệp V1 (VERSIONING 2.2): tsutramon_{v}_windows-{arch}.zip; tên cũ {..}_{arch}_Lite.zip phát hành song song
    // một bản cho Scoop/liên kết cũ (ADR-0001 mục 3.2) - mỗi kiến trúc chỉ hiện MỘT gói, ưu tiên tên mới.
    const kienTrucCua = (ten: string) =>
      ten.match(/_windows-(x64|x86|arm64ec)\.zip$/)?.[1] ?? ten.match(/_(x64|x86|arm64ec)_Lite\.zip$/)?.[1] ?? '';
    const theoKienTruc = new Map<string, GoiTai>();
    for (const a of r.assets.filter((x) => x.name.endsWith('.zip'))) {
      const kienTruc = kienTrucCua(a.name);
      const moi = /_windows-/.test(a.name);
      const cu = theoKienTruc.get(kienTruc);
      if (cu && (/_windows-/.test(cu.ten) || !moi)) continue;
      theoKienTruc.set(kienTruc, { kienTruc, ten: a.name, url: a.browser_download_url, sha256: bang.get(a.name) ?? '', kichThuoc: a.size });
    }
    const goi = [...theoKienTruc.values()].sort((a, b) => thuTu.indexOf(a.kienTruc) - thuTu.indexOf(b.kienTruc));
    return {
      tag: r.tag_name,
      phienBan: r.tag_name.replace(/^[Vv]/, '').replace(/-vn$/, ''),   // V26.11.1202-vn (cũ) và v26.11.DDNN (VERSIONING 2.2)
      ngay: new Date(r.published_at).toLocaleDateString('en-GB', { timeZone: 'Asia/Ho_Chi_Minh' }),   // DD/MM/YYYY giờ VN (DESIGN_SYSTEM mục 4)
      url: r.html_url,
      goi,
    };
  })();
  return boNho;
}

export const TEN_KIEN_TRUC: Record<string, string> = {
  x64: 'Windows 64-bit (x64) - phổ biến nhất',
  x86: 'Windows 32-bit (x86)',
  arm64ec: 'Windows trên ARM (ARM64EC)',
};
