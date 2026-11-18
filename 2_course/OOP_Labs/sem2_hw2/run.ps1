# Сборка и запуск комплектом Qt. PATH меняется только внутри этого сценария.
[CmdletBinding()]
param(
    [string]$QtRoot = 'C:\Qt\6.12.0\mingw_64',
    [string]$QtTools = 'C:\Qt\Tools',
    [switch]$Test,
    [switch]$BuildOnly,
    [switch]$Demo
)

$ErrorActionPreference = 'Stop'
$previousPath = $env:PATH
$cmake = Join-Path $QtTools 'CMake_64\bin\cmake.exe'
$ninja = Join-Path $QtTools 'Ninja\ninja.exe'
$compiler = Join-Path $QtTools 'mingw1310_64\bin\g++.exe'
$toolchain = Join-Path $QtRoot 'lib\cmake\Qt6\qt.toolchain.cmake'
$buildDirectory = Join-Path $PSScriptRoot 'build'

foreach ($required in @($cmake, $ninja, $compiler, $toolchain)) {
    if (!(Test-Path -LiteralPath $required)) {
        throw "Не найден файл комплекта Qt: $required. Укажите -QtRoot и -QtTools."
    }
}

try {
    # Qt должен использовать свой MinGW, а не другой g++ из системного PATH.
    $env:PATH = "$(Join-Path $QtRoot 'bin');$(Split-Path $compiler);$(Split-Path $ninja);$previousPath"
    & $cmake -S $PSScriptRoot -B $buildDirectory -G Ninja '-DCMAKE_BUILD_TYPE=Debug' `
        "-DCMAKE_MAKE_PROGRAM=$ninja" "-DCMAKE_CXX_COMPILER=$compiler" `
        "-DCMAKE_TOOLCHAIN_FILE=$toolchain" '-DBUILD_TESTING=ON'
    if ($LASTEXITCODE -ne 0) { throw 'Ошибка настройки CMake.' }
    & $cmake --build $buildDirectory --parallel
    if ($LASTEXITCODE -ne 0) { throw 'Ошибка компиляции.' }
    if ($Test) {
        & (Join-Path $QtTools 'CMake_64\bin\ctest.exe') --test-dir $buildDirectory --output-on-failure --timeout 45
        if ($LASTEXITCODE -ne 0) { throw 'Проверки завершились ошибкой.' }
    }
    if (!$BuildOnly) {
        $executable = Join-Path $buildDirectory 'sem2_hw2_xml_viewer.exe'
        # Приложению нужны DLL Qt и MinGW. Дочерний процесс наследует PATH.
        if ($Demo) {
            $demoFiles = @('cafes.xml', 'coffeehouses.xml', 'bars.xml', 'fast_food.xml') |
                ForEach-Object { '"' + (Join-Path $buildDirectory "data\$_") + '"' }
            Start-Process -FilePath $executable -ArgumentList $demoFiles -WorkingDirectory $buildDirectory
        } else {
            Start-Process -FilePath $executable -WorkingDirectory $buildDirectory
        }
    }
} finally {
    $env:PATH = $previousPath
}
