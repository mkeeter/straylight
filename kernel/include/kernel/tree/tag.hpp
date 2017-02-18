#pragma once

namespace Kernel {

/*
 *  A Tag can be attached to a particular item in a tree
 */
class Tag
{
public:
    virtual ~Tag() {}
    virtual Tag* clone() const=0;
};

}   // namespace Kernel
