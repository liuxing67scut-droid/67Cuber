# 魔方色块与面方向调整方案

## 当前问题

当前游戏界面已经偏木色、低饱和，背景也经过压暗处理；但魔方贴纸仍使用高饱和标准色，例如纯绿、纯红、纯黄。视觉上会比按钮和背景更“跳”，整体不够统一。

另外，当前程序的前后面颜色约定是：

| 逻辑面 | 当前颜色字符 | 当前视觉颜色 |
| --- | --- | --- |
| F 前 | G | 绿 |
| B 后 | B | 蓝 |
| R 右 | R | 红 |
| L 左 | O | 橙 |
| U 上 | Y | 黄 |
| D 下 | W | 白 |

如果希望更接近常见实体魔方摆放，可以把前后面的蓝、绿对调，也就是：

| 逻辑面 | 新颜色字符 | 新视觉颜色 |
| --- | --- | --- |
| F 前 | B | 蓝 |
| B 后 | G | 绿 |
| R 右 | R | 红 |
| L 左 | O | 橙 |
| U 上 | Y | 黄 |
| D 下 | W | 白 |

这不是单纯换 RGB。它会影响求解器初始颜色、教学模式中心块、教学填色、练习模式方向标识和注释说明，需要一次性处理完整。

## 推荐路线

推荐分两步做，而不是一次性只替换几个 `RGB(...)`。

第一步：集中颜色配置。

把魔方颜色字符到 RGB 的映射集中到一个新模块，例如：

- `core/cube_colors.h`
- `core/cube_colors.cpp`

这个模块只负责两件事：

- `cubeColorToRgb(char color)`：把 `G/R/B/O/Y/W` 转成实际绘制颜色。
- `cubeFaceColorToRgb(int face)`：按逻辑面 `F/L/U/D/R/B` 返回该面的默认显示颜色。

这样以后如果只想调色调，不用到处搜索 `RGB(...)`。

第二步：统一切换前后面颜色约定。

把 `cubesolver.cpp` 里的默认颜色顺序从：

```cpp
const char Color[6] = { 'G','O','Y','W','R','B' };
```

改为：

```cpp
const char Color[6] = { 'B','O','Y','W','R','G' };
```

这表示：

- `F` 面默认是蓝色字符 `B`
- `B` 面默认是绿色字符 `G`
- 其他四个面保持不变

求解算法本身主要按 `F/L/U/D/R/B` 面编号转动，不依赖“G 一定是前面”。只要所有入口和显示统一使用新的 `Color` 约定，算法逻辑不需要重写。

## 建议色值

参考你给的色调图，可以先用这一组低饱和木质环境配色：

| 颜色字符 | 中文 | RGB |
| --- | --- | --- |
| Y | 黄 | `RGB(220, 180, 100)` |
| W | 白 | `RGB(240, 230, 210)` |
| R | 红 | `RGB(180, 80, 60)` |
| O | 橙 | `RGB(200, 120, 60)` |
| B | 蓝 | `RGB(100, 140, 180)` |
| G | 绿 | `RGB(120, 160, 100)` |

这组颜色会比当前纯色柔和很多。风险是：魔方辨识度会下降一点，尤其蓝和绿都偏灰，需要实际测试。如果觉得不够鲜明，可以只把每个通道略微提高 10 到 20。

## 需要修改的位置

### 1. 新增颜色模块

新增：

- `core/cube_colors.h`
- `core/cube_colors.cpp`

建议接口：

```cpp
COLORREF cubeColorToRgb(char color);
COLORREF cubeFaceColorToRgb(int face);
```

注意避免命名为 `setColor`，因为 `cubesolver.cpp` 已经有控制台颜色函数 `setColor(...)`。

### 2. 修改求解器默认颜色约定

文件：

- `core/cubesolver.cpp`

修改：

- `Color[6]`
- 文件顶部注释

目标：

```cpp
//F蓝 L橙 U黄 D白 R红 B绿
const char Color[6] = { 'B','O','Y','W','R','G' };
```

这一步会影响：

- 自动模式随机打乱后的逻辑颜色
- 练习模式随机打乱后的逻辑颜色
- 教学模式中心块默认颜色
- `isCubeSolved`
- `validateColoring`

这些函数都以 `Color[]` 或中心块字符为依据，理论上会自动跟随新约定。

### 3. 修改可视魔方初始颜色

文件：

- `core/rubikcube.cpp`

当前构造函数里直接写了多个 `RGB(...)`。应改成按逻辑面取色，例如：

- 前面 `F` 用 `cubeFaceColorToRgb(F)`
- 后面 `B` 用 `cubeFaceColorToRgb(B)`
- 右面 `R` 用 `cubeFaceColorToRgb(R)`
- 左面 `L` 用 `cubeFaceColorToRgb(L)`
- 顶面 `U` 用 `cubeFaceColorToRgb(U)`
- 底面 `D` 用 `cubeFaceColorToRgb(D)`

同时更新注释为：

```cpp
//面颜色约定：F蓝 R红 B绿 L橙 U黄 D白
```

### 4. 修改教学模式中心块颜色

文件：

- `core/rubikcube.cpp`

函数：

- `RubikCube::setCentersColoredOnly()`

这里目前也写死了六个中心块的 `RGB(...)`。应改成同样使用 `cubeFaceColorToRgb(...)`。

否则教学模式中心块会和新的 `Color[]` 逻辑不一致。

### 5. 修改逻辑颜色转显示颜色

文件：

