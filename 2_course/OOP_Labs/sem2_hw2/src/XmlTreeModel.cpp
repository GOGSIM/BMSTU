#include "XmlTreeModel.h"

#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <memory>

namespace
{
// Вспомогательные функции скрыты в этом файле, чтобы не расширять интерфейс
// класса. Данные не редактируются: приложение предназначено для просмотра.
QStandardItem* makeItem(const QString& text)
{
    QStandardItem* item = new QStandardItem(text);
    item->setEditable(false);
    item->setToolTip(text);
    return item;
}

QString displayText(const QString& text)
{
    return text.trimmed().isEmpty() ? QStringLiteral("Нет данных") : text.trimmed();
}

void appendField(QStandardItem* parent, const QString& label, const QString& value)
{
    parent->appendRow({makeItem(label), makeItem(displayText(value))});
}

QString field(const QDomElement& organization, const QString& name)
{
    // Читаем именно непосредственный дочерний элемент известной структуры.
    return organization.firstChildElement(name).text().trimmed();
}
}

XmlTreeModel::XmlTreeModel(QObject* parent)
    : QStandardItemModel(parent)
{
    setHorizontalHeaderLabels({QStringLiteral("Поле"), QStringLiteral("Значение")});
}

XmlTreeModel::XmlTreeModel(const QString& fileName, QObject* parent)
    : XmlTreeModel(parent) // Делегирование конструктора не повторяет настройку.
{
    appendFile(fileName);
}

QString XmlTreeModel::normalizedPath(const QString& fileName)
{
    const QFileInfo info(fileName);
    const QString canonical = info.canonicalFilePath();
    return canonical.isEmpty() ? info.absoluteFilePath() : canonical;
}

int XmlTreeModel::findFile(const QString& fileName) const
{
    const QString path = normalizedPath(fileName);
    for (int row = 0; row < rowCount(); ++row) {
        const QString storedPath = item(row)->data(FilePathRole).toString();
#ifdef Q_OS_WIN
        // Windows обычно не различает регистр букв в пути.
        if (storedPath.compare(path, Qt::CaseInsensitive) == 0) {
#else
        if (storedPath == path) {
#endif
            return row;
        }
    }
    return -1;
}

QString XmlTreeModel::lastError() const
{
    return lastError_;
}

bool XmlTreeModel::appendFile(const QString& fileName)
{
    lastError_.clear();
    if (findFile(fileName) >= 0) {
        return true;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        lastError_ = tr("Не удалось открыть файл «%1»:\n%2")
                         .arg(fileName, file.errorString());
        return false;
    }

    // DOM загружает XML целиком в дерево узлов в памяти. В отличие от
    // потокового разбора здесь можно обращаться к дочерним элементам по имени.
    QDomDocument document;
    const QDomDocument::ParseResult result = document.setContent(&file);
    if (!result) {
        lastError_ = tr("Ошибка XML в файле «%1», строка %2, столбец %3:\n%4")
                         .arg(fileName).arg(result.errorLine)
                         .arg(result.errorColumn).arg(result.errorMessage);
        return false;
    }

    const QDomElement root = document.documentElement();
    if (root.tagName() != QStringLiteral("catering")) {
        lastError_ = tr("Неподдерживаемая структура файла «%1».\n"
                        "Ожидается корень <catering> и записи <organization>.")
                         .arg(fileName);
        return false;
    }
    // Иначе XML с произвольными дочерними узлами выглядел бы как пустой набор.
    for (QDomElement element = root.firstChildElement(); !element.isNull();
         element = element.nextSiblingElement()) {
        if (element.tagName() != QStringLiteral("organization")) {
            lastError_ = tr("Неподдерживаемый элемент <%1> в файле «%2».")
                             .arg(element.tagName(), fileName);
            return false;
        }
    }

    // unique_ptr владеет ещё не добавленным поддеревом. До успешного
    // завершения разбора модель не меняется. После release() владельцем
    // корневого элемента и всех его детей становится QStandardItemModel.
    std::unique_ptr<QStandardItem> fileItem(makeItem(QFileInfo(fileName).fileName()));
    fileItem->setData(normalizedPath(fileName), FilePathRole);
    fileItem->setToolTip(normalizedPath(fileName));
    int organizationCount = 0;
    for (QDomElement organization = root.firstChildElement("organization");
         !organization.isNull();
         organization = organization.nextSiblingElement("organization")) {
        QStandardItem* nameItem = makeItem(tr("Название организации"));
        fileItem->appendRow({nameItem, makeItem(displayText(field(organization, "Name")))});
        appendField(nameItem, tr("Административный округ"), field(organization, "AdmArea"));
        appendField(nameItem, tr("Район"), field(organization, "District"));
        appendField(nameItem, tr("Адрес"), field(organization, "Address"));

        // В XML может быть несколько телефонов. Пустые элементы пропускаем,
        // а номера объединяем, сохраняя все доступные значения.
        QStringList phones;
        const QDomElement phoneGroup = organization.firstChildElement("PublicPhone");
        for (QDomElement phone = phoneGroup.firstChildElement("phone");
             !phone.isNull(); phone = phone.nextSiblingElement("phone")) {
            if (!phone.text().trimmed().isEmpty()) {
                phones.append(phone.text().trimmed());
            }
        }
        appendField(nameItem, tr("Телефон"), phones.join("; "));
        appendField(nameItem, tr("Кол-во посадочных мест"), field(organization, "SeatsCount"));
        QStandardItem* geoItem = makeItem(tr("Геоданные"));
        nameItem->appendRow({geoItem, makeItem(QString())});
        appendField(geoItem, tr("Широта"), field(organization, "Latitude_WGS84"));
        appendField(geoItem, tr("Долгота"), field(organization, "Longitude_WGS84"));
        ++organizationCount;
    }

    appendRow({fileItem.release(), makeItem(tr("Организаций: %1").arg(organizationCount))});
    return true;
}
