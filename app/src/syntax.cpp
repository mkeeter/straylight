#include <cassert>

#include "bridge.hpp"
#include "syntax.hpp"
#include "material.hpp"

QList<SyntaxHighlighter::Rule> SyntaxHighlighter::rules;

////////////////////////////////////////////////////////////////////////////////

typedef std::pair<int, char> ParenthesisInfo;

class BlockData : public QTextBlockUserData
{
public:
    std::list<ParenthesisInfo> parens;
};

////////////////////////////////////////////////////////////////////////////////

int SyntaxHighlighter::searchLeft(QTextDocument* doc, int pos)
{
    int depth = 1;
    for(QTextBlock b = doc->findBlock(pos); b.isValid(); b=b.previous())
    {
        auto data = static_cast<BlockData*>(doc->findBlock(pos).userData());
        if (!data)
        {
            continue;
        }
        const auto& parens = data->parens;

        // Search left to find a right-parens match
        auto start = std::find_if(parens.rbegin(), parens.rend(),
                [=](const ParenthesisInfo& val) { return val.first < pos; });

        for (auto itr=start; itr != parens.rend(); ++itr)
        {
            depth += (itr->second == ')') - (itr->second == '(');
            if (depth == 0)
            {
                return itr->first;
            }
        }
    }
    return -1;
}

int SyntaxHighlighter::searchRight(QTextDocument* doc, int pos)
{
    int depth = 1;
    for(QTextBlock b = doc->findBlock(pos); b.isValid(); b=b.next())
    {
        auto data = static_cast<BlockData*>(doc->findBlock(pos).userData());
        if (!data)
        {
            continue;
        }
        const auto& parens = data->parens;

        // Search right to find a left-parens match
        auto start = std::find_if(parens.begin(), parens.end(),
                [=](const ParenthesisInfo& val) { return val.first > pos; });

        for (auto itr=start; itr != parens.end(); ++itr)
        {
            depth += (itr->second == '(') - (itr->second == ')');
            if (depth == 0)
            {
                return itr->first;
            }
        }
    }
    return -1;
}

int SyntaxHighlighter::matchedParen(QTextDocument* doc, int pos)
{
    auto block = doc->findBlock(pos);
    assert(block.isValid());

    auto data = static_cast<BlockData*>(block.userData());
    if (!data)
    {
        return -1;
    }

    const auto& parens = data->parens;
    auto found = std::find_if(parens.begin(), parens.end(),
            [=](const ParenthesisInfo& val) { return val.first == pos; });

    if (found == parens.end())
    {
        return -1;
    }
    else if (found->second == ')')
    {
        return searchLeft(doc, pos);
    }
    else if (found->second == '(')
    {
        return searchRight(doc, pos);
    }

    assert(false);
    return -1;
}

////////////////////////////////////////////////////////////////////////////////

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* doc)
    : QSyntaxHighlighter(doc)
{
    if (rules.empty())
    {
        buildRules();
    }
}

void SyntaxHighlighter::buildRules()
{
    {   // Strings (single and multi-line)
        QTextCharFormat string_format;
        string_format.setForeground(Material::orange_500);

        // Strings on a single line
        // (with clever regex for escaped chars)
        rules << Rule(R"("(?:\\.|[^"\\])*")", string_format, BASE, BASE);

        // Multi-line strings
        rules << Rule(R"("(?:\\.|[^"\\])*$)", string_format, BASE, STRING);
        rules << Rule(R"(^(?:\\.|[^"\\])*")", string_format, STRING, BASE);
        rules << Rule(R"(.+)", string_format, STRING, STRING);
    }

    {   // Numbers (float and integer)
        {   // All the possible float formats
            QTextCharFormat float_format;
            float_format.setForeground(Material::green_500);

            rules << Rule(R"((?:-|)\d+\.\d*e\d+)", float_format);
            rules << Rule(R"((?:-|)\d+\.\d*)", float_format);
            rules << Rule(R"((?:-|)\d+e\d+)", float_format);
        }

        {   // Integers
            QTextCharFormat int_format;
            int_format.setForeground(Material::light_green_500);

            rules << Rule(R"((?:-|)\d+\b)", int_format);
        }
    }

    {   // Comments!
        QTextCharFormat comment_format;
        comment_format.setForeground(Material::grey_500);

        rules << Rule(R"(\;.*)", comment_format);
    }

    {
        QTextCharFormat keyword_format;
        keyword_format.setForeground(Material::blue_500);
        for (const auto& k : Bridge::singleton()->keywords())
        {
            rules << Rule(QRegularExpression::escape(QString::fromStdString(k)),
                          keyword_format);
        }
    }
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    int offset = 0;
    int state = previousBlockState();

    auto parens = new BlockData;

    while (offset <= text.length())
    {
        if (state == BASE && text[offset] == '(')
        {
            parens->parens.push_back({offset++, '('});
        }
        else if (state == BASE && text[offset] == ')')
        {
            parens->parens.push_back({offset++, ')'});
        }
        else
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
    }

    setCurrentBlockState(state);
    setCurrentBlockUserData(parens);
}
