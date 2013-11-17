
#include "Highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywordPatterns;
    keywordPatterns << "\\bselect\\b"
                    << "\\bfrom\\b"
                    << "\\bwhere\\b"
                    << "\\bgroup by\\b"
                    << "\\having\\b"
                    << "\\border by\\b"
                    << "\\basc\\b"
                    << "\\bdesc\\b"
                    << "\\blike\\b"
                    << "\\bnull\\b"
                    << "\\bbetween\\b"
                    << "\\bin\\b"
                    << "\\bjoin\\b"
                    << "\\bexists\\b"
                    << "\\binner\\b"
                    << "\\bleft\\b"
                    << "\\bright\\b"
                    << "\\binsert\\b"
                    << "\\binto\\b"
                    << "\\bdelete\\b"
                    << "\\bupdate\\b"
                    << "\\bset\\b"
                    << "\\bcommit\\b"
                    << "\\brollback\\b"
                    << "\\bas\\b"
                    << "\\bbegin\\b"
                    << "\\btransaction\\b";

    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern, Qt::CaseInsensitive);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    QTextCharFormat oneQuoteFormat;
    oneQuoteFormat.setForeground(Qt::magenta);
    rule.pattern = QRegExp("\".*\"");
    rule.pattern.setMinimal(true);
    rule.format = oneQuoteFormat;
    highlightingRules.append(rule);

    QTextCharFormat twoQuoteFormat;
    twoQuoteFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\'.*\'");
    rule.pattern.setMinimal(true);
    rule.format = twoQuoteFormat;
    highlightingRules.append(rule);

    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::green);
    rule.pattern = QRegExp("--.*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = text.indexOf(expression);

        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = text.indexOf(expression, index + length);
        }
    }
} 
