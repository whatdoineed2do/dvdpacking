#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <iomanip>

#include "Item.h"
#include "Items.h"
#include "Bin.h"
#include "Bins.h"
#include "Packer.h"
#include "dvdpacking.h"

const char*  argv0 = NULL;

const double  GIGABYTE = 1073741824.0;
const double  MEGABYTE = 1048576.0;


std::string _humansizes(off_t  size_)
{
    std::ostringstream  os;
    os << std::setprecision(4);
    if (size_ >= GIGABYTE) {
        os << (double)size_/GIGABYTE << "GiB";
    }
    else {
        os << (double)size_/MEGABYTE << "MiB";
    }
    return os.str();
}

void  _usage()
{
    LOG_INFO("usage: " << argv0);
    LOG_INFO("       -c <capacity>    - value in bytes of i\"dvd\" or \"cd\"");
    LOG_INFO("      [-p <algo>]       - packing algorithm: \"firstfit\" \"worstfit\" \"all\":  default=\"all\"");
    LOG_INFO("      [-r <reserve>]    - reserve number of bytes or % of capacity:  default=0");
    exit(1);
}

int main(int argc, char* argv[])
{
    const off_t  DVD_MAX = 4707319808;
    const off_t  CD_MAX  =     737280;
    off_t  capacity = DVD_MAX;
    long  reserve = 0;

    argv0 = basename(argv[0]);

    Packer*  bps[] = {
        new PackFirstFit(),
        new PackFirstFirtDesc(),
        new PackWorstFit(),
        new PackWorstFitDesc(),
        NULL
    };
    Packer*  bp = NULL;

    int c;
    while ( (c=getopt(argc, argv, "p:r:c:h")) != EOF) {
        switch (c)
        {
            case 'c':
            {
                capacity = 0;

                if (strcasecmp(optarg, "dvd") == 0)  capacity = DVD_MAX;
                if (strcasecmp(optarg, "cd")  == 0)  capacity = CD_MAX;

                if (capacity == 0) {
                    char*  ep = NULL;
                    long  val = strtol(optarg, &ep, 10);
                    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
                        LOG_ERR("unhandled capacity - '" << optarg << "' - " << strerror(errno));
                    }
                    else
                    {
                        if (ep == optarg) {
                            LOG_ERR("invalid capacity arg - '" << optarg << "'");
                        }
                        else 
                        {
                            if (*ep != NULL) {
                                LOG_ERR("chars included in capacity arg - '" << optarg << "'");
                            }
                            else
                            {
                                capacity = val;
                            }
                        }
                    }
                }
            } break;

            case 'p':
            {
                if (strcasecmp(optarg, "firstfit")     == 0)  bp = bps[0];
                if (strcasecmp(optarg, "firstfitdesc") == 0)  bp = bps[1];
                if (strcasecmp(optarg, "worstfit")     == 0)  bp = bps[2];
                if (strcasecmp(optarg, "worstfitdesc") == 0)  bp = bps[3];
            } break;

            case 'r':
            {
                char*  ep = NULL;
                long  val = strtol(optarg, &ep, 10);
                if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
                    LOG_ERR("unhandled reserve arg - '" << optarg << "' - " << strerror(errno));
                }
                else
                {
                    if (ep == optarg) {
                        LOG_ERR("invalid reserve arg - '" << optarg << "'");
                    }
                    else 
                    {
                        if (*ep != NULL) {
                            if (strcmp(ep, "%") == 0) {
                                reserve = -1*val;
                            }
                            else {
                                LOG_ERR("chars included in capacity arg - '" << optarg << "'");
                            }
                        }
                        else
                        {
                            reserve = val;
                        }
                    }
                }
            } break;

            case 'h':
            default:
                _usage();
        }
    }

    if (reserve < 0) {
        // calc % of capacity
        reserve = ((double)reserve/100.0)*capacity*-1.0;
    }

    capacity -= reserve;
    if (capacity <= 0) {
        LOG_ERR("capacity/reserve values invalid");
        _usage();
    }

    if (optind == argc) {
        LOG_WARN("no input files");
        _usage();
    }

    Items  items;
    int  a = optind;
    bool  allok = true;
    while (a < argc)
    {
        try
        {
            items.push_back(argv[a]);
            LOG_DEBUG(items.back().what() << ": " << items.back().size());
        }
        catch (const std::exception& ex)
        {
            LOG_ERR("failed to examine: " << ex.what());
            allok = false;
        }

        ++a;
    }

    if (!allok) {
        LOG_ERR("failed to process all requested inputs");
        return 2;
    }

    LOG_INFO("ttl items=" << items.count() << "  ttl size=" << items.size() << " (" << _humansizes(items.size()) << ")" << "  lower bounds=" << ceil((double)items.size()/(double)capacity));
    for (Items::const_iterator i=items.begin(); i!=items.end(); ++i) {
        LOG_INFO("  " << std::setw(10) << i->size() << "  " << i->what());
    }

    /* work through all the packing algos til we find the selected one .. if not
     * selected, it'll try them all
     */
    Packer**  pp = bps;
    while (*pp != NULL) 
    {
        if (bp == NULL || (bp && bp == *pp)) {
            Packer*  p = *pp;  
            Items  unhandled;
            Bins  bins = p->pack(items, capacity, unhandled);

            LOG_INFO("---");
            LOG_INFO(p->name() << " #bins=" << bins.count() << "  #unhandled=" << unhandled.count() << " size=" << unhandled.size() << " (" << _humansizes(unhandled.size()) << ")");
            if (!unhandled.empty()) {
                LOG_WARN("unhandled items = {");
                for (Items::const_iterator i=unhandled.begin(); i!=unhandled.end(); ++i) {
                    LOG_WARN("  " << std::setw(1) << i->size() << "  " << i->what());
                }
                LOG_WARN("}");
            }

            unsigned  i = 0;
            for (Bins::const_iterator  b=bins.begin(); b!=bins.end(); ++b, ++i) {
                LOG_INFO("  bin# " << i << "  size=" << b->size() << " (" << _humansizes(b->size()) << ") " << " remain=" << b->remain() << " (" << _humansizes(b->remain()) << ") " << " {");
                for (Items::const_iterator j=b->items().begin(); j!=b->items().end(); ++j) {
                    LOG_INFO("    \"" << j->what() << "\"");
                }
                LOG_INFO("  }");
            }
        }
        ++pp;
    }


    return 0;
}
