#include "Standard.h"
#include "Scene_MainMenu.h"
#include "Application.h"
#include "Resources.h"
#include "Scene_MHZ.h"
#include "S3Object.h"
#include "LevelScene.h"

#include "Scene_FBZ.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) App->Textures[name]
#define S(name) App->Sprites[name]

// Unique
const char LevelID = 0x70;
// Modularized
Scene_MHZ::Scene_MHZ(Application* app, int act, int checkpoint) {
    SDL_RWops *_RWList[35] = {
        newFile(res_MHZ_level_lvl, "Resource/Stages/MHZ/level.lvl"),
        // Musics
        newFile(res_MHZ_act_1_ogg, "Resource/Stages/MHZ/Music/Act 1.ogg"),
        newFile(res_MHZ_act_2_ogg, "Resource/Stages/MHZ/Music/Act 2.ogg"),
        // Layouts
        newFile(res_MHZ_layout_1_bin, "Resource/Stages/MHZ/Data (Layout)/1.bin"), // Pre-Act
        newFile(res_MHZ_layout_1_bin, "Resource/Stages/MHZ/Data (Layout)/1.bin"), // Act 1
        newFile(res_MHZ_layout_2_bin, "Resource/Stages/MHZ/Data (Layout)/2.bin"),
        newFile(res_MHZ_layout_2_bin, "Resource/Stages/MHZ/Data (Layout)/2.bin"),
        // Chunks (128x128)
        newFile(res_MHZ_chunks_1_bin, "Resource/Stages/MHZ/Data (Chunks)/1.bin"), // Pre-Act
        newFile(res_MHZ_chunks_1_bin, "Resource/Stages/MHZ/Data (Chunks)/1.bin"), // Act 1
        newFile(res_MHZ_chunks_2_bin, "Resource/Stages/MHZ/Data (Chunks)/2.bin"),
        newFile(res_MHZ_chunks_2_bin, "Resource/Stages/MHZ/Data (Chunks)/2.bin"),
        // Tiles (16x16)
        newFile(res_MHZ_tiles_0_bin, "Resource/Stages/MHZ/Data (Tiles)/0.bin"), // Pre-Act
        newFile(res_MHZ_tiles_1_bin, "Resource/Stages/MHZ/Data (Tiles)/1.bin"), // Act 1
        newFile(res_MHZ_tiles_2_bin, "Resource/Stages/MHZ/Data (Tiles)/2.bin"),
        newFile(res_MHZ_tiles_2_bin, "Resource/Stages/MHZ/Data (Tiles)/2.bin"),
        // Rings
        newFile(res_MHZ_rings_2_bin, "Resource/Stages/MHZ/Data (Rings)/1.bin"), // Pre-Act
        newFile(res_MHZ_rings_1_bin, "Resource/Stages/MHZ/Data (Rings)/1.bin"), // Act 1
        newFile(res_MHZ_rings_2_bin, "Resource/Stages/MHZ/Data (Rings)/2.bin"),
        newFile(res_MHZ_rings_2_bin, "Resource/Stages/MHZ/Data (Rings)/2.bin"),
        // Objects
        newFile(res_MHZ_objects_1_bin, "Resource/Stages/MHZ/Data (Objects)/1.bin"), // Pre-Act
        newFile(res_MHZ_objects_1_bin, "Resource/Stages/MHZ/Data (Objects)/1.bin"), // Act 1
        newFile(res_MHZ_objects_2_bin, "Resource/Stages/MHZ/Data (Objects)/2.bin"),
        newFile(res_MHZ_objects_2_bin, "Resource/Stages/MHZ/Data (Objects)/2.bin"),
        // Palettes (Dry)
        newFile(res_MHZ_PAL1_DRY_png, "Resource/Stages/MHZ/Palettes/PAL1_DRY.png"), // Pre-Act
        newFile(res_MHZ_PAL1_DRY_png, "Resource/Stages/MHZ/Palettes/PAL1_DRY.png"), // Act 1
        newFile(res_MHZ_PAL2_DRY_png, "Resource/Stages/MHZ/Palettes/PAL2_DRY.png"),
        newFile(res_MHZ_PAL2_DRY_png, "Resource/Stages/MHZ/Palettes/PAL2_DRY.png"),
        // Palettes (Wet)
        newFile(res_MHZ_PAL1_WET_png, "Resource/Stages/MHZ/Palettes/PAL1_WET.png"), // Pre-Act
        newFile(res_MHZ_PAL1_WET_png, "Resource/Stages/MHZ/Palettes/PAL1_WET.png"), // Act 1
        newFile(res_MHZ_PAL2_WET_png, "Resource/Stages/MHZ/Palettes/PAL2_WET.png"),
        newFile(res_MHZ_PAL2_WET_png, "Resource/Stages/MHZ/Palettes/PAL2_WET.png"),
        // Palettes (Other)
        newFile(res_MHZ_PALm_DRY_png, "Resource/Stages/MHZ/Palettes/PAL1_DRY.png"), // Mini-boss
        newFile(res_MHZ_PALm_DRY_png, "Resource/Stages/MHZ/Palettes/PAL1_DRY.png"), // Boss
        newFile(res_MHZ_PALm_WET_png, "Resource/Stages/MHZ/Palettes/PAL1_DRY.png"), // Extra 1 (Mini-boss)
        newFile(res_MHZ_PALm_WET_png, "Resource/Stages/MHZ/Palettes/PAL1_DRY.png"), // Extra 2
    };

    App = app;

    memcpy(RWList, _RWList, sizeof(_RWList));
    Init(app, LevelID + act, checkpoint);
}
void Scene_MHZ::DoSwitchActZone() {
	if (!doSwitch) return;
	doSwitch = false;

    if (ActualAct == 2) {
        Scene_FBZ* nextLevel = new Scene_FBZ(App, 1, -1);
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
void Scene_MHZ::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {
    int a = act & 0xF;

    if (a == 1)
        Tileset = new ITexture(IResources::Load("Stages/MHZ/Sprites/TILES1.png"), false);
    else if (a == 2)
        Tileset = new ITexture(IResources::Load("Stages/MHZ/Sprites/TILES2.png"), false);

    // Parallax Sizes and Auto Scrolls
    int PS[] =    { 0x7FFF };
    float PAS[] = {      0 };
    float PM[] =  { 1.000f };

    memcpy(ParallaxAutoScroll, PAS, sizeof(ParallaxAutoScroll));
    memcpy(ParallaxSizes, PS, sizeof(ParallaxSizes));
    memcpy(ParallaxMult, PM, sizeof(PM));
    ParallaxCount = sizeof(PS) / sizeof(*PS);

    // Animated Tiles
    AnimTilesCount = 0;

    LevelScene::InitZone(resetTextures, check, specialRing, actTransition);
}
void Scene_MHZ::CreateUniqueTextures(Application* app, int act) {
	unsigned int nextTick = SDL_GetTicks();

    T("MHZ") = new ITexture(IResources::Load("Stages/MHZ/Sprites/Texture.png"), false);



	App->print(0, "UNIQUE Texture load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void Scene_MHZ::UnloadUniqueTextures(Application* app, int act) {

}

void Scene_MHZ::Update() {
    BGx = 1.f / 4.f;
    BGy = 1.f / 8.f;
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

int Scene_MHZ::CustomAnimTile(int ind, int i, int parRow) {
    if (ind >= 37 && ind < 37 + 4) {
        int frm = frameAnim % 93;
        if (frm < 25)
            return 2048 + 0 + ind - 37 + 0;
        else if (frm < 26)
            return 2048 + 0 + ind - 37 + 4;
        else if (frm < 27)
            return 2048 + 0 + ind - 37 + 0;
        else if (frm < 28)
            return 2048 + 0 + ind - 37 + 4;
        else if (frm < 29)
            return 2048 + 0 + ind - 37 + 8;
        else if (frm < 30)
            return 2048 + 0 + ind - 37 + 4;
        else if (frm < 31)
            return 2048 + 0 + ind - 37 + 8;
        else if (frm < 32)
            return 2048 + 0 + ind - 37 + 12;
        else if (frm < 33)
            return 2048 + 0 + ind - 37 + 8;
        else if (frm < 34)
            return 2048 + 0 + ind - 37 + 12;
        else if (frm < 35)
            return 2048 + 0 + ind - 37 + 8;
        else if (frm < 36)
            return 2048 + 0 + ind - 37 + 12;
        else if (frm < 37)
            return 2048 + 0 + ind - 37 + 16;
        else if (frm < 38)
            return 2048 + 0 + ind - 37 + 12;
        else if (frm < 39)
            return 2048 + 0 + ind - 37 + 16;
        else if (frm < 40)
            return 2048 + 0 + ind - 37 + 20;
        else if (frm < 41)
            return 2048 + 0 + ind - 37 + 16;
        else if (frm < 91)
            return 2048 + 0 + ind - 37 + 20;
        else if (frm < 92)
            return 2048 + 0 + ind - 37 + 0;
        else if (frm < 93)
            return 2048 + 0 + ind - 37 + 20;
    }
    if (ind >= 25 && ind < 25 + 4) {
        int frm = frameAnim % 83;
        if (frm < 30)
            return 2048 + 24 + ind - 25 + 0;
        else if (frm < 31)
            return 2048 + 24 + ind - 25 + 4;
        else if (frm < 32)
            return 2048 + 24 + ind - 25 + 0;
        else if (frm < 33)
            return 2048 + 24 + ind - 25 + 4;
        else if (frm < 34)
            return 2048 + 24 + ind - 25 + 0;
        else if (frm < 35)
            return 2048 + 24 + ind - 25 + 4;
        else if (frm < 36)
            return 2048 + 24 + ind - 25 + 0;
        else if (frm < 38)
            return 2048 + 24 + ind - 25 + 4;
        else if (frm < 68)
            return 2048 + 24 + ind - 25 + 8;
        else if (frm < 73)
            return 2048 + 24 + ind - 25 + 12;
        else if (frm < 78)
            return 2048 + 24 + ind - 25 + 16;
        else if (frm < 83)
            return 2048 + 24 + ind - 25 + 20;
    }
    if (ind >= 93 && ind < 93 + 8) {
        int frm = frameAnim % 44;
        if (frm < 5)
            return 2048 + 48 + ind - 93 + 0;
        else if (frm < 10)
            return 2048 + 48 + ind - 93 + 8;
        else if (frm < 17)
            return 2048 + 48 + ind - 93 + 16;
        else if (frm < 22)
            return 2048 + 48 + ind - 93 + 8;
        else if (frm < 27)
            return 2048 + 48 + ind - 93 + 0;
        else if (frm < 32)
            return 2048 + 48 + ind - 93 + 24;
        else if (frm < 39)
            return 2048 + 48 + ind - 93 + 32;
        else if (frm < 44)
            return 2048 + 48 + ind - 93 + 24;
    }
    if (ind >= 29 && ind < 29 + 8) {
        int frm = frameAnim % 44;
        if (frm < 7)
            return 2048 + 88 + ind - 29 + 16;
        else if (frm < 12)
            return 2048 + 88 + ind - 29 + 8;
        else if (frm < 17)
            return 2048 + 88 + ind - 29 + 0;
        else if (frm < 22)
            return 2048 + 88 + ind - 29 + 24;
        else if (frm < 29)
            return 2048 + 88 + ind - 29 + 32;
        else if (frm < 34)
            return 2048 + 88 + ind - 29 + 24;
        else if (frm < 39)
            return 2048 + 88 + ind - 29 + 0;
        else if (frm < 44)
            return 2048 + 88 + ind - 29 + 8;
    }
	return ind;
}

// OVERRIDES
void Scene_MHZ::HandleCamera() {
	LevelScene::HandleCamera();

	if (MyPlayer->Action == ActionType::Dead)
		return;
}
void Scene_MHZ::DrawAboveEverythingNonHUD() {
	LevelScene::DrawAboveEverythingNonHUD();
    return;
}
void Scene_MHZ::Render() {
	LevelScene::Render();
}
void Scene_MHZ::Free() {
	LevelScene::Free();
    UnloadUniqueTextures(app, 1);
    UnloadUniqueTextures(app, 2);
    unloadAnimatedChunks(app);
}
