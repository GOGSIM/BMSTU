#pragma once

#include <QStandardItemModel>

// QStandardItemModel уже реализует index(), parent(), data() и подсчёт строк.
// Он является потомком QAbstractItemModel, поэтому такое наследование
// соответствует методичке и примеру иерархической модели из лекции 3.
class XmlTreeModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit XmlTreeModel(QObject* parent = nullptr);
    // Обязательный по заданию конструктор: сразу загружает один документ.
    explicit XmlTreeModel(const QString& fileName, QObject* parent = nullptr);

    // При ошибке возвращает false; уже открытые файлы остаются в модели.
    bool appendFile(const QString& fileName);
    QString lastError() const;
    int findFile(const QString& fileName) const;
    static QString normalizedPath(const QString& fileName);

    // Собственная роль хранит путь отдельно от видимого имени файла.
    enum { FilePathRole = Qt::UserRole + 1 };

private:
    QString lastError_;
};

