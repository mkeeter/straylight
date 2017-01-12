#include <cassert>

#include "syntax.hpp"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* doc)
    : QSyntaxHighlighter(doc)
{
    {   // Strings (single and multi-line)
        QTextCharFormat string_format;
        string_format.setForeground(Qt::red);

        // Strings on a single line
        // (with clever regex for escaped chars)
        rules << Rule(R"("(\\.|[^"\\])*")", string_format, BASE, BASE);

        // Multi-line strings
        rules << Rule(R"("(\\.|[^"\\])*$)", string_format, BASE, STRING);
        rules << Rule(R"(^(\\.|[^"\\])*")", string_format, STRING, BASE);
        rules << Rule(R"(.+)", string_format, STRING, STRING);
    }

    {   // Numbers (float and integer), with the help of a magical
        // string that can be prepended to a regex to make it detect negative
        // numbers (but not subtraction).  Note that a closing parenthesis is
        // needed and the desired number is the last match group.
        QString neg = R"((^|\*\*|[(+\-=*\/,\[])([+\-\s]*)";

        {   // All the possible float formats
            QTextCharFormat float_format;
            float_format.setForeground(Qt::green);

            rules << Rule(neg + R"(\b\d+\.\d*e\d+))", float_format);
            rules << Rule(neg + R"(\b\d+\.\d*))", float_format);
            rules << Rule(neg + R"(\b\d+e\d+))", float_format);
        }

        {   // Integers
            QTextCharFormat int_format;
            int_format.setForeground(Qt::blue);

            rules << Rule(neg + R"(\b\d+\b))", int_format);
        }
    }

    {   // Comments!
        QTextCharFormat comment_format;
        comment_format.setForeground(Qt::gray);

        rules << Rule(R"(\;.*)", comment_format);
    }
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
