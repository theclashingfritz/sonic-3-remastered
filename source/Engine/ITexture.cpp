#include "ITexture.h"

#include <Application.h>

void ITexture::CreateTexture(Application* MainApp, SDL_Surface* base_surface, bool save_pixels) {
    if (MainApp != NULL)
        SDL_GL_MakeCurrent(MainApp->window, MainApp->context);

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(base_surface, SDL_PIXELFORMAT_ABGR8888, 0);

    Width = surface->w;
    Height = surface->h;

    glGenTextures(1, &GL_Tex);
    glBindTexture(GL_TEXTURE_2D, GL_Tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        surface->w, surface->h, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, surface->pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    if (save_pixels) {
        Pixels = (unsigned char*)malloc(surface->w * surface->h * 4);
        memcpy(Pixels, (unsigned char*)surface->pixels, surface->w * surface->h * 4);
    }

    //Free up the memory.
    SDL_FreeSurface(surface);
    SDL_FreeSurface(base_surface);

    surface = NULL;
    base_surface = NULL;
}
void ITexture::Free() {
    glDeleteTextures(1, &GL_Tex);
}

void IVideo::Load() {
    CurrentFrame = 0;
    Length = 0;
    Texture = new ITexture();

    glGenTextures(1, &Texture->GL_Tex);

    #if !defined(PLATFORM_WINDOWS)
        #ifdef PLATFORM_MAC
        Reader = cv::VideoCapture("/Users/Justin/dropbox/sonic3/source/Resource/Movies/Mania LQ.mp4");
        #else
        Reader = cv::VideoCapture("Resource/Movies/Mania LQ.mp4");
        #endif
        printf("Frame Rate: %f\n", Reader.get(CV_CAP_PROP_FPS));
        fflush(stdout);

        for (int i = 0; i < ChunkSize * 0; i++) {
            if (!Reader.read(Frames[i])) {
                Finished = true;
                break;
            }
            Length++;
            FramesLoaded++;
        }

        Finished = false;

        NextTimeTick = SDL_GetTicks() + 1000;
        Thread = SDL_CreateThread(&IVideo::LoadNext, "vg.aurum.Sonic3.VideoThread", (void*)this);
    #else
        Finished = true;
    #endif
}
int IVideo::LoadNext(void* data) {
    #if !defined(PLATFORM_WINDOWS)
        IVideo* me = (IVideo*)data;
        bool running = false;

    	//SDL_Delay(10);

        while (running) {
            if (me->FramesLoaded == 0) {
                for (int i = 0; i < me->ChunkSize; i++) {
                    //printf("Reading frame %d....\n", me->Length);
                    //fflush(stdout);
                    if (!me->Reader
    					.read(
    						  me->
    						  NextFrames[
    									 i])) {
                        running = false;
                        me->Finished = true;
                        return 1;
                    }
                    me->Length++;
                    me->FramesLoaded++;
                }
    		}
    		SDL_Delay(10);
        }
    #endif
    return 1;
}
void IVideo::NextFrame() {
    NextFrame((int)CurrentFrame++);
}

void IVideo::NextFrame(int Frame) {
    if (CurrentFrame == Frame)
        return;

	if (Finished)
		return;

    CurrentFrame = Frame;

    #if !defined(PLATFORM_WINDOWS)
        cv::Mat Matrix;
        Matrix.release();
    	if (!Reader.read(Matrix)) {
    		Finished = true;
            return;
    	}

        glBindTexture(GL_TEXTURE_2D, Texture->GL_Tex);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D,
            0, GL_RGB,
            Matrix.cols, Matrix.rows,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            Matrix.data);
        glBindTexture(GL_TEXTURE_2D, 0);

    	if (/* DISABLES CODE */ (!false))
    		return;
        //*/

        CurrentFrame = Frame;

        if (CurrentFrame % ChunkSize >= ChunkSize - 1 && FramesLoaded > 0) {
    		FramesLoaded = 0;

    		printf("Reloading.\n");
    		fflush(stdout);

            for (int i = 0; i < ChunkSize; i++)
                Frames[i].release();
            memcpy(Frames, NextFrames, sizeof(Frames));

            //for (int i = 0; i < ChunkSize; i++)
                //NextFrames[i].release();
    		//memset(NextFrames, 0x00, sizeof(NextFrames));

            NextTimeTick = SDL_GetTicks() + (long)(1000 * (24 / 23.976f));

        }

    	glBindTexture(GL_TEXTURE_2D, Texture->GL_Tex);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Frames[CurrentFrame % ChunkSize].cols, Frames[CurrentFrame % ChunkSize].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, Frames[CurrentFrame % ChunkSize].data);
        glTexImage2D(GL_TEXTURE_2D,
            0, GL_RGB,
            Frames[CurrentFrame % ChunkSize].cols, Frames[CurrentFrame % ChunkSize].rows,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            Frames[CurrentFrame % ChunkSize].data);
        glBindTexture(GL_TEXTURE_2D, 0);
    #endif
}
