#ifndef RESOURCES_H
#define RESOURCES_H

#if defined(PLATFORM_MAC)
    #include <mach-o/getsect.h>
    #include <mach-o/ldsyms.h>

    #ifdef BUILD_SHARED_LIBS
        #if !defined(__LP64__)
            const struct mach_header _mh_execute_header;
        #else
            const struct mach_header_64 _mh_execute_header;
        #endif
    #else
        #if !defined(__LP64__)
            extern const struct mach_header _mh_execute_header;
        #else
            extern const struct mach_header_64 _mh_execute_header;
        #endif
    #endif

    int macDataLen(const char* oldStr);
    unsigned char* macDataPtr(const char* oldStr);
    #define EXTLD(NAME) /*extern unsigned char __DATA__ ## NAME;*/
    #define LDVAR(NAME) macDataPtr(#NAME)
    #define LDLEN(NAME) macDataLen(#NAME)

    #if defined(DEBUGGLE)
        #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, "../../../source/" SRC, FRAMES, FRAMESH, false)
        #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, "../../../source/" SRC, FRAMES, FRAMESH, true)
        #define newSound(NAME, SRC) new Audio("../../../source/" SRC, false)
        #define newMusic(NAME, SRC) new Audio("../../../source/" SRC,  true)
        #define newMusic2(NAME, SRC) new Audio("../../../source/" SRC,    2)
        #define newFile(NAME, SRC) SDL_RWFromFile("../../../source/" SRC, "rb")
    #else
        #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, false)
        #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, true)
        #define newSound(NAME, SRC) new Audio(SRC, false)
        #define newMusic(NAME, SRC) new Audio(SRC,  true)
        #define newMusic2(NAME, SRC) new Audio(SRC,    2)
        #define newFile(NAME, SRC) SDL_RWFromFile(SRC, "rb")
    #endif
#elif defined(PLATFORM_IOS)
    #define EXTLD(NAME) ;
    #define LDVAR(NAME) (NULL)
    #define LDLEN(NAME) (0)

    #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, false)
    #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, SRC, FRAMES, FRAMESH, true)
    #define newSound(NAME, SRC) new Audio(SRC, false)
    #define newMusic(NAME, SRC) new Audio(SRC,  true)
    #define newMusic2(NAME, SRC) new Audio(SRC,    2)
    #define newFile(NAME, SRC) SDL_RWFromFile(SRC, "rb")
#else // PLATFORM_WINDOWS, PLATFORM_VITA
    #define EXTLD(NAME) \
        extern unsigned char _binary_source_##NAME##_start; \
        extern unsigned char _binary_source_##NAME##_size; \
        extern unsigned char _binary_source_##NAME##_end;
    #define LDVAR(NAME) &_binary_source_##NAME##_start
    #define LDLEN(NAME) (int)&_binary_source_##NAME##_size

    #if defined(DEBUGGLE)
        #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, "source/" SRC, FRAMES, FRAMESH, false)
        #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, "source/" SRC, FRAMES, FRAMESH, true)
        #define newSound(NAME, SRC) new Audio("source/" SRC, false)
        #define newMusic(NAME, SRC) new Audio("source/" SRC,  true)
        #define newMusic2(NAME, SRC) new Audio("source/" SRC,    2)
        #define newFile(NAME, SRC) SDL_RWFromFile("source/" SRC, "rb")
        #define testtest(NAME, SRC) ("source/" SRC)
    #else
        #define newTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, LDVAR(NAME), LDLEN(NAME), FRAMES, FRAMESH, SRC, false)
        #define newPalTexture(NAME, FRAMES, FRAMESH, SRC) new Texture(app, LDVAR(NAME), LDLEN(NAME), FRAMES, FRAMESH, SRC, true)
        #define newSound(NAME, SRC) new Audio(LDVAR(NAME), LDLEN(NAME), false)
        #define newMusic(NAME, SRC) new Audio(LDVAR(NAME), LDLEN(NAME),  true)
        #define newMusic2(NAME, SRC) new Audio(LDVAR(NAME), LDLEN(NAME),    2)
        #define newFile(NAME, SRC) SDL_RWFromMem(LDVAR(NAME), LDLEN(NAME))
    #endif
#endif

### RESOURCES_H_AUTOFILL ###

#endif /* RESOURCES_H */
