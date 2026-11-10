# VSCode 算法刷题环境 · 配置说明

交互式配置器：启动先看状态，再按需勾选要配置的内容，也可以一键全配。
**插件缺了自动装，装了的不重复下载，编译包用你本地的 zip 解压，全程不联网下编译器。**

---

## 一、快速使用

### 1. 准备文件夹

把三样东西放在任意一个文件夹里（桌面、U 盘、D 盘都行，不需要预先建目录）：

```
某文件夹\
├── start.bat                                            ← 启动器（推荐用这个）
├── setup-vscode-cp-env.ps1                              ← 主脚本
└── winlibs-x86_64-posix-seh-gcc-15.2.0-...zip           ← 编译包
```

压缩包文件名含 `winlibs` / `mingw` / `gcc` 任意一个即可被自动识别。

**工具链不会解压到这里**，而是统一装到纯英文路径（见下），脚本放哪、从哪双击，装出来的位置都一样。

#### 硬规则：工具链和工作区的路径都必须是纯英文

MinGW 的 `g++`/`ld` 处理不了非 ASCII 路径——它会把 `D:\再搬一次` 解析成 `D:/鍐嶆惉涓€娆?`，
然后报 `cannot find .../crt2.o`、`cannot find -lstdc++`，编译必然失败（936 和 65001 两种代码页都一样）。
cp/code-runner 这类插件在中文路径下也容易出问题，所以**两个路径都按英文选**。

脚本按这个顺序决定位置：

| 情况 | 工具链 | 工作区 |
|---|---|---|
| 脚本目录是纯英文 | `<脚本目录>` | `<脚本目录>\workspace`（便携） |
| 脚本目录含中文 | `<英文父目录>\CppEnv` | `<英文父目录>\CppWorkspace` |
| `-ToolRoot` / `-Root` 指定了中文 | **拒绝**，按上面两行重新选 | **拒绝**，按上面两行重新选 |

“英文父目录”的选取顺序：非系统固定盘根（如 `D:\`）→ `C:\ProgramData\CppRoot` →
`%LOCALAPPDATA%\CppRoot`（用户名本身是中文时会被剔除）→ `C:\CppRoot`。

脚本目录含中文时的提示：

```
[!!]   脚本目录含中文: D:\刷题环境
       MinGW 的 g++ 在中文路径下会找不到 crt2.o，无法编译
       工具链改放: D:\CppEnv

工作区: D:\CppWorkspace
```

所以**中文目录照样能用**，只是产物都落到英文位置，不会在中文目录里生成 `mingw64` 或 `workspace`。

### 2. 运行

**双击 `start.bat`**（不要双击 `.ps1`）。

`start.bat` 只做五件事，但它决定了换电脑后会不会"闪退"：

| 动作 | 解决什么 |
|---|---|
| `chcp 65001` | 让 logo 的 `░ █` 和中文正常显示 |
| 检查 UTF-8 BOM | 文件在传输中被转码时直接给出中文提示，而不是崩在语法错误上 |
| `Unblock-File` | 去掉"来自其他计算机"标记，否则 RemoteSigned 下会被拒绝 |
| `-ExecutionPolicy Bypass` | 绕过执行策略，新电脑默认是 `Restricted` |
| 末尾 `pause` | 无论成功失败，窗口都停住，报错看得见 |

也可以手动在终端里跑（调试用）：

```powershell
.\setup-vscode-cp-env.ps1 -Action status
```

启动画面是 Mr.L 像素字标，随后是状态面板和菜单。

```
  ============================================================

     ░░░░░░░░    ░░░░░░░░                   ░░░░░░░
    ░████████   ░████████                   ███████
    ░░████████ ░███████░                    ░████░
     ░████████ ░███████░                     ░███░
     ░████░███████░█████  ███████████░      ░█████
     ░████ ░░████░ ████░  ░░████░░████░     ░████░
     ░████  ░████  ████░   ░████  ░███░     ░████░
     ░████         █████   ░████            ░█████
     ░████         ████░   ░████             ░███░        ░
     ░████         █████   ░████             █████       ░█
    ░██████░      ███████ ███████      ░███ ███████████████
    ░██████░      ░██████ ░██████       ██░ ███████████████

                       L 君 · 雄心工作室
                        AMBITION STUDIO

                      算法刷题环境配置器

  ============================================================
