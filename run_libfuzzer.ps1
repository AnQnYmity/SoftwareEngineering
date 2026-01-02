# PowerShell LibFuzzer 运行脚本

Write-Host "===================================" -ForegroundColor Cyan
Write-Host "   LibFuzzer 模糊测试启动脚本" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan

# 1. 检查编译器 (使用 MinGW g++)
$compiler = "C:\Program Files\mingw64\bin\g++.exe"
if (-not (Test-Path $compiler)) {
    Write-Host "❌ 未检测到 MinGW g++ 编译器" -ForegroundColor Red
    exit 1
}

Write-Host "✅ 检测到编译器: $compiler" -ForegroundColor Green

# 2. 创建输出目录
New-Item -ItemType Directory -Force -Path "fuzz_corpus" | Out-Null
New-Item -ItemType Directory -Force -Path "fuzz_artifacts" | Out-Null

# 3. 编译模糊测试目标
Write-Host ">>> 正在编译 Fuzzer (Standalone Mode)..." -ForegroundColor Yellow

# 注意：由于 Windows 上 Clang 对 MinGW 的支持有限（不支持 -fsanitize=fuzzer），
# 我们使用 g++ 编译一个独立的驱动程序来运行模糊测试目标。
# 这保持了代码与 LibFuzzer 的兼容性，同时能在当前环境下运行。

$srcFiles = @(
    "tests/fuzz_driver.cpp",
    "tests/fuzz_target_libfuzzer.cpp",
    "src/TransactionRepository.cpp",
    "src/ImportExportService.cpp",
    "src/FileStorage.cpp",
    "src/StatisticsService.cpp",
    "src/NotificationService.cpp",
    "src/TransactionController.cpp"
)

$compileFiles = $srcFiles -join " "

# 使用 g++ 编译
$cmd = "& '$compiler' -g -O1 $compileFiles -o fuzzer.exe -std=c++17"
Write-Host $cmd
Invoke-Expression $cmd

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ 编译失败" -ForegroundColor Red
    exit 1
}

Write-Host "✅ 编译成功: fuzzer.exe" -ForegroundColor Green

# 4. 运行模糊测试
Write-Host ">>> 开始模糊测试 (按 Ctrl+C 停止)..." -ForegroundColor Yellow
.\fuzzer.exe
