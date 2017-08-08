#ifndef BIN_H
#define BIN_H

#pragma ident  "@(#)$Id$"

#include <stdexcept>
#include <iosfwd>

#include "Item.h"
#include "Items.h"


class Bin
{
  public:
    Bin(const off_t  max_) : _max(max_), _remain(_max), _closed(false) { }
    Bin(const Bin& rhs_)   : _max(rhs_._max), _remain(rhs_._remain), _items(rhs_._items), _closed(rhs_._closed) { }

    off_t  max() const { return _max; }
    off_t  remain() const { return _remain; }
    off_t  size()  const  { return _max - _remain; }

    Bin&  add(const Item&)  throw (std::underflow_error,  /* adding when not sufficient space */ 
                                   std::overflow_error    /* adding when closed*/
                                  );

    bool  closed() const { return _closed; }
    void  close() { _closed = true; }

    const Items&  items() const  { return _items; }

    bool  operator<(const Bin& rhs_)
    { return size() < rhs_.size(); }


  private:
    void operator=(const Bin&);

    const off_t  _max;
    off_t        _remain;

    Items  _items;
    bool  _closed;
};

std::ostream&  operator<<(std::ostream&, const Bin&);

#endif
