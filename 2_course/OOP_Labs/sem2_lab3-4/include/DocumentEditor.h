#pragma once
#include <QTextEdit>

// Аналог DocWindow из лекции 4. Диалоги остаются в главном окне.
class DocumentEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit DocumentEditor(const QString& untitledName, QWidget* parent = nullptr);
    bool loadFile(const QString& path);
    bool saveFile(const QString& path);
    QString filePath() const { return path_; }
    QString displayName() const;
    QString lastError() const { return error_; }
private:
    QString path_;
    QString untitledName_;
    QString error_;
};
