# PowerShell 测试运行脚本
# 适用于 Windows 系统

Write-Host "===================================" -ForegroundColor Cyan
Write-Host "   单元测试与集成测试执行脚本" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan

# 创建构建目录
Write-Host ""
Write-Host ">>> 创建构建目录..." -ForegroundColor Yellow
New-Item -ItemType Directory -Force -Path "build" | Out-Null
Set-Location build

# 配置 CMake
Write-Host ""
Write-Host ">>> 配置 CMake..." -ForegroundColor Yellow
& "C:\Program Files\CMake\bin\cmake.exe" .. -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/Program Files/mingw64/bin/g++.exe" -DCMAKE_C_COMPILER="C:/Program Files/mingw64/bin/gcc.exe" -DCMAKE_BUILD_TYPE=Debug

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ CMake 配置失败" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# 编译项目
Write-Host ""
Write-Host ">>> 编译项目..." -ForegroundColor Yellow
& "C:\Program Files\CMake\bin\cmake.exe" --build . --config Debug

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ 编译失败" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host ""
Write-Host "===================================" -ForegroundColor Cyan
Write-Host "   运行单元测试" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan

# 运行 TransactionRepository 测试
Write-Host ""
Write-Host ">>> 测试 1: TransactionRepository 单元测试" -ForegroundColor Yellow
Write-Host "-----------------------------------" -ForegroundColor Yellow
if (Test-Path "Debug\test_transaction_repository.exe") {
    .\Debug\test_transaction_repository.exe --gtest_output=xml:test_transaction_repository_results.xml
} else {
    .\test_transaction_repository.exe --gtest_output=xml:test_transaction_repository_results.xml
}
$repoTestResult = $LASTEXITCODE

# 运行 StatisticsService 测试
Write-Host ""
Write-Host ">>> 测试 2: StatisticsService 单元测试" -ForegroundColor Yellow
Write-Host "-----------------------------------" -ForegroundColor Yellow
if (Test-Path "Debug\test_statistics_service.exe") {
    .\Debug\test_statistics_service.exe --gtest_output=xml:test_statistics_service_results.xml
} else {
    .\test_statistics_service.exe --gtest_output=xml:test_statistics_service_results.xml
}
$statsTestResult = $LASTEXITCODE

# 运行集成测试
Write-Host ""
Write-Host "===================================" -ForegroundColor Cyan
Write-Host "   运行集成测试" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan
Write-Host ""
Write-Host ">>> 测试 3: 集成测试" -ForegroundColor Yellow
Write-Host "-----------------------------------" -ForegroundColor Yellow
if (Test-Path "Debug\test_integration.exe") {
    .\Debug\test_integration.exe --gtest_output=xml:test_integration_results.xml
} else {
    .\test_integration.exe --gtest_output=xml:test_integration_results.xml
}
$integrationTestResult = $LASTEXITCODE

# 总结测试结果
Write-Host ""
Write-Host "===================================" -ForegroundColor Cyan
Write-Host "   测试结果总结" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan
Write-Host ""

if ($repoTestResult -eq 0) {
    Write-Host "✓ TransactionRepository 测试: 通过" -ForegroundColor Green
} else {
    Write-Host "✗ TransactionRepository 测试: 失败" -ForegroundColor Red
}

if ($statsTestResult -eq 0) {
    Write-Host "✓ StatisticsService 测试: 通过" -ForegroundColor Green
} else {
    Write-Host "✗ StatisticsService 测试: 失败" -ForegroundColor Red
}

if ($integrationTestResult -eq 0) {
    Write-Host "✓ 集成测试: 通过" -ForegroundColor Green
} else {
    Write-Host "✗ 集成测试: 失败" -ForegroundColor Red
}

Write-Host ""
Write-Host "===================================" -ForegroundColor Cyan

# 返回总体结果
Set-Location ..
if ($repoTestResult -eq 0 -and $statsTestResult -eq 0 -and $integrationTestResult -eq 0) {
    Write-Host "✓ 所有测试通过！" -ForegroundColor Green
    exit 0
} else {
    Write-Host "✗ 部分测试失败" -ForegroundColor Red
    exit 1
}
