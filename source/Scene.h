#ifndef SCENE_H
#define SCENE_H

#include "Standard.h"

#include "Engine/IGraphics.h"

class Scene {
public:
	Application* app;
    Scene* sc = NULL;

	IGraphics* G = NULL;
	Application* App = NULL;

	// Debug Texture
	Texture* tex_DebugFont = NULL;
    Audio* aud_BGM_Act1 = NULL;
    Audio* aud_BGM_Act2 = NULL;

    // Global Audio
    Audio* aud_BadnikDestroyed = NULL;
	Audio* aud_BGM_ActClear = NULL;
	Audio* aud_BGM_Boss1 = NULL;
    Audio* aud_BGM_Boss2 = NULL;
    Audio* aud_BGM_DataSelect = NULL;
    Audio* aud_BGM_Drowning = NULL;
    Audio* aud_BGM_HCZ1 = NULL;
    Audio* aud_BGM_HCZ2 = NULL;
    Audio* aud_BGM_Invincibility = NULL;
    Audio* aud_BGM_LifeGet = NULL;
    Audio* aud_BGM_SEGA = NULL;
    Audio* aud_BGM_SpecialStage = NULL;
    Audio* aud_BGM_TitleScreen = NULL;
    Audio* aud_BIGLaser = NULL;
    Audio* aud_BombDrop = NULL;
    Audio* aud_BossHit = NULL;
    Audio* aud_Break = NULL;
    Audio* aud_BreakDeeper = NULL;
    Audio* aud_BubbleGet = NULL;
    Audio* aud_Checkpoint = NULL;
    Audio* aud_C_ChaosEmerald = NULL;
    Audio* aud_C_ChaosEmeraldContinue = NULL;
    Audio* aud_C_Drown = NULL;
    Audio* aud_C_EnemyShot = NULL;
    Audio* aud_C_IntoLargeRing = NULL;
    Audio* aud_C_ResultsRings = NULL;
    Audio* aud_C_RingLoss = NULL;
    Audio* aud_C_ShortBeep = NULL;
    Audio* aud_C_TailsFly = NULL;
    Audio* aud_C_ToBonusStage = NULL;
    Audio* aud_C_ToSpecialStage = NULL;
    Audio* aud_Die = NULL;
    Audio* aud_ExtraLife = NULL;
    Audio* aud_Jump = NULL;
    Audio* aud_Ring = NULL;
    Audio* aud_Roll = NULL;
    Audio* aud_ShieldBounce = NULL;
    Audio* aud_ShieldBubble = NULL;
    Audio* aud_ShieldEJump = NULL;
    Audio* aud_ShieldElectric = NULL;
    Audio* aud_ShieldFire = NULL;
    Audio* aud_Skid = NULL;
    Audio* aud_Spiked = NULL;
    Audio* aud_SpikeHurt = NULL;
    Audio* aud_SpinRev = NULL;
    Audio* aud_Spring = NULL;
    Audio* aud[0xDF];

	void drawLongFont(Texture* fnt, const char* text, float x, float y, float align);
	void drawText(Texture* texture, const char* text, float x, float y, int xa, int ya, int type);
	void drawText(Texture* texture, const char* text, float x, float y, int xa, int ya, int type, char offset);
	void DrawTextDebug(const char* text, float x, float y, int xa, int ya);
	void setPixel(unsigned char* pixels, int x, int y, short rgb);
	void setPixel(unsigned char* pixels, int x, int y, char r, char g, char b);
	void setPixelFull(unsigned char* pixels, int x, int y, char r, char g, char b);

	void DrawLongFont(ISprite* fnt, const char* text, float x, float y, float align);
	void DrawSmallFont(ISprite* fnt, const char* text, float x, float y, float align);


	virtual void Load() { }
	virtual void Update() { }
    virtual void Render() { }
    virtual void Free() { }
	virtual void PostMixProcess(void *udata, Uint8 *stream, int len) { }
	void FreeAudio();
};

#endif
