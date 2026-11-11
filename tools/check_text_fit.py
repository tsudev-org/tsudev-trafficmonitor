#!/usr/bin/env python3
"""Ước lượng chữ bị cắt trong hộp thoại: đo chuỗi dịch bằng Segoe UI 9pt (cần Pillow và tệp segoeui.ttf)
so với kích thước điều khiển trong tsudev-trafficmonitor.rc.

Cách dùng: python tools/check_text_fit.py tsudev-trafficmonitor/tsudev-trafficmonitor.rc tsudev-trafficmonitor/language/Vietnamese.ini [segoeui.ttf]
Lưu ý: điều khiển được CBaseDialog::RepositionTextBasedControls tự giãn khi chạy vẫn bị liệt kê; cần đối chiếu InitializeControls.
"""
import re,sys
from PIL import ImageFont
rc=open(sys.argv[1],'rb').read()[2:].decode('utf-16-le')
ini={}
for l in open(sys.argv[2],encoding='utf-8-sig'):
    m=re.match(r'\s*([A-Za-z0-9_]+)\s*=\s*"?(.*?)"?\s*$',l)
    if m: ini[m.group(1)]=m.group(2).replace('""','"')
ini.setdefault('TXT_OK','OK')
font=ImageFont.truetype(sys.argv[3] if len(sys.argv)>3 else 'C:/Windows/Fonts/segoeui.ttf',12)  # 9pt @96dpi
avg=(font.getlength('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz')/26+1)//2
print('base unit x =',avg, file=sys.stderr)
dlg=None; out=[]
raw_lines=rc.splitlines(); joined=[]
for l in raw_lines:
    if joined and joined[-1].rstrip().endswith((',','|')): joined[-1]=joined[-1].rstrip()+' '+l.strip()
    else: joined.append(l)

ctl=re.compile(r'^\s*(LTEXT|RTEXT|CTEXT|PUSHBUTTON|DEFPUSHBUTTON|GROUPBOX|CONTROL)\s+"([^"]*)"\s*,\s*(\w+)\s*,(.*)$')
for line in joined:
    m=re.match(r'^(\w+)\s+DIALOGEX',line)
    if m: dlg=m.group(1); continue
    if line.strip()=='END': continue
    m=ctl.match(line)
    if not m or not dlg: continue
    kind,text,cid,rest=m.groups()
    if not text.startswith('TXT_'): continue
    parts=[p.strip() for p in rest.split(',')]
    if kind=='CONTROL':
        cls=parts[0].strip('"'); style=parts[1]
        x,y,w,h=map(int,parts[2:6])
        if 'BS_AUTOCHECKBOX' in style or 'BS_AUTORADIOBUTTON' in style or 'BS_3STATE' in style: kind='CHECK'
        elif cls=='Button': kind='PUSHBUTTON'
        else: kind='CONTROL:'+cls
    else:
        x,y,w,h=map(int,parts[:4])
    tr=ini.get(text)
    if tr is None: continue
    tr_disp=tr.replace('&&','\0').replace('&','').replace('\0','&')
    lines=tr_disp.replace('\\n','\n').split('\n')
    tw=max(font.getlength(s) for s in lines)
    avail=w*avg/4
    if kind=='CHECK': avail-=16
    elif kind in('PUSHBUTTON','DEFPUSHBUTTON'): avail-=8
    elif kind=='GROUPBOX': avail-=12
    # multi-line statics wrap: allow if height fits
    rows_avail=max(1,int(h*16/8//16)) if kind in('LTEXT','RTEXT','CTEXT') else 1
    if kind in('LTEXT','RTEXT','CTEXT') and rows_avail>1:
        need_rows=sum(int(font.getlength(s)//max(avail,1))+1 for s in lines)
        if need_rows<=rows_avail: continue
    elif tw<=avail: continue
    out.append((dlg,cid,kind,w,round(tw*4/avg),text,tr_disp))
for o in out: print('%-36s %-34s %-12s rc=%3d cần=%3d  %s = %s'%o)
print(len(out),'mục có thể bị cắt',file=sys.stderr)
