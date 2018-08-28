#include "Standard.h"
#include "Application.h"
#include "TextureAudio.h"
#include "Resources.h"
#include "Scene_MainMenu.h"
#include "LevelScene.h"

#define Z_MID 0
#define Z_FRONT -1
#define Z_BACK 1
#define T(name) app->Textures[name]
#define S(name) app->Sprites[name]

#define OBJECT_MACRO(name) name* obj = new name(); \
    obj->X = X; \
    obj->Y = Y; \
    obj->G = G; \
    obj->ID = ID; \
    obj->App = App; \
    obj->Scene = this; \
    obj->FlipX = (FLIPX == 1); \
    obj->FlipY = (FLIPY == 1); \
    obj->SubType = objs[i + 5]; \
    Objects[ObjectCount++] = obj;
	
#define OBJ_MACRO OBJECT_MACRO

/*
#define OBJ_MACRO(name) name* obj = new name(objs[i + 5]); \
    obj->x = X; \
    obj->y = Y; \
    obj->data1 = objs[i + 4]; \
    obj->data2 = objs[i + 5]; \
    obj->flipX = (FLIPX == 1); \
    obj->flipY = (FLIPY == 1); \
    obj->prior = (PRIORITY == 1); \
    obj->sc = this; \
    obj->app = app; \
    objects[objects_count++] = obj;
*/

#define NEW_OBJECT_MACRO(name) name* obj = new name(); \
    obj->X = X; \
    obj->Y = Y; \
    obj->G = G; \
    obj->App = App; \
    obj->Scene = this; \
    Objects[ObjectCount++] = obj;

Particle::Particle(Texture* tex, float x, float y, int max_life, int life, int slow, bool Active, float vel_x, float vel_y, int maxFrame, bool underwater) {
    this->tex = tex;
    this->x = x;
    this->y = y;
    this->max_life = max_life;
    this->life = life;
    this->slow = slow;
    this->Active = Active;
    this->vel_x = vel_x;
    this->vel_y = vel_y;
    this->maxFrame = maxFrame;
    this->underwater = underwater;
}
Particle::Particle(Texture* tex, float x, float y, int max_life, int life, int slow, bool Active) {
    this->tex = tex;
    this->x = x;
    this->y = y;
    this->max_life = max_life;
    this->life = life;
    this->slow = slow;
    this->Active = Active;
    this->vel_x = 0;
    this->vel_y = 0;
    this->maxFrame = 0;
    this->underwater = false;
}
void Particle::Render(Application* app, float camX, float camY) {
    int ind = max_life - life;
    if (ind >= 0)// && ind < particles[p]->max_life)
        app->drawSprite(tex, (int)x - camX, (int)y - camY, 0, std::min(ind / slow, maxFrame == 0 ? tex->frames : maxFrame), 1, 1);
}

ISprite* S_HUD = NULL;
ISprite* S_Font = NULL;
ISprite* S_FontLives = NULL;
ISprite* S_IconHyperRing = NULL;
ISprite* S_PlayerHeads = NULL;
ISprite* S_PauseMenu = NULL;

void LevelScene::Init(Application* app, int act, int checkpoint) {
    // Load extra level data
    LoadRW(RWList[0], listGroupsPerAct);

    ManiacMode = false;

    this->app = app;
    this->act = act;
    this->checkpoint = checkpoint;

    App = app;
    G = app->g;

    for (int i = 0; i < 360; i++) {
        Sin[i] = sin(i * Math_PI / 180.0f);
        Cos[i] = cos(i * Math_PI / 180.0f);
    }

    // Load and do some important things
    memcpy(levelTextLine1, listGroupsPerAct + 0x70, 32);
    loadCommonTextures(app);
    beginActZoneTransfer(act);

    unsigned int nextTick = SDL_GetTicks();

    aud_BGM_Act1 = new Audio(RWList[1], true);
    aud_BGM_Act2 = new Audio(RWList[2], true);

    app->print(0, "AUDIO (musics) load finished in %.3f seconds.", GetElapsed(nextTick));

    for (int p = 0; p < 32; p++)
        ParallaxMult[p] = 1.f;

    // General Physics Loads
    nextTick = SDL_GetTicks();

    LoadRWX(newFile(res_Common_LVL_Heights_bin, "res/Common/LVL_Heights.bin"), heights);
    LoadRWX(newFile(res_Common_LVL_Angles_bin, "res/Common/LVL_Angles.bin"), angles);

    app->print(0, "GENERAL (physics) load finished in %.3f seconds.", GetElapsed(nextTick));

    // Initalize Act
    InitZone(true, checkpoint, -1, false);
    app->print(0, "ALL LOADS finished in %.3f seconds.", GetElapsed(nextTick));

    // Reset input buffer
    memset(inputBuffer, 0, 60 * sizeof(bool));


    /*
    // TileConfig
    IResource* TileConfig = IResources::Load("Stages/HCZ/HCZ1 TileConfig.bin");
    size_t TileConfigSize = (size_t)SDL_RWsize(TileConfig->RW);
    unsigned char TileConfigData[TileConfigSize];
    SDL_RWread(TileConfig->RW, TileConfigData, 1, TileConfigSize);
    IResources::Close(TileConfig);

    IStream TileConfigReader(TileConfigData);
    int TileCount = TileConfigReader.ReadByte();
    for (int i = 0; i < TileCount; i++) {
        memcpy(Tile[i].Collision1, TileConfigReader.ReadBytes(0x10), 0x10);
        memcpy(Tile[i].Collision2, TileConfigReader.ReadBytes(0x10), 0x10);
        Tile[i].Angle1 = TileConfigReader.ReadByte();
        Tile[i].Angle2 = TileConfigReader.ReadByte();
    }

    // StageConfig
    IResource* StageConfig = IResources::Load("Stages/HCZ/HCZ1 StageConfig.bin");
    size_t StageConfigSize = (size_t)SDL_RWsize(StageConfig->RW);
    unsigned char StageConfigData[StageConfigSize];
    SDL_RWread(StageConfig->RW, StageConfigData, 1, StageConfigSize);
    IResources::Close(StageConfig);

    IStream StageConfigReader(StageConfigData);
    LevelWidth = StageConfigReader.ReadUInt16E();
    LevelHeight = StageConfigReader.ReadUInt16E();
    app->print(3, "Level Size: %X x %X", LevelWidth, LevelHeight);
    for (int y = 0; y < LevelHeight; y++) {
        for (int x = 0; x < LevelWidth; x++) {
            Level[x + y * LevelWidth] = (unsigned short)StageConfigReader.ReadUInt16E();
        }
    }

    // 16x16 Tiles
    LevelTiles = new ITexture(IResources::Load("Stages/HCZ/HCZ1 16x16Tiles.png"), false);
    //*/
}

void LevelScene::LoadLayoutRW(int actt) {
    int         act       = actt & 0xF;
    int         li        = 3 + 0x0 + act;
    int         ci        = 3 + 0x4 + act;
    int         ti        = 3 + 0x8 + act;
    int         ri        = 3 + 0xC + act;

    LoadRW(RWList[li], levelBuffer);
    LoadRW(RWList[ci], chunksBuffer);
    LoadRW(RWList[ti], tilesBuffer);

    ringTotal = 0;
    for (int index = 0; index < SDL_RWsize(RWList[ri]) / 4; index++) {
        unsigned char data[4];
        SDL_RWread(RWList[ri], data, 4, 1);
        ringArrBuffer[ringTotal++] = LevelScene::Ring { (data[0] << 8) + data[1], (data[2] << 8) + data[3], true };
    }
    /*
    LevelScene* scene     = ((ThreadLoader*)data)->scene;
    int         act       = ((ThreadLoader*)data)->act & 0xF;
    int         li        = 3 + 0x0 + act;
    int         ci        = 3 + 0x4 + act;
    int         ti        = 3 + 0x8 + act;
    int         ri        = 3 + 0xC + act;

    LoadRW(RWList[li], scene->levelBuffer);
    LoadRW(RWList[ci], scene->chunksBuffer);
    LoadRW(RWList[ti], scene->tilesBuffer);

    for (int index = 0; index < SDL_RWsize(RWList[ri]) / 4; index++) {
        unsigned char data[4];
        SDL_RWread(RWList[ri], data, 4, 1);
        scene->ringArrBuffer[scene->ringTotal++] = LevelScene::Ring { (data[0] << 8) + data[1], (data[2] << 8) + data[3], true };
    }
    //*/
}
void LevelScene::LoadPalette() {
    int a = (act & 0xF);
    paletteDry = new Texture(app, RWList[23 + 0 + a], 1, 1, true);
    paletteWet = new Texture(app, RWList[23 + 4 + a], 1, 1, true);
    if (a == 1 && (act & 0xF0) != 0x30) {
        paletteBossDry = new Texture(app, RWList[23 + 8 + 0], 1, 3, true);
        paletteBossWet = new Texture(app, RWList[23 + 8 + 2], 3, 1, true);
    }
}
void LevelScene::LoadObjectData() {
    int a = act & 0xF;
    int oi = 3 + 0x10 + a;
    objs_length = (int)LoadRW(RWList[oi], objs);
}
void LevelScene::DoSwitchActZone() {
    if (!doSwitch) return;
    doSwitch = false;
    this->act = 0x10 + (act & 0xF) + 1;
    InitZone(true, 0, -1, true);
}

void LevelScene::LoadMusicAndSounds(Application* app, int act) {
    if (aud_BadnikDestroyed == NULL) {
        // Clear audio loading banks
        for (unsigned int i = 0; i < 0xDF; i++) {
            aud[i] = NULL;
        }

        aud_BadnikDestroyed = newSound(snd_BadnikDestroyed_ogg, "snd/BadnikDestroyed.ogg");
        aud_BossHit = newSound(snd_BossHit_ogg, "snd/BossHit.ogg");
        aud_BreakDeeper = newSound(snd_BreakDeeper_ogg, "snd/BreakDeeper.ogg");
        aud_BubbleGet = newSound(snd_BubbleGet_ogg, "snd/BubbleGet.ogg");
        aud_Checkpoint = newSound(snd_Checkpoint_ogg, "snd/Checkpoint.ogg");
        aud_C_Drown = newSound(snd_C_Drown_ogg, "snd/C_Drown.ogg");
        aud_C_ShortBeep = newSound(snd_C_ShortBeep_ogg, "snd/C_ShortBeep.ogg");
        aud_C_ResultsRings = newSound(snd_C_ResultsRings_ogg, "snd/C_ResultsRings.ogg");
        aud_C_RingLoss = newSound(snd_C_RingLoss_ogg, "snd/C_RingLoss.ogg");
        aud_Die = newSound(snd_Die_ogg, "snd/Die.ogg");
        aud_Jump = newSound(snd_Jump_ogg, "snd/Jump.ogg");
        aud_Ring = newSound(snd_Ring_ogg, "snd/Ring.ogg");
        aud_Roll = newSound(snd_Roll_ogg, "snd/Roll.ogg");
        aud_ShieldBounce = newSound(snd_ShieldBounce_ogg, "snd/ShieldBounce.ogg");
        aud_ShieldBubble = newSound(snd_ShieldBubble_ogg, "snd/ShieldBubble.ogg");
        aud_ShieldEJump = newSound(snd_ShieldEJump_ogg, "snd/ShieldEJump.ogg");
        aud_ShieldElectric = newSound(snd_ShieldElectric_ogg, "snd/ShieldElectric.ogg");
        aud_ShieldFire = newSound(snd_ShieldFire_ogg, "snd/ShieldFire.ogg");
        aud_Skid = newSound(snd_Skid_ogg, "snd/Skid.ogg");
        aud_Spiked = newSound(snd_Spiked_ogg, "snd/Spiked.ogg");
        aud_SpinRev = newSound(snd_SpinRev_ogg, "snd/SpinRev.ogg");
        aud_Spring = newSound(snd_Spring_ogg, "snd/Spring.ogg");

        aud_BGM_LifeGet = newMusic2(snd_BGM_LifeGet_ogg, "snd/BGM_LifeGet.ogg");
        aud_BGM_Invincibility = newMusic2(snd_BGM_Invincibility_ogg, "snd/BGM_Invincibility.ogg");
        aud_BGM_ActClear = newMusic2(snd_BGM_ActCleared_ogg, "snd/BGM_ActCleared.ogg");

        aud_BGM_Boss1 = newMusic(snd_BGM_Boss1_ogg, "snd/BGM_Boss1.ogg");
        aud_BGM_Boss2 = newMusic(snd_BGM_Boss2_ogg, "snd/BGM_Boss2.ogg");
        aud_BGM_Drowning = newMusic2(snd_BGM_Drowning_ogg, "snd/BGM_Drowning.ogg");

        aud[0x3E] = newSound(snd_sfx_S3K_3E_ogg, "snd/sfx/S3K_3E.ogg");
        aud[0x3F] = newSound(snd_sfx_S3K_3F_ogg, "snd/sfx/S3K_3F.ogg");
        aud[0x41] = newSound(snd_sfx_S3K_41_ogg, "snd/sfx/S3K_41.ogg");
        aud[0x42] = newSound(snd_sfx_S3K_42_ogg, "snd/sfx/S3K_42.ogg");
        aud[0x43] = newSound(snd_sfx_S3K_43_ogg, "snd/sfx/S3K_43.ogg");
        aud[0x44] = newSound(snd_sfx_S3K_44_ogg, "snd/sfx/S3K_44.ogg");
        aud[0x45] = newSound(snd_sfx_S3K_45_ogg, "snd/sfx/S3K_45.ogg");

        aud[0x39] = newSound(snd_sfx_S3K_39_ogg, "snd/sfx/S3K_39.ogg");
        aud[0x48] = newSound(snd_sfx_S3K_48_ogg, "snd/sfx/S3K_48.ogg");
        aud[0x49] = newSound(snd_sfx_S3K_49_ogg, "snd/sfx/S3K_49.ogg");
        aud[0x4A] = newSound(snd_sfx_S3K_4A_ogg, "snd/sfx/S3K_4A.ogg");
        aud[0x4D] = newSound(snd_sfx_S3K_4D_ogg, "snd/sfx/S3K_4D.ogg");
        aud[0x56] = newSound(snd_sfx_S3K_56_ogg, "snd/sfx/S3K_56.ogg");
        aud[0x59] = newSound(snd_sfx_S3K_59_ogg, "snd/sfx/S3K_59.ogg");
        aud[0x69] = newSound(snd_sfx_S3K_69_ogg, "snd/sfx/S3K_69.ogg");
        aud[0x6F] = newSound(snd_sfx_S3K_6F_ogg, "snd/sfx/S3K_6F.ogg");

        aud[0x9F] = newSound(snd_sfx_S3K_9F_ogg, "snd/sfx/S3K_9F.ogg");

        aud[0xA9] = newSound(snd_sfx_S3K_A9_ogg, "snd/sfx/S3K_A9.ogg");
        aud[0xB3] = newSound(snd_sfx_S3K_B3_ogg, "snd/sfx/S3K_B3.ogg");
        aud[0xBD] = newSound(snd_sfx_S3K_BD_ogg, "snd/sfx/S3K_BD.ogg");
        aud[0xDB] = newSound(snd_sfx_S3K_DB_ogg, "snd/sfx/S3K_DB.ogg");

        aud[0xB2] = newSound(snd_sfx_S3K_B2_ogg, "snd/sfx/S3K_B2.ogg");
    }

    LoadThreadDone = true;
}

