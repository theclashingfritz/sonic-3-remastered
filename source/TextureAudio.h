#ifndef TEXTUREAUDIO_H
#define TEXTUREAUDIO_H

#if defined(PLATFORM_MAC) || defined(PLATFORM_IOS)
    #include "SDL.h"
    #include "SDL_image.h"
    #include "SDL_mixer.h"
#endif
#if defined(PLATFORM_ANDROID)
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_mixer.h>

    #include <GLES2/gl2.h>
    #include <SDL_opengles2_khrplatform.h>
#endif
#if defined(PLATFORM_WINDOWS)
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_mixer.h>
    // AURUM FIX
    #include <SDL_opengl.h>
    #include <GL/GLU.h>
#endif

#include "Standard.h"
#include "Application.h"

class Texture {
public:
    // AURUM FIX: SDL_Texture* tex;
    int w, h, frames, framesH;

    GLuint tex;
    GLuint VBO;
    GLuint TBO[256];

    char src[100];

    unsigned char* pixels = NULL;
    bool paletted = false;

    bool Shared = false;

    GLfloat ROUND_TO(GLfloat n, int w, int sp) {
        //if (sp % 2 == 1)
            //return (int)(n * w) / (GLfloat)w;
        return n;
    }

    Texture(Application* app, unsigned char *buffer, int size, int frames, int framesH, const char* str, bool paletted) {
        this->paletted = paletted;
        Init(app, buffer, size, frames, framesH, str);
    }
    Texture(Application* app, unsigned char *buffer, int size, int frames, const char* str, bool paletted) {
        this->paletted = paletted;
        Init(app, buffer, size, frames, 1, str);
    }
    Texture(Application* app, const char* str, int frames, int framesH, bool paletted) {
        this->paletted = paletted;
        Init2(app, str, frames, framesH);
    }
    Texture(Application* app, SDL_RWops* rw, int frames, int framesH, bool paletted) {
        this->paletted = paletted;
        Init3(app, rw, frames, framesH);
    }
    Texture(Application* app, SDL_RWops* rw, int frames, int framesH) {
        Init3(app, rw, frames, framesH);
    }

    void CreateTexture(SDL_Surface* test, Application* app) {
        SDL_Surface* newTest = SDL_ConvertSurfaceFormat(test, SDL_PIXELFORMAT_ABGR8888, 0);

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newTest->w,
            newTest->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            newTest->pixels);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (paletted) {
            pixels = (unsigned char*)malloc(newTest->w * newTest->h * 4);
            memcpy(pixels, (unsigned char*)newTest->pixels, newTest->w * newTest->h * 4);
        }

