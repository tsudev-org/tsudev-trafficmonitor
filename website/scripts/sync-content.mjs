// Chép ảnh chụp màn hình của repo (Screenshots/) vào public/anh/ trước khi build.
// Nguồn duy nhất của ảnh là thư mục Screenshots/ ở gốc repo; public/anh/ là bản sinh ra (không commit).
import { cpSync, rmSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';

const web = join(dirname(fileURLToPath(import.meta.url)), '..');
const dich = join(web, 'public/anh');
rmSync(dich, { recursive: true, force: true });
cpSync(join(web, '../Screenshots'), dich, { recursive: true });
console.log('[sync] Screenshots/ -> public/anh/');
