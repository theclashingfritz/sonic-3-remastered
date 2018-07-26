#include "Standard.h"
#include "Resources.h"
#include "Application.h"
#include "TextureAudio.h"
#include "Scene_TitleScreen.h"
#include "Scene_MainMenu.h"
#include "Scene_AIZ.h"
#include "Scene_HCZ.h"
#include "Scene_CNZ.h"
#include "Scene_ICZ.h"
#include "Scene_SpecialStage.h"
#include "Engine/IO/INIReader.h"
#if defined(PLATFORM_WINDOWS)
    #include <GL/glew.h>
#endif
#if defined(PLATFORM_ANDROID)
    #include <android/log.h>
#endif

Application::Application(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        print(1, "%s", argv[i]);
        int arg1 = 0;
        int arg2 = 0;
        int arg3 = 0;
        if (sscanf(argv[i], "STARTPOS %d %d", &arg1, &arg2) > 0) {
            DEBUG_START_X = arg1;
            DEBUG_START_Y = arg2;
        }
        else if (sscanf(argv[i], "STARTSCENE %d %d %d", &arg1, &arg2, &arg3) > 0) {
            DEBUG_START_SCENE = arg1;
            DEBUG_START_DATA1 = arg2;
            DEBUG_START_DATA2 = arg3;
        }
    }
    // Set GL settings
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);
    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);

    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
    SDL_SetHint(SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "1");

    // Initialize SDL backend
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) < 0) {
        print(2, "SDL backend could not initialize: %s", SDL_GetError());
        return;
    }

    // Set the current platform ID
    #if defined(PLATFORM_WINDOWS)
        platform = P_Windows;
    #elif defined(PLATFORM_MAC)
        platform = P_Mac;
    #elif defined(PLATFORM_IOS)
        platform = P_iOS;
        Mobile = true;
    #elif defined(PLATFORM_ANDROID)
        platform = P_Android;
        Mobile = true;
    #else
        print(2, "Current platform is unknown.");
    #endif

    #if defined(PLATFORM_MAC)
        #undef PLATFORM_IOS
    #endif

    // Load settings
    ReadSettings();

    // Create window
    int X = SDL_WINDOWPOS_UNDEFINED;
    int Y = SDL_WINDOWPOS_UNDEFINED;
    Uint32 Flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    if (!Mobile) {
        X = SDL_WINDOWPOS_CENTERED_DISPLAY(Settings_Display_Monitor);
        //Flags |= SDL_WINDOW_ALLOW_HIGHDPI;
        Flags |= SDL_WINDOW_INPUT_FOCUS;
        if (Settings_Display_Borderless) {
            Flags |= SDL_WINDOW_BORDERLESS;
        }
    }
    else {
        Flags |= SDL_WINDOW_ALLOW_HIGHDPI;
        Flags |= SDL_WINDOW_BORDERLESS;
        CRTfilter = false;

        Settings_Display_Width = 400;
        Settings_Display_Height = 225;
    }

    window = SDL_CreateWindow("Sonic 3", X, Y, Settings_Display_Width, Settings_Display_Height, Flags);
    if (window == NULL) {
        print(2, "Window could not be created.");
        return;
    }

    if (Settings_Display_Fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    Settings_Display_Width = windowWidth;
    Settings_Display_Height = windowHeight;
    Resize();

    // Initalize GL context
    context = SDL_GL_CreateContext(window);
    if (!Mobile) {
        SDL_GL_SetSwapInterval(1);
    }

    // Log the computer's information
    print(0, "Current Platform: %s", SDL_GetPlatform());
    print(0, "Number of logical CPU cores: %d", SDL_GetCPUCount());
    print(0, "CPU Cache Line Size: %d", SDL_GetCPUCacheLineSize());
    print(0, "Total System Memory: %dMB", SDL_GetSystemRAM());
    print(0, "OpenGL Version: %s", glGetString(GL_VERSION));
    print(0, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    print(0, "Working Directory: %s", SDL_GetBasePath());

    // Get rendering size
    int renderWidth, renderHeight;
    SDL_GL_GetDrawableSize(window, &renderWidth, &renderHeight);
    print(0, "Let's see........%d x %d", renderWidth, renderHeight);
    if (renderWidth / Settings_Display_Width == 2) {
        Retina = true;
        print(1, "Retina Display is enabled!");
    }

    this->renderWidth = (int)std::ceil(gameHeight * ((float)Settings_Display_Width / Settings_Display_Height));
    this->renderHeight = gameHeight;
    print(1, "Render size: %d x %d", this->renderWidth, this->renderHeight);

    // Init GLEW for windows
    #if defined(PLATFORM_WINDOWS)
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if (glewError != GLEW_OK) {
            print(2, "Error initializing GLEW: %s", glewGetErrorString(glewError));
        }
    #endif

    // Create the framebuffer
    CreateFramebuffer();

    // Initialize drawing functions
    #ifndef PLATFORM_IOS
    #ifndef PLATFORM_ANDROID
        // NOTE: These functions do not exist in OpenGL ES
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, gameWidth, gameHeight, 0.0, 1.0, -1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glEnable(GL_TEXTURE_2D);
    #endif
    #endif
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Debugger warning
    #ifdef DEBUGGLE
    print(3, "%s", "Be sure to rebuild the resources file!");
    #endif

    // Initalize SDL subsystems
    if (IMG_Init(IMG_INIT_PNG) < 0) {
        print(2, "SDL_Image subsystem could not initialize: %s", IMG_GetError());
        return;
    }
    print(0, "SDL_Image initialized!");

    if (Mix_OpenAudio(32000, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        print(2, "SDL_Mixer subsystem could not initialize: %s", Mix_GetError());
        return;
    }
    print(0, "SDL_Mixer initialized!");
    //Mix_SetPostMix(PostMixProcess, (void*)this);
    //print(0, "PostMix initialized!");

    // Initialize input system
    input = new Input();
    input->app = this;
    input->Init();
    print(0, "Input initialized!");

    // Load shaders
    if (!LoadShader(&shader_basic, newFile(res_basicVS_bin, "res/basicVS.bin"), newFile(res_basicFS_bin, "res/basicFS.bin"))) {
        running = false;
        return;
    }
    print(0, "Basic Shader initialized!");
    if (!LoadShader(&shader_blackwhite, newFile(res_bw_bin, "res/bwVS.bin"), newFile(res_bw_bin, "res/bwFS.bin"))) {
        running = false;
        return;
    }
    print(0, "Black and White Shader initialized!");
    /*if (!LoadShader(&shader_test, newFile(res_levelHCZ1VS_bin, "res/levelHCZ1VS.bin"), newFile(res_levelHCZ1FS_bin, "res/levelHCZ1FS.bin"))) {
        running = false;
        return;
    }*/
    print(0, "Palette Shader initialized!");
    if (!Mobile) {
        if (!LoadShader(&shader_crt, newFile(res_crtVS_bin, "res/crtVS.bin"), newFile(res_crtFS_bin, "res/crtFS.bin"))) {
            running = false;
            return;
        }
        print(0, "CRT Shader initialized!");
    }

    // Create buffer objects for drawing later
    CreateVBOs();
    print(0, "VBOs initialized!");

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        print(2, "OpenGL error: %s", getGLEnum(err));
    }
}

void Application::ReadSettings() {
    INIReader reader("settings.ini");

    if (reader.ParseError() < 0) {
        print(2, "Can't load 'settings.ini'");
        return;
    }

    Settings_Display_Monitor = (int)reader.GetInteger("display", "monitor", 0);
    Settings_Display_Width = (int)reader.GetInteger("display", "width", 800);
    Settings_Display_Height = (int)reader.GetInteger("display", "height", 450);
    Settings_Display_Widescreen = reader.GetBoolean("display", "widescreen", true);
    Settings_Display_Fullscreen = reader.GetBoolean("display", "fullscreen", false);
    Settings_Display_Borderless = reader.GetBoolean("display", "borderless", false);
    Settings_Display_SharpestPixels = reader.GetBoolean("display", "sharpestPixels", false);
    Settings_Music_Volume = reader.GetReal("audio", "music", 1.0f);
    CRTfilter = reader.GetBoolean("display", "crt", false);

    pixelScale = (int)std::ceil(Settings_Display_Height / 225.0f);

    if (CRTfilter) {
        pixelScale = 1;
        CRTfilter = false;
    }
}

char* Application::ReadTextFile(SDL_RWops* rw) {
        if (rw == NULL) {
            print(2, "Text file could not be read.");
            return NULL;
        }

        Sint64 res_size = SDL_RWsize(rw);
        char* res = (char*)malloc((size_t)res_size + 1);

        Sint64 nb_read_total = 0, nb_read = 1;
        char* buf = res;
        while (nb_read_total < res_size && nb_read != 0) {
                nb_read = SDL_RWread(rw, buf, 1, (size_t)(res_size - nb_read_total));
                nb_read_total += nb_read;
                buf += nb_read;
        }
        SDL_RWclose(rw);
        if (nb_read_total != res_size) {
                free(res);
                return NULL;
        }

        res[nb_read_total] = '\0';
        return res;
}

bool Application::LoadShader(GLuint *shader_basic, SDL_RWops* rwVS, SDL_RWops* rwFS) {
    GLint programSuccess = GL_TRUE;
    GLint vShaderCompiled = GL_FALSE;
    GLint fShaderCompiled = GL_FALSE;

    *shader_basic = glCreateProgram();


    char buf[4096];
    SDL_RWops* rw;

    rw = rwVS;
    sprintf(buf, "%s", ReadTextFile(rw));

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexShaderSource[] = {
        buf
    };
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
    if (vShaderCompiled != GL_TRUE) {
        print(2, "Unable to compile vertex shader %d!", vertexShader);
        printShaderLog(vertexShader);
        return false;
    }
    glAttachShader(*shader_basic, vertexShader);


    rw = rwFS;
    sprintf(buf, "%s", ReadTextFile(rw));

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentShaderSource[] = {
        buf
    };
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
    if (fShaderCompiled != GL_TRUE) {
        print(2, "Unable to compile fragment shader %d!", fragmentShader);
        printShaderLog(fragmentShader);
        return false;
    }
    glAttachShader(*shader_basic, fragmentShader);

    glLinkProgram(*shader_basic);

    //Check for errors
    glGetProgramiv(*shader_basic, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE)
    {
        print(2, "Failure to link program %d!", *shader_basic);
        printProgramLog(*shader_basic);
        return false;
    }

    print(0, "Shader %d successfully created.", *shader_basic);

    return true;
}

void Application::printProgramLog( GLuint program ) {
    //Make sure name is shader
    if( glIsProgram( program ) )
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            print(0, "%s", infoLog);
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        print(2, "Name %d is not a program", program);
    }
}

