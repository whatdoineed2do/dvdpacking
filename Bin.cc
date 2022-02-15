#ident "%W%"

/*  $Id$
 */

#pragma ident  "@(#)$Id$"


#include "Bin.h"

#include <iostream>
#include <sstream>


Bin& Bin::add(const Item& i_)
{
    if (_closed) {
        throw std::overflow_error("bin closed");
    }

    if (_remain < i_.size()) {
        std::ostringstream  err;
        err << "insufficient space - " << i_.what() << " size=" << i_.size() << " remain=" << _remain;
        throw std::underflow_error(err.str());
    }

    _remain -= i_.size();
    _items.push_back(i_);

    if (_remain == 0) {
        _closed = true;
    }

    return *this;
}


std::ostream&  operator<<(std::ostream& os_, const Bin& obj_)
{
    os_ << "size=" << obj_.max() << " remain=" << obj_.remain() << " { ";
    for (Items::const_iterator i=obj_.items().begin(); i!=obj_.items().end(); ++i)
    {
        os_ << "\"" << i->what() << "\" ";
    }
    return os_ << " }";
}
