<#
.SYNOPSIS
    VSCode 算法刷题环境配置器（洛谷 / LeetCode / 牛客 / CPH + MinGW C++11）

.DESCRIPTION
    交互式环境配置工具。启动后先显示本机当前状态，再通过菜单选择要配置的内容。

    工作方式
      1. 状态面板   实时展示 VSCode / 编译器 / 调试器 / Node / 插件 / 工作区的就绪情况
      2. 交互菜单   一键全配，或按项目自由勾选（编译包 / 插件 / C++ 配置 / 平台参数 / 工作区）
      3. 编译包     从脚本同目录的 winlibs-*.zip 解压安装，不联网下载
      4. 幂等       已装的插件、已解压的编译器、已生成的配置一律跳过
      5. 零污染     只生成独立工作区，不读写全局 settings.json

    压缩包放置
      把 winlibs-*.zip（或 mingw*.zip）与本脚本放在同一目录。

    便携式布局
      默认把 mingw64\ 和 workspace\ 都生成在脚本所在目录里，
      整个文件夹可以拷到任何电脑（U 盘、D 盘、桌面都行），也可以整个删掉。
      不写死任何盘符；只有 VSCode 扩展是机器级的，换电脑会自动补装。

.PARAMETER Root
    工作区根目录。默认 <脚本所在目录>\workspace

.PARAMETER CompilerBin
    已有 MinGW 的 bin 目录（含 g++.exe），也兼容直接填 g++.exe 的完整路径。留空则自动查找。

.PARAMETER CompilerZip
    编译包压缩包路径。留空则在脚本同目录自动查找。

.PARAMETER ToolRoot
    解压目标目录。留空则解压到压缩包所在目录。

.PARAMETER Action
    跳过菜单直接执行。可选 full / plugins / config / unpack / status。
    留空则进入交互菜单。

.PARAMETER WithExtras
    额外安装体验类扩展（图标主题 / ErrorLens / Git Graph）

.PARAMETER SkipExtensions
    跳过插件安装

.PARAMETER Overwrite
    覆盖已存在的配置文件（默认保留）。覆盖前自动生成 .bak 备份。

.PARAMETER NoPathUpdate
    解压后不把编译器目录写入用户 PATH（默认会写入，方便新终端直接调用 g++）。

.PARAMETER DryRun
    演练模式：只打印动作，不解压、不装插件、不写文件

.EXAMPLE
    .\setup-vscode-cp-env.ps1
    进入交互菜单（推荐）。

.EXAMPLE
    .\setup-vscode-cp-env.ps1 -Action status
    只显示当前状态，不做任何修改。

.EXAMPLE
    .\setup-vscode-cp-env.ps1 -Action full -Root "D:\刷题环境\workspace"
    免菜单，直接完整配置。

.NOTES
    目标平台对齐：蓝桥杯考场为 Dev-C++ 5.11 + TDM-GCC，标准 C++11、默认不开优化。
    本脚本生成的 C++ 配置有意锁定 C++11，请勿擅自升级。
#>
[CmdletBinding()]
param(
    [string]$Root = '',
    [string]$CompilerBin = '',
    [string]$CompilerZip = '',
    [string]$ToolRoot = '',
    [ValidateSet('', 'full', 'plugins', 'config', 'unpack', 'status')]
    [string]$Action = '',
    [switch]$WithExtras,
    [switch]$SkipExtensions,
    [switch]$Overwrite,
    [switch]$NoPathUpdate,
    [switch]$DryRun
)

$ErrorActionPreference = 'Stop'

# ============================================================
#  迁移兜底：任何异常都必须留在屏幕上
#  换电脑后常见的三类"闪退"（执行策略 / 编码损坏 / 探测异常）
#  都在这一层被拦住，窗口不会一闪而过。
# ============================================================

# 安全的 Join-Path：入参为空时返回 $null，而不是抛异常
# （32 位系统没有 ProgramFiles(x86)，空变量会让 Join-Path 直接终止脚本）
function JP {
    param([string]$Base, [string]$Child)
    if (-not $Base) { return $null }
    try { return (Join-Path $Base $Child) } catch { return $null }
}

function Wait-Key {
    # 由 start.bat 拉起时，bat 自己会 pause，这里就不必再停一次
    if ($env:CP_NO_PAUSE) { return }
    Write-Host ""
    Write-Host "  按回车键退出..." -ForegroundColor DarkGray
    try { [void](Read-Host) } catch { }
}

# 接管所有未捕获的终止性错误，避免控制台瞬间关闭
trap {
    $msg = $_.Exception.Message
    $where = ''
    try {
        if ($_.InvocationInfo.ScriptLineNumber) { $where = "第 $($_.InvocationInfo.ScriptLineNumber) 行" }
    } catch { }

    Write-Host ""
    Write-Host "  [FAIL] 运行中断" -ForegroundColor Red
    Write-Host ("         $msg") -ForegroundColor Yellow
    if ($where) { Write-Host ("         位置: $where") -ForegroundColor DarkGray }
    Write-Host ""
    Write-Host "  排查顺序：" -ForegroundColor DarkGray
    Write-Host "    1. 改用同目录的 start.bat 运行（绕过执行策略，并把报错留在窗口里）" -ForegroundColor DarkGray
    Write-Host "    2. 确认脚本与 winlibs-*.zip 是一起解压出来的，没有被聊天软件转码" -ForegroundColor DarkGray
    Write-Host "    3. 加 -Action status 只看状态，不改任何文件" -ForegroundColor DarkGray
    Wait-Key
    exit 1
}

# 版本检查：替代 #Requires（后者会直接拒绝执行，且不给中文提示、不给暂停机会）
$__v = $PSVersionTable.PSVersion
if (($__v.Major -lt 5) -or (($__v.Major -eq 5) -and ($__v.Minor -lt 1))) {
    Write-Host ""
    Write-Host "  [FAIL] 本脚本需要 PowerShell 5.1 或更高版本" -ForegroundColor Red
    Write-Host ("         当前版本: $__v") -ForegroundColor Yellow
    Write-Host "         该电脑可能是 Windows 7 / 8，自带版本过低" -ForegroundColor DarkGray
    Write-Host "         升级方式: 安装 Windows Management Framework 5.1 或 PowerShell 7" -ForegroundColor DarkGray
    Wait-Key
    exit 1
}

$script:LINE = '=' * 62
$script:GxxPath = ''    # 本轮已确认的 g++ 路径（解压/迁移后立刻生效，不必等 PATH 刷新）
$script:PortableRoot = ''   # 脚本所在目录
$script:ResolvedToolRoot = ''   # 工具链根（一定是纯英文）
$script:AsciiParent = ''        # 纯英文的可写父目录
$script:AsciiWarned = $false
$script:THIN = '-' * 62

# ============================================================
#  输出辅助
# ============================================================
function Stage  ($m) { Write-Host "`n  >> $m" -ForegroundColor Cyan }
function Ok     ($m) { Write-Host "  [OK]   $m" -ForegroundColor Green }
function SkipIt ($m) { Write-Host "  [--]   $m" -ForegroundColor DarkGray }
function Caution($m) { Write-Host "  [!!]   $m" -ForegroundColor Yellow }
function Hint   ($m) { Write-Host "         $m" -ForegroundColor DarkGray }
function Mark($ok)   { if ($ok) { '[OK]' } else { '[--]' } }

# 已有可用配置且没加 -Overwrite → 任何动作都不许改动它
# 判据：workspace 里有 settings.json，且里面记录的编译器真实存在（或压根没记编译器）
function Test-ConfigLocked {
    param($st)
    if ($Overwrite) { return $false }
    if (-not $st) { return $false }
    return [bool]$st.ConfigUsable
}

# ============================================================
#  启动画面
# ============================================================
function Show-Logo {
    $bar = '  ' + ('=' * 60)
    Write-Host ""
    Write-Host $bar -ForegroundColor DarkCyan
    Write-Host ""
    Write-Host "     ░░░░░░░░    ░░░░░░░░                   ░░░░░░░" -ForegroundColor Cyan
    Write-Host "    ░████████   ░████████                   ███████" -ForegroundColor Cyan
    Write-Host "    ░░████████ ░███████░                    ░████░" -ForegroundColor Cyan
    Write-Host "     ░████████ ░███████░                     ░███░" -ForegroundColor Cyan
    Write-Host "     ░████░███████░█████  ███████████░      ░█████" -ForegroundColor Cyan
    Write-Host "     ░████ ░░████░ ████░  ░░████░░████░     ░████░" -ForegroundColor Cyan
    Write-Host "     ░████  ░████  ████░   ░████  ░███░     ░████░" -ForegroundColor Cyan
    Write-Host "     ░████         █████   ░████            ░█████" -ForegroundColor Cyan
    Write-Host "     ░████         ████░   ░████             ░███░        ░" -ForegroundColor Cyan
    Write-Host "     ░████         █████   ░████             █████       ░█" -ForegroundColor Cyan
    Write-Host "    ░██████░      ███████ ███████      ░███ ███████████████" -ForegroundColor Cyan
    Write-Host "    ░██████░      ░██████ ░██████       ██░ ███████████████" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "                       L 君 · 雄心工作室" -ForegroundColor White
    Write-Host "                        AMBITION STUDIO" -ForegroundColor DarkGray
    Write-Host ""
    Write-Host "                      算法刷题环境配置器" -ForegroundColor DarkGray
    Write-Host ""
    Write-Host $bar -ForegroundColor DarkCyan
    Write-Host ""
}

# ============================================================
#  路径工具 —— 应对"不同电脑安装位置不同"
# ============================================================