        //Free up the memory.
        SDL_FreeSurface(newTest);
        SDL_FreeSurface(test);
        //delete newTest;
        //delete test;
        newTest = NULL;
        test = NULL;
    }
    void CreateVBOs(int frames, int framesH) {
        /*GLfloat vertexData[] =
        {
               -0.5f,  0.5f,  0.0f,
             0.5f,  0.5f,  0.0f,
             0.5f, -0.5f,  0.0f,
            -0.5f, -0.5f,  0.0f,
        };

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 3 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);*/

        for (int i = 0; i < frames * framesH; i++) {
            int x = i % frames;
            int y = i / frames;

            float texel = 0.0f;
            if (y != framesH - 1) {
                //texel = 0.06125f / h;
            }

            GLfloat texelData[] = {
                ROUND_TO((x + 0) / (float)frames, w, frames), (y + 1) / (float)framesH - texel,
                ROUND_TO((x + 0) / (float)frames, w, frames) + 1 / (float)frames, (y + 1) / (float)framesH - texel,
                ROUND_TO((x + 0) / (float)frames, w, frames) + 1 / (float)frames, (y + 0) / (float)framesH,
                ROUND_TO((x + 0) / (float)frames, w, frames), (y + 0) / (float)framesH,
            };

            glGenBuffers(1, &TBO[i]);
            glBindBuffer(GL_ARRAY_BUFFER, TBO[i]);
            glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), texelData, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    void Init(Application* app, unsigned char *buffer, int size, int frames, int framesH, const char* str) {
        SDL_RWops* rw = SDL_RWFromMem(buffer, size);
        SDL_Surface* test = IMG_LoadPNG_RW(rw);
        if (!test) {
            app->print(2, "Texture from \"%s\" could not be loaded.", str);
        }
        this->w = test->w / frames;
        this->h = test->h / framesH;

        int dot = -1;
        int slash = -1;
        for (int i = (int)strlen(str) - 1; i >= 6; i--) {
            if (str[i] == '.') {
                dot = i;
            }
            else if (str[i] == '/') {
                slash = i;
                break;
            }
        }

        strncpy(src, str + slash + 1, dot - slash);
        src[dot - slash] = '\0';

        CreateTexture(test, app);
        CreateVBOs(frames, framesH);

        delete rw;
        rw = NULL;

        this->frames = frames;
        this->framesH = framesH;
    }
    void Init2(Application* app, const char* str, int frames, int framesH) {
        SDL_Surface* test = IMG_Load(str);
        if (!test) {
            app->print(2, "Texture from \"%s\" could not be loaded.", str);
        }
        this->w = test->w / frames;
        this->h = test->h / framesH;

        int dot = -1;
        int slash = -1;
        for (int i = (int)strlen(str) - 1; i >= 6; i--) {
            if (str[i] == '.') {
                dot = i;
            }
            else if (str[i] == '/') {
                slash = i;
                break;
            }
        }

        strncpy(src, str + slash + 1, dot - slash);
        src[dot - slash] = '\0';

        CreateTexture(test, app);
        CreateVBOs(frames, framesH);

        this->frames = frames;
        this->framesH = framesH;
    }
    void Init3(Application* app, SDL_RWops* rw, int frames, int framesH) {
        SDL_Surface* test = IMG_LoadPNG_RW(rw);
        if (!test) {
            app->print(2, "Texture from memory could not be loaded.");
        }
        this->w = test->w / frames;
        this->h = test->h / framesH;

        CreateTexture(test, app);
        CreateVBOs(frames, framesH);

        if (!paletted) {
            SDL_RWclose(rw);
            //delete rw;
            rw = NULL;
        }

        this->frames = frames;
        this->framesH = framesH;
    }
    void Free() {
        //glDeleteBuffers(1, &VBO);
        if (pixels != NULL) {
            delete [] pixels;
            pixels = NULL;
        }
        glDeleteBuffers(frames * framesH, TBO);
        if (tex != 0)
        {
            glDeleteTextures(1, &tex);
        }
        tex = 0;//NULL;
    }
};

class Audio {
public:
    Mix_Music* msc;
    Mix_Chunk* snd;
    int isMusic;
    float volume;
    bool playing = false;

    Audio(unsigned char *buffer, int size, int isMusic) {
        this->isMusic = isMusic;
        this->volume = 1.0f;

        if (isMusic > 0)
            snd = Mix_LoadWAV_RW(SDL_RWFromMem(buffer, (int)size), SDL_TRUE);//msc = Mix_LoadMUS_RW(SDL_RWFromMem(buffer, (int)size), SDL_TRUE);
        else
            snd = Mix_LoadWAV_RW(SDL_RWFromMem(buffer, (int)size), SDL_TRUE);
    }
    Audio(const char* str, int isMusic) {
        this->isMusic = isMusic;
        this->volume = 1.0f;

        if (isMusic > 0) {
            msc = Mix_LoadMUS(str);
            if (msc == NULL)
                printf("ERROR: Could not find file %s\n", str);
        }
        else {
            snd = Mix_LoadWAV(str);
            if (snd == NULL)
                printf("ERROR: Could not find file %s\n", str);
        }
    }
    Audio(SDL_RWops* rw, int isMusic) {
        this->isMusic = isMusic;
        this->volume = 1.0f;

        if (isMusic > 0)
            msc = Mix_LoadMUS_RW(rw, SDL_TRUE);
        else
            snd = Mix_LoadWAV_RW(rw, SDL_TRUE);
    };

