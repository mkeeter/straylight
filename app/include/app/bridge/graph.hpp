#pragma once

#include <map>

#include "app/bridge/items.hpp"
#include "app/bridge/sheets.hpp"

#include "graph/types/env.hpp"
#include "graph/response.hpp"

namespace App {
namespace Bridge {

class GraphModel
{
public:
    /*
     *  Update the model state with the given response
     */
    void operator()(const Graph::Response& r);

protected:
    std::map<Graph::Env, std::unique_ptr<ItemsModel>> items;
    std::map<Graph::Env, std::unique_ptr<SheetsModel>> sheets;
};

}   // namespace Bridge
}   // namespace App
