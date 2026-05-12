# 练习模式右键拖拽单层旋转方案

## 目标

只在练习模式中加入“右键拖拽旋转外层”的交互，用于手动复原时快速操作魔方。

要求：

- 只在 `PLAY_MODE_PRACTICE` 生效。
- 自动模式不响应右键拖拽旋转。
- 教学模式继续使用右键选贴纸填色，不受影响。
- 公式面板和右键拖拽可以混合使用。
- 最终真正旋转时，必须同时更新可视魔方和逻辑 `Cube` 数组。
- 手动复原完成判定、计时和成绩记录仍按原逻辑工作。

## 交互思路

右键拖拽不应简单理解成“点到哪个面就转哪个面”。更接近真实操作的方式是：

1. 右键按下时选中一个可见贴纸。
2. 根据该贴纸在屏幕上的四边形，生成两条局部方向轴。
3. 在按下位置或贴纸中心显示辅助十字架。
4. 右键按住拖动时，显示从起点到当前鼠标位置的拖拽轨迹。
5. 松开右键时，根据拖拽向量更接近哪条局部方向轴，判断用户想沿哪个方向“推”这一块。
6. 结合被选中贴纸所在的逻辑面、行列位置，推断应该旋转哪个外层。
7. 第一阶段只打印候选公式，不真正旋转。
8. 方向判断稳定后，再调用和公式面板相同的执行函数真正旋转。

第一版只处理外层，不做中层 `M/E/S`。如果用户点在中间行或中间列并拖出中层含义，第一版先忽略这次操作或打印 `no outer move`，不强行转动。

## 当前代码基础

### 练习模式公式执行

文件：

- `game/play_modes/game_play_mode_practice.cpp`

当前公式面板点击后会：

1. 调用 `up/down/front/back/right/left(Cube, mode)` 修改逻辑数组。
2. 把同一个命令传给 `pRubikcube->bindCommand(Command)` 播放动画。

这是正确路径。后续右键拖拽真正执行旋转时，也必须复用这条路径，不能只让画面动。

### 教学模式右键选贴纸

文件：

- `game/play_modes/game_play_mode_teach.cpp`
- `core/rubikcube.cpp`

已有可复用能力：

- `RubikCube::pickStickerAtScreen(...)`：按屏幕坐标选中可见贴纸。
- `RubikCube::mapStickerToFace(...)`：把物理贴纸映射到逻辑面 `F/R/B/L/U/D` 和行列。

这说明右键拖拽的起点识别已有基础。

## 核心难点

### 1. 需要贴纸屏幕四边形

现在 `RubikCube::pickStickerAtScreen(...)` 能返回：

- `k/i/j`
- `planeIndex`

但它没有把命中的贴纸四边形返回给外部。要画十字架和判断局部方向，需要新增一个安全的查询接口，例如：

```cpp
bool RubikCube::getStickerScreenPolygon(int k, int i, int j, int planeIndex, POINT out[4]) const;
```

这个函数只读几何数据，不改变魔方状态，风险低。

### 2. 需要从四边形得到局部方向轴

贴纸四边形四个点为 `p0 p1 p2 p3`。

可以计算两条屏幕轴：

```cpp
axisA = normalize((p1 - p0) + (p2 - p3));
axisB = normalize((p3 - p0) + (p2 - p1));
```

含义：

- `axisA` 近似沿贴纸一组边方向。
- `axisB` 近似沿另一组边方向。

十字架可以画成以贴纸中心或右键按下位置为中心，两条线分别沿 `axisA`、`axisB`。

### 3. 需要把拖拽方向映射到外层

右键松开时得到拖拽向量：

```cpp
drag = end - start;
```

先判断长度是否足够：

```cpp
dragLength < 20px -> 误触，不执行
```

再比较拖拽方向和两条局部轴的夹角：

```cpp
scoreA = abs(dot(normalize(drag), axisA));
scoreB = abs(dot(normalize(drag), axisB));
```

分数更大的轴就是用户主要拖动方向。

方向正负由 `dot(drag, chosenAxis)` 的正负决定。

