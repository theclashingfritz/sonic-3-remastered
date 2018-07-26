#ifndef ITEXTURE_H
#define ITEXTURE_H

#include "IStandardHeaders.h"
#include "Gen/IResourceGen.h"
#include "IO/IResources.h"

#include "Standard.h"

#if !defined(PLATFORM_WINDOWS)
#if defined(PLATFORM_IOS) && !defined(PLATFORM_MAC)
#include "opencv2/core/core.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#else
#include <opencv2/core/core.hpp>
#include <opencv2/core/opengl_interop.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>
#endif
#endif

class Application;


class ITexture {
public:
    GLuint          GL_Tex;
    unsigned char*  Pixels = NULL;
    bool            Paletted = false;
    int             Width = 32;
    int             Height = 32;
    const char*     Source = NULL;

    ITexture() { }
    ITexture(IResource* res, bool save_pixels) {
        if (res == NULL) {
            SDL_Log("ERROR: No resource.\n");
            return;
        }
        if (res->RW == NULL) {
            SDL_Log("ERROR: Resource could not be found at %s.\n", res->Res->ShortName);
            return;
        }
        SDL_Surface* test = IMG_LoadPNG_RW(res->RW);
        if (!test) {
            SDL_Log("Texture from \"%s\" could not be loaded.\n", res->Res->ShortName);
            return;
        }
        Source = res->Res->ShortName;
        CreateTexture(NULL, test, save_pixels);
        SDL_RWclose(res->RW);
    }
    ITexture(Application* App, IResource* res, bool save_pixels) {
        if (res == NULL) {
            SDL_Log("ERROR: No resource.\n");
            return;
        }
        if (res->RW == NULL) {
            SDL_Log("ERROR: Resource could not be found at %s.\n", res->Res->ShortName);
            return;
        }
        SDL_Surface* test = IMG_LoadPNG_RW(res->RW);
        if (!test) {
            SDL_Log("Texture from \"%s\" could not be loaded.\n", res->Res->ShortName);
            return;
        }
        Source = res->Res->ShortName;
        CreateTexture(App, test, save_pixels);
        SDL_RWclose(res->RW);
    }
    void CreateTexture(Application* MainApp, SDL_Surface* base_surface, bool save_pixels);
    void Free();
};

struct ISpriteFrame {
    float U_left;
    float U_right;
    float V_top;
    float V_bottom;
    int OffX;
    int OffY;
    int OffW;
    int OffH;
    int FullW;
    int FullH;
};

class ISprite {
public:
    ITexture*       Texture = NULL;
    ISpriteFrame    Frame[1024];
    int             FrameCount = 0;
    const char*     Source = NULL;

    ISprite(IResource* res, ITexture* Tex) {
        if (Tex == NULL) {
            SDL_Log("ERROR: Texture does not exist.\n");
            fflush(stdout);
            return;
        }
        if (Tex->GL_Tex == 0) {
            SDL_Log("ERROR: Texture did not load.\n");
            fflush(stdout);
            return;
        }
        if (res == NULL) {
            SDL_Log("ERROR: No resource.\n");
            fflush(stdout);
            return;
        }
        if (res->RW == NULL) {
            SDL_Log("ERROR: Resource could not be found at %s.\n", res->Res->ShortName);
            fflush(stdout);
            return;
        }

        Source = res->Res->ShortName;
        Texture = Tex;

        Sint64 len = SDL_RWsize(res->RW);

        unsigned char spr_data[len];
        SDL_RWread(res->RW, spr_data, (size_t)len, 1);

        for (int i = 0; i < len; i += 14) {
            float U_left   = (spr_data[i + 0] * 0x100 + spr_data[i + 1]) / (float)Tex->Width;
            float U_right  = (spr_data[i + 0] * 0x100 + spr_data[i + 1] + spr_data[i + 4] * 0x100 + spr_data[i + 5]) / (float)Tex->Width;
            float V_top    = (spr_data[i + 2] * 0x100 + spr_data[i + 3]) / (float)Tex->Height;
            float V_bottom = (spr_data[i + 2] * 0x100 + spr_data[i + 3] + spr_data[i + 6] * 0x100 + spr_data[i + 7]) / (float)Tex->Height;
            int OffX = spr_data[i + 8];
            int OffY = spr_data[i + 9];
            int OffW = spr_data[i + 10] * 0x100 + spr_data[i + 11];
            int OffH = spr_data[i + 12] * 0x100 + spr_data[i + 13];
            int FullW = spr_data[i + 4] * 0x100 + spr_data[i + 5];
            int FullH = spr_data[i + 6] * 0x100 + spr_data[i + 7];
            Frame[FrameCount++] = ISpriteFrame { U_left, U_right, V_top, V_bottom, OffX, OffY, OffW, OffH, FullW, FullH };

            /*SDL_Log("   : Frame (X: %d, Y: %d, Width: %d, Height: %d) %f %f %f %f %d %d %d %d\n",
                spr_data[i + 0] * 0x100 + spr_data[i + 1],
                spr_data[i + 2] * 0x100 + spr_data[i + 3],
                spr_data[i + 4] * 0x100 + spr_data[i + 5],
                spr_data[i + 6] * 0x100 + spr_data[i + 7],

                U_left, U_right, V_top, V_bottom, OffX, OffY, OffW, OffH
            );*/
        }

        SDL_RWclose(res->RW);
    }
};

class IFont {
public:

};

class IVideo {
public:
    GLuint           GL_Tex;
    int              ChunkSize = 48;
    int              FramesLoaded = 0;
    long             NextTimeTick = 0;
    unsigned long    CurrentFrame;
    unsigned long    CurrentFrame60;
    unsigned long    Length;
    ITexture*        Texture;
    SDL_Thread*      Thread;
    bool             Finished;

    #if !defined(PLATFORM_WINDOWS)
    cv::VideoCapture Reader;
    cv::Mat          Frames[48];
    cv::Mat          NextFrames[48];
    #endif

    void Load();
    void NextFrame();
    void NextFrame(int Frame);

    static int LoadNext(void* data);
};

#endif // ITEXTURE_H
