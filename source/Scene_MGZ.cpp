#include "Standard.h"
#include "Scene_MainMenu.h"
#include "Application.h"
#include "Resources.h"
#include "Player.h"
#include "Scene_MGZ.h"
#include "S3Object.h"
#include "LevelScene.h"

#include "Scene_CNZ.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) App->Textures[name]
#define S(name) App->Sprites[name]

// Unique
const char LevelID = 0x30;
// Modularized
Scene_MGZ::Scene_MGZ(Application* app, int act, int checkpoint) {
    SDL_RWops *_RWList[35] = {
        newFile(res_MGZ_level_lvl, "Resource/Stages/MGZ/level.lvl"),
        // Musics
        newFile(res_MGZ_act_1_ogg, "Resource/Stages/MGZ/Music/Act 1.ogg"),
        newFile(res_MGZ_act_2_ogg, "Resource/Stages/MGZ/Music/Act 2.ogg"),
        // Layouts
        newFile(res_MGZ_layout_1_bin, "Resource/Stages/MGZ/Data (Layout)/1.bin"), // Pre-Act
        newFile(res_MGZ_layout_1_bin, "Resource/Stages/MGZ/Data (Layout)/1.bin"), // Act 1
        newFile(res_MGZ_layout_2_bin, "Resource/Stages/MGZ/Data (Layout)/2.bin"),
        newFile(res_MGZ_layout_2_bin, "Resource/Stages/MGZ/Data (Layout)/2.bin"),
        // Chunks (128x128)
        newFile(res_MGZ_chunks_1_bin, "Resource/Stages/MGZ/Data (Chunks)/1.bin"), // Pre-Act
        newFile(res_MGZ_chunks_1_bin, "Resource/Stages/MGZ/Data (Chunks)/1.bin"), // Act 1
        newFile(res_MGZ_chunks_2_bin, "Resource/Stages/MGZ/Data (Chunks)/2.bin"),
        newFile(res_MGZ_chunks_2_bin, "Resource/Stages/MGZ/Data (Chunks)/2.bin"),
        // Tiles (16x16)
        newFile(res_MGZ_tiles_0_bin, "Resource/Stages/MGZ/Data (Tiles)/0.bin"), // Pre-Act
        newFile(res_MGZ_tiles_1_bin, "Resource/Stages/MGZ/Data (Tiles)/1.bin"), // Act 1
        newFile(res_MGZ_tiles_2_bin, "Resource/Stages/MGZ/Data (Tiles)/2.bin"),
        newFile(res_MGZ_tiles_2_bin, "Resource/Stages/MGZ/Data (Tiles)/2.bin"),
        // Rings
        newFile(res_MGZ_rings_2_bin, "Resource/Stages/MGZ/Data (Rings)/1.bin"), // Pre-Act
        newFile(res_MGZ_rings_1_bin, "Resource/Stages/MGZ/Data (Rings)/1.bin"), // Act 1
        newFile(res_MGZ_rings_2_bin, "Resource/Stages/MGZ/Data (Rings)/2.bin"),
        newFile(res_MGZ_rings_2_bin, "Resource/Stages/MGZ/Data (Rings)/2.bin"),
        // Objects
        newFile(res_MGZ_objects_1_bin, "Resource/Stages/MGZ/Data (Objects)/1.bin"), // Pre-Act
        newFile(res_MGZ_objects_1_bin, "Resource/Stages/MGZ/Data (Objects)/1.bin"), // Act 1
        newFile(res_MGZ_objects_2_bin, "Resource/Stages/MGZ/Data (Objects)/2.bin"),
        newFile(res_MGZ_objects_2_bin, "Resource/Stages/MGZ/Data (Objects)/2.bin"),
        // Palettes (Dry)
        newFile(res_MGZ_PAL1_DRY_png, "Resource/Stages/MGZ/Palettes/PAL1_DRY.png"), // Pre-Act
        newFile(res_MGZ_PAL1_DRY_png, "Resource/Stages/MGZ/Palettes/PAL1_DRY.png"), // Act 1
        newFile(res_MGZ_PAL2_DRY_png, "Resource/Stages/MGZ/Palettes/PAL2_DRY.png"),
        newFile(res_MGZ_PAL2_DRY_png, "Resource/Stages/MGZ/Palettes/PAL2_DRY.png"),
        // Palettes (Wet)
        newFile(res_MGZ_PAL1_WET_png, "Resource/Stages/MGZ/Palettes/PAL1_DRY.png"), // Pre-Act
        newFile(res_MGZ_PAL1_WET_png, "Resource/Stages/MGZ/Palettes/PAL1_DRY.png"), // Act 1
        newFile(res_MGZ_PAL2_WET_png, "Resource/Stages/MGZ/Palettes/PAL2_DRY.png"),
        newFile(res_MGZ_PAL2_WET_png, "Resource/Stages/MGZ/Palettes/PAL2_DRY.png"),
        // Palettes (Other)
        newFile(res_MGZ_PALm_DRY_png, "Resource/Stages/MGZ/Palettes/PAL_MB_DRY.png"), // Mini-boss
        newFile(res_MGZ_PALm_DRY_png, "Resource/Stages/MGZ/Palettes/PAL_MB_DRY.png"), // Boss
        newFile(res_MGZ_PALm_WET_png, "Resource/Stages/MGZ/Palettes/PAL_MB_WET.png"), // Extra 1 (Mini-boss)
        newFile(res_MGZ_PALm_WET_png, "Resource/Stages/MGZ/Palettes/PAL_MB_WET.png"), // Extra 2
    };

    App = app;

    memcpy(RWList, _RWList, sizeof(_RWList));
    Init(app, LevelID + act, checkpoint);
}
void Scene_MGZ::DoSwitchActZone() {
	if (!doSwitch) return;
	doSwitch = false;

    if (ActualAct == 2) {
        Scene_CNZ* nextLevel = new Scene_CNZ(App, 1, -1);
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
void Scene_MGZ::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {
    int a = act & 0xF;

    if (a == 1)
        Tileset = new ITexture(IResources::Load("Stages/MGZ/Sprites/TILES1.png"), false);
    else if (a == 2)
        Tileset = new ITexture(IResources::Load("Stages/MGZ/Sprites/TILES2.png"), false);

    // Parallax Sizes and Auto Scrolls
    int PS[] =    {  0x00000010, 0x00000004, 0x00000004, 0x00000008, 0x00000008, 0x00000008, 0x0000000D, 0x00000013, 0x00000008, 0x00000008, 0x00000008, 0x00000008, 0x00000018, 0x00007FFF };
    float PAS[] = {  0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
    float PM[] =  {  0.0546875f, 0.0703125f, 0.0859375f, 0.1171875f, 0.1015625f, 0.1250000f, 0.1406250f, 0.1562500f, 0.1718750f, 0.1875000f, 0.2031250f, 0.2187500f, 0.2343750f, 0.2500000f };

    memcpy(ParallaxAutoScroll, PAS, sizeof(ParallaxAutoScroll));
    memcpy(ParallaxSizes, PS, sizeof(ParallaxSizes));
    memcpy(ParallaxMult, PM, sizeof(PM));
    ParallaxCount = sizeof(PS) / sizeof(*PS);

    // Animated Tiles
    AnimTilesCount = 0;

    LevelScene::InitZone(resetTextures, check, specialRing, actTransition);
}
void Scene_MGZ::CreateUniqueTextures(Application* app, int act) {
	unsigned int nextTick = SDL_GetTicks();

    T("MGZ") = new ITexture(IResources::Load("Stages/MGZ/Sprites/Texture.png"), false);

    S("Non Anim 1") = NULL;
    S("Non Anim 2") = NULL;

    S("Breakable Wall") = new ISprite(IResources::Load("Stages/MGZ/Sprites/Breakable Wall.spr"), T("MGZ"));
    S("Collapsing Platform") = new ISprite(IResources::Load("Stages/MGZ/Sprites/Collapsing Platform.spr"), T("MGZ"));
    S("Dash Trigger") = new ISprite(IResources::Load("Stages/MGZ/Sprites/Dash Trigger.spr"), T("MGZ"));
    S("Floating Platform") = new ISprite(IResources::Load("Stages/MGZ/Sprites/Floating Platform.spr"), T("MGZ"));
    S("Non Anim") = new ISprite(IResources::Load("Stages/MGZ/Sprites/Non Anim.spr"), T("MGZ"));
    S("Smashing Pillar") = new ISprite(IResources::Load("Stages/MGZ/Sprites/Smashing Pillar.spr"), T("MGZ"));
    S("Spinning Platform") = new ISprite(IResources::Load("Stages/MGZ/Sprites/Spinning Platform.spr"), T("MGZ"));
    S("Trigger Platform") = new ISprite(IResources::Load("Stages/MGZ/Sprites/Trigger Platform.spr"), T("MGZ"));


	App->print(0, "UNIQUE Texture load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void Scene_MGZ::UnloadUniqueTextures(Application* app, int act) {

}

void Scene_MGZ::Update() {
    BGx = 1.f;
    BGy = 0;
    BGstart = 0;
    BGsize = 4;

    loop = true;

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

int Scene_MGZ::CustomAnimTile(int ind, int i, int parRow) {
    if (ind >= 546 && ind < 546 + 48) {
        int frm = frameAnim % 60;
        if (frm < 10)
            return 2048 + 0 + ind - 546 + 0;
        else if (frm < 20)
            return 2048 + 0 + ind - 546 + 48;
        else if (frm < 30)
            return 2048 + 0 + ind - 546 + 96;
        else if (frm < 40)
            return 2048 + 0 + ind - 546 + 144;
        else if (frm < 50)
            return 2048 + 0 + ind - 546 + 192;
        else if (frm < 60)
            return 2048 + 0 + ind - 546 + 240;
    }
    if (ind >= 594 && ind < 594 + 1) {
        int frm = frameAnim % 46;
        if (frm < 8)
            return 2048 + 288 + ind - 594 + 0;
        else if (frm < 23)
            return 2048 + 288 + ind - 594 + 1;
        else if (frm < 31)
            return 2048 + 288 + ind - 594 + 2;
        else if (frm < 46)
            return 2048 + 288 + ind - 594 + 1;
    }
	return ind;
}

// OVERRIDES
void Scene_MGZ::HandleCamera() {
	LevelScene::HandleCamera();

	if (MyPlayer->Action == ActionType::Dead)
		return;
}
void Scene_MGZ::DrawAboveEverythingNonHUD() {
	LevelScene::DrawAboveEverythingNonHUD();
    return;
}
void Scene_MGZ::Render() {
	LevelScene::Render();
}
void Scene_MGZ::Free() {
	LevelScene::Free();
    UnloadUniqueTextures(app, 1);
    UnloadUniqueTextures(app, 2);
    unloadAnimatedChunks(app);
}
