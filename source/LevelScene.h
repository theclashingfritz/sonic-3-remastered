#ifndef LEVELSCENE_H
#define LEVELSCENE_H

#include "Standard.h"
#include "Resources.h"
#include "TextureAudio.h"

#include "Engine/IO/IStream.h"
#include "Engine/IGraphics.h"

#include "S3/Object.h"
#include "S3/Player.h"
#include "S3/Objects/Ring.h"
#include "S3/Objects/Button.h"
#include "S3/Objects/Points.h"
#include "S3/Objects/Spikes.h"
#include "S3/Objects/Spring.h"
#include "S3/Objects/Monitor.h"
#include "S3/Objects/Starpole.h"
#include "S3/Objects/EggCapsule.h"
#include "S3/Objects/RollEnforcer.h"
#include "S3/Objects/PathSwitcher.h"
#include "S3/Objects/BreakableWall.h"
#include "S3/Objects/AnimatedStill.h"
#include "S3/Objects/InvisibleSolid.h"
#include "S3/Objects/NonAnimatedStill.h"
#include "S3/Objects/SpecialStageRing.h"

#include "S3/AIZ/Cork.h"
#include "S3/AIZ/Rock.h"
#include "S3/AIZ/Ridge.h"
#include "S3/AIZ/RideVine.h"
#include "S3/AIZ/SwingVine.h"
#include "S3/AIZ/TreeBark.h"
#include "S3/AIZ/LogBridge.h"
#include "S3/AIZ/SpikedLog.h"
#include "S3/AIZ/FallingLog.h"
#include "S3/AIZ/GiantHollowLog.h"
#include "S3/AIZ/ForegroundPlant.h"
#include "S3/AIZ/FloatingPlatform.h"
#include "S3/AIZ/CollapsingPlatform.h"
#include "S3/AIZ/DisappearingPlatform.h"

#include "S3/AIZ/RhinoBot.h"
#include "S3/AIZ/MonkeyDude.h"
#include "S3/AIZ/Bloominator.h"

#include "S3/AIZ/Bosses/AIZBoss.h"
#include "S3/AIZ/Bosses/AIZMinibossCutscene.h"

#include "S3/HCZ/Bubbler.h"

#include "S3/CNZ/Barrel.h"

#define GetElapsed(nextTick) (SDL_GetTicks() - nextTick) / 1000.0f

class Particle {
public:
    Texture *tex;
    float x;
    float y;
    int max_life;
    int life;
    int slow;
    bool Active;
    float vel_x;
    float vel_y;
    int maxFrame;
    bool underwater;
    Particle(Texture *tex, float x, float y, int max_life, int life, int slow, bool Active);
    Particle(Texture *tex, float x, float y, int max_life, int life, int slow, bool Active, float vel_x, float vel_y, int maxFrame, bool underwater);
    void Render(Application *app, float camX, float camY);
};

typedef struct {
    LevelScene* scene;
    int act;
    bool layout;
    char str[10];
    bool type;
} ThreadLoader;

struct TileCFG {
    unsigned char Collision1[16];
    unsigned char Collision2[16];
    unsigned char Angle1;
    unsigned char Angle2;
};

struct ChunkVertex {
    // Position Information
    GLfloat X;
    GLfloat Y;
    GLfloat Z;
    // Palette Information
    GLfloat PaletteShift;
    // Texture Information
    GLfloat U;
    GLfloat V;
};

class ChunkBO {
public:
    GLuint Buffer;
    ChunkVertex VerticesExtra[1536];
    int AnimTiles[256];
    int AnimTilesCount = 0;
};

class LevelScene : public Scene {
public:
    struct ParallaxLayer { int Height; float Per; };
    struct Ring { int X; int Y; bool Active; int Time; };
    struct Damage { int X; int Y; bool Active; int Radius; };

    ChunkBO ChunkBufferObjects[256];

    IPlayer *MyPlayer = NULL;
    IPlayer *GetPlayer(int ID);
    int PlayerCount = 1;

    Object *Objects[10000];
    int ObjectCount = 0;

    Object *ObjectsSolid[1000];
    int ObjectSolidCount = 0;

    Object *ObjectsSpring[1000];
    int ObjectSpringCount = 0;

    Object *ObjectsEnemies[1000];
    int ObjectEnemiesCount = 0;

    Object *ObjectsBreakable[1000];
    int ObjectBreakableCount = 0;

