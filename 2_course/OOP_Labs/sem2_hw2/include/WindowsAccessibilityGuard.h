#pragma once

#include <QAbstractNativeEventFilter>

class QCoreApplication;

// Локальный обход зависания Qt/Windows UI Automation. Он не меняет настройки
// Windows и не затрагивает другие приложения. Подробности и ограничение
// поддержки экранных дикторов описаны в README.
class WindowsAccessibilityGuard : public QAbstractNativeEventFilter
{
public:
    explicit WindowsAccessibilityGuard(QCoreApplication& application);
    ~WindowsAccessibilityGuard() override;

    bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override;

private:
    QCoreApplication& application_;
    bool installed_ = false;
};
