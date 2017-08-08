#include "Item.h"
#include "dvdpacking.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sstream>


Item::Item(const char* what_)  throw (std::invalid_argument, std::underflow_error, std::out_of_range)
    : _what(what_), _size(0)
{
    if (what_ == NULL) {
        throw std::invalid_argument("non null dir/file location required");
    }

    struct stat  st;
    if (stat(_what.c_str(), &st)) {
        std::ostringstream  err;
        err << "failed to open '" << _what << "' - " << strerror(errno);
        throw std::underflow_error(err.str());
    }

    LOG_DEBUG("examinging " << _what);
    if (st.st_mode & S_IFREG) {
        _size = st.st_size;
        return;
    }

    if (st.st_mode & S_IFDIR) {
        _size = _du(_what.c_str());
        return;
    }


    std::ostringstream  err;
    err << "unhandled st_mode=" << st.st_mode << " - '" << _what << "' " << __FILE__ << ", line " << __LINE__;
    throw std::out_of_range(err.str());
}

Item::Item(const Item& rhs_)
    : _what(rhs_._what), _size(rhs_._size)
{ }


off_t  Item::_du(const char* where_) const  throw (std::invalid_argument)
{
    LOG_DEBUG("examinging dir " << where_);
    off_t  size = 0;
    DIR*  d;
    if ( (d = opendir(where_)) == NULL) {
        std::ostringstream  err;
        err << "failed to open '" << where_ << "' - " << strerror(errno);
        throw std::invalid_argument(err.str());
    }

    char  path[PATH_MAX];

    struct dirent*  dent = (struct dirent*)malloc(sizeof(struct dirent) + pathconf(where_, _PC_NAME_MAX));
    struct dirent*  result = NULL;
    while ( readdir_r(d, dent, &result) == 0 && result)
    {
        if (strcmp(result->d_name, ".") == 0 || strcmp(result->d_name, "..") == 0) {
            continue;
        }

        struct stat  st;
        sprintf(path, "%s/%s", where_, result->d_name);
        if (stat(path, &st) < 0)
        {
            free(dent);

            std::ostringstream  err;
            err << "failed to stat '" << path << "' - " << strerror(errno);
            throw std::invalid_argument(err.str());
        }

        try
        {
            LOG_DEBUG("mode=" << st.st_mode << " ino=" << st.st_ino << " size=" << st.st_size);
            size += st.st_size;
            if (st.st_mode & S_IFDIR) {
                size += _du(path);
            }
        }
        catch (...)
        {
            free(dent);
            closedir(d);
            throw;
        }
    }
    free(dent);
    closedir(d);

    LOG_DEBUG(where_ << " = " << size);
    return size;
}