### 4. 只支持外层

选中贴纸映射到逻辑面和行列后，可以判断拖拽对应哪条“外层线”。

示例：

- 在 `F` 面上，沿面内水平轴拖动，可能对应转 `U` 或 `D`，取决于点在上排还是下排。
- 在 `F` 面上，沿面内竖直轴拖动，可能对应转 `L` 或 `R`，取决于点在左列还是右列。
- 如果点在中间行并且拖动需要中层，则第一版忽略。

这和“点到 F 面就转 F”不同。新的方案更接近真实拖拽。

## 数据结构建议

文件：

- `game/game_state.h`

在 `MainViewState` 中新增练习模式右键拖拽状态：

```cpp
bool practiceRightDragActive = false;
bool practiceRightDragPreviewVisible = false;
int practiceDragStartX = 0;
int practiceDragStartY = 0;
int practiceDragCurrentX = 0;
int practiceDragCurrentY = 0;
int practiceDragK = -1;
int practiceDragI = -1;
int practiceDragJ = -1;
int practiceDragPlaneIndex = -1;
int practiceDragFace = -1;
int practiceDragRow = -1;
int practiceDragCol = -1;
POINT practiceDragPolygon[4] = {};
POINT practiceDragCenter = { 0, 0 };
```

说明：

- `practiceRightDragActive` 表示右键正在拖拽。
- `practiceRightDragPreviewVisible` 表示本帧需要画十字架和轨迹。
- `practiceDragFace/Row/Col` 用于后续推断候选公式。
- `practiceDragPolygon` 用于绘制十字架和计算方向轴。

右键松开或取消后，这些状态清空。它们只属于当前视图交互，不需要持久化。

## 新增函数建议

### RubikCube 几何查询

文件：

- `core/rubikcube.h`
- `core/rubikcube.cpp`

新增：

```cpp
bool getStickerScreenPolygon(int k, int i, int j, int planeIndex, POINT out[4]) const;
```

职责：

- 参数合法时调用对应 `Plane::getScreenPolygon(...)`。
- 返回当前贴纸投影到屏幕坐标后的四边形。
- 不改变高亮、不改变颜色、不改逻辑 Cube。

### 练习模式右键拖拽处理

文件：

- `game/play_modes/game_play_mode_practice.cpp`
- `game/game_internal.h`

新增：

```cpp
void handlePracticeRightDragRotation(MainViewState& viewState);
void drawPracticeRightDragPreview(MainViewState& viewState);
```

调用建议：

- `handlePracticeRightDragRotation()` 放在游戏运行时交互流程里。
- `drawPracticeRightDragPreview()` 放在场景绘制之后、弹窗和面板之前。

第一版为了简单，也可以在 `handlePracticeRightDragRotation()` 里只更新状态，在 `handleGameRuntimeUi()` 中调用绘制函数。

### 候选公式推断

文件：

- `game/play_modes/game_play_mode_practice.cpp`

新增内部函数：

```cpp
static bool inferPracticeDragMove(
    int face,
    int row,
    int col,
    const POINT polygon[4],
    int startX,
    int startY,
    int endX,
    int endY,
    char& outMove,
    bool& outRev
);
```

职责：

- 计算拖拽向量。
- 根据贴纸四边形计算局部轴。
- 判断更接近哪条轴。
- 根据 `face/row/col/axis/sign` 推断外层公式。
- 如果需要中层或拖拽距离太短，返回 false。

这个函数是调试核心，建议集中写，方便后续修正方向表。

## 外层推断规则

### 基本原则

第一版规则只需要做到：

- 能判断外层。
- 不确定时不执行。
- 方向反了可以集中调整。

不要为了覆盖所有情况强行转动，否则会破坏用户信任。

### 面内行列含义

`mapStickerToFace(...)` 已经给出逻辑面、行、列：

- `row == 0` 表示该面的上侧行。
- `row == 2` 表示该面的下侧行。
- `col == 0` 表示该面的左侧列。
- `col == 2` 表示该面的右侧列。

当拖拽方向接近面内横轴：