    Object *ObjectsPathSwitcher[1000];
    int ObjectPathSwitcherCount = 0;

    Object *Particles[50];
    int ParticleCount = 0;

    bool ManiacMode = false;

    bool Chunked = false;

    int LevelWidth = 0;
    int LevelHeight = 0;
    unsigned short Level[0xF00000];
    ITexture *LevelTiles = NULL;

    int ApparentAct = 1;
    int ActualAct = 1;
    int ActualZone = 1;
    bool JustInstantDrawAnimTiles = true;

    int ButtonCount = 0;

    TileCFG Tile[0x1000];

    int FadeOut = -1;
    int TransitionAction = -1;
    bool WaterFlash = false;

    bool CollisionAt(int, int, int layer, int *angle, bool objectCheck, Object **out, bool checkJumpThrough);

    ISprite *spr_HUD_Act = NULL;
    ISprite *spr_HUD_S3K = NULL;
    ISprite *spr_HUD_ZoneNameFont = NULL;

    char levelTextLine1[32];
    const char* levelTextLine2 = "ZONE";

    ISprite* buttons = NULL;

    // Level Special
    char listGroupsPerAct[0x80];
    bool inputBuffer[6 * 20];

    unsigned char level[0xFFFF];
    unsigned char heights[0xFFFF];
    unsigned char colinds[0xFFFF];
    unsigned char angles[0xFFFF];
    unsigned char chunks[0xFFFF];
    unsigned char objs[0xFFFF];
    unsigned char tiles[0x30000];
    Ring ringArr[1024];

    unsigned char levelBuffer[0xFFFF];
    unsigned char chunksBuffer[0xFFFF];
    unsigned char tilesBuffer[0x30000];
    Ring ringArrBuffer[1024];

    int CameraLanding[4];
    int CameraFastHScroll[4];
    int CameraFastVScroll[4];
    float cameraX[4];
    float cameraY[4];
    float cameraMinX = 0.0f;
    float cameraMinY = 0.0f;
    float cameraMaxX = 0.0f;
    float cameraMaxY = 0.0f;
    int cameraAutoScrollX = 0;
    int cameraAutoScrollY = 0;
    bool actTransition = false;
    int LevelRoutine = 0;
	
    IPlayer *cameraPlayer = NULL;

    IPlayer *player = NULL;
    IPlayer *playerAI = NULL;
    std::array<IPlayer *, 16> playerBuffer;
    int player_count = 0;

    bool levelComplete = false;
    int levelCompleteTimer = -1;
    int maxLevelCompleteTimer = 5 * 60;
    int awardedScore = 0;
    int resultsSoundTick = 0;
    bool resultsRingSoundPlayed = false;

    Texture *tex_objects[256];

    ITexture *Tileset = NULL;

    unsigned int act = 0;
    unsigned int rings = 0;
    unsigned int ringLifeRank = 100;
    float testFloat = 1.0f;
    float testFloat2 = 2.0f;
    unsigned int score = 0;
    unsigned int lives = 5;
    int checkpoint = 0;
    unsigned int waterLevel = 0xFF * 128;
    unsigned int objects_count = 0;
    unsigned int pathswitchers_count = 0;
    unsigned int part_next = 0;
    unsigned int part_max = 96;
    unsigned int ringTotal = 0;
    unsigned int frameTimer = 0;

    S3Object *objects[512];
    S3Object *pathswitchers[64];
    Particle *particles[96];
    Damage damages[64];
    unsigned int damages_count = 0;

    bool doSwitch = false;

    bool firstUpdateCheck = false;
    bool firstRenderCheck = false;

    int animChunks[256];

    unsigned char levelW = 0;
    unsigned char levelH = 0;
    unsigned char levelSubW = 0;
    unsigned char levelSubH = 0;

    float Sin[360];
    float Cos[360];
    bool loop = false;

    int frame = 0;
    int frameAnim = 0;
    int sceneIn = 180;
    int sceneOut = -2;
    bool cameraLock = false;

    double startPosX = 8.5 * 128;
    double startPosY = 8.5 * 128;

    ParallaxLayer layers[32];
    int layerCount = 0;

    // Animated Tile Information (Act 1)
    int animCount = 46;
    int animList[50];
    int animType[50];
    Texture *animTex[50];
    Texture *anim2Tex[50];
    int AnimTiles[80];
    int AnimTilesCount = 0;