    void Play(int loops) {
        if (isMusic > 0) { // -1 loops
            //Mix_PlayMusic(msc, loops);
            //Mix_PlayChannel(isMusic - 1, snd, loops);
            SetVolume(this->volume);
        }
        else { // -1 whatever channel
            Mix_PlayChannel(2 + loops, snd, 0);
            SetVolume(this->volume);
        }
        playing = true;
    }
    bool IsPlaying() {
        if (Mix_Playing(isMusic > 0 ? isMusic - 1 : 2) == 0) playing = false;
        return playing;
    }
    void Pause() {
        if (isMusic > 0) { // -1 loops
            //Mix_Pause(isMusic - 1);
        }
        else { // -1 whatever channel
            Mix_Pause(2);
        }
        playing = false;
    }
    void Resume() {
        if (isMusic > 0) { // -1 loops
            //Mix_Resume(isMusic - 1);
        }
        else { // -1 whatever channel
            Mix_Resume(2);
        }
        playing = true;
    }
    void Stop() {
        if (isMusic > 0) { // -1 loops
            //Mix_HaltChannel(isMusic - 1);
        }
        else {
            Mix_HaltChannel(2);
        }
        playing = false;
    }
    void Free() {
        if (this->isMusic > 0) {
            //Mix_FreeChunk(this->snd);
            //this->snd = NULL;
            Mix_FreeMusic(this->msc);
            this->msc = NULL;
        }
        else {
            Mix_FreeChunk(this->snd);
            this->snd = NULL;
        }
        playing = false;
    }
    void SetVolume(float percent) {
        if (this->isMusic > 0) {
            this->volume = std::max(std::min(percent, 1.0f), 0.0f);
            //Mix_VolumeChunk(this->snd, (int)(128 * this->volume * /*Settings_Music_Volume*/ 0.1f));//Mix_VolumeMusic((int)(128 * this->volume * /*Settings_Music_Volume*/ 0.05f));
        }
        else {
            this->volume = std::max(std::min(percent, 1.0f), 0.0f);
            Mix_VolumeChunk(this->snd, (int)(128 * this->volume * /*Settings_Music_Volume*/ 1.0f));
        }
    }
};

class MusicStackNode {
public:
    class MusicStackNode    *Next = NULL;
    Audio                   *Data = NULL;
    unsigned int  Priority = 0;
    float FadeInDuration = 0.0f;
    float FadeOutDuration = 0.0f;
    float Progress = 0.0f;
    float LoopStart = 0.0f;
    bool  Looping = false;
    bool  Stopping = false;
    bool  ClearAfter = false;
    char Label[32];
};

namespace {
    MusicStack *musicStack;
};

class MusicStack {
public:
    class MusicStackNode *Top = NULL;
    class MusicStackNode *ToAddToTop = NULL;
    class MusicStackNode *ToRemove = NULL;

    int Bits;
    int Rate;
    int Channels;
    int SampleSize;
    bool Paused;
    Uint16 Format;

    MusicStack() {
        musicStack = this;

        Mix_QuerySpec(&Rate, &Format, &Channels);
    	Bits = Format & 0xFF;
    	SampleSize = Channels + Bits / 8;

        Mix_HookMusicFinished(&MusicStack::_Finished);
        Mix_RegisterEffect(MIX_CHANNEL_POST, &MusicStack::_PassThrough, NULL, this);
    }

    static void _PassThrough(int chan, void *stream, int len, void *udata) {
        //printf("=============: Music (%s) .\n", musicStackTop->Label); //Top->Label
        if (musicStack->Paused) return;
        if (musicStack->Top == NULL) return;
        musicStack->Top->Progress += (float)len / musicStack->SampleSize / musicStack->Rate;
    }
    static void _Finished() {
        if (musicStack->Top == NULL) return;

        //printf("=============: Music (%s) finished [loop=%s, stopping=%s].\n", musicStack->Top->Label, musicStack->Top->Looping ? "true" : "false", musicStack->Top->Stopping ? "true" : "false");

        if (musicStack->Top->Looping && !musicStack->Top->Stopping) {
            musicStack->Top->Progress = musicStack->Top->LoopStart;
            Mix_PlayMusic(musicStack->Top->Data->msc, musicStack->Top->Looping ? /*-1*/0 : 0);
            Mix_VolumeMusic(0xC0);
            Mix_SetMusicPosition(musicStack->Top->Progress);
        }

        //printf("=============: Music (%s) finished [loop=%s, stopping=%s].\n", musicStack->Top->Label, musicStack->Top->Looping ? "true" : "false", musicStack->Top->Stopping ? "true" : "false");
        if (musicStack->Top->ClearAfter) {
            //printf("=============: Cleared\n");
            musicStack->Clear();
        }
        else {
            if (!musicStack->Top->Looping || musicStack->Top->Stopping) {
                if (musicStack->ToAddToTop != NULL) {
                    musicStack->Top->Stopping = false;
                    musicStack->ToAddToTop->Next = musicStack->Top;
                    musicStack->Top = musicStack->ToAddToTop;
                    musicStack->ToAddToTop = NULL;
                    musicStack->_Play(musicStack->Top);
                }
                else {
                    musicStack->Top = musicStack->Top->Next;
                    //printf("=============: Music (%s) beginning [%s, %s].\n", musicStack->Top->Label, musicStack->Top->Looping ? "true" : "false", musicStack->Top->Stopping ? "true" : "false");
                    musicStack->_Play(musicStack->Top);
                }
            }
        }
    }

