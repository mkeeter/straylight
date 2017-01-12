#include <cassert>

#include "syntax.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* doc)
    : QSyntaxHighlighter(doc)
{
    QTextCharFormat quote_format;
    quote_format.setForeground(Qt::red);

    // Strings on a single line
    // (with clever regex for escaped chars)
    rules << Rule(R"("(\\.|[^"\\])*")", quote_format, BASE, BASE);

    // Multi-line strings
    rules << Rule(R"("(\\.|[^"\\])*$)", quote_format, BASE, STRING);
    rules << Rule(R"(^(\\.|[^"\\])*")", quote_format, STRING, BASE);
    rules << Rule(R"(.+)", quote_format, STRING, STRING);
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    int offset = 0;
    int state = previousBlockState();

    while (offset <= text.length())
    {
        int match_start = -1;
        int match_length;
        Rule rule;

        for (auto r : rules)
        {
            if (r.state_in != state)
            {
                continue;
            }

            auto match = r.regex.match(text, offset);
            if (!match.hasMatch())
            {
                continue;
            }

            if (match_start == -1 || match.capturedStart(0) < match_start)
            {
                match_start = match.capturedStart(0);
                match_length = match.capturedLength(0);
                rule = r;
            }
        }

        if (match_start == -1)
        {
            break;
        }

        // Otherwise we end up in an infinite loop
        assert(match_length > 0);

        setFormat(match_start, match_length, rule.format);
        offset = match_start + match_length;
        state = rule.state_out;
    }

    setCurrentBlockState(state);
}
