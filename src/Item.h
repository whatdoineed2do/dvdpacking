#ifndef ITEM_H
#define ITEM_H

#pragma ident  "@(#)$Id$"

#include <sys/types.h>

#include <stdexcept>
#include <string>


/* represents a dir or file that will be packed
 */

class Item
{
  public:
    Item(const char* what_);
    Item(const Item&);

    Item(const Item&& rhs_) : _what(std::move(rhs_._what)), _size(rhs_._size)
    { }

    void operator=(const Item&)  = delete;
    void operator=(const Item&&) = delete;


    const char*  what() const  { return _what.c_str(); }
    const off_t  size() const  { return _size; }

    bool  operator<(const Item& rhs_)  { return _size < rhs_._size; }


  private:
    const std::string  _what;
    off_t  _size; 

    off_t  _du(const char* where_) const;
};

#endif