```

`█` 是实心笔画，`░` 是半调阴影，来自工作室原图的采样还原。

### 3. 结果

```
D:\CppEnv\                        ← 统一的工具链安装位置
└── mingw64\bin\g++.exe           ← 解压出来的编译器（自动加入用户 PATH）

%USERPROFILE%\cp-workspace\       ← 默认工作区
├── .vscode\                      ← 全套配置
├── luogu\  leetcode\  nowcoder\  cph\
└── template.cpp
```

工作区想换位置用 `-Root "D:\刷题环境\workspace"`；工具链想换位置用 `-ToolRoot "D:\别处"`。

### 4. 检测到已有配置就跳过（默认行为）

**只要找到 `settings.json`，就什么都不动**——这是硬规则，优先级高于其他所有动作。

检测两处：

1. `<工作区>\.vscode\settings.json`
2. **脚本所在目录的** `.vscode\settings.json`（你自己原来配好的那套）

命中后：

```
  [OK]   检测到已有配置: E:\Projects0\algorithm-practice\.vscode\settings.json
         默认跳过，不安装、不解压、不写配置、不改 PATH
         确实想按本机重写，再显式加 -Overwrite
```

状态面板也会同步显示，`配置` 标记为 `[OK]` 并注明来源：

```
  配置     [OK] .vscode/settings.json
           已检测到现有配置，默认不改动（要重写加 -Overwrite）
           来源: E:\Projects0\algorithm-practice\.vscode\settings.json
```

判据故意做得**很保守**：只要文件存在就算“已配置”，连解析失败（写了注释）也算。
因为把你的可用配置写坏，比少装一次严重得多。

唯一能突破这条规则的是 `-Overwrite`，且它会先备份成 `.bak-时间戳`。

### 5. 已经解压过的怎么办

如果某个文件夹里已经有解压好的 `mingw64\bin\g++.exe`（比如上次解压到了本目录），
脚本会**把它搬到 `D:\CppEnv` 并直接用**，不会重新解压一遍：

```
  >> 解压编译包
  [OK]   安装目录: D:\CppEnv
  [!!]   检测到本目录已解压的编译器: D:\刷题环境\mingw64
         将迁移到: D:\CppEnv（同盘是秒级改名；跨盘需复制约 900 MB）
  [OK]   迁移完成: D:\CppEnv\mingw64\bin\g++.exe
```

两条保护：

- 已经在**用户 PATH 里**的编译器视为既有安装（例如 `D:\APP\gcc\winlibs`），**绝不搬动**，只记录路径
- 源目录如果就是脚本所在目录本身（摊平的 `bin\g++.exe` 结构），也不搬，就地使用

解压时会显示实时进度，不再像卡死：

```
  [!!]   正在解压 253.4 MB / 11976 个文件 —— 下面会实时显示进度，不要关窗口
    [████████░░░░░░░░░░░░░░]  36%   4310/11976 个    4s
    [██████████████████████] 100%  11576/11576 个   11s
  [OK]   解压完成（tar）  11576 个文件  11 秒
```

---

## 二、状态面板

每次启动和每次操作后都会重新检测，一眼看出缺什么：

```
==============================================================
  算法刷题环境 · 当前状态
==============================================================
  编辑器   [OK] VSCode 1.145.2
           D:\APP\Microsoft VS Code\bin\code.cmd
  编译器   [OK] GCC 15.2.0   (windows-gcc-x64)
           D:\APP\gcc\winlibs\mingw64\bin
  调试器   [OK] GDB 16.3
  Node     [OK] Node.js v22.22.2
  编译包   [OK] winlibs-....zip  (253.4 MB)