void Application::printShaderLog( GLuint shader ) {
    //Make sure name is shader
    if( glIsShader( shader ) )
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            print(0, "%s", infoLog);
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        print(2, "Name %d is not a shader", shader);
    }
}

void Application::CreateVBOs() {
    VBO = 0;
    VBOquad = 0;
    TBOquad = 0;

    Blend[0] = 1.0f;
    Blend[1] = 1.0f;
    Blend[2] = 1.0f;
    Blend[3] = 1.0f;

    long startTime = SDL_GetTicks();

    GLfloat texelData[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1,
    };
    GLfloat vertexData[] =
    {
        -0.5f,  0.5f,  0.0f, 0,  0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.0f, 0,  1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.0f, 0,  1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 0,  0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,
    };
    GLfloat vertexData2[] =
    {
        -0.0f,  1.0f,  0.0f, -1,     0.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         1.0f,  1.0f,  0.0f, -1,     1.0f, 0.0f,     1.0f, 1.0f, 1.0f, 1.0f,
         1.0f, -0.0f,  0.0f, -1,     1.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,
        -0.0f, -0.0f,  0.0f, -1,     0.0f, 1.0f,     1.0f, 1.0f, 1.0f, 1.0f,
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 10 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &VBOquad);
    glBindBuffer(GL_ARRAY_BUFFER, VBOquad);
    glBufferData(GL_ARRAY_BUFFER, 10 * 4 * sizeof(GLfloat), vertexData2, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &TBOquad);
    glBindBuffer(GL_ARRAY_BUFFER, TBOquad);
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), texelData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    print(0, "Time to create VBO buffer: %3.f", (SDL_GetTicks() - startTime) / 1000.f);
}

