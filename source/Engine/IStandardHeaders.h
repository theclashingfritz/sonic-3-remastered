#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <unordered_map>

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
#endif
#include "IE.h"
