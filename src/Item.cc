#include "Item.h"
#include "dvdpacking.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sstream>
#if HAVE_CXX17 == 1
#include <filesystem>
#else
#include <dirent.h>
#endif


Item::Item(const char* what_)
    : _what(what_), _size(0)
{
    if (what_ == nullptr) {
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


off_t  Item::_du(const char* where_) const
{
    LOG_DEBUG("examinging dir " << where_);
    off_t  size = 0;

 #if HAVE_CXX17 == 1
    if (!std::filesystem::exists(where_)) {
        std::ostringstream  err;
        err << "failed to open '" << where_ << "' - " << strerror(errno);
        throw std::invalid_argument(err.str());
    }

    for (const auto& de : std::filesystem::directory_iterator{where_,
                                                              std::filesystem::directory_options::follow_directory_symlink}
	)
    {
	switch (auto  t = de.symlink_status().type()) {
	    case std::filesystem::file_type::directory:
		size += _du(de.path().c_str());
		break;

	    case std::filesystem::file_type::regular:
		size += de.file_size();
		break;

	    default:
	        LOG_ERR(de.path().c_str() << " is not file/dir ????");
	}
    }
#else
    DIR*  d;
    if ( (d = opendir(where_)) == nullptr) {
        std::ostringstream  err;
        err << "failed to open '" << where_ << "' - " << strerror(errno);
        throw std::invalid_argument(err.str());
    }

    char  path[PATH_MAX];

    struct dirent*  dent = (struct dirent*)malloc(sizeof(struct dirent) + pathconf(where_, _PC_NAME_MAX));
    struct dirent*  result = nullptr;
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
#endif

    LOG_DEBUG(where_ << " = " << size);
    return size;
}
