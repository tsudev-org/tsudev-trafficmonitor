<#
.SYNOPSIS
  Chụp ảnh giao diện TrafficMonitor-VN đang chạy (cho README và để soát chữ bị cắt).

.DESCRIPTION
  Chế độ "readme": chụp cửa sổ nổi, cửa sổ trên thanh tác vụ, hộp thoại Cài đặt và Chi tiết mạng.
  Chế độ "soat": chụp mọi tab của hộp thoại Cài đặt (cuộn từng trang) và các hộp thoại Kho giao diện,
  Quản lý Plugin, Giới thiệu, Lịch sử lưu lượng để kiểm tra chữ bị cắt bằng mắt.

  Script tự mở rồi đóng các hộp thoại bằng cách gửi lệnh menu (WM_COMMAND) tới cửa sổ chính.
  Cần TrafficMonitor đang chạy, cửa sổ nổi và cửa sổ thanh tác vụ đang hiện.

  LƯU Ý QUYỀN RIÊNG TƯ: ảnh Chi tiết mạng chứa địa chỉ MAC, IP, tên card mạng của máy chụp.
  Phải che các giá trị này trước khi đưa ảnh lên repo.

.EXAMPLE
  powershell -NoProfile -ExecutionPolicy Bypass -File tools\chup_man_hinh.ps1 -OutDir C:\temp\shots -Mode readme
#>
param(
    [Parameter(Mandatory = $true)][string]$OutDir,
    [ValidateSet("readme", "soat")][string]$Mode = "readme"
)
$ErrorActionPreference = "Stop"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
Add-Type -AssemblyName System.Drawing
Add-Type @"
using System; using System.Text; using System.Runtime.InteropServices; using System.Collections.Generic;
public class C {
  public delegate bool EP(IntPtr h, IntPtr l);
  [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
  [DllImport("user32.dll")] public static extern bool EnumWindows(EP f, IntPtr l);
  [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr p, EP f, IntPtr l);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder s, int n);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L,T,R,B; }
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
  [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr h, IntPtr hdc, uint f);
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("dwmapi.dll")] public static extern int DwmGetWindowAttribute(IntPtr h, int a, out RECT r, int s);
  public static IntPtr Find(uint target, string cls, string titleNot) {
    IntPtr found = IntPtr.Zero;
    EnumWindows((h,l)=>{ uint pid; GetWindowThreadProcessId(h, out pid);
      if (pid==target && IsWindowVisible(h)) { var c=new StringBuilder(256); GetClassName(h,c,256); var t=new StringBuilder(256); GetWindowText(h,t,256);
        if (c.ToString()==cls && t.ToString()!=titleNot) { found=h; return false; } }
      return true; }, IntPtr.Zero);
    return found;
  }
  public static List<IntPtr> Children(IntPtr p, string cls){ var r=new List<IntPtr>(); EnumChildWindows(p,(h,l)=>{ var c=new StringBuilder(256); GetClassName(h,c,256); if(c.ToString()==cls) r.Add(h); return true;}, IntPtr.Zero); return r; }
  [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
  [DllImport("user32.dll")] public static extern IntPtr GetParent(IntPtr h);
  public static IntPtr FindChild(string trayClass, uint target) {
    IntPtr found = IntPtr.Zero;
    EnumWindows((h,l)=>{ var c=new StringBuilder(256); GetClassName(h,c,256);
      if (c.ToString()==trayClass) { EnumChildWindows(h,(ch,l2)=>{ uint pid; GetWindowThreadProcessId(ch, out pid);
        if (pid==target && IsWindowVisible(ch)) { found=ch; return false; } return true; }, IntPtr.Zero); }
      return found==IntPtr.Zero; }, IntPtr.Zero);
    return found;
  }
  public static string Title(IntPtr h){ var t=new StringBuilder(256); GetWindowText(h,t,256); return t.ToString(); }
}
"@

[C]::SetProcessDPIAware() | Out-Null
$p = Get-Process TrafficMonitor -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $p) { Write-Error "TrafficMonitor chưa chạy." }
$pid_ = [uint32]$p.Id
$main = $p.MainWindowHandle
# Mã lệnh menu (TrafficMonitor\resource.h)
$ID_NETWORK_INFO = 32772; $ID_CHANGE_SKIN = 32819; $ID_TRAFFIC_HISTORY = 32826; $ID_OPTIONS = 32829
$ID_PLUGIN_MANAGE = 32886; $ID_APP_ABOUT = 57664

