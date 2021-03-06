#ifndef CONTAINER_H
#define CONTAINER_H

#pragma ident  "@(#)$Id$"

#include <sys/types.h>
#include <list>


template<class T> class Container
{
  public:
    typedef std::list<T>  value_type;
    typedef typename value_type::size_type  size_type;
    typedef typename value_type::iterator  iterator;
    typedef typename value_type::const_iterator  const_iterator;
    typedef typename value_type::reverse_iterator  reverse_iterator;
    typedef typename value_type::const_reverse_iterator  const_reverse_iterator;


    Container() : _size(0) { }
    Container(const Container& rhs_) : _c(rhs_._c), _size(rhs_._size) { }

    Container(const Container::value_type& c_) : _c(c_)
    {
        for (const_iterator i=_c.begin(); i!=_c.end(); ++i) {
            _size += i->size();
        }
    }


    size_type  count() const  { return _c.size(); }
    size_type  size()  const  { return _size; }
    bool       empty() const  { return _c.empty(); }


    void   push_back(const T& i_)  { _c.push_back(i_);  _size += i_.size(); }
    void  push_front(const T& i_)  { _c.push_front(i_); _size += i_.size(); }


    iterator  begin()  { return _c.begin(); }
    iterator  end()    { return _c.end(); }

    const_iterator  begin() const  { return _c.begin(); }
    const_iterator  end()   const  { return _c.end(); }

    reverse_iterator  rbegin()  { return _c.rbegin(); }
    reverse_iterator  rend()    { return _c.rend(); }

    const_reverse_iterator  rbegin() const  { return _c.rbegin(); }
    const_reverse_iterator  rend()   const  { return _c.rend(); }


    const T&  front() const { return _c.front(); }
    const T&  back()  const { return _c.back(); }

    T&  front() { return _c.front(); }
    T&  back()  { return _c.back(); }


    Container::value_type  items() const  { return _c; }

    void  sort()  { _c.sort(); }


  private:
    void operator=(const Container&);

    value_type  _c;
    off_t  _size;
};

#endif
