#pragma once
#include <QList>
#include <QRegularExpression>
#include <QStringList>
#include <QTextCharFormat>

struct HighlightRule
{
    QRegularExpression pattern;
    QRegularExpression endPattern; // Непустой шаблон означает многострочное правило.
    QTextCharFormat format;
};

struct SyntaxDefinition
{
    QString id;
    QString name;
    QStringList extensions;
    QList<HighlightRule> rules;
};

// Стратегия отделяет формат хранения от потребителя настроек.
class SyntaxSettingsReader
{
public:
    virtual ~SyntaxSettingsReader() = default;
    virtual bool read(const QString& path, QList<SyntaxDefinition>& result,
                      QString& error) const = 0;
};

class XmlSyntaxSettingsReader : public SyntaxSettingsReader
{
public:
    bool read(const QString& path, QList<SyntaxDefinition>& result,
              QString& error) const override;
};
