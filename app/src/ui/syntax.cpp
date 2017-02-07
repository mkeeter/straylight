#include <cassert>

#include "ui/syntax.hpp"
#include "ui/material.hpp"

#include "core/bridge.hpp"

namespace App {
namespace UI {

QList<SyntaxHighlighter::Rule> SyntaxHighlighter::rules;

////////////////////////////////////////////////////////////////////////////////

typedef std::pair<int, QString> MatchInfo;

class BlockData : public QTextBlockUserData
{
public:
    std::list<MatchInfo> data;
};

////////////////////////////////////////////////////////////////////////////////

int SyntaxHighlighter::searchLeft(QTextDocument* doc, int pos)
{
    int depth = 1;
    for(QTextBlock b = doc->findBlock(pos); b.isValid(); b=b.previous())
    {
        auto data = static_cast<BlockData*>(b.userData());
        if (!data)
        {
            continue;
        }
        const auto& parens = data->data;

        // Search left to find a right-parens match
        auto start = std::find_if(parens.rbegin(), parens.rend(),
                [=](const MatchInfo& val)
                { return val.first + b.position() < pos; });

        for (auto itr=start; itr != parens.rend(); ++itr)
        {
            depth += (itr->second == ")") - (itr->second == "(");
            if (depth == 0)
            {
                return itr->first + b.position();
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
        auto data = static_cast<BlockData*>(b.userData());
        if (!data)
        {
            continue;
        }
        const auto& parens = data->data;

        // Search right to find a left-parens match
        auto start = std::find_if(parens.begin(), parens.end(),
                [=](const MatchInfo& val)
                { return val.first + b.position() > pos; });

        for (auto itr=start; itr != parens.end(); ++itr)
        {
            depth += (itr->second == "(") - (itr->second == ")");
            if (depth == 0)
            {
                return itr->first + b.position();
            }
        }
    }
    return -1;
}

QPoint SyntaxHighlighter::matchedParen(QTextDocument* doc, int pos)
{
    auto block = doc->findBlock(pos);
    assert(block.isValid());

    auto data = static_cast<BlockData*>(block.userData());
    if (!data)
    {
        return {-1, -1};
    }

    const auto& parens = data->data;
    auto found = std::find_if(parens.begin(), parens.end(),
            [=](const MatchInfo& val)
            { return val.first + block.position() == pos ||
                     val.first + block.position() == pos - 1; });

    if (found == parens.end())
    {
        return {-1, -1};
    }

    auto p = found->first + block.position();
    if (found->second == ")")
    {
        return {p, searchLeft(doc, p)};
    }
    else if (found->second == "(")
    {
        return {p, searchRight(doc, p)};
    }

    assert(false);
    return {-1, -1};
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

            rules << Rule(R"(\b(?:-|)\d+\.\d*e\d+)", float_format);
            rules << Rule(R"(\b(?:-|)\d+\.\d*)", float_format);
            rules << Rule(R"(\b(?:-|)\d+e\d+)", float_format);
        }

        {   // Integers
            QTextCharFormat int_format;
            int_format.setForeground(Material::light_green_500);

            rules << Rule(R"(\b(?:-|)\d+\b)", int_format);
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
        for (const auto& k : App::Core::Bridge::singleton()->keywords())
        {
            auto esc = QRegularExpression::escape(QString::fromStdString(k));
            rules << Rule("\\b" + esc + "\\b", keyword_format);
        }
    }

    // Special regex to catch parentheses
    // The target capture group is 1
    rules << Rule(R"([^()]*(\(|\)))", QTextCharFormat(), BASE, BASE, 1);
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    int offset = 0;
    int state = previousBlockState();

    auto data = new BlockData;

    while (offset <= text.length())
    {
        Rule rule;
        QRegularExpressionMatch match;

        // Iterate over every rule, picking out the first match
        for (auto r : rules)
        {
            if (r.state_in != state)
            {
                continue;
            }

            auto m = r.regex.match(text, offset);
            if (!m.hasMatch())
            {
                continue;
            }

            if (!match.hasMatch() || m.capturedStart(r.capture) < match.capturedStart(rule.capture))
            {
                match = m;
                rule = r;
            }
        }

        if (!match.isValid() || !match.hasMatch())
        {
            offset++;
            continue;
        }

        const auto start = match.capturedStart(rule.capture);
        const auto length = match.capturedLength(rule.capture);

        // Otherwise we end up in an infinite loop
        assert(length > 0);
        setFormat(start, length, rule.format);
        offset = start + length;
        state = rule.state_out;

        for (int i=1; i <= match.lastCapturedIndex(); ++i)
        {
            data->data.push_back({match.capturedStart(i), match.captured(i)});
        }
    }

    setCurrentBlockState(state);
    setCurrentBlockUserData(data);
}

}   // namespace UI
}   // namespace App
