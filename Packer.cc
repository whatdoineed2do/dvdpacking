#ident "%W%"

/*  $Id$
 */

#pragma ident  "@(#)$Id$"


#include "Packer.h"

Bins  PackFirstFit::pack(const Items& items_, off_t binsz_, Items& unhandled_)
{
    /* add to first open bin that has capacity; if no bins open, create new bin and add
     */

    Bins  bins;
    for (Items::const_iterator i=items_.begin(); i!=items_.end(); ++i)
    {
        if (i->size() > binsz_) {
            unhandled_.push_back(*i);
            continue;
        }

        Bin*  b = NULL;
        for (Bins::iterator j = bins.begin(); j!=bins.end(); ++j) 
        {
            if (j->closed() ) {
                continue;
            }

            if (j->remain() > i->size()) {
                b = &(*j);
                break;
            }
        }

        if (b == NULL) {
            bins.push_back( Bin(binsz_) );
            b = &bins.back();
        }

        b->add(*i);
    }
    
    return bins;
}

Bins  PackFirstFirtDesc::pack(const Items& items_, off_t binsz_, Items& unhandled_)
{
    Items::value_type  l = items_.items();
    l.sort();
    return PackFirstFit::pack(Items(l), binsz_, unhandled_);
}

Bins  PackWorstFit::pack(const Items& items_, off_t binsz_, Items& unhandled_)
{
    /* add to open bin with MOST capacity; if no bins available, create new bin and add
     */

    Bins  bins;  // bin will be kept sorted, small -> largest

    for (Items::const_iterator i=items_.begin(); i!=items_.end(); ++i)
    {
        if (i->size() > binsz_) {
            unhandled_.push_back(*i);
            continue;
        }

        Bin*  b = NULL;
        for (Bins::reverse_iterator j = bins.rbegin(); j!=bins.rend(); ++j) 
        {
            if (j->closed() ) {
                continue;
            }

            if (j->remain() > i->size()) {
                b = &(*j);
                break;
            }
        }

        if (b == NULL) {
            bins.push_back( Bin(binsz_) );
            b = &bins.back();
        }

        b->add(*i);
        bins.sort();
    }
    
    return bins;
}

Bins  PackWorstFitDesc::pack(const Items& items_, off_t binsz_, Items& unhandled_)
{
    Items::value_type  l = items_.items();
    l.sort();
    return PackWorstFit::pack(Items(l), binsz_, unhandled_);
}


Bins  PackBestFit::pack(const Items& items_, off_t binsz_, Items& unhandled_)
{
    /* add to open bin with LEAST capacity; if no bins available, create new bin and add
     */

    Bins  bins;  // bin will be kept sorted, small -> largest

    for (Items::const_iterator i=items_.begin(); i!=items_.end(); ++i)
    {
        if (i->size() > binsz_) {
            unhandled_.push_back(*i);
            continue;
        }

        Bin*  b = NULL;
        for (Bins::iterator j = bins.begin(); j!=bins.end(); ++j) 
        {
            if (j->closed() ) {
                continue;
            }

            if (j->remain() > i->size()) {
                b = &(*j);
                break;
            }
        }

        if (b == NULL) {
            bins.push_back( Bin(binsz_) );
            b = &bins.back();
        }

        b->add(*i);
        bins.sort();
    }
    
    return bins;
}

Bins  PackBestFitDesc::pack(const Items& items_, off_t binsz_, Items& unhandled_)
{
    Items::value_type  l = items_.items();
    l.sort();
    return PackBestFit::pack(Items(l), binsz_, unhandled_);
}