- 如果 `row == 0`，倾向转上侧外层。
- 如果 `row == 2`，倾向转下侧外层。
- 如果 `row == 1`，第一版忽略，因为可能是中层。

当拖拽方向接近面内纵轴：

- 如果 `col == 0`，倾向转左侧外层。
- 如果 `col == 2`，倾向转右侧外层。
- 如果 `col == 1`，第一版忽略，因为可能是中层。

### 面到外层的映射表

需要为每个可见逻辑面定义：

- 横轴上排对应哪个逻辑外层。
- 横轴下排对应哪个逻辑外层。
- 纵轴左列对应哪个逻辑外层。
- 纵轴右列对应哪个逻辑外层。

这是最容易出错的部分，建议第一阶段只打印结果，不转魔方。

示例思想：

| 起始面 | 横轴 row=0 | 横轴 row=2 | 纵轴 col=0 | 纵轴 col=2 |
| --- | --- | --- | --- | --- |
| F | U | D | L | R |
| B | U | D | R | L |
| U | B | F | L | R |
| D | F | B | L | R |
| R | U | D | F | B |
| L | U | D | B | F |

这张表是初始草案，不保证方向完全正确。第一阶段打印候选公式就是为了校正它。

### 顺逆判断

确定外层后，还要判断是否反向。

第一版可用方向表，例如：

```cpp
static bool isReverseForDragMove(int face, int move, bool axisPositive);
```

其中：

- `face` 是起始贴纸所在面。
- `move` 是候选外层。
- `axisPositive` 表示拖拽方向沿所选局部轴正向。

方向表会根据实际测试微调。

## 可视辅助设计

### 十字架

右键按下后显示：

- 中心：建议用贴纸屏幕中心，而不是鼠标按下点。这样更稳定。
- 两条轴：沿贴纸四边形的两组边方向。
- 长度：建议 60 到 80 像素。
- 颜色：浅金色或浅木色，避免和魔方六色冲突。
- 线宽：2px。

### 拖拽轨迹

右键按住拖动时显示：

- 从起点到当前鼠标位置的线。
- 末端可画一个小圆点。
- 如果拖拽距离不足阈值，可以用灰色；超过阈值后用高亮色。

### 消失时机

右键松开后：

- 打印判断结果。
- 清除高亮。
- 清空拖拽状态。
- 十字架和轨迹立即消失。

不需要保存上一轮轨迹。

## 分阶段执行计划

### 第 1 步：只补几何查询和拖拽状态

修改：

- `RubikCube::getStickerScreenPolygon(...)`
- `MainViewState` 增加右键拖拽状态
- 新增状态清理函数

不画十字架，不打印公式。

测试：

- 编译通过。
- 三个模式原功能不变。

### 第 2 步：右键按下高亮贴纸并画十字架

修改：

- 练习模式右键按下时调用 `pickStickerAtScreen(...)`。
- 保存贴纸四边形。
- 绘制十字架。

不处理右键松开公式。

测试：

- 练习模式右键按下可见贴纸，高亮和十字架出现。
- 右键松开后辅助显示消失。
- 自动模式右键无变化。
- 教学模式右键仍然弹出颜色面板。

### 第 3 步：显示拖拽轨迹

修改：

- 右键按住移动时更新当前鼠标坐标。
- 绘制拖拽轨迹。
- 距离不足和距离足够可以用不同颜色。

测试：

- 轨迹跟随鼠标。
- 松开后消失。
- 不影响左键视角旋转。

### 第 4 步：打印候选公式，不转魔方

修改：

- 实现 `inferPracticeDragMove(...)`。
- 右键松开后，在终端打印：

```text
practice drag: face=F row=0 col=2 axis=A sign=+ candidate=U
```

如果无法判断：

```text
practice drag: no outer move
```

测试：

- 逐个可见面测试拖拽方向。
- 记录哪些方向反了。
- 调整映射表和顺逆表。

### 第 5 步：抽出练习模式公共执行函数

修改：

- 从公式面板逻辑中抽出 `executePracticeMove(...)`。
- 公式面板继续调用它。

