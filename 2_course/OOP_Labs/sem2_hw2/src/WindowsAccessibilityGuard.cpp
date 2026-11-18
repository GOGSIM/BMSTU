#include "WindowsAccessibilityGuard.h"

#include <QCoreApplication>
#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

WindowsAccessibilityGuard::WindowsAccessibilityGuard(QCoreApplication& application)
    : application_(application)
{
#ifdef Q_OS_WIN
    // Два снятых стека зависших процессов показывали блокировку GUI-потока
    // внутри UIAutomationCore при обращении к провайдеру Qt. setActive(false)
    // недостаточно: Qt снова активирует мост при следующем WM_GETOBJECT.
    // Поэтому перехватываем запрос до создания COM-провайдера Qt.
    // Явное включение штатного моста оставлено для экранных дикторов и проверки
    // на другой версии Qt/Windows: SEM2_HW2_NATIVE_ACCESSIBILITY=1.
    if (qEnvironmentVariableIntValue("SEM2_HW2_NATIVE_ACCESSIBILITY") != 1) {
        application_.installNativeEventFilter(this);
        installed_ = true;
    }
#endif
}

WindowsAccessibilityGuard::~WindowsAccessibilityGuard()
{
    if (installed_) {
        application_.removeNativeEventFilter(this);
    }
}

bool WindowsAccessibilityGuard::nativeEventFilter(
    const QByteArray& eventType, void* message, qintptr* result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        const MSG* nativeMessage = static_cast<const MSG*>(message);
        if (nativeMessage->message == WM_GETOBJECT) {
            // Нулевой результат означает отсутствие собственного провайдера.
            // Системе остаётся стандартное описание окна; Qt не обходит дерево.
            *result = 0;
            return true;
        }
    }
#else
    Q_UNUSED(eventType);
    Q_UNUSED(message);
    Q_UNUSED(result);
#endif
    // Мышь, клавиатура, рисование, меню и прочие события обрабатывает Qt.
    return false;
}