--------------------------------------------------------------
  插件
           [OK] 洛谷
           [OK] LeetCode
           [OK] 牛客
           [OK] CPH
           [OK] C/C++
           本机共 61 个扩展，缺 0 个
--------------------------------------------------------------
  工作区
  目录     [OK] D:\CppWorkspace
  配置     [OK] .vscode/settings.json
  模板     [OK] template.cpp
  标准     C++11 / C11（对齐蓝桥杯）
  中文名   [OK] 未发现中文/全角文件名
==============================================================
```

标记含义：`[OK]` 就绪　`[--]` 缺失　`[!!]` 配置与本机不符

配置行的 `[!!]` 尤其重要——换电脑后配置里记的还是旧路径，状态面板会直接标出来。
「中文名」一行会列出工作区里中文/全角文件名的数量与示例（只报告、不改名），
有中文名时提示 F5 断点调试需改用英文文件名，详见第九节。

---

## 三、交互菜单

### 主菜单

```
  [1] 一键配置全部      解压 + 插件 + 配置，懒人选择
  [2] 自定义配置        分项勾选要做的内容
  [3] 插件与参数        补装缺失插件；已装齐则改为写入平台参数
  [4] 只生成配置
  [5] 只解压编译包
  [6] 刷新状态
  [0] 退出
```

`[3]` 不是纯粹的"装插件"。逻辑是：缺什么装什么；**如果五个插件都在，就跳过下载，转而把四平台参数写进 `settings.json`**。
这样在已经装过插件的电脑上点 `[3]`，不会空跑一趟就退出。

### 自定义配置（选 [2]）

```
  自定义配置 —— 选择要做的项目（可多选，如 1,3,5）

    [1] 解压 MinGW 编译包       当前: 已安装，将跳过
    [2] VSCode 插件与参数       当前: 缺 0 个
    [3] C/C++ 编译配置          IntelliSense + 编译任务 + 调试
    [4] 四平台插件参数          洛谷 / LeetCode / 牛客 / CPH
    [5] 工作区目录与模板        目录骨架 + template.cpp

    [A] 全选（等同「一键配置」）
    [回车] 返回