    bool paused = false;
    int pauseTimer = 0;

    SDL_Thread *threadID;
    SDL_Thread *threadID2;

    Audio *mainTrack = NULL;
    Audio *sideTrack = NULL;

    Texture *paletteDry = NULL;
    Texture *paletteWet = NULL;
    Texture *paletteBossDry = NULL;
    Texture *paletteBossWet = NULL;

    Texture *tex_AirCountdown = NULL;
    Texture *tex_Results_GotThrough = NULL;
    Texture *tex_Results_BonuTotal = NULL;
    Texture *tex_Results_Player = NULL;

    Texture *tex_Effect_BigWave = NULL;
    Texture *tex_Effect_DashDust = NULL;
    Texture *tex_Effect_EnemyExplosion = NULL;
    Texture *tex_Effect_Explosion = NULL;
    Texture *tex_Effect_SkidDust = NULL;
    Texture *tex_Effect_WaterRun = NULL;
    Texture *tex_Effect_WaterSplash = NULL;
    Texture *tex_EnemyPoints = NULL;
    Texture *tex_GrayButton = NULL;
    Texture *tex_Font = NULL;
    Texture *tex_HUD_Act = NULL;
    Texture *tex_HUD_S3K = NULL;
    Texture *tex_HUD_STR = NULL;
    Texture *tex_HUD_ZoneNameFont = NULL;
    Texture *tex_ZoneNameFont = NULL;
    Texture *tex_HUD_Buttons = NULL;
    Texture *tex_HUD_SmallNumbers = NULL;
    Texture *tex_Lives_Sonic = NULL;
    Texture *tex_Lives_Tails = NULL;
    Texture *tex_Lives_Knuckles = NULL;
    Texture *tex_Monitor = NULL;
    Texture *tex_Ring = NULL;
    Texture *tex_Side = NULL;
    Texture *tex_Shield_Bubble = NULL;
    Texture *tex_Shield_Fire = NULL;
    Texture *tex_Shield_Insta = NULL;
    Texture *tex_Shield_Invincibility = NULL;
    Texture *tex_Shield_Lightning = NULL;
    Texture *tex_Signpost = NULL;
    Texture *tex_SignpostKnux = NULL;
    Texture *tex_SignpostStub = NULL;
    Texture *tex_SpecialRing = NULL;
    Texture *tex_Spikes = NULL;
    Texture *tex_SpringRed = NULL;
    Texture *tex_SpringYellow = NULL;
    Texture *tex_Starpost = NULL;

    Texture *tex_ChunksBack = NULL;
    Texture *tex_ChunksFront = NULL;
    Texture *tex_Bubble = NULL;
    Texture *tex_Waves = NULL;

    Texture *tex_Boss = NULL;
    Texture *tex_BossEffects = NULL;
    Texture *tex_BossExtra = NULL;
    Texture *tex_Miniboss = NULL;
    Texture *tex_MinibossEffects = NULL;
    Texture *tex_MinibossExtra = NULL;
    Texture *tex_NonAnim_ = NULL;

    Texture *tex_Sonic = NULL;
    Texture *tex_SonicSuper = NULL;
    Texture *tex_Tails = NULL;
    Texture *tex_Tails_Tails = NULL;
    Texture *tex_Knuckles = NULL;

    SDL_RWops *loadingSpace[512];
    int loadingSpace_count = 0;

    int visualWaterHeight = 0xD;
    int backHorizonChunkHeight = 4;
    int frontHorizonChunkHeight = 13;

    int BGstart = 0;
    int BGsize = 4;
    int BGxoff = 0;
    int BGyoff = 0;
    int BGxCamOff = 0;
    int BGyCamOff = 0;
    bool BGcameraLock = false;
    float BGx = 7.f / 16.f;
    float BGy = 13.f / 128.f;

    bool DebugMode = false;
    char DebugText[100];
	
	bool WantHUD = true;

    SDL_RWops *RWList[40];

    Audio *actMusics[2];
    int sonStartPos[2];
    int knuStartPos[2];

    int ParallaxSizes[32];
    float ParallaxMult[32];
    float ParallaxAutoScroll[32];
    int ParallaxCount = 0;

    int CurrentTileset = 1;
    bool CurrentTilesetAnimated = true;

