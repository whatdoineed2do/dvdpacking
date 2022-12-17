#ifndef DVDPACKING_H
#define DVDPACKING_H


#include <iostream>
#include "../config.h"

class Bin;
class Item;

extern const char*  argv0;


#define LOG_INFO(x)  std::cout << argv0 << ": " << x << std::endl;
#define LOG_WARN(x)  std::cout << argv0 << ": [warn] " << x << std::endl;
#define LOG_ERR(x)   std::cout << argv0 << ": [ err] " << x << std::endl;

#ifdef DEBUG
#define LOG_DEBUG(x)  std::cout << argv0 << ": " __FILE__ << ", line " << __LINE__ << " " << x << std::endl;
#else
#define LOG_DEBUG(x)
#endif

#endif