测试：

- 公式面板所有按钮仍正常。
- 练习模式随机后公式操作仍能计时和判定复原。

### 第 6 步：右键拖拽真正执行公式

修改：

- 右键松开推断出候选公式后，调用 `executePracticeMove(...)`。

测试：

- 右键拖拽可以真正旋转外层。
- 公式面板和右键拖拽混合操作后，逻辑和显示一致。
- 复原后能正常弹窗、停止计时、记录成绩。

## 模式隔离

### 自动模式

右键拖拽不处理。

条件：

```cpp
if (currentMode != PLAY_MODE_PRACTICE) return;
```

### 教学模式

教学模式继续使用：

```cpp
handleTeachModePickInput(viewState);
```

练习模式右键逻辑不能抢走教学模式的 `WM_RBUTTONDOWN`。

### 浮层面板

如果设置面板或排行榜可见，练习模式右键拖拽应直接忽略，避免穿透面板操作后面的魔方。

### 魔方执行中

如果 `!pRubikcube->isExecuteOver()` 或 `isSolving == true`，右键拖拽应忽略。

## 与公式面板并存

公式面板使用左键，右键拖拽使用右键，输入不冲突。

真正执行旋转时，两者都调用同一个 `executePracticeMove(...)`。这能保证：

- `Cube` 逻辑数组一致。
- `RubikCube` 动画一致。
- 计时启动逻辑一致。
- `isRestored`、`hasRand` 等状态一致。

## 不应修改的内容

不应修改：

- `front/back/up/down/right/left` 转动算法。
- `RubikCube::mapStickerToFace(...)` 的逻辑面映射。
- 教学模式右键填色流程。
- 自动模式算法还原流程。
- 魔方颜色映射。

新增功能只是一条练习模式输入入口。

## 测试清单

- 自动模式右键无反应。
- 教学模式右键仍然选贴纸并弹出颜色面板。
- 练习模式右键按下贴纸，高亮出现。
- 十字架方向与贴纸边线平行。
- 右键拖拽轨迹显示正常。
- 右键松开后辅助显示消失。
- 拖拽距离过短不生成候选公式。
- 中间行或中间列需要中层时，第一版输出 `no outer move`。
- 候选公式打印结果符合预期后，再接入真实旋转。
- 接入真实旋转后，公式面板和右键拖拽混合操作仍能正常复原。
- 复原后计时停止并记录成绩。

## 结论

这个方案比“点哪个面就转哪个面”更接近真实拖拽旋转，但也更容易出现方向判断错误。

因此必须分阶段实现：

1. 先做十字架和拖拽轨迹。
2. 再只打印候选公式。
3. 确认方向表后再真正转魔方。

这样即使方向判断不准，也不会破坏 `Cube` 逻辑状态，调试风险可控。

## 阶段复盘：原始贴纸身份方案的问题

当前已经验证：

- 复原状态下，按 `mapStickerToFace(...)` 得到的 `face/row/col` 可以推断出正确外层。
- 6 个起始面的顺逆方向表已经可以在复原状态下校准。
- 抽出 `executePracticeMove(...)` 后，公式面板和右键拖拽可以走同一条执行路径。

但实际接入旋转后出现新问题：

- 执行一次旋转或随机打乱后，右键点到的贴纸已经移动到新的空间位置。
- `mapStickerToFace(k, i, j, planeIndex, ...)` 返回的是这个贴纸原本属于哪个逻辑面，而不是它当前处于哪个空间面。
- 因此拖拽推断会像是在操作“贴纸原始身份”，不是操作“当前看到的这一层”。

示例：

```text
初始状态点 F 面右上角，拖拽得到 R'，这是正确的。
执行 R' 后，当前位置已经换成黄色贴纸。
继续点屏幕上同一位置时，mapStickerToFace 可能返回 U 面贴纸。
此时推断结果会偏向 U 面原始身份，而不是当前空间位置。
```

所以，后续方案必须从“原始贴纸身份”改成“当前几何空间位置”。

## 修订方案：基于当前几何空间推断

### 核心原则

