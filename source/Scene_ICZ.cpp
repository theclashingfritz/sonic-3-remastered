#include "Standard.h"
#include "Scene_MainMenu.h"
#include "Application.h"
#include "Resources.h"
#include "Scene_ICZ.h"
#include "S3Object.h"
#include "LevelScene.h"

#include "Scene_LBZ.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) App->Textures[name]
#define S(name) App->Sprites[name]

// Unique
const char LevelID = 0x50;
// Modularized
Scene_ICZ::Scene_ICZ(Application* app, int act, int checkpoint) {
    SDL_RWops *_RWList[35] = {
        newFile(res_ICZ_level_lvl, "Resource/Stages/ICZ/level.lvl"),
        // Musics
        newFile(res_ICZ_act_1_ogg, "Resource/Stages/ICZ/Music/Act 1.ogg"),
        newFile(res_ICZ_act_2_ogg, "Resource/Stages/ICZ/Music/Act 2.ogg"),
        // Layouts
        newFile(res_ICZ_layout_1_bin, "Resource/Stages/ICZ/Data (Layout)/1.bin"), // Pre-Act
        newFile(res_ICZ_layout_1_bin, "Resource/Stages/ICZ/Data (Layout)/1.bin"), // Act 1
        newFile(res_ICZ_layout_2_bin, "Resource/Stages/ICZ/Data (Layout)/2.bin"),
        newFile(res_ICZ_layout_2_bin, "Resource/Stages/ICZ/Data (Layout)/2.bin"),
        // Chunks (128x128)
        newFile(res_ICZ_chunks_1_bin, "Resource/Stages/ICZ/Data (Chunks)/1.bin"), // Pre-Act
        newFile(res_ICZ_chunks_1_bin, "Resource/Stages/ICZ/Data (Chunks)/1.bin"), // Act 1
        newFile(res_ICZ_chunks_2_bin, "Resource/Stages/ICZ/Data (Chunks)/2.bin"),
        newFile(res_ICZ_chunks_2_bin, "Resource/Stages/ICZ/Data (Chunks)/2.bin"),
        // Tiles (16x16)
        newFile(res_ICZ_tiles_0_bin, "Resource/Stages/ICZ/Data (Tiles)/0.bin"), // Pre-Act
        newFile(res_ICZ_tiles_1_bin, "Resource/Stages/ICZ/Data (Tiles)/1.bin"), // Act 1
        newFile(res_ICZ_tiles_2_bin, "Resource/Stages/ICZ/Data (Tiles)/2.bin"),
        newFile(res_ICZ_tiles_2_bin, "Resource/Stages/ICZ/Data (Tiles)/2.bin"),
        // Rings
        newFile(res_ICZ_rings_2_bin, "Resource/Stages/ICZ/Data (Rings)/1.bin"), // Pre-Act
        newFile(res_ICZ_rings_1_bin, "Resource/Stages/ICZ/Data (Rings)/1.bin"), // Act 1
        newFile(res_ICZ_rings_2_bin, "Resource/Stages/ICZ/Data (Rings)/2.bin"),
        newFile(res_ICZ_rings_2_bin, "Resource/Stages/ICZ/Data (Rings)/2.bin"),
        // Objects
        newFile(res_ICZ_objects_1_bin, "Resource/Stages/ICZ/Data (Objects)/1.bin"), // Pre-Act
        newFile(res_ICZ_objects_1_bin, "Resource/Stages/ICZ/Data (Objects)/1.bin"), // Act 1
        newFile(res_ICZ_objects_2_bin, "Resource/Stages/ICZ/Data (Objects)/2.bin"),
        newFile(res_ICZ_objects_2_bin, "Resource/Stages/ICZ/Data (Objects)/2.bin"),
        // Palettes (Dry)
        newFile(res_ICZ_PAL1_DRY_png, "Resource/Stages/ICZ/Palettes/PAL1_DRY.png"), // Pre-Act
        newFile(res_ICZ_PAL1_DRY_png, "Resource/Stages/ICZ/Palettes/PAL1_DRY.png"), // Act 1
        newFile(res_ICZ_PAL2_DRY_png, "Resource/Stages/ICZ/Palettes/PAL2_DRY.png"),
        newFile(res_ICZ_PAL2_DRY_png, "Resource/Stages/ICZ/Palettes/PAL2_DRY.png"),
        // Palettes (Wet)
        newFile(res_ICZ_PAL1_WET_png, "Resource/Stages/ICZ/Palettes/PAL1_WET.png"), // Pre-Act
        newFile(res_ICZ_PAL1_WET_png, "Resource/Stages/ICZ/Palettes/PAL1_WET.png"), // Act 1
        newFile(res_ICZ_PAL2_WET_png, "Resource/Stages/ICZ/Palettes/PAL2_WET.png"),
        newFile(res_ICZ_PAL2_WET_png, "Resource/Stages/ICZ/Palettes/PAL2_WET.png"),
        // Palettes (Other)
        newFile(res_ICZ_PALm_DRY_png, "Resource/Stages/ICZ/Palettes/PAL1_DRY.png"), // Mini-boss
        newFile(res_ICZ_PALm_DRY_png, "Resource/Stages/ICZ/Palettes/PAL1_DRY.png"), // Boss
        newFile(res_ICZ_PALm_WET_png, "Resource/Stages/ICZ/Palettes/PAL1_DRY.png"), // Extra 1 (Mini-boss)
        newFile(res_ICZ_PALm_WET_png, "Resource/Stages/ICZ/Palettes/PAL1_DRY.png"), // Extra 2
    };

    App = app;

    memcpy(RWList, _RWList, sizeof(_RWList));
    Init(app, LevelID + act, checkpoint);
}
void Scene_ICZ::DoSwitchActZone() {
	if (!doSwitch) return;
	doSwitch = false;

    if (ActualAct == 2) {
        Scene_LBZ* nextLevel = new Scene_LBZ(App, 1, -1);
        nextLevel->score = score;
        nextLevel->lives = lives;
        App->nextScene = nextLevel;
        return;
    }

    int ObjectPlayerOffsetX = 0x0000;
    int ObjectPlayerOffsetY = 0x0000;

	MyPlayer->X += ObjectPlayerOffsetX;
	MyPlayer->Y += ObjectPlayerOffsetY;

	act = LevelID + ActualAct + 1;

	InitZone(true, 0, -1, true);

	cameraX[App->CurrentViewport] += ObjectPlayerOffsetX;
	cameraY[App->CurrentViewport] += ObjectPlayerOffsetY;
	cameraMinX = 0;
	cameraMinY = 0;
}
// Unique
void Scene_ICZ::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {
    int a = act & 0xF;

    if (a == 1)
        Tileset = new ITexture(IResources::Load("Stages/ICZ/Sprites/TILES1.png"), false);
    else if (a == 2)
        Tileset = new ITexture(IResources::Load("Stages/ICZ/Sprites/TILES2.png"), false);

    // Parallax Sizes and Auto Scrolls
    if (a <= 1) {
        int PS[] =    { 0x00000044, 0x0000000C, 0x0000000B, 0x0000000D, 0x00000018, 0x00000050, 0x00000002, 0x00000006, 0x00000008, 0x00000010, 0x00000018, 0x00000020, 0x00000028, 0x00007FFF };
        float PAS[] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
        float PM[] =  { 0.0312500f, 0.0625000f, 0.0937500f, 0.1250000f, 0.1562500f, 0.2187500f, 0.2656250f, 0.3125000f, 0.3593750f, 0.4062500f, 0.4531250f, 0.5009765f, 0.5488281f, 0.5966797f };

        memcpy(ParallaxAutoScroll, PAS, sizeof(ParallaxAutoScroll));
        memcpy(ParallaxSizes, PS, sizeof(ParallaxSizes));
        memcpy(ParallaxMult, PM, sizeof(PM));
        ParallaxCount = sizeof(PS) / sizeof(*PS);
    }
    else {
        int PS[] =    { 0x0008, 0x0008, 0x0090, 0x0010, 0x0008, 0x0030, 0x0018, 0x0008, 0x0008, 0x00A8, 0x0030, 0x0018, 0x0008, 0x0008, 0x00A8, 0x0030, 0x0018, 0x0008, 0x0008, 0x00B0, 0x0010, 0x0008, 0x7FFF };
        float PAS[] = { 0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,     };
        float PM[] =  { 0.3750f,0.3125f,0.1250f,0.1875f,0.3750f,0.5000f,0.4375f,0.3750f,0.3125f,0.2500f,0.5000f,0.4375f,0.3750f,0.3125f,0.2500f,0.5000f,0.4375f,0.3750f,0.3125f,0.1250f,0.1875f,0.3750f,0.5000f };

        memcpy(ParallaxAutoScroll, PAS, sizeof(PAS));
        memcpy(ParallaxSizes, PS, sizeof(PS));
        memcpy(ParallaxMult, PM, sizeof(PM));
        ParallaxCount = sizeof(PS) / sizeof(*PS);
    }

    // Animated Tiles
    AnimTilesCount = 0;

    LevelScene::InitZone(resetTextures, check, specialRing, actTransition);
}
void Scene_ICZ::CreateUniqueTextures(Application* app, int act) {
	unsigned int nextTick = SDL_GetTicks();

    T("AIZ") = new ITexture(IResources::Load("Stages/AIZ/Sprites/Texture.png"), false);



	App->print(0, "UNIQUE Texture load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void Scene_ICZ::UnloadUniqueTextures(Application* app, int act) {

}

void Scene_ICZ::Update() {
    loop = true;

	LevelScene::Update();

    if (cameraX[0] < 0x3900 && BGxoff != 0) {
        BGx = 1.f / 4.f;
        BGy = 0;
        BGxoff = 0;
        BGstart = 49 + 4 - 1;
        BGsize = 4;

        short IntroPalette[15] = {
            0xEEE, 0xEEC,
            0xEEA, 0xECA,
            0xEC8, 0xEA6,
    		0xE86, 0xE64,
    		0xE40, 0xE00,
    		0xC00, 0x000,
    		0xAEC, 0xCEA,
    		0xE80
        };
        for (int i = 0; i < 15; i++) {
            setPixel(paletteDry->pixels, 3, 1 + i, IntroPalette[i]);
            //setPixel(paletteWet->pixels, 3, 1 + i, IntroPalette[i]);
        }
    }
    else if (BGxoff != 0x3B00) {
        BGx = 1.f / 2.f;
        BGy = 1.f / 2.f;
        BGxoff = 0x3B00;
        BGstart = 0;
        BGsize = 49;

        float PM[] =  { 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f, 1.0000000f };
        memcpy(ParallaxMult, PM, sizeof(PM));

        short IntroPalette[11] = {
            0xEC0, 0xE40,
            0xE04, 0xC00,
            0x600, 0x200,
            0x000, 0xE64,
            0xE24, 0xA02,
    		0x402
        };
        for (int i = 0; i < 11; i++) {
            setPixel(paletteDry->pixels, 3, 1 + i, IntroPalette[i]);
        }
    }
	// Do palette stuffs

    if (frameAnim % 8 == 0) {
    	int len = 4;
        int pick;

        // Do common palette stuffs (Super Sonic)
        if (MyPlayer->SuperForm || MyPlayer->SuperFormAnim != SuperFormAnimType::None) {
            // Super Sonic
            len = 4;
            pick = 3 * ((frameAnim / 16) % len + 6);

            if (MyPlayer->SuperFormAnim == SuperFormAnimType::Transforming) {
                pick = ((MyPlayer->SuperFormAnimTimerMax - MyPlayer->SuperFormAnimTimer) / 4);
                if (pick < 2)
                    pick = 2;
                pick = 3 * pick;
            }
            else if (MyPlayer->SuperFormAnim == SuperFormAnimType::Deforming) {
                pick = 3 * (MyPlayer->SuperFormAnimTimer / 4);
            }
            else if (MyPlayer->SuperFormAnim == SuperFormAnimType::Super) {
                pick = 0;
            }

            int *PalCycle_ChoseSuper = PalCycle_SuperSonicUnderwaterHCZCNZLBZ;
            if (LevelID == 0x10 || LevelID == 0x50)
                PalCycle_ChoseSuper = PalCycle_SuperSonicUnderwaterAIZICZ;

            setPixel(paletteDry->pixels, 0, 2, (PalCycle_SuperSonic[pick] & 0xF), (PalCycle_SuperSonic[pick] & 0xF0) / 0x10, (PalCycle_SuperSonic[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 0, 2, (PalCycle_ChoseSuper[pick] & 0xF), (PalCycle_ChoseSuper[pick] & 0xF0) / 0x10, (PalCycle_ChoseSuper[pick] & 0xF00) / 0x100);
            pick++;
            setPixel(paletteDry->pixels, 0, 3, (PalCycle_SuperSonic[pick] & 0xF), (PalCycle_SuperSonic[pick] & 0xF0) / 0x10, (PalCycle_SuperSonic[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 0, 3, (PalCycle_ChoseSuper[pick] & 0xF), (PalCycle_ChoseSuper[pick] & 0xF0) / 0x10, (PalCycle_ChoseSuper[pick] & 0xF00) / 0x100);
            pick++;
            setPixel(paletteDry->pixels, 0, 4, (PalCycle_SuperSonic[pick] & 0xF), (PalCycle_SuperSonic[pick] & 0xF0) / 0x10, (PalCycle_SuperSonic[pick] & 0xF00) / 0x100);
            setPixel(paletteWet->pixels, 0, 4, (PalCycle_ChoseSuper[pick] & 0xF), (PalCycle_ChoseSuper[pick] & 0xF0) / 0x10, (PalCycle_ChoseSuper[pick] & 0xF00) / 0x100);
        }

        glBindTexture(GL_TEXTURE_2D, paletteDry->tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0,
            0, 0, paletteDry->w, paletteDry->h,
            GL_RGBA, GL_UNSIGNED_BYTE,
            paletteDry->pixels);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, paletteWet->tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0,
            0, 0, paletteWet->w, paletteWet->h,
            GL_RGBA, GL_UNSIGNED_BYTE,
            paletteWet->pixels);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

int Scene_ICZ::CustomAnimTile(int ind, int i, int parRow) {
    if (ind >= 286 && ind < 286 + 4) {
        int frm = frameAnim % 32;
        if (frm < 4)
            return 2048 + 0 + ind - 286 + 0;
        else if (frm < 8)
            return 2048 + 0 + ind - 286 + 4;
        else if (frm < 12)
            return 2048 + 0 + ind - 286 + 8;
        else if (frm < 16)
            return 2048 + 0 + ind - 286 + 12;
        else if (frm < 20)
            return 2048 + 0 + ind - 286 + 16;
        else if (frm < 24)
            return 2048 + 0 + ind - 286 + 20;
        else if (frm < 28)
            return 2048 + 0 + ind - 286 + 24;
        else if (frm < 32)
            return 2048 + 0 + ind - 286 + 28;
    }
	return ind;
}

// OVERRIDES
void Scene_ICZ::HandleCamera() {
	LevelScene::HandleCamera();

	if (MyPlayer->Action == ActionType::Dead)
		return;
}
void Scene_ICZ::DrawAboveEverythingNonHUD() {
	LevelScene::DrawAboveEverythingNonHUD();
    return;
}
void Scene_ICZ::Render() {
	LevelScene::Render();
}
void Scene_ICZ::Free() {
	LevelScene::Free();
    UnloadUniqueTextures(app, 1);
    UnloadUniqueTextures(app, 2);
    unloadAnimatedChunks(app);
}