```

**勾选是真正生效的，不是形式**。实测只勾 `[4]` 时：

- 只生成 `settings.json` + `extensions.json` 两个文件
- `settings.json` 里只有 26 个平台键，**没有任何 `C_Cpp.*` 键**
- 不创建 `luogu/` 等目录，不写 `tasks.json` / `launch.json` / `template.cpp`

各选项实际影响：

| 选项 | 产出 |
|---|---|
| [1] 解压编译包 | `mingw64\` + 用户 PATH |
| [2] 插件与参数 | 缺失的 5 个扩展；**若插件已装齐，转写 `luogu.*` / `leetcode.*` / `nowcoder.*` / `cph.*` + `extensions.json`** |
| [3] C/C++ 配置 | settings 的 `C_Cpp.*` / `[cpp]` / `[c]` / 终端 / Code Runner + `tasks.json` + `launch.json` + `c_cpp_properties.json` |
| [4] 平台参数 | settings 的 `luogu.*` / `leetcode.*` / `nowcoder.*` / `cph.*` + `extensions.json` |
| [5] 工作区 | 5 个目录 + `template.cpp` |

只勾 `[2]` 时会自动补写平台参数；如果同时勾了 `[3]` / `[4]` / `[5]`，参数交给后面的配置步骤统一写，
**同一轮里 `settings.json` 只会被写一次**——写两次的话第二次会因"已存在，保留"被跳过，
或者加 `-Overwrite` 时把第一次写的键覆盖掉。

---

## 四、免菜单模式

不想走菜单、或要写进自动化脚本时，用 `-Action` 直接执行：

```powershell
.\setup-vscode-cp-env.ps1 -Action status     # 只看状态
.\setup-vscode-cp-env.ps1 -Action unpack     # 只解压
.\setup-vscode-cp-env.ps1 -Action plugins    # 插件 + 平台参数（已装齐则只写参数）
.\setup-vscode-cp-env.ps1 -Action config     # 只生成配置
.\setup-vscode-cp-env.ps1 -Action full       # 完整配置
```

`-Action` 指定的操作不受菜单勾选限制（`config` 等同全选）。

`plugins` 与 `config` 的区别只在一个地方：`plugins` 会先确认扩展在不在，缺就装；
`config` 不碰扩展，只写配置文件。两者都会写平台参数，所以已经装好插件的电脑上
用 `-Action plugins` 补齐参数是安全的——它不会重装任何扩展。

---

## 五、参数

| 参数 | 缺省 | 说明 |
|---|---|---|
| `-Root` | `%USERPROFILE%\cp-workspace` | 工作区根目录 |
| `-CompilerBin` | 自动查找 | 已有 MinGW 的 `bin` 目录（也兼容直接填 `g++.exe` 完整路径） |
| `-CompilerZip` | 脚本同目录自动找 | 指定压缩包路径 |
| `-ToolRoot` | **`D:\CppEnv`**（无 D 盘则 `%LOCALAPPDATA%\cp-toolchain`） | 自定义工具链安装位置 |
| `-Action` | 空（进菜单） | 跳过菜单：full / plugins / config / unpack / status |
| `-WithExtras` | 关 | 额外装图标主题 / ErrorLens / Git Graph |
| `-SkipExtensions` | 关 | 不动插件 |
| `-Overwrite` | 关 | **唯一能改动已有配置的开关**；覆盖前自动 `.bak` 备份 |
| `-NoPathUpdate` | 关 | 解压后不写入用户 PATH |
| `-DryRun` | 关 | 演练，不解压、不装插件、不写文件 |

---

## 六、四个平台的插件与配置

配置键均取自各扩展 `package.json` 的真实定义。

| 平台 | 扩展 ID | 关键配置 | 易踩的坑 |
|---|---|---|---|
| 洛谷 | `yltx.vscode-luogu` | `luogu.cphPort = 27121` | 端口与 CPH 不一致则题目推不过去 |
| LeetCode | `leetcode.vscode-leetcode` | `leetcode.endpoint = leetcode-cn` | **硬依赖 Node.js**，缺了会静默失效 |
| 牛客 | `nowcoder.nowcoder` | `nowcoder.defaultLanguage = C++` | 语言枚举是 `C++` 大写，不是 `cpp` |
| CPH | `divyanshuagrawal...helper` | `cph.language.cpp.Args` | `Args` 管本地编译，`SubmissionCompiler` 只是提交标签 |

- **洛谷 ↔ CPH 桥接**：洛谷扩展把题目和样例直接推到 CPH 测试面板，靠 `luogu.cphPort`（默认 27121）。
- **Node.js 只影响 LeetCode**：没有 node 时洛谷 / 牛客 / CPH 照常可用。

---

## 七、编译任务

| 任务 | 参数 | 用途 |
|---|---|---|
| 构建并运行（考场模式） | `-std=c++11 -O0 -Wall` | **默认任务**，等价 Dev-C++ 5.11 |
| 调试构建 | `-std=c++11 -g -O0` | F5 断点，launch.json 依赖此任务 |
| OJ 极限压测 | `-std=c++11 -O2 -DNDEBUG` | 只看性能上限，**不是**考场环境 |
| C 构建并运行 | `-std=c11 -O0` | C 语言 |

四个任务的 `command` 都写成**编译器的绝对路径**（如 `D:\CppEnv\mingw64\bin\g++.exe`），
CPH 的 `cph.language.cpp.Command` 和 Code Runner 的 `executorMap` 同理。
四个任务的 `-o` 一律是 `a.exe`（纯 ASCII），`launch.json` 的 `program` 与之对应——
这样源文件名起中文也不会生成乱码 exe，详见第九节。

原因：只写 `g++` 会依赖用户 PATH 是否刷新过。实测在没有 g++ 的 PATH 下执行任务，
报错是 `'g++' 不是内部或外部命令`；改成绝对路径后同样条件下编译退出码 0、可正常运行。
换电脑或挪动工具链后，用 `-Action config -Overwrite` 重写一次即可。

---

## 八、路径处理与换电脑

### 核心原则：没有一处路径是写死的

脚本里不存在"必须装在 D:\APP"这类假设。所有位置都在运行时推导：

| 目标 | 查找顺序 |
|---|---|
| VSCode | PATH → 标准安装路径 → **注册表 Uninstall 键的 InstallLocation** |
| g++ | `-CompilerBin` 参数 → PATH → 压缩包解压目录 → msys64/mingw64/TDM-GCC 等常见位置 → winget 包目录 |
| 编译包 zip | `-CompilerZip` 参数 → 脚本所在目录 → 当前工作目录（文件名匹配 winlibs/mingw/gcc） |
| Node | PATH → Program Files → 用户目录 → winget 包目录 |
| `includePath` | 从 `g++ -dumpversion` **实时读出 GCC 版本**，反推 `<mingw根>\include\c++\<版本>` |

所以换一台电脑，只要 g++ 在 PATH 里、或把压缩包放在脚本旁边，路径就能自己对上。

### 只有 C 盘的电脑

完全没问题，三种情况都已覆盖：

```powershell
# 默认就是 C 盘用户目录，不用改任何参数
.\setup-vscode-cp-env.ps1
#   → 工具链落在 C:\Users\<用户名>\AppData\Local\cp-toolchain（D 盘不存在时自动降级）
#   → 工作区落在 C:\Users\<用户名>\cp-workspace

