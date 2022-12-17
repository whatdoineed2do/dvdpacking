#ifndef PACKER_H
#define PACKER_H

#include <sys/types.h>

#include "Items.h"
#include "Bins.h"
#include "dvdpacking.h"

class Packer
{
  public:
    virtual ~Packer() = default;

    Packer(const Packer&)  = delete;
    Packer(const Packer&&) = delete;

    void operator=(const Packer&)  = delete;
    void operator=(const Packer&&) = delete;

    /* it may be the case that some items are too big for the bin size so
     * need to add such items to the 'unhandled' item
     */
    virtual Bins  pack(const Items&, off_t binsz_, Items& unhandled_) = 0;

    const char*  name() const { return _name; }

  protected:
    Packer(const char* name_) : _name(name_) { }

    const char*  _name;
};


class PackFirstFit : public Packer
{
  public:
    PackFirstFit() : Packer("First Fit")  { }
    virtual ~PackFirstFit() = default;

    virtual Bins  pack(const Items&, off_t binsz_, Items& unhandled_);

  protected:
    PackFirstFit(const char* n_) : Packer(n_)  { }
};

struct PackFirstFirtDesc : public PackFirstFit
{
    PackFirstFirtDesc() : PackFirstFit("First Fit Descending")  { }
    ~PackFirstFirtDesc() = default;
    Bins  pack(const Items&, off_t binsz_, Items& unhandled_);
};

class PackWorstFit : public Packer
{
  public:
    PackWorstFit() : Packer("Worst Fit")  { }
    virtual ~PackWorstFit() = default;
    virtual Bins  pack(const Items&, off_t binsz_, Items& unhandled_);

  protected:
    PackWorstFit(const char* n_) : Packer(n_) { }
};

struct PackWorstFitDesc : public PackWorstFit
{
    PackWorstFitDesc() : PackWorstFit("Worst Fit Descending")  { }
    ~PackWorstFitDesc() = default;
    Bins  pack(const Items&, off_t binsz_, Items& unhandled_);
};


class PackBestFit : public Packer
{
  public:
    PackBestFit() : Packer("Best Fit")  { }
    virtual ~PackBestFit() = default;
    virtual Bins  pack(const Items&, off_t binsz_, Items& unhandled_);

  protected:
    PackBestFit(const char* n_) : Packer(n_) { }
};

struct PackBestFitDesc : public PackBestFit
{
    PackBestFitDesc() : PackBestFit("Best Fit Descending")  { }
    ~PackBestFitDesc() = default;
    Bins  pack(const Items&, off_t binsz_, Items& unhandled_);
};


#endif
