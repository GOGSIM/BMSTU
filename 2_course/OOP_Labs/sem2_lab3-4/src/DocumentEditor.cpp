#include "DocumentEditor.h"
#include <QFile>
#include <QFileInfo>
#include <QFontDatabase>
#include <QSaveFile>
#include <QTextStream>

DocumentEditor::DocumentEditor(const QString& name, QWidget* parent)
    : QTextEdit(parent), untitledName_(name)
{
    setAcceptRichText(false);
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setLineWrapMode(QTextEdit::NoWrap);
    // Цвета поставляемого XML рассчитаны на светлую рабочую область.
    // Явная палитра сохраняет контраст и при тёмной теме Windows.
    QPalette colors = palette();
    colors.setColor(QPalette::Base, QColor("#ffffff"));
    colors.setColor(QPalette::Text, QColor("#202124"));
    colors.setColor(QPalette::Highlight, QColor("#c2dcff"));
    colors.setColor(QPalette::HighlightedText, QColor("#202124"));
    setPalette(colors);
}

QString DocumentEditor::displayName() const
{
    return path_.isEmpty() ? untitledName_ : QFileInfo(path_).fileName();
}

bool DocumentEditor::loadFile(const QString& path)
{
    error_.clear();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error_ = file.errorString();
        return false;
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    const QString text = stream.readAll();
    if (stream.status() != QTextStream::Ok || file.error() != QFileDevice::NoError) {
        error_ = tr("Не удалось прочитать документ: %1").arg(file.errorString());
        return false;
    }
    setPlainText(text);
    path_ = QFileInfo(path).absoluteFilePath();
    document()->setModified(false);
    return true;
}

bool DocumentEditor::saveFile(const QString& path)
{
    error_.clear();
    // QSaveFile сначала пишет временный файл. commit заменяет оригинал
    // только после успешной записи, поэтому ошибка не обрежет старый файл.
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        error_ = file.errorString();
        return false;
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << toPlainText();
    stream.flush();
    if (stream.status() != QTextStream::Ok || !file.commit()) {
        error_ = tr("Не удалось записать документ: %1").arg(file.errorString());
        return false;
    }
    path_ = QFileInfo(path).absoluteFilePath();
    document()->setModified(false);
    return true;
}
