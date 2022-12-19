#include "Item.h"
#include "dvdpacking.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sstream>
#if HAVE_CXX17 == 1 && defined(CXX17_READDIR)
#include <filesystem>
#include <numeric>
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
    long unsigned  size = 0;

 #if HAVE_CXX17 == 1 && defined(CXX17_READDIR)
    if (!std::filesystem::exists(where_)) {
        std::ostringstream  err;
        err << "failed to open '" << where_ << "' - " << strerror(errno);
        throw std::invalid_argument(err.str());
    }

    struct stat  st = { 0 };

    const auto  de = std::filesystem::directory_iterator{ where_, std::filesystem::directory_options::follow_directory_symlink };

    unsigned  dircount = 0;
    if (std::filesystem::is_directory(where_)) {
        dircount = 1;
	stat(where_, &st);
	// dirent has a size but its size depends on the filesystem; we do this to keep it in sync with "du -sb ..."
    }
    return std::accumulate(std::filesystem::begin(de), std::filesystem::end(de), 0lu, [this](auto x_, auto y_) {
	switch (auto  t = y_.symlink_status().type()) {
	    case std::filesystem::file_type::directory:
		LOG_DEBUG(y_.path() << " dir");
		return x_ + _du(y_.path().c_str());
		break;

	    case std::filesystem::file_type::regular:
		LOG_DEBUG(y_.path() << " " << y_.file_size());
		return x_ + y_.file_size();
		break;

	    default:
	        LOG_ERR(y_.path().c_str() << " is not file/dir ????");
		return x_;
	}
    }) + dircount * st.st_size;
#else
    DIR*  d;
    if ( (d = opendir(where_)) == nullptr) {
        std::ostringstream  err;
        err << "failed to open '" << where_ << "' - " << strerror(errno);
        throw std::invalid_argument(err.str());
    }

    char  path[PATH_MAX];
    struct stat  st = { 0 };
    stat(where_, &st);
    if (st.st_mode & S_IFDIR) {
	size += st.st_size;
    }

    struct dirent*  dent = (struct dirent*)malloc(sizeof(struct dirent) + pathconf(where_, _PC_NAME_MAX));
    struct dirent*  result = nullptr;
    while ( readdir_r(d, dent, &result) == 0 && result)
    {
        if (strcmp(result->d_name, ".") == 0 || strcmp(result->d_name, "..") == 0) {
            continue;
        }

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
            LOG_DEBUG("mode=" << st.st_mode << " ino=" << st.st_ino << " size=" << st.st_size << " '" << path << "'");
            if (st.st_mode & S_IFDIR) {
                size += _du(path);
            }
	    else {
		size += st.st_size;
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
