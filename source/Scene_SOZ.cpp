#include "Standard.h"
#include "Scene_MainMenu.h"
#include "Application.h"
#include "Resources.h"
#include "Scene_SOZ.h"
#include "S3Object.h"
#include "LevelScene.h"

#include "Scene_LRZ.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) App->Textures[name]
#define S(name) App->Sprites[name]

// Unique
const char LevelID = 0x90;
// Modularized
Scene_SOZ::Scene_SOZ(Application* app, int act, int checkpoint) {
    SDL_RWops *_RWList[35] = {
        newFile(res_SOZ_level_lvl, "Resource/Stages/SOZ/level.lvl"),
        // Musics
        newFile(res_SOZ_act_1_ogg, "Resource/Stages/SOZ/Music/Act_1.ogg"),
        newFile(res_SOZ_act_2_ogg, "Resource/Stages/SOZ/Music/Act_2.ogg"),
        // Layouts
        newFile(res_SOZ_layout_1_bin, "Resource/Stages/SOZ/Data (Layout)/1.bin"), // Pre-Act
        newFile(res_SOZ_layout_1_bin, "Resource/Stages/SOZ/Data (Layout)/1.bin"), // Act 1
        newFile(res_SOZ_layout_2_bin, "Resource/Stages/SOZ/Data (Layout)/2.bin"),
        newFile(res_SOZ_layout_2_bin, "Resource/Stages/SOZ/Data (Layout)/2.bin"),
        // Chunks (128x128)
        newFile(res_SOZ_chunks_1_bin, "Resource/Stages/SOZ/Data (Chunks)/1.bin"), // Pre-Act
        newFile(res_SOZ_chunks_1_bin, "Resource/Stages/SOZ/Data (Chunks)/1.bin"), // Act 1
        newFile(res_SOZ_chunks_2_bin, "Resource/Stages/SOZ/Data (Chunks)/2.bin"),
        newFile(res_SOZ_chunks_2_bin, "Resource/Stages/SOZ/Data (Chunks)/2.bin"),
        // Tiles (16x16)
        newFile(res_SOZ_tiles_0_bin, "Resource/Stages/SOZ/Data (Tiles)/0.bin"), // Pre-Act
        newFile(res_SOZ_tiles_1_bin, "Resource/Stages/SOZ/Data (Tiles)/1.bin"), // Act 1
        newFile(res_SOZ_tiles_2_bin, "Resource/Stages/SOZ/Data (Tiles)/2.bin"),
        newFile(res_SOZ_tiles_2_bin, "Resource/Stages/SOZ/Data (Tiles)/2.bin"),
        // Rings
        newFile(res_SOZ_rings_2_bin, "Resource/Stages/SOZ/Data (Rings)/1.bin"), // Pre-Act
        newFile(res_SOZ_rings_1_bin, "Resource/Stages/SOZ/Data (Rings)/1.bin"), // Act 1
        newFile(res_SOZ_rings_2_bin, "Resource/Stages/SOZ/Data (Rings)/2.bin"),
        newFile(res_SOZ_rings_2_bin, "Resource/Stages/SOZ/Data (Rings)/2.bin"),
        // Objects
        newFile(res_SOZ_objects_1_bin, "Resource/Stages/SOZ/Data (Objects)/1.bin"), // Pre-Act
        newFile(res_SOZ_objects_1_bin, "Resource/Stages/SOZ/Data (Objects)/1.bin"), // Act 1
        newFile(res_SOZ_objects_2_bin, "Resource/Stages/SOZ/Data (Objects)/2.bin"),
        newFile(res_SOZ_objects_2_bin, "Resource/Stages/SOZ/Data (Objects)/2.bin"),
        // Palettes (Dry)
        newFile(res_SOZ_PAL1_DRY_png, "Resource/Stages/SOZ/Palettes/PAL1_DRY.png"), // Pre-Act
        newFile(res_SOZ_PAL1_DRY_png, "Resource/Stages/SOZ/Palettes/PAL1_DRY.png"), // Act 1
        newFile(res_SOZ_PAL2_DRY_png, "Resource/Stages/SOZ/Palettes/PAL2_DRY.png"),
        newFile(res_SOZ_PAL2_DRY_png, "Resource/Stages/SOZ/Palettes/PAL2_DRY.png"),
        // Palettes (Wet)
        newFile(res_SOZ_PAL1_WET_png, "Resource/Stages/SOZ/Palettes/PAL1_WET.png"), // Pre-Act
        newFile(res_SOZ_PAL1_WET_png, "Resource/Stages/SOZ/Palettes/PAL1_WET.png"), // Act 1
        newFile(res_SOZ_PAL2_WET_png, "Resource/Stages/SOZ/Palettes/PAL2_WET.png"),
        newFile(res_SOZ_PAL2_WET_png, "Resource/Stages/SOZ/Palettes/PAL2_WET.png"),
        // Palettes (Other)
        newFile(res_SOZ_PALm_DRY_png, "Resource/Stages/SOZ/Palettes/PAL1_DRY.png"), // Mini-boss
        newFile(res_SOZ_PALm_DRY_png, "Resource/Stages/SOZ/Palettes/PAL1_DRY.png"), // Boss
        newFile(res_SOZ_PALm_WET_png, "Resource/Stages/SOZ/Palettes/PAL1_WET.png"), // Extra 1 (Mini-boss)
        newFile(res_SOZ_PALm_WET_png, "Resource/Stages/SOZ/Palettes/PAL1_WET.png"), // Extra 2
    };

    App = app;

    memcpy(RWList, _RWList, sizeof(_RWList));
    Init(app, LevelID + act, checkpoint);
}
void Scene_SOZ::DoSwitchActZone() {
	if (!doSwitch) return;
	doSwitch = false;

    if (ActualAct == 2) {
        Scene_LRZ* nextLevel = new Scene_LRZ(App, 1, -1);
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
void Scene_SOZ::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {
    int a = act & 0xF;

    if (a == 1)
        Tileset = new ITexture(IResources::Load("Stages/SOZ/Sprites/TILES1.png"), false);
    else if (a == 2)
        Tileset = new ITexture(IResources::Load("Stages/SOZ/Sprites/TILES2.png"), false);

    // Parallax Sizes and Auto Scrolls
    if (a <= 1) {
        int PS[] =    {  0x0040,   0x0008,   0x0008,   0x0005,   0x0005,   0x0006,   0x00F0,   0x0006,   0x0005,   0x0005,   0x0008,   0x0008,   0x0030,   0x80C0,   0x7FFF };
        float PAS[] = {  0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,        0,     };
        float PM[] =  {  0.250f, 0.21875f,  0.1875f, 0.15625f,   0.125f, 0.09375f,  0.0625f, 0.09375f,   0.125f, 0.15625f,  0.1875f, 0.21875f,   0.250f,   0.250f,   0.125f };

        // $FFFFA800 - BG deformation RAM address
        // d0 = 1 / 4
        // d1 = 1 / 32

        // 0x00: 1 / 4
        // 0x02: 1 / 4 - 1 / 32
        // 0x04: 1 / 4 - 1 / 32 - 1 / 32
        // 0x06: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x08: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x0A: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x0C: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x0E: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x10: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x12: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32
        // 0x14: 1 / 4 - 1 / 32 - 1 / 32
        // 0x16: 1 / 4 - 1 / 32
        // 0x18: 1 / 4
        //0x19A: 1 / 4 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32 - 1 / 32
        // d3 = 1 / 4

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
void Scene_SOZ::CreateUniqueTextures(Application* app, int act) {
	unsigned int nextTick = SDL_GetTicks();

    T("SOZ") = new ITexture(IResources::Load("Stages/SOZ/Sprites/Texture.png"), false);



	App->print(0, "UNIQUE Texture load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void Scene_SOZ::UnloadUniqueTextures(Application* app, int act) {

}

void Scene_SOZ::Update() {
    BGx = 1.f;
    BGy = 1.f / 4.f;
    BGstart = 0;
    BGsize = 4;

	LevelScene::Update();

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

int Scene_SOZ::CustomAnimTile(int ind, int i, int parRow) {
	int tileID = AnimTiles[i * 5 + 0];
	int tileCount = AnimTiles[i * 5 + 1];
	int aniID = AnimTiles[i * 5 + 2];
	int frmCount = AnimTiles[i * 5 + 3];
	int frmSpd = AnimTiles[i * 5 + 4];

    if (ActualAct == 1) {
//%%AnimTiles1%%
    }
    else {
//%%AnimTiles2%%
    }
	return ind;
}

// OVERRIDES
void Scene_SOZ::HandleCamera() {
	LevelScene::HandleCamera();

	if (MyPlayer->Action == ActionType::Dead)
		return;
}
void Scene_SOZ::DrawAboveEverythingNonHUD() {
	LevelScene::DrawAboveEverythingNonHUD();
    return;
}
void Scene_SOZ::Render() {
	LevelScene::Render();
}
void Scene_SOZ::Free() {
	LevelScene::Free();
    UnloadUniqueTextures(app, 1);
    UnloadUniqueTextures(app, 2);
    unloadAnimatedChunks(app);
}
