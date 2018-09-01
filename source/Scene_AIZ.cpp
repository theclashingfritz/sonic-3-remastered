#include "Standard.h"
#include "Scene_MainMenu.h"
#include "Application.h"
#include "Resources.h"
#include "Scene_AIZ.h"
#include "S3Object.h"
#include "LevelScene.h"

#include "Scene_HCZ.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) App->Textures[name]
#define S(name) App->Sprites[name]

ITexture*  Tileset1 = NULL;
ITexture*  Tileset2 = NULL;

ISprite*   _SpriteTransition = NULL;
int        ATimerForFire = 0x180;

unsigned char shipTileBuffer[0x30000];
bool          shipLoaded = false;

float ShipX = 0;
float ShipY = 0;

/*/ Routine List

0:  Normal
4:  Going through final tube
5:  Ship area entered
6:  Ship exited, moving to woods
7:  In woods loop
8:  In boss area, waiting on boss to change routine

//*/

// Unique
const char LevelID = 0x10;

// Modularized
Scene_AIZ::Scene_AIZ(Application* app, int act, int checkpoint) {
    App = app;
    RefreshRW();
    Init(app, LevelID + act, checkpoint);
}

void Scene_AIZ::RefreshRW() {
    SDL_RWops *_RWList[35] = {
        newFile(Resource_Stages_AIZ_level_lvl, "Resource/Stages/AIZ/level.lvl"),
        // Musics
        newFile(Resource_Stages_AIZ_Music_Act_1_ogg, "Resource/Stages/AIZ/Music/Act_1.ogg"),
        newFile(Resource_Stages_AIZ_Music_Act_2_ogg, "Resource/Stages/AIZ/Music/Act_2.ogg"),
        // Layouts
        newFile(Resource_Stages_AIZ_Data_Layout_1_bin, "Resource/Stages/AIZ/Data (Layout)/1.bin"), // Pre-Act
        newFile(Resource_Stages_AIZ_Data_Layout_1_bin, "Resource/Stages/AIZ/Data (Layout)/1.bin"), // Act 1
        newFile(Resource_Stages_AIZ_Data_Layout_2_bin, "Resource/Stages/AIZ/Data (Layout)/2.bin"),
        newFile(Resource_Stages_AIZ_Data_Layout_2_bin, "Resource/Stages/AIZ/Data (Layout)/2.bin"),
        // Chunks (128x128)
        newFile(Resource_Stages_AIZ_Data_Chunks_2_bin, "Resource/Stages/AIZ/Data (Chunks)/1.bin"), // Pre-Act
        newFile(Resource_Stages_AIZ_Data_Chunks_1_bin, "Resource/Stages/AIZ/Data (Chunks)/1.bin"), // Act 1
        newFile(Resource_Stages_AIZ_Data_Chunks_2_bin, "Resource/Stages/AIZ/Data (Chunks)/2.bin"),
        newFile(Resource_Stages_AIZ_Data_Chunks_2_bin, "Resource/Stages/AIZ/Data (Chunks)/2.bin"),
        // Tiles (16x16)
        newFile(Resource_Stages_AIZ_Data_Tiles_0_bin, "Resource/Stages/AIZ/Data (Tiles)/0.bin"), // Pre-Act
        newFile(Resource_Stages_AIZ_Data_Tiles_1_bin, "Resource/Stages/AIZ/Data (Tiles)/1.bin"), // Act 1
        newFile(Resource_Stages_AIZ_Data_Tiles_2_bin, "Resource/Stages/AIZ/Data (Tiles)/2.bin"),
        newFile(Resource_Stages_AIZ_Data_Tiles_2_bin, "Resource/Stages/AIZ/Data (Tiles)/2.bin"),
        // Rings
        newFile(Resource_Stages_AIZ_Data_Rings_2_bin, "Resource/Stages/AIZ/Data (Rings)/1.bin"), // Pre-Act
        newFile(Resource_Stages_AIZ_Data_Rings_1_bin, "Resource/Stages/AIZ/Data (Rings)/1.bin"), // Act 1
        newFile(Resource_Stages_AIZ_Data_Rings_2_bin, "Resource/Stages/AIZ/Data (Rings)/2.bin"),
        newFile(Resource_Stages_AIZ_Data_Rings_2_bin, "Resource/Stages/AIZ/Data (Rings)/2.bin"),
        // Objects
        newFile(Resource_Stages_AIZ_Data_Objects_1_bin, "Resource/Stages/AIZ/Data (Objects)/1.bin"), // Pre-Act
        newFile(Resource_Stages_AIZ_Data_Objects_1_bin, "Resource/Stages/AIZ/Data (Objects)/1.bin"), // Act 1
        newFile(Resource_Stages_AIZ_Data_Objects_2_bin, "Resource/Stages/AIZ/Data (Objects)/2.bin"),
        newFile(Resource_Stages_AIZ_Data_Objects_2_bin, "Resource/Stages/AIZ/Data (Objects)/2.bin"),
        // Palettes (Dry)
        newFile(Resource_Stages_AIZ_PAL0_DRY_png, "Resource/Stages/AIZ/Palettes/PAL0_DRY.png"), // Pre-Act
        newFile(Resource_Stages_AIZ_PAL1_DRY_png, "Resource/Stages/AIZ/Palettes/PAL1_DRY.png"), // Act 1
        newFile(Resource_Stages_AIZ_PAL2_DRY_png, "Resource/Stages/AIZ/Palettes/PAL2_DRY.png"),
        newFile(Resource_Stages_AIZ_PAL2_DRY_png, "Resource/Stages/AIZ/Palettes/PAL2_DRY.png"),
        // Palettes (Wet)
        newFile(Resource_Stages_AIZ_PAL1_WET_png, "Resource/Stages/AIZ/Palettes/PAL1_WET.png"), // Pre-Act
        newFile(Resource_Stages_AIZ_PAL1_WET_png, "Resource/Stages/AIZ/Palettes/PAL1_WET.png"), // Act 1
        newFile(Resource_Stages_AIZ_PAL2_WET_png, "Resource/Stages/AIZ/Palettes/PAL2_WET.png"),
        newFile(Resource_Stages_AIZ_PAL2_WET_png, "Resource/Stages/AIZ/Palettes/PAL2_WET.png"),
        // Palettes (Other)
        newFile(res_HCZ_PALm_DRY_png, "Resource/Stages/AIZ/Palettes/PAL_MB.png"), // Mini-boss
        newFile(res_HCZ_PALm_DRY_png, "Resource/Stages/AIZ/Palettes/PAL_MB.png"), // Boss
        newFile(res_HCZ_PALm_WET_png, "Resource/Stages/AIZ/Palettes/PAL_MB.png"), // Extra 1 (Mini-boss)
        NULL, // Extra 2
    };

    memcpy(RWList, _RWList, sizeof(_RWList));
}
void Scene_AIZ::DoSwitchActZone() {
	if (!doSwitch) return;
	doSwitch = false;

    int a = act & 0xF;

    if (ApparentAct == 2) {
        Scene_HCZ* nextLevel = new Scene_HCZ(App, 1, -1);
        nextLevel->score = score;
        nextLevel->lives = lives;
        App->nextScene = nextLevel;
        return;
    }

    if (ApparentAct == 1) {
        int tempFrameTimer = frameTimer;
        int tempRings = rings;
        int tempRingLifeRank = ringLifeRank;
        int tempCheckpoint = checkpoint;

    	MyPlayer->X -= 0x2F00;
    	MyPlayer->Y -= 0x0080;

        float tempCameraX = cameraX[App->CurrentViewport] - 0x2F00;
        float tempCameraY = cameraY[App->CurrentViewport] - 0x0080;

    	act = LevelID + a + 1;

    	InitZone(true, 0, -1, true);
        ApparentAct = 1;
        sceneIn = -2;
        MyPlayer->Layer = 0;

    	cameraX[App->CurrentViewport] = tempCameraX;
    	cameraY[App->CurrentViewport] = tempCameraY;
    	cameraMinX = 0;
    	cameraMinY = 0;

        rings = tempRings;
        checkpoint = tempCheckpoint;
        frameTimer = tempFrameTimer;
        ringLifeRank = tempRingLifeRank;
    }
}
// Mandatory
int  Scene_AIZ::CustomAnimTile(int ind, int i, int parRow) {
    int aniID = AnimTiles[i * 5 + 2];

    if ((act & 0xF) == 1) {
        if (ind >= 742 && ind < 742 + 12) {
            aniID = AnimTiles[0 * 5 + 2];
            int frm = frameAnim % 190;
            if (frm < 80)
                return 2048 + aniID + ind - 742 + 60;
            else if (frm < 86)
                return 2048 + aniID + ind - 742 + 48;
            else if (frm < 92)
                return 2048 + aniID + ind - 742 + 24;
            else if (frm < 98)
                return 2048 + aniID + ind - 742 + 12;
            else if (frm < 178)
                return 2048 + aniID + ind - 742 + 0;
            else if (frm < 182)
                return 2048 + aniID + ind - 742 + 12;
            else if (frm < 186)
                return 2048 + aniID + ind - 742 + 24;
            else if (frm < 188)
                return 2048 + aniID + ind - 742 + 36;
            else if (frm < 190)
                return 2048 + aniID + ind - 742 + 48;
        }
        else if (ind >= 754 && ind < 754 + 12) {
            aniID = AnimTiles[1 * 5 + 2];
            int frm = frameAnim % 82;
            if (frm < 6)
                return 2048 + aniID + ind - 754 + 24;
            else if (frm < 12)
                return 2048 + aniID + ind - 754 + 36;
            else if (frm < 18)
                return 2048 + aniID + ind - 754 + 48;
            else if (frm < 58)
                return 2048 + aniID + ind - 754 + 60;
            else if (frm < 64)
                return 2048 + aniID + ind - 754 + 0;
            else if (frm < 70)
                return 2048 + aniID + ind - 754 + 12;
            else if (frm < 76)
                return 2048 + aniID + ind - 754 + 24;
            else if (frm < 82)
                return 2048 + aniID + ind - 754 + 36;
        }
        else if (ind >= 766 && ind < 766 + 6) {
            aniID = AnimTiles[2 * 5 + 2];
            int frm = frameAnim % 40;
            if (frm < 8)
                return 2048 + aniID + ind - 766 + 0;
            else if (frm < 12)
                return 2048 + aniID + ind - 766 + 6;
            else if (frm < 16)
                return 2048 + aniID + ind - 766 + 12;
            else if (frm < 20)
                return 2048 + aniID + ind - 766 + 18;
            else if (frm < 28)
                return 2048 + aniID + ind - 766 + 24;
            else if (frm < 32)
                return 2048 + aniID + ind - 766 + 18;
            else if (frm < 36)
                return 2048 + aniID + ind - 766 + 12;
            else if (frm < 40)
                return 2048 + aniID + ind - 766 + 6;
        }
    }
    else {
        if (Tileset != Tileset2) {
            if (ind >= 179 && ind < 179 + 23) {
                aniID = AnimTiles[0 * 5 + 2];
                int frm = frameAnim % 16;
                if (frm < 4)
                    return 2048 + aniID + ind - 179 + 0;
                else if (frm < 8)
                    return 2048 + aniID + ind - 179 + 23;
                else if (frm < 12)
                    return 2048 + aniID + ind - 179 + 46;
                else if (frm < 16)
                    return 2048 + aniID + ind - 179 + 69;
            }
            else if (ind >= 202 && ind < 202 + 35 && cameraX[App->CurrentViewport] < 0x100) {
                aniID = 276;
                return 2048 + aniID + ind - 202;
            }
            else if (ind >= 202 && ind < 202 + 12 && cameraX[App->CurrentViewport] >= 0x100) {
                aniID = AnimTiles[1 * 5 + 2];
                int frm = frameAnim % 190;
                if (frm < 80)
                    return 2048 + aniID + ind - 202 + 60;
                else if (frm < 86)
                    return 2048 + aniID + ind - 202 + 48;
                else if (frm < 92)
                    return 2048 + aniID + ind - 202 + 24;
                else if (frm < 98)
                    return 2048 + aniID + ind - 202 + 12;
                else if (frm < 178)
                    return 2048 + aniID + ind - 202 + 0;
                else if (frm < 182)
                    return 2048 + aniID + ind - 202 + 12;
                else if (frm < 186)
                    return 2048 + aniID + ind - 202 + 24;
                else if (frm < 188)
                    return 2048 + aniID + ind - 202 + 36;
                else if (frm < 190)
                    return 2048 + aniID + ind - 202 + 48;
            }
            else if (ind >= 214 && ind < 214 + 12) {
                aniID = AnimTiles[2 * 5 + 2];
                int frm = frameAnim % 82;
                if (frm < 6)
                    return 2048 + aniID + ind - 214 + 24;
                else if (frm < 12)
                    return 2048 + aniID + ind - 214 + 36;
                else if (frm < 18)
                    return 2048 + aniID + ind - 214 + 48;
                else if (frm < 58)
                    return 2048 + aniID + ind - 214 + 60;
                else if (frm < 64)
                    return 2048 + aniID + ind - 214 + 0;
                else if (frm < 70)
                    return 2048 + aniID + ind - 214 + 12;
                else if (frm < 76)
                    return 2048 + aniID + ind - 214 + 24;
                else if (frm < 82)
                    return 2048 + aniID + ind - 214 + 36;
            }
            else if (ind >= 226 && ind < 226 + 4) {
                aniID = AnimTiles[3 * 5 + 2];
                int frm = frameAnim % 16;
                if (frm < 4)
                    return 2048 + aniID + ind - 226 + 0;
                else if (frm < 8)
                    return 2048 + aniID + ind - 226 + 4;
                else if (frm < 12)
                    return 2048 + aniID + ind - 226 + 8;
                else if (frm < 16)
                    return 2048 + aniID + ind - 226 + 12;
            }
            else if (ind >= 230 && ind < 230 + 24) {
                aniID = AnimTiles[4 * 5 + 2];
                int frm = frameAnim % 16;
                if (frm < 4)
                    return 2048 + aniID + ind - 230 + 0;
                else if (frm < 8)
                    return 2048 + aniID + ind - 230 + 24;
                else if (frm < 12)
                    return 2048 + aniID + ind - 230 + 48;
                else if (frm < 16)
                    return 2048 + aniID + ind - 230 + 72;
            }
        }
        else {
            if (ind >= 179 && ind < 179 + 23) {
                aniID = 102;
                int frm = frameAnim % 16;
                if (frm < 4)
                    return 2048 + aniID + ind - 179 + 0;
                else if (frm < 8)
                    return 2048 + aniID + ind - 179 + 23;
                else if (frm < 12)
                    return 2048 + aniID + ind - 179 + 46;
                else if (frm < 16)
                    return 2048 + aniID + ind - 179 + 69;
            }
            else if (ind >= 202 && ind < 202 + 12) {
                aniID = 194;
                int frm = frameAnim % 190;
                if (frm < 80)
                    return 2048 + aniID + ind - 202 + 60;
                else if (frm < 86)
                    return 2048 + aniID + ind - 202 + 48;
                else if (frm < 92)
                    return 2048 + aniID + ind - 202 + 24;
                else if (frm < 98)
                    return 2048 + aniID + ind - 202 + 12;
                else if (frm < 178)
                    return 2048 + aniID + ind - 202 + 0;
                else if (frm < 182)
                    return 2048 + aniID + ind - 202 + 12;
                else if (frm < 186)
                    return 2048 + aniID + ind - 202 + 24;
                else if (frm < 188)
                    return 2048 + aniID + ind - 202 + 36;
                else if (frm < 190)
                    return 2048 + aniID + ind - 202 + 48;
            }
            else if (ind >= 214 && ind < 214 + 12) {
                aniID = 194;
                int frm = frameAnim % 82;
                if (frm < 6)
                    return 2048 + aniID + ind - 214 + 24;
                else if (frm < 12)
                    return 2048 + aniID + ind - 214 + 36;
                else if (frm < 18)
                    return 2048 + aniID + ind - 214 + 48;
                else if (frm < 58)
                    return 2048 + aniID + ind - 214 + 60;
                else if (frm < 64)
                    return 2048 + aniID + ind - 214 + 0;
                else if (frm < 70)
                    return 2048 + aniID + ind - 214 + 12;
                else if (frm < 76)
                    return 2048 + aniID + ind - 214 + 24;
                else if (frm < 82)
                    return 2048 + aniID + ind - 214 + 36;
            }
            else if (ind >= 226 && ind < 226 + 4) {
                aniID = 266;
                int frm = frameAnim % 16;
                if (frm < 4)
                    return 2048 + aniID + ind - 226 + 0;
                else if (frm < 8)
                    return 2048 + aniID + ind - 226 + 4;
                else if (frm < 12)
                    return 2048 + aniID + ind - 226 + 8;
                else if (frm < 16)
                    return 2048 + aniID + ind - 226 + 12;
            }
            else if (ind >= 230 && ind < 230 + 24) {
                aniID = 282;
                int frm = frameAnim % 16;
                if (frm < 4)
                    return 2048 + aniID + ind - 230 + 0;
                else if (frm < 8)
                    return 2048 + aniID + ind - 230 + 24;
                else if (frm < 12)
                    return 2048 + aniID + ind - 230 + 48;
                else if (frm < 16)
                    return 2048 + aniID + ind - 230 + 72;
            }
        }
    }
    return ind;
}
// Unique
void Scene_AIZ::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {
    int a = act & 0xF;

    ShipX = 0;
    ShipY = 0;

    if (a == 1) {
        Tileset = new ITexture(IResources::Load("Stages/AIZ/Sprites/TILES1.png"), false);
    }
    else if (a == 2) {
        Tileset1 = new ITexture(IResources::Load("Stages/AIZ/Sprites/TILES2.png"), false);
        Tileset2 = new ITexture(IResources::Load("Stages/AIZ/Sprites/TILES3.png"), false);
        Tileset = Tileset1;

        if (!shipLoaded) {
            LoadRW(newFile(Resource_Stages_AIZ_Data_Tiles_3_bin, "Resource/Stages/AIZ/Data (Tiles)/3.bin"), shipTileBuffer);
            shipLoaded = true;
        }
    }

    // Parallax Sizes and Auto Scrolls
    if (a <= 1) {
        //                    0        1        2        3        4        5        6        7            8        9        A        B        C        D
        int PS[] =    { 0x00D0, 0x0020, 0x0030, 0x0030, 0x0010, 0x0010, 0x0010,
            // 0x800D, or 13 of them
            0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001,
            0x8001, 0x8001, 0x8001, 0x8001, 0x8001,

            0x000F, 0x0006, 0x000E, 0x0050, 0x0020, 0x7FFF };
        float PAS[] = { 1/2.f,  1/2.f,  1/3.f,  1/3.f,  1/4.f,  1/8.f,  0,

            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

            0,      0,      0,      0,      0,      0      };
        float PM[] =  { 0.50000000f, 0.17187500f, 0.14062500f, 0.10937500f, 0.07812500f, 0.04687500f, 0.01562500f, 0.03515625f, 0.03906250f, 0.04296875f, 0.04687500f, 0.05078125f, 0.05468750f, 0.05859375f, 0.06250000f, 0.06640625f, 0.07031250f, 0.07421875f, 0.07812500f, 0.08203125f, 0.43750000f, 0.50000000f, 0.56250000f, 0.62500000f, 0.56250000f, 0.50000000f };

        memcpy(ParallaxAutoScroll, PAS, sizeof(PAS));
        memcpy(ParallaxSizes, PS, sizeof(PS));
        memcpy(ParallaxMult, PM, sizeof(PM));
        ParallaxCount = sizeof(PS) / sizeof(*PS);
    }
    else {
        int PS[] =    { 0x0010, 0x0020, 0x0038, 0x0058, 0x0028, 0x0040, 0x0038, 0x0018, 0x0018, 0x0090, 0x0048, 0x0010, 0x0018, 0x0020, 0x0038, 0x0058, 0x0028, 0x0040, 0x0038, 0x0018, 0x0018, 0x0090, 0x0048, 0x0010, 0x7FFF, };
        float PAS[] = { 0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      };

        memcpy(ParallaxAutoScroll, PAS, sizeof(ParallaxAutoScroll));
        memcpy(ParallaxSizes, PS, sizeof(ParallaxSizes));
        ParallaxCount = sizeof(PS) / sizeof(*PS);
    }

    // Animated Tiles
    if (a <= 1) {
        int animTilesTmp[3 * 5] = {
        //  Tile ID        # of tiles        AniTile ID        # of frames        # frames to next anim frame (0 for background tile)
            742,           12,               0,                6,                 12,
            754,           12,               0,                6,                 12,
            766,           6,                72,               5,                 4,
        };
        memcpy(AnimTiles, animTilesTmp, sizeof(AnimTiles));
        AnimTilesCount = sizeof(animTilesTmp) / sizeof(*animTilesTmp) / 5;
    }
    else {
        int animTilesTmp[5 * 5] = {
        //  Tile ID        # of tiles        AniTile ID        # of frames        # frames to next anim frame (0 for background tile)
            179,           23,               0,                4,                 1,
            202,           12,               92,               6,                 12,
            214,           12,               92,               6,                 12,
            226,           4,                164,              1,                 1,
            230,           24,               180,              1,                 0,
        };
        memcpy(AnimTiles, animTilesTmp, sizeof(AnimTiles));
        AnimTilesCount = sizeof(animTilesTmp) / sizeof(*animTilesTmp) / 5;
    }

    if (LevelRoutine >= 4) {
        memcpy(tiles, tilesBuffer, 0x30000 * sizeof(char));
        Chunked = false;
        CreateChunkBOs();
        resetTextures = true;
    }
    LevelRoutine = 0;

    LevelScene::InitZone(resetTextures, check, specialRing, actTransition);
}
void Scene_AIZ::CreateUniqueTextures(Application* app, int act) {
    unsigned int nextTick = SDL_GetTicks();

    if (!T("AIZ")) {
        T("AIZ") = new ITexture(IResources::Load("Stages/AIZ/Sprites/Texture.png"), false);

        T("Palette (Miniboss)") = new ITexture(IResources::Load("Stages/AIZ/Palettes/PAL_MB.png"), true);

        S("Miniboss") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Miniboss.spr"), T("AIZ"));
        S("Miniboss Flame") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Miniboss Flame.spr"), T("AIZ"));
        S("Miniboss Small") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Miniboss Small.spr"), T("AIZ"));

        S("Transition Fire") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Transition Fire.spr"), T("AIZ"));
        _SpriteTransition = S("Transition Fire");

        S("End Boss") = new ISprite(IResources::Load("Stages/AIZ/Sprites/End Boss.spr"), T("AIZ"));
        S("Bombership Bomb") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Bombership Bomb.spr"), T("AIZ"));
        S("Bombership Propeller") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Bombership Propeller.spr"), T("AIZ"));
        S("Boss Small") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Boss Small.spr"), T("AIZ"));

        S("Anim") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Anim.spr"), T("AIZ"));
        S("Bloominator") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Bloominator.spr"), T("AIZ"));
        S("Breakable Wall") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Breakable Wall.spr"), T("AIZ"));
        S("Catekiller Jr") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Catekiller Jr.spr"), T("AIZ"));
        S("Collapsing Bridge") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Collapsing Bridge.spr"), T("AIZ"));
        S("Collapsing Platform") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Collapsing Platform.spr"), T("AIZ"));
        S("Cork Floor") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Cork Floor.spr"), T("AIZ"));
        S("Disappearing Platform (Water)") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Disappearing Platform (Water).spr"), T("AIZ"));
        S("Disappearing Platform") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Disappearing Platform.spr"), T("AIZ"));
        S("Drawbridge") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Drawbridge.spr"), T("AIZ"));
        S("Falling Log") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Falling Log.spr"), T("AIZ"));
        S("Flipping Bridge") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Flipping Bridge.spr"), T("AIZ"));
        S("Floating Platform") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Floating Platform.spr"), T("AIZ"));
        S("Foreground Plant") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Foreground Plant.spr"), T("AIZ"));
        S("MonkeyDude") = new ISprite(IResources::Load("Stages/AIZ/Sprites/MonkeyDude.spr"), T("AIZ"));
        S("Non Anim 2") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Non Anim.spr"), T("AIZ"));
        S("RhinoBot") = new ISprite(IResources::Load("Stages/AIZ/Sprites/RhinoBot.spr"), T("AIZ"));
        S("Rock") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Rock.spr"), T("AIZ"));
        S("Spiked Log") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Spiked Log.spr"), T("AIZ"));
        S("Swing Vine") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Swing Vine.spr"), T("AIZ"));
        S("Zipline Peg") = new ISprite(IResources::Load("Stages/AIZ/Sprites/Zipline Peg.spr"), T("AIZ"));
    }

    app->print(0, "UNIQUE Texture load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void Scene_AIZ::UnloadUniqueTextures(Application* app, int act) {
    if (act == 1) {
        if (tex_ChunksFront != NULL)
            tex_ChunksFront->Free();
        tex_ChunksFront = NULL;
    }
    else {
        if (tex_Bubble != NULL)
            tex_Bubble->Free();
        tex_Bubble = NULL;

        if (tex_ChunksFront != NULL)
            tex_ChunksFront->Free();
        tex_ChunksFront = NULL;

        if (tex_Miniboss != NULL)
            tex_Miniboss->Free();
        tex_Miniboss = NULL;

        if (tex_NonAnim_ != NULL)
            tex_NonAnim_->Free();
        tex_NonAnim_ = NULL;

        if (tex_Waves != NULL)
            tex_Waves->Free();
        tex_Waves = NULL;
    }
}

void Scene_AIZ::HandleCamera() {
    if ((act & 0xF) == 1) {
        if (cameraX[App->CurrentViewport] >= 0x1300 && cameraX[App->CurrentViewport] < 0x1628) {
            cameraMinY = 0;
            cameraMaxY = 0x390;
        }
        else if (cameraX[App->CurrentViewport] >= 0x1628 && cameraX[App->CurrentViewport] < 0x1AC8 && cameraY[App->CurrentViewport] <= 0x3C0) {
            cameraMinY = 0;
            cameraMaxY = 0x3B0;
        }
        else if (cameraX[App->CurrentViewport] >= 0x1AC8 && cameraX[App->CurrentViewport] < 0x2100) {
            cameraMinY = 0;
            cameraMaxY = 0x430;
        }
        else if (cameraX[App->CurrentViewport] >= 0x2100 && cameraX[App->CurrentViewport] < 0x2B50) {
            cameraMinY = 0;
            cameraMaxY = 0x4C0;
        }
		else if (cameraX[App->CurrentViewport] >= 0x2B50 && cameraX[App->CurrentViewport] < 0x2CB8) {
			cameraMinY = 0;
            cameraMaxY = 0x3B0;
		}
		else if (cameraX[App->CurrentViewport] >= 0x2CB8 && cameraX[App->CurrentViewport] < 0x2D78) {
            cameraMinY = 0x2D8;
            cameraMaxY = 0x2D8;
		}
		else if (cameraX[App->CurrentViewport] >= 0x2D78) {
			cameraMinX = std::max(cameraMinX, cameraX[App->CurrentViewport]);
			cameraMaxX = 0x2F10;

            cameraMinY = 0x2D8;
            cameraMaxY = 0x2D8;
		}
	}
    else {
        if (cameraX[App->CurrentViewport] < 0x02A0 && cameraY[App->CurrentViewport] <= 0x260) {
			cameraMinX = 0;
			cameraMaxX = 0x2F00;

            cameraMinY = 0;
            cameraMaxY = 0x258;
		}
        else if (cameraX[App->CurrentViewport] > 0x0980 && cameraX[App->CurrentViewport] <= 0xD60) {
			cameraMinY = 0;
            cameraMaxY = 0x590;
		}
        else if (cameraX[App->CurrentViewport] > 0xED0 && cameraX[App->CurrentViewport] <= 0xF90) {
			cameraMinY = 0;
            cameraMaxY = 0x2B8;
		}
        else if (cameraX[App->CurrentViewport] > 0xF90 && cameraX[App->CurrentViewport] <= 0x1100 && ApparentAct == 1) {
            cameraMinX = std::max(cameraMinX, cameraX[App->CurrentViewport]);
			cameraMaxX = 0x10E0 - (App->renderWidth - 320) / 2;

			cameraMinY = 0;
            cameraMaxY = 0x2B8;
		}
        else if (ApparentAct == 1) {
            cameraMaxY = 0x1F58;
        }

        if (MyPlayer->Character != CharacterType::Knuckles) {
            if (cameraX[App->CurrentViewport] > 0x3A00 && LevelRoutine < 4) {
                Tileset  = Tileset2;
                LevelRoutine = 4;

                short BossPalette[0x30] = {
                    0x0000,
                    0x0EEE,
                    0x02AE,
                    0x006E,
                    0x004C,
                    0x00EE,
                    0x0088,
                    0x0224,
                    0x008E,
                    0x0066,
                    0x0042,
                    0x0020,
                    0x0CAA,
                    0x0866,
                    0x0644,
                    0x0044,
                    0x0000,
                    0x028E,
                    0x006A,
                    0x0048,
                    0x0024,
                    0x0002,
                    0x00AA,
                    0x0088,
                    0x0048,
                    0x02AC,
                    0x000A,
                    0x0006,
                    0x0004,
                    0x06EE,
                    0x0A0E,
                    0x000E,
                    0x0000,
                    0x008E,
                    0x00AA,
                    0x000C,
                    0x000A,
                    0x0008,
                    0x0006,
                    0x0004,
                    0x008E,
                    0x006E,
                    0x004E,
                    0x000E,
                    0x0EE2,
                    0x0EC4,
                    0x0EA4,
                    0x0EE8,
                };

                for (int i = 0; i < 0x30; i++) {
                    setPixel(paletteDry->pixels, i / 0xF + 1, i & 0xF, BossPalette[i]);
                }

                memcpy(tiles, shipTileBuffer, 0x30000 * sizeof(char));
                Chunked = false;
                CreateChunkBOs();
            }
            if (cameraX[App->CurrentViewport] >= 0x4200 && LevelRoutine < 5) {
                LevelRoutine = 5;
            }

            if (cameraX[App->CurrentViewport] > 0x1000 && cameraX[App->CurrentViewport] <= 0x1200 && ApparentAct == 2) {
                cameraMinX = 0x10E0 - (App->renderWidth - 320) / 2;
    			cameraMaxX = 0xFFFF;

    			cameraMinY = 0;
                cameraMaxY = 0x2B8;
    		}
            else if (cameraX[App->CurrentViewport] > 0x1200 && cameraX[App->CurrentViewport] <= 0x3EB0 && ApparentAct == 2) {
                cameraMinX = 0x10E0 - (App->renderWidth - 320) / 2;
    			cameraMaxX = 0xFFFF;

    			cameraMinY = 0;
                cameraMaxY = 0xFFFF;
    		}
            else if (cameraX[App->CurrentViewport] >= 0x3EB0 && cameraX[App->CurrentViewport] < 0x4200 && ApparentAct == 2) {
                cameraMinX = 0x0000;
    			cameraMaxX = 0xFFFF;

    			cameraMinY = 0x015A;
                cameraMaxY = cameraY[App->CurrentViewport];
    		}
            else if (cameraX[App->CurrentViewport] >= 0x4200 && cameraX[App->CurrentViewport] < 0x4E00 && ApparentAct == 2 && LevelRoutine < 5) {
                cameraMinX = 0x4200;
    			cameraMaxX = 0xFFFF;

    			cameraMinY = 0x015A;
                cameraMaxY = 0x015A;
    		}
        }
        else {
            if (ApparentAct == 2) {
                cameraMinX = 0x10B0;
    			cameraMaxX = 0xFFFF;

    			cameraMinY = 0;
                cameraMaxY = 0xFFFF;
    		}
        }
    }

    LevelScene::HandleCamera();
}
void Scene_AIZ::DrawAboveEverythingNonHUD() {
    LevelScene::DrawAboveEverythingNonHUD();
}

void Scene_AIZ::Update() {
    // ZONE SPECIFIC
    if ((act & 0xF) == 0) {
        CurrentTileset = 0;
        CurrentTilesetAnimated = false;
        BGyoff = 0;
        BGy = 1.f;
        BGcameraLock = true;
        BGstart = 0;
        BGsize = 16;

        player->ObjectControlled = 7; // non-jumpable
        player->FlagsAllowXMovement = true;
        rings = 2;
        player->SuperForm = true;
        player->SuperFlight = true;
        player->Action = ActionType::Normal;
        player->Speed = 8.125f;
        player->Ground = true;
        //cameraMinY = 0x7 * 128;
        //cameraMaxY = 0x7 * 128;
    }
    else if ((act & 0xF) == 1) {
        CurrentTilesetAnimated = true;
        BGstart = 0;
        BGsize = 4;
        BGx = 7.f / 16.f;
        BGy = 1.f / 2.f;
        if ((act & 0xF) == 1) {
            cameraMinX = 0x26 * 128;
        }
        cameraMinY = 0x0 * 128;
        //cameraMaxY = 0x490 - app->gameHeight;
        BGcameraLock = false;
    }
    else if ((act & 0xF) == 2) {
        CurrentTilesetAnimated = true;
        BGstart = 0;
        BGsize = 4;
        BGx = 7.f / 16.f;
        BGy = 1.f / 2.f;
        BGcameraLock = false;
    }
    if (player->X > 0x29 * 128 && CurrentTileset == 0 && (act & 0xF) == 0) {
        CurrentTileset = 1;
        player->ObjectControlled = 0;
        player->SuperForm = false;
        player->SuperFormAnim = SuperFormAnimType::None;
        player->SuperFormTimer = 4 * 10;
        beginActZoneTransfer(1);
        levelComplete = true;
        levelCompleteTimer = 0;
    }


    LevelScene::Update();

    if (App->realPaused)
        return;

    if (ActualAct == 1) {

    }
    else if (ActualAct == 2) {
        if (ATimerForFire == 0x300) {
            int list[7] = {
                0x8EE,
                0x0AA,
                0x08E,
                0x04E,
                0x02E,
                0x00C,
                0x008,
            };
            for (int i = 0; i < 7; i++) {
                setPixel(paletteDry->pixels, 3, 1 + i, list[i]);
            }
        }
        if (ATimerForFire <= 0x300)
            ATimerForFire += 4;

        if (LevelRoutine != 3 && cameraX[0] < 0x200) {
            int list[6] = {
                0x04E,
                0x06E,
                0x0AE,
                0x0CE,
                0x2EE,
                0xAEE,
            };
            for (int i = 0; i < 6; i++) {
                setPixel(paletteDry->pixels, 3, 1 + i, list[i]);
            }
            LevelRoutine = 3;
        }
        if (LevelRoutine == 5) {
            if (cameraX[0] >= 0x4400) {
                MyPlayer->X -= 0x200;
                MyPlayer->DisplayX -= 0x200;
                cameraX[0] -= 0x200;
                BGxCamOff += 0x200;
            }

            if (frameAnim % 16 == 0)
                aud[0xBD]->Play(1);

            cameraAutoScrollX = 4;
            ShipX += 9 / 17.f              * 8;
            if (ShipX >= 0x300)
                ShipY -= 4 / 15.f              * 8;

            if (ShipY < -0x70) {
                LevelRoutine = 6;
                int off = (level[8] & 0xFF) * 0x100 + (level[9] & 0xFF) - 0x8000;
                level[off + (0x90) + (0x2) * levelW] = 0xEA;
                level[off + (0x90) + (0x3) * levelW] = 0xE9;
                level[off + (0x90) + (0x4) * levelW] = 0xE8;
                ShipX = 0;
            }
        }
        else if (LevelRoutine == 6) {
            if (cameraX[0] >= 0x46CC) {
                MyPlayer->X -= 0x80;
                MyPlayer->DisplayX -= 0x80;
                cameraX[0] -= 0x80;
                BGxCamOff += 0x80;
            }
            cameraAutoScrollX = 4;
            ShipX += 1.f;

            if (ShipX > 0xC0) {
                LevelRoutine = 7;

                if (cameraX[0] >= 0x468C) {
                    MyPlayer->X -= 0x80;
                    MyPlayer->DisplayX -= 0x80;
                    cameraX[0] -= 0x80;
                    BGxCamOff += 0x80;
                }

                int off = (level[8] & 0xFF) * 0x100 + (level[9] & 0xFF) - 0x8000;
                level[off + (0x90) + (0x2) * levelW] = 0xEC;
                level[off + (0x90) + (0x3) * levelW] = 0x00;
                level[off + (0x90) + (0x4) * levelW] = 0x48;
            }
        }
        else if (LevelRoutine == 7) {
            cameraAutoScrollX = 0;
            cameraMinX = 0x460C;
            if (cameraX[0] >= 0x4880) {
                LevelRoutine = 8;
                cameraMinX = 0x4880;
                ShipX = 0;
            }
        }
        else if (LevelRoutine == 8) {
            cameraAutoScrollX = 0;
            cameraMinX = 0x4880;
            cameraMaxX = 0x4880;
        }
        else if (LevelRoutine == 9) {
            cameraAutoScrollX = 0;
            if (cameraMinX < 0x48E0) {
                cameraMinX += 2;
            }
            cameraMaxX = 0x4AE0 - App->renderWidth;
        }
    }

    if ((act & 0xF) > 1) {
        if (ButtonCount == 0) {
            if (cameraX[0] > 0x2868) {
                if (waterLevel < 0x0618) {
                    waterLevel++;
                    visualWaterHeight = waterLevel;
                }
            }
            else {
                if (waterLevel < 0x0528) {
                    waterLevel++;
                    visualWaterHeight = waterLevel;
                }
                if (waterLevel > 0x0528) {
                    waterLevel--;
                    visualWaterHeight = waterLevel;
                }
            }
        }
        else {
            if (waterLevel < 0x0618) {
                waterLevel++;
                visualWaterHeight = waterLevel;
            }

            int left = 0x4E;
            int top  = 0x07;
            for (int i = 0; i < 4; i++)
                level[(level[8] & 0xFF) * 0x100 + (level[9] & 0xFF) - 0x8000 + (left + 0) + (top + i) * levelW] = 0;
        }

        if (cameraX[App->CurrentViewport] >= 0x3EB0) {
            if (cameraY[App->CurrentViewport] > 0x400) {
                BGyoff = -0x198;
            }
            else {
                BGyoff = 0xA8;
            }
        }
    }

    // Palette Editing
    int len = 4;
    int pick;
    if ((act & 0xF) <= 1) {
        int ew[] = { 0xEE8, 0xEE2, 0xEA4, 0xE64 };
        // Waterfalls
        len = 4;
        pick = (frameAnim / 8) % len;
        setPixel(paletteDry->pixels, 2, 0x16 / 2, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 2, 0x16 / 2, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        pick = (frameAnim / 8 + 1) % len;
        setPixel(paletteDry->pixels, 2, 0x18 / 2, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 2, 0x18 / 2, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        pick = (frameAnim / 8 + 2) % len;
        setPixel(paletteDry->pixels, 2, 0x1A / 2, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 2, 0x1A / 2, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        pick = (frameAnim / 8 + 3) % len;
        setPixel(paletteDry->pixels, 2, 0x1C / 2, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 2, 0x1C / 2, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        // BG Water
        if ((act & 0xF) == 1) {
            len = 4;
            pick = (frameAnim / 8) % len;
            setPixel(paletteDry->pixels, 3, 0x16 / 2 + 1, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

            pick = (frameAnim / 8 + 2) % len;
            setPixel(paletteDry->pixels, 3, 0x1A / 2 + 1, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        }

        // set buffer color to flower color
        if (cameraX[0] < 0x2BB0 - app->gameWidth / 2) {
            if (LevelRoutine != 1) {
                setPixel(paletteDry->pixels, 2, 15,
                    0xE, 0x0, 0x2);
                LevelRoutine = 1;
            }
        }
        // set buffer color to giant log
        else if (cameraX[0] < 0x2E50 - app->gameWidth / 2) {
            if (LevelRoutine != 2) {
                setPixel(paletteDry->pixels, 2, 15,
                    0x4, 0x0, 0x0);
                LevelRoutine = 2;
            }
        }
        // set buffer tree BG color to current BG color (row 3, index 11)
        else if (LevelRoutine != 3) {
            setPixelFull(paletteDry->pixels, 2, 15,
                paletteDry->pixels[(11 * 4 + 3) * 4 + 0],
                paletteDry->pixels[(11 * 4 + 3) * 4 + 1],
                paletteDry->pixels[(11 * 4 + 3) * 4 + 2]);

            if (cameraX[App->CurrentViewport] >= 0x2D78) {
                int list[6] = {
                    0x04E,
                    0x06E,
                    0x0AE,
                    0x0CE,
                    0x2EE,
                    0xAEE,
                };
                for (int i = 0; i < 6; i++) {
                    setPixel(paletteDry->pixels, 3, 1 + i, list[i]);
                }
            }

            LevelRoutine = 3;
        }
    }
    else {
        // Find: \$([A-Z][A-Z]+)
        // Repl:  0x$1
        int ew[] = { 0xEE8, 0xEE2, 0xEC4, 0xEA4, 0xEE2, 0xEC4, 0xEA4, 0xEE8, 0xEC4, 0xEA4, 0xEE8, 0xEE2, 0xEA4, 0xEE8, 0xEE2, 0xEC4 };

        // Waterfalls
        len = 4;
        pick = (frameAnim / 8) % len;
        setPixel(paletteDry->pixels, 3, 12, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 3, 12, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        pick = (frameAnim / 8 + 1) % len;
        setPixel(paletteDry->pixels, 3, 13, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 3, 13, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        pick = (frameAnim / 8 + 2) % len;
        setPixel(paletteDry->pixels, 3, 14, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 3, 14, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        pick = (frameAnim / 8 + 3) % len;
        setPixel(paletteDry->pixels, 3, 15, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);
        setPixel(paletteWet->pixels, 3, 15, (ew[pick] & 0xF), (ew[pick] & 0xF0) / 0x10, (ew[pick] & 0xF00) / 0x100);

        // BG flashing
        if (LevelRoutine < 4) {
            pick = (frameAnim / 6) % 8;
            if (pick % 2 == 0 && pick < 5) {
                setPixelFull(paletteDry->pixels, 3, 11,
                    paletteDry->pixels[(11 * 4 + 3) * 4 + 0],
                    0xAA,
                    paletteDry->pixels[(11 * 4 + 3) * 4 + 2]);
            }
            else {
                setPixelFull(paletteDry->pixels, 3, 11,
                    paletteDry->pixels[(11 * 4 + 3) * 4 + 0],
                    0x88,
                    paletteDry->pixels[(11 * 4 + 3) * 4 + 2]);
            }
        }

        if (cameraX[App->CurrentViewport] < 0x1D0) {
            // set tree BG color to current BG color (row 3, index 11)
            setPixelFull(paletteDry->pixels, 2, 14,
                paletteDry->pixels[(11 * 4 + 3) * 4 + 0],
                paletteDry->pixels[(11 * 4 + 3) * 4 + 1],
                paletteDry->pixels[(11 * 4 + 3) * 4 + 2]);
            setPixelFull(paletteWet->pixels, 2, 14,
                paletteWet->pixels[(11 * 4 + 3) * 4 + 0],
                paletteWet->pixels[(11 * 4 + 3) * 4 + 1],
                paletteWet->pixels[(11 * 4 + 3) * 4 + 2]);
        }
        else {
            //set tree BG color back to purple
            setPixelFull(paletteDry->pixels, 2, 14,
                0xEE,
                0x00,
                0xAA);
            setPixelFull(paletteWet->pixels, 2, 14,
                0x88,
                0x22,
                0xEE);
        }
    }

    // Do common palette stuffs (Super Sonic)
    if (player->SuperForm || player->SuperFormAnim == SuperFormAnimType::None) {
        // Super Sonic
        len = 4;
        pick = 3 * ((frameAnim / 16) % len + 6);

        if (player->SuperFormAnim == SuperFormAnimType::Transforming) {
            pick = ((4 * 10 - player->SuperFormTimer) / 4);
            if (pick < 2)
                pick = 2;
            pick = 3 * pick;
        }
        else if (player->SuperFormAnim == SuperFormAnimType::None) {
            pick = 3 * (player->SuperFormTimer / 4);
        }
        else if (player->SuperFormTimer == 0) {
            pick = 0;
        }

        int *PalCycle_ChoseSuper = PalCycle_SuperSonicUnderwaterHCZCNZLBZ;
        if ((act & 0xF0) == 0x10 || (act & 0xF0) == 0x50)
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
void Scene_AIZ::Render() {
    app->drawRectangle(0, 0, app->gameWidth, app->gameHeight, 0, 0, 0, 1);
    LevelScene::Render();

    if (ActualAct == 2 && LevelRoutine == 5) {
        JustInstantDrawAnimTiles = false;
        int sX = (int)cameraX[0] + ShipX;
        int sY = (int)cameraY[0] - 66 + std::sin((frameAnim & 0x7F) * Math_PI / 64) * 2 + ShipY;

        G->PaletteShift(0);
        DrawChunk(0, 0, 0xF0, sX - 0x2C0, sY, 16, 16, -0.4f);
        DrawChunk(0, 0, 0xF1, sX - 0x240, sY, 16, 16, -0.4f);
        DrawChunk(0, 0, 0xF1, sX - 0x1C0, sY, 16, 16, -0.4f);
        DrawChunk(0, 0, 0xF1, sX - 0x140, sY, 16, 16, -0.4f);
        DrawChunk(0, 0, 0xF2, sX - 0x0C0, sY, 16, 16, -0.4f);
        JustInstantDrawAnimTiles = true;
    }

    if (ActualAct == 2 && ATimerForFire < 0x300) {
        G->PaletteShift(3);
        for (int i = 0; i < std::ceil(App->renderWidth / 16.f); i++) {
            int fr = (i + frameAnim / 6) % 4 + 8;
            int hhbrother = _SpriteTransition->Frame[fr / 2].FullH;
            G->DrawSprite(_SpriteTransition, (fr % 2) * 16, 0, 16, hhbrother, i * 16, App->renderHeight - ATimerForFire + 0x180, -2.4f, fr / 2, IE::LeftAlign | IE::TopAlign);

            fr -= 4;
            hhbrother = 128;
            G->DrawSprite(_SpriteTransition, (fr % 2) * 16, 128 - hhbrother, 16, hhbrother, i * 16, App->renderHeight - ATimerForFire + 0x080 + 128 - hhbrother, -2.4f, fr / 2, IE::LeftAlign | IE::TopAlign);
            G->DrawSprite(_SpriteTransition, (fr % 2) * 16, 128 - hhbrother, 16, hhbrother, i * 16, App->renderHeight - ATimerForFire + 0x100 + 128 - hhbrother, -2.4f, fr / 2, IE::LeftAlign | IE::TopAlign);
        }
    }
}
void Scene_AIZ::Free() {
    if (Tileset1 != NULL)
        Tileset1->Free();
    if (Tileset2 != NULL)
        Tileset2->Free();
    Tileset1 = NULL;
    Tileset2 = NULL;

    LevelScene::Free();
    UnloadUniqueTextures(app, 1);
    UnloadUniqueTextures(app, 2);
    unloadAnimatedChunks(app);
}