function ShotScreen($h, $file, $pad) {
  $r = New-Object C+RECT; [C]::GetWindowRect($h, [ref]$r) | Out-Null
  $w = $r.R - $r.L + 2*$pad; $hh = $r.B - $r.T + 2*$pad
  $bmp = New-Object System.Drawing.Bitmap $w, $hh
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen($r.L - $pad, $r.T - $pad, 0, 0, $bmp.Size)
  $bmp.Save($file, [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bmp.Dispose()
  "$file $w x $hh"
}
function ShotWindow($h, $file) {
  # Khung nhìn thấy thực (bỏ viền vô hình của Win10/11)
  $r = New-Object C+RECT; [C]::DwmGetWindowAttribute($h, 9, [ref]$r, 16) | Out-Null
  $w = $r.R - $r.L; $hh = $r.B - $r.T
  [C]::SetForegroundWindow($h) | Out-Null; Start-Sleep -Milliseconds 700
  $bmp = New-Object System.Drawing.Bitmap $w, $hh
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen($r.L, $r.T, 0, 0, $bmp.Size)
  $bmp.Save($file, [System.Drawing.Imaging.ImageFormat]::Png); $g.Dispose(); $bmp.Dispose()
  "$file $w x $hh"
}
function OpenDlg($cmd) {
  [C]::PostMessage($main, 0x0111, [IntPtr]$cmd, [IntPtr]::Zero) | Out-Null
  for ($i=0; $i -lt 30; $i++) { Start-Sleep -Milliseconds 200; $d = [C]::Find($pid_, "#32770", "TrafficMonitorTaskbarWindow"); if ($d -ne [IntPtr]::Zero) { Start-Sleep -Milliseconds 800; return $d } }
  return [IntPtr]::Zero
}
function CloseDlg($d) { [C]::PostMessage($d, 0x0111, [IntPtr]2, [IntPtr]::Zero) | Out-Null; Start-Sleep -Milliseconds 500 }


function Audit($cmd, $name, $tabs) {
  $d = OpenDlg $cmd
  if ($d -eq [IntPtr]::Zero) { "$name not found"; return }
  $tabCtl = [C]::Children($d, "SysTabControl32") | Select -First 1
  $n = if ($tabCtl) { $tabs } else { 1 }
  for ($t=0; $t -lt $n; $t++) {
    if ($tabCtl) {
      [C]::SendMessage($tabCtl, 0x1330, [IntPtr]$t, [IntPtr]::Zero) | Out-Null   # TCM_SETCURFOCUS
      Start-Sleep -Milliseconds 600
    }
    # trang con đang hiện của tab
    $pages = [C]::Children($d, "#32770") | ? { [C]::IsWindowVisible($_) }
    $page = $pages | Select -First 1
    for ($k=0; $k -lt 5; $k++) {
      ShotWindow $d (Join-Path $OutDir ("{0}_t{1}_p{2}.png" -f $name,$t,$k))
      if (-not $page) { break }
      [C]::SendMessage($page, 0x0115, [IntPtr]3, [IntPtr]::Zero) | Out-Null    # WM_VSCROLL SB_PAGEDOWN
      Start-Sleep -Milliseconds 300
    }
  }
  CloseDlg $d
}

if ($Mode -eq "readme") {
    ShotScreen $main (Join-Path $OutDir "main.png") 0
    $tb = [C]::FindChild("Shell_TrayWnd", $pid_)
    if ($tb -ne [IntPtr]::Zero) { ShotScreen $tb (Join-Path $OutDir "taskbar.png") 6 }
    $d = OpenDlg $ID_OPTIONS
    if ($d -ne [IntPtr]::Zero) { ShotWindow $d (Join-Path $OutDir "options.png"); CloseDlg $d }
    $d = OpenDlg $ID_NETWORK_INFO
    if ($d -ne [IntPtr]::Zero) { ShotWindow $d (Join-Path $OutDir "network_info.png"); CloseDlg $d }
}
else {
    Audit $ID_OPTIONS "options" 3
    Audit $ID_CHANGE_SKIN "skin" 1
    Audit $ID_PLUGIN_MANAGE "plugin" 1
    Audit $ID_APP_ABOUT "about" 1
    Audit $ID_TRAFFIC_HISTORY "history" 1
}
