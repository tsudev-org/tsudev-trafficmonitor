**[Tiếng Việt](./HuongDanTaoSkin.md) | English** · [Home](./README_en-us.md) · [Skin gallery](./SkinGallery_en-us.md)

# Skin making-up tutorial for TrafficMonitor-VN

A skin defines the background image, font, text colours and item positions of TrafficMonitor's **main window** (the floating window). The taskbar window does not use skins.

> This guide is written from the skin loader source code (`TrafficMonitor/SkinFile.cpp`).

## 1. Folder structure

Each skin is a subfolder of the `skins` folder (next to `TrafficMonitor.exe`). **The folder name is the skin name.**

```
TrafficMonitor.exe
skins\
  0_tsudev_Cu\
    background.png      ← background when "Show More Info" is OFF (small window)
    background_l.png    ← background when "Show More Info" is ON (large window)
    skin.xml            ← skin configuration file (recommended)
```

| File | Required | Notes |
|---|---|---|
| `background.png` / `background.bmp` | Yes | Small background. PNG is preferred; BMP is used if loading the PNG fails. |
| `background_l.png` / `background_l.bmp` | Yes | Large background. |
| `skin.xml` | Recommended | Current format; supports every item (temperature, GPU, disk, plugins...). |
| `skin.ini` | No | Legacy format, only for compatibility with skins made before 1.80. |

* **PNG** supports a **transparent** background (alpha channel). With PNG, the window is drawn with `UpdateLayeredWindow` + GDI+.
* **BMP** does not support transparency; you can still use the window-wide opacity (**Window Opacity** in the right-click menu).
* Name the folder with **plain ASCII letters and no spaces** (for example `LightBlue_Accent`) to avoid path problems.

## 2. The `skin.xml` file

The file must be saved as **UTF-8**. A complete example:

```xml
<root>
  <skin>
    <text_color>16777215,16777215,16777215,16777215</text_color>
    <specify_each_item_color>0</specify_each_item_color>
    <skin_author>Your name</skin_author>
    <font name="Segoe UI" size="9" style="0"/>
    <display_text>
      <up>Up: </up>
      <down>Down: </down>
      <cpu>CPU: </cpu>
      <memory>RAM: </memory>
    </display_text>
  </skin>
  <layout text_height="20" no_label="0">
    <layout_l width="220" height="60">
      <up     x="6"   y="4"  width="100" align="0" show="1"/>
      <down   x="114" y="4"  width="100" align="0" show="1"/>
      <cpu    x="6"   y="32" width="100" align="0" show="1"/>
      <memory x="114" y="32" width="100" align="0" show="1"/>
    </layout_l>
    <layout_s width="220" height="28">
      <up   x="6"   y="4" width="100" align="0" show="1"/>
      <down x="114" y="4" width="100" align="0" show="1"/>
    </layout_s>
  </layout>
  <preview width="230" height="100">
    <l x="0" y="0"/>
    <s x="0" y="66"/>
  </preview>
</root>
```

### 2.1. The `<skin>` node — general information

| Node | Meaning |
|---|---|
| `<text_color>` | Comma-separated text colours, in the order of the display items. Colours are written as the **decimal value of a COLORREF** (`0xBBGGRR`): white = `16777215`, red (`RGB(255,0,0)`) = `255`, blue (`RGB(0,0,255)`) = `16711680`. Items without a colour use the first one. |
| `<specify_each_item_color>` | `1`: each item uses its own colour from `text_color`; `0`: every item uses the first colour. |
| `<skin_author>` | Author name, shown in the **Change Skin** dialog. |
| `<font name=".." size=".." style=".."/>` | Default skin font. `style` is a sum of bits: `1` bold, `2` italic, `4` underline, `8` strikethrough. |
| `<display_text>` | Default label of each item (child node names in section 2.3). Users can still change them in the display text settings. |

### 2.2. The `<layout>` node — layout

* Attribute `text_height`: line height (pixels at 96 DPI). `no_label="1"`: hide all labels and show values only.
* `<layout_l>`: layout when **Show More Info** is ON; `<layout_s>`: when it is OFF. The `width`/`height` attributes are the window size and should match the corresponding background image.
* Each item node has these attributes:

| Attribute | Meaning |
|---|---|
| `x`, `y` | Top-left position (pixels at 96 DPI; the program scales them to the screen DPI). |
| `width` | Width of the display area. |
| `align` | `0` left, `1` right, `2` centre, `3` justified (label on the left, value on the right). |
| `show` | `1` visible, `0` hidden. |

### 2.3. Node names of the display items

| Node name | Item |
|---|---|
| `up` / `down` / `total_speed` | Upload / download / total speed |
| `today_traffic` | Traffic used today |
| `cpu` / `cpu_freq` / `memory` | CPU usage / CPU frequency / memory usage |
| `gpu` / `hdd` | GPU usage / disk usage |
| `cpu_temperature` / `gpu_temperature` / `hdd_temperature` / `main_board_temperature` | CPU / GPU / disk / motherboard temperature (only in builds with temperature monitoring) |

### 2.4. Showing plugin items — the `<plugin_map>` node

To place a plugin item on a skin, declare a node name of your choice mapped to the **plugin item ID**, then use that name in `<layout_l>`/`<layout_s>`:

```xml
<plugin_map>
  <clock>ra1YX2g1</clock>
</plugin_map>
...
<clock x="6" y="60" width="100" align="0" show="1"/>
```

Find the plugin item ID in **Plug-in manage...** → **Details...** of that plugin.

### 2.5. The `<preview>` node — preview image

Defines how the preview in the **Change Skin** dialog is composed: `width`/`height` are the size of the preview area; `<l>` and `<s>` are where the large and small windows are placed in it.

## 3. Quick workflow

1. Copy an existing skin (for example `skins\0_tsudev_Cu`) into a new folder.
2. Replace `background.png` and `background_l.png` with your images (keep their size, or update `width`/`height` in `skin.xml` to match).
3. Edit `skin.xml`: author, font, colours, item positions.
4. Open TrafficMonitor → right-click menu → **Other Functions** → **Change Skin...**, select the new skin to preview and apply it. After each edit, switch to another skin and back to reload it.

## 4. Safety notes

* A skin contains only images and configuration files, **never executable code**. If a downloaded "skin" package includes `.exe`, `.dll`, `.bat`, `.ps1`, `.vbs`... files, **do not run them**.
* Only download skins from sources you trust. Included skins and how to install more: [Skin gallery](./SkinGallery_en-us.md).