void Application::CreateFramebuffer() {
    GLint defaultFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    print(3, "Default FBO: %d", defaultFBO);

    //pixelScale = 1;

    #ifndef PLATFORM_IOS
        #ifndef PLATFORM_ANDROID
        GLuint renderBuffer;
        glGenRenderbuffers(1, &renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, gameWidth * pixelScale, gameHeight * pixelScale);
        #endif
    #endif
    // GL_RGB32F

    #ifndef PLATFORM_IOS
        #ifndef PLATFORM_ANDROID
        GLuint depthrenderbuffer;
        glGenRenderbuffers(1, &depthrenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        #if defined(PLATFORM_IOS)
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, gameWidth * pixelScale, gameHeight * pixelScale);
        #else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, gameWidth * pixelScale, gameHeight * pixelScale);
        #endif
        #endif
    #endif

    glGenTextures(1, &renderedTexture);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    if (Settings_Display_SharpestPixels ) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gameWidth * pixelScale, gameHeight * pixelScale, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &renderedTextureDepth);
    CheckGLError("glGenTextures");
    glBindTexture(GL_TEXTURE_2D, renderedTextureDepth);
    CheckGLError("glBindTexture");
    if (Settings_Display_SharpestPixels) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    #ifdef PLATFORM_IOS
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, gameWidth * pixelScale, gameHeight * pixelScale, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
        CheckGLError("glTexImage2D PLATFORM_IOS");
    #elif defined(PLATFORM_ANDROID)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, gameWidth * pixelScale, gameHeight * pixelScale, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
        CheckGLError("glTexImage2D PLATFORM_ANDROID");
    #else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, gameWidth * pixelScale, gameHeight * pixelScale, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        CheckGLError("glTexImage2D");
    #endif
    glBindTexture(GL_TEXTURE_2D, 0);
    CheckGLError("glBindTexture");

    glGenFramebuffers(1, &framebufferScreen);
    #if defined(PLATFORM_WINDOWS)
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferScreen);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderedTextureDepth, 0);
    #elif defined(PLATFORM_MAC)
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferScreen);
        glFramebufferTextureEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
        glFramebufferTextureEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderedTextureDepth, 0);
    #else
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferScreen);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderedTextureDepth, 0);
    #endif

    print(1, "Framebuffer status: %s (%d)", getGLEnum(glCheckFramebufferStatus(GL_FRAMEBUFFER)), glCheckFramebufferStatus(GL_FRAMEBUFFER));


    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
}

void Application::doWaitEvent() {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
            }
        }
        else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                realPaused = true;
        }
        else {
            if (e.type == SDL_JOYDEVICEADDED || e.type == SDL_JOYDEVICEREMOVED) {
                input->Init();
            }
            else if (!Mobile) {
                if (e.type == SDL_MOUSEMOTION) {
                    input->mouseX = (int)((float)e.motion.x / windowWidth * renderWidth);
                    input->mouseY = (int)((float)e.motion.y / windowHeight * renderHeight);
                    input->mouseMoved = true;
                    if (Retina) {
                        //input->mouseX /= 2;
                        //input->mouseY /= 2;
                    }
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    input->mouse = true;
                    if (e.button.button == SDL_BUTTON_LEFT)
                        input->mousePrimary = true;
                    else
                        input->mousePrimary = false;
                }
                else if (e.type == SDL_MOUSEBUTTONUP) {
                    input->mouse = false;
                    input->mouseReleased = true;
                }
            }
            else {
                if (e.type == SDL_FINGERDOWN) {
                    input->mouse = true;
                    input->mouseX = (int)(e.tfinger.x * gameWidth);
                    input->mouseY = (int)(e.tfinger.y * gameHeight);

                    for (int i = 0; i < 10; i++) {
                        if (input->touchID[i] == -1) {
                            input->touchX[i] = (int)(e.tfinger.x * gameWidth);
                            input->touchY[i] = (int)(e.tfinger.y * gameHeight);
                            input->touchDown[i] = true;
                            input->touchPressed[i] = true;
                            input->touchReleased[i] = false;
                            input->touchID[i] = e.tfinger.fingerId;
                            break;
                        }
                    }
                }
                else if (e.type == SDL_FINGERMOTION) {
                    input->mouseX = (int)(e.tfinger.x * gameWidth);
                    input->mouseY = (int)(e.tfinger.y * gameHeight);

                    for (int i = 0; i < 10; i++) {
                        if (input->touchID[i] == e.tfinger.fingerId) {
                            input->touchX[i] = (int)(e.tfinger.x * gameWidth);
                            input->touchY[i] = (int)(e.tfinger.y * gameHeight);
                            input->touchDown[i] = true;
                            break;
                        }
                    }
                }
                else if (e.type == SDL_FINGERUP) {
                    input->mouse = false;
                    input->mouseReleased = true;
                    input->mouseX = (int)(e.tfinger.x * gameWidth);
                    input->mouseY = (int)(e.tfinger.y * gameHeight);

                    for (int i = 0; i < 10; i++) {
                        if (input->touchID[i] == e.tfinger.fingerId) {
                            input->touchDown[i] = false;
                            input->touchReleased[i] = true;
                            input->touchID[i] = -1;
                            break;
                        }
                    }
                }
                else if (e.type == SDL_MOUSEMOTION) {

                }
                else if (e.type == SDL_MOUSEBUTTONDOWN) {

                }
                else if (e.type == SDL_MOUSEBUTTONUP) {

                }
                else if (e.type == SDL_APP_WILLENTERBACKGROUND) {
                    realPaused = true;
                }
                else if (e.type == SDL_APP_DIDENTERBACKGROUND) {
                    //input->pausePressed[0] = true;
                    pauseApp = true;
                }
                else if (e.type == SDL_APP_DIDENTERFOREGROUND) {
                    //input->pausePressed[0] = true;
                    pauseApp = false;
                }
                else if (e.type == SDL_MULTIGESTURE) {

                }
                else if (e.type == SDL_DOLLARGESTURE) {

                }
                else if (e.type == SDL_JOYAXISMOTION) {
                    //printf("90 %s\n", "weeeeee");
                }
                else {
                    switch (e.type) {
                        case SDL_APP_TERMINATING:
                            print(1, "%s %d", "SDL_APP_TERMINATING", e.type);
                            break;

                        case SDL_APP_LOWMEMORY:
                            print(1, "%s %d", "SDL_APP_LOWMEMORY", e.type);
                            break;

                        case SDL_APP_WILLENTERBACKGROUND:
                            print(1, "%s %d", "SDL_APP_WILLENTERBACKGROUND", e.type);
                            break;

                        case SDL_APP_DIDENTERBACKGROUND:
                            print(1, "%s %d", "SDL_APP_DIDENTERBACKGROUND", e.type);
                            break;

                        case SDL_APP_WILLENTERFOREGROUND:
                            print(1, "%s %d", "SDL_APP_WILLENTERFOREGROUND", e.type);
                            break;

                        case SDL_APP_DIDENTERFOREGROUND:
                            print(1, "%s %d", "SDL_APP_DIDENTERFOREGROUND", e.type);
                            break;

                        case SDL_WINDOWEVENT:
                            print(1, "%s %d", "SDL_WINDOWEVENT", e.type);
                            break;

                        case SDL_SYSWMEVENT:
                            print(1, "%s %d", "SDL_SYSWMEVENT", e.type);
                            break;

                        case SDL_MOUSEMOTION:
                            print(1, "%s %d", "SDL_MOUSEMOTION", e.type);
                            break;

                        case SDL_MOUSEBUTTONDOWN:
                            print(1, "%s %d", "SDL_MOUSEBUTTONDOWN", e.type);
                            break;

                        case SDL_MOUSEBUTTONUP:
                            print(1, "%s %d", "SDL_MOUSEBUTTONUP", e.type);
                            break;

                        case SDL_MOUSEWHEEL:
                            print(1, "%s %d", "SDL_MOUSEWHEEL", e.type);
                            break;

                        case SDL_JOYAXISMOTION:
                            print(1, "%s %d", "SDL_JOYAXISMOTION", e.type);
                            break;

                        case SDL_JOYBALLMOTION:
                            print(1, "%s %d", "SDL_JOYBALLMOTION", e.type);
                            break;

                        case SDL_JOYHATMOTION:
                            print(1, "%s %d", "SDL_JOYHATMOTION", e.type);
                            break;

                        case SDL_JOYBUTTONDOWN:
                            print(1, "%s %d", "SDL_JOYBUTTONDOWN", e.type);
                            break;

                        case SDL_JOYBUTTONUP:
                            print(1, "%s %d", "SDL_JOYBUTTONUP", e.type);
                            break;

                        case SDL_JOYDEVICEADDED:
                            print(1, "%s %d", "SDL_JOYDEVICEADDED", e.type);
                            break;

                        case SDL_JOYDEVICEREMOVED:
                            print(1, "%s %d", "SDL_JOYDEVICEREMOVED", e.type);
                            break;
                    }

                    //SDL_iPhoneSetEventPump(SDL_FALSE);
                }
            }
        }
    }
}

