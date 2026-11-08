import { SITE } from '../lib/noi-dung';

const TRANG = ['/', '/tai-ve/', '/huong-dan/', '/huong-dan/hoi-dap/', '/huong-dan/kho-giao-dien/', '/huong-dan/tu-lam-giao-dien/',
  '/huong-dan/plugin/', '/huong-dan/phat-trien-plugin/', '/nhat-ky/', '/bao-mat/', '/ban-quyen/'];

export function GET() {
  const body = `<?xml version="1.0" encoding="UTF-8"?>\n<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">\n${TRANG.map((p) => `  <url><loc>${SITE}${p}</loc></url>`).join('\n')}\n</urlset>\n`;
  return new Response(body, { headers: { 'Content-Type': 'application/xml; charset=utf-8' } });
}
