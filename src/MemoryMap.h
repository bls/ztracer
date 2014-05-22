#ifndef _MEMORY_MAP
#define _MEMORY_MAP

#include "pin.H"
#include <vector>

class MemoryRegion {
 public:
  ADDRINT addr;
  ADDRINT endaddr;
  bool readable;
  bool writable;
  bool executable;
};

bool
read_memory_regions(std::vector<MemoryRegion>& regions);

#endif
