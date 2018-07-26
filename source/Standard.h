#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <zlib.h>
#include <map>

#undef max
#undef min

#if defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    #include "SDL.h"
    #include "SDL_image.h"
    #include "SDL_mixer.h"
    #include "SDL_syswm.h"

    #if defined(PLATFORM_MAC)
        #include "gl.h"
        #include "SDL_opengl.h"
        #include "glu.h"
    #else
        #include "OpenGLES/ES3/gl.h"
        #include "SDL_opengl.h"
        #include "OpenGLES/ES3/glext.h"
    #endif
#elif defined(PLATFORM_ANDROID)
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_mixer.h>
    #include <SDL_syswm.h>

    #include <GLES2/gl2.h>
    #include <SDL_opengles2_khrplatform.h>
#else
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_mixer.h>

    #include <GL/glew.h>
    #include <GL/GL.h>
    #include <SDL_opengl.h>
    #include <GL/GLU.h>

    #include <SDL_syswm.h>

    #include <windows.h>

    #define uint unsigned int
#endif

class Application;
class ITexture;
class Texture;
class Audio;
class MusicStackNode;
class MusicStack;
class Scene;
class Scene_TitleScreen;
class Scene_MainMenu;
class Scene_HCZ;
class LevelScene;
class S3Object;
class Particle;
class IPlayer;

// Global Functions
extern float Math_PI;
int mod(int a, int n);
int modulo(int a, int b);
int HUD_PlaceOfNum(int num, int ind, int y);
float toRadians(float n);
float toDegrees(float n);
float wrapAngle(float n);
float wrapAngleRad(float n);
float clamp(float val, int lo, int hi);
float signum(float n);
int numPlaces(int n);
int zigzag(int f, int max);
float zigzag(float f, float max);
float ZigZagCosine(float x);
float slope(float from, float to, float percent);
char getZoneNameLetterWidth(int f);
int TenTo(int n);

size_t LoadRW(SDL_RWops *context, void *ptr);
size_t LoadRWX(SDL_RWops *context, void *ptr);
int Log2(int x);

#define QUOTE(...) #__VA_ARGS__

#define $AudioFree(audio) app->AudioFree(sc->audio)
#define $AudioStop(audio) app->AudioStop(sc->audio)
#define $AudioPause(audio) app->AudioPause(sc->audio)
#define $AudioResume(audio) app->AudioResume(sc->audio)
#define $AudioIsPlaying(audio) app->AudioIsPlaying(sc->audio)
#define $AudioPlay(audio, loops) app->AudioPlay(sc->audio, loops)
#define $AudioSetVolume(audio, percent) app->AudioSetVolume(sc->audio, percent)

#define WITH_ALL(IN) for (int _______p = 0; _______p < player_count; _______p++) { \
    IPlayer* player = playerBuffer[_______p]; \
    IN \
}