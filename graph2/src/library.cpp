#include "graph/library.hpp"

Library::Library()
{
    sheets.insert({{0}, {}});
}

bool Library::canInsert(NameKey key) const
{
    // Require that the sheet exists and that the name isn't taken
    return key.second.size() &&
           sheets.find(key.first) != sheets.end() &&
           names.left.find(key) == names.left.end();
}

SheetIndex Library::insert(std::string name, SheetIndex parent)
{
    NameKey nk = {parent, name};
    assert(canInsert(nk));

    unsigned next = sheets.rbegin()->first.i + 1;
    SheetIndex out = {next};

    sheets.insert({out, {}});
    names.insert({nk, out});

    return out;
}

void Library::rename(SheetIndex s, std::string new_name)
{
    auto prev = names.right.at(s);
    auto parent = prev.first;
    assert(canInsert({parent, new_name}));

    names.left.erase(prev);
    names.insert({{parent, new_name}, s});
}

void Library::erase(SheetIndex s)
{
    auto nk = names.right.find(s);
    auto k = sheets.find(s);

    assert(nk != names.right.end());
    assert(k != sheets.end());

    sheets.erase(k);
    names.right.erase(nk);
}

std::list<SheetIndex> Library::iterSheets(SheetIndex parent) const
{
    auto itr = names.left.upper_bound(std::make_pair(parent, ""));

    std::list<SheetIndex> out;
    while (itr != names.left.end() && itr->first.first == parent)
    {
        out.push_back((itr++)->second);
    }
    return out;
}
