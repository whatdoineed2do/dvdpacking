#include <sys/stat.h>
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
    LOG_INFO("      [-p <algo>]       - packing algorithm: \"firstfit{desc}\" \"worstfit{desc}\" \"bestfit{desc}\" \"all\":  default=\"all\"");
    LOG_INFO("      [-r <reserve>]    - reserve number of bytes or % of capacity:  default=0");
    LOG_INFO("      [-o <output base>]- create dirs representing each packing algorithm and symlinks to the bin contents");
    exit(1);
}

int main(int argc, char* argv[])
{
    const off_t  DVD_MAX = 4707319808;
    const off_t  CD_MAX  =     737280;
    off_t  capacity = DVD_MAX;
    long  reserve = 0;
    char*  output = NULL;

    argv0 = basename(argv[0]);

    struct {
        PackFirstFit       ff;
        PackFirstFirtDesc  ffd;
        PackWorstFit       wf;
        PackWorstFitDesc   wfd;
        PackBestFit        bf;
        PackBestFitDesc    bfd;
    } allbps;
 
    Packer*  bps[] = {
        &allbps.ff,
        &allbps.ffd,
        &allbps.wf,
        &allbps.wfd,
        &allbps.bf,
        &allbps.bfd,
        NULL
    };
    Packer*  bp = NULL;

    int c;
    while ( (c=getopt(argc, argv, "p:r:c:o:h")) != EOF) {
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
                if (strcasecmp(optarg, "firstfit")     == 0)  bp = bps[0];  else
                if (strcasecmp(optarg, "firstfitdesc") == 0)  bp = bps[1];  else
                if (strcasecmp(optarg, "worstfit")     == 0)  bp = bps[2];  else
                if (strcasecmp(optarg, "worstfitdesc") == 0)  bp = bps[3];  else
                if (strcasecmp(optarg, "bestfit")      == 0)  bp = bps[4];  else
                if (strcasecmp(optarg, "bestfitdesc")  == 0)  bp = bps[5];
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
                                LOG_ERR("chars included in reserve arg - '" << optarg << "'");
                            }
                        }
                        else
                        {
                            reserve = val;
                        }
                    }
                }
            } break;

            case 'o':
                output = optarg;
            break;

            case 'h':
            default:
                _usage();
        }
    }

    if (output)
    {
        if (access(output, W_OK) < 0) {
            LOG_ERR("invalid output location: '" << output << "' - " << strerror(errno));
            _usage();
        }
        
        struct stat  st;
        if (stat(output, &st) == 0 && S_ISDIR(st.st_mode) == 0) {
            LOG_ERR("invalid output location: '" << output << "' - is not a directory");
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

    const mode_t  umsk = umask(0);
    umask(umsk);

    char  path[PATH_MAX+1];
    char  rpath[PATH_MAX+1];
    char*  ppath = NULL;

    int  ret = 0;
    
    std::list<std::pair<unsigned, std::string> >  summaries;
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

            if (output) {
                sprintf(path, "%s/%s", output, p->name());
                if (mkdir(path, 0777 & ~umsk) < 0) {
                    LOG_ERR("unable to create output dir: " << path << " - " << strerror(errno));
                    ppath = NULL;
                    ret = 4;
                }
                else {
                    ppath = path + strlen(path);
                    *ppath++ = '/';
                }
            }

            summaries.push_back(std::make_pair(bins.count(), p->name()));

            if (!unhandled.empty()) {
                LOG_WARN("unhandled items = {");
                for (Items::const_iterator i=unhandled.begin(); i!=unhandled.end(); ++i) {
                    LOG_WARN("  " << std::setw(1) << i->size() << "  " << i->what());
                }
                LOG_WARN("}");
                ret = 3;
            }

            unsigned  i = 0;
            for (Bins::const_iterator  b=bins.begin(); b!=bins.end(); ++b, ++i) 
            {
                LOG_INFO("  bin# " << i << "  size=" << b->size() << " (" << _humansizes(b->size()) << ") " << " remain=" << b->remain() << " (" << _humansizes(b->remain()) << ") " << " {");

                char*  bpath = ppath;
                if (ppath) {
                    sprintf(bpath, "bin%02d/", i);
                    if (mkdir(path, 0777 & ~umsk) < 0) {
                        LOG_ERR("unable to create output bin dir: " << path << " - " << strerror(errno));
                        ppath = NULL;
                        ret = 4;
                    }
                    else {
                        bpath = path + strlen(path);
                    }
                }

                for (Items::const_iterator j=b->items().begin(); j!=b->items().end(); ++j)
                {
                    LOG_INFO("    \"" << j->what() << "\"");

                    if (ppath) {
                        if (realpath(j->what(), rpath) == NULL) {
                            LOG_ERR("failed to resolve path: '" << j->what() << "' - " << strerror(errno) << " - will not generate output");
                            ppath = NULL;
                            ret = 5;
                        }
                        else
                        {
                            sprintf(bpath, "%s", j->what());
                            if (symlink(rpath, path) < 0) {
                                LOG_ERR("failed to create symlink: '" << rpath << "'  '" << path << "' - " << strerror(errno) << " - will not generate output");
                                ppath = NULL;
                                ret = 5;
                            }
                        }
                    }
                }
                LOG_INFO("  }");
            }
        }
        ++pp;
    }
    
    if (summaries.size() > 1) {
        LOG_INFO("------");
        LOG_INFO("summary:");
        summaries.sort();
        for (std::list< std::pair<unsigned, std::string> >::const_iterator s=summaries.begin(); s!=summaries.end(); ++s) {
            LOG_INFO("    bins=" << std::setw(2) << s->first << "  " << s->second.c_str());
        }
    }

    return ret;
}
