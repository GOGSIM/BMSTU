#pragma once
#include "SyntaxSettings.h"
#include <QSyntaxHighlighter>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QTextDocument* document);
    void setRules(const QList<HighlightRule>& rules);
protected:
    void highlightBlock(const QString& text) override;
private:
    QList<HighlightRule> rules_;
};
