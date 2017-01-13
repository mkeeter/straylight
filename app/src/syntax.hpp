#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    SyntaxHighlighter(QTextDocument* doc);
    static int matchedParen(QTextDocument* doc, int pos);

protected:
    static int searchLeft(QTextDocument* doc, int pos);
    static int searchRight(QTextDocument* doc, int pos);

    /*  Define states to keep track of multiline strings. */
    enum State { BASE = -1, STRING = 1 };

    /*  Structure to use as a rule when highlighting    */
    struct Rule
    {
        Rule() {}
        Rule(QString r, QTextCharFormat f, State si=BASE, State so=BASE)
            : regex(QRegularExpression(r)), format(f), state_in(si), state_out(so)
        { /* Nothing to do here */ }

        QRegularExpression regex;
        QTextCharFormat format;
        State state_in, state_out;
    };

    void buildRules();
    static QList<Rule> rules;

    void highlightBlock(const QString& text) override;
};