void Application::PostMixProcess(void *udata, Uint8 *stream, int len) {
    Application* app = (Application*)udata;

    //bool shittyAudioMode = false;

    if (app->currentScene != NULL)
        app->currentScene->PostMixProcess(udata, stream, len);

    // increment every 2 because STEREO sound
    // only do this for half the samples, since we're squishing it to half
    /*for (int i = 0; i < sampleCount; i += 2) {
        short finalValue = (short)(p[i + 0] * value);
        if (shittyAudioMode) {
            p[i + 0] = p[i + 4 + 0];
            p[i + 1] = p[i + 4 + 1];
            p[i + 2 + 0] = p[i + 6 + 0];
            p[i + 2 + 1] = p[i + 6 + 1];
            i += 6;
        }
    }*/

    /*short pMod[sampleCount / 2];

    for (int i = 0; i < sampleCount / 2 / 2; i++) {
        pMod[i * 2 + 0] = p[i * 4 + 0];
        pMod[i * 2 + 1] = p[i * 4 + 1];
    }

    for (int i = 0; i < sampleCount / 2 / 2; i++) {
        p[i * 2 + 0] = pMod[i * 4 + 0];
        p[i * 2 + 1] = pMod[i * 4 + 1];

        p[i * 2 + 0 + sampleCount / 2] = 0;
        p[i * 2 + 1 + sampleCount / 2] = 0;
    }*/
}
bool Application::AudioFree(Audio* audio) {
    if (audio != NULL) {
        audio->Free();
        return true;
    }
    return false;
}
bool Application::AudioStop(Audio* audio) {
    if (audio != NULL) {
        audio->Stop();
        return true;
    }
    return false;
}
bool Application::AudioPause(Audio* audio) {
    if (audio != NULL) {
        audio->Pause();
        return true;
    }
    return false;
}
bool Application::AudioResume(Audio* audio) {
    if (audio != NULL) {
        audio->Resume();
        return true;
    }
    return false;
}
bool Application::AudioIsPlaying(Audio* audio) {
    if (audio != NULL) {
        return audio->IsPlaying();
    }
    print(2, "In function `Application::AudioIsPlaying`, 'audio' is NULL.");
    return false;
}
bool Application::AudioPlay(Audio* audio, int loops) {
    if (audio != NULL) {
        audio->Play(loops);
        return true;
    }
    return false;
}
bool Application::AudioSetVolume(Audio* audio, float percent) {
    if (audio != NULL) {
        audio->SetVolume(percent);
        return true;
    }
    return false;
}
void Application::Resize() { }
void Application::SwapFullscreen() {
    Settings_Display_Fullscreen = !Settings_Display_Fullscreen;

    if (Settings_Display_Fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        Settings_Display_Width = windowWidth;
        Settings_Display_Height = windowHeight;
        renderWidth = (int)(gameHeight * ((float)Settings_Display_Width / Settings_Display_Height));
        renderHeight = gameHeight;

        print(0, "Fullscreened to %d x %d", windowWidth, windowHeight);

        //pixelScale = (int)std::floor(Settings_Display_Height / 225.0f) + 1;
    }
    else {
        SDL_SetWindowFullscreen(window, 0);
        SDL_SetWindowSize(window, Settings_Display_Width, Settings_Display_Height);
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        print(0, "Resized to %d x %d", windowWidth, windowHeight);
    }
    Resize();
}

