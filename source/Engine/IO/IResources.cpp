#include "IResources.h"
#include "../Gen/IResourceGen.h"

#define CHUNK 0x4000

IResFiles ResFiles;

IResource* IResources::Load(const char* path) {
    IRes* node = &ResFiles.RES_HEAD;
    //IRes* foot = &ResFiles.RES_TAIL;
    //long resourceCount = ((long)foot - (long)node) / sizeof(IRes) - 2;
    //printf("%ld resources available.\n", resourceCount);

    node++;
    while (node->ShortName != NULL) {
        if (strcmp(path, node->ShortName) == 0) {
            printf("Short name: %s Full name: %s\n", node->ShortName, node->FullName);
            IResource* res = new IResource;
            res->Res = node;
            if (node->FullName != NULL)
                res->RW = SDL_RWFromFile(node->FullName, "rb");
#ifndef PLATFORM_IOS
#ifndef PLATFORM_ANDROID
            else if (node->BinaryStartRef != NULL)
                res->RW = SDL_RWFromMem(node->BinaryStartRef, (size_t)node->BinarySizeRef);
#endif
#endif
            else
                return NULL;
            return res;
        }
        node++;
    }
    #if defined(DEBUGGLE) || defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
        // Backup load if the file isn't on the generated list.
        char FullName[strlen(FULLNAME("")) + strlen(path)];
        sprintf(FullName, "%s%s", FULLNAME(""), path);

        //printf("Trying to load \"%s\" through backup method.\n", FullName);

        IResource* res = new IResource;
        res->Res = new IRes { path, FullName, NULL, NULL };
        res->RW = SDL_RWFromFile(FullName, "rb");

        if (res->RW != NULL) {
            return res;
        }
        printf("Full file \"%s\" could not be found.\n", FullName);
    #else
        printf("File \"%s\" could not be found.\n", path);
    #endif
    return NULL;
}

bool IResources::Close(IResource* res) {
    if (res == NULL) {
        printf("ERROR: Resource error.\n");
        return false;
    }
    if (res->RW == NULL) {
        printf("ERROR: Resource does not exist.\n");
        return false;
    }
    SDL_RWclose(res->RW);
    res->RW = NULL;
    return true;
}

unsigned long IResources::Decompress(void* dst, int dstLen, const void* src, int srcLen) {
    z_stream strm  = {0};
    strm.total_in  = strm.avail_in  = srcLen;
    strm.total_out = strm.avail_out = dstLen;
    strm.next_in   = (Bytef *) src;
    strm.next_out  = (Bytef *) dst;

    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;

    int err = -1;
    unsigned long ret = -1;

    err = inflateInit2(&strm, (15 + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
    if (err == Z_OK) {
        err = inflate(&strm, Z_FINISH);
        if (err == Z_STREAM_END) {
            ret = strm.total_out;
        }
        else {
             inflateEnd(&strm);
             return err;
        }
    }
    else {
        inflateEnd(&strm);
        return err;
    }

    inflateEnd(&strm);
    return ret;
}

void IResource::Reload() {
    RW = SDL_RWFromFile(Res->ShortName, "rb");
}
