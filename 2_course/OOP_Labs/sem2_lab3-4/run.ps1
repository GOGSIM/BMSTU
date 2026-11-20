# Сценарий работает в Windows PowerShell 5.1 и PowerShell 7.
[CmdletBinding()]
param(
    [string]$QtRoot = 'C:\Qt\6.12.0\mingw_64',
    [string]$QtTools = 'C:\Qt\Tools',
    [switch]$Test,
    [switch]$BuildOnly
)
$ErrorActionPreference = 'Stop'
$previousPath = $env:PATH
$cmake = Join-Path $QtTools 'CMake_64\bin\cmake.exe'
$ninja = Join-Path $QtTools 'Ninja\ninja.exe'
$compiler = Join-Path $QtTools 'mingw1310_64\bin\g++.exe'
$deploy = Join-Path $QtRoot 'bin\windeployqt.exe'
$toolchain = Join-Path $QtRoot 'lib\cmake\Qt6\qt.toolchain.cmake'
$buildDirectory = Join-Path $PSScriptRoot 'build'
$executable = Join-Path $buildDirectory 'sem2_lab34_text_editor.exe'
foreach ($required in @($cmake, $ninja, $compiler, $toolchain, $deploy)) {
    if (!(Test-Path -LiteralPath $required)) { throw "Не найден файл комплекта Qt: $required" }
}
# Не закрываем редактор автоматически: в нём могут оставаться несохранённые документы.
$running = @(Get-Process -Name 'sem2_lab34_text_editor' -ErrorAction SilentlyContinue |
    Where-Object { $_.Path -and [string]::Equals($_.Path, $executable, [StringComparison]::OrdinalIgnoreCase) })
if ($running.Count -gt 0) { throw 'Сохраните документы и закройте редактор перед пересборкой.' }
try {
    $env:PATH = "$(Join-Path $QtRoot 'bin');$(Split-Path $compiler);$(Split-Path $ninja);$previousPath"
    & $cmake -S $PSScriptRoot -B $buildDirectory -G Ninja '-DCMAKE_BUILD_TYPE=Debug' `
        "-DCMAKE_MAKE_PROGRAM=$ninja" "-DCMAKE_CXX_COMPILER=$compiler" `
        "-DCMAKE_TOOLCHAIN_FILE=$toolchain" '-DBUILD_TESTING=ON'
    if ($LASTEXITCODE -ne 0) { throw 'Ошибка настройки CMake.' }
    & $cmake --build $buildDirectory --parallel 4
    if ($LASTEXITCODE -ne 0) { throw 'Ошибка компиляции.' }
    # После сборки exe можно запускать напрямую: DLL и платформенный плагин рядом.
    & $deploy --compiler-runtime --translations ru --no-opengl-sw --no-system-d3d-compiler $executable
    if ($LASTEXITCODE -ne 0) { throw 'Ошибка размещения библиотек Qt.' }
    if ($Test) {
        & (Join-Path $QtTools 'CMake_64\bin\ctest.exe') --test-dir $buildDirectory --output-on-failure
        if ($LASTEXITCODE -ne 0) { throw 'Проверки завершились ошибкой.' }
    }
    if (!$BuildOnly) { Start-Process -FilePath $executable -WorkingDirectory $PSScriptRoot }
} finally { $env:PATH = $previousPath }