右键拖拽应该判断：

- 被点击的小块当前处于魔方的哪一个空间面。
- 被点击的小块当前处于该空间面的哪一行、哪一列。
- 鼠标拖拽方向更接近当前空间面的哪条方向轴。

它不应该判断：

- 这个贴纸原来是什么颜色。
- 这个贴纸原来属于 `F/R/B/L/U/D` 哪个逻辑面。
- 这个小块在数组里原始的 `k/i/j` 位置。

公式面板执行的是当前空间层，右键拖拽也必须执行当前空间层。

### 需要新增的几何查询

建议在 `RubikCube` 内部新增一个只读查询函数，专门给练习模式拖拽使用。

文件：

- `core/rubikcube.h`
- `core/rubikcube.cpp`

建议结构：

```cpp
struct StickerDragGeometry {
    POINT screenPolygon[4];
    Point cubieCenter;
    Point stickerCenter;
    Point normal;
    Point rowAxis;
    Point colAxis;
    int currentFace;
    int currentRow;
    int currentCol;
};
```

建议接口：

```cpp
bool getStickerDragGeometry(
    int k,
    int i,
    int j,
    int planeIndex,
    StickerDragGeometry& out
) const;
```

职责：

- 返回贴纸当前屏幕四边形。
- 根据当前几何位置判断贴纸现在朝向哪个空间面。
- 根据小块中心当前坐标判断它处于该空间面的哪一行、哪一列。
- 返回当前空间面的行方向轴和列方向轴，用于拖拽方向比较。

这个函数仍然只读，不改变颜色、不改变高亮、不改变逻辑 `Cube`。

### 当前空间面如何判断

不能再用：

```cpp
planeIndex == 0 -> F
planeIndex == 3 -> U
```

应该用当前贴纸法线方向判断。

思路：

1. 计算当前贴纸法线 `normal`。
2. 取魔方当前 6 个空间方向轴。
3. 比较 `normal` 和 6 个方向轴的点积。
4. 点积最大的方向就是当前空间面。

例如：

```text
normal 最接近当前 F 方向 -> currentFace = F
normal 最接近当前 U 方向 -> currentFace = U
```

这样即使黄色贴纸转到了前面，也会被判断为当前空间的 `F` 面，而不是原始 `U` 面。

### 当前行列如何判断

不能再用原始数组下标直接判断：

```cpp
k/i/j -> row/col
```

应该用小块当前中心点相对魔方中心的位置判断。

思路：

1. 取当前小块中心 `cubieCenter`。
2. 取当前空间面的两个面内方向轴：`rowAxis` 和 `colAxis`。
3. 用点积判断小块中心在这两个方向上的投影。
4. 投影接近负方向、中间、正方向，分别映射到 `0/1/2`。

示例：

```text
投影在 rowAxis 负方向 -> row = 0
投影接近 0 -> row = 1
投影在 rowAxis 正方向 -> row = 2
```

第一版仍然只支持外层：

- `row == 1` 且拖拽需要按行选层 -> `no outer move`
- `col == 1` 且拖拽需要按列选层 -> `no outer move`

### 拖拽方向如何判断

原方案用贴纸四边形的 `axisA/axisB` 判断拖拽方向，复原状态下可用，但后续建议改为当前空间面的标准方向轴。

推荐：

1. 把 `rowAxis` 和 `colAxis` 投影到屏幕。
2. 得到 `screenRowAxis` 和 `screenColAxis`。
3. 比较拖拽向量和这两条屏幕轴的夹角。
4. 更接近 `screenRowAxis`，说明用户在沿行方向拖。
5. 更接近 `screenColAxis`，说明用户在沿列方向拖。

这样比依赖贴纸顶点顺序更稳定，也能减少 `planeIndex == 5` 这种特殊处理。

### 候选公式推断改造

原函数：

```cpp
inferPracticeDragMove(face, row, col, planeIndex, polygon, ...)
```

建议改为：

```cpp
inferPracticeDragMove(const StickerDragGeometry& geometry, ...)
```

内部使用：