# 想指定 C 盘其他位置
.\setup-vscode-cp-env.ps1 -Root "C:\刷题\workspace" -ToolRoot "C:\CppEnv"

# 压缩包放桌面也行，脚本会把它装到 D:\CppEnv（或降级目录），不会在桌面留一堆文件
.\setup-vscode-cp-env.ps1 -CompilerZip "C:\Users\me\Desktop\winlibs-xxx.zip"
```

三种自动兜底：

1. **盘符不存在** → 直接报错并提示，不会瞎创建目录
   ```
   [FAIL] 目标盘符不存在: Z:\
          该电脑可能只有 C 盘，请改用 -Root 指向现有磁盘
          例如: -Root "C:\Users\lu\cp-workspace"
   ```
2. **解压目标不可写**（比如放在 Program Files）→ 自动回退到 `%LOCALAPPDATA%\cp-toolchain`
3. **路径过长**（接近 Windows 260 字符上限）→ 状态面板给出警告

相对路径也支持，会以当前目录为基准展开：

```powershell
.\setup-vscode-cp-env.ps1 -Root ".\my-workspace"    # 解析成当前目录下的绝对路径
```

### 换电脑后路径失效怎么发现

状态面板会主动比对配置里的路径与本机实际路径，不一致就打 `[!!]`：

```
  工作区
  目录     [OK] E:\Projects0\_t2
  配置     [!!] .vscode/settings.json
           路径已失效: LeetCode 目录 -> E:\Projects0\_t1\leetcode
           路径已失效: 牛客目录 -> E:\Projects0\_t1\nowcoder
           加 -Overwrite 重跑即可按本机路径重写
