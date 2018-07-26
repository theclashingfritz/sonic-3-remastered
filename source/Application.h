#ifndef APPLICATION_H
#define APPLICATION_H

#include "Standard.h"
#include "Input.h"
#include "Scene.h"

#include "Engine/IGraphics.h"
#include "Engine/ITexture.h"

struct OPT {
    GLfloat OPTvertPoints[8192 * 64 * 2]; // 2 is to support Pallete Plane
    GLfloat OPTtexePoints[8192 * 64];
    GLuint  OPTvbo;
    GLuint  OPTtbo;
    int     OPTvD = 0;
    int     OPTtD = 0;
    int     OPTverts = 0;
};

class Application {
public:
    enum Platform { P_Vita, P_Windows, P_Mac, P_iOS, P_Android };
    struct DrawState {
        float Z;
    };

    IGraphics* g = new IGraphics;
    bool gRenderReady = false;

    Scene* currentScene = NULL;
    Scene* nextScene = NULL;

    SDL_Surface* screen = NULL;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* screenTex = NULL;
    SDL_Event e;

    SDL_GLContext context;

    DrawState drawState;

    class MusicStack* Stack;

    const int FPS = 60;
    const int SKIP_TICKS = 1000 / FPS;
    const int MAX_FRAMESKIP = 1;

    Input* input;

    Platform platform = P_Windows;
    bool Mobile = false;

    bool running = true;
    bool firstFrame = false;

    int windowWidth = 400;
    int windowHeight = 225;
    int pixelScale = 3;
    int palette = 0;
    int renderWidth = 400;
    int renderHeight = 225;
    const int gameWidth = 400;
    const int gameHeight = 225;
    int ViewportCount = 1;
    int CurrentViewport = 0;

    bool paused = false;
    bool realPaused = false;
    bool soundPaused = false;

    int  Settings_Display_Monitor = 0;
    int  Settings_Display_Width = 400 * 3;
    int  Settings_Display_Height = 225 * 3;
    bool Settings_Display_Widescreen = false;
    bool Settings_Display_Fullscreen = false;
    bool Settings_Display_Borderless = false;
    bool Settings_Display_SharpestPixels = false;
    float Settings_Music_Volume = 1.0f;
    bool Retina = false;

    bool Settings_Rumble = true;

    bool pauseApp = false;

    bool CRTfilter = false;

    char log[1234567];

    float Blend[4];
    bool BlendOverride = false;

    ITexture* TextureBank[10];
    ISprite* SpriteBank[100];
    std::map<std::string, ITexture*> Textures;
    std::map<std::string, ISprite*> Sprites;

    GLuint shader_basic;
    GLuint shader_crt;


    GLuint shader_test;
    GLuint shader_blackwhite;

    GLuint current_shader;

    GLuint VBO = 0;
    GLuint TBOquad = 0;
    GLuint VBOquad = 0;
    GLuint VBOquadR = 0;
    GLuint TBO[1024];
    GLuint framebufferScreen = 0;
    GLuint renderedTexture;
    GLuint renderedTextureDepth;

    GLint locPosition;
    GLint locTexCoord;
    GLint locTexture;
    GLint locTranslate;
    GLint locRotate;
    GLint locScale;
    GLint locColor;
    GLint locUseTex;

    GLint locData;

    int DEBUG_START_SCENE = -1;
    int DEBUG_START_DATA1 = -1;
    int DEBUG_START_DATA2 = -1;
    int DEBUG_START_X = -1;
    int DEBUG_START_Y = -1;

    long frameCount = 0;
    long lastFrameTime = 0;
    float FPScalc = 60;

    long minFrameTime = 1000;
    long maxFrameTime = 0;
    float avgFrameTime = 0;


    Application(int argc, char *argv[]);

    void Run();
    void Resize();
    void doWaitEvent();
    void SwapFullscreen();
    bool LoadShader(GLuint *shader_basic, SDL_RWops* rwVS, SDL_RWops* rwFS);
    void ChangeShader(GLuint shader);
    void CreateVBOs();
    void CreateFramebuffer();

    static int StaticRender(void* data);

    static void DoFrameWrapper(void*);
    void DoFrame();

    static void PostMixProcess(void *udata, Uint8 *stream, int len);
    char* ReadTextFile(SDL_RWops* rw);
    const char* getGLEnum(GLint err) {
        switch (err) {
            case GL_INVALID_OPERATION:
                return "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_ENUM:
                return "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                return "GL_INVALID_VALUE";
                break;
            case GL_OUT_OF_MEMORY:
                return "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                return "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_FRAMEBUFFER_COMPLETE:
                return "GL_FRAMEBUFFER_COMPLETE";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                return "GL_FRAMEBUFFER_UNSUPPORTED";
                break;
            default:
                return "none";
        }
        return "none";
    }
    void CheckGLError(const char* function) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            print(2, "OpenGL error at function %s: %s", function, getGLEnum(err));
        }
    }

    void printProgramLog( GLuint program );
    void printShaderLog( GLuint shader );
    void print(int sev, const char* string, ...);
    void throwFatalError(const char* string, ...);
    void Breakpoint(const char* str);
    bool BreakpointsEnabled = true;

    void ReadSettings();

    void SetBlend(float r, float g, float b, float a);

    void drawTextureTopR(Texture* tex, float x, float y, float w, float h, float sx, float sy, float sw, float sh, int ID);
    void drawTextureR(Texture* tex, float x, float y, float w, float h, float sx, float sy, float sw, float sh, float rot, int ID);
    void drawTextureTop(Texture* tex, float x, float y, float w, float h, float sx, float sy, float sw, float sh);
    void drawTexture(Texture* tex, float x, float y, float w, float h, float sx, float sy, float sw, float sh, float rot);

    void drawSprite(Texture* tex, float x, float y, float z, int frame, float xa, float ya);
    void drawSpriteH(Texture* tex, float x, float y, int frame, int frameH, float xa, float ya);
    void drawSpriteR(Texture* tex, float x, float y, int frame, int frameH, float xa, float ya, float sx, float sy, float rot);
    void drawSpriteT(Texture* t, GLint TBO, float x, float y, float xa, float ya, float w, float h, float rot);

    bool AudioStop(Audio* audio);
    bool AudioFree(Audio* audio);
    bool AudioPause(Audio* audio);
    bool AudioResume(Audio* audio);
    bool AudioIsPlaying(Audio* audio);
    bool AudioPlay(Audio* audio, int loops);
    bool AudioSetVolume(Audio* audio, float percent);

    void SaveDrawState() {
        drawState.Z = drawZ;
    }
    void RestoreDrawState() {
        drawZ = drawState.Z;
    }

    OPT OPTs[64];
    int optc = 64;

    float drawZ = 0;

    void drawSpriteOPT(float x, float y, float w, float h, float z, float sx, float sy, float sw, float sh, int ID);
    void drawSpriteOPTeasy(Texture* t, float x, float y, int frame, int frameH, float xa, float ya, float scaleX, float scaleY, int ID);
    void renderSpriteOPT(GLint tex, int ID);
    void renderSpriteOPT(Texture* t, int ID);

    void drawRectangle(float x, float y, float w, float h, float r, float g, float b, float a);
    void drawRectangle(float x, float y, float w, float h, float r, float g, float b, float a, float rot);
};

#endif
