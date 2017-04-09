#include "graph/graph.hpp"

namespace Graph
{

const std::list<std::pair<std::string, std::string>> Graph::_bad_item_names = {
        {"^$", "Name cannot be empty"},
        {"^[A-Z].*$", "First character must be lowercase"}};
const std::list<std::pair<std::string, std::string>> Graph::_bad_sheet_names = {
        {"^$", "Name cannot be empty"},
        {"^[a-z].*$", "First character must be uppercase"}};

const std::string Graph::_good_item_name = "^[a-z][A-Za-z0-9\\-_]*$";
const std::string Graph::_good_sheet_name = "^[A-Z][A-Za-z0-9\\-_]*$";

Graph::Graph()
    : good_item_name(_good_item_name), good_sheet_name(_good_sheet_name)
{
    // Initialize the dirty stack
    dirty.push({});
}

}   // namespace Graph
