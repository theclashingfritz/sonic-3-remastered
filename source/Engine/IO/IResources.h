#ifndef IRESOURCES_H
#define IRESOURCES_H

#include "../IStandardHeaders.h"
#include "../Gen/IResourceGen.h"
#include <zlib.h>

class IResource {
public:
    SDL_RWops*  RW;
    IRes*       Res;
    void        Reload();
};
class IResources {
public:
    static IResource*    Load(const char* path);
    static bool          Close(IResource* res);
    static unsigned long Decompress(void* dst, int dstLen, const void* src, int srcLen);
};

#endif // IRESOURCES_H