int LevelScene::staticLoad(void* data) {
    if (!((ThreadLoader*)data)->type) {
        return ((ThreadLoader*)data)->scene->loadLayout(data);
    }
    else {
        return ((ThreadLoader*)data)->scene->loadAnims(data);
    }
    return 1;
}
int LevelScene::loadLayout(void *data) {
    LevelScene* scene  = ((ThreadLoader*)data)->scene;
    int act            = ((ThreadLoader*)data)->act;

    //scene->loadUniqueTextures(scene->app, act & 0xF);

    unsigned long nextTick = SDL_GetTicks();
    if (((ThreadLoader*)data)->layout) {
        scene->LoadLayoutRW(act);
    }
    app->print(0, "LAYOUT loaded in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);

    return 1;
}
int LevelScene::loadAnims(void *data) {
    LevelScene* scene  = ((ThreadLoader*)data)->scene;
    int act            = ((ThreadLoader*)data)->act;

    scene->LoadMusicAndSounds(scene->app, act & 0xF);

    return 1;
}
void LevelScene::loadCommonTextures(Application* app) {
    for (int i = 0; i < 256; i++) {
        tex_objects[i] = NULL;
    }

    unsigned long nextTick = SDL_GetTicks();
    if (!T("DebugFont"))
        T("DebugFont") = new ITexture(IResources::Load("Fonts/Font16x16.png"), false);

    if (!T("Common"))
        T("Common") = new ITexture(IResources::Load("Sprites/Common/Texture.png"), false);

    S("Ring") = new ISprite(IResources::Load("Sprites/Common/Level/Ring.spr"), T("Common"));
    S("Monitor") = new ISprite(IResources::Load("Sprites/Common/Level/Monitor.spr"), T("Common"));
    S("Button") = new ISprite(IResources::Load("Sprites/Common/Level/Button.spr"), T("Common"));
    S("Spring") = new ISprite(IResources::Load("Sprites/Common/Level/Spring.spr"), T("Common"));
	S("Bubble") = new ISprite(IResources::Load("Sprites/Common/Level/Bubble.spr"), T("Common"));
    S("Spikes") = new ISprite(IResources::Load("Sprites/Common/Level/Spikes.spr"), T("Common"));
    S("Big Ring") = new ISprite(IResources::Load("Sprites/Common/Level/Big Ring.spr"), T("Common"));
    S("Starpole") = new ISprite(IResources::Load("Sprites/Common/Level/Starpole.spr"), T("Common"));

    S("Shield (Bubble)") = new ISprite(IResources::Load("Sprites/Common/Level/Shield (Bubble).spr"), T("Common"));
    S("Shield (Fire)") = new ISprite(IResources::Load("Sprites/Common/Level/Shield (Fire).spr"), T("Common"));
    S("Shield (Instashield)") = new ISprite(IResources::Load("Sprites/Common/Level/Shield (Instashield).spr"), T("Common"));
    S("Shield (Invincibility)") = new ISprite(IResources::Load("Sprites/Common/Level/Shield (Invincibility).spr"), T("Common"));
    S("Shield (Lightning)") = new ISprite(IResources::Load("Sprites/Common/Level/Shield (Lightning).spr"), T("Common"));

    S("FX (Dash Dust)") = new ISprite(IResources::Load("Sprites/Common/Effects/Dash Dust.spr"), T("Common"));
    S("FX (Enemy Explosion)") = new ISprite(IResources::Load("Sprites/Common/Effects/Enemy Explosion.spr"), T("Common"));
    S("FX (Explosion)") = new ISprite(IResources::Load("Sprites/Common/Effects/Explosion.spr"), T("Common"));
    S("Points") = new ISprite(IResources::Load("Sprites/Common/Effects/Points.spr"), T("Common"));
    S("FX (Skid Dust)") = new ISprite(IResources::Load("Sprites/Common/Effects/Skid Dust.spr"), T("Common"));
    S("FX (Water Run)") = new ISprite(IResources::Load("Sprites/Common/Effects/Water Run.spr"), T("Common"));
    S("FX (Water Splash)") = new ISprite(IResources::Load("Sprites/Common/Effects/Water Splash.spr"), T("Common"));

    spr_HUD_Act = new ISprite(IResources::Load("Sprites/Common/HUD/Act Image.spr"), T("Common"));
    spr_HUD_S3K = new ISprite(IResources::Load("Sprites/Common/HUD/Sonic 3 Banner.spr"), T("Common"));
    spr_HUD_ZoneNameFont = new ISprite(IResources::Load("Sprites/Common/HUD/Font (Zone Name).spr"), T("Common"));
    S_PauseMenu = new ISprite(IResources::Load("Sprites/Common/HUD/Pause Menu.spr"), T("Common"));

    T("Capsule") = new ITexture(IResources::Load("Sprites/Common/Capsule.png"), false);

    buttons = new ISprite(IResources::Load("Sprites/Common/HUD/Mobile Buttons.spr"), T("Common"));

    T("Mania") = new ITexture(IResources::Load("Sprites/Maniac Mode/Texture.png"), false);
    if (ManiacMode) {
        S_HUD = new ISprite(IResources::Load("Sprites/Maniac Mode/HUD.spr"), T("Mania"));
        S_PlayerHeads = new ISprite(IResources::Load("Sprites/Maniac Mode/Player Heads.spr"), T("Mania"));
        S_Font = new ISprite(IResources::Load("Sprites/Maniac Mode/Font.spr"), T("Mania"));
    } else {
        S_HUD = new ISprite(IResources::Load("Sprites/Common/HUD/HUD.spr"), T("Common"));
        S_PlayerHeads = new ISprite(IResources::Load("Sprites/Common/HUD/Lives (Sonic).spr"), T("Common"));
        S_Font = new ISprite(IResources::Load("Sprites/Common/Main Menu/Font (Small).spr"), T("Common"));
        S_FontLives = new ISprite(IResources::Load("Sprites/Common/HUD/Font (Small Numbers).spr"), T("Common"));
    }
    S_IconHyperRing = new ISprite(IResources::Load("Sprites/Maniac Mode/Icon (Hyper Ring).spr"), T("Mania"));

    app->print(0, "COMMON Textures loaded in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void LevelScene::unloadCommonTextures(Application* app) {

}

void LevelScene::getRing(int r) {
    rings += r;
    if (rings >= ringLifeRank) {
        getLife(1);
        ringLifeRank += 100;
    }
}
void LevelScene::getLife(int l) {
    lives += l;
    app->Stack->Push("Life Get!", aud_BGM_LifeGet, 20, 0, 0, false, true);
}
void LevelScene::givePoint(int type, int enemyCombo, int X, int Y) {
    if (type == 0) {
        int frame = 0;
        if (enemyCombo == 1) {
            score += 100;
            frame = 0;
        }
        else if (enemyCombo == 2) {
            score += 200;
            frame = 1;
        }
        else if (enemyCombo == 3) {
            score += 500;
            frame = 2;
        }
        else if (enemyCombo >= 4 && enemyCombo < 16) {
            score += 1000;
            frame = 3;
        }
        else if (enemyCombo >= 16) {
            score += 10000;
            frame = 5;
        }
        else {
            score += 10;
            frame = 4;
        }

        NEW_OBJECT_MACRO(IPoints);
        obj->Frame = frame;
        obj->Create();
    }
    else {
        score += type;
    }
}
void LevelScene::beginActZoneTransfer(int act_n) {
    Chunked = false;

    app->print(3, "Act to load: %02X", act_n & 0xF);

    if ((act_n & 0xF) >= 3)
        return;

    for (int i = 0; i < 512; i++) {
        loadingSpace[i] = NULL;
    }

    if (threadID != NULL)
        SDL_DetachThread(threadID);

    if (threadID2 != NULL)
        SDL_DetachThread(threadID2);

    ThreadLoader* data = new ThreadLoader();
    data->scene = this;
    data->act = act_n;
    data->layout = true;
    data->type = false;
    threadID = SDL_CreateThread(&LevelScene::staticLoad, "co.aurum.Sonic3.ThreadLoader1", (void*)data);
    // this is to prevent overwrite of the pointer to the data
    ThreadLoader* data2 = new ThreadLoader();
    data2->scene = this;
    data2->act = act_n;
    data2->layout = true;
    data2->type = true;
    threadID2 = SDL_CreateThread(&LevelScene::staticLoad, "co.aurum.Sonic3.ThreadLoader2", (void*)data2);
}

void LevelScene::createAnimatedChunks(Application* app, int act) {
    unsigned long nextTick = SDL_GetTicks();
    app->print(0, "ANIMATED Textures loaded in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
}
void LevelScene::unloadAnimatedChunks(Application* app) {

}

IPlayer* LevelScene::GetPlayer(int ID) {
    return MyPlayer;
}

bool LevelScene::CollisionAt(int probeX, int probeY, int layer, int* angle, bool objectCheck, Object** out, bool checkJumpThrough) {
    if (MyPlayer->Ground && std::abs(MyPlayer->Speed) > 7 && MyPlayer->Angle == 0 && probeY >= waterLevel && !MyPlayer->Underwater && MyPlayer->Y < waterLevel && (act & 0xF0) == 0x20) {
        MyPlayer->WaterRunning = true;
        return true;
    }
    else if (!MyPlayer->Ground || std::abs(MyPlayer->Speed) < 7 || MyPlayer->Angle != 0 || MyPlayer->Underwater || MyPlayer->Y > waterLevel || (act & 0xF0) != 0x20) {
        MyPlayer->WaterRunning = false;
    }

    int Layer = layer;
    int X2 = probeX;
    int Y2 = probeY;

    if (loop) {
        if (Y2 < 0)
            Y2 += levelH * 128;
        if (Y2 >= levelH * 128)
            Y2 -= levelH * 128;
    }

    int chunkX = (int)(X2 / 128);
    int chunkY = (int)(Y2 / 128);
    if (X2 < 0) {
        return false;//S3Object::Terrain { false, -1, true };
    }
    if (Y2 < 0) {
        return false;//S3Object::Terrain { false, -1, true };
    }
    int chunkID = level[(level[8] & 0xFF) * 0x100 + (level[9] & 0xFF) - 0x8000 + chunkX + chunkY * levelW] & 0xFF;

    if (chunkID != 0) {
        int blockX = (int)(X2 / 16) - chunkX * 8;
        int blockY = (int)(Y2 / 16) - chunkY * 8;
        int blockData = chunks[chunkID * 192 + blockX * 24 + blockY * 3] & 0xFF;

        int heightID1 = chunks[chunkID * 192 + blockX * 24 + blockY * 3 + 1] & 0xFF;
        int heightID2 = chunks[chunkID * 192 + blockX * 24 + blockY * 3 + 2] & 0xFF;

        int xToCheck = (int)X2 - chunkX * 128 - blockX * 16;
        int yToCheck = (int)Y2 - chunkY * 128 - blockY * 16;

        int solid1 = (blockData >> 6) & 0x3;
        int solid2 = (blockData >> 4) & 0x3;
        int flipY = (blockData >> 3) & 0x1;
        int flipX = (blockData >> 2) & 0x1;

        int height1 = 0;
        int height2 = 0;

        if (flipX == 1) {
            height1 = heights[heightID1 * 16 + 15 - xToCheck] & 0xFF;
            height2 = heights[heightID2 * 16 + 15 - xToCheck] & 0xFF;
        }
        else {
            height1 = heights[heightID1 * 16 + xToCheck] & 0xFF;
            height2 = heights[heightID2 * 16 + xToCheck] & 0xFF;
        }

        int angleFinal = -1;
        if (solid1 > 0 && Layer == 1) {
            angleFinal = angles[heightID1] & 0xFF;
        }
        if (solid2 > 0 && Layer == 0) {
            angleFinal = angles[heightID2] & 0xFF;
        }

        if (angleFinal != -1) {
            if ((solid1 > 0 ||
                solid2 > 0) && angleFinal != 0xFF) { // if not top-solid
                if (flipX == 0) {
                    angleFinal = -angleFinal;
                }
                if (flipY == 1) {
                    angleFinal = (128 - angleFinal) % 256;
                }
            }

            //angleFinal = -angleFinal;

            //angleFinal = (int)std::round(angleFinal / 8.0f) * 8;

            if (angleFinal == 0xFF)
                angleFinal = MyPlayer->AngleMode * 64;
        }

        bool PlayerCol = false;

        if (act == 0x11) {
            if ((int)MyPlayer->X + 20 < 0x2B60) {
                PlayerCol = true;
            }
        }
        else {
            if (checkJumpThrough && MyPlayer->AngleMode == 0 && MyPlayer->Y + 16 < (int)(Y2 / 16) * 16 + yToCheck) {
                PlayerCol = true;
            }
        }

        if (solid1 > 0 && Layer == 1) {
            bool collided = false;
            if (height1 <= 16) {
                if ((yToCheck >= 16 - height1 && (flipY == 0 || solid1 == 1)) ||
                    (yToCheck <       height1 && flipY == 1 && solid1 != 1))
                    collided = true;
            }
            else {
                if ((yToCheck >= 16 - (255 - height1) && flipY == 1 && solid1 != 1) ||
                    (yToCheck <       (255 - height1) && (flipY == 0 || solid1 == 1)))
                    collided = true;
            }

            if (solid1 == 1) {
                if (collided && !PlayerCol) {
                    collided = false;
                }
            }
            if (collided) {
                *angle = wrapAngle((int)(angleFinal * 360.0f / 256.0f));
                return true;//S3Object::Terrain { true, this->Layer, solid1 == 1, 0, height1, (int)X2, (int)Y2, scrWrapAngle((int)(angleFinal * 360.0f / 256.0f)) };
            }
        }
        if (solid2 > 0 && Layer == 0) {
            bool collided = false;
            if (height2 <= 16) {
                if ((yToCheck >= 16 - height2 && flipY == 0) ||
                    (yToCheck <       height2 && flipY == 1))
                    collided = true;
            }
            else {
                if ((yToCheck >= 16 - (255 - height2) && flipY == 1) ||
                    (yToCheck <       (255 - height2) && flipY == 0))
                    collided = true;
            }
            if (solid2 == 1) {
                if (collided && !PlayerCol) {
                    collided = false;
                }
            }
            if (collided) {
                *angle = wrapAngle((int)(angleFinal * 360.0f / 256.0f));
                return true;//S3Object::Terrain { true, this->Layer, solid2 == 1, 0, height2, (int)X2, (int)Y2, scrWrapAngle((int)(angleFinal * 360.0f / 256.0f)) };
            }
        }
    }

    for (unsigned int o = 0; o < (unsigned int)ObjectSolidCount && objectCheck; o++) {
        Object* obj = ObjectsSolid[o];
        if (obj != NULL) {
            if (obj->Active) {
                if (obj->SolidCustomized) {
                    if (obj->CustomSolidityCheck(probeX, probeY, MyPlayer->PlayerID, checkJumpThrough && MyPlayer->Gravity >= 0)) {
                        if (out != NULL)
                            *out = obj;
                        return true;
                    }
                }
                else {
                    if (obj->Solid || (obj->SolidTop && checkJumpThrough && MyPlayer->Gravity >= 0 && MyPlayer->Y + MyPlayer->H / 2 - 4 <= obj->Y - obj->H / 2)) {
                        int obj_X = obj->X;
                        int obj_Y = obj->Y;
                        int obj_W = obj->W / 2;
                        int obj_H = obj->H / 2;
                        if (X2 >= obj_X - obj_W &&
                            Y2 >= obj_Y - obj_H &&
                            X2 <  obj_X + obj_W &&
                            Y2 <  obj_Y + obj_H) {
                            if (out != NULL)
                                *out = obj;
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

void LevelScene::InitZone(bool resetTextures, int check, int specialRing, bool actTransition) {
    int a = act & 0xF;
    ApparentAct = a;
    ActualAct = a;
    ActualZone = (act & 0xF0) / 0x10;
    actMusics[0] = aud_BGM_Act1;
    actMusics[1] = aud_BGM_Act2;

    sonStartPos[0] = 0x00000000;
    sonStartPos[1] = 0x00000000;
    knuStartPos[0] = 0x00000000;
    knuStartPos[1] = 0x00000000;
    for (int n = 0; n < 4; n++) {
        sonStartPos[0] = (sonStartPos[0] * 0x100) + (listGroupsPerAct[0x40 + 0x04 * a + n] & 0xFF);
        sonStartPos[1] = (sonStartPos[1] * 0x100) + (listGroupsPerAct[0x40 + 0x04 * a + n] & 0xFF);
        knuStartPos[0] = (knuStartPos[0] * 0x100) + (listGroupsPerAct[0x50 + 0x04 * a + n] & 0xFF);
        knuStartPos[1] = (knuStartPos[1] * 0x100) + (listGroupsPerAct[0x50 + 0x04 * a + n] & 0xFF);
    }

    int CharacterChoice = 1;

    unsigned long nextTick = SDL_GetTicks();

    memcpy(level, levelBuffer, 0xFFFF * sizeof(char));
    memcpy(ringArr, ringArrBuffer, 1024 * sizeof(Ring));

    if (resetTextures) {
        if (!S("Sonic")) {
            if (ManiacMode) {
                S("Sonic") = new ISprite(IResources::Load("Sprites/Player/ManiaSonic.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaSonicPal.png"), false));
                S("Tails") = new ISprite(IResources::Load("Sprites/Player/ManiaTails.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaTails.png"), false));
                S("Knuckles") = new ISprite(IResources::Load("Sprites/Player/ManiaKnux.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaKnux.png"), false));
            } else {
                S("Sonic") = new ISprite(IResources::Load("Sprites/Player/Sonic.spr"), new ITexture(IResources::Load("Sprites/Player/Texture.png"), false));
                //S("Tails") = new ISprite(IResources::Load("Sprites/Player/ManiaTails.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaTails.png"), false));
                S("Knuckles") = S("Sonic");
            }
        }

        memcpy(chunks, chunksBuffer, 0xFFFF * sizeof(char));
        memcpy(tiles, tilesBuffer, 0x30000 * sizeof(char));

        CreateChunkBOs();

        // Reset Act Data
        checkpoint = 0;
        levelW = level[1];
        levelH = level[5];
        levelSubW = level[3];
        levelSubH = level[7];
        App->print(1, "Level Size: %d x %d", levelW, levelH);

        LoadPalette();

        app->print(0, "GENERAL (InitZone texture reset) load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
    }

    sc = this;

    nextTick = SDL_GetTicks();

    // Reset Level Play Data
    frame = 0;
    frameAnim = 0;
    frameTimer = 0;
    sceneIn = 180;
    sceneOut = -2;
    cameraLock = false;
    rings = 0;
    part_next = 0;
    damages_count = 0;
    ringLifeRank = 100;
    cameraMinX = 0.0f;
    cameraMinY = 0.0f;
    cameraMaxX = 0.0f;
    cameraMaxY = 0.0f;
    cameraAutoScrollX = 0;
    cameraAutoScrollY = 0;

    this->actTransition = actTransition;

    checkpoint = check;
    if (specialRing > -1) {
        check = 0;
    }

    // Play the music
    app->Stack->Clear();
    if ((act & 0xF) > 0)
        app->Stack->Push("Act Music", actMusics[(act & 0xF) - 1], 0, 0, 0, true, false, false, 1 / 44100.f);
    else
        app->Stack->Push("Act Music", actMusics[0], 0, 0, 0, true, false, false, 1 / 44100.f);
    //*/


    // Create textures
    if (resetTextures) {
        CreateUniqueTextures(app, act & 0xF);
        createAnimatedChunks(app, act & 0xF);
    }

    // WaterLevel updating
    if ((act & 0xF) == 1) {
        waterLevel = (unsigned char)listGroupsPerAct[0x60] * 0x100 + (unsigned char)listGroupsPerAct[0x61];
        visualWaterHeight = waterLevel;
    }
    else {
        waterLevel = (unsigned char)listGroupsPerAct[0x62] * 0x100 + (unsigned char)listGroupsPerAct[0x63];
        visualWaterHeight = waterLevel;
    }


    if (checkpoint <= 0)
        ButtonCount = 0;

    nextTick = SDL_GetTicks();

    // Object loading
    LoadObjectData();

    // Reset Damages
    for (unsigned int d = 0; d < 64; d++) {
        damages[d] = Damage { 0, 0, false, 0 };
    }
    damages_count = 0;

    // Reset Objects
    for (unsigned int o = 0; o < objects_count; o++) {
        objects[o] = NULL;
    }
    objects_count = 0;

    for (unsigned int o = 0; o < pathswitchers_count; o++) {
        pathswitchers[o] = NULL;
    }
    pathswitchers_count = 0;

    for (int o = 0; o < ObjectCount; o++)
        Objects[o] = NULL;
    for (int o = 0; o < ObjectSolidCount; o++)
        ObjectsSolid[o] = NULL;
    for (int o = 0; o < ObjectSpringCount; o++)
        ObjectsSpring[o] = NULL;
    for (int o = 0; o < ObjectEnemiesCount; o++)
        ObjectsEnemies[o] = NULL;
    for (int o = 0; o < ObjectBreakableCount; o++)
        ObjectsBreakable[o] = NULL;
    for (int o = 0; o < ObjectPathSwitcherCount; o++)
        ObjectsPathSwitcher[o] = NULL;
    for (int o = 0; o < ParticleCount; o++)
        Particles[o] = NULL;
    ObjectCount = 0;
    ObjectSolidCount = 0;
    ObjectSpringCount = 0;
    ObjectEnemiesCount = 0;
    ObjectBreakableCount = 0;
    ObjectPathSwitcherCount = 0;
    ParticleCount = 0;

    // Create players
    if (!actTransition) {
        player_count = 0;

        MyPlayer = new IPlayer();
		
        S("Sonic") = new ISprite(IResources::Load("Sprites/Player/Sonic.spr"), new ITexture(IResources::Load("Sprites/Player/Texture.png"), false));

        //S("Sonic") = new ISprite(IResources::Load("Sprites/Player/ManiaSonic.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaSonicPal.png"), false));
        S("Tails") = new ISprite(IResources::Load("Sprites/Player/ManiaTails.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaTails.png"), false));
        S("Knuckles") = new ISprite(IResources::Load("Sprites/Player/ManiaKnux.spr"), new ITexture(IResources::Load("Sprites/Player/ManiaKnux.png"), false));

        MyPlayer->App = App;
        MyPlayer->G = G;
        MyPlayer->Scene = this;
        MyPlayer->ForegroundLayer = 0;
        MyPlayer->SpriteDashDust = S("FX (Dash Dust)");
        MyPlayer->SpriteWaterRun = S("FX (Water Run)");

        MyPlayer->SpriteShieldFire = S("Shield (Fire)");
        MyPlayer->SpriteShieldElectric = S("Shield (Lightning)");
        MyPlayer->SpriteShieldBubble = S("Shield (Bubble)");
        MyPlayer->SpriteShieldInsta = S("Shield (Instashield)");

        MyPlayer->X = (sonStartPos[(act & 0xF) / 2] / 0x10000) & 0xFFFF;
        MyPlayer->Y = (sonStartPos[(act & 0xF) / 2]) & 0xFFFF;
        // Sonic
        if (CharacterChoice == 0) {
            MyPlayer->Sprite = S("Sonic");
            MyPlayer->Character = CharacterType::Sonic;
            MyPlayer->Palette1 = new ITexture(IResources::Load("Sprites/Player/PaletteManiaSonic.png"), true);
            if ((act & 0xF0) == 0x20)
                MyPlayer->Palette2 = new ITexture(IResources::Load("Sprites/Player/PaletteManiaSonicHCZ.png"), true);
            else
                MyPlayer->Palette2 = new ITexture(IResources::Load("Sprites/Player/PaletteManiaSonicWater.png"), true);
        }
        // Sonic & Tails
        else if (CharacterChoice == 1) {
            MyPlayer->Sprite = S("Sonic");
            MyPlayer->Character = CharacterType::Sonic;
            MyPlayer->Palette1 = new ITexture(IResources::Load("Sprites/Player/PaletteManiaSonic.png"), true);
            if ((act & 0xF0) == 0x20)
                MyPlayer->Palette2 = new ITexture(IResources::Load("Sprites/Player/PaletteManiaSonicHCZ.png"), true);
            else
                MyPlayer->Palette2 = new ITexture(IResources::Load("Sprites/Player/PaletteManiaSonicWater.png"), true);
        }
        // Tails
        else if (CharacterChoice == 2) {
            MyPlayer->Sprite = S("Tails");
            MyPlayer->Character = CharacterType::Tails;
        }
        // Knuckles
        else if (CharacterChoice == 3) {
            MyPlayer->Sprite = S("Knuckles");
            MyPlayer->Character = CharacterType::Knuckles;
            MyPlayer->X = (knuStartPos[(act & 0xF) / 2] / 0x10000) & 0xFFFF;
            MyPlayer->Y = (knuStartPos[(act & 0xF) / 2]          ) & 0xFFFF;
        }

        MyPlayer->Create();

        cameraX[0] = MyPlayer->X;
        cameraY[0] = MyPlayer->Y;

        playerBuffer[0] = MyPlayer;
		player = MyPlayer;
        player_count++;

        /*
		playerAI = new Player();
		playerAI->PlayerID = 1;
		playerAI->app = app;
		playerAI->sc = this;
		playerAI->CharacterId = 1;
		if (playerAI->CharacterId == 0)
			playerAI->tex = tex_Sonic;
		else if (playerAI->CharacterId == 1) {
			playerAI->tex = tex_Tails;
			playerAI->tex_Tails = tex_Tails_Tails;
		}
		else if (playerAI->CharacterId == 2)
			playerAI->tex = tex_Knuckles;
		playerAI->tex_Insta = tex_Shield_Insta;
		playerAI->tex_Fire = tex_Shield_Fire;
		playerAI->tex_Lightning = tex_Shield_Lightning;
		playerAI->tex_Invinc = tex_Shield_Invincibility;
		playerAI->tex_Bubble = tex_Shield_Bubble;
		playerAI->tex_DashDust = tex_Effect_DashDust;
		playerAI->tex_SkidDust = tex_Effect_SkidDust;
		playerAI->tex_WaterRun = tex_Effect_WaterRun;
		playerAI->tex_BubbleFX = tex_Bubble;

		playerAI->Controllable = false;

		playerAI->x = player->x;
		playerAI->y = player->y;

		// ((UNIQUE))
		playerAI->Action = Actions::Peril;

		playerBuffer[1] = playerAI;
		player_count++;
        */
    }

    app->print(0, "GENERAL (InitZone object data) load finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
    nextTick = SDL_GetTicks();

    // Create Objects
    createObjects(check, specialRing, actTransition);

    // Adjust camera
    cameraX[0] = MyPlayer->X;
    cameraY[0] = MyPlayer->Y;

    app->print(0, "GENERAL (InitZone object creation) finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
    nextTick = SDL_GetTicks();

    if (check == 0 && !actTransition) {
        if (player->Character < CharacterType::Tails) {
            WITH_ALL(
                player->X = (sonStartPos[(act & 0xF) / 2] / 0x10000) & 0xFFFF;
                player->Y = (sonStartPos[(act & 0xF) / 2]          ) & 0xFFFF;
            );
        }
        else {
            WITH_ALL(
                player->X = (knuStartPos[(act & 0xF) / 2] / 0x10000) & 0xFFFF;
                player->Y = (knuStartPos[(act & 0xF) / 2]          ) & 0xFFFF;
            );
        }
    }

    WITH_ALL(
        if (App->DEBUG_START_X != -1)
            MyPlayer->X = App->DEBUG_START_X;
        if (App->DEBUG_START_Y != -1)
            MyPlayer->Y = App->DEBUG_START_Y;
    );

    app->print(0, "GENERAL (player creation) finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);

    nextTick = SDL_GetTicks();

    if (resetTextures) {
        if (threadID != NULL) {
            SDL_DetachThread(threadID);
            threadID = NULL;
        }

        app->print(0, "GENERAL (thread waiting) finished in %.3f seconds.", (SDL_GetTicks() - nextTick) / 1000.0f);
    }
}

void LevelScene::CreateChunkBOs() {
    if (Chunked)
        return;

    for (int us = 0; us < 256; us++) {
        GenerateChunkBO(us);
    }

    Chunked = true;
}
void LevelScene::GenerateChunkBO(int us) {
    if (us == 0)
        return;

    float ox = us % 16;
    float oy = us / 16;

    int CBOindex = 0;

    for (int xx = 0; xx < 16; xx++) {
        for (int yy = 0; yy < 16; yy++) {
            GLfloat CX = xx * 8;
            GLfloat CY = yy * 8;

            int wX = ox * 16 + xx;
            int wY = oy * 16 + yy;
            int index = wX + wY * 256;

            int word = tiles[index * 3] * 0x10000 + tiles[index * 3 + 1] * 0x100 + tiles[index * 3 + 2];

            int ind    = (word & 0x7FF);
            if (CustomAnimTile(ind, 0, -1) >= 2048)
                ind = 0;

            int flipTY = (word >> 17) & 0x1;
            int flipTX = (word >> 16) & 0x1;
            int prior  = (word >> 15) & 0x1;
            int pal    = (word >> 13) & 0x3;
            int flipY  = (word >> 12) & 0x1;
            int flipX  = (word >> 11) & 0x1;

            if (flipTX == 1) {
                flipX = 1 - flipX;
                CX += (xx % 2 == 1 ? -8 : 8);
            }
            if (flipTY == 1) {
                flipY = 1 - flipY;
                CY += (yy % 2 == 1 ? -8 : 8);
            }

            float p = 0.4f - 0.6f * prior;

            float temp;
            float u1 = (ind % 64) / 64.f;
            float u2 = u1 + 1 / 64.f;
            float v1 = (ind / 64) / 64.f;
            float v2 = v1 + 1 / 64.f;

            if (flipX == 1) {
                temp = u2;
                u2 = u1;
                u1 = temp;
            }
            if (flipY == 1) {
                temp = v2;
                v2 = v1;
                v1 = temp;
            }

            GLfloat palF = pal;

            // First triangle (bottom-right)
            ChunkBufferObjects[us].VerticesExtra[CBOindex + 0] = ChunkVertex { CX,     CY + 8, p, palF, u1, v2 };
            ChunkBufferObjects[us].VerticesExtra[CBOindex + 1] = ChunkVertex { CX + 8, CY + 8, p, palF, u2, v2 };
            ChunkBufferObjects[us].VerticesExtra[CBOindex + 2] = ChunkVertex { CX + 8, CY,     p, palF, u2, v1 };
            // Second triangle (top-left)
            ChunkBufferObjects[us].VerticesExtra[CBOindex + 3] = ChunkVertex { CX, CY,         p, palF, u1, v1 };
            ChunkBufferObjects[us].VerticesExtra[CBOindex + 4] = ChunkVertex { CX, CY + 8,     p, palF, u1, v2 };
            ChunkBufferObjects[us].VerticesExtra[CBOindex + 5] = ChunkVertex { CX + 8, CY,     p, palF, u2, v1 };

            CBOindex += 6;
        }
    }

    glGenBuffers(1, &ChunkBufferObjects[us].Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, ChunkBufferObjects[us].Buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * CBOindex * sizeof(GLfloat), ChunkBufferObjects[us].VerticesExtra, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LevelScene::RestorePalette() {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, paletteDry->tex);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, paletteWet->tex);
}

void LevelScene::createObjects(int check, int specialRing, bool actTransition) {
    char unImps[256];
    for (int i = 0; i < 256; i++) {
        unImps[i] = 0x00;
    }

    for (int o = 0; o < 1000; o++) {
        Objects[o] = NULL;
    }

    for (int r = 0; r < ringTotal; r++) {
        IRing* obj = new IRing();
        obj->X = ringArr[r].X;
        obj->Y = ringArr[r].Y;
        obj->Sprite = S("Ring");

        obj->G = G;
        obj->App = App;
        obj->Scene = this;
        Objects[ObjectCount++] = obj;
    }

    // Create Objects
    for (int i = 0; i < objs_length; i += 6) {
        int X = (objs[i + 0] & 0xFF) * 0x100 + (objs[i + 1] & 0xFF);
        int Y = (objs[i + 2] & 0x0F) * 0x100 + (objs[i + 3] & 0xFF);

        if ((objs[i + 0] & 0xFF) == 0x2A &&
            (objs[i + 1] & 0xFF) == 0xC0 &&
            (objs[i + 2] & 0xFF) == 0x05 &&
            (objs[i + 3] & 0xFF) == 0x42 &&
            (objs[i + 4] & 0xFF) == 0x02 &&
            (objs[i + 5] & 0xFF) == 0x01) {
            continue;
        }
        if ((objs[i + 0] & 0xFF) == 0x2A &&
            (objs[i + 1] & 0xFF) == 0xC0 &&
            (objs[i + 2] & 0xFF) == 0x05 &&
            (objs[i + 3] & 0xFF) == 0x88 &&
            (objs[i + 4] & 0xFF) == 0x02 &&
            (objs[i + 5] & 0xFF) == 0x05) {
            continue;
        }
        if ((objs[i + 0] & 0xFF) == 0x31 &&
            (objs[i + 1] & 0xFF) == 0xC0 &&
            (objs[i + 2] & 0xFF) == 0x02 &&
            (objs[i + 3] & 0xFF) == 0x88) {
            continue;
        }

        int ID = objs[i + 4];
        int SubType = objs[i + 5];
        int data = (objs[i + 2] & 0xF0) / 0x10;
        int PRIORITY = (data >> 3) & 0x01;
        int FLIPX = (data >> 1) & 0x01;
        int FLIPY = (data >> 2) & 0x01;

        if (ID == 0x00) { // Ring

        }
        else if (ID == 0x01) { // Monitor
            OBJECT_MACRO(IMonitor);
            obj->Sprite = S("Monitor");
        }
        else if (ID == 0x02) { // Path Switcher
            OBJECT_MACRO(IPathSwitcher);
        }
        else if (ID == 0x03 && ActualZone < 6) { // AIZ_GiantLog
            OBJECT_MACRO(IGiantHollowLog);
        }
        else if (ID == 0x04) { // Collapsing Platform
            OBJECT_MACRO(ICollapsingPlatform);
        }
        else if (ID == 0x05) { // Rock
            OBJECT_MACRO(IRock);
        }
        else if (ID == 0x06 && ActualZone < 6) { // Zipline
            OBJECT_MACRO(IRideVine);
        }
        else if (ID == 0x07) { // Spring
            OBJECT_MACRO(ISpring);
            obj->Sprite = S("Spring");
        }
        else if (ID == 0x08) { // Spikes
            OBJECT_MACRO(ISpikes);
            obj->Sprite = S("Spikes");
        }
        else if (ID == 0x09 && ActualZone < 6) { // Tree Bark
            OBJECT_MACRO(ITreeBark);
        }
        else if (ID == 0x0A && ActualZone < 6) { // Rope Peg
            OBJECT_MACRO(IZiplinePeg);
        }
        else if (ID == 0x0B) { // Ring (unused)

        }
        else if (ID == 0x0C) { // Swing Vine
            OBJECT_MACRO(ISwingVine);
            //obj->Y += 64;
        }
        else if (ID == 0x0D) { // Breakable Wall
            OBJECT_MACRO(IBreakableWall);
        }
        else if (ID == 0x0E) { // Ridge
            OBJECT_MACRO(IRidge);
        }
        else if (ID == 0x33) { // Button
            OBJECT_MACRO(IButton);
        }
        else if (ID == 0x85) { // Special Stage Ring
            OBJECT_MACRO(ISpecialStageRing);
        }

        else if (ID == 0x35 && ActualZone < 6) { // Foreground Plant
            OBJECT_MACRO(IForegroundPlant);
        }
        else if (ID == 0x51 && ActualZone < 6) { // Floating Platform
            OBJECT_MACRO(IFloatingPlatform);
        }
        else if (ID == 0x29 && ActualZone < 6) { // Disappearing Platform
            OBJECT_MACRO(IDisappearingPlatform);
        }
        else if (ID == 0x2A && ActualZone < 6) { // Cork
            OBJECT_MACRO(ICork);
        }
        else if (ID == 0x2C && ActualZone < 6) { // Log Bridge
            OBJECT_MACRO(ILogBridge);
        }
        else if (ID == 0x2D && ActualZone < 6) { // Falling Log
            for (int f = 0; f < 6; f++) {
                OBJECT_MACRO(IFallingLog);
                obj->OffsetY = f * 0x80;
            }
        }
        else if (ID == 0x2E && ActualZone < 6) { // Spiked Log
            OBJECT_MACRO(ISpikedLog);
        }


        /*
        else if (ID == 0x0F) { // Collapsing Bridge
            CollapsingBridge* obj = new CollapsingBridge(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x0F];
            obj->prior = (PRIORITY == 1);
            obj->overAll = true;
            if (FLIPX == 1) {
                obj->flipX = true;
            }
            else {
                obj->flipX = false;
            }
            //obj->onscreen = i / 6;
            obj->audio = aud_BreakDeeper;
            obj->sc = this;            
			obj->app = app;            
			objects[objects_count] = obj;
            objects_count++;
        }
        //*/

        else if (ID == 0x2F) { // Non Animated Sprites
            OBJECT_MACRO(INonAnimatedStill);
            if ((act & 0xF) <= 1)
                obj->Sprite = S("Non Anim 1");
            else
                obj->Sprite = S("Non Anim 2");
        }
        else if (ID == 0x30) { // Animated Sprites
            OBJECT_MACRO(IAnimatedStill);
            obj->Sprite = S("Anim");
        }
        else if (ID == 0x34) { // Starpole
            OBJECT_MACRO(IStarpole);
            obj->Sprite = S("Starpole");

            if (check > 0 && check == SubType) {
                WITH_ALL(
                    MyPlayer->X = X;
                    MyPlayer->Y = Y;
                )

                cameraX[0] = MyPlayer->X;
                cameraY[0] = MyPlayer->Y;
            }
        }
        else if (ID == 0x26) { // Roll Enforcers
            OBJECT_MACRO(IRollEnforcer);
        }
        else if (ID == 0x28) { // Invisible Solid
            OBJECT_MACRO(IInvisibleSolid);
        }
        else if (ID == 0x54) { // Bubbler
		    OBJECT_MACRO(IBubbler);
        }
        else if (ID == 0x6A) { // Invisible Spikes
            OBJECT_MACRO(IInvisibleSolid);
            obj->Painful = true;
        }
        else if (ID == 0x6B) { // Invisible Death
            OBJECT_MACRO(IInvisibleSolid);
            obj->Deadly = true;
        }

        /*
        else if (ID == 0x36) { // BreakableBar
            BreakableBar* obj = new BreakableBar(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->flipX = (FLIPX == 1);
            obj->flipY = (FLIPY == 1);
            obj->prior = (PRIORITY == 1);
            obj->sc = this;
            obj->app = app;
            objects[objects_count++] = obj;
        }
        else if (ID == 0x37) { // Objectx37
            Objectx37* obj = new Objectx37(objs[i + 5]);
            obj->x = X - 48;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x40];
            obj->tex_WaterRush = tex_objects[0x37];

            obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
            objects_count++;
        }
        else if (ID == 0x38) { // Fan
            Fan* obj = new Fan(objs[i + 5]);
            obj->x = X;
            obj->y = Y + 28;
            obj->flipX = FLIPX == 1;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x38];
            obj->tex_Block = tex_objects[0x40];
            obj->tex_BubbleFX = tex_Bubble;
            obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
            objects_count++;
        }
        else if (ID == 0x39) { // LargeFan
            LargeFan* obj = new LargeFan(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x39];
            obj->tex_BubbleFX = tex_Bubble;

            obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
            objects_count++;
        }
        else if (ID == 0x3A) { // Hand Launcher
            HandLauncher* obj = new HandLauncher(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->flipX = FLIPX == 1;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x3A];

            obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
            objects_count++;
        }
        else if (ID == 0x3B) { // Objectx3B (Geyser)
            Objectx3B* obj = new Objectx3B(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x3B];

            obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
            objects_count++;
        }
        else if (ID == 0x3C) { // Door /CNZ/
            Door* obj = new Door(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x40];

            obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
            objects_count++;
        }
        else if (ID == 0x40) { // Block
            Block* obj = new Block(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x40];

            obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
            objects_count++;
        }
        else if (ID == 0x3E) { // ConveyorNode
            int r = i + 6;
            int top = 35;
            // only search 10 objects ahead
            for (r = i + 6; r < i + 6 + top * 6; r += 6) {
                if (ID == objs[r + 4] && objs[i + 5] == objs[r + 5])
                    break;
            }

            if (r < i + 6 + top * 6) {
                int X2 = (objs[r + 0] & 0xFF) * 0x100 + (objs[r + 1] & 0xFF);

                ConveyorNode* obj = new ConveyorNode(objs[i + 5]);
                obj->x = (X + X2) / 2;
                obj->y = Y;
                obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
                obj->w = std::abs(X2 - X) - 16 * 2;
                //obj->tex = tex_objects[0x40];
                if (FLIPX == 1) {
                    obj->flipX = true;
                }

                obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
                objects_count++;
            }
        }
        else if (ID == 0x69) { // Slide Path Switcher
            SlidePathSwitcher* obj = new SlidePathSwitcher(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->flipX = (FLIPX == 1);
            obj->flipY = (FLIPY == 1);
            obj->prior = (PRIORITY == 1);
            obj->sc = this;
            obj->app = app;
            objects[objects_count++] = obj;
        }
        else if (ID == 0x68) { // SpinningColumn
            SpinningColumn* obj = new SpinningColumn(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->flipX = FLIPX;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x68];

            obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
            objects_count++;
        }
        //*/

        /*
        else if (ID == 0x41) { // Balloon
            Balloon* obj = new Balloon(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->flipX = FLIPX;
            obj->flipY = FLIPY;
            obj->prior = PRIORITY;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];

            obj->tex = tex_objects[0x41];

            obj->sc = this;
            obj->app = app;
            objects[objects_count++] = obj;
        }
		*/
        else if (ID == 0x47) { // Barrel
            OBJECT_MACRO(IBarrel);
        }
		/*
        else if (ID == 0x46) { // CNZ Fan
            CNZ_Fan* obj = new CNZ_Fan(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->flipX = FLIPX == 1;
            obj->flipY = FLIPY == 1;
            obj->prior = PRIORITY == 1;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];

            obj->tex = tex_objects[0x46];

            obj->sc = this;
            obj->app = app;
            objects[objects_count++] = obj;
        }
        else if (ID == 0x4A) { // Bumper
            Bumper* obj = new Bumper(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->flipX = FLIPX;
            obj->flipY = FLIPY;
            obj->prior = PRIORITY;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];

            obj->tex = tex_objects[0x4A];

            obj->sc = this;
            obj->app = app;
            objects[objects_count++] = obj;
        }
        else if (ID == 0x4E) { // MegaRotatingColumn
            MegaRotatingColumn* obj = new MegaRotatingColumn(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->flipX = FLIPX;
            obj->flipY = FLIPY;
            obj->prior = PRIORITY;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];

            obj->sc = this;
            obj->app = app;
            objects[objects_count++] = obj;
        }
        //*/

        /*
        else if (ID == 0x6C) { // Bridge
            Bridge* obj = new Bridge(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->tex = tex_objects[0x6C];
            obj->solidTop = true;
            obj->prior = (PRIORITY == 1);
            obj->overAll = true;
            obj->flipX = (FLIPX == 1);
            obj->flipY = (FLIPY == 1);
            obj->sc = this;            obj->app = app;            objects[objects_count] = obj;
            objects_count++;
        }
        else if (ID == 0x6D) { // Water Splash
            WaterSplash* obj = new WaterSplash(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->flipX = (FLIPX == 1);
            obj->flipY = (FLIPY == 1);
            obj->prior = (PRIORITY == 1);
            obj->sc = this;
            obj->app = app;
            objects[objects_count++] = obj;
        }
        else if (ID == 0x6E) { // Water Drop
            WaterDrop* obj = new WaterDrop(objs[i + 5]);
            obj->x = X;
            obj->y = Y;
            obj->data1 = objs[i + 4];
            obj->data2 = objs[i + 5];
            obj->flipX = (FLIPX == 1);
            obj->flipY = (FLIPY == 1);
            obj->prior = (PRIORITY == 1);
            obj->sc = this;
            obj->app = app;
            objects[objects_count++] = obj;
        }
        //*/
		
		/*
        else if (ID == 0x80) { // Hidden Monitor
            OBJECT_MACRO(IMonitor);
            obj->Sprite = S("Monitor");
            obj->Y -= 32;
        }
        */

        /*

        // HCZ Enemies
        else if (ID == 0x93) { // Jawz
            OBJ_MACRO(Jawz);
            obj->xS = X;
            obj->yS = Y;
        }
        else if (ID == 0x94) { // Blastoid
            OBJ_MACRO(Blastoid);
            obj->tex = tex_objects[0x94];
            obj->tex_Explosion = tex_Effect_EnemyExplosion;
        }
        else if (ID == 0x95) { // Buggernaut
            OBJ_MACRO(Buggernaut);
            obj->tex = tex_objects[0x95];
            obj->tex_Explosion = tex_Effect_EnemyExplosion;
        }
        else if (ID == 0x96) { // Hector
            OBJ_MACRO(TurboSpiker);
            obj->player = player;
            obj->xS = X;
            obj->yS = Y;
            obj->tex = tex_objects[0x96];
            obj->tex_Explosion = tex_Effect_EnemyExplosion;
        }
        else if (ID == 0x97) { // MegaChopper
            OBJ_MACRO(MegaChopper);
        }
        else if (ID == 0x98) { // Pointdexter
            OBJ_MACRO(Pointdexter);
            obj->tex = tex_objects[0x98];
            obj->tex_Explosion = tex_Effect_EnemyExplosion;
        }
        //*/
        else if (ID == 0x99 && ActualZone < 6) { // Miniboss (HCZ)
            OBJECT_MACRO(IEggCapsule);
        }
        else if (ID == 0x9A && ActualZone < 6) { // Boss (HCZ)
            OBJECT_MACRO(IEggCapsule);
        }
        // CNZ Enemies
        else if (ID == 0xA6 && ActualZone < 6) { // Miniboss (CNZ)
            OBJECT_MACRO(IEggCapsule);
        }
        else if (ID == 0xA7 && ActualZone < 6) { // Boss (CNZ)
            OBJECT_MACRO(IEggCapsule);
        }
        // AIZ Enemies
        else if (ID == 0x83) { // Button
            OBJECT_MACRO(IButton);
        }
        else if (ID == 0x8C && ActualZone < 6) { // Bloominator
            OBJECT_MACRO(IBloominator);
        }
        else if (ID == 0x8D && ActualZone < 6) { // RhinoBot
            OBJECT_MACRO(IRhinoBot);
        }
        else if (ID == 0x8E && ActualZone < 6) { // Monkey Dude
            OBJECT_MACRO(IMonkeyDude);
        }
        else if (ID == 0x90 && ActualZone < 6) { // Miniboss Cutscene Actor (AIZ)
            OBJECT_MACRO(IAIZMinibossCutscene);
        }
        else if (ID == 0x91 && ActualZone < 6) { // Miniboss (AIZ)
            OBJECT_MACRO(IEggCapsule);
        }
        else if (ID == 0x92 && ActualZone < 6) { // Boss (AIZ)
            OBJECT_MACRO(IAIZBoss);
        }
        else {
            unImps[objs[i + 4] & 0xFF] = 0x79;
        }
    }

    // Print out unimplemented objects
    for (int i = 0; i < 256; i++) {
        if (unImps[i] > 0x00)
            app->print(1, "Unimplemented object type: %02X", i);
    }

    // Run 'Create' event on all objects
    for (int o = 0; o < ObjectCount; o++) {
        Objects[o]->Create();
    }
}

void LevelScene::Update() {
    if (app->realPaused) {
        int topX = app->renderWidth - 160 - 30;
        int topY = app->renderHeight / 2 - 40;
        for (int n = 0; n < 4; n++) {
            float itemX = topX + n * 40;
            float itemY = topY + n * 40 - 6;
            float itemW = 128;
            float itemH = 24 + 12;
            for (int i = 0; i < 4; i++) {
                if (app->input->touchReleased[i]) {
                    int X3 = app->input->touchX[i];
                    int Y3 = app->input->touchY[i];
                    if (X3 >= itemX &&
                        Y3 >= itemY &&
                        X3 <  itemX + itemW &&
                        Y3 <  itemY + itemH) {
                        pauseMenuItemSelected = n;
                        app->input->jumpPressed[0] = true;
                    }
                }
            }
            int X2 = app->input->mouseX;
            int Y2 = app->input->mouseY;
            if (X2 >= itemX &&
                Y2 >= itemY &&
                X2 <  itemX + itemW &&
                Y2 <  itemY + itemH) {
                if (app->input->mouseMoved) {
                    if (pauseMenuItemSelected != n)
                        aud_C_ShortBeep->Play(3);
                    pauseMenuItemSelected = n;
                }
                if (app->input->mouseReleased)
                    app->input->jumpPressed[0] = true;
            }
        }
        if (pauseMenuFrameCounter == 0) {
            pauseMenuItemSelected = 0;
        }
        if (pauseMenuFrameCounter < 6) {
            pauseMenuFrameCounter++;
        }
        if (app->input->upPressed[0] && FadeOut <= 0) {
            pauseMenuItemSelected--;
            aud_C_ShortBeep->Play(3);
        }
        if (app->input->downPressed[0] && FadeOut <= 0) {
            pauseMenuItemSelected++;
            aud_C_ShortBeep->Play(3);
        }
        pauseMenuItemSelected = (pauseMenuItemSelected + 4) % 4;

        if (app->input->jumpPressed[0] && FadeOut <= 0) {
            switch (pauseMenuItemSelected) {
                case 0:
                    app->realPaused = false;
                    app->paused = false;
                    app->input->jumpPressed[0] = false;
                    aud_C_ShortBeep->Play(3);
                    break;
                case 1:
                    if (TransitionAction == -1) {
                        TransitionAction = 1;
                        FadeOut = 20;
                    }
                    break;
                case 2:
                    aud[0xB2]->Play(3);
                    break;
                case 3:
                    app->nextScene = new Scene_MainMenu(app, 0, 0);
                    break;
            }
        }

        if (!app->soundPaused) {
            app->Stack->Pause();
            for (int i = 0; i < 8; i++) {
                Mix_Pause(i);
            }
            app->soundPaused = true;
        }
    }
    else {
        pauseMenuFrameCounter = 0;
        if (app->soundPaused) {
            app->Stack->Resume();
            for (int i = 0; i < 8; i++) {
                Mix_Resume(i);
            }
            app->soundPaused = false;
        }
    }

    if (app->realPaused) return;

    if (!app->paused && (app->input->downPressed[0] || app->input->down[0]) 
	    && (app->input->upPressed[0] || app->input->up[0]) && app->input->mousePressed) {
		
		App->print(3, "Toggling Debug Mode.");
        DebugMode = !DebugMode;
    }

    if (sceneIn > -2) {
        sceneIn -= 1;
    }
    else if (sceneIn == -2) {
        player->ObjectControlled = 0;
    }
    if (sceneIn <= -2 || actTransition) {
        frame++;
        if (!levelComplete && MyPlayer->Action != ActionType::Dead)
            frameTimer++;
    }
    frameAnim++;

    if (frameTimer >= 60 * 60 * 10) {
        frameTimer = 60 * 60 * 10 - 1;
        MyPlayer->Die();
    }

    if (WaterFlash) {
        WaterFlash = false;
    }

    MyPlayer->Update();

    if (loop) {
        if (MyPlayer->Y < 0) {
            MyPlayer->Y += levelH * 128;
            cameraY[App->CurrentViewport] += levelH * 128;
        }
        if (MyPlayer->Y >= levelH * 128) {
            MyPlayer->Y -= levelH * 128;
            cameraY[App->CurrentViewport] -= levelH * 128;
        }
    }

    app->CurrentViewport = 0;
    for (unsigned int o = 0; o < (unsigned int)ObjectCount; o++) {
        Object* obj = Objects[o];
        if (obj != NULL) {
            if (obj->Active) {
                bool OnScreen = (
                    obj->X - cameraX[app->CurrentViewport] >= -obj->W &&
                    obj->Y - cameraY[app->CurrentViewport] >= -obj->H &&
                    obj->X - cameraX[app->CurrentViewport] < app->gameWidth + obj->W &&
                    obj->Y - cameraY[app->CurrentViewport] < app->gameHeight + obj->H);
                obj->OnScreen = OnScreen;
                if (obj->Priority || OnScreen) {
                    if (obj->X + obj->W / 2 >= MyPlayer->X - MyPlayer->W / 2 - 1 &&
                        obj->Y + obj->H / 2 >= MyPlayer->Y - MyPlayer->H / 2 - 1 &&
                        obj->X - obj->W / 2 <  MyPlayer->X + MyPlayer->W / 2 + 1 &&
                        obj->Y - obj->H / 2 <  MyPlayer->Y + MyPlayer->H / 2 + 1) {
                        float wy = (MyPlayer->W + obj->W) * (MyPlayer->Y - obj->Y);
                        float hx = (MyPlayer->H + obj->H) * (MyPlayer->X - obj->X);

                        int hitFrom = 0;

                        if (wy > hx)
                            if (wy > -hx)
                                hitFrom = 3;
                            else
                                hitFrom = 2;
                        else
                            if (wy > -hx)
                                hitFrom = 0;
                            else
                                hitFrom = 1;

                        obj->CollidingWithPlayer = obj->OnCollisionWithPlayer(0 /* PLAYER ID */, hitFrom, 0);
                    }
                    else {
                        obj->CollidingWithPlayer = false;
                    }

                    obj->Update();
                }
            }
        }
    }
    for (unsigned int o = 0; o < (unsigned int)ParticleCount; o++) {
        Object* obj = Particles[o];
        if (obj != NULL) {
            if (obj->Active) {
                obj->Update();
            }
        }
    }

    MyPlayer->LateUpdate();

    // Player handling (Underwater)
    if (MyPlayer->Y > waterLevel) {
        if (!MyPlayer->Underwater) {
            MyPlayer->Underwater = true;
            MyPlayer->Gravity *= 0.25f;
            MyPlayer->Speed *= 0.5f;
            if (MyPlayer->Shield == ShieldType::Electric)
                WaterFlash = true;

            if (MyPlayer->Shield == ShieldType::Fire || MyPlayer->Shield == ShieldType::Electric)
                MyPlayer->Shield = ShieldType::None;
            if (MyPlayer->Action == ActionType::Fly) {
                // audio_stop "flying sound"
            }


            IEnemyExplosion* obj = new IEnemyExplosion();
            obj->X = MyPlayer->X;
            obj->Y = waterLevel - 16;
            obj->G = G;
            obj->App = App;
            obj->Scene = this;
            Particles[ParticleCount] = obj;
            ParticleCount = (ParticleCount + 1) % 50;
            obj->Create();
            obj->Sprite = S("FX (Water Splash)");
            obj->Frame = 0;
            obj->Speed = 4;

            aud[0x39]->Play(2);
            MyPlayer->Vibrate(0, 0.25, 150);
        }
    }
    else if (MyPlayer->Underwater) {
        MyPlayer->Underwater = false;
        MyPlayer->Gravity += 0.0625f;
        //MyPlayer->Gravity *= 2.0f;
        if (MyPlayer->Action == ActionType::Fly) {
            // audio_play "flying sound"
        }

        IEnemyExplosion* obj = new IEnemyExplosion();
        obj->X = MyPlayer->X;
        obj->Y = waterLevel - 16;
        obj->G = G;
        obj->App = App;
        obj->Scene = this;
        Particles[ParticleCount] = obj;
        ParticleCount = (ParticleCount + 1) % 50;
        obj->Create();
        obj->Sprite = S("FX (Water Splash)");
        obj->Frame = 0;
        obj->Speed = 4;

        //particles[part_next++ % 32] = new Particle(tex_Effect_WaterSplash, player->x, (float)waterLevel - 16, 10 * 4, 8 * 4, 4, true);
        aud[0x39]->Play(2);
        MyPlayer->Vibrate(0, 0.25, 150);
    }

    HandleCamera();

    // Wait for fade in
    if (sceneIn > -2 && !actTransition && (act & 0xF) > 0) {
        return;
    }

    // Particle Updating
    for (unsigned int p = 0; p < part_max; p++) {
        if (particles[p] != NULL) {
            if (particles[p]->life > 0) {
                particles[p]->life--;
            }

            particles[p]->x += particles[p]->vel_x;
            particles[p]->y += particles[p]->vel_y;

            if (particles[p]->underwater) {
                if (particles[p]->y < waterLevel) {
                    particles[p] = NULL;
                    continue;
                }
            }

            if (particles[p]->life == 0) {
                particles[p] = NULL;
                continue;
            }
        }
    }

    // Level Complete
    if (levelComplete && levelCompleteTimer == -1) {
        levelCompleteTimer = 10 * 60;
        maxLevelCompleteTimer = levelCompleteTimer;
    }
    if (levelComplete) {
        if (levelCompleteTimer > 0 && !app->realPaused) {
            levelCompleteTimer--;
            WITH_ALL(
                MyPlayer->UnderwaterTimer = 1800;
            );
        }
    }
}

void LevelScene::DrawTile(int ind, float x, float y, float w, float h, float p, int top, int size) {
    DrawTile(ind, x, y, w, h, p, top, size, -1);
}
void LevelScene::DrawTile(int ind, float x, float y, float w, float h, float p, int top, int size, int parRow) {
    if (ind == 0) return;

    ind = CustomAnimTile(ind, 0, parRow);

    #if 0
    for (int i = 0; i < 1 + 0 * AnimTilesCount/* && CurrentTilesetAnimated*/ && ind < 2048; i++) {
        int tileID = AnimTiles[i * 5 + 0];
        int tileCount = AnimTiles[i * 5 + 1];
        /*
        int aniID = AnimTiles[i * 5 + 2];
        int frmCount = AnimTiles[i * 5 + 3];
        int frmSpd = AnimTiles[i * 5 + 4];
        //*/

        if (ind < tileID || ind >= tileID + tileCount) {
            //continue;
        }

        ind = CustomAnimTile(ind, i, parRow);
        /*
        int frm = ((int)cameraX[app->CurrentViewport] / 4);
        if (frmSpd != 0)
            frm = (frameAnim / frmSpd);

        if (frmSpd != 0) {
            if (ind >= tileID && ind < tileID + tileCount) {
                ind = 2048 + aniID + ind - tileID + (tileCount * frm) % (tileCount * frmCount);
                break;
            }
        }
        else {
            if (ind >= tileID && ind < tileID + tileCount) {
                int frm = ((int)cameraX[app->CurrentViewport] / 4);
    			if (frmSpd != 0)
    				frm = (frameAnim / frmSpd);

    			int bgTileColumnCount = 4;
    			int bgTileColumnHeight = 1;
    			int offset = ( (tileCount * frm) / (tileCount * frmCount) ) % bgTileColumnCount;
    			ind = 2048 + aniID + (ind - tileID + offset * bgTileColumnHeight) % (tileCount) + (tileCount * frm) % (tileCount * frmCount);
                break;
            }
        }
        //*/
    }
    #endif

    int tX = ind % 64;
    int tY = ind / 64;

    G->DrawTexture(Tileset, tX * 8, tY * 8 + top, 8, size, x, y, p, w, h);
}
void LevelScene::DrawChunk(int x, int y, int us, int offX, int offY, int maxTilesX, int maxTilesY, float z) {
    DrawChunk(x, y, us, offX, offY, 0, 0, maxTilesX, maxTilesY, z);
}
void LevelScene::DrawChunk(int x, int y, int us, int offX, int offY, int startX, int startY, int maxTilesX, int maxTilesY, float z) {
    int ssX = startX;
    int ssY = startY;

    if (us == -1)
        us = level[(level[8] & 0xFF) * 0x100 + (level[9] & 0xFF) - 0x8000 + x + ((y + levelH) % levelH) * levelW] & 0xFF;

    float ox = us % 16;
    float oy = us / 16;

    if (us == 0) return;

    int pppp = (int)G->GraphicState.PaletteShift;

    for (int xx = ssX; xx < ssX + maxTilesX; xx++) {
        for (int yy = ssY; yy < ssY + maxTilesY; yy++) {
            int CX = (x) * 128 - (int)cameraX[app->CurrentViewport] + xx * 8 + offX;
            int CY = (y) * 128 - (int)cameraY[app->CurrentViewport] + yy * 8 + offY;

            int wX = ox * 16 + xx;
            int wY = oy * 16 + yy;
            int index = wX + wY * 256;
            int word = tiles[index * 3] * 0x10000 + tiles[index * 3 + 1] * 0x100 + tiles[index * 3 + 2];
            int ind = word & 0x7FF;

            if (ind == 0) continue;
            ind = CustomAnimTile(ind, 0, -1);
            if (ind < 2048 && JustInstantDrawAnimTiles) continue;

            int flipTY = (word >> 17) & 0x1;
            int flipTX = (word >> 16) & 0x1;
            int prior = (word & 0x8000) / 0x8000;
            int pal = (word & 0x6000) >> 13;
            int flipY = (word & 0x1000) / 0x1000;
            int flipX = (word & 0x800) / 0x800;

            if (flipTX == 1) {
                flipX = 1 - flipX;
                CX += (xx % 2 == 1 ? -8 : 8);
            }
            if (flipTY == 1) {
                flipY = 1 - flipY;
                CY += (yy % 2 == 1 ? -8 : 8);
            }

            if (CX + 8 < 0)
                continue;
            if (CY + 8 < 0)
                continue;
            if (CX > app->renderWidth)
                continue;
            if (CY > app->renderHeight)
                continue;

            float p = 0.f;
            if (prior != 0)
                p = -0.2f;
            else
                p = 0.4f;

            if (pal >= 4)
                pal = pal % 4;

            if (JustInstantDrawAnimTiles)
                G->PaletteShift(pal);
            else
                G->PaletteShift((pal + pppp) & 0x3);

            DrawTile(ind, CX, CY, 8 * (flipX == 1 ? -1 : 1), 8 * (flipY == 1 ? -1 : 1), p + z, 0, 8);
        }
    }

    G->PaletteShift(pppp);
}
void LevelScene::DrawTileLine(int xxx, int yyy, int line, int top, int size) {
    int baseX = -xxx;
    int y = line / 16;
    int yy = line;

    size = std::min(8 - top, size);

    if (yyy + size <= 0)
        return;
    if (yyy >= app->gameHeight)
        return;
    if (yy * 8 >= levelSubH * 128 && !loop)
        return;

    if (yy * 8 < 0 && loop)
        yy += levelSubH * 16;
    if (yy * 8 >= levelSubH * 128 && loop)
        yy -= levelSubH * 16;

    int cX = baseX;
    int cW = app->renderWidth;

    for (int xx = cX / 8 - 1; xx < cX / 8 + cW / 8 + 2; xx++) {
        int newY = y;

        if (newY < 0)
            newY += levelSubH;
        if (newY >= levelSubH)
            newY -= levelSubH;
        //*/

        int us = level[(level[newY * 4 + 10] & 0xFF) * 0x100 + (level[newY * 4 + 11] & 0xFF) - 0x8000 + (((xx / 16) - BGstart) % BGsize + BGstart)] & 0xFF;
        float ox = us % 16;
        float oy = us / 16;

        if (us == 0) continue;

        //for (int xx = 0; xx < 16; xx++) {
            int wX = ox * 16 + xx % 16;
            int wY = oy * 16 + yy % 16;
            int index = wX + wY * 256;
            int word = tiles[index * 3] * 0x10000 + tiles[index * 3 + 1] * 0x100 + tiles[index * 3 + 2];

            int flipTY = (word >> 17) & 0x1;
            int flipTX = (word >> 16) & 0x1;
            int prior = (word & 0x8000) / 0x8000;
            int pal = (word >> 13) & 0x3;
            int flipY = (word & 0x1000) / 0x1000;
            int flipX = (word & 0x800) / 0x800;
            int ind = word & 0x7FF;

            int CX = (xx / 16) * 128 + (xx % 16) * 8 + xxx;
            int CY = yyy;//y * 128 + yy * 8;

            if (ind == 0) continue;

            if (flipTY == 1) {
                int new_yy = ((yy % 16) / 2) * 2 + (1 - (yy % 2));
                wY = oy * 16 + new_yy;
                index = wX + wY * 256;
                word = tiles[index * 3] * 0x10000 + tiles[index * 3 + 1] * 0x100 + tiles[index * 3 + 2];

                flipTY = (word >> 17) & 0x1;
                flipTX = (word >> 16) & 0x1;
                prior = (word & 0x8000) / 0x8000;
                pal = (word >> 13) & 0x3;
                flipY = (word & 0x1000) / 0x1000;
                flipX = (word & 0x800) / 0x800;
                ind = word & 0x7FF;

                flipY = 1 - flipY;

                if (ind == 0) continue;
            }

            if (flipTX == 1) {
                flipX = 1 - flipX;
                CX += (xx % 2 == 1 ? -8 : 8);
            }

            if (CX + flipX * 8 + 8 <= 0) {
                continue;
            }
            if (CX > app->renderWidth) {
                continue;
            }

            float p = 0.f;
            if (prior != 0)
                p = 3.85f;
            else
                p = 3.95f;

            G->PaletteShift(pal);
            DrawTile(ind, CX, CY, 8 * (flipX == 1 ? -1 : 1), size * (flipY == 1 ? -1 : 1), p, flipY == 1 ? 8 - size - top : top, size, 0);
        //}
    }
    G->PaletteShift(0);

    return;

    for (int x = baseX / 128; x < baseX / 128 + 5; x++) {
        int newY = y;
        int us = level[(level[newY * 4 + 10] & 0xFF) * 0x100 + (level[newY * 4 + 11] & 0xFF) - 0x8000 + ((x - BGstart) % BGsize + BGstart)] & 0xFF;
        float ox = us % 16;
        float oy = us / 16;

        if (us == 0) continue;

        for (int xx = 0; xx < 16; xx++) {
            int wX = ox * 16 + xx % 16;
            int wY = oy * 16 + yy % 16;
            int index = wX + wY * 256;
            int word = tiles[index * 3] * 0x10000 + tiles[index * 3 + 1] * 0x100 + tiles[index * 3 + 2];

            int flipTY = (word >> 17) & 0x1;
            int flipTX = (word >> 16) & 0x1;
            int prior = (word & 0x8000) / 0x8000;
            int pal = (word >> 13) & 0x3;
            int flipY = (word & 0x1000) / 0x1000;
            int flipX = (word & 0x800) / 0x800;
            int ind = word & 0x7FF;

            int CX = x * 128 + (xx % 16) * 8 + xxx;
            int CY = yyy;//y * 128 + yy * 8;

            if (ind == 0) continue;

            if (flipTX == 1) {
                flipX = 1 - flipX;
                CX += (xx % 2 == 1 ? -8 : 8);
            }

            if (CX + flipX * 8 + 8 <= 0) {
                continue;
            }
            if (CX > app->renderWidth) {
                continue;
            }

            if (flipTY == 1) {
                int new_yy = ((yy % 16) / 2) * 2 + (1 - (yy % 2));
                wY = oy * 16 + new_yy;
                index = wX + wY * 256;
                word = tiles[index * 3] * 0x10000 + tiles[index * 3 + 1] * 0x100 + tiles[index * 3 + 2];

                flipTY = (word >> 17) & 0x1;
                flipTX = (word >> 16) & 0x1;
                prior = (word & 0x8000) / 0x8000;
                pal = (word >> 13) & 0x3;
                flipY = (word & 0x1000) / 0x1000;
                //flipX = (word & 0x800) / 0x800;
                ind = word & 0x7FF;

                flipY = 1 - flipY;

                if (ind == 0) continue;
            }

            float p = 0.f;
            if (prior != 0)
                p = 0.8f;
            else
                p = 0.9f;

            G->PaletteShift(pal % 4);
            DrawTile(ind, CX, CY, 8 * (flipX == 1 ? -1 : 1), size * (flipY == 1 ? -1 : 1), p, flipY == 1 ? 8 - size - top : top, size, 0);
        }
    }
    G->PaletteShift(0);
}

void LevelScene::HandleCamera() {
    bool OffCenteredCamera = false;
    if (app->CurrentViewport == 0)
        cameraPlayer = player;
    else
        cameraPlayer = playerAI;

    if (cameraAutoScrollX != 0) {
        cameraX[app->CurrentViewport] += cameraAutoScrollX;

        if (MyPlayer->X - MyPlayer->W / 2 < cameraX[app->CurrentViewport]) {
            MyPlayer->X = std::max(cameraX[app->CurrentViewport] + MyPlayer->W / 2, MyPlayer->X);
            if (MyPlayer->Speed < cameraAutoScrollX)
                MyPlayer->Speed = cameraAutoScrollX;
        }
        if (MyPlayer->X > 8 * OffCenteredCamera + cameraX[app->CurrentViewport] + app->renderWidth / 2) {
            MyPlayer->X = std::min(8 * OffCenteredCamera + cameraX[app->CurrentViewport] + app->renderWidth / 2, MyPlayer->X);
        }

        return;
    }

    if (!loop && MyPlayer->Y - MyPlayer->CameraY > cameraY[App->CurrentViewport] + App->renderHeight)
        MyPlayer->Die();

    if (MyPlayer->Action == ActionType::Dead)
        return;

    float camX = (int)(MyPlayer->DisplayX + MyPlayer->CameraX * 0);
    float camY = (int)(MyPlayer->DisplayY + MyPlayer->CameraY + 16);

    float OffsetX = (camX - (cameraX[app->CurrentViewport] + app->renderWidth / 2.0f - 8 * OffCenteredCamera));
    float OffsetY = (camY - (cameraY[app->CurrentViewport] + app->renderHeight / 2.0f));

    float Max = 8;
    if (MyPlayer->WaterRunning) {
        Max = 16;
    }
    if(std::abs(OffsetX) > 8) {
        OffsetX -= Max * signum(OffsetX);
    }
    else {
        OffsetX  = 0;
    }


    if (MyPlayer->CameraLockTimer > 0)
        OffsetX = 0;


    OffsetX = std::min(std::abs(OffsetX), 16.f) * signum(OffsetX);

    if (!MyPlayer->Ground) {
        if (std::abs(OffsetY) > 32)
            OffsetY -= 32 * signum(OffsetY);
        else
            OffsetY  = 0;
    }

    Max = 6;
    if (std::abs(MyPlayer->Speed) >= 8)
        Max = 24;

    if (MyPlayer->Ground)
        OffsetY = std::min(std::abs(OffsetY), Max) * signum(OffsetY);

    if (cameraMaxX == 0) {
        cameraMaxX = (float)(levelW * 128 - app->renderWidth);
        cameraMaxY = (float)(levelH * 128 - app->renderHeight);
    }

    if (!app->realPaused) {
        cameraX[app->CurrentViewport] += OffsetX;
        cameraY[app->CurrentViewport] += OffsetY;
    }

    //cameraX[app->CurrentViewport] = (float)floor(cameraX[app->CurrentViewport] * 1.f) / 1.f;
    //cameraY[app->CurrentViewport] = (float)floor(cameraY[app->CurrentViewport] * 1.f) / 1.f;

    //cameraX[app->CurrentViewport] = (cameraX[app->CurrentViewport]);
    //cameraY[app->CurrentViewport] = (cameraY[app->CurrentViewport]);

    cameraX[app->CurrentViewport] = std::max(cameraX[app->CurrentViewport], cameraMinX);
    cameraX[app->CurrentViewport] = std::min(cameraX[app->CurrentViewport], cameraMaxX);

    cameraX[app->CurrentViewport] = (int)std::max(cameraX[app->CurrentViewport], 0.0f);
    cameraX[app->CurrentViewport] = (int)std::min(cameraX[app->CurrentViewport], levelW * 128.f);

    cameraY[app->CurrentViewport] = std::max(cameraY[app->CurrentViewport], cameraMinY);
    cameraY[app->CurrentViewport] = std::min(cameraY[app->CurrentViewport], cameraMaxY);

    if (MyPlayer->X - MyPlayer->W / 2 < cameraMinX) {
        MyPlayer->X = std::max(cameraMinX + MyPlayer->W / 2, MyPlayer->X);
        MyPlayer->Speed = 0;
    }
    if (MyPlayer->X + MyPlayer->W / 2 > cameraMaxX + app->renderWidth) {
        MyPlayer->X = std::min(cameraMaxX + app->renderWidth - MyPlayer->W / 2, MyPlayer->X);
        MyPlayer->Speed = 0;
    }

    if (loop) {
        //cameraX[app->CurrentViewport] = camX - app->renderWidth / 2.0f;
        cameraY[app->CurrentViewport] = camY - app->renderHeight / 2.0f;

        if (cameraMaxX == 0) {
            cameraMaxX = (float)(levelW * 128 - app->renderWidth);
            cameraMaxY = (float)(levelH * 128 - app->renderHeight);
        }

        //cameraX[app->CurrentViewport] = std::max(cameraX[app->CurrentViewport], cameraMinX);
        //cameraX[app->CurrentViewport] = std::min(cameraX[app->CurrentViewport], cameraMaxX);
        //return;
    }
}
void LevelScene::DoPaletteWaterStuff() {
    GLint locWaterline = glGetUniformLocation(app->current_shader, "u_waterline");
    GLint locWaterlineStop = glGetUniformLocation(app->current_shader, "u_waterlineStop");

    if (app->ViewportCount == 1) {
        glUniform1f(locWaterline, (app->gameHeight - (visualWaterHeight - cameraY[app->CurrentViewport])) / (float)(app->gameHeight / app->pixelScale));
        glUniform1f(locWaterlineStop, 0.f);
    }
    else {
        glUniform1f(locWaterline, (app->ViewportCount - app->CurrentViewport - 1) * 0.5f + 0.5f * (app->gameHeight - (visualWaterHeight - cameraY[app->CurrentViewport])) / (float)(app->gameHeight / app->pixelScale));
        glUniform1f(locWaterlineStop, (app->ViewportCount - app->CurrentViewport - 1) * 0.49f);
    }
}
void LevelScene::DoBackgroundStuff() {
    BackgroundParallaxRender();
}
void LevelScene::DrawPrettyMuchEverything() {
    int cameraX_BG = (int)cameraX[0];
    int cameraY_BG = (int)cameraY[0];

    ///* Background Chunks
    float multiplier = 1.f;
    int pSum = 0;
    for (int p = 0; p < ParallaxCount; p++) {
        multiplier = ParallaxMult[p] * BGx;
        for (int y = pSum; y < pSum + (ParallaxSizes[p] & 0x7FF); y += 8) {
            int xOffset = (int)(ParallaxAutoScroll[p] * frameAnim / multiplier) - BGxoff;

            int finalX = (-cameraX_BG - BGxCamOff - xOffset) * multiplier;
            int sizeLeft = std::min(pSum + (ParallaxSizes[p] & 0x7FF) - y, 8);

            DrawTileLine(finalX, y - cameraY_BG * BGy - BGyoff, y / 8, y % 8, sizeLeft);

            if (y % 8 + sizeLeft > 8) {
                int overshot = y % 8 + sizeLeft - 8;
                DrawTileLine(finalX, y - cameraY_BG * BGy - BGyoff + sizeLeft - overshot, y / 8 + 1, 0, overshot);
            }
        }
        if (p < 99)
            pSum += ParallaxSizes[p] & 0x7FF;
    }
    //*/

    ///* Foreground Chunks
    for (int x = (int)(cameraX[app->CurrentViewport] / 128); x < (int)(cameraX[app->CurrentViewport] / 128) + 5; x++) {
        for (int y = (int)std::floor(cameraY_BG / 128.f); y < (int)std::floor(cameraY_BG / 128.f) + 3; y++) {
            DrawChunk(x, y, -1, 0, 0, 16, 16, 0.f);
        }
    }
    //*/

    /*
    int x1 = std::max(0, cameraX_BG / 16);
    int x2 = std::min(LevelWidth - 1, x1 + app->renderWidth / 16 + 1);
    int y1 = std::max(0, cameraY_BG / 16);
    int y2 = std::min(LevelHeight - 1, y1 + app->renderHeight / 16 + 1);

    G->PaletteShift(-1);
    for (int x = x1; x < x2; x++) {
        for (int y = y1; y < y2; y++) {
            short tile = Level[x + y * LevelWidth] & 0x3FF;
            bool tileFlipX = (Level[x + y * LevelWidth] & 0x400) == 0x400;
            bool tileFlipY = (Level[x + y * LevelWidth] & 0x800) == 0x800;
            if (tile > 0x0)
                G->DrawTexture(LevelTiles,
                    (tile & 31) * 16,
                    (tile >> 1) & 0xFFF0,
                    16,
                    16,
                    x * 16 - cameraX_BG,
                    y * 16 - cameraY_BG,
                    0.2f,
                    16 * (tileFlipX ? -1 : 1),
                    16 * (tileFlipY ? -1 : 1));
        }
    }
    G->PaletteShift(-1);
    //*/

    // Player Rendering
    MyPlayer->Render(cameraX_BG, cameraY_BG);

    ///* Object Rendering
    int DEBUG_int = 0;  // DEBUG: Debug mouse over
    for (unsigned int o = 0; o < (unsigned int)ObjectCount; o++) {
        Object* obj = Objects[o];
        if (obj != NULL) {
            if (obj->Active) {
                if (obj->Priority || (
                    obj->X - cameraX_BG >= -obj->W &&
                    obj->Y - cameraY_BG >= -obj->H &&
                    obj->X - cameraX_BG < app->gameWidth + obj->W &&
                    obj->Y - cameraY_BG < app->gameHeight + obj->H)) {
                    obj->Render(cameraX_BG, cameraY_BG);

                    if (!App->Mobile) {  // DEBUG: Debug mobile
                        G->PaletteShift(-1);
                        if (App->input->mouseX >= obj->X - cameraX_BG - obj->W / 2 &&
                            App->input->mouseY >= obj->Y - cameraY_BG - obj->H / 2 &&
                            App->input->mouseX <  obj->X - cameraX_BG + obj->W / 2 &&
                            App->input->mouseY <  obj->Y - cameraY_BG + obj->H / 2) {
                            char werwer[50];
                            sprintf(werwer, "ID: %02X", obj->ID);
                            DrawTextDebug(werwer, App->input->mouseX + 4, DEBUG_int + App->input->mouseY, 0, 0);
                            sprintf(werwer, "SubType: %02X", obj->SubType);
                            DrawTextDebug(werwer, App->input->mouseX + 4, DEBUG_int + App->input->mouseY + 10, 0, 0);
                            sprintf(werwer, "Position: %04X %04X", (int)obj->X, (int)obj->Y);
                            DrawTextDebug(werwer, App->input->mouseX + 4, DEBUG_int + App->input->mouseY + 20, 0, 0);

                            DEBUG_int += 35;
                        }
                    }
                }
            }
        }
    }
    //*/
    for (unsigned int o = 0; o < (unsigned int)ParticleCount; o++) {
        Object* obj = Particles[o];
        if (obj != NULL) {
            if (obj->Active) {
                obj->Render(cameraX_BG, cameraY_BG);
            }
        }
    }

    G->PaletteShift(-1);

    if (WaterFlash) {
        G->DrawRectangle(0, waterLevel - cameraY_BG, Z_FRONT * 2, app->renderWidth, app->renderHeight, IColor(0.933f, 0.933f, 0.933f, 1.0f));
    }
}
void LevelScene::DrawAboveEverythingNonHUD() { }

void LevelScene::renderHUD() {
    if ((act & 0xF) < 1)
        return;

    if (!App->Mobile && DebugMode) { // DEBUG: Debug text
        char werwer[50];
        sprintf(werwer, "Angle: %d", MyPlayer->Angle);
        DrawTextDebug(werwer, App->renderWidth, 0, 2, 0);

        sprintf(werwer, "Mode: %d", MyPlayer->AngleMode);
        DrawTextDebug(werwer, App->renderWidth, 10, 2, 0);

        sprintf(werwer, "Ground: %s", MyPlayer->Ground ? "TRUE" : "FALSE");
        DrawTextDebug(werwer, App->renderWidth, 20, 2, 0);
		
		sprintf(werwer, "Landed: %s", MyPlayer->Landed ? "TRUE" : "FALSE");
        DrawTextDebug(werwer, App->renderWidth, 30, 2, 0);

        sprintf(werwer, "Speed: %.3f", MyPlayer->Speed);
        DrawTextDebug(werwer, App->renderWidth, 40, 2, 0);

        sprintf(werwer, "Gravity: %.3f", MyPlayer->Gravity);
        DrawTextDebug(werwer, App->renderWidth, 50, 2, 0);

        sprintf(werwer, "Position: %04X %04X", (int)MyPlayer->X, (int)MyPlayer->Y);
        DrawTextDebug(werwer, App->renderWidth, 60, 2, 0);

        sprintf(werwer, "Camera: %04X %04X", (int)cameraX[0], (int)cameraY[0]);
        DrawTextDebug(werwer, App->renderWidth, 70, 2, 0);

        sprintf(werwer, "Waterline: %04X", (int)waterLevel);
        DrawTextDebug(werwer, App->renderWidth, 78, 2, 0);

        sprintf(werwer, "VisualLayer: %d", MyPlayer->VisualLayer);
        DrawTextDebug(werwer, App->renderWidth, 90, 2, 0);

        sprintf(werwer, "LevelRoutine: %d", LevelRoutine);
        DrawTextDebug(werwer, App->renderWidth, 100, 2, 0);

        sprintf(werwer, "Checkpoint: %d", checkpoint);
        DrawTextDebug(werwer, App->renderWidth, 110, 2, 0);
    }

    float livesX = app->renderWidth - 16 - 48;
    float livesY = 10;

    // Draw STR
    if (ManiacMode) {
        if (!app->Mobile) {
            livesX = 16;
            livesY = app->renderHeight - 12 - 16;
        }
        G->DrawSprite(S_HUD, 0, 0x00, 41, 16, 16, 0x0C, Z_FRONT * 4.5f, 0, IE::LeftAlign | IE::TopAlign);
        G->DrawSprite(S_HUD, 0, 0x10, 41, 16, 16, 0x1C, Z_FRONT * 4.5f, (frameTimer > 9 * 60 * 60 && frameAnim % 16 < 8) ? 1 : 0, IE::LeftAlign | IE::TopAlign);
        G->DrawSprite(S_HUD, 0, 0x20, 41, 16, 16, 0x2C, Z_FRONT * 4.5f, (rings == 0 && frameAnim % 16 < 8) ? 1 : 0, IE::LeftAlign | IE::TopAlign);
    }
    else {
        if (!app->Mobile) {
            livesX = 16;
            livesY = app->renderHeight - 10 - 16;
        }
        G->DrawSprite(S_HUD, 16, 10, Z_FRONT * 4.5f, 0, IE::TopAlign | IE::LeftAlign);
    }

    // Draw Numbers
    if (ManiacMode) {
        // Draw numbers for score 7 places
        for (int n = 0; n < 7; n++) {
            if (6 - n < numPlaces((int)score)) {
                int f = (score / TenTo(6 - n)) % 10;
                G->DrawSprite(S_Font, 61 + n * 8, 12, Z_FRONT * 4.5f - n * 0.01f, f, IE::LeftAlign | IE::TopAlign);
            }
        }

        // Draw numbers for timer
        int minutes = frameTimer / (60 * 60);
        int seconds = (frameTimer / 60) % 60;
        int millis = (frameTimer * 100 / 60) % 100;
        G->DrawSprite(S_Font, 59, 28, Z_FRONT * 4.5f, minutes % 10, IE::LeftAlign | IE::TopAlign);
        for (int n = 0; n < 2; n++) {
            int f = (seconds / TenTo(1 - n)) % 10;
            G->DrawSprite(S_Font, 76 + n * 8, 28, Z_FRONT * 4.5f - n * 0.01f, f, IE::LeftAlign | IE::TopAlign);
        }
        for (int n = 0; n < 2; n++) {
            int f = (millis / TenTo(1 - n)) % 10;
            G->DrawSprite(S_Font, 101 + n * 8, 28, Z_FRONT * 4.5f - n * 0.01f, f, IE::LeftAlign | IE::TopAlign);
        }
        G->DrawSprite(S_Font, 68, 26, Z_FRONT * 4.5f, 10, IE::LeftAlign | IE::TopAlign);
        G->DrawSprite(S_Font, 93, 26, Z_FRONT * 4.5f, 11, IE::LeftAlign | IE::TopAlign);

        // Draw numbers for rings
        for (int n = 0; n < 3; n++) {
            if (2 - n < numPlaces((int)rings)) {
                int f = (rings / TenTo(2 - n)) % 10;
                if (MyPlayer->HyperRings) f += 12;
                G->DrawSprite(S_Font, 93 + n * 8, 44, Z_FRONT * 4.5f - n * 0.01f, f, IE::LeftAlign | IE::TopAlign);
            }
        }
    }
    else {
        // Draw numbers for score 7 places
        for (int n = 0; n < 7; n++) {
            if (6 - n < numPlaces((int)score)) {
                int f = (score / TenTo(6 - n)) % 10 + 26;
                G->DrawSprite(S_Font, 64 + n * 8, 10, Z_FRONT * 4.5f - n * 0.01f, f, IE::LeftAlign | IE::TopAlign);
            }
        }

        // Draw numbers for timer
        int minutes = frameTimer / (60 * 60);
        int seconds = (frameTimer / 60) % 60;
        int millis = (frameTimer * 100 / 60) % 100;
        G->DrawSprite(S_Font, 64, 26, Z_FRONT * 4.5f, minutes % 10 + 26, IE::LeftAlign | IE::TopAlign);
        for (int n = 0; n < 2; n++) {
            int f = (seconds / TenTo(1 - n)) % 10 + 26;
            G->DrawSprite(S_Font, 80 + n * 8, 26, Z_FRONT * 4.5f - n * 0.01f, f, IE::LeftAlign | IE::TopAlign);
        }
        for (int n = 0; n < 2; n++) {
            int f = (millis / TenTo(1 - n)) % 10 + 26;
            G->DrawSprite(S_Font, 104 + n * 8, 26, Z_FRONT * 4.5f - n * 0.01f, f, IE::LeftAlign | IE::TopAlign);
        }
        G->DrawSprite(S_Font, 72, 26, Z_FRONT * 4.5f, 38, IE::LeftAlign | IE::TopAlign);
        G->DrawSprite(S_Font, 96, 26, Z_FRONT * 4.5f, 39, IE::LeftAlign | IE::TopAlign);

        // Draw numbers for rings
        for (int n = 0; n < 3; n++) {
            if (2 - n < numPlaces((int)rings)) {
                int f = (rings / TenTo(2 - n)) % 10 + 26;
                G->DrawSprite(S_Font, 96 + n * 8, 42, Z_FRONT * 4.5f - n * 0.01f, f, IE::LeftAlign | IE::TopAlign);
            }
        }
    }

    // Draw Lives
    if (ManiacMode) {
        if (MyPlayer->HyperRings) {
            G->DrawSprite(S_IconHyperRing, 102 - numPlaces((int)rings) * 8, 44, Z_FRONT * 4.5f, 0, IE::LeftAlign | IE::TopAlign);
        }

        if (MyPlayer->Character == CharacterType::Sonic)
            G->DrawSprite(S_PlayerHeads, livesX, livesY, Z_FRONT * 4.5f, 0, IE::LeftAlign | IE::TopAlign);
        else if (MyPlayer->Character == CharacterType::Tails)
            G->DrawSprite(S_PlayerHeads, livesX, livesY, Z_FRONT * 4.5f, 1, IE::LeftAlign | IE::TopAlign);
        else if (MyPlayer->Character == CharacterType::Knuckles)
            G->DrawSprite(S_PlayerHeads, livesX, livesY, Z_FRONT * 4.5f, 2, IE::LeftAlign | IE::TopAlign);

        G->DrawSprite(S_PlayerHeads, livesX + 18, livesY, Z_FRONT * 4.5f, 3, IE::LeftAlign | IE::TopAlign);

        // Draw numbers for Lives
        for (int n = 0; n < numPlaces((int)lives); n++) {
            int f = HUD_PlaceOfNum((int)lives, n, numPlaces((int)lives));
            G->DrawSprite(S_Font, livesX + 36 + n * 8, livesY + 3, Z_FRONT * 4.5f, f, IE::LeftAlign | IE::TopAlign);
        }
    }
    else {
        G->DrawSprite(S_PlayerHeads, livesX, livesY, Z_FRONT * 4.5f, 0, IE::LeftAlign | IE::TopAlign);

        // Draw numbers for Lives
        /*
        for (int n = 0; n < numPlaces((int)lives); n++) {
            int f = HUD_PlaceOfNum((int)lives, n, numPlaces((int)lives));
            G->DrawSprite(S_FontLives, livesX + 36 + n * 8, livesY + 3, Z_FRONT * 4.5f, f, IE::LeftAlign | IE::TopAlign);
        }
        //*/

        for (int n = 0; n < 2; n++) {
            if (1 - n < numPlaces((int)lives)) {
                int f = (lives / TenTo(1 - n)) % 10;
                G->DrawSprite(S_FontLives, livesX + 32 + n * 8, livesY + 8, Z_FRONT * 4.5f - n * 0.01f, f, IE::LeftAlign | IE::TopAlign);
            }
        }
    }

    /*
    G->DrawSprite(S_Font, 10, 80, Z_FRONT * 4.5f, MyPlayer->AngleMode, IE::LeftAlign | IE::TopAlign);
    G->DrawSprite(S_Font, 26, 80, Z_FRONT * 4.5f, MyPlayer->AngleModeLast, IE::LeftAlign | IE::TopAlign);

    for (int n = 0; n < numPlaces(MyPlayer->Angle); n++) {
        int f = HUD_PlaceOfNum(MyPlayer->Angle, n, numPlaces(MyPlayer->Angle));
        G->DrawSprite(S_Font, 10 + n * 8, 100, Z_FRONT * 4.5f, f, IE::LeftAlign | IE::TopAlign);
    }
    for (int n = 0; n < numPlaces(MyPlayer->AngleLast); n++) {
        int f = HUD_PlaceOfNum(MyPlayer->AngleLast, n, numPlaces(MyPlayer->AngleLast));
        G->DrawSprite(S_Font, 60 + n * 8, 100, Z_FRONT * 4.5f, f, IE::LeftAlign | IE::TopAlign);
    }
    //*/

    if (lives > 99)
        lives = 99;


    if (app->Mobile)
        app->input->DrawControls(buttons);
}
void LevelScene::renderPauseScreen() {
    if (!app->realPaused)
        return;

    if (!App->Mobile)
        return;

    //app->drawRectangle(app->renderWidth - 110, 0, 110, app->gameHeight, 0, 178 / 225.f, 0, 1);

    int topX = app->renderWidth - 160 - 30;
    int topY = app->renderHeight / 2 - 40;
    int curX;
    //int curY;

    // -70
    // +100

    // Bottom Right
    app->SetBlend(200 / 225.f, 0, 0, 1);
    curX = app->renderWidth + 70;
    //curY = 0;
    G->DrawSprite(S_PauseMenu, curX + (int)((6 - pauseMenuFrameCounter) / 6.f * 256), 0, Z_FRONT * 4.9990f, 0, IE::RightAlign | IE::TopAlign);
    G->DrawRectangle(curX - 128 + (int)((6 - pauseMenuFrameCounter) / 6.f * 256), 256 - 96, Z_FRONT * 4.9991f, 128, 96, IColor(1, 1, 1, 1.0f));
    G->DrawRectangle(curX + (int)((6 - pauseMenuFrameCounter) / 6.f * 256), 0, Z_FRONT * 4.9992f, 128, 225, IColor(1, 1, 1, 1.0f));
    //app->drawRectangle(curX - 128 + (int)((6 - pauseMenuFrameCounter) / 6.f * 256), 256 - 96, 128, 96,   app->Blend[0], app->Blend[1], app->Blend[2], 1);
    //app->drawRectangle(curX       + (int)((6 - pauseMenuFrameCounter) / 6.f * 256), 0, 128, 225,         app->Blend[0], app->Blend[1], app->Blend[2], 1);

    /*/ Top Right
    app->SetBlend(0, 80 / 225.f, 190 / 225.f, 1);
    curX = app->renderWidth - 30 + 70;
    curY = 60;
    G->Scale(1.f, -1.f);
    G->DrawSprite(S_PauseMenu, curX + (int)((6 - pauseMenuFrameCounter) / 6.f * 256), curY, Z_FRONT * 4.9993f, 0, IE::RightAlign | IE::MiddleAlign);
    G->Scale(1.f, 1.f);
    app->drawRectangle(curX - 128 + (int)((6 - pauseMenuFrameCounter) / 6.f * 256), curY, 128, 96,          app->Blend[0], app->Blend[1], app->Blend[2], 1);
    app->drawRectangle(curX       + (int)((6 - pauseMenuFrameCounter) / 6.f * 256), curY, 64, 225,          app->Blend[0], app->Blend[1], app->Blend[2], 1);
    //*/

    //app->SetBlend(33 / 255.f, 33 / 255.f, 33 / 255.f, 1);
    for (int i = 0; i < 4; i++) {
        //app->drawSpriteOPT(topX - 000 + i * 40, topY + i * 40 + 2, 256, 20, app->drawZ + 0.001f, 0 / 256.f, (160 - 24) / 256.f, 256 / 256.f, 20 / 256.f, 0);
        //app->drawSpriteOPT(topX - 110 + i * 40, topY + i * 40 + 2, 256, 20, app->drawZ + 0.001f, 0 / 256.f, (160 - 24) / 256.f, 256 / 256.f, 20 / 256.f, 0);
    }
    //app->renderSpriteOPT(tex_Side, 0);

    for (int i = 0; i < 4; i++) {
        // Text
        if (i != pauseMenuItemSelected)
            G->DrawSprite(S_PauseMenu, 128, 160 + i * 24, 128, 24, topX + i * 40, topY + i * 40, Z_FRONT * 4.9995f, 0, IE::LeftAlign | IE::TopAlign);
    }
    G->DrawSprite(S_PauseMenu, 0, 136, 256, 20, topX + pauseMenuItemSelected * 40 - 110 + 4, topY + pauseMenuItemSelected * 40 + 2 + 4, Z_FRONT * 4.9994f, 0, IE::LeftAlign | IE::TopAlign);
    G->DrawSprite(S_PauseMenu, 0, 136, 256, 20, topX + pauseMenuItemSelected * 40 - 000 + 4, topY + pauseMenuItemSelected * 40 + 2 + 4, Z_FRONT * 4.9994f, 0, IE::LeftAlign | IE::TopAlign);

    G->DrawSprite(S_PauseMenu, 128, 160 + pauseMenuItemSelected * 24, 128, 24, topX + pauseMenuItemSelected * 40, topY + pauseMenuItemSelected * 40, Z_FRONT * 4.99945f, 0, IE::LeftAlign | IE::TopAlign);
}
void LevelScene::renderTransition() {
    if ((act & 0xF) < 1)
        return;

    // Opening Transition
    float time = 120.0f;
    int fullOff = (app->gameWidth - app->renderWidth) / 2;

    if (sceneIn > -2) {
        const char* text = levelTextLine1;
        const char* text2 = levelTextLine2;
        char width[27] = { 11, 13, 13, 14, 10,
            10, 12, 10,  3,  7,
            10,  7, 24,  9, 24,
            11, 24, 11,  9, 10,
            8, 13, 24, 14, 12, 11, 12};

        float x = std::max(0.0f, std::min(1.0f, (time - sceneIn) / 20.0f))
        - std::max(0.0f, std::min(1.0f, (40.0f - sceneIn) / 10.0f));

        float simn = std::sin((M_PI * x) / 2.0f);
        float start = -88;

        G->PaletteShift(4);
        G->DrawSprite(spr_HUD_S3K, 104 - fullOff, start + 176.f * simn, Z_FRONT * 4.8f, 0, IE::LeftAlign | IE::MiddleAlign);

        int ww = -8;
        int addx = 0;

        start = app->gameWidth - fullOff - 64.0f - 24.0f;

        /*

        Attractive
        Similar Interests
        Single

        Pick two

        */

        // Calculate width of TEXT1
        for (unsigned char p = 0; p < strlen(text); p++) {
            int frame = text[p] - 65;
            if (text[p] == ' ') {
                frame = 26;
            }
            ww += width[frame] + 2;
        }
        G->DrawRectangle((float)start + app->gameWidth - (float)simn * app->gameWidth - ww, 112.f - 4, Z_FRONT * 4.85f, ww, 16, IColor(0.145098f, 0.145098f, 0.145098f, 1.0f));

        for (unsigned char p = 0; p < strlen(text); p++) {
            int frame = text[p] - 65;
            if (text[p] != ' ') {
                G->DrawSprite(spr_HUD_ZoneNameFont,
                    start + app->gameWidth - simn * app->gameWidth - ww + addx,
                    112.f, Z_FRONT * (4.87f + p * 0.001f), frame, IE::CenterAlign | IE::MiddleAlign);
                addx += width[frame] + 2;
            }
            else {
                addx += width[26] + 2;
            }
        }

        int w = -8;
        // Calculate width of TEXT2
        for (unsigned char p = 0; p < strlen(text2); p++) {
            int frame = text2[p] - 65;
            if (text2[p] == ' ') {
                frame = 26;
            }
            w += width[frame] + 2;
        }
        G->DrawRectangle((float)start + app->gameWidth - (float)simn * app->gameWidth - w, 112.f - 4 + 32, Z_FRONT * 4.85f, w, 16, IColor(0.145098f, 0.145098f, 0.145098f, 1.0f));

        addx = 0;
        for (unsigned char p = 0; p < strlen(text2); p++) {
            int frame = text2[p] - 65;
            if (text2[p] != ' ') {
                G->DrawSprite(spr_HUD_ZoneNameFont,
                    start + app->gameWidth - simn * app->gameWidth - w + addx,
                    144.f, Z_FRONT * (4.87f + p * 0.01f), frame, IE::CenterAlign | IE::MiddleAlign);
                addx += width[frame] + 2;
            }
            else {
                addx += width[26] + 2;
            }
        }


        x =
          std::max(0.0, std::min(1.0, ((time - 5.0) - sceneIn) / 20.0))
        - std::max(0.0, std::min(1.0, (40.0 - sceneIn) / 10.0));

        simn = std::sin((M_PI * x) / 2.0f);
        start = app->gameWidth - fullOff - 64.0f - 36.0f;

        G->DrawSprite(spr_HUD_Act, (float)start + app->gameWidth - (float)simn * app->gameWidth, 176, Z_FRONT * 4.999f, ApparentAct - 1, IE::RightAlign | IE::MiddleAlign);

        if (!actTransition)
            G->SetFade(std::max(0.f, std::min(1.f, ((float)sceneIn - 60.f) / 20.f)));
    }
    else {
        if (FadeOut > -1)
            G->SetFade((20 - FadeOut) / 20.f);
    }
}
void LevelScene::renderResultsScreen() {
    if ((act & 0xF) < 1)
        return;
    if (!levelComplete)
        return;

    int NonManiacCharOffset = 0;
    if (!ManiacMode)
        NonManiacCharOffset = 26;

    int t = maxLevelCompleteTimer - levelCompleteTimer;
    if (t == 0) {
        resultsSoundTick = 0;
    }
    if (t > 0) {
        int timeBonus = 6969;
        int seconds = frameTimer / 60;
        if (seconds < 60) {
            timeBonus = 50000;
        }
        else if (seconds < 1 * 60 + 30) {
            timeBonus = 10000;
        }
        else if (seconds < 1 * 60 + 60) {
            timeBonus = 5000;
        }
        else if (seconds < 2 * 60 + 30) {
            timeBonus = 4000;
        }
        else if (seconds < 2 * 60 + 60) {
            timeBonus = 3000;
        }
        else if (seconds < 3 * 60 + 30) {
            timeBonus = 1000;
        }
        else if (seconds < 9 * 60 + 59) {
            timeBonus = 100;
        }
        else {
            timeBonus = 100000;
        }
        if (t == 1) {
            awardedScore = timeBonus + rings * 100;
            resultsRingSoundPlayed = false;
        }
        else if (t > 60 * 5) {
            if (resultsSoundTick % 3 == 0) {
                aud_C_ShortBeep->Play(3);
                resultsSoundTick++;
            }

            if (awardedScore >= 100) {
                awardedScore -= 100;
                score += 100;
                resultsSoundTick++;
            }
            else if (awardedScore >= 10) {
                awardedScore -= 10;
                score += 10;
                resultsSoundTick++;
            }
            else if (awardedScore >= 1) {
                awardedScore -= 1;
                score += 1;
                resultsSoundTick++;
            }
            if (awardedScore == 0 && !resultsRingSoundPlayed) {
                resultsSoundTick = -1;
                aud_C_ResultsRings->Play(3);
                resultsRingSoundPlayed = true;
            }

            if (awardedScore > 0) {
                levelCompleteTimer++;
            }
        }
        float tTime = std::min(t / 40.0f, 1.f);
        float sX = -0.5f * std::cos(M_PI * tTime) + 0.5f;
        // Player
        app->drawSprite(tex_Results_Player, sX * app->renderWidth - app->renderWidth * 0.5f + 8, 64, 419, 0, 2, 1);

        // Got
        app->drawSprite(tex_Results_GotThrough, sX * -app->renderWidth + app->renderWidth * 1.5f + 16, 64, 419, 1, 0, 1);

        tTime = std::max(0.f, std::min((t - 10) / 40.0f, 1.f));
        sX = -0.5f * std::cos(M_PI * tTime) + 0.5f;
        // Through
        app->drawSprite(tex_Results_GotThrough, sX * -app->renderWidth + app->renderWidth * 1.5f, 88, 419, 0, 1, 1);

        tTime = std::max(0.f, std::min((t - 20) / 40.0f, 1.f));
        sX = -0.5f * std::cos(M_PI * tTime) + 0.5f;
        // Act
        G->DrawSprite(spr_HUD_Act, sX * -app->renderWidth + app->renderWidth * 1.5f + 64, 80, Z_FRONT * 4.999f, ApparentAct - 1, IE::LeftAlign | IE::MiddleAlign);
        // Bonus (Time)
        app->drawSprite(tex_Results_BonuTotal, sX * app->renderWidth - app->renderWidth * 0.5f - 60, 112, 419, 1, 0, 1);
        // Bonus (Ring)
        app->drawSprite(tex_Results_BonuTotal, sX * app->renderWidth - app->renderWidth * 0.5f - 60, 128, 419, 1, 0, 1);

        // Total
        app->drawSprite(tex_Results_BonuTotal, sX * app->renderWidth - app->renderWidth * 0.5f - 80, 160, 419, 0, 0, 1);

        int places;
        // Number (Time Bonus)
        places = numPlaces(timeBonus);
        for (int n = 0; n < places; n++) {
            int f = HUD_PlaceOfNum(timeBonus, n, places);
            G->DrawSprite(S_Font, sX * -app->renderWidth + app->renderWidth * 1.5f + 96 + (n - places) * 8, 112, Z_FRONT * 4.5f, f + NonManiacCharOffset, IE::LeftAlign | IE::TopAlign);
        }

        // Number (Ring Bonus)
        places = numPlaces(rings * 100);
        for (int n = 0; n < places; n++) {
            int f = HUD_PlaceOfNum(rings * 100, n, places);
            G->DrawSprite(S_Font, sX * -app->renderWidth + app->renderWidth * 1.5f + 96 + (n - places) * 8, 128, Z_FRONT * 4.5f, f + NonManiacCharOffset, IE::LeftAlign | IE::TopAlign);
        }

        // Number (Total)
        places = numPlaces(std::max(0, awardedScore));
        for (int n = 0; n < places; n++) {
            int f = HUD_PlaceOfNum(std::max(0, awardedScore), n, places);
            G->DrawSprite(S_Font, sX * -app->renderWidth + app->renderWidth * 1.5f + 96 + (n - places) * 8, 160, Z_FRONT * 4.5f, f + NonManiacCharOffset, IE::LeftAlign | IE::TopAlign);
        }


        /*app->drawSpriteOPT(
                            sX * app->renderWidth - app->renderWidth * 0.5f - 60 - 40,
                            112 - 4 + 1,

                            32,
                            32,

                            app->drawZ,

                            0.f, 1.f / 3.f, 0.8f, 2.f / 3.f, 8);
        app->renderSpriteOPT(tex_HUD_STR->tex, 8);*/
    }
}

void LevelScene::Render() {
    G->DrawRectangle(0, 0, Z_BACK * 4, app->renderWidth, app->renderHeight, IColor(0.0f, 0.0f, 0.0f, 1.0f));

    glUniform1i(G->Loc_UseTex, 1);
    glUniform1f(G->Loc_Fade, G->Fade);
    glUniform1i(G->Loc_FadeToWhite, 0);

    ///*
    glEnableVertexAttribArray(G->Loc_Position);
    glEnableVertexAttribArray(G->Loc_TexCoord);
    glEnableVertexAttribArray(G->Loc_VColor);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Tileset->GL_Tex);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, paletteDry->tex);
        glUniform1i(G->Loc_Palette1, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, paletteWet->tex);
        glUniform1i(G->Loc_Palette2, 2);

        int us;
        for (int x = (int)(cameraX[app->CurrentViewport] / 128); x < (int)(cameraX[app->CurrentViewport] / 128) + 5; x++) {
            for (int y = (int)std::floor(cameraY[app->CurrentViewport] / 128.f); y < (int)std::floor(cameraY[app->CurrentViewport] / 128.f) + 3; y++) {
                us = level[(level[8] & 0xFF) * 0x100 + (level[9] & 0xFF) - 0x8000 + x + ((y + levelH) % levelH) * levelW] & 0xFF;

                if (x < 0) continue;
                if (y < 0 && !loop) continue;
                if (x >= levelW) continue;
                if (y >= levelH && !loop) continue;
                if (us == 0) continue;

                glUniform4f(G->Loc_Translate, x * 128 - std::floor(cameraX[app->CurrentViewport]), y * 128 - std::floor(cameraY[app->CurrentViewport]), 0.f, 0.f);

                glBindBuffer(GL_ARRAY_BUFFER, ChunkBufferObjects[us].Buffer);

                glVertexAttribPointer(G->Loc_Position, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));
                glVertexAttribPointer(G->Loc_TexCoord, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(4 * sizeof(GLfloat)));
                glVertexAttribPointer(G->Loc_VColor, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(0 * sizeof(GLfloat)));

                // 1536 is the amount of vertices per chunk
                // = 3 verts per tri
                // * 2 tris per tile
                // * 256 tiles per chunk
                glDrawArrays(GL_TRIANGLES, 0, 1536);
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUniform4f(G->Loc_Translate, 0.f, 0.f, 0.f, 0.f);

    glDisableVertexAttribArray(G->Loc_Position);
    glDisableVertexAttribArray(G->Loc_TexCoord);
    glDisableVertexAttribArray(G->Loc_VColor);

    G->DefaultPalette1 = paletteDry->tex;
    G->DefaultPalette2 = paletteWet->tex;
    //*/

    if (LoadThreadDone && threadID2 != NULL) {
        SDL_DetachThread(threadID2);
        threadID2 = NULL;
		//SDL_DetachThread(threadID);
		//threadID = NULL;
    }

    if (app->realPaused) {
        if (pauseTimer < 5)
            pauseTimer++;
        app->SetBlend(-pauseTimer / 6.5f, 1.f, -2, 1); // to set to black and white
    }
    else {
        pauseTimer = 0;
    }

    DoPaletteWaterStuff();

    DoBackgroundStuff();

    DrawPrettyMuchEverything();

    DrawAboveEverythingNonHUD();

    renderHUD();

    renderResultsScreen();

    renderTransition();

    if (!app->Mobile && app->input->mouse) {
        /*float sel = 1.0f;
        S3Object::Terrain terr = player->collision_line_terrain(app->input->mouseX + cameraX[app->CurrentViewport], app->input->mouseY + cameraY[app->CurrentViewport], app->input->mouseX + cameraX[app->CurrentViewport], app->input->mouseY + cameraY[app->CurrentViewport]);
            char werwer[50];
            sprintf(werwer, "%X", app->input->mouseX + (int)cameraX[app->CurrentViewport]);
            DrawTextDebug(werwer, app->input->mouseX + 5, app->input->mouseY + 5, 0, 0);
            sprintf(werwer, "%X", app->input->mouseY + (int)cameraY[app->CurrentViewport]);
            DrawTextDebug(werwer, app->input->mouseX + 5, app->input->mouseY + 15, 0, 0);
        if (terr.Active) {
            sel = 0.0f;
            if (terr.Platform)
                drawText(tex_Font, "TOP SOLID", app->input->mouseX + 5, app->input->mouseY + 0, 0, 2, 1);
            else
                drawText(tex_Font, "SOLID", app->input->mouseX + 5, app->input->mouseY + 0, 0, 2, 1);
        }
        app->drawRectangle(app->input->mouseX - 4.0f, app->input->mouseY, 4, 1, sel, terr.Platform, 1.0f - sel, 0.5f);
        app->drawRectangle(app->input->mouseX + 1.0f, app->input->mouseY, 4, 1, sel, terr.Platform, 1.0f - sel, 0.5f);
        app->drawRectangle(app->input->mouseX, app->input->mouseY - 4.0f, 1, 4, sel, terr.Platform, 1.0f - sel, 0.5f);
        app->drawRectangle(app->input->mouseX, app->input->mouseY + 1.0f, 1, 4, sel, terr.Platform, 1.0f - sel, 0.5f);*/
    }

    /*char tesxt[0x40];
    sprintf(tesxt, "Sonic %d %d (%04X %04X)", (int)player->x, (int)player->y, (int)player->x, (int)player->y);
    app->SetBlend(0, 0, 0, 1);
    DrawTextDebug(tesxt, app->renderWidth, app->renderHeight, 2, 2);
    app->SetBlend(1, 1, 1, 1);
    DrawTextDebug(tesxt, app->renderWidth - 1, app->renderHeight - 1, 2, 2);*/

    /*MusicStackNode* next = app->Stack->Top;
    int DebugY = 20;
    char Label[32];

    if (app->Stack->ToAddToTop != NULL) {
        sprintf(Label, "%s (%d)", app->Stack->ToAddToTop->Label, app->Stack->ToAddToTop->Priority);
        app->SetBlend(0, 0, 0, 1);
        DrawTextDebug(Label, app->renderWidth, 10, 2, 2);
        app->SetBlend(1, 0, 0, 1);
        DrawTextDebug(Label, app->renderWidth - 1, 10 - 1, 2, 2);
    }

    while (next != NULL) {
        sprintf(Label, "%s (%d) %d %d %.3f", next->Label, next->Priority, next->Looping, next->Stopping, next->Progress);

        app->SetBlend(0, 0, 0, 1);
        DrawTextDebug(Label, app->renderWidth, DebugY, 2, 2);
        app->SetBlend(1, 1, 1, 1);
        DrawTextDebug(Label, app->renderWidth - 1, DebugY - 1, 2, 2);
        DebugY += 10;

        next = next->Next;
    }*/

    if (app->realPaused)
        app->SetBlend(1.f, 1.f, -3, 1); // to remove black and white

    renderPauseScreen();

    int line = 1;
    if ((DebugMode || !app->Mobile) && false) {
        char Label[100];

        app->SetBlend(1, 1, 1, 1);
        WITH_ALL(
            sprintf(Label, "%s (%04X %04X) Spd %d Grv %d Grd %d", "P", (int)player->X, (int)player->Y, (int)player->Speed, (int)player->Gravity, (int)player->Ground);
            DrawTextDebug(Label, app->renderWidth, line++ * 10, 2, 2);
        );
        sprintf(Label, "%s (%04X %04X)", "Camera", (int)cameraX[0], (int)cameraY[0]);
        DrawTextDebug(Label, app->renderWidth, line++ * 10, 2, 2);

        sprintf(Label, "ParallaxCount %d", ParallaxCount);
        DrawTextDebug(Label, app->renderWidth, line++ * 10, 2, 2);

        int need;
        memcpy(&need, &testFloat2, sizeof testFloat2);

        sprintf(Label, "testFloat %f %p %X", testFloat, &testFloat, need);
        DrawTextDebug(Label, app->renderWidth, line++ * 10, 2, 2);

        sprintf(Label, "ringLifeRank %p", &ringLifeRank);
        DrawTextDebug(Label, app->renderWidth, line++ * 10, 2, 2);

        sprintf(Label, "%s", DebugText);
        DrawTextDebug(Label, app->renderWidth, app->renderHeight, 2, 2);

        /*for (int p = 0; p < ParallaxCount; p++) {
            sprintf(Label, "ParallaxSizes[%d] = %d", p, ParallaxSizes[p] & 0xFF);
            DrawTextDebug(Label, app->renderWidth, line++ * 10, 2, 2);
        }*/
    }

    if (levelCompleteTimer == 0) {
        levelCompleteTimer = -1;
        levelComplete = false;

        if ((act & 0xF) < 2) {
            doSwitch = true;
        }
        else if (ApparentAct == (int)(act & 0xF)) {
            if (TransitionAction == -1) {
                TransitionAction = 2;
                FadeOut = 20;
            }
        }
        else {
            // Essentially, do nothing.
            sceneIn = 120;
            ApparentAct = 2;
            ActualAct = 2;
            ActualZone = (act & 0xF0) / 0x10;
            rings = 0;
            frameTimer = 0;
            app->Stack->Clear();
            if ((act & 0xF) > 0)
                app->Stack->Push("Act Music", actMusics[(act & 0xF) - 1], 0, 0, 0, true, false, false, 1 / 44100.f);
            else
                app->Stack->Push("Act Music", actMusics[0], 0, 0, 0, true, false, false, 1 / 44100.f);
        }
    }

    if (MyPlayer->Action == ActionType::Dead && MyPlayer->Y - cameraY[app->CurrentViewport] > app->gameHeight + 224) {
        if (TransitionAction == -1) {
            TransitionAction = 0;
            FadeOut = 20;
        }
    }
    if (FadeOut > -1) {
        FadeOut--;
    }
    if (FadeOut == 0) {
        FadeOut = -1;
        if (TransitionAction == 0) {
            TransitionAction = -1;

            int tempAppAct = ApparentAct;

            bool NotApparentAct = false;
            if (checkpoint <= 2) {
                NotApparentAct = ApparentAct != (int)(act & 0xF);
                act = (act & 0xF0) + ApparentAct;

                if (NotApparentAct) {
                    Chunked = false;
                    RefreshRW();
                    LoadLayoutRW(ApparentAct);
                }
            }
            InitZone(NotApparentAct, checkpoint, -1, false);
            ApparentAct = tempAppAct;
        }
        else if (TransitionAction == 1) {
            TransitionAction = -1;

            bool NotApparentAct;
            app->realPaused = false;
            app->paused = false;
            checkpoint = 0;

            NotApparentAct = ApparentAct != (int)(act & 0xF);
            act = (act & 0xF0) + ApparentAct;

            if (NotApparentAct) {
                Chunked = false;
                RefreshRW();
                LoadLayoutRW(ApparentAct);
            }
            InitZone(NotApparentAct, checkpoint, -1, false);
        }
        else if (TransitionAction == 2) {
            TransitionAction = -1;
            doSwitch = true;
        }
    }

    DoSwitchActZone();
}

void LevelScene::PostMixProcess(void *udata, Uint8 *stream, int len) {
    short* p = (short*)stream;
    int sampleCount = len / sizeof(short);
    /////////////////////
    // Low pass filter //
    /////////////////////
    if (player->Underwater) {
        int oldValue = 0;
        float smoothing = 2.5f;
        for (int i = 0; i < sampleCount / 2; i++) {
            short finalValue = (short)(p[i * 2 + 0]);

            int newValue = finalValue;
            float filteredValue = oldValue + (newValue - oldValue) / smoothing;
            oldValue = filteredValue;
            p[i * 2 + 0] = filteredValue;
            p[i * 2 + 1] = filteredValue;
        }
    }
}

void LevelScene::Free() {
    if (Tileset != NULL)
        Tileset->Free();
    Tileset = NULL;
}

float LevelScene::getFullscreenOffset() {
    return 0;
}
