// Tên miền cũ trafficmonitor.tsudev.com chuyển hướng 301 VĨNH VIỄN sang tramon.tsudev.com, giữ đường dẫn + truy vấn
// (ADR-0001 mục 3.6). Cả hai tên miền gắn vào cùng dự án Cloudflare Pages; _redirects không lọc theo host nên dùng
// middleware. KHÔNG gỡ: bản app <= 26.11.1202, README cũ, kết quả tìm kiếm còn trỏ tên miền cũ.
const CU = 'trafficmonitor.tsudev.com';
const MOI = 'tramon.tsudev.com';

export async function onRequest({ request, next }) {
  const url = new URL(request.url);
  if (url.hostname === CU) {
    url.hostname = MOI;
    url.protocol = 'https:';
    url.port = '';
    return new Response(null, { status: 301, headers: { Location: url.toString(), 'Cache-Control': 'public, max-age=86400' } });
  }
  return next();
}