    void _Play(class MusicStackNode *next) {
        if (next == NULL) {
            //printf("=============: Sup\n");
            return;
        }
        Mix_HaltMusic();
        Mix_PlayMusic(next->Data->msc, next->Looping ? /*-1*/0 : 0);
        Mix_VolumeMusic(0xC0);
        Mix_SetMusicPosition(next->Progress);
    }

    void Push(const char *label, Audio *data, unsigned int priority, float fadeIn, float fadeOut, bool loop, bool overridable) {
        Push(label, data, priority, fadeIn, fadeOut, loop, overridable, false, 0.0f);
    }
    void Push(const char *label, Audio *data, unsigned int priority, float fadeIn, float fadeOut, bool loop, bool overridable, bool clearAfter) {
        Push(label, data, priority, fadeIn, fadeOut, loop, overridable, clearAfter, 0.0f);
    }
    void Push(const char *label, Audio *data, unsigned int priority, float fadeIn, float fadeOut, bool loop, bool overridable, bool clearAfter, float loopPoint) {
        musicStack = this;

        class MusicStackNode *checkFirst = Find(label);
        if (checkFirst != NULL) {
            if (checkFirst == Top) {
                Mix_RewindMusic();
                checkFirst->Progress = 0;
                return;
            }
        }

        class MusicStackNode *next = Top;
        class MusicStackNode *prev = NULL;
        if (Top == NULL) {
            Top = new class MusicStackNode();
            Top->Data = data;
            Top->Priority = priority;
            Top->FadeInDuration  = fadeIn;
            Top->FadeOutDuration = fadeOut;
            Top->Looping = loop;
            Top->LoopStart = loopPoint;
            Top->Stopping = !loop;
            Top->ClearAfter = clearAfter;
            Top->Next = NULL;
            strcpy(Top->Label, label);

            _Play(Top);
            return;
        }
        do {
            if (priority >= next->Priority) {
                break;
            }
            prev = next;
            next = next->Next;
        }
        while (next != NULL);

        class MusicStackNode *Node = new class MusicStackNode();
        Node->Data = data;
        Node->Priority = priority;
        Node->FadeInDuration  = fadeIn;
        Node->FadeOutDuration = fadeOut;
        Node->Looping = loop;
        Node->LoopStart = loopPoint;
        Node->Stopping = !loop;
        Node->ClearAfter = clearAfter;
        Node->Next = next;
        strcpy(Node->Label, label);
        if (Top == next) {
            ToAddToTop = Node;
            Top->Stopping = true;
            Mix_FadeOutMusic((int)(fadeOut * 1000) + 1);
        }
        else if (overridable) {
            prev->Next = Node;
        }
        else {
            free(Node);
            Node = NULL;
        }
    }

    void Pause() {
        Paused = true;
        Mix_PauseMusic();
    }
    void Resume() {
        Paused = false;
        Mix_ResumeMusic();
    }

    void Clear() {
        class MusicStackNode *current = Top;
        class MusicStackNode *currentNext = NULL;

        if (Top == NULL) return;

        do {
            currentNext = current->Next;
            current->Next = NULL;
            current = currentNext;
        }
        while (current != NULL);
        current = NULL;
        currentNext = NULL;
        Top = NULL;
    }
    void Remove(const char *label) {
        class MusicStackNode *current = Top;
        class MusicStackNode *currentPrev = NULL;

        if (current == NULL) return;

        do {
            if (strcmp(current->Label, label) == 0) {
                if (current == Top) {
                    Top->Stopping = true;
                    Mix_FadeOutMusic((int)(1 * 1000));
                }
                else
                    currentPrev->Next = current->Next;

                //free(current);
                current = NULL;
                return;
            }
            currentPrev = current;
            current = current->Next;
        }
        while (current != NULL);
    }
    class MusicStackNode *Find(const char *label) {
        class MusicStackNode *current = NULL;
        current = Top;
        if (current == NULL) return NULL;

        while (current != NULL) {
            if (strcmp(current->Label, label) == 0) {
                return current;
            }
            current = current->Next;
        }
        return NULL;
    }
};

#endif