```

检测覆盖三项：编译器路径、LeetCode 目录、牛客目录。

修复只需一条命令，脚本会按本机实际路径重写全部配置（原文件自动备份为 `.bak-时间戳`）：

```powershell
.\setup-vscode-cp-env.ps1 -Overwrite
```

### 迁移对照表

| 项目 | 能否直接拷 | 说明 |
|---|---|---|
| `.vscode/` 配置 | 可以 | 绝对路径需 `-Overwrite` 重写，状态面板会提示 |
| 插件 | 不能 | 需重装，脚本自动处理 |
| 各平台登录态 | 不能 | 洛谷 / LeetCode / 牛客 均需重新登录 |
| CPH 测试数据 | 可以 | 在 `cph/` 目录，随工作区拷贝 |
| 编译包 | 可以 | 带上 zip，新机器跑脚本自动解压 |
| 整个 setup 文件夹 | 可以 | 脚本用 `$PSScriptRoot` 定位压缩包，换位置无需改代码 |
| `start.bat` | 必须带上 | 新电脑靠它绕过执行策略；漏了它就容易"闪退" |

### 换电脑后"闪退"的三种原因

这是踩过的坑，全部实测过。**先看现象对号，再按表格处理。**

| 现象 | 原因 | 实测证据 | 处理 |
|---|---|---|---|
| 黑一下就没，连 logo 都没有 | 执行策略 `Restricted` | 直接 `-File` 运行：退出码 1，只有一行"在此系统上禁止运行脚本" | 改用 `start.bat`（内含 `-ExecutionPolicy Bypass`） |
| 同样黑一下就没 | 文件带"来自其他计算机"标记 | 下载/聊天软件传来的 `.ps1` 会被 RemoteSigned 拦下 | `start.bat` 已内置 `Unblock-File`；或手动 `Unblock-File .\*.ps1` |
| 报"表达式或语句中包含意外的标记 `}`" | UTF-8 BOM 丢失，中文注释被按 GBK 解析 | 剥离 BOM 后实测：第 121/131/140 行连环语法错误，退出码 1 | `start.bat` 会先查 BOM 并给中文提示；从原始压缩包重新解压 |
| 中文全是 `?`、`░` 显示不出来 | 文件被转成 GBK，或控制台代码页不是 UTF-8 | 转 GBK 后实测：能跑但中文与块字符全部变 `?` | 用 `start.bat` 启动（`chcp 65001`） |
| 看到 logo 后报错退出 | 探测阶段抛异常，配合 `$ErrorActionPreference='Stop'` 变致命 | 32 位系统 `Join-Path $null` 抛异常；stdin 不可用时 `$null.Trim()` 抛异常 | 已修：参数判空 + 探测段降为 `Continue` + 全局 `trap` 兜底 |
| 正常跑完窗口也关 | `.ps1` 双击运行，脚本结束即关窗，`exit 0` 也一样 | — | 这不是崩溃；用 `start.bat` 会 `pause` |

**排查口诀**：先在终端里跑 `.\setup-vscode-cp-env.ps1 -Action status`。
能看到状态面板 → 只是双击关窗的问题；报错留在屏幕上 → 按报错对号。

### 已内置的四层防护

脚本自身不再假设"运行环境和开发机一样"：

1. **`trap` 全局兜底** —— 任何未捕获的异常都会打印消息、行号和三条排查建议，然后停住，不再静默退出
2. **版本检查替代 `#Requires`** —— 低于 PowerShell 5.1 时给中文提示并暂停，而不是硬拒绝
3. **探测不再致命** —— `Get-EnvState` 内部把错误等级降为 `Continue`，本机有、别机没有，都只算"未找到"
4. **空值防护** —— `Join-Path` 统一走 `JP` 包装（32 位系统没有 `ProgramFiles(x86)`）；`Read-Host` 返回 `$null` 时按退出处理

写入配置也改为逐个文件 try/catch：某个文件被 VSCode 占用或目录只读，只跳过该文件，不会中断整轮。

### 蓝桥杯对齐约束（重要）

考场是 **Dev-C++ 5.11 + TDM-GCC**，标准 **C++11**、默认**不开优化**。因此：

1. 编译参数固定 `-std=c++11`，**不要升级到 C++14/17**，否则本地过、考场 CE。
2. 默认任务用 `-O0` 而非 `-O2`，避免"本地 1 秒、考场 5 秒"的假象。
3. 即便同为 C++11，本地 GCC 15.2 与考场 GCC 4.9.2 仍不等价，以下在考场可能不可用：
   - `std::thread` / `std::mutex`（TDM-GCC 为 win32 线程模型）
   - `std::regex`（4.9 实现有缺陷）
   - `__int128`（32 位 TDM-GCC 不可用）