    int PalCycle_SuperSonic[30] = {
        0xE66, 0xC42, 0x822,
        0xE88, 0xC66, 0x844,
        0xEAA, 0xC88, 0xA66,
        0xECC, 0xEAA, 0xC88,
        0xEEE, 0xECC, 0xEAA,
        0xEEE, 0xEEE, 0xEEE,
        0xCEE, 0xCEE, 0xAEE,
        0xAEE, 0x8EE, 0x6CC,
        0x8EE, 0x0EE, 0x0AA,
        0xAEE, 0x8EE, 0x6CC,
    };
    int PalCycle_SuperSonicUnderwaterAIZICZ[30] = {
        0xA82, 0x860, 0x640,
        0xE88, 0xC66, 0x844,
        0xEAA, 0xC88, 0xA66,
        0xECC, 0xEAA, 0xC88,
        0xEEE, 0xECC, 0xEAA,
        0xEEE, 0xEEE, 0xEEE,
        0xCEE, 0xCEC, 0xAEC,
        0xAEC, 0x8EC, 0x6CA,
        0x8EC, 0x4EA, 0x4A8,
        0xCEE, 0xCEC, 0xAEC,
    };
    int PalCycle_SuperSonicUnderwaterHCZCNZLBZ[30] = {
        0xC66, 0xA44, 0x624,
        0xE88, 0xC66, 0x844,
        0xEAA, 0xC88, 0xA66,
        0xECC, 0xEAA, 0xC88,
        0xEEE, 0xECC, 0xEAA,
        0xEEE, 0xEEE, 0xEEE,
        0xCEE, 0xCEC, 0xAEC,
        0xAEC, 0x8EC, 0x6CA,
        0x8EC, 0x4EA, 0x4A8,
        0xCEE, 0xCEC, 0xAEC,
    };

    int pauseMenuItemSelected = 2;
    int pauseMenuFrameCounter = 0;

    bool ShowHitboxes = false;

    void CreateChunkBOs();
    void GenerateChunkBO(int us);

    void Init(Application *app, int act, int checkpoint);

    void loadCommonTextures(Application *app);
    void unloadCommonTextures(Application *app);

    void getRing(int r);
    void getLife(int l);
    void givePoint(int type, int enemyCombo, int X, int Y);

    void RestorePalette();

    void beginActZoneTransfer(int act_n);
    int loadLayout(void *data);
    int loadAnims(void *data);

    static int staticLoad(void* data);

    float getFullscreenOffset();
    void renderHUD();
    void renderPauseScreen();
    void renderTransition();
    void renderResultsScreen();

    bool LoadThreadDone = false;

    void PostMixProcess(void *udata, Uint8 *stream, int len);

    void LoadLayoutRW(int actt);
    virtual void DoSwitchActZone();

    int objs_length = 0;
    virtual void LoadPalette();
    virtual void createParallaxLayers() { }
    void LoadObjectData();
    virtual void createObjects(int check, int specialRing, bool actTransition);
    virtual void InitZone(bool resetTextures, int check, int specialRing, bool actTransition);
    virtual void RefreshRW() { }

    virtual void loadUniqueTextures(Application *app, int act) { }
    virtual void CreateUniqueTextures(Application *app, int act) { }
    virtual void UnloadUniqueTextures(Application *app, int act) { }

    void LoadMusicAndSounds(Application *app, int act);
    void createAnimatedChunks(Application *app, int act);
    void unloadAnimatedChunks(Application *app);

    virtual void HandleCamera();
    virtual void BackgroundParallaxRender() { }
    virtual void createBackgroundTBOs() { }
    virtual void DoPaletteWaterStuff();
    virtual void DoBackgroundStuff();
    virtual void DrawPrettyMuchEverything();
    virtual void DrawAboveEverythingNonHUD();

    void DrawTileLine(int x, int y, int line, int top, int size);
    virtual int CustomAnimTile(int ind, int i, int parRow) { return ind; }
    virtual void DrawTile(int ind, float x, float y, float w, float h, float p, int top, int size);
    virtual void DrawTile(int ind, float x, float y, float w, float h, float p, int top, int size, int parRow);
    virtual void DrawChunk(int x, int y, int us, int offX, int offY, int maxTilesX, int maxTilesY, float z);
    virtual void DrawChunk(int x, int y, int us, int offX, int offY, int startX, int startY, int maxTilesX, int maxTilesY, float z);

    virtual void Update();
    virtual void Render();
    virtual void Free();
};

#endif
