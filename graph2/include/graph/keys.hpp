#pragma once

#include <string>

#include "graph/index.hpp"

/*  These keys are only valid for a particular Root instance */
typedef std::pair<SheetIndex, std::string> NameKey;
typedef std::pair<SheetIndex, ItemIndex> CellKey;
