#include <cstdlib>
#include <map>

#include "ui.hpp"
#include "datum.hpp"
#include "s7.h"

int main(int, char**)
{
    s7_scheme* s7 = s7_init();
    if (!s7)
    {
        fprintf(stderr, "Error: could not create s7\n");
        exit(1);
    }

    auto window = ui::init();

    std::map<std::string, Datum*> ds;
    ds["x"] = new Datum("(+ 1 2)", s7);
    ds["y"] = new Datum("(+ 3 4)", s7);

    // Main loop
    while (!ui::finished(window))
    {
        ui::draw(window, ds);
    }

    ui::shutdown();
    free(s7);

    return 0;
}
