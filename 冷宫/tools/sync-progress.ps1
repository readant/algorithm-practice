# Progress scan for algorithm-practice (read-only).
# Usage (repo root):
#   powershell -NoProfile -File tools\sync-progress.ps1

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

function Count-Markdown([string]$path) {
    if (-not (Test-Path -LiteralPath $path)) { return 0 }
    return @(Get-ChildItem -LiteralPath $path -Recurse -Filter 'tiexie.md' -ErrorAction SilentlyContinue).Count
}

# Luogu uses Chinese filename 题解.md — filter in a second pass if needed
function Count-Tiexie([string]$path) {
    if (-not (Test-Path -LiteralPath $path)) { return 0 }
    $files = Get-ChildItem -LiteralPath $path -Recurse -File -ErrorAction SilentlyContinue
    return @($files | Where-Object { $_.Name -eq '题解.md' -or $_.Name -match 'tiexie|题解' }).Count
}

Write-Host '=== COMPETE progress scan (suggestions only) ===' -ForegroundColor Cyan
Write-Host ("Root: {0}" -f $root)
Write-Host ''

$luoguRoot = Join-Path $root '题解归档/洛谷'
$lists = @{}
if (Test-Path -LiteralPath $luoguRoot) {
    Get-ChildItem -LiteralPath $luoguRoot -Directory | ForEach-Object {
        $files = Get-ChildItem -LiteralPath $_.FullName -Recurse -File -ErrorAction SilentlyContinue
        $n = @($files | Where-Object { $_.Extension -eq '.md' -and $_.BaseName -match '题解' }).Count
        $lists[$_.Name] = $n
    }
}

$sumLuogu = 0
Write-Host '[Luogu beginner lists - count of solution writeups]'
foreach ($k in ($lists.Keys | Sort-Object)) {
    Write-Host ('  {0,-28} {1}' -f $k, $lists[$k])
    $sumLuogu += $lists[$k]
}
Write-Host ('  {0,-28} {1}' -f 'TOTAL writeup files', $sumLuogu)
Write-Host '  (AC count may exceed writeup count; trust progress.md for KPI)'
Write-Host ''

$zhenRoot = Join-Path $root '题解归档/蓝桥杯/真题强化24'
$zhenDirs = 0
$zhenSol = 0
$zhenMd = 0
if (Test-Path -LiteralPath $zhenRoot) {
    $zhenDirs = @(Get-ChildItem -LiteralPath $zhenRoot -Directory -ErrorAction SilentlyContinue).Count
    $all = Get-ChildItem -LiteralPath $zhenRoot -Recurse -File -ErrorAction SilentlyContinue
    $zhenSol = @($all | Where-Object { $_.Name -eq 'solution.cpp' }).Count
    $zhenMd = @($all | Where-Object { $_.Extension -eq '.md' -and $_.BaseName -match '题解' }).Count
}
Write-Host '[ZhenTi 24]'
Write-Host ('  problem folders : {0}' -f $zhenDirs)
Write-Host ('  writeup md      : {0}' -f $zhenMd)
Write-Host ('  solution.cpp    : {0}' -f $zhenSol)
Write-Host ''

Write-Host '[Console files]'
foreach ($f in @('COMPETE.md', '竞赛轨/蓝桥杯/主线.md', '竞赛轨/蓝桥杯/进度.md')) {
    $p = Join-Path $root $f
    $ok = 'OK'
    if (-not (Test-Path -LiteralPath $p)) { $ok = 'MISSING' }
    Write-Host ('  [{0}] {1}' -f $ok, $f)
}
Write-Host ''

$a1 = Join-Path $root '学习模块/02-学习层/01-C++/02-数据结构/A1-STL容器与算法（工具箱）.md'
$a1ok = 'MISSING'
if (Test-Path -LiteralPath $a1) { $a1ok = 'OK' }
Write-Host '[Block material]'
Write-Host ('  [{0}] A1 STL toolbox' -f $a1ok)
Write-Host ''

Write-Host 'Next steps:' -ForegroundColor Yellow
Write-Host '  1. Compare with 竞赛轨/蓝桥杯/进度.md; if mismatch, ask agent to update progress'
Write-Host '  2. Daily training follows COMPETE instruction card; do not use total problem count as KPI'
Write-Host '  3. This script never git-commits and never edits md files'
