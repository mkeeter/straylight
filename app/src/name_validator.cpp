#include "name_validator.hpp"

bool NameValidator::checkName(QString name) const
{
    return name.count("x");
}
