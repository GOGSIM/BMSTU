#include "SyntaxSettings.h"
#include <QColor>
#include <QDomDocument>
#include <QFile>
#include <QFont>
#include <QSet>

bool XmlSyntaxSettingsReader::read(const QString& path, QList<SyntaxDefinition>& result,
                                 QString& error) const
{
    error.clear();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        error = file.errorString();
        return false;
    }
    QDomDocument xml;
    const auto parsed = xml.setContent(&file);
    if (!parsed) {
        error = QString("XML: %1, строка %2, столбец %3")
                    .arg(parsed.errorMessage).arg(parsed.errorLine).arg(parsed.errorColumn);
        return false;
    }
    const auto root = xml.documentElement();
    const auto syntaxes = root.firstChildElement("syntaxes");
    if (root.tagName() != "settings" || syntaxes.isNull()) {
        error = "Ожидаются settings и syntaxes";
        return false;
    }
    QList<SyntaxDefinition> definitions;
    QSet<QString> ids, extensions;
    // Настройки публикуются только после проверки всего файла.
    for (auto syntax = syntaxes.firstChildElement("syntax"); !syntax.isNull();
         syntax = syntax.nextSiblingElement("syntax")) {
        SyntaxDefinition definition;
        definition.id = syntax.attribute("id").trimmed();
        definition.name = syntax.attribute("name").trimmed();
        definition.extensions = syntax.attribute("ext_list").toLower().split(
            QRegularExpression("[;,\\s]+"), Qt::SkipEmptyParts);
        if (definition.id.isEmpty() || ids.contains(definition.id)
            || definition.extensions.isEmpty()) {
            error = "Пустой или повторный id, либо пустой ext_list";
            return false;
        }
        ids.insert(definition.id);
        for (QString& extension : definition.extensions) {
            if (extension.startsWith('.')) extension.remove(0, 1);
            if (extension.isEmpty() || extensions.contains(extension)) {
                error = "Пустое или повторное расширение: " + extension;
                return false;
            }
            extensions.insert(extension);
        }
        for (auto rule = syntax.firstChildElement("rule"); !rule.isNull();
             rule = rule.nextSiblingElement("rule")) {
            HighlightRule value;
            const auto pattern = rule.firstChildElement("pattern");
            const auto format = rule.firstChildElement("format");
            value.pattern = QRegularExpression(pattern.attribute("value"));
            const QString end = pattern.attribute("end");
            value.endPattern = QRegularExpression(end);
            const QColor color(format.attribute("foreground"));
            if (pattern.isNull() || value.pattern.pattern().isEmpty()
                || !value.pattern.isValid() || !value.endPattern.isValid() || !color.isValid()) {
                error = "Неверный шаблон или цвет в синтаксисе " + definition.id;
                return false;
            }
            value.format.setForeground(color);
            if (format.hasAttribute("font_weight")) {
                bool ok = false;
                int weight = format.attribute("font_weight").toInt(&ok);
                if (!ok || weight < 0 || weight > 1000) {
                    error = "Неверный font_weight";
                    return false;
                }
                // Пример методички использует шкалу Qt5 (Normal=50, Bold=75).
                if (weight <= 99) {
                    if (weight <= 12) weight = QFont::Thin;
                    else if (weight <= 25) weight = QFont::ExtraLight;
                    else if (weight <= 50) weight = weight < 50 ? QFont::Light : QFont::Normal;
                    else if (weight <= 57) weight = QFont::Medium;
                    else if (weight <= 63) weight = QFont::DemiBold;
                    else if (weight <= 75) weight = QFont::Bold;
                    else if (weight <= 81) weight = QFont::ExtraBold;
                    else weight = QFont::Black;
                }
                value.format.setFontWeight(weight);
            }
            value.format.setFontItalic(format.attribute("font_italic") == "true");
            definition.rules.append(value);
        }
        if (definition.rules.isEmpty()) {
            error = "Нет правил в синтаксисе " + definition.id;
            return false;
        }
        definitions.append(definition);
    }
    if (definitions.isEmpty()) {
        error = "Не определён ни один синтаксис";
        return false;
    }
    result = definitions;
    return true;
}
