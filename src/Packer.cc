#ident "%W%"

/*  $Id$
 */

#pragma ident  "@(#)$Id$"


#include "Packer.h"
#include <algorithm>


Bins  PackFirstFit::pack(const Items& items_, off_t binsz_, Items& unhandled_)
{
    /* add to first open bin that has capacity; if no bins open, create new bin and add
     */

    Bins  bins;
    std::all_of(items_.begin(), items_.end(), [&bins, &binsz_, &unhandled_](const Item& i) {
        if (i.size() > binsz_) {
            unhandled_.push_back(i);
            return true;
        }

        Bin*  b = nullptr;
        for (Bins::iterator j = bins.begin(); j!=bins.end(); ++j) 
        {
            if (j->closed() ) {
                continue;
            }

            if (j->remain() > i.size()) {
                b = &(*j);
                break;
            }
        }

        if (b == nullptr) {
            bins.emplace_back( std::move(Bin(binsz_)) );
            b = &bins.back();
        }

        b->add(i);
        return true;
    });
    
    return std::move(bins);
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

    std::all_of(items_.begin(), items_.end(), [&bins, &binsz_, &unhandled_](const Item& i) {
        if (i.size() > binsz_) {
            unhandled_.push_back(i);
            return true;
        }

        Bin*  b = nullptr;
        for (Bins::reverse_iterator j = bins.rbegin(); j!=bins.rend(); ++j) 
        {
            if (j->closed() ) {
                continue;
            }

            if (j->remain() > i.size()) {
                b = &(*j);
                break;
            }
        }

        if (b == nullptr) {
            bins.emplace_back( std::move(Bin(binsz_)) );
            b = &bins.back();
        }

        b->add(i);
        bins.sort();
        return true;
    });
    
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

    std::all_of(items_.begin(), items_.end(), [&bins, &binsz_, &unhandled_](const Item& i) {
        if (i.size() > binsz_) {
            unhandled_.push_back(i);
            return true;
        }

        Bin*  b = nullptr;
        for (Bins::iterator j = bins.begin(); j!=bins.end(); ++j) 
        {
            if (j->closed() ) {
                continue;
            }

            if (j->remain() > i.size()) {
                b = &(*j);
                break;
            }
        }

        if (b == nullptr) {
            bins.emplace_back( std::move(Bin(binsz_)) );
            b = &bins.back();
        }

        b->add(i);
        bins.sort();
        return true;
    });
    
    return bins;
}

Bins  PackBestFitDesc::pack(const Items& items_, off_t binsz_, Items& unhandled_)
{
    Items::value_type  l = items_.items();
    l.sort();
    return PackBestFit::pack(Items(l), binsz_, unhandled_);
}