- `core/rubikcube.cpp`

函数：

- `RubikCube::recolorFromLogicalCube(...)`

当前函数内部有一个 `charToColor` lambda，把 `G/R/B/O/Y/W` 转成 RGB。应删除这个局部 lambda，改为统一调用：

```cpp
cubeColorToRgb(c)
```

这样自动模式、练习模式、算法还原动画后的颜色显示都会统一。

### 6. 修改练习模式坐标轴颜色

文件：

- `core/rubikcube.cpp`

函数：

- `RubikCube::drawTestAxes()`

当前 `F/B/R/L/U/D` 方向标识颜色写死。应改成：

```cpp
COLORREF baseCol = cubeFaceColorToRgb(csIdx);
```

这样坐标轴的 F 方向会跟随蓝色，B 方向会跟随绿色。

### 7. 修改教学填色面板颜色

文件：

- `ui/ui.cpp`

位置：

- `TeachColorPanel::TeachColorPanel()`

当前 `G/O/Y/W/R/B` 六个颜色按钮写死 RGB。应改成：

```cpp
m_keys.push_back({ 'G', cubeColorToRgb('G'), ... });
```

这样教学模式弹出的颜色面板与实际魔方色块一致。

### 8. 修改教学点击颜色写回

文件：

- `game/play_modes/game_play_mode_teach.cpp`

函数：

- `handleTeachPanelInteraction(...)`

当前内部有 `colorCharToRGB` lambda。应删除这个 lambda，改为：

```cpp
COLORREF col = cubeColorToRgb(chosen);
```

这样教学模式用户选择颜色后，视觉贴纸和逻辑字符一致。

### 9. 更新注释

需要检查并更新这些注释：

- `core/cubesolver.cpp` 顶部面颜色约定
- `core/rubikcube.cpp` 构造函数颜色约定
- `core/rubikcube.cpp` `drawTestAxes()` 中“标准色”说明
- 新增 `cube_colors.*` 的模块说明

注释原则：

- 中文简洁
- `//` 后不加空格
- 使用 UTF-8 BOM + CRLF

## 不建议修改的位置

### 不建议改公式按钮

文件：

- `ui/ui.cpp`

公式面板按钮是 `U/D/F/B/R/L`，它们表示转动方向，不表示颜色。前后面变色后，公式符号仍然应该保持 `F/B/R/L/U/D` 不变。

### 不建议改旋转算法

文件：

- `core/cubesolver.cpp`

函数：

- `front/back/right/left/up/down`
- `Solute`
- 各种分层还原函数

这些函数按逻辑面编号工作。只要 `Color[]` 和显示映射同步，不需要重写算法。

### 不建议改 `mapStickerToFace`

文件：

- `core/rubikcube.cpp`

函数：

- `RubikCube::mapStickerToFace(...)`

它负责把屏幕上点到的物理贴纸映射到逻辑面 `F/R/B/L/U/D`。这和颜色无关，不应该因为蓝绿对调而修改。

## 风险点

### 风险 1：只换 RGB，不换 `Color[]`

如果只把绿色 RGB 和蓝色 RGB 互换，显示上会接近目标，但内部逻辑仍然是：

- `F = G`
- `B = B`

这会导致代码注释和实际显示含义不一致。短期能跑，但后续维护容易混乱。

### 风险 2：只换 `Color[]`，不换所有显示映射

如果只改 `Color[6]`，但 `rubikcube.cpp`、教学面板仍然用旧 RGB，那么逻辑和显示会错位。教学模式尤其容易出问题。

### 风险 3：色值过低饱和

参考图的蓝、绿、红都更柔和，整体更协调，但可能降低魔方辨识度。建议先按参考图改一版，编译后重点观察：

- 自动模式复原动画中颜色是否容易区分
- 练习模式手动转动时蓝/绿是否容易混
- 教学填色面板中六色是否清楚
- 排行榜/设置面板弹出时是否遮挡后仍清晰

## 推荐执行顺序

### 第 1 步：只做颜色集中和蓝绿面约定切换

修改代码：

- 新增 `core/cube_colors.h/.cpp`
- 修改 `core/cubesolver.cpp`
- 修改 `core/rubikcube.cpp`
- 修改 `ui/ui.cpp`
- 修改 `game/play_modes/game_play_mode_teach.cpp`
- 更新 `cube.vcxproj`
- 更新 `cube.vcxproj.filters`

不改算法函数。

### 第 2 步：VS2022 编译测试

测试项目：

- 自动模式：随机打乱、算法还原、一键复位
- 练习模式：随机打乱、公式面板、手动复原完成判定
- 教学模式：中心块颜色、右键填色、颜色未填完整提示、填完整后算法还原
- 坐标轴显示：F 是否蓝，B 是否绿

### 第 3 步：根据观感微调 RGB

如果色块偏灰：

- 蓝色可从 `RGB(100,140,180)` 调到 `RGB(90,150,210)`
- 绿色可从 `RGB(120,160,100)` 调到 `RGB(105,175,100)`
- 红色可从 `RGB(180,80,60)` 调到 `RGB(200,70,55)`

这一步只改 `cube_colors.cpp`，不再碰算法和模式代码。

## 建议先确认的问题

正式改代码前建议确认两点：

1. 是否确定要把前后面约定改为 `F蓝 B绿`，而不是只做视觉调色？
2. 是否先采用参考图中的低饱和 RGB 作为第一版测试色？

如果确认，就按“推荐执行顺序”的第 1 步实施。
