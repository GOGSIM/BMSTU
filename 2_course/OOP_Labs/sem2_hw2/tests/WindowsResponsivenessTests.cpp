#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QProcess>
#include <QProcessEnvironment>
#include <QtTest>
#include <qt_windows.h>
#include <initguid.h>
#include <uiautomation.h>

namespace
{
// Этот клиент запускается отдельно: зависший вызов COM не должен повесить
// сам тест и оставить окно приложения без контроля.
int probeAccessibility(HWND window)
{
    if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
        return 1;
    }
    IUIAutomation* automation = nullptr;
    HRESULT status = CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                     IID_IUIAutomation, reinterpret_cast<void**>(&automation));
    if (SUCCEEDED(status)) {
        for (int attempt = 0; attempt < 10 && SUCCEEDED(status); ++attempt) {
            IUIAutomationElement* root = nullptr;
            status = automation->ElementFromHandle(window, &root);
            if (SUCCEEDED(status) && root) {
                IUIAutomationCondition* condition = nullptr;
                status = automation->CreateTrueCondition(&condition);
                if (SUCCEEDED(status)) {
                    IUIAutomationElementArray* children = nullptr;
                    status = root->FindAll(TreeScope_Descendants, condition, &children);
                    if (children) {
                        children->Release();
                    }
                    condition->Release();
                }
                root->Release();
            }
        }
        automation->Release();
    }
    CoUninitialize();
    return SUCCEEDED(status) ? 0 : 2;
}

struct WindowSearch
{
    DWORD processId;
    HWND window = nullptr;
};

BOOL CALLBACK findWindow(HWND window, LPARAM argument)
{
    auto* search = reinterpret_cast<WindowSearch*>(argument);
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId == search->processId && IsWindowVisible(window)
        && GetWindow(window, GW_OWNER) == nullptr) {
        search->window = window;
        return FALSE;
    }
    return TRUE;
}

bool responds(HWND window)
{
    DWORD_PTR result = 0;
    return SendMessageTimeoutW(window, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK,
                               250, &result) != 0;
}

quint64 cpuTime(HANDLE process)
{
    FILETIME created, exited, kernel, user;
    if (!GetProcessTimes(process, &created, &exited, &kernel, &user)) {
        return 0;
    }
    ULARGE_INTEGER kernelTicks, userTicks;
    kernelTicks.LowPart = kernel.dwLowDateTime;
    kernelTicks.HighPart = kernel.dwHighDateTime;
    userTicks.LowPart = user.dwLowDateTime;
    userTicks.HighPart = user.dwHighDateTime;
    return (kernelTicks.QuadPart + userTicks.QuadPart) / 10000; // миллисекунды
}
}

class WindowsResponsivenessTests : public QObject
{
    Q_OBJECT

private slots:
    void idleWindowStaysResponsiveDuringNativeAccessibilityQueries()
    {
        QProcess viewer;
        QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
        // Проверяем штатный защищённый режим, даже если терминал настроен иначе.
        environment.remove("SEM2_HW2_NATIVE_ACCESSIBILITY");
        viewer.setProcessEnvironment(environment);
        QStringList files;
        for (const QString& name : {"cafes.xml", "coffeehouses.xml", "bars.xml", "fast_food.xml"}) {
            files.append(QStringLiteral(TEST_DATA_DIR) + '/' + name);
        }
        viewer.start(QDir(QCoreApplication::applicationDirPath()).filePath("sem2_hw2_xml_viewer.exe"), files);
        QVERIFY(viewer.waitForStarted(5000));
        WindowSearch search{static_cast<DWORD>(viewer.processId())};
        QElapsedTimer startup;
        startup.start();
        while (!search.window && startup.elapsed() < 5000) {
            EnumWindows(findWindow, reinterpret_cast<LPARAM>(&search));
            QTest::qWait(50);
        }
        QVERIFY2(search.window, "The viewer did not create a native window");
        QVERIFY(responds(search.window));

        QProcess probe;
        probe.setProcessEnvironment(environment);
        probe.start(QCoreApplication::applicationFilePath(),
                    {"--probe", QString::number(reinterpret_cast<quintptr>(search.window))});
        QVERIFY(probe.waitForStarted(5000));
        QElapsedTimer queryTime;
        queryTime.start();
        while (probe.state() != QProcess::NotRunning && queryTime.elapsed() < 8000) {
            if (!responds(search.window)) {
                probe.kill();
                probe.waitForFinished(1000);
                QFAIL("The GUI stopped responding during Windows UI Automation queries");
            }
            probe.waitForFinished(100);
        }
        if (probe.state() != QProcess::NotRunning) {
            probe.kill();
            probe.waitForFinished(1000);
            QFAIL("Windows UI Automation query timed out");
        }
        QCOMPARE(probe.exitCode(), 0);

        HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, search.processId);
        QVERIFY(process);
        const quint64 before = cpuTime(process);
        QElapsedTimer idle;
        idle.start();
        bool responsive = true;
        while (idle.elapsed() < 10000 && responsive) {
            responsive = responds(search.window);
            QTest::qWait(100);
        }
        const quint64 usedCpu = cpuTime(process) - before;
        CloseHandle(process);
        QVERIFY2(responsive, "The window hung while idle after accessibility queries");
        QVERIFY2(usedCpu < 2000, "The idle GUI consumed more than two CPU seconds in ten seconds");
        // Это обычное закрытие созданного тестом окна, а не завершение
        // произвольных пользовательских процессов.
        PostMessageW(search.window, WM_CLOSE, 0, 0);
        QVERIFY2(viewer.waitForFinished(3000), "The application did not close normally");
        QCOMPARE(viewer.exitStatus(), QProcess::NormalExit);
    }
};

int main(int argc, char* argv[])
{
    QCoreApplication application(argc, argv);
    const QStringList arguments = application.arguments();
    if (arguments.size() == 3 && arguments.at(1) == "--probe") {
        return probeAccessibility(reinterpret_cast<HWND>(arguments.at(2).toULongLong()));
    }
    WindowsResponsivenessTests tests;
    return QTest::qExec(&tests, argc, argv);
}

#include "WindowsResponsivenessTests.moc"
