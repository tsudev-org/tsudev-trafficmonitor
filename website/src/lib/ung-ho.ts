// Dữ liệu trang /ung-ho/: kênh ủng hộ (src/data/ung-ho.json). Chép từ bản tham chiếu tsudev-files/website/src/lib/ung-ho.ts
// (phiếu 20261112-01), bỏ phần nhà tài trợ. Sai dữ liệu thì dừng build.
import { createHash } from 'node:crypto';
import { existsSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import duLieu from '../data/ung-ho.json';

export interface VietQr { bin: string; soTaiKhoan: string; payload: string }
export interface KenhUngHo { ten: string; moTa: string; url?: string; chiTiet: string[]; anhQr?: string; vietqr?: VietQr }

/** CRC-16/CCITT-FALSE của VietQR (EMVCo), 4 chữ số hex in hoa. */
export function crc16(s: string): string {
  let c = 0xffff;
  for (const b of Buffer.from(s, 'ascii')) {
    c ^= b << 8;
    for (let i = 0; i < 8; i++) c = c & 0x8000 ? ((c << 1) ^ 0x1021) & 0xffff : (c << 1) & 0xffff;
  }
  return c.toString(16).toUpperCase().padStart(4, '0');
}

/** Tách chuỗi TLV (tag 2 số, độ dài 2 số) thành bảng tag -> giá trị. */
function tlv(s: string): Map<string, string> {
  const m = new Map<string, string>();
  for (let i = 0; i < s.length; ) {
    const tag = s.slice(i, i + 2), len = Number(s.slice(i + 2, i + 4));
    if (!/^\d{2}$/.test(tag) || !Number.isInteger(len) || i + 4 + len > s.length) throw new Error('VietQR sai cấu trúc TLV');
    m.set(tag, s.slice(i + 4, i + 4 + len));
    i += 4 + len;
  }
  return m;
}

/** VietQR hợp lệ (CRC đúng) và chuyển tiền tới đúng BIN + số tài khoản đã khai. */
export function kiemVietQr(q: VietQr): void {
  const { payload, bin, soTaiKhoan } = q;
  if (!payload.endsWith(crc16(payload.slice(0, -4))) || payload.slice(-8, -4) !== '6304') throw new Error('VietQR sai CRC');
  const goc = tlv(payload);
  const nhan = tlv(tlv(goc.get('38') ?? '').get('01') ?? '');
  if (tlv(goc.get('38') ?? '').get('00') !== 'A000000727') throw new Error('VietQR không phải của NAPAS');
  if (goc.get('53') !== '704' || goc.get('58') !== 'VN') throw new Error('VietQR không phải VND/VN');
  if (nhan.get('00') !== bin || nhan.get('01') !== soTaiKhoan)
    throw new Error(`VietQR chuyển tới ${nhan.get('00')}/${nhan.get('01')}, khác ${bin}/${soTaiKhoan} đã khai`);
}

// Tên/miền của dự án gốc và các kênh tài trợ cũ: không bao giờ được xuất hiện ở trang ủng hộ của tsudev.
const CAM = /zhongyang|alipay|wechat|weixin|paypal\.me|ko-fi|patreon|afdian|github\.com\/sponsors\/(?!tsudev)/i;
const THU_MUC_QR = join(process.cwd(), 'public');

/** Kênh hợp lệ: có tên, mô tả và ít nhất một trong url/chiTiet/anhQr. Sai thì dừng build (không lặng lẽ bỏ qua). */
export function kiemKenh(kenh: unknown[]): KenhUngHo[] {
  return kenh.map((k, i) => {
    const { ten, moTa, url, chiTiet, anhQr, anhQrSha256, vietqr } = (k ?? {}) as Record<string, unknown>;
    const loi = (m: string) => new Error(`ung-ho.json, kênh #${i + 1}${typeof ten === 'string' ? ` (${ten})` : ''}: ${m}`);
    if (typeof ten !== 'string' || !ten.trim()) throw loi('thiếu "ten"');
    if (typeof moTa !== 'string' || !moTa.trim()) throw loi('thiếu "moTa"');
    if (url !== undefined && typeof url !== 'string') throw loi('"url" phải là chuỗi');
    if (chiTiet !== undefined && (!Array.isArray(chiTiet) || chiTiet.some((d) => typeof d !== 'string')))
      throw loi('"chiTiet" phải là danh sách chuỗi, ví dụ ["Ngân hàng: ...", "Số tài khoản: ..."]');
    if (anhQr !== undefined && (typeof anhQr !== 'string' || !/^\/ung-ho\/[a-z0-9-]+\.(png|webp|jpg|svg)$/.test(anhQr)))
      throw loi('"anhQr" phải dạng "/ung-ho/<ten-tep>.png" (chữ thường, số, gạch ngang)');
    if (anhQr && !existsSync(join(THU_MUC_QR, anhQr))) throw loi(`không thấy tệp website/public${anhQr}`);
    // Ghim chuỗi băm: tráo ảnh QR (đổi tài khoản nhận tiền) mà không sửa tệp dữ liệu này thì build dừng.
    if (anhQr) {
      if (typeof anhQrSha256 !== 'string' || !/^[0-9a-f]{64}$/.test(anhQrSha256)) throw loi('thiếu "anhQrSha256" (sha256sum của ảnh QR)');
      const that = createHash('sha256').update(readFileSync(join(THU_MUC_QR, anhQr))).digest('hex');
      if (that !== anhQrSha256) throw loi(`ảnh QR có sha256 ${that}, khác "anhQrSha256" đã ghim`);
    }
    if (vietqr !== undefined) {
      const q = vietqr as VietQr;
      if (typeof q?.bin !== 'string' || typeof q.soTaiKhoan !== 'string' || typeof q.payload !== 'string')
        throw loi('"vietqr" cần bin, soTaiKhoan, payload');
      try { kiemVietQr(q); } catch (e) { throw loi((e as Error).message); }
      const chu = ((chiTiet as string[] | undefined) ?? []).join(' ').replace(/\s/g, '');
      if (!chu.includes(q.soTaiKhoan)) throw loi('số tài khoản trong "chiTiet" khác số tài khoản của VietQR');
    }
    if (!url && !(chiTiet as string[] | undefined)?.length && !anhQr) throw loi('cần ít nhất một trong "url", "chiTiet", "anhQr"');
    let href: string | undefined;
    if (url) {
      const u = new URL(url);
      if (u.protocol !== 'https:') throw loi('URL phải bắt đầu bằng https://');
      href = u.href;
    }
    if (CAM.test(JSON.stringify(k))) throw loi('còn trỏ tới dự án gốc/kênh cũ');
    return { ten: ten.trim(), moTa: moTa.trim(), url: href, chiTiet: (chiTiet as string[] | undefined) ?? [], anhQr: anhQr as string | undefined, vietqr: vietqr as VietQr | undefined };
  });
}

export const KENH: KenhUngHo[] = kiemKenh(duLieu.kenh);