int Application::StaticRender(void* data) {
    Application* self = (Application*)data;

    return 0;

    SDL_GL_MakeCurrent(self->window, self->context);

    unsigned int nextTick;
    while (self->running) {
        if (!self->gRenderReady)
            continue;

        nextTick = SDL_GetTicks();

        glBindFramebuffer(GL_FRAMEBUFFER, self->framebufferScreen);

        //glViewport(0.f, 0.f, windowWidth, windowHeight);
        glEnable(GL_DEPTH_TEST);

        glColorMask(false, false, false, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColorMask(true, true, true, true);

        for (self->CurrentViewport = 0; self->CurrentViewport < self->ViewportCount; self->CurrentViewport++) {
            glViewport(0.f, self->pixelScale * (float)self->gameHeight / self->ViewportCount * (self->ViewportCount - 1 - self->CurrentViewport), self->pixelScale * self->gameWidth, self->pixelScale * self->gameHeight / self->ViewportCount);

            self->ChangeShader(self->shader_basic);

            //self->currentScene->Render();
            self->g->FinishFrame();
        }

        glDisable(GL_DEPTH_TEST);

        #ifndef PLATFORM_IOS
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        #else
            glBindFramebuffer(GL_FRAMEBUFFER, 1);
        #endif

        if (self->CRTfilter) {
            self->ChangeShader(self->shader_crt);
        }
        else {
            self->ChangeShader(self->shader_basic);
        }

        float retina = 1;
        if (self->Retina)
            retina = 2;

        float customRatioFix = self->Settings_Display_Height * 16.f / 9.f / self->Settings_Display_Width;

        glViewport(
            (self->windowWidth / 2 - self->Settings_Display_Width / 2) * retina,
            (self->windowHeight / 2 - self->Settings_Display_Height / 2) * retina,
            self->Settings_Display_Width * retina,
            self->Settings_Display_Height * retina);

        GLint locPosition = glGetAttribLocation(self->current_shader, "v_position");
        GLint locTexCoord = glGetAttribLocation(self->current_shader, "v_texcoord");
        GLint locTexture  = glGetUniformLocation(self->current_shader, "u_sampler");
        GLint locTexture2 = glGetUniformLocation(self->current_shader, "u_sampler2");
        GLint locTranslate = glGetUniformLocation(self->current_shader, "u_translate");
        GLint locRotate = glGetUniformLocation(self->current_shader, "u_rotate");
        GLint locScale = glGetUniformLocation(self->current_shader, "u_scale");
        GLint locVColor = glGetAttribLocation(self->current_shader, "v_color");


        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, self->renderedTextureDepth);
        glUniform1i(locTexture2, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, self->renderedTexture);
        glUniform1i(locTexture, 0);

        if (self->CRTfilter) {
            GLint rubyInputSize   = glGetUniformLocation(self->shader_crt, "rubyInputSize");
            GLint rubyOutputSize  = glGetUniformLocation(self->shader_crt, "rubyOutputSize");
            GLint rubyTextureSize = glGetUniformLocation(self->shader_crt, "rubyTextureSize");

            //glUniform2f(rubyInputSize, gameWidth / (Settings_Display_Width / gameWidth), gameHeight / (Settings_Display_Height / gameHeight));
            glUniform2f(rubyInputSize, self->gameWidth, self->gameHeight);
            glUniform2f(rubyOutputSize, self->gameWidth, self->gameHeight);
            glUniform2f(rubyTextureSize, self->gameWidth, self->gameHeight);
        }

        glUniform4f(locTranslate, 0.0f, 0.0f, 0.0f, 0.0f);
        glUniform3f(locScale, self->gameWidth * customRatioFix, self->gameHeight, 1.0f);
        glUniform1f(locRotate, 0.0f);

        glEnableVertexAttribArray(locPosition);
        glEnableVertexAttribArray(locTexCoord);
        glEnableVertexAttribArray(locVColor);
            glBindBuffer(GL_ARRAY_BUFFER, self->VBOquad);
            glVertexAttribPointer(locPosition, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
            glVertexAttribPointer(locTexCoord, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid *)(4 * sizeof(GLfloat)));
            glVertexAttribPointer(locVColor,   4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisableVertexAttribArray(locPosition);
        glDisableVertexAttribArray(locTexCoord);
        glDisableVertexAttribArray(locVColor);

        SDL_GL_SwapWindow(self->window);

        if (!self->Mobile) {
            nextTick = SDL_GetTicks() - nextTick;
            if (nextTick < 1000 / 60) {
                SDL_Delay(1000 / 60 - nextTick);
            }
        }
        else {
            SDL_Delay(1);
        }

        self->gRenderReady = false;
    }

    return 1;
}

void Application::DoFrameWrapper(void* data) {
    printf("booooop\n");
    ((Application*)data)->DoFrame();
}
void Application::DoFrame() {
    unsigned int nextTick = SDL_GetTicks();

    doWaitEvent();

    if (pauseApp) return;

    input->Poll();

    if (input->pausePressed[0]) {
        paused = !paused;
        realPaused = paused;
    }

    if (input->jump3Pressed[0] && paused) {
        realPaused = false;
    }

    currentScene->Update();

    if (input->jump3Pressed[0] && paused) {
        realPaused = true;
    }

    input->Reset();

    ///*

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferScreen);

    //glViewport(0.f, 0.f, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);

    //glColorMask(false, false, false, true);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColorMask(true, true, true, true);

    ChangeShader(shader_basic);

    for (CurrentViewport = 0; CurrentViewport < ViewportCount; CurrentViewport++) {
        glViewport(0.f, pixelScale * (float)gameHeight / ViewportCount * (ViewportCount - 1 - CurrentViewport), pixelScale * gameWidth, pixelScale * gameHeight / ViewportCount);

        currentScene->Render();
        g->FinishFrame();
    }


    glDisable(GL_DEPTH_TEST);

    #ifndef PLATFORM_IOS
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    #else
        glBindFramebuffer(GL_FRAMEBUFFER, 1);
    #endif

    if (CRTfilter)
        ChangeShader(shader_crt);

    float retina = 1;
    if (Retina)
        retina = 2;

    float customRatioFix = Settings_Display_Height * 16.f / 9.f / Settings_Display_Width;

    glViewport(
        (windowWidth / 2 - Settings_Display_Width / 2) * retina,
        (windowHeight / 2 - Settings_Display_Height / 2) * retina,
        Settings_Display_Width * retina,
        Settings_Display_Height * retina);

    GLint locPosition = glGetAttribLocation(current_shader, "v_position");
    GLint locTexCoord = glGetAttribLocation(current_shader, "v_texcoord");
    GLint locTexture  = glGetUniformLocation(current_shader, "u_sampler");
    GLint locTexture2 = glGetUniformLocation(current_shader, "u_sampler2");
    GLint locTranslate = glGetUniformLocation(current_shader, "u_translate");
    GLint locRotate = glGetUniformLocation(current_shader, "u_rotate");
    GLint locScale = glGetUniformLocation(current_shader, "u_scale");
    GLint locVColor = glGetAttribLocation(current_shader, "v_color");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderedTextureDepth);
    glUniform1i(locTexture2, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderedTexture);
    //glBindTexture(GL_TEXTURE_2D, IntroMovie->Texture->GL_Tex);
    glUniform1i(locTexture, 0);

    if (CRTfilter) {
        GLint rubyInputSize   = glGetUniformLocation(shader_crt, "rubyInputSize");
        GLint rubyOutputSize  = glGetUniformLocation(shader_crt, "rubyOutputSize");
        GLint rubyTextureSize = glGetUniformLocation(shader_crt, "rubyTextureSize");

        //glUniform2f(rubyInputSize, gameWidth / (Settings_Display_Width / gameWidth), gameHeight / (Settings_Display_Height / gameHeight));
        glUniform2f(rubyInputSize, gameWidth, gameHeight);
        glUniform2f(rubyOutputSize, gameWidth, gameHeight);
        glUniform2f(rubyTextureSize, gameWidth, gameHeight);
    }

    glUniform4f(locTranslate, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform3f(locScale, gameWidth * customRatioFix, gameHeight, 1.0f);
    glUniform1f(locRotate, 0.0f);

    glEnableVertexAttribArray(locPosition);
    glEnableVertexAttribArray(locTexCoord);
    glEnableVertexAttribArray(locVColor);
        glBindBuffer(GL_ARRAY_BUFFER, VBOquad);
        glVertexAttribPointer(locPosition, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
        glVertexAttribPointer(locTexCoord, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid *)(4 * sizeof(GLfloat)));
        glVertexAttribPointer(locVColor,   4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(locPosition);
    glDisableVertexAttribArray(locTexCoord);
    glDisableVertexAttribArray(locVColor);

    SDL_GL_SwapWindow(window);
    //*/

    glUniform3f(locScale, 1.0f, 1.0f, 1.0f);

    nextTick = SDL_GetTicks() - nextTick;
    if (nextTick < 1000 / 60) {
        SDL_Delay(1000 / 60 - nextTick);
    }

    firstFrame = true;

    if (nextTick < minFrameTime) {
        minFrameTime = nextTick;
    }
    if (nextTick > maxFrameTime) {
        maxFrameTime = nextTick;
    }
    avgFrameTime += nextTick / 60.f;

    frameCount++;
    if (frameCount == 60) {
        long fullFrameTime = SDL_GetTicks() - lastFrameTime;
        FPScalc = 60.f / (fullFrameTime / 1000.f);
        lastFrameTime = SDL_GetTicks();
        frameCount = 0;

        if (!Mobile) {
            char name[250];
            sprintf(name, "Sonic 3 - FPS: %.1f - AVG: %.1fms - MIN: %ldms - MAX: %ldms", FPScalc, avgFrameTime, minFrameTime, maxFrameTime);
            SDL_SetWindowTitle(window, name);
        }
        else {
            //print(3, "Sonic 3 - FPS: %.1f - AVG: %.1fms - MIN: %ldms - MAX: %ldms", FPScalc, avgFrameTime, minFrameTime, maxFrameTime);
        }

        minFrameTime = 1000;
        maxFrameTime = 0;
        avgFrameTime = 0;
    }

    if (nextScene != NULL) {
        print(0, "Beginning scene transfer...");

        print(0, "Freeing current scene...");
        currentScene->Free();
        print(0, "Current scene freed.");
        currentScene = nextScene;
        currentScene->Load();
        nextScene = NULL;

        nextTick = SDL_GetTicks();

        print(0, "Ended scene transfer.");
    }



    #if defined(PLATFORM_IOS)
    //SDL_iPhoneSetEventPump(SDL_FALSE);
    #endif
}

void Application::Run() {
    Stack = new MusicStack();

    currentScene = new Scene_TitleScreen(this, -1, -1);
    //currentScene = new Scene_HCZ(this, 1, 1);
    //nextScene = new Scene_CNZ(this, 1, 0);
    //nextScene = new Scene_HCZ(this, 1, 0);
    //nextScene = new Scene_AIZ(this, 1, 0);
    //nextScene = new Scene_SpecialStage(this);
    if (DEBUG_START_SCENE == 0)
        nextScene = new Scene_AIZ(this, DEBUG_START_DATA1, DEBUG_START_DATA2);
    else if (DEBUG_START_SCENE == 1)
        nextScene = new Scene_CNZ(this, DEBUG_START_DATA1, DEBUG_START_DATA2);
    else if (DEBUG_START_SCENE == 2)
        nextScene = new Scene_HCZ(this, DEBUG_START_DATA1, DEBUG_START_DATA2);
    else if (DEBUG_START_SCENE == 3)
        nextScene = new Scene_ICZ(this, DEBUG_START_DATA1, DEBUG_START_DATA2);
    else if (DEBUG_START_SCENE == 4)
        nextScene = new Scene_MainMenu(this, DEBUG_START_DATA1, DEBUG_START_DATA2);
    else if (DEBUG_START_SCENE == 5)
        nextScene = new Scene_SpecialStage(this, DEBUG_START_DATA1, DEBUG_START_DATA2);
    else if (DEBUG_START_SCENE == 6)
        nextScene = new Scene_TitleScreen(this, DEBUG_START_DATA1, DEBUG_START_DATA2);

    if (currentScene == NULL) {
        return;
    }

    SDL_Thread* threadID = SDL_CreateThread(&Application::StaticRender, "co.aurum.imposterengine.Renderer", (void*)this);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Main loop.
    while (running) {
        DoFrame();
    }

    SDL_DetachThread(threadID);

    print(0, "Freeing current scene...");

    currentScene->Free();
    currentScene = NULL;

    for (std::map<std::string, ITexture*>::iterator it = Textures.begin(); it != Textures.end(); ++it) {
        if (it->second != NULL)
            it->second->Free();
    }
    //

    print(0, "Current scene freed.");

    input->Free();
    SDL_DestroyWindow(window);
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void Application::ChangeShader(GLuint shader) {
    current_shader = shader;
    glUseProgram(current_shader);

    locPosition = glGetAttribLocation(current_shader, "v_position");
    locTexCoord = glGetAttribLocation(current_shader, "v_texcoord");
    locTexture  = glGetUniformLocation(current_shader, "u_sampler");

    locTranslate = glGetUniformLocation(current_shader, "u_translate");
    locRotate = glGetUniformLocation(current_shader, "u_rotate");
    locScale = glGetUniformLocation(current_shader, "u_scale");

    locColor = glGetUniformLocation(current_shader, "u_color");
    locUseTex = glGetUniformLocation(current_shader, "u_useTex");

    locData = glGetUniformLocation(current_shader, "u_data");

    g->ChangeShader(shader);
}

void Application::SetBlend(float r, float g, float b, float a) {
    if (b == -2) {
        BlendOverride = true;
    }
    if (b == -3) {
        BlendOverride = false;
    }
    Blend[0] = r;
    Blend[1] = g;
    if (!BlendOverride)
        Blend[2] = b;
    else
        Blend[2] = 1;
    Blend[3] = a;
}

void Application::drawTextureTopR(Texture* tex, float x, float y, float w, float h, float sx, float sy, float sw, float sh, int ID) {
    drawTextureR(tex, x + w / 2, y + h / 2, w, h, sx, sy, sw, sh, 0, ID);
}
void Application::drawTextureR(Texture* tex, float x, float y, float w, float h, float sx, float sy, float sw, float sh, float rot, int ID) {
    print(3, "PLS DELETE");
}
void Application::drawTextureTop(Texture* tex, float x, float y, float w, float h, float sx, float sy, float sw, float sh) {
    print(3, "REMOVE THIS NEPHEW");
}
void Application::drawTexture(Texture* tex, float x, float y, float w, float h, float sx, float sy, float sw, float sh, float rot) {
    print(3, "REMOVE THIS");
}

void Application::drawSprite(Texture* tex, float x, float y, float z, int frame, float xa, float ya) {
    drawSpriteR(tex, x, y, frame, 0, xa, ya, (z >= 420 && (int)z % 2 == 0 ? -1 : 1), (z > 420 ? -1 : 1), 0);
}
void Application::drawSpriteH(Texture* tex, float x, float y, int frame, int frameH, float xa, float ya) {
    drawSpriteR(tex, x, y, frame, frameH, xa, ya, 1, 1, 0);
}
void Application::drawSpriteR(Texture* tex, float x, float y, int frame, int frameH, float xa, float ya, float sx, float sy, float rot) {
    if (tex == NULL) {
        //throwFatalError("TEXTURE_UNAVAILABLE:\nCould not find texture.");
        return;
    }
    drawSpriteT(tex,
            tex->TBO[(frame + tex->frames * frameH) % (tex->frames * tex->framesH)],
            x, y,
            xa, ya,
            tex->w * sx, tex->h * sy,
            rot);
}
void Application::drawSpriteT(Texture* t, GLint TBO, float x, float y, float xa, float ya, float w, float h, float rot) {
    if (t == NULL) {
        throwFatalError("TEXTURE_UNAVAILABLE:\nCould not find texture.");
        return;
    }
    if (t->tex == 0) {
        throwFatalError("TEXTURE_NOT_LOADED:\nCould not find texture: %s", t->src);
        return;
    }

	#if 0

    GLint tex = t->tex;

    SDL_Rect renderQuad = {
        (int)(x) + (int)((-w / 2.0f) * (xa - 1)),
        (int)(y + (-h / 2.0f) * (ya - 1)),
        (int)(w),
        (int)(h)
    };

    if (locPosition == -1) {
        print(2, "Could not find \"v_position\"!");
        return;//oh
    }
    if (locTexCoord == -1) {
        print(2, "Could not find \"v_texcoord\"!");
        return;//oh
    }
    if (locTexture == -1) {
        print(2, "Could not find \"u_sampler\"!");
        return;//oh
    }

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(locTexture, 0);

    glUniform1i(locUseTex, 1);

    glUniform4f(locTranslate, renderQuad.x, renderQuad.y, drawZ, palette);
    glUniform3f(locScale, renderQuad.w, renderQuad.h, 1.0f);
    glUniform1f(locRotate, rot * 180.0f / 3.14159265f);
    //glUniform4f(locColor, Blend[0], Blend[1], Blend[2], Blend[3]);
    glUniform4f(locColor, 1.0f, 1.0f, 1.0f, 1.0f);
        glEnableVertexAttribArray(locPosition);
        glEnableVertexAttribArray(locTexCoord);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(locPosition, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, TBO);
        glVertexAttribPointer(locTexCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(locPosition);
        glDisableVertexAttribArray(locTexCoord);
    drawZ -= 0.001f;

	#endif
}

void Application::drawSpriteOPTeasy(Texture* t, float x, float y, int frame, int frameH, float xa, float ya, float scaleX, float scaleY, int ID) {
    #if 0

    int xx = frame;
    int yy = frameH;
    float sx = (xx) / (float)t->frames;
    float sy = (yy) / (float)t->framesH;
    float sw = 1.f / t->frames;
    float sh = 1.f / t->framesH;
    float w = t->w * scaleX;
    float h = t->h * scaleY;
    float z = drawZ;
    x -= xa * w / 2;
    y -= ya * h / 2;

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y + h);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy + sh;

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x + w);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y + h);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx + sw;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy + sh;

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x + w);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx + sw;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy;


    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy;

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y + h);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy + sh;

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x + w);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx + sw;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy;

    OPTs[ID].OPTverts += 6;

	#endif
}
void Application::drawSpriteOPT(float x, float y, float w, float h, float z, float sx, float sy, float sw, float sh, int ID) {
    #if 0

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y + h);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy + sh;

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x + w);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y + h);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx + sw;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy + sh;

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x + w);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx + sw;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy;


    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy;

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y + h);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy + sh;

    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(x + w);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = (int)(y);
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = z;
    OPTs[ID].OPTvertPoints[OPTs[ID].OPTvD++] = palette;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sx + sw;
    OPTs[ID].OPTtexePoints[OPTs[ID].OPTtD++] = sy;

    OPTs[ID].OPTverts += 6;

	#endif
}
void Application::renderSpriteOPT(Texture* t, int ID) {
    if (t == NULL) {
        throwFatalError("TEXTURE_UNAVAILABLE:\nCould not find texture.");
        //return;
    }
    if (t->tex == 0) {
        throwFatalError("TEXTURE_NOT_LOADED:\nCould not find texture: %s", t->src);
        //return;
    }

    renderSpriteOPT(t->tex, ID);
}
void Application::renderSpriteOPT(GLint tex, int ID) {
    #if 0

    glGenBuffers(1, &OPTs[ID].OPTvbo);
    glBindBuffer(GL_ARRAY_BUFFER, OPTs[ID].OPTvbo);
    glBufferData(GL_ARRAY_BUFFER, 4 * OPTs[ID].OPTverts * sizeof(GLfloat), OPTs[ID].OPTvertPoints, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &OPTs[ID].OPTtbo);
    glBindBuffer(GL_ARRAY_BUFFER, OPTs[ID].OPTtbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * OPTs[ID].OPTverts * sizeof(GLfloat), OPTs[ID].OPTtexePoints, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(locTexture, 0);

    glUniform1i(locUseTex, 1);

    glUniform4f(locTranslate, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform3f(locScale, 1.0f, 1.0f, 1.0f);
    glUniform1f(locRotate, 0.0f);
    //glUniform4f(locColor, Blend[0], Blend[1], Blend[2], Blend[3]);
        glEnableVertexAttribArray(locPosition);
        glEnableVertexAttribArray(locTexCoord);

        glBindBuffer(GL_ARRAY_BUFFER, OPTs[ID].OPTvbo);
        glVertexAttribPointer(locPosition, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, OPTs[ID].OPTtbo);
        glVertexAttribPointer(locTexCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, OPTs[ID].OPTverts);

        glDisableVertexAttribArray(locPosition);
        glDisableVertexAttribArray(locTexCoord);

    glDeleteBuffers(1, &OPTs[ID].OPTvbo);
    glDeleteBuffers(1, &OPTs[ID].OPTtbo);
    OPTs[ID].OPTverts = 0;
    OPTs[ID].OPTvD = 0;
    OPTs[ID].OPTtD = 0;
    drawZ -= 0.001f;

	#endif
}

void Application::drawRectangle(float x, float y, float w, float h, float r, float g, float b, float a) {
    drawRectangle(x, y, w, h, r, g, b, a, 0.0f);
}
void Application::drawRectangle(float x, float y, float w, float h, float r, float g, float b, float a, float rot) {
    /*

    glUniform1i(locUseTex, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(locTexture, 0);

    glUniform4f(locTranslate, x + w / 2, y + h / 2, -0.9f, palette);
    glUniform3f(locScale, w, h, 1.0f);
    glUniform1f(locRotate, rot);
    //glUniform4f(locColor, r, g, b, a);
        glEnableVertexAttribArray(locPosition);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(locPosition, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(locPosition);

    glUniform1i(locUseTex, 1);

    drawZ -= 0.001f;
    //*/
}

void Application::print(int sev, const char* string, ...) {
    va_list args;
    va_start(args, string);

    char str[4096];
    char errorCode[4096];
    vsprintf(str, string, args);

    char prefix[8];

    int ForgC = 0;
    if (sev == 0) { // LOG
        sprintf(errorCode, "%s %s", "LOG:", str);
        ForgC = 8;
    }
    else if (sev == 1) { // WARNING
        sprintf(errorCode, "%s %s", "WARNING:", str);
        ForgC = 14;
    }
    else if (sev == 2) { // ERROR
        sprintf(errorCode, "%s %s", "ERROR:", str);
        ForgC = 12;
    }
    else if (sev == 3) { // IMPORTANT
        sprintf(errorCode, "%s %s", "IMPORTANT:", str);
        ForgC = 11;
    }

    sprintf(prefix, "%s", "");

    #if defined(PLATFORM_WINDOWS)
        WORD wColor;
        //This handle is needed to get the current background attribute

        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        //csbi is used for wAttributes word

        if (GetConsoleScreenBufferInfo(hStdOut, &csbi)) {
          //To mask out all but the background attribute, and to add the color
          wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
          SetConsoleTextAttribute(hStdOut, wColor);
        }
    #endif

    #if defined(PLATFORM_MAC)
        if (sev == 0) {
            sprintf(prefix, "%s", "\x1b[0m");
        }
        else if (sev == 1) {
            sprintf(prefix, "%s", "\x1b[33m");
        }
        else if (sev == 2) {
            sprintf(prefix, "%s", "\x1b[31m");
        }
        else if (sev == 3) {
            sprintf(prefix, "%s", "\x1b[36m");
        }
    #endif

    #if defined(PLATFORM_ANDROID)
        if (sev == 0) {
            __android_log_print(ANDROID_LOG_INFO, "ImpostorEngine", "%s", errorCode);
        }
        else if (sev == 1) {
            __android_log_print(ANDROID_LOG_WARN, "ImpostorEngine", "%s", errorCode);
        }
        else if (sev == 2) {
            __android_log_print(ANDROID_LOG_ERROR, "ImpostorEngine", "%s", errorCode);
        }
        else if (sev == 3) {
            __android_log_print(ANDROID_LOG_FATAL, "ImpostorEngine", "%s", errorCode);
        }

    #endif

    printf("%s%s\n", prefix, errorCode);
    fflush(stdout);
    #if defined(PLATFORM_WINDOWS)
        wColor = (csbi.wAttributes & 0xF0) + (7 & 0x0F);
        SetConsoleTextAttribute(hStdOut, wColor);
    #endif
    //sprintf(log, "%s%s\n", log,  errorCode);
}
void Application::throwFatalError(const char* string, ...) {
    va_list args;
    va_start(args, string);

    char str[4096];
    vsprintf(str, string, args);

    #if !defined(PLATFORM_IOS) && !defined(PLATFORM_ANDROID)
        const SDL_MessageBoxButtonData buttons[] = {
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Ignore" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Abort" },
        };
        const SDL_MessageBoxData messageboxdata = {
            SDL_MESSAGEBOX_ERROR,
            window,
            "Fatal Error",
            str,
            SDL_arraysize(buttons),
            buttons,
            NULL
        };
        int buttonid;
        SDL_ShowMessageBox(&messageboxdata, &buttonid);
        if (buttonid == 1) {
            std::exit(0);
        }
    #else
        print(3, str);
    #endif
}
void Application::Breakpoint(const char* str) {
    if (!BreakpointsEnabled) return;

    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Ignore" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "Disable Breakpoint" },
    };
    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_WARNING,
        window,
        "Breakpoint",
        str,
        SDL_arraysize(buttons),
        buttons,
        NULL
    };
    int buttonid;
    SDL_ShowMessageBox(&messageboxdata, &buttonid);
    if (buttonid == 1) {
        BreakpointsEnabled = false;
    }
}
