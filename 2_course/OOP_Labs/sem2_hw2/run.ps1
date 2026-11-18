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
$executable = Join-Path $buildDirectory 'sem2_hw2_xml_viewer.exe'

foreach ($required in @($cmake, $ninja, $compiler, $toolchain)) {
    if (!(Test-Path -LiteralPath $required)) {
        throw "Не найден файл комплекта Qt: $required. Укажите -QtRoot и -QtTools."
    }
}

try {
    # Windows не позволяет компоновщику заменить работающий exe. Закрываем
    # только экземпляры из каталога сборки этого проекта, проверяя полный путь.
    # Приложение только читает XML: закрытие не изменяет файлы данных.
    $runningViewers = @(Get-Process -Name 'sem2_hw2_xml_viewer' -ErrorAction SilentlyContinue |
        Where-Object { $_.Path -and [string]::Equals($_.Path, $executable,
            [StringComparison]::OrdinalIgnoreCase) })
    foreach ($viewer in $runningViewers) {
        try {
            if (!$viewer.HasExited) {
                Write-Host "Закрываю предыдущее окно ДЗ2 (PID $($viewer.Id)) перед сборкой..."
                [void]$viewer.CloseMainWindow()
                if (!$viewer.WaitForExit(3000)) {
                    # Зависшее окно не обработает WM_CLOSE. Повторно проверяем
                    # путь перед завершением именно этого процесса.
                    if (![string]::Equals($viewer.Path, $executable,
                        [StringComparison]::OrdinalIgnoreCase)) {
                        throw 'Не удалось подтвердить путь запущенного приложения.'
                    }
                    Write-Host 'Окно не ответило за 3 секунды. Завершаю экземпляр ДЗ2.'
                    Stop-Process -InputObject $viewer -ErrorAction Stop
                    if (!$viewer.WaitForExit(3000)) {
                        throw 'Предыдущее приложение не завершилось; сборка отменена.'
                    }
                }
            }
        } catch {
            # Пользователь мог закрыть окно одновременно со сценарием.
            # Если процесс уже завершён, файл больше не заблокирован.
            if (!$viewer.HasExited) { throw }
        } finally {
            $viewer.Dispose()
        }
    }
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