4. 位宽差异：考场多为 32 位，`size_t` 与指针为 4 字节；本地 64 位为 8 字节。

---

## 九、中文文件名与乱码（专项）

**结论先行**：目录必须纯英文（见第八节）；**文件名可以中文，但绝不能拿中文当产物名，中文名也不能 F5 单步调试**。
脚本已把所有编译入口的产物名统一成 ASCII，并在状态面板里扫描提醒。

### 实测结论（GCC 15.2.0 / GDB 16.3 / Windows ACP=936）

| 场景 | 结果 | 处理 |
|---|---|---|
| 中文源文件 `两数之和.cpp` 参与编译 | 正常，`cmd` 936 / 65001 下都能打开 | 不用管 |
| `-o 两数之和.exe`（按 basename 命名产物） | **实际生成 `Á½ÊýÖ®ºÍ.exe`**，运行命令和 `launch.json` 全部对不上 | 产物固定 `a.exe` |
| 有语法错误时诊断里的中文文件名 | 正常 UTF-8，`$gcc` problemMatcher 能跳转到文件 | 不用管 |
| 中文目录名下的 `-o` | `ld: cannot open output file ...: No such file or directory` | 纯英文路径规则已挡住 |
| 中文名源文件 + F5 断点 | `gdb: charset conversion failure` / `No source file named ...` | 改成 ASCII 文件名 |
| 程序里 `printf("中文")` | 正常（`-fexec-charset=GBK` + 936 终端） | 别和上面几类混淆 |

`ld` 会把 `-o` 的中文参数按字节写成 Latin-1 风格的文件名，这是 MinGW 的硬限制。
`chcp 936`、`chcp 65001`、`set host-charset` 都改不了，均已实测。

### 脚本已经做的四件事

1. `tasks.json` 四个任务一律 `-o a.exe`；`launch.json` 的 `program` 同步为 `${fileDirname}\a.exe`。
2. Code Runner `executorMap`：输入用 `"$fileName"` 加引号（兼容中文与空格），输出 `a.exe`，运行 `.\a.exe`；
   顺带改用 `cd /d "$dir"` 解决跨盘符 `cd` 不生效。优化级别维持原样 `-O2`，不在此变更。
3. 洛谷 `luogu.cphStyle` 从 `ProblemIDwithProblemName` 改回插件默认的 `ProblemID`——
   从源头不再生成「P1001 两数之和.cpp」这种中文带空格的文件名。
   CPH 自己会把输出名 ASCII 化成 `_u4e24…bin`，这一环本来就是安全的。
4. 状态面板新增「中文名」一行：扫描工作区（最多两层、400 个文件），列出中文/全角文件名数量与示例，
   **只报告，不改名、不动用户文件**。

### 唯一没法自动修的：F5 断点调试

GDB 在中文文件名上会报 `charset conversion failure`，按文件名下断点失败；
按行号虽然能设上断点，但调试信息里的源文件名是乱码，VSCode 无法把断点映射回编辑器。
实测 `set source-charset`、`set host-charset`、`set ada source-charset` 全部无效。

> **约定**：只编译运行 → 中文题名随便起；需要 F5 单步调试 → 另存为 `p1001.cpp` 这类纯 ASCII 名再调试。
> 同一目录多个题共用 `a.exe`，每次构建覆盖即可，不影响使用。

### 两种"乱码"别搞混

| 现象 | 根因 | 归属参数 |
|---|---|---|
| exe 名字乱码、找不到可执行文件 | `-o` 用了中文 basename | `tasks.json` / `executorMap` / `launch.json`（已修） |
| 程序输出中文变 `涓枃` / `???` | 源码 UTF-8 被当 GBK，或终端代码页不匹配 | `-finput-charset=UTF-8 -fexec-charset=GBK` + 终端 936 |

在 VSCode 集成终端里手动 `chcp 65001` 会让 **GBK 输出**变乱码，编译和运行不受影响；
`start.bat` 的 `chcp 65001` 只作用于脚本自己，不会传给 VSCode 终端。

