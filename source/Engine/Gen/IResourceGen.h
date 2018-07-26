#ifndef IRESOURCEGEN_H
#define IRESOURCEGEN_H

#if defined(PLATFORM_MAC)
    #define EXTERN(NAME) ;
    #define STARTREF(NAME) NULL
    #define SIZEREF(NAME) NULL

    #ifdef DEBUGGLE
        #define FULLNAME(PATH) ("../../../source/Resource/" PATH)
    #else
        #define FULLNAME(PATH) (PATH)
    #endif
#elif defined(PLATFORM_IOS)
    #define EXTERN(NAME) ;
    #define STARTREF(NAME) NULL
    #define SIZEREF(NAME) NULL

    #define FULLNAME(PATH) ("Resource/" PATH)
#elif defined(PLATFORM_ANDROID)
    #define EXTERN(NAME) ;
    #define STARTREF(NAME) NULL
    #define SIZEREF(NAME) NULL

    #define FULLNAME(PATH) ("Resource/" PATH)
#else
    #ifdef DEBUGGLE
        #define EXTERN(NAME) ;
        #define STARTREF(NAME) NULL
        #define SIZEREF(NAME) NULL

        #define FULLNAME(PATH) ("source/Resource/" PATH)
    #else
        #define EXTERN(NAME) \
extern unsigned char _binary_source_Resource_##NAME##_start; \
extern unsigned char _binary_source_Resource_##NAME##_size;
        #define STARTREF(NAME) &_binary_source_Resource_##NAME##_start
        #define  SIZEREF(NAME) &_binary_source_Resource_##NAME##_size

        #define FULLNAME(PATH) (PATH)
    #endif
#endif

struct IRes {
    const char* ShortName;
    const char* FullName;
    char*       BinaryStartRef;
    char*       BinarySizeRef;
};

EXTERN(Shaders_basicFS_bin);
EXTERN(Shaders_basicVS_bin);
EXTERN(Shaders_bwFS_bin);
EXTERN(Shaders_bwVS_bin);
EXTERN(Sprites_Menu_Signs4_png);
EXTERN(Sprites_Menu_Sign_spr);

struct IResFiles {
    IRes RES_HEAD;
    IRes Shaders_basicFS_bin { "Shaders/basicFS.bin", FULLNAME("Shaders/basicFS.bin"), STARTREF(Shaders_basicFS_bin), SIZEREF(Shaders_basicFS_bin) };
    IRes Shaders_basicVS_bin { "Shaders/basicVS.bin", FULLNAME("Shaders/basicVS.bin"), STARTREF(Shaders_basicVS_bin), SIZEREF(Shaders_basicVS_bin) };
    IRes Shaders_bwFS_bin { "Shaders/bwFS.bin", FULLNAME("Shaders/bwFS.bin"), STARTREF(Shaders_bwFS_bin), SIZEREF(Shaders_bwFS_bin) };
    IRes Shaders_bwVS_bin { "Shaders/bwVS.bin", FULLNAME("Shaders/bwVS.bin"), STARTREF(Shaders_bwVS_bin), SIZEREF(Shaders_bwVS_bin) };
    IRes Sprites_Menu_Signs4_png { "Sprites/Menu/Signs4.png", FULLNAME("Sprites/Menu/Signs4.png"), STARTREF(Sprites_Menu_Signs4_png), SIZEREF(Sprites_Menu_Signs4_png) };
    //IRes Sprites_Menu_Sign_spr { "Sprites/Menu/Sign.spr", FULLNAME("Sprites/Menu/Sign.spr"), STARTREF(Sprites_Menu_Sign_spr), SIZEREF(Sprites_Menu_Sign_spr) };
    IRes RES_TAIL;
};

#endif // IRESOURCEGEN_H