# 探测目录是否真的可写（有些目录存在但需要管理员权限）
# 连续失败 3 次才判负：刚改名/刚解压完的目录偶尔会被占用一瞬间，
# 只试一次会把"暂不可写"误判成"不可写"，进而把整套环境挪到别的盘
function Test-DirWritable {
    param([string]$Dir)
    if (-not $Dir) { return $false }
    for ($i = 0; $i -lt 3; $i++) {
        try {
            if (-not (Test-Path $Dir)) { New-Item -ItemType Directory -Force -Path $Dir -ErrorAction Stop | Out-Null }
            $probe = Join-Path $Dir ('.w-' + [guid]::NewGuid().ToString('N').Substring(0, 8))
            [System.IO.File]::WriteAllText($probe, 'x')
            [System.IO.File]::Delete($probe)
            return $true
        } catch {
            $script:LastWriteError = $_.Exception.Message
            if ($i -lt 2) { Start-Sleep -Milliseconds 200 }
        }
    }
    return $false
}

# 把用户输入的路径变成绝对路径；相对路径基于当前目录
function Resolve-AbsPath {
    param([string]$P)
    if (-not $P) { return '' }
    $expanded = [Environment]::ExpandEnvironmentVariables($P)
    if (-not [System.IO.Path]::IsPathRooted($expanded)) {
        $cwd = ''
        try { $cwd = (Get-Location).Path } catch { }
        if (-not $cwd) { $cwd = $env:USERPROFILE }
        if (-not $cwd) { $cwd = $env:SystemDrive + '\' }
        $expanded = Join-Path $cwd $expanded
    }
    try { return [System.IO.Path]::GetFullPath($expanded) } catch { return $expanded }
}

# 判断路径所在盘符是否存在（只有 C 盘的机器上，D:\ 会返回 false）
function Test-PathRoot {
    param([string]$P)
    try {
        $root = [System.IO.Path]::GetPathRoot($P)
        if (-not $root) { return $true }
        return (Test-Path $root)
    } catch { return $false }
}

# ============================================================
#  环境探测
# ============================================================

function Find-CodeCli {
    $cmd = Get-Command code -ErrorAction SilentlyContinue
    if ($cmd -and $cmd.Source) { return $cmd.Source }
    foreach ($p in @(
        (JP $env:LOCALAPPDATA 'Programs\Microsoft VS Code\bin\code.cmd'),
        (JP $env:ProgramFiles 'Microsoft VS Code\bin\code.cmd'),
        (JP ${env:ProgramFiles(x86)} 'Microsoft VS Code\bin\code.cmd'))) {
        if ($p -and (Test-Path $p)) { return $p }
    }
    foreach ($hive in @(
        'HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\*',
        'HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\*',
        'HKCU:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\*')) {
        try {
            $e = Get-ItemProperty $hive -ErrorAction SilentlyContinue |
                 Where-Object { $_.DisplayName -match 'Visual Studio Code' -and $_.InstallLocation } |
                 Select-Object -First 1
            if ($e) {
                $c = Join-Path $e.InstallLocation 'bin\code.cmd'
                if (Test-Path $c) { return $c }
            }
        } catch { }
    }
    return $null
}

function Find-Gxx {
    param([string]$HintDir)

    # 本轮已经确认过的路径优先（解压/迁移后立刻生效，不再依赖 PATH 是否刷新）
    if ($script:GxxPath -and (Test-Path $script:GxxPath)) { return $script:GxxPath }

    if ($HintDir) {
        # 文档要求传 bin 目录；顺手兼容有人直接把 g++.exe 的完整路径填了进来
        if ($HintDir -match '(?i)g\+\+\.exe$' -and (Test-Path -LiteralPath $HintDir)) { return $HintDir }
        $hintGxx = Join-Path $HintDir 'g++.exe'
        if (Test-Path -LiteralPath $hintGxx) { return $hintGxx }
    }

    # 便携优先：本目录 / -ToolRoot 里的编译器压过系统 PATH。
    # 否则机器上本来就装了 MinGW 时，配置会被写成系统路径，"整个文件夹搬走就能用"就不成立了。
    $local = @()
    if ($ToolRoot) {
        $local += (JP $ToolRoot 'mingw64\bin\g++.exe')
        $local += (JP $ToolRoot 'bin\g++.exe')
    }
    foreach ($r in @($PSScriptRoot, (Get-PortableRoot))) {
        if (-not $r) { continue }
        $local += (JP $r 'mingw64\bin\g++.exe')
        $local += (JP $r 'bin\g++.exe')
    }
    try {
        $cwd = (Get-Location).Path
        $local += (JP $cwd 'mingw64\bin\g++.exe')
        $local += (JP $cwd 'bin\g++.exe')
    } catch { }
    foreach ($c in ($local | Where-Object { $_ } | Select-Object -Unique)) {
        if (Test-Path $c) { return $c }
    }

    # 工具链可能被放到了英文位置（脚本目录是中文时会这样）
    $tr = Get-DefaultToolRoot
    if ($tr) {
        foreach ($c in @((JP $tr 'mingw64\bin\g++.exe'), (JP $tr 'bin\g++.exe'))) {
            if ($c -and (Test-Path $c)) { return $c }
        }
    }

    # 本目录没有，才用系统里的
    $g = Get-Command g++ -ErrorAction SilentlyContinue
    if ($g -and $g.Source) { return $g.Source }

    $cands = @(
        'C:\msys64\mingw64\bin\g++.exe',
        'C:\msys64\ucrt64\bin\g++.exe',
        'C:\mingw64\bin\g++.exe',
        'C:\MinGW\bin\g++.exe',
        'C:\TDM-GCC-64\bin\g++.exe',
        (JP $env:LOCALAPPDATA 'Programs\mingw64\bin\g++.exe')
    )
    foreach ($c in $cands) { if ($c -and (Test-Path $c)) { return $c } }

    $pkgRoot = JP $env:LOCALAPPDATA 'Microsoft\WinGet\Packages'
    if ($pkgRoot -and (Test-Path $pkgRoot)) {
        $hit = Get-ChildItem $pkgRoot -Filter 'g++.exe' -Recurse -Depth 6 -ErrorAction SilentlyContinue |
               Select-Object -First 1
        if ($hit) { return $hit.FullName }
    }
    return $null
}

function Find-GxxUnder {
    param([string]$Base)
    if (-not (Test-Path $Base)) { return $null }
    foreach ($rel in @('mingw64\bin\g++.exe', 'bin\g++.exe')) {
        $p = Join-Path $Base $rel
        if (Test-Path $p) { return $p }
    }
    $hit = Get-ChildItem $Base -Filter 'g++.exe' -Recurse -Depth 4 -ErrorAction SilentlyContinue |
           Select-Object -First 1
    if ($hit) { return $hit.FullName }
    return $null
}

function Find-CompilerZip {
    param([string]$Explicit)

    if ($Explicit -and (Test-Path $Explicit)) { return (Get-Item $Explicit).FullName }

    $dirs = @()
    if ($PSScriptRoot) { $dirs += $PSScriptRoot }
    try { $dirs += (Get-Location).Path } catch { }
    $dirs = $dirs | Where-Object { $_ } | Select-Object -Unique

    foreach ($d in $dirs) {
        if (-not (Test-Path $d)) { continue }
        $z = Get-ChildItem $d -Filter '*.zip' -File -ErrorAction SilentlyContinue |
             Where-Object { $_.Name -match '(?i)winlibs|mingw|gcc' } |
             Sort-Object LastWriteTime -Descending
        if ($z) { return $z[0].FullName }
    }
    return $null
}

function Find-Node {
    $n = Get-Command node -ErrorAction SilentlyContinue
    if ($n -and $n.Source) { return $n.Source }
    $cands = @(
        (JP $env:ProgramFiles 'nodejs\node.exe'),
        (JP ${env:ProgramFiles(x86)} 'nodejs\node.exe'),
        (JP $env:LOCALAPPDATA 'Programs\nodejs\node.exe'))
    if ($ToolRoot) { $cands += (JP $ToolRoot 'nodejs\node.exe') }
    foreach ($c in $cands) { if ($c -and (Test-Path $c)) { return $c } }

    $pkgRoot = JP $env:LOCALAPPDATA 'Microsoft\WinGet\Packages'
    if ($pkgRoot -and (Test-Path $pkgRoot)) {
        $hit = Get-ChildItem $pkgRoot -Filter 'node.exe' -Recurse -Depth 6 -ErrorAction SilentlyContinue |
               Select-Object -First 1
        if ($hit) { return $hit.FullName }
    }
    return $null
}

# ============================================================
#  中文源文件名探测：只报告，绝不改名、绝不移动用户文件
#  目录本身已被上游规则限制为纯英文，但题目文件名由人/插件决定，
#  中文题名很常见，所以单独查一次。
# ============================================================
function Get-NonAsciiSourceNames {
    param([string]$Dir)
    if (-not $Dir -or -not (Test-Path -LiteralPath $Dir)) { return @() }
    $exts = @('.cpp', '.cc', '.cxx', '.c', '.h', '.hpp')
    $found = @()
    try {
        $files = @(Get-ChildItem -LiteralPath $Dir -Recurse -Depth 2 -File -ErrorAction SilentlyContinue |
                   Where-Object { $exts -contains $_.Extension.ToLower() } |
                   Select-Object -First 400)
        foreach ($f in $files) {
            # 非 ASCII：中文、全角标点、emoji 都算
            if ($f.Name -match '[^\x00-\x7F]') { $found += $f.Name }
        }
    } catch { }
    return @($found | Select-Object -Unique)
}

function Get-EnvState {
    # 探测阶段不允许异常终止：本机有、别机没有，都只能算"未找到"
    $ErrorActionPreference = 'Continue'
    $st = @{}

    $st.CodeCli = Find-CodeCli
    $st.CodeVer = ''
    if ($st.CodeCli) {
        try { $st.CodeVer = (& $st.CodeCli --version 2>$null | Select-Object -First 1) } catch { }
    }

    $st.Gpp = Find-Gxx $CompilerBin
    $st.GccVer = ''
    $st.Gdb = $null
    $st.GdbVer = ''
    $st.SysInc = ''
    $st.CxxInc = ''
    $st.IsMode = 'windows-gcc-x64'

    if ($st.Gpp) {
        try { $st.GccVer = (& $st.Gpp -dumpversion 2>$null | Select-Object -First 1).Trim() } catch { }
        $cdir = Split-Path $st.Gpp -Parent
        $croot = Split-Path $cdir -Parent
        $st.SysInc = Join-Path $croot 'x86_64-w64-mingw32\include'
        $st.CxxInc = if ($st.GccVer) { Join-Path $croot "include\c++\$($st.GccVer)" } else { Join-Path $croot 'include\c++' }
        if ($croot -match 'mingw32|i686|Win32') { $st.IsMode = 'windows-gcc-x86' }

        $g = Get-Command gdb -ErrorAction SilentlyContinue
        if ($g -and $g.Source) { $st.Gdb = $g.Source }
        else {
            $ge = Join-Path $cdir 'gdb.exe'
            if (Test-Path $ge) { $st.Gdb = $ge }
        }
        if ($st.Gdb) {
            try {
                $line = (& $st.Gdb --version 2>$null | Select-Object -First 1)
                if ($line -match '(\d+\.\d+(\.\d+)?)') { $st.GdbVer = $Matches[1] }
            } catch { }
        }
    }

    $st.Node = Find-Node
    $st.NodeVer = ''
    if ($st.Node) {
        try { $st.NodeVer = (& $st.Node -v 2>$null).Trim() } catch { }
    }

    $st.Zip = Find-CompilerZip $CompilerZip
    $st.ZipSize = 0
    if ($st.Zip) {
        try { $st.ZipSize = [math]::Round((Get-Item $st.Zip).Length / 1MB, 1) } catch { $st.Zip = $null }
    }

    $st.Exts = @()
    if ($st.CodeCli) {
        try { $st.Exts = @(& $st.CodeCli --list-extensions 2>$null | ForEach-Object { $_.Trim().ToLower() }) }
        catch { }
    }

    $st.PluginMap = @(
        @{ Id = 'yltx.vscode-luogu';                               Name = '洛谷' },
        @{ Id = 'leetcode.vscode-leetcode';                        Name = 'LeetCode' },
        @{ Id = 'nowcoder.nowcoder';                               Name = '牛客' },
        @{ Id = 'divyanshuagrawal.competitive-programming-helper';  Name = 'CPH' },
        @{ Id = 'ms-vscode.cpptools';                              Name = 'C/C++' }
    )
    $st.Missing = @($st.PluginMap | Where-Object { $st.Exts -notcontains $_.Id.ToLower() })

    $st.VsCodeDir   = Join-Path $Root '.vscode'
    $st.ConfigReady = Test-Path (Join-Path $st.VsCodeDir 'settings.json')
    $st.WsReady     = Test-Path $Root
    $st.TplReady    = Test-Path (Join-Path $Root 'template.cpp')
    $st.NonAsciiSources = @(Get-NonAsciiSourceNames $Root)
    $st.NonAsciiCount   = $st.NonAsciiSources.Count

    $st.CfgGpp = ''
    $st.CfgDrift = @()
    if ($st.ConfigReady) {
        try {
            $cfg = Get-Content (Join-Path $st.VsCodeDir 'settings.json') -Raw -Encoding UTF8 | ConvertFrom-Json
            $st.CfgGpp = $cfg.'C_Cpp.default.compilerPath'
            if ($st.CfgGpp -and $st.Gpp -and ($st.CfgGpp -ne $st.Gpp)) {
                $st.CfgDrift += "编译器 -> $($st.CfgGpp)"
            }
            $leetExp = Join-Path $Root 'leetcode'
            $nowExp  = Join-Path $Root 'nowcoder'
            if ($cfg.'leetcode.workspaceFolder' -and ($cfg.'leetcode.workspaceFolder' -ne $leetExp)) {
                $st.CfgDrift += "LeetCode 目录 -> $($cfg.'leetcode.workspaceFolder')"
            }
            if ($cfg.'nowcoder.workspaceFolder' -and ($cfg.'nowcoder.workspaceFolder' -ne $nowExp)) {
                $st.CfgDrift += "牛客目录 -> $($cfg.'nowcoder.workspaceFolder')"
            }
        } catch {
            $st.CfgDrift += '配置文件无法解析（可能含注释）'
        }
    }

    # 是否已经有一套"能用"的配置：
    # 除了 <Root>\.vscode，也认脚本目录本身的 .vscode（用户原来就配好的那套）
    # 判据故意保守：只要存在 settings.json 就算"已配置"，一律不覆盖。
    # 因为把用户能用的配置写坏，比少装一次严重得多。
    $st.ConfigUsable = $false
    $st.ExistingConfig = ''
    foreach ($cand in @(
            (Join-Path $st.VsCodeDir 'settings.json'),
            (JP $PSScriptRoot '.vscode\settings.json'))) {
        if (-not $cand) { continue }
        if (-not (Test-Path $cand)) { continue }
        # 能解析、解析不了（含注释）都算"已有配置"
        $st.ConfigUsable = $true
        $st.ExistingConfig = $cand
        break
    }

    return [PSCustomObject]$st
}

# ============================================================
#  状态面板
# ============================================================
function Show-Status {
    param($st)

    Write-Host ""
    Write-Host $script:LINE -ForegroundColor DarkCyan
    Write-Host "  算法刷题环境 · 当前状态" -ForegroundColor White
    Write-Host $script:LINE -ForegroundColor DarkCyan

    $m = Mark ([bool]$st.CodeCli)
    $c = if ($st.CodeCli) { 'Gray' } else { 'DarkYellow' }
    $v = if ($st.CodeVer) { "VSCode $($st.CodeVer)" } else { 'VSCode' }
    Write-Host ("  编辑器   {0} {1}" -f $m, $v) -ForegroundColor $c
    if ($st.CodeCli) { Write-Host ("           {0}" -f $st.CodeCli) -ForegroundColor DarkGray }
    else { Write-Host "           未安装，插件无法安装" -ForegroundColor DarkYellow }

    $m = Mark ([bool]$st.Gpp)
    $c = if ($st.Gpp) { 'Gray' } else { 'DarkYellow' }
    $v = if ($st.GccVer) { "GCC $($st.GccVer)   ($($st.IsMode))" } else { '编译器' }
    Write-Host ("  编译器   {0} {1}" -f $m, $v) -ForegroundColor $c
    if ($st.Gpp) {
        Write-Host ("           {0}" -f (Split-Path $st.Gpp -Parent)) -ForegroundColor DarkGray
        if ((Split-Path $st.Gpp -Parent).Length -gt 180) {
            Write-Host "           路径偏长，接近 Windows 260 字符上限，建议换浅目录" -ForegroundColor DarkYellow
        }
        if (-not (Test-AsciiPath $st.Gpp)) {
            Write-Host "           路径含中文：g++ 在该位置无法编译（找不到 crt2.o），请移到纯英文目录" -ForegroundColor DarkYellow
        }
    } elseif ($st.Zip) {
        Write-Host "           未安装，但检测到编译包，可解压" -ForegroundColor DarkYellow
    } else {
        Write-Host "           未安装，且同目录无编译包" -ForegroundColor DarkYellow
    }

    $m = Mark ([bool]$st.Gdb)
    $v = if ($st.GdbVer) { "GDB $($st.GdbVer)" } else { '调试器' }
    Write-Host ("  调试器   {0} {1}" -f $m, $v) -ForegroundColor $(if ($st.Gdb) { 'Gray' } else { 'DarkGray' })

    $m = Mark ([bool]$st.Node)
    $v = if ($st.NodeVer) { "Node.js $($st.NodeVer)" } else { 'Node.js（仅 LeetCode 需要）' }
    Write-Host ("  Node     {0} {1}" -f $m, $v) -ForegroundColor $(if ($st.Node) { 'Gray' } else { 'DarkGray' })

    if ($st.Zip) {
        Write-Host ("  编译包   [OK] {0}  ({1} MB)" -f (Split-Path $st.Zip -Leaf), $st.ZipSize) -ForegroundColor Gray
    } else {
        Write-Host "  编译包   [--] 同目录未找到 winlibs/mingw/gcc 相关 zip" -ForegroundColor DarkGray
    }

    Write-Host $script:THIN -ForegroundColor DarkCyan
    Write-Host "  插件" -ForegroundColor White
    foreach ($p in $st.PluginMap) {
        $has = $st.Exts -contains $p.Id.ToLower()
        $tag = if ($has) { "[OK] $($p.Name)" } else { "[--] $($p.Name)" }
        Write-Host ("           {0}" -f $tag) -ForegroundColor $(if ($has) { 'Gray' } else { 'DarkYellow' })
    }
    if ($st.Exts.Count -gt 0) {
        Write-Host ("           本机共 {0} 个扩展，缺 {1} 个" -f $st.Exts.Count, $st.Missing.Count) -ForegroundColor DarkGray
    }

    Write-Host $script:THIN -ForegroundColor DarkCyan
    Write-Host "  工作区" -ForegroundColor White

    Write-Host ("  目录     {0} {1}" -f (Mark $st.WsReady), $Root) -ForegroundColor Gray

    $m = Mark $st.ConfigReady
    if ($st.CfgDrift.Count -gt 0) { $m = '[!!]' }
    # 配置在脚本目录里（不是 <Root>\.vscode）时，也算已就绪，避免出现 [--] 却说"已检测到"
    if ((-not $st.ConfigReady) -and $st.ConfigUsable -and (-not $Overwrite)) { $m = '[OK]' }
    Write-Host ("  配置     {0} .vscode/settings.json" -f $m) -ForegroundColor $(if ($st.CfgDrift.Count -gt 0) { 'Yellow' } else { 'Gray' })
    foreach ($d in $st.CfgDrift) {
        Write-Host ("           路径已失效: {0}" -f $d) -ForegroundColor DarkYellow
    }
    if ($st.CfgDrift.Count -gt 0) {
        Write-Host "           加 -Overwrite 重跑即可按本机路径重写" -ForegroundColor DarkGray
    } elseif ($st.ConfigUsable -and -not $Overwrite) {
        Write-Host "           已检测到现有配置，默认不改动（要重写加 -Overwrite）" -ForegroundColor DarkGray
        if ($st.ExistingConfig) {
            Write-Host ("           来源: {0}" -f $st.ExistingConfig) -ForegroundColor DarkGray
        }
    }

    Write-Host ("  模板     {0} template.cpp" -f (Mark $st.TplReady)) -ForegroundColor Gray
    Write-Host "  标准     C++11 / C11（对齐蓝桥杯）" -ForegroundColor DarkGray

    # 中文文件名是 MinGW/GDB 的硬限制区，单独列一行，别等编译失败才发现
    $zhNames = @($st.NonAsciiSources)
    if ($zhNames.Count -gt 0) {
        $sample = (@($zhNames | Select-Object -First 3) -join '、')
        Write-Host ("  中文名   [!!] {0} 个源文件: {1}" -f $zhNames.Count, $sample) -ForegroundColor DarkYellow
        Write-Host  "           编译/运行已兼容（产物统一 a.exe）；F5 断点调试请改用英文文件名" -ForegroundColor DarkGray
    } else {
        Write-Host  "  中文名   [OK] 未发现中文/全角文件名" -ForegroundColor DarkGray
    }

    if ($DryRun) { Write-Host "`n  [演练模式] 所有操作只打印，不实际执行" -ForegroundColor Yellow }

    Write-Host $script:LINE -ForegroundColor DarkCyan
}

# ============================================================
#  动作：解压编译包
# ============================================================
function Add-UserPath {
    param([string]$Dir)
    if (-not $Dir) { return }
    if ($NoPathUpdate) { SkipIt '已指定 -NoPathUpdate，不写入用户 PATH'; return }
    $cur = [Environment]::GetEnvironmentVariable('Path', 'User')
    $items = @()
    if ($cur) { $items = @($cur -split ';' | Where-Object { $_ -and $_.Trim() }) }

    # 便携模式下文件夹会被搬走，PATH 里会留下死路径。
    # 但只在 -Overwrite（用户明确要求重写）时才清理，否则一律不动现有 PATH
    $kept = @()
    $dropped = 0
    foreach ($it in $items) {
        $t = $it.TrimEnd('\')
        if ($Overwrite -and ($t -match '(?i)\\mingw64\\bin$') -and (-not (Test-Path $t))) { $dropped++; continue }
        if ($kept -contains $t) { continue }
        $kept += $t
    }
    if ($dropped -gt 0) { Caution "已清理 $dropped 条失效的 mingw 路径（文件夹被移动或删除过）" }

    $target = $Dir.TrimEnd('\')
    $hasTarget = $kept -contains $target
    if ($hasTarget -and ($dropped -eq 0)) { SkipIt '已在用户 PATH'; return }
    if ($DryRun) { SkipIt '将更新用户 PATH'; return }

    try {
        if (-not $hasTarget) { $kept += $target }
        [Environment]::SetEnvironmentVariable('Path', ($kept -join ';'), 'User')
        if (-not (($env:Path -split ';') -contains $target)) { $env:Path = "$env:Path;$target" }
        Ok '已加入用户 PATH'
    } catch { Caution "无法写入用户 PATH: $($_.Exception.Message)" }
}

# ---------- 工具链安装位置（便携式：跟着脚本目录走）----------

# 便携根目录 = 脚本所在目录。不猜盘符，放到 U 盘 G:\ 也一样。
# 只有脚本目录不可写时（只读介质、直接在压缩包里运行）才退回用户目录。
function Get-PortableRoot {
    if ($script:PortableRoot) { return $script:PortableRoot }

    $base = $PSScriptRoot
    if (-not $base) { try { $base = (Get-Location).Path } catch { } }
    if (-not $base) { $base = $env:USERPROFILE }

    if (-not (Test-DirWritable $base)) {
        Caution "脚本所在目录不可写: $base"
        if ($script:LastWriteError) { Hint ("原因: {0}" -f $script:LastWriteError) }
        Hint  "已改用用户目录: $env:USERPROFILE（只读介质才应该走到这里）"
        $base = $env:USERPROFILE
    }
    $script:PortableRoot = $base
    return $base
}

# 纯 ASCII 路径判定：MinGW 的 g++/ld 无法处理非 ASCII 路径，
# 会把自身路径解析成乱码（D:\再搬一次 -> D:/鍐嶆惉涓€娆?），
# 进而找不到 crt2.o / libstdc++，编译必然失败。所以工具链路径必须全英文。
function Test-AsciiPath {
    param([string]$Path)
    if (-not $Path) { return $true }
    return ($Path -match '^[\x20-\x7E]*$')
}

# 找一个纯英文的可写位置放工具链：非系统盘优先，其次 ProgramData / LocalAppData
# 选一个纯英文、可写的"父目录"，工具链和工作区都挂在它下面
# 非系统盘优先，其次 ProgramData / LocalAppData / C:\CppRoot
function Get-AsciiParent {
    if ($script:AsciiParent) { return $script:AsciiParent }

    $cands = @()
    try {
        foreach ($d in [System.IO.DriveInfo]::GetDrives()) {
            try {
                if ($d.DriveType -ne 'Fixed' -or -not $d.IsReady) { continue }
                $root = $d.Name.TrimEnd('\')
                if ($root -ieq $env:SystemDrive) { continue }
                $cands += $root
            } catch { }
        }
    } catch { }
    $cands += (JP $env:ProgramData 'CppRoot')
    $cands += (JP $env:LOCALAPPDATA 'CppRoot')   # 用户名是中文时会被下面的 ASCII 判断剔除
    $cands += 'C:\CppRoot'

    foreach ($c in $cands) {
        if (-not $c) { continue }
        if (-not (Test-AsciiPath $c)) { continue }
        $dr = $null
        try { $dr = [System.IO.Path]::GetPathRoot($c) } catch { }
        if (-not $dr -or -not (Test-Path $dr)) { continue }
        if ($DryRun) { $script:AsciiParent = $c; return $c }
        if (Test-DirWritable $c) { $script:AsciiParent = $c; return $c }
    }
    return $null
}

# 工具链安装根
# 脚本目录是英文 → 就放脚本目录（真便携）
# 脚本目录含中文 → 必须改放到英文位置，否则编译全挂
function Get-DefaultToolRoot {
    if ($script:ResolvedToolRoot) { return $script:ResolvedToolRoot }

    $portable = Get-PortableRoot
    if (Test-AsciiPath $portable) {
        $script:ResolvedToolRoot = $portable
        return $portable
    }

    $parent = Get-AsciiParent
    if ($parent) {
        $alt = Join-Path $parent 'CppEnv'
        if (-not $script:AsciiWarned) {
            $script:AsciiWarned = $true
            Caution "脚本目录含中文: $portable"
            Hint  "MinGW 的 g++ 在中文路径下会找不到 crt2.o，无法编译"
            Hint  "工具链改放: $alt"
        }
        $script:ResolvedToolRoot = $alt
        return $alt
    }

    Caution '找不到纯英文的可写位置，工具链只能放在中文路径下 —— 编译会失败'
    Hint    '请把脚本移到纯英文目录（例如 D:\CppEnv），或用 -ToolRoot 指定'
    $script:ResolvedToolRoot = $portable
    return $portable
}

# 工作区根：与工具链同一块盘，同样是纯英文
# 脚本目录是英文 → <脚本目录>\workspace（跟着文件夹走）
# 脚本目录含中文 → <英文父目录>\CppWorkspace
function Get-DefaultWorkspaceRoot {
    $portable = Get-PortableRoot
    if (Test-AsciiPath $portable) { return (Join-Path $portable 'workspace') }

    $parent = Get-AsciiParent
    if ($parent) { return (Join-Path $parent 'CppWorkspace') }
    return (Join-Path $portable 'workspace')
}

# 在几个可能的位置找"已经解压好的 mingw64"
# 返回 @{ Bin = g++.exe 路径; Root = 工具链根目录 }，找不到返回 $null
function Find-ExtractedToolchain {
    param([string]$Zip, [string]$Dest)

    $roots = @()
    if ($Dest) { $roots += $Dest }
    if ($PSScriptRoot) { $roots += $PSScriptRoot }
    if ($Zip) { $roots += (Split-Path $Zip -Parent) }
    try { $roots += (Get-Location).Path } catch { }
    $roots = $roots | Where-Object { $_ } | Select-Object -Unique

    foreach ($r in $roots) {
        $p = JP $r 'mingw64\bin\g++.exe'
        if ($p -and (Test-Path $p)) { return @{ Bin = $p; Root = (JP $r 'mingw64') } }
    }
    foreach ($r in $roots) {
        $p = JP $r 'bin\g++.exe'
        if ($p -and (Test-Path $p)) { return @{ Bin = $p; Root = $r } }
    }
    return $null
}

# 从 g++.exe 反推工具链根目录：<根>\mingw64\bin\g++.exe → <根>\mingw64；<根>\bin\g++.exe → <根>
function Get-ToolchainRootFromBin {
    param([string]$Bin)
    if (-not $Bin) { return '' }
    try {
        $binDir = Split-Path $Bin -Parent
        if (-not $binDir) { return '' }
        $root = Split-Path $binDir -Parent
        if (-not $root) { return $binDir }
        return $root
    } catch { return '' }
}

# 把工具链目录搬到安装目录，成功返回迁移后的 g++.exe 路径
function Move-ToolchainTo {
    param([string]$Root, [string]$Dest)
    if (-not $Root -or -not (Test-Path $Root)) { return $null }
    try {
        New-Item -ItemType Directory -Force -Path $Dest | Out-Null
        Move-Item -LiteralPath $Root -Destination $Dest -Force -ErrorAction Stop
        return (Find-GxxUnder $Dest)
    } catch {
        Caution "迁移失败: $($_.Exception.Message)"
        return $null
    }
}

# ---------- 解压进度用的辅助函数 ----------

# 压缩包里有多少个条目（数不出来就返回 0，进度条退化为"已解压 N 个"）
function Get-ZipEntryCount {
    param([string]$Zip)
    try {
        Add-Type -AssemblyName System.IO.Compression.FileSystem -ErrorAction Stop
        $z = [System.IO.Compression.ZipFile]::OpenRead($Zip)
        $n = $z.Entries.Count
        $z.Dispose()
        return $n
    } catch { return 0 }
}

# 目标目录里已有多少文件（只枚举，不取文件大小，1 万多个也很快）
function Get-FileCount {
    param([string]$Dir)
    try { return @([System.IO.Directory]::EnumerateFiles($Dir, '*', [System.IO.SearchOption]::AllDirectories)).Count }
    catch { return 0 }
}

# 原地刷新一行进度：[████░░░░] 42%  5120/11576 个  12s
function Show-ExtractProgress {
    param([string]$Dir, [int]$Total, [double]$Sec)
    $n = Get-FileCount $Dir
    if ($Total -gt 0) {
        $pct  = [int]([math]::Min(100, $n * 100.0 / $Total))
        $w    = 22
        $fill = [int]($w * $pct / 100)
        $bar  = ('█' * $fill) + ('░' * ($w - $fill))
        Write-Host ("`r    [{0}] {1,3}%  {2,5}/{3} 个  {4,3:N0}s " -f $bar, $pct, $n, $Total, $Sec) -NoNewline -ForegroundColor DarkCyan
    } else {
        Write-Host ("`r    已解压 {0,5} 个文件   {1,3:N0}s   " -f $n, $Sec) -NoNewline -ForegroundColor DarkCyan
    }
}

function Invoke-Unpack {
    param($st)

    # 已有可用配置：不解压、不迁移、不动 PATH
    if (Test-ConfigLocked $st) {
        Ok "检测到已有可用配置，跳过解压与迁移"
        Hint '要重写请加 -Overwrite'
        return
    }

    Stage '解压编译包'

    # 先算安装目录（即使已有编译器也要算，用来判断它是不是散落在本目录）
    if ($ToolRoot) {
        $dest = Resolve-AbsPath $ToolRoot
        if (-not (Test-PathRoot $dest)) {
            Caution "指定的 -ToolRoot 盘符不存在: $dest"
            $dest = Get-DefaultToolRoot
            Hint "已改用: $dest"
        } elseif (-not (Test-AsciiPath $dest)) {
            # 中文路径下的 g++ 100% 编译失败，宁可换位置也不装进去
            Caution "指定的 -ToolRoot 含中文，g++ 无法在这种路径下工作: $dest"
            $dest = Get-DefaultToolRoot
            Hint "已改用: $dest"
        }
    } else {
        $dest = Get-DefaultToolRoot
    }
    $destNorm = $dest.TrimEnd('\')
    Ok "安装目录: $dest"

    # 本目录 / 压缩包目录 / 当前目录 —— 这几个位置解出来的算"散落"，应当迁移
    $localRoots = @()
    if ($PSScriptRoot) { $localRoots += $PSScriptRoot }
    if ($st.Zip) { $localRoots += (Split-Path $st.Zip -Parent) }
    try { $localRoots += (Get-Location).Path } catch { }
    $localRoots = $localRoots | Where-Object { $_ } | Select-Object -Unique

    function Test-UnderRoot {
        param([string]$Path, [string]$Root)
        if (-not $Path -or -not $Root) { return $false }
        $p = $Path.TrimEnd('\'); $r = $Root.TrimEnd('\')
        return (($p -eq $r) -or $p.StartsWith($r + '\'))
    }

    # ---------- 已经有编译器 ----------
    if ($st.Gpp) {
        $binDir   = Split-Path $st.Gpp -Parent
        $root     = Get-ToolchainRootFromBin $st.Gpp
        $underDest = Test-UnderRoot $root $dest

        # 已经在用户 PATH 里的，是既有安装，绝不动它
        $inPath = $false
        foreach ($pe in ($env:Path -split ';')) {
            if ($pe -and ($pe.TrimEnd('\') -ieq $binDir.TrimEnd('\'))) { $inPath = $true; break }
        }

        if ($underDest -or $inPath) {
            Ok "已有编译器，无需解压: $($st.Gpp)"
            $script:GxxPath = $st.Gpp
            if (-not $inPath) { Add-UserPath $binDir }
            return
        }

        # 不在安装目录、也不在 PATH：多半是本目录解压出来的，迁移过去
        $isLocal = $false
        foreach ($lr in $localRoots) { if (Test-UnderRoot $root $lr) { $isLocal = $true; break } }

        if ($isLocal -and $root) {
            Caution "检测到本目录已解压的编译器: $root"
            Hint "将迁移到: $dest（同盘是秒级改名；跨盘需复制约 900 MB）"
            if ($DryRun) {
                SkipIt '演练模式，不真的移动'
                $script:GxxPath = $st.Gpp
                return
            }
            $movedBin = Move-ToolchainTo $root $dest
            if ($movedBin) {
                Ok "迁移完成: $movedBin"
                $script:GxxPath = $movedBin
                Add-UserPath (Split-Path $movedBin -Parent)
            } else {
                Hint "改为直接使用原位置: $($st.Gpp)"
                $script:GxxPath = $st.Gpp
                Add-UserPath $binDir
            }
            return
        }

        Ok "已有编译器，无需解压: $($st.Gpp)"
        $script:GxxPath = $st.Gpp
        return
    }

    # ---------- 没有编译器 ----------
    if (-not $st.Zip) {
        Caution '未找到可用编译器，也没找到编译包'
        Hint '把 winlibs-*.zip 放到脚本同目录，或用 -CompilerZip 指定'
        Hint '下载地址: https://github.com/brechtsanders/winlibs_mingw/releases'
        return
    }

    Ok "编译包: $(Split-Path $st.Zip -Leaf)  ($($st.ZipSize) MB)"

    # 安装目录里已经有解压好的，直接用
    $existing = Find-ExtractedToolchain $st.Zip $dest
    if ($existing) {
        if (Test-UnderRoot $existing.Root $dest) {
            Ok "安装目录已有编译器: $($existing.Bin)"
            $script:GxxPath = $existing.Bin
            Add-UserPath (Split-Path $existing.Bin -Parent)
            return
        }
        Caution "检测到别处已解压的编译器: $($existing.Root)"
        Hint "将迁移到: $dest"
        if ($DryRun) {
            SkipIt '演练模式，不真的移动'
            $script:GxxPath = $existing.Bin
            return
        }
        $movedBin = Move-ToolchainTo $existing.Root $dest
        if ($movedBin) {
            Ok "迁移完成: $movedBin"
            $script:GxxPath = $movedBin
            Add-UserPath (Split-Path $movedBin -Parent)
        } else {
            Hint "改为直接使用原位置: $($existing.Bin)"
            $script:GxxPath = $existing.Bin
            Add-UserPath (Split-Path $existing.Bin -Parent)
        }
        return
    }

    if ($DryRun) { SkipIt "将解压到 $dest"; return }

    # 真正解压前再确认一次可写
    if (-not (Test-DirWritable $dest)) {
        $old = $dest
        $dest = JP $env:LOCALAPPDATA 'cp-toolchain'
        Caution "安装目录不可写: $old"
        Hint "已改用: $dest"
        $destNorm = "$dest".TrimEnd('\')
    }

    # ---------- 解压：tar → .NET → Expand-Archive，全过程显示进度 ----------
    $total = Get-ZipEntryCount $st.Zip      # 0 = 数不出来，退化为只显示已解压数
    $t0 = [Diagnostics.Stopwatch]::StartNew()
    $slowWarned = $false

    if ($total -gt 0) {
        Caution ("正在解压 {0} MB / {1} 个文件 —— 下面会实时显示进度，不要关窗口" -f $st.ZipSize, $total)
    } else {
        Caution ("正在解压 {0} MB —— 下面会实时显示已解压的文件数" -f $st.ZipSize)
    }

    if (-not (Test-Path $dest)) {
        try { New-Item -ItemType Directory -Force -Path $dest | Out-Null }
        catch {
            Caution "无法创建解压目录: $dest"
            Hint "去掉 -ToolRoot 默认就用脚本所在目录；也可指定其它可写位置"
            return
        }
    }

    $done = $false
    $how  = ''

    # 1) tar：Windows 10 1803+ 自带 bsdtar。实测 253MB / 1.2 万文件约 11 秒
    $tar = Join-Path $env:SystemRoot 'System32\tar.exe'
    if (-not (Test-Path $tar)) {
        $c = Get-Command tar -ErrorAction SilentlyContinue
        if ($c -and $c.Source) { $tar = $c.Source } else { $tar = $null }
    }
    if ($tar) {
        try {
            $argStr = '-xf "' + $st.Zip + '" -C "' + $dest + '"'
            $proc = Start-Process -FilePath $tar -ArgumentList $argStr -NoNewWindow -PassThru -ErrorAction Stop
            while (-not $proc.HasExited) {
                Start-Sleep -Milliseconds 1200
                if ((-not $slowWarned) -and ($t0.Elapsed.TotalSeconds -gt 120)) {
                    $slowWarned = $true
                    Write-Host ""
                    Caution '已超过 2 分钟。若装了杀毒软件，它可能在逐个扫描新文件，可把目标目录加入白名单'
                    Write-Host ""
                }
                Show-ExtractProgress $dest $total $t0.Elapsed.TotalSeconds
            }
            $proc.WaitForExit()
            Write-Host ""
            if ($proc.ExitCode -eq 0) { $done = $true; $how = 'tar' }
            else { Caution ("tar 退出码 $($proc.ExitCode)，改用 .NET 解压") }
        } catch {
            Write-Host ""
            Caution "tar 启动失败（$($_.Exception.Message)），改用 .NET 解压"
        }
    }

    # 2) .NET ZipFile：实测约 17 秒。它是阻塞调用，中途刷不了进度，只在没有 tar 时用
    if (-not $done) {
        try {
            Add-Type -AssemblyName System.IO.Compression.FileSystem -ErrorAction Stop
            Ok '使用 .NET 解压（阻塞执行，无进度条，通常 20-40 秒）'
            try { [System.IO.Compression.ZipFile]::ExtractToDirectory($st.Zip, $dest, $true) }
            catch [System.Management.Automation.MethodException] {
                [System.IO.Compression.ZipFile]::ExtractToDirectory($st.Zip, $dest)
            }
            $done = $true; $how = '.NET'
        } catch {
            Caution '.NET 解压不可用，改用内置 Expand-Archive（PS 5.1 下对 1.2 万个小文件可能要好几分钟）'
        }
    }

    # 3) Expand-Archive：最后兜底
    if (-not $done) {
        try {
            Expand-Archive -LiteralPath $st.Zip -DestinationPath $dest -Force -ErrorAction Stop
            $done = $true; $how = 'Expand-Archive（慢速兜底）'
        } catch {
            Caution "解压失败: $($_.Exception.Message)"
            Hint '若提示文件被占用，关闭相关程序后重试'
            return
        }
    }

    $t0.Stop()
    if ($done) {
        $n = Get-FileCount $dest
        # 压缩包里含目录条目，最后一条进度可能停在 96% 左右，补一条满格避免看着像没走完
        try {
            Write-Host ("`r    [{0}] 100%  {1}/{1} 个  {2,3:N0}s " -f ('█' * 22), $n, $t0.Elapsed.TotalSeconds) -NoNewline -ForegroundColor DarkCyan
            Write-Host ""
        } catch { }
        Ok ("解压完成（{0}）  {1} 个文件  {2:N0} 秒" -f $how, $n, $t0.Elapsed.TotalSeconds)
    }


    $gxx = Find-GxxUnder $dest
    if ($gxx) {
        Ok "已定位 g++: $gxx"
        $script:GxxPath = $gxx
        Add-UserPath (Split-Path $gxx -Parent)
    } else {
        Caution '解压完成但没找到 g++.exe，请检查压缩包结构'
        Hint "解压目录: $dest"
        Hint '正常结构应为 <安装目录>\mingw64\bin\g++.exe'
    }
}

# ============================================================
#  动作：补装插件
# ============================================================
function Invoke-Plugins {
    param($st, [string[]]$Only = $null, [switch]$Standalone)

    # -Standalone：本步骤是独立执行的（菜单 [3] / -Action plugins / 自定义里只勾了插件），
    # 后面不会再有人写配置，所以插件就绪后要接着把平台参数写掉。
    # Invoke-Full 不传这个开关——它后面会统一调 Invoke-Config，写两次会互相覆盖。

    Stage '检查并补装插件'

    if ($SkipExtensions) { SkipIt '已指定 -SkipExtensions'; return }
    if (-not $st.CodeCli) {
        Caution '未找到 code CLI，无法安装插件'
        Hint '在 VSCode 里 Ctrl+Shift+P -> Install ''code'' command in PATH'
        return
    }

    Ok "本机已装 $($st.Exts.Count) 个扩展"

    $targets = $st.Missing
    if ($Only) {
        $targets = @($st.PluginMap | Where-Object { $Only -contains $_.Id -and $st.Exts -notcontains $_.Id.ToLower() })
    }
    if ($targets.Count -eq 0) {
        Ok '目标插件均已就绪，跳过下载'
        if ($Standalone) {
            Stage '插件无需安装，改为写入平台参数'
            Invoke-Config $st -Platforms -Workspace
        }
        return
    }

    foreach ($p in $targets) {
        if ($DryRun) { SkipIt "将安装 $($p.Name)"; continue }
        try {
            # 用 2>$null 而非 2>&1：PowerShell 5.1 下把原生 stderr 并入管道会生成
            # NativeCommandError，在 Stop 模式下会中断整轮安装
            & $st.CodeCli --install-extension $p.Id --force 2>$null | Out-Null
            Ok "已安装 $($p.Name)"
        } catch { Caution "安装失败: $($p.Name)，可在扩展商店搜索 $($p.Id)" }
    }

    # 装完插件同样要把参数落到 settings.json，否则新电脑上 CPH 会退回默认
    # （没有 -std=c++11、没有 GBK 转换，中文直接乱码）
    if ($Standalone -and (-not $DryRun)) {
        Stage '写入平台参数'
        Invoke-Config (Get-EnvState) -Platforms -Workspace
    }
}

# ============================================================
#  动作：生成配置文件
# ============================================================
function Write-Utf8File {
    param([string]$Path, [string]$Content)
    $leaf = Split-Path $Path -Leaf
    if ($DryRun) { SkipIt "将写入 $leaf"; return }
    if ((Test-Path $Path) -and (-not $Overwrite)) {
        SkipIt "$leaf 已存在，保留（需覆盖请加 -Overwrite）"
        return
    }
    if (Test-Path $Path) {
        try {
            $bk = "$Path.bak-$(Get-Date -Format 'yyyyMMdd-HHmmss')"
            Copy-Item $Path $bk -Force
            Caution "已备份 $leaf -> $(Split-Path $bk -Leaf)"
        } catch {
            Caution "备份 $leaf 失败，仍继续写入"
        }
    }
    try {
        New-Item -ItemType Directory -Force -Path (Split-Path $Path -Parent) | Out-Null
        [System.IO.File]::WriteAllText($Path, $Content, (New-Object System.Text.UTF8Encoding($false)))
        Ok "写入 $leaf"
    } catch {
        Caution "写入 $leaf 失败: $($_.Exception.Message)"
        Hint '文件可能被 VSCode 占用（关闭编辑器后重试），或目标目录只读'
        Hint '去掉 -Root 默认就用 <脚本所在目录>\workspace；也可指定其它可写位置'
    }
}

function Invoke-Config {
    param($st, [switch]$Core, [switch]$Platforms, [switch]$Workspace)

    # 已有可用配置：一个配置文件都不写
    if (Test-ConfigLocked $st) {
        Ok "检测到已有可用配置，跳过写入"
        Hint '要重写请加 -Overwrite'
        return
    }

    if (-not ($Core -or $Platforms -or $Workspace)) {
        $Core = $true; $Platforms = $true; $Workspace = $true
    }

    Stage '生成配置'

    if ($Workspace) {
        foreach ($d in @($Root, $st.VsCodeDir,
                        (Join-Path $Root 'luogu'), (Join-Path $Root 'leetcode'),
                        (Join-Path $Root 'nowcoder'), (Join-Path $Root 'cph'))) {
            if ($DryRun) { SkipIt "将创建 $d" }
            elseif (Test-Path $d) { SkipIt "已存在 $d" }
            else {
                try { New-Item -ItemType Directory -Force -Path $d | Out-Null; Ok "创建 $d" }
                catch { Caution "无法创建 $d（权限不足或磁盘只读）" }
            }
        }
    }

    if ($Core -and (-not $st.Gpp)) {
        Caution '尚未安装编译器，无法生成 C/C++ 配置'
        Hint '请先执行「解压编译包」，或取消勾选 C/C++ 配置项'
        return
    }

    $gxx     = $st.Gpp
    $ismode  = $st.IsMode
    # gcc 与 g++ 同目录；缺失时退回命令名
    $gccAbs  = Join-Path (Split-Path $gxx -Parent) 'gcc.exe'
    if (-not (Test-Path $gccAbs)) { $gccAbs = 'gcc' }
    # 全部编译入口都用绝对路径：不依赖用户 PATH 是否刷新过，
    # 也不依赖 VSCode 是在写 PATH 之前还是之后启动的
    $qGxx = if ($gxx -match '\s') { '"' + $gxx + '"' } else { $gxx }
    $qGcc = if ($gccAbs -match '\s') { '"' + $gccAbs + '"' } else { $gccAbs }
    $sysInc  = $st.SysInc
    $cxxInc  = $st.CxxInc
    $leetWs  = Join-Path $Root 'leetcode'
    $nowWs   = Join-Path $Root 'nowcoder'
    $cphSave = Join-Path $Root 'cph'
    $tplFile = Join-Path $Root 'template.cpp'
    $settingsPath = Join-Path $st.VsCodeDir 'settings.json'

    # ---------- settings.json：按勾选项拼装 ----------
    if ($Core -or $Platforms) {
        $cfg = [ordered]@{}

        if ($Core) {
            $cfg['C_Cpp.default.compilerPath']       = $gxx
            $cfg['C_Cpp.default.cStandard']          = 'c11'
            $cfg['C_Cpp.default.cppStandard']        = 'c++11'
            $cfg['C_Cpp.default.intelliSenseMode']   = $ismode
            $cfg['C_Cpp.default.includePath']        = @("$sysInc\**", "$cxxInc\**")
            $cfg['C_Cpp.intelliSenseEngine']         = 'default'
            $cfg['C_Cpp.clang_format_style']         = 'Microsoft'
            $cfg['[cpp]'] = [ordered]@{
                'editor.defaultFormatter'   = 'ms-vscode.cpptools'
                'editor.formatOnSave'       = $true
                'editor.wordBasedSuggestions' = 'off'
                'editor.suggest.insertMode' = 'replace'
            }
            $cfg['[c]'] = [ordered]@{
                'editor.defaultFormatter' = 'ms-vscode.cpptools'
                'editor.formatOnSave'     = $true
            }
            $cfg['files.encoding']         = 'utf8'
            $cfg['files.autoGuessEncoding'] = $false
            $cfg['files.eol']              = "`r`n"
            $cfg['terminal.integrated.defaultProfile.windows'] = 'Command Prompt'
            $cfg['code-runner.runInTerminal']      = $true
            $cfg['code-runner.fileDirectoryAsCwd'] = $true
            $cfg['code-runner.ignoreSelection']    = $true
            # 中文/含空格的源文件名：输入用 "$fileName" 加引号，输出固定 ASCII 名 a.exe。
            # 不能用 $fileNameWithoutExt 当 -o 目标：MinGW 的 ld 会把中文名写成乱码文件，
            # 后面的运行命令 / launch.json 就永远找不到那个 exe（见 README 第九节）。
            # 优化级别沿用原值 -O2，不在此变更：是否统一到考场的 -O0 仍待确认。
            $cfg['code-runner.executorMap'] = [ordered]@{
                'cpp' = 'cd /d "$dir" && ' + $qGxx + ' -std=c++11 -O2 -Wall -finput-charset=UTF-8 -fexec-charset=GBK "$fileName" -o a.exe && .\a.exe'
                'c'   = 'cd /d "$dir" && ' + $qGcc + ' -std=c11 -O2 -Wall -finput-charset=UTF-8 -fexec-charset=GBK "$fileName" -o a.exe && .\a.exe'
            }
            $cfg['editor.rulers']          = @(80, 100, 120)
            $cfg['editor.minimap.enabled'] = $false
            $cfg['editor.wordWrap']        = 'on'
        }

        if ($Platforms) {
            $cfg['luogu.defaultProblemSet']            = '洛谷题库'
            $cfg['luogu.guessProblemID']               = $true
            $cfg['luogu.showSelectDifficultyHint']     = $false
            $cfg['luogu.showSelectProblemsetHint']     = $false
            $cfg['luogu.showRecordPanel']              = $true
            # 用纯题号当文件名（插件默认值）。另两种会带中文题名/空格，
            # 正是中文文件名乱码的源头；要可读性可自行改回，代价见 README 第九节。
            $cfg['luogu.cphStyle']                     = 'ProblemID'
            $cfg['luogu.cphPort']                      = 27121
            $cfg['leetcode.endpoint']                  = 'leetcode-cn'
            $cfg['leetcode.defaultLanguage']           = 'cpp'
            $cfg['leetcode.workspaceFolder']           = $leetWs
            $cfg['leetcode.showDescription']           = 'In Webview'
            $cfg['leetcode.enableStatusBar']           = $true
            $cfg['leetcode.hideSolved']                = $false
            $cfg['nowcoder.defaultLanguage']           = 'C++'
            $cfg['nowcoder.workspaceFolder']           = $nowWs
            $cfg['nowcoder.submitType']                = '练习模式'
            $cfg['cph.general.defaultLanguage']        = 'cpp'
            $cfg['cph.general.firstTime']              = $false
            $cfg['cph.general.timeOut']                = 3000
            $cfg['cph.general.autoShowJudge']          = $true
            $cfg['cph.general.saveLocation']           = $cphSave
            $cfg['cph.general.defaultLanguageTemplateFileLocation'] = $tplFile
            $cfg['cph.language.cpp.Command']           = $qGxx
            $cfg['cph.language.cpp.Args']              = '-std=c++11 -O2 -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=GBK'
            $cfg['cph.language.cpp.OutputArg']         = '-o'
            $cfg['cph.language.cpp.SubmissionCompiler'] = 'GNU G++23 14.2 (64 bit, msys2)'
        }

        Write-Utf8File -Path $settingsPath -Content ($cfg | ConvertTo-Json -Depth 10)
    }

    # ---------- tasks.json ----------
    # 产物一律叫 a.exe：源文件名可以是中文，但 -o 目标绝不能是中文。
    # 实测 g++ 读中文名的 .cpp 没问题，可 ld 写出的 exe 名会变成
    # "Á½ÊýÖ®ºÍ.exe" 这种乱码，launch.json 与运行命令随后全部对不上。
    if ($Core) {
        $tasks = [ordered]@{
            version = '2.0.0'
            tasks = @(
                [ordered]@{
                    label = '构建并运行 (考场模式 -O0)'
                    type = 'process'
                    command = $gxx
                    args = @('-fdiagnostics-color=always','-std=c++11','-O0','-Wall','-Wextra',
                             '-finput-charset=UTF-8','-fexec-charset=GBK',
                             '${fileBasename}','-o','a.exe')
                    options = [ordered]@{ cwd = '${fileDirname}' }
                    problemMatcher = @('$gcc')
                    group = [ordered]@{ kind = 'build'; isDefault = $true }
                    detail = '与 Dev-C++ 5.11 等价：C++11、不开优化、不生成调试符号'
                },
                [ordered]@{
                    label = '调试构建 (-g -O0)'
                    type = 'process'
                    command = $gxx
                    args = @('-fdiagnostics-color=always','-std=c++11','-g','-O0','-Wall','-Wextra',
                             '-finput-charset=UTF-8','-fexec-charset=GBK',
                             '${fileBasename}','-o','a.exe')
                    options = [ordered]@{ cwd = '${fileDirname}' }
                    problemMatcher = @('$gcc')
                    group = [ordered]@{ kind = 'build'; isDefault = $false }
                    detail = '带调试符号，供 F5 断点调试使用'
                },
                [ordered]@{
                    label = 'OJ 极限压测 (-O2)'
                    type = 'process'
                    command = $gxx
                    args = @('-fdiagnostics-color=always','-std=c++11','-O2','-DNDEBUG',
                             '-finput-charset=UTF-8','-fexec-charset=GBK',
                             '${fileBasename}','-o','a.exe')
                    options = [ordered]@{ cwd = '${fileDirname}' }
                    problemMatcher = @('$gcc')
                    group = [ordered]@{ kind = 'build'; isDefault = $false }
                    detail = '开启优化，查看性能上限；注意这不是考场环境'
                },
                [ordered]@{
                    label = 'C 构建并运行 (-O0)'
                    type = 'process'
                    command = $gccAbs
                    args = @('-fdiagnostics-color=always','-std=c11','-O0','-Wall','-Wextra',
                             '-finput-charset=UTF-8','-fexec-charset=GBK',
                             '${fileBasename}','-o','a.exe')
                    options = [ordered]@{ cwd = '${fileDirname}' }
                    problemMatcher = @('$gcc')
                    group = [ordered]@{ kind = 'build'; isDefault = $false }
                    detail = 'C11 标准'
                }
            )
        }
        Write-Utf8File -Path (Join-Path $st.VsCodeDir 'tasks.json') -Content ($tasks | ConvertTo-Json -Depth 10)
    }

    # ---------- launch.json ----------
    if ($Core) {
        $launch = [ordered]@{
            version = '0.2.0'
            configurations = @(
                [ordered]@{
                    name = 'C++ 调试 (GDB)'
                    type = 'cppdbg'
                    request = 'launch'
                    # 必须与 tasks.json 的 -o 完全一致（ASCII 固定名）
                    program = '${fileDirname}\a.exe'
                    args = @()
                    stopAtEntry = $false
                    cwd = '${fileDirname}'
                    environment = @()
                    externalConsole = $false
                    MIMode = 'gdb'
                    miDebuggerPath = $(if ($st.Gdb) { $st.Gdb } else { 'gdb' })
                    setupCommands = @(
                        [ordered]@{ description = '为 gdb 启用整齐格式化'; text = '-enable-pretty-printing'; ignoreFailures = $true },
                        [ordered]@{ description = '反汇编风格设为 Intel'; text = '-gdb-set disassembly-flavor intel'; ignoreFailures = $true }
                    )
                    preLaunchTask = '调试构建 (-g -O0)'
                }
            )
        }
        Write-Utf8File -Path (Join-Path $st.VsCodeDir 'launch.json') -Content ($launch | ConvertTo-Json -Depth 10)
    }

    # ---------- c_cpp_properties.json ----------
    if ($Core) {
        $props = [ordered]@{
            version = 4
            configurations = @(
                [ordered]@{
                    name = 'Win32'
                    # MinGW 下不需要 VS 那套 _DEBUG/UNICODE 宏，留着只会干扰条件编译
                    defines = @()
                    compilerPath = $gxx
                    cStandard = 'c11'
                    cppStandard = 'c++11'
                    intelliSenseMode = $ismode
                    includePath = @('${workspaceFolder}/**', "$sysInc\**", "$cxxInc\**")
                }
            )
        }
        Write-Utf8File -Path (Join-Path $st.VsCodeDir 'c_cpp_properties.json') -Content ($props | ConvertTo-Json -Depth 10)
    }

    # ---------- extensions.json（推荐清单，随插件项一起写） ----------
    if ($Core -or $Platforms) {
        $ext = [ordered]@{
            recommendations = @(
                'yltx.vscode-luogu',
                'leetcode.vscode-leetcode',
                'nowcoder.nowcoder',
                'divyanshuagrawal.competitive-programming-helper',
                'ms-vscode.cpptools'
            )
            unwantedRecommendations = @()
        }
        Write-Utf8File -Path (Join-Path $st.VsCodeDir 'extensions.json') -Content ($ext | ConvertTo-Json -Depth 10)
    }

    # ---------- template.cpp ----------
    if ($Workspace) {
        if (Test-Path $tplFile) {
            SkipIt 'template.cpp 已存在，保留用户版本'
        } else {
            $tplCpp = @'
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef unsigned long long ull;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    return 0;
}
'@
            Write-Utf8File -Path $tplFile -Content $tplCpp
        }
    }

    if (-not $DryRun) {
        Write-Host ""
        Write-Host "  完成。用 VSCode 打开工作区：" -ForegroundColor White
        Write-Host ("    code `"{0}`"" -f $Root) -ForegroundColor Gray
        if ($Core) {
            Write-Host "  文件名规则：源文件可中文；产物统一 a.exe；F5 断点调试请用英文文件名" -ForegroundColor DarkGray
        }
    }
}

# ============================================================
#  动作汇总
# ============================================================
function Invoke-Full {
    $st = Get-EnvState

    # 已经有一套能用的配置 → 一个字节都不动
    if (Test-ConfigLocked $st) {
        Ok ("检测到已有配置: {0}" -f $(if ($st.ExistingConfig) { $st.ExistingConfig } else { $st.VsCodeDir }))
        Hint '默认跳过，不安装、不解压、不写配置、不改 PATH'
        Hint '确实想按本机重写，再显式加 -Overwrite'
        return
    }

    Invoke-Unpack  $st
    $st = Get-EnvState          # 解压后重新探测
    Invoke-Plugins $st
    Invoke-Config  $st -Core -Platforms -Workspace
}

# ============================================================
#  菜单
# ============================================================
function Show-PluginChoices {
    param($st)

    Write-Host ""
    Write-Host "  选择要安装的插件（可多选，如 1,3,5；A 全选；回车返回）：" -ForegroundColor White
    for ($i = 0; $i -lt $st.PluginMap.Count; $i++) {
        $p = $st.PluginMap[$i]
        $has = $st.Exts -contains $p.Id.ToLower()
        $tag = if ($has) { '已装' } else { '缺失' }
        Write-Host ("    [{0}] {1,-10} {2}" -f ($i + 1), $p.Name, $tag) -ForegroundColor $(if ($has) { 'DarkGray' } else { 'Gray' })
    }
    Write-Host "    [A] 全部缺失项" -ForegroundColor Gray
    Write-Host "    [回车] 返回" -ForegroundColor DarkGray
    return (Read-Host "  输入")
}

function Show-CustomConfig {
    param($st)

    Write-Host ""
    Write-Host "  自定义配置 —— 选择要做的项目（可多选，如 1,3,5）" -ForegroundColor White
    Write-Host ""
    Write-Host "    [1] 解压 MinGW 编译包       当前: $(if ($st.Gpp) { '已安装，将跳过' } elseif ($st.Zip) { '待解压' } else { '无压缩包' })" -ForegroundColor Gray
    $plugNote = if ($st.Missing.Count -eq 0) { '已装齐，将写入平台参数' } else { "缺 $($st.Missing.Count) 个" }
    Write-Host "    [2] VSCode 插件与参数       当前: $plugNote" -ForegroundColor Gray
    Write-Host "    [3] C/C++ 编译配置          IntelliSense + 编译任务 + 调试" -ForegroundColor Gray
    Write-Host "    [4] 四平台插件参数          洛谷 / LeetCode / 牛客 / CPH" -ForegroundColor Gray
    Write-Host "    [5] 工作区目录与模板        目录骨架 + template.cpp" -ForegroundColor Gray
    Write-Host ""
    Write-Host "    [A] 全选（等同「一键配置」）" -ForegroundColor Gray
    Write-Host "    [回车] 返回" -ForegroundColor DarkGray
    Write-Host ""
    return (Read-Host "  输入序号，逗号分隔")
}

# ============================================================
#  主流程
# ============================================================

# 工作区路径预处理
# 未指定 -Root 时：脚本目录是英文就放 <脚本目录>\workspace，否则放到英文位置
if (-not $Root) { $Root = Get-DefaultWorkspaceRoot }
$Root = Resolve-AbsPath $Root

# 显式指定的 -Root 也不能含中文：cp/code-runner 等在中文路径下会出各种问题
if (-not (Test-AsciiPath $Root)) {
    Caution "工作区路径含中文，MinGW 与部分插件在中文路径下会出问题: $Root"
    $Root = Get-DefaultWorkspaceRoot
    Hint "已改用: $Root"
    Hint "如果需要自定义位置，请用纯英文路径，例如 -Root `"D:\CppWorkspace`""
}

if (-not (Test-PathRoot $Root)) {
    $drv = $null
    try { $drv = [System.IO.Path]::GetPathRoot($Root) } catch { }
    Write-Host ""
    Write-Host ("  [FAIL] 目标盘符不存在: {0}" -f $drv) -ForegroundColor Red
    Write-Host "         这台电脑上没有这个盘符（比如指定的 D 盘不存在）" -ForegroundColor DarkGray
    Write-Host "         去掉 -Root 即可用默认位置：<脚本所在目录>\workspace" -ForegroundColor DarkGray
    Write-Host ""
    exit 1
}

Show-Logo
Write-Host ("  工作区: {0}" -f $Root) -ForegroundColor DarkGray

if ($Action) {
    $s = Get-EnvState
    Show-Status $s
    switch ($Action) {
        'status'  { }
        'unpack'  { Invoke-Unpack  $s }
        'plugins' { Invoke-Plugins $s -Standalone }
        'config'  { Invoke-Config  $s -Core -Platforms -Workspace }
        'full'    { Invoke-Full }
    }
    Write-Host ""
    exit 0
}

if ($DryRun) {
    $s = Get-EnvState
    Show-Status $s
    Invoke-Full
    Write-Host "`n  [演练结束] 去掉 -DryRun 才会真正执行。" -ForegroundColor Yellow
    Write-Host ""
    exit 0
}

# ---------------- 交互循环 ----------------
$running = $true
while ($running) {
    $state = Get-EnvState
    Show-Status $state

    Write-Host ""
    Write-Host "  请选择：" -ForegroundColor White
    Write-Host "    [1] 一键配置全部      解压 + 插件 + 配置，懒人选择" -ForegroundColor Green
    Write-Host "    [2] 自定义配置        分项勾选要做的内容" -ForegroundColor Gray
    Write-Host "    [3] 插件与参数        补装缺失插件；已装齐则改为写入平台参数" -ForegroundColor Gray
    Write-Host "    [4] 只生成配置" -ForegroundColor Gray
    Write-Host "    [5] 只解压编译包" -ForegroundColor Gray
    Write-Host "    [6] 刷新状态" -ForegroundColor DarkGray
    Write-Host "    [0] 退出" -ForegroundColor DarkGray
    Write-Host ""

    $choice = Read-Host "  输入序号"
    # stdin 不可用（任务计划、部分启动器）时 Read-Host 返回 $null，按退出处理而不是崩
    if ($null -eq $choice) { $choice = '0' }

    switch ($choice.Trim()) {
        '1' { Invoke-Full }

        '2' {
            $sel = Show-CustomConfig $state
            if ($null -eq $sel) { $sel = '' }
            $sel = $sel.Trim()
            if ($sel) {
                $items = @($sel -split '[,\s]+' | Where-Object { $_ })
                $all = ($items -contains 'a') -or ($items -contains 'A')

                $doUnpack    = $all -or ($items -contains '1')
                $doPlugins   = $all -or ($items -contains '2')
                $doCore      = $all -or ($items -contains '3')
                $doPlatforms = $all -or ($items -contains '4')
                $doWorkspace = $all -or ($items -contains '5')

                if ($doUnpack) {
                    Invoke-Unpack (Get-EnvState)
                }
                $cur = Get-EnvState     # 解压后重新探测
                if ($doPlugins) {
                    # 只勾了插件、没勾任何配置项时，插件就绪后接着把平台参数写掉
                    $solo = -not ($doCore -or $doPlatforms -or $doWorkspace)
                    Invoke-Plugins $cur -Standalone:$solo
                }
                if ($doCore -or $doPlatforms -or $doWorkspace) {
                    Invoke-Config $cur -Core:$doCore -Platforms:$doPlatforms -Workspace:$doWorkspace
                }
            }
        }

        '3' { Invoke-Plugins (Get-EnvState) -Standalone }
        '4' { Invoke-Config  (Get-EnvState) -Core -Platforms -Workspace }
        '5' { Invoke-Unpack  (Get-EnvState) }
        '6' { Write-Host "`n  正在重新检测..." -ForegroundColor DarkGray }
        '0' { $running = $false }
        ''  { }
        default { Write-Host "`n  无效输入：$choice（请输入 0-6）" -ForegroundColor Yellow }
    }
}

Write-Host ""
Write-Host "  已退出。" -ForegroundColor DarkGray
Write-Host ""
