#include <cstdio>
#include <cstring>
#include "MemoryMap.h"

#ifdef __linux__
#include <linux/limits.h>

bool
read_memory_regions(std::vector<MemoryRegion> *regions)
{
  FILE *fp = fopen("/proc/self/maps", "r");
  if(fp == NULL) {
    return false;
  }

  long long addr;
  long long endaddr;
  char permissions[8];
  long long offset;
  char device[8];
  long long inode;
  char filename[PATH_MAX];

  regions->clear();
  while(1) {
    int ret = fscanf (fp,  "%llx-%llx %s %llx %s %llx", 
		      &addr, &endaddr, &permissions[0], &offset, 
		      &device[0], &inode);
    if (ret > 0 && ret != EOF) { // && inode != 0
      ret += fscanf (fp, "%s\n", filename);
    } else {
      filename[0] = '\0'; /* no filename */
      int UNUSED ignored = fscanf (fp, "\n");
    }
    if(ret == 0 || ret == EOF) {
      break;
    }
    MemoryRegion r;
    r.addr = addr;
    r.endaddr = endaddr;
    r.perms = std::string(permissions);
    regions->push_back(r);
  }

  fclose(fp);
  return true;
}

#endif
