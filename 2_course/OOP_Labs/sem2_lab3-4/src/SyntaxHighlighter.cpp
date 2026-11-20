#include "SyntaxHighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* document)
    : QSyntaxHighlighter(document) {}

void SyntaxHighlighter::setRules(const QList<HighlightRule>& rules)
{
    rules_ = rules;
    rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    setCurrentBlockState(0);
    // Единый проход по токенам не принимает // внутри строки за комментарий.
    // При одинаковой позиции более позднее правило XML имеет приоритет.
    int position = 0;
    int active = previousBlockState() > 0 ? previousBlockState() - 1 : -1;
    while (position < text.size() || active >= 0) {
        if (active >= 0 && active < rules_.size()) {
            const auto& rule = rules_.at(active);
            const auto end = rule.endPattern.match(text, position);
            const int finish = end.hasMatch() ? end.capturedEnd() : text.size();
            setFormat(position, finish - position, rule.format);
            if (!end.hasMatch()) {
                setCurrentBlockState(active + 1);
                return;
            }
            position = qMax(position + 1, finish);
            active = -1;
            continue;
        }
        int selected = -1;
        QRegularExpressionMatch first;
        for (int i = 0; i < rules_.size(); ++i) {
            const auto match = rules_.at(i).pattern.match(text, position);
            if (match.hasMatch() && (selected < 0 || match.capturedStart() <= first.capturedStart())) {
                selected = i;
                first = match;
            }
        }
        if (selected < 0) break;
        const auto& rule = rules_.at(selected);
        const int start = first.capturedStart();
        int finish = first.capturedEnd();
        if (!rule.endPattern.pattern().isEmpty()) {
            const auto end = rule.endPattern.match(text, finish);
            finish = end.hasMatch() ? end.capturedEnd() : text.size();
            if (!end.hasMatch()) setCurrentBlockState(selected + 1);
        }
        setFormat(start, finish - start, rule.format);
        position = qMax(start + 1, finish); // Даже нулевая длина regex не зациклит проход.
    }
}
