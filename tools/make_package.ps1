# Đóng gói bản phát hành (dùng chung cho release.yml và bước thử trong build.yml).
# Chép exe/dll + skins + giấy phép vào <WorkDir>\<Name>\TrafficMonitor rồi tạo zip TẤT ĐỊNH bằng tools/make_zip.py
# (thời gian mục = Epoch, thường là thời điểm commit; thứ tự và thuộc tính cố định).
# Giấy phép Anti 996 yêu cầu kèm nguyên văn giấy phép trong mọi bản phân phối.
param(
    [Parameter(Mandatory)] [string]$Name,       # vd tsudev-trafficmonitor_26.11.0905_x64_Lite
    [Parameter(Mandatory)] [string]$BinDir,     # thư mục chứa TrafficMonitor.exe
    [Parameter(Mandatory)] [string]$WorkDir,    # thư mục tạm để dựng cấu trúc gói
    [Parameter(Mandatory)] [string]$OutZip,
    [Parameter(Mandatory)] [long]$Epoch
)
$ErrorActionPreference = 'Stop'
$root = Join-Path $WorkDir $Name
$stage = Join-Path $root 'TrafficMonitor'
if (Test-Path $root) { Remove-Item $root -Recurse -Force }
New-Item -ItemType Directory -Force -Path $stage | Out-Null
Copy-Item (Join-Path $BinDir 'TrafficMonitor.exe') $stage
Get-ChildItem (Join-Path $BinDir '*.dll') -ErrorAction SilentlyContinue | Copy-Item -Destination $stage
Copy-Item 'TrafficMonitor\skins' $stage -Recurse
Copy-Item LICENSE, LICENSE_CN, LICENSE_VI.md, NOTICE.md, README.md, SECURITY.md $stage
python -I tools/make_zip.py --root $root --entry TrafficMonitor --out $OutZip --epoch $Epoch
if ($LASTEXITCODE -ne 0) { throw "make_zip.py thất bại" }