- `geometry.currentFace`
- `geometry.currentRow`
- `geometry.currentCol`
- `geometry.rowAxis`
- `geometry.colAxis`

而不是使用 `mapStickerToFace(...)` 的结果。

输出日志也要改成更清晰：

```text
[PracticeDrag] currentFace=F row=0 col=2 axis=col sign=+ candidate=R'
```

如果需要保留调试信息，可以额外打印原始身份：

```text
rawFace=U rawPlane=3
```

但候选公式只能基于 `currentFace/currentRow/currentCol`。

## 修订后的分阶段计划

### 第 7 步：暂停真实旋转，恢复为只打印

目的：

- 避免当前错误推断继续修改逻辑 `Cube`。
- 先验证新的当前空间几何推断是否正确。

修改：

- `finishPracticeRightDrag(...)` 暂时只打印新的候选公式。
- 暂时不调用 `executePracticeMove(...)`。
- 公式面板继续保持可用。

测试：

- 公式面板原功能正常。
- 右键拖拽只打印，不改变魔方状态。

### 第 8 步：新增当前几何空间查询

修改：

- 在 `RubikCube` 中新增 `getStickerDragGeometry(...)`。
- 内部计算当前贴纸法线、当前空间面、当前行列、当前行列方向轴。
- 保留 `getStickerScreenPolygon(...)`，用于画辅助线。

测试：

- 复原状态下，当前空间面和行列应与之前结果一致。
- 执行一次 `R/R'/U/U'` 后，再点同一空间位置，应输出新的当前空间面，而不是贴纸原始面。
- 随机打乱后，当前空间面和行列仍然符合视觉直觉。

### 第 9 步：用当前空间几何重新推断候选公式

修改：

- `inferPracticeDragMove(...)` 改用 `StickerDragGeometry`。
- 顺逆表基于 `currentFace` 校准。
- 日志打印 `currentFace/currentRow/currentCol/candidate`。

测试：

- 复原状态下 6 个面的主要拖拽仍正确。
- 执行一次旋转后，继续点当前空间外层，候选公式仍符合视觉直觉。
- 随机打乱后，候选公式不受颜色和贴纸原始身份影响。

### 第 10 步：重新接入真实旋转

前提：

- 第 9 步候选公式稳定。
- 复原、旋转后、随机打乱后三类场景都能输出正确候选公式。

修改：

- 在 `result.ok == true` 时重新调用 `executePracticeMove(...)`。
- 继续让公式面板和右键拖拽共用 `executePracticeMove(...)`。

测试：

- 右键拖拽真正旋转当前空间外层。
- 公式面板和右键拖拽混合操作后，显示和逻辑数组一致。
- 随机打乱后，右键拖拽能正常辅助复原。
- 复原后 `isCubeSolved(Cube)` 能正确触发弹窗、停止计时、保存成绩。

## 修订后的风险点

### 1. 当前空间轴要和公式执行一致

`RubikCube` 的动画执行依赖当前空间轴映射。新的几何查询必须和动画执行使用同一套空间方向，否则会出现“打印是 R，但动画转的不是视觉右层”。

### 2. 左键转视角后的语义要确认

如果用户左键旋转视角，右键拖拽应该按当前视觉空间判断。测试时需要覆盖：

- 不转视角。
- 左键转过视角。
- 随机打乱后再转视角。

### 3. 教学模式不能受影响

教学模式仍然需要 `mapStickerToFace(...)`，因为它要把填色写回逻辑 `Cube`。新的当前空间几何查询只给练习模式右键拖拽使用，不应替换教学模式逻辑。

## 修订结论

原方案完成了交互原型、方向校准和公共执行函数，但它基于“原始贴纸身份”，只能在复原状态附近可靠。

下一阶段应改成“当前几何空间推断”：

1. 先暂停右键真实旋转。
2. 新增当前空间几何查询。
3. 只打印并验证候选公式。
4. 确认稳定后再重新接入 `executePracticeMove(...)`。

这比继续补方向表更可靠，也能保证右键拖拽在旋转后、打乱后、混合公式操作后仍然和用户视觉直觉一致。
