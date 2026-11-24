// Dữ liệu trang /security/: bằng chứng tin cậy TỰ KIỂM CHỨNG ĐƯỢC (phiếu 20261125-SEC2).
// Nguồn: docs/security/trust.json, đọc lúc build (không gọi dịch vụ ngoài khi người xem mở trang). Sai dữ liệu thì dừng build.
import { docTep } from './noi-dung';

interface BangChung { ma: string; ten: string; ket_qua: string; lien_ket: string; ngay_do: string; ghi_chu?: string }
interface ChuaCo { ma: string; ten: string; ly_do: string; lien_ket?: string }

/** Mục đo cũ hơn số ngày này (tính từ `cap_nhat`) thì dừng build: số liệu phải được đo lại. */
const HAN_NGAY = 120;
const NGAY = /^\d{4}-\d{2}-\d{2}$/;

function kiemLienKet(url: string, noi: string): void {
  const u = new URL(url);
  if (u.protocol !== 'https:') throw new Error(`trust.json: ${noi} phải là liên kết https: ${url}`);
  // Trang kết quả phải do bên thứ ba lưu trữ, không trỏ về chính website này
  if (u.hostname === 'tramon.tsudev.com') throw new Error(`trust.json: ${noi} không được trỏ về chính website: ${url}`);
}

const duLieu = JSON.parse(docTep('docs/security/trust.json')) as { cap_nhat: string; bang_chung: BangChung[]; chua_co: ChuaCo[] };
if (!NGAY.test(duLieu.cap_nhat)) throw new Error('trust.json: cap_nhat sai định dạng YYYY-MM-DD');
const capNhat = Date.parse(duLieu.cap_nhat);

for (const b of duLieu.bang_chung) {
  for (const k of ['ma', 'ten', 'ket_qua', 'lien_ket', 'ngay_do'] as const) {
    if (!b[k]?.trim()) throw new Error(`trust.json: bằng chứng "${b.ten ?? '?'}" thiếu ${k}`);
  }
  kiemLienKet(b.lien_ket, b.ten);
  if (!NGAY.test(b.ngay_do)) throw new Error(`trust.json: "${b.ten}" ngay_do sai định dạng`);
  const tuoi = (capNhat - Date.parse(b.ngay_do)) / 86_400_000;
  if (tuoi < 0) throw new Error(`trust.json: "${b.ten}" đo sau ngày cap_nhat`);
  if (tuoi > HAN_NGAY) throw new Error(`trust.json: "${b.ten}" đo đã ${Math.round(tuoi)} ngày, cần đo lại`);
}
for (const c of duLieu.chua_co) {
  if (!c.ma?.trim() || !c.ten?.trim() || !c.ly_do?.trim()) throw new Error(`trust.json: mục chưa có "${c.ten ?? '?'}" thiếu ma/ten/ly_do`);
  if (c.lien_ket) kiemLienKet(c.lien_ket, c.ten);
}

/** "2026-11-25" -> "25/11/2026" (quy ước ngày DD/MM/YYYY của website). */
export const ngayVn = (iso: string): string => iso.split('-').reverse().join('/');
export const { cap_nhat: CAP_NHAT, bang_chung: BANG_CHUNG, chua_co: CHUA_CO } = duLieu;
