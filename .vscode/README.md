# VS Code 配置说明 - 算法竞赛仓库

本仓库的 VS Code 配置（`.vscode/` 目录）已纳入 Git 版本控制，克隆仓库后即可获得完整一致的开发环境。

## 已配置的扩展

| 扩展 | 功能 | 快捷键 |
| ------ | ------ | -------- |
| **Competitive Programming Helper (CPH)** | 测试用例管理、自动提交 | `Ctrl+Alt+B` 运行测试<br>`Ctrl+Alt+S` 提交<br>`Ctrl+Alt+D` 聚焦面板 |
| **C/C++** | IntelliSense、调试、代码浏览 | - |
| **Code Runner** | 一键编译运行 | `Ctrl+Alt+N` |
| **Git Graph** | Git 提交历史可视化 | - |
| **Material Icon Theme** | 文件图标主题 | - |

> 推荐扩展列表见 `.vscode/extensions.json`，打开工作区时 VS Code 会自动提示安装。

## 配置概览

| 文件 | 用途 |
| ------ | ------ |
| `.vscode/settings.json` | 工作区设置：C/C++、Java、终端、文件关联、编码、CPH、洛谷 |
| `.vscode/tasks.json` | 构建任务：g++/gcc（Debug 与 Release O2）、javac |
| `.vscode/launch.json` | 调试配置：C++ (GDB)、Java: 当前文件 |
| `.vscode/c_cpp_properties.json` | C/C++ IntelliSense 配置 |
| `.vscode/extensions.json` | 推荐安装的扩展 |

## 编译与调试

### 编译任务（Ctrl+Shift+B）

| 任务 | 命令 | 标准 | 优化 | 用途 |
| ------ | ------ | ------ | ------ | ------ |
| C/C++: g++.exe 生成活动文件（默认） | `g++` | C++11 | `-O0 -g` | Debug 编译 |
| C/C++: gcc.exe 生成活动文件 | `gcc` | C11 | `-O0 -g` | C 语言 Debug |
| C++ Release (O2) | `g++` | C++11 | `-O2 -DNDEBUG` | 模拟 OJ 环境 |
| C Release (O2) | `gcc` | C11 | `-O2 -DNDEBUG` | C 语言 Release |
| Java: javac 编译活动文件 | `javac` | - | UTF-8 | Java 编译 |

- 编译器通过 **PATH 环境变量**解析（`g++`/`gcc`/`javac`/`gdb`），不再依赖绝对路径
- 统一携带 `-finput-charset=UTF-8 -fexec-charset=GBK`：UTF-8 源码编译为 GBK 可执行文件（Windows 中文终端兼容）

### 调试（F5）

| 配置 | 类型 | 说明 |
| ------ | ------ | ------ |
| C++ (GDB) | `cppdbg` | 调试当前 `.cpp` 文件，自动先编译 |
| Java: 当前文件 | `java` | 调试当前 `.java` 文件，集成终端 |

## CPH 插件配置

### 编译配置

```json
{
    "cph.language.cpp.Args": "-std=c++11 -O2 -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=GBK",
    "cph.language.cpp.Command": "g++",
    "cph.language.cpp.OutputArg": "-o",
    "cph.language.cpp.SubmissionCompiler": "GNU G++23 14.2 (64 bit, msys2)"
}
```

### 通用配置

```json
{
    "cph.general.timeOut": 3000,           // 测试超时 3 秒
    "cph.general.defaultLanguage": "cpp",  // 默认语言 C++
    "cph.general.autoShowJudge": true,     // 自动显示评测面板
    "cph.general.firstTime": false         // 已完成首次设置
}
```

## 使用方法

### 1. 测试题目

1. 打开 `.cpp` 文件
2. 按 `Ctrl+Alt+B` 运行测试用例
3. CPH 面板自动显示测试结果

### 2. 下载题目

1. 复制题目 URL（如洛谷、Codeforces）
2. 按 `Ctrl+Alt+P`（或在命令面板搜索 "CPH: Fetch Problem"）
3. CPH 自动下载题目并创建测试用例文件

### 3. 提交代码

1. 确保测试通过
2. 按 `Ctrl+Alt+S` 提交到 CodeForces
3. 或手动提交到其他 OJ

### 4. 查看测试用例

测试用例存储在 `.cph` 目录（已 git 忽略），格式为 `.prob` 文件，包含题目信息与输入/输出用例。

## 环境依赖

| 工具 | 说明 |
| ------ | ------ |
| `g++` / `gcc` / `gdb` | GCC 工具链（MinGW-w64），需加入系统 PATH |
| `javac` / `java` | JDK，配置依赖环境变量 `JAVA_HOME` |
| `bash` | Git Bash（可选终端），需加入 PATH |

> 若工具不在 PATH 中，可设置对应环境变量并在 `settings.json` 中引用（如 `C_Cpp.default.compilerPath: "${env:CPP_COMPILER_PATH}"`）。

## 注意事项

1. **编码设置**：源文件 UTF-8，编译输出 GBK（Windows 中文环境）
2. **版本控制**：`.vscode/` 已纳入 Git 版本控制，配置随仓库一起管理
3. **编译标准**：C++11 / C11，兼容主流 OJ
4. **优化级别**：CPH 与 Release 任务使用 `-O2`，模拟 OJ 评测环境
5. **测试超时**：CPH 超时 3 秒，留有余量避免误判 TLE

## 常见问题

### Q: 如何修改编译参数？

A: 编辑 `.vscode/settings.json` 中的 `cph.language.cpp.Args`，或 `.vscode/tasks.json` 对应任务的 `args`。

### Q: 如何切换提交编译器？

A: 修改 `settings.json` 中 `cph.language.cpp.SubmissionCompiler`，可选值见 CPH 扩展文档。

### Q: 换电脑后配置不生效？

A: 确认 `g++`/`gcc`/`gdb`/`javac` 已加入 PATH、`JAVA_HOME` 已设置，然后重新加载窗口（`Ctrl+Shift+P` → "Reload Window"）。

### Q: 测试用例文件在哪里？

A: 在 `.cph` 目录中，文件名格式为 `.{文件名}.{hash}.prob`。

## 相关链接

- [CPH 扩展文档](https://github.com/agrawal-d/competitive-programming-helper)
- [洛谷](https://www.luogu.com.cn/)
- [Codeforces](https://codeforces.com/)
- [AtCoder](https://atcoder.jp/)
