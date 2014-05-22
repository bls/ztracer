#ifndef _MEMORY_MAP
#define _MEMORY_MAP

#include "pin.H"
#include <vector>
#include <string>

class MemoryRegion {
 public:
  ADDRINT addr;
  ADDRINT endaddr;
  std::string perms;
};

bool
read_memory_regions(std::vector<MemoryRegion> *regions);

#endif
