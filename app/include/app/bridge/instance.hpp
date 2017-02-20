#pragma once

#include "app/bridge/items.hpp"
#include "app/bridge/sheets.hpp"

#include "graph/response.hpp"

namespace App {
namespace Bridge {

class SheetInstanceModel : public QObject
{
public:
    SheetInstanceModel(QObject* parent=nullptr) : QObject(parent) {}
    void updateFrom(const Graph::Response& r);

protected:
    ItemsModel items;
    SheetsModel library;
};

}   // namespace Bridge
}   // namespace App