---

## 十、常见问题

**Q：状态面板显示工作区 `[--]`，但目录其实存在？**
A：检查路径是否一致。菜单顶部会打印当前使用的 `-Root` 值。

**Q：换了电脑，配置里的路径还是旧的？**
A：状态面板会标 `[!!]`。加 `-Overwrite` 重跑，按本机实际路径重新生成（原文件备份为 `.bak-时间戳`）。

**Q：插件装到一半失败？**
A：重跑即可，已装的跳过，只补缺失的。

**Q：提示"无法加载，因为在此系统上禁止运行脚本"？**
A：**改用 `start.bat`**，它用 `-ExecutionPolicy Bypass` 启动，不改你电脑的全局策略。
如果坚持手动运行，才需要 `Set-ExecutionPolicy -Scope CurrentUser RemoteSigned`（这会改动本机安全设置）。

**Q：双击 `start.bat` 后窗口还是闪一下就没？**
A：三种可能：① 脚本或 bat 被转码（bat 会自己查 BOM 并提示）；② 杀毒软件拦截了 PowerShell，
把它加入白名单；③ 路径里有 `&`、`^` 等 cmd 特殊字符，换一个纯英文路径试试。
如果连 `start.bat` 的抬头都没打印出来，那就是 bat 本身没跑到，检查文件名后缀是否被改成了 `.bat.txt`。

**Q：为什么不能双击 `.ps1`？**
A：`.ps1` 双击默认用记事本打开；即使关联成"用 PowerShell 运行"，脚本结束时窗口也会立即关闭，
正常跑完和报错退出看起来一模一样。`start.bat` 的 `pause` 就是为了让信息留在屏幕上。

**Q：解压完了却说"尚未安装编译器"？**
A：旧版本会有这个问题：解压到本目录后，`Find-Gxx` 只查 PATH 和几个固定路径，不查脚本所在目录，
所以本目录里明明有 `mingw64\bin\g++.exe` 也认不出来。现在已修：

- `Find-Gxx` 会扫描 `D:\CppEnv`、`%LOCALAPPDATA%\cp-toolchain`、脚本目录、当前目录
- 解压/迁移成功后把路径缓存到内存，本轮立即生效，不必等 PATH 刷新
- 若仍认不到，状态面板会打印"解压目录: ..."和期望结构 `<安装目录>\mingw64\bin\g++.exe`

**Q：编译报 `cannot find .../crt2.o`、`cannot find -lstdc++`？**
A：**g++ 所在路径含中文**（或全角字符、emoji）。这不是配置问题，是 MinGW 的硬限制：
它把自身路径按 ANSI 解析，中文直接变乱码。把工具链移到纯英文路径即可——
新版脚本已自动规避（见“硬规则：工具链路径必须是纯英文”），状态面板也会对中文路径标黄提示。

**Q：源文件名用了中文，编译/运行的 exe 名字变成乱码，或者根本找不到 exe？**
A：这是 `-o` 目标用了中文 basename 导致的（MinGW `ld` 的硬限制）。脚本已把 `tasks.json`、
Code Runner、`launch.json` 的产物统一成 `a.exe`，并让洛谷按纯题号生成文件名。
中文名源文件**编译运行没问题**，但 **F5 断点调试仍不支持**，需要调试请改成英文文件名。
完整实测表见第九节。

**Q：解压卡住好几分钟，看不到进展？**
A：已改为三级解压 + 实时进度条。实测同一个 253 MB / 1.2 万文件包：
`tar` 11 秒（首选）、`.NET ZipFile` 17 秒、`Expand-Archive` 数分钟（仅兜底）。
如果超过 2 分钟，界面会提示可能是杀毒软件在逐个扫描，可把安装目录加入白名单。

**Q：想全部重来一遍？**
A：删掉工作区目录 `.vscode/`，然后选 `[1]`；或加 `-Overwrite`。
