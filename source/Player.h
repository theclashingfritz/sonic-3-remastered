#ifndef PLAYER_H
#define PLAYER_H

#include "Application.h"
#include "Standard.h"
#include "LevelScene.h"
#include "S3Object.h"
#include "TextureAudio.h"

#include "S3/LevelScene.h"

class Player : public S3Object {
public:
    int animOverride = -1;
    int animOverrideFrame = -1;

    bool ControlLock = false;

    // <editor-fold> PLAYER VARIABLES

    float Acceleration = 0.046875f;
    float GravityRate = 0.21875f;
    float JumpHeight = -6.5f;
    float JumpRelease = -4.0f;
    float SkidRate = 0.5f;

    float RollingFriction = 0.046875f;

    float AccelRate = 0.046875f;
    float JumpValueWater = -3.5f;
    float JumpReleaseWater = -2.0f;
    float JumpValue = -6.5f;
    float JumpReleaseValue = -4.0f;

    int JumpVariable = 0;
    bool Delay = false;
    int Spindash = 1;
    int SpindashMin = 8; // - Minimum Spindash Speed .
    int SpindashMax = 12; // - Maximum Spindash Speed.
    float SpindashRev = 0.0f;
    int SpindashRevAmount = 2; // - How much Speed is added per button press.
    int Invincibility = 0;
    int InvincibilityTimer = 0;
    float InvAngle = 0.0f;
    bool SpeedSneakers = false;
    int SpeedSneakersTimer = 0;
    //int Shield = 0; // FEB
    bool ShieldUsable = true;
    bool ShieldAction = false;
    int ShieldAnimation = 0;

    int FlyTimer = 480;
    int MaxFlyTimer = 480;

    int FlyFlag = 0;
    int SuperFormTimer = 0;

    int Angle = 0;
    int AngleMode = 0; // - Floor Mode
    int AngleModeLast = 0;
    int AngleLast = 0;
    int AngleLast2 = 0;
    bool Ground = false;
    float QuicksandGravity = 0.35f; // - Quicksand Gimmick Gravity.
    //TerrainLeft = noone;
    //TerrainRight = noone;
    //TerrainId = noone;
    int InputAlarm = 0;
    int LeftEdge = 30;
    int RightEdge = 30;
    int Layer = 0;
    int EnemyChain = 0;
    bool Underwater = false;
    int UnderwaterTimer = 1200;
    bool SuperFlight = true;
    bool SuperForm = false;
    bool HyperForm = false;
    bool DisableSuper = false;
    bool Landed = false;
    int GrabType = 0;
    bool Deform = false;
    bool ForceRoll = false;
    bool FlagsAllowXMovement = true;
    bool FlagsAllowYMovement = true;
    bool FlagsAllowObjectHandle = true;
    bool Fan = false;
    bool AirDash = false;

    bool SKMonitors = false;
    bool RollJumpLock = false;
    bool RollMoveLock = false;
    bool Angle360Detection = false;  // - If set to true angles will be detected smoother, but using a different method then Sonic 2.
    bool AngleBothSensors = false;  // - If set to true, both sensors must return a terrain object to check for angles.
    int AngleModeSwitchMode = 1; // - If set to 0, it will follow a more accurate angle mode switch(Reccomended).
    int CollisionPushMode = 0; // - 0: Legacy, 1: Safe
    int PushPrecision = 2; // - If the number is lower, it will be more accurate, but use more CPU.

    int SlopeFactorMode = 1; // - 0: Sonic 1 & 2, 1: Sonic 3, 2: Sonic 3 and Knuckles?

    bool Use360SpriteRotation = true; // - If set to true characters will use 360 degree sprite rotations, false for 8 degree rotations.
    bool UseWorldsRotation = true; // - If set to true, the player will only rotate if angle is greater than 45 degrees(Only applies to 360 rotation).

    bool UseSuperForms = true; // - If set to true, characters with Super forms can transform.
    bool ShowShieldsInv = false; // - If set to true, elemental shields will be visible when super or invicible.
    bool UseShieldSuper  = true; // - If set to true, elemental shields are usable when Super.

    bool Sonic3RollReq = true; // - If set to true, the speed required for a roll will be the same as Sonic 3, otherwise the same as Sonic 1 & 2.

    bool TrailType = false; // - If set to true, Players will have a trail following them.

    bool UseHoming = false; // - If set to true, Sonic can perform a homing attack and air dash.
    bool UseCurlButton  = false; // - If set to true, Sonic can roll(curl) while in mid air.
    bool UsePeelout = false; // - If set to true, Sonic can perform the Super Peelout.
    bool UseWallJump = false; // - N/A

    bool FlagRollSpeedCap = true; // - If set to true speed is capped at 16 when rolling.

    int FlagSpeedCap = 0; // - 0 - none, 1 - Sonic 1, 2 - Sonic 2/CD,

    bool CameraLag = true;
    int CameraLock = 0;
    int SpringFlip = 0;

    int AngleSensorHeight = 90; // - Maximum distance the angle checking sensors can move.
    int AngleSensorWidth = 6;
    int GroundSensorHeight = 36; // - Maximum distance the ground checking sensors can move.
    int GroundSensorHeightMin = 29; // - Minimum distance the ground checking sensors can move.

    bool Sounds3D_Ring = false; // - Use 3D sounds for Rings(CPU intensive).
    bool RingStereoMix = false; // - If set to true the ring sound effect will play on both left and right channels, otherwise it'll switch between the two.

    bool HangingBar = false;

    int Rot3D = -1;
    int ObjectControlled = 0;
    int PlayerID = -1;

    float GroundSpeed = 0.0f;
    int SpeedMultiplier = 1;
    int SplitSpeedAmount  = 4;
    float JumpAnimSpeed = 1.0f;
    float TopSpeed = 6.0f;
    int LookTimer = 0;
    float TurnSpeed = 0.0f;
    int FloorAngle = 0;
    Terrain TerrainId { false };
    Terrain TerrainIdLast { false };

    bool DropDashEnabled = true;
    int  DropDashRev = 0;

    float Sin[360];
    float Cos[360];
    int AnglePos[4][2];

    float CurrentPalette = 0.0f;

    bool OverrideRotation = false;

    float dispX = 0.0f;
    float dispY = 0.0f;
    float dispAng = 0.0f;
    float dispFlip = 1.0f;
    int offY = 0;
    // </editor-fold>

    int AnimationOffset = 0;

    Texture* tex = NULL;
    Texture* tex_Super = NULL;
    Texture* tex_Tails = NULL;
    Texture* tex_Insta = NULL;
    Texture* tex_Fire = NULL;
    Texture* tex_Lightning = NULL;
    Texture* tex_Bubble = NULL;
    Texture* tex_Invinc = NULL;

    Texture* tex_DashDust = NULL;
    Texture* tex_SkidDust = NULL;
    Texture* tex_WaterRun = NULL;

    // DO NOT FREE
    Texture* tex_BubbleFX;

    LevelScene* sc;

    float camShiftX = 0.0f;
    float camShiftY = 0.0f;

    int formingTimer = 8 * 10;
    int formingType = 0;

    float GrabY = -1;
    float LastGrabY = -1;
    int GrabTimer = -1;
    int ConveyorSpeed = 0;

    bool WaterRun = false;

    int enemyCombo = 0;

    struct TrailData {
        float x;
        float y;
        float angle;
        float flip;
        int frame;
    };
    TrailData Trail[60];
    int       TrailIndex = 0;
    bool      TrailSameFrame = true;

    // <editor-fold> OTHER VARIABLES
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool jump = false;
    bool jumpHold = false;
    int waitClock = 0;
    bool Controllable = true;

    float sh = 0;
    int startAu = 150;
    int endAu = 155;
    int speedAu = 1;
    bool stopAu = false;

    int fcount = 0;
    float frm = 0;
    int fcountOffset = 0;
    int overrideFrame = -1;

    bool loop = false;

    int CharacterId = 0;
    const int consCharacterSonic = 0;
    const int consCharacterTails = 1;
    const int consCharacterKnuckles = 2;
    const int consCharacterMetalSonic = 3;

    int objParentTerrain = 0;
    bool objChek = true;
    // </editor-fold>

    int RespawnTimer = 0;

    int bubbleShieldAnim[55] = {
        1, 0, 9, 0, 9, 0, 9, 1, 0xA, 1, 0xA, 1, 0xA, 2, 9, 2, 9, 2, 9, 3,
        0xA, 3, 0xA, 3, 0xA, 4, 9, 4, 9, 4, 9, 5, 0xA, 5, 0xA, 5, 0xA, 6, 9, 6,
        9,   6,   9,   7,  0xA,   7,  0xA,   7,  0xA,   8,   9,   8,   9,   8,   9,
    };
    int electricShieldAnim1[43] = {
        1,   0,   0,   1,   1,   2,   2,   3,   3,   4,   4,   5,   5,   6,   6,   7,   7,   8,   8,   9,
		0xA,  0xB, 0x16, 0x16, 0x15, 0x15, 0x14, 0x14, 0x13, 0x13, 0x12, 0x12,
		0x11, 0x11,	0x10, 0x10,  0xF,	0xF,  0xE,  0xE,	9,  0xA,	 0xB
    };

    bool DebugMode = true;

    Player();

    void animExecute();

    void anim(int start, int end, int speed, bool stop);
    void create();

    int mod(int a, int n);
    int angle_difference(int a, int b);

    void bounceAwayFrom(float xx, float yy, float vel);

    void beginStep();
    int scrCheckAngle();
    int scrWrapAngle(int angle);
    float scrWrapAngleRad(float angle);
    float scrPlayerGetSlopeFactor();

    Terrain collision_line_terrain(float xr, float yr, float x2, float y2);

    // <editor-fold> COLLISION SENSORS
    bool scrPlayerCollisionSensor360(int obj, int len, int A, int no360);
    bool scrPlayerCollisionASensor(int obj, int len);
    bool scrPlayerCollisionBSensor(int obj, int len);
    bool scrPlayerCollisionASensor360(int obj, int len);
    bool scrPlayerCollisionBSensor360(int obj, int len);
    bool scrPlayerCollisionMSensor360(int obj, int len);

    bool scrPlayerCollisionLeftSensor(int obj);
    bool scrPlayerCollisionRightSensor(int obj);

    bool scrPlayerCollisionCSensor(int obj, int len);
    bool scrPlayerCollisionDSensor(int obj, int len);

    bool scrPlayerCollisionSquish();
    // </editor-fold>

    void scrPushPlayerX_Id(Terrain obj);
    void scrPushPlayerX();
    void scrPushPlayerY();
    void scrPushPlayerYLand();
    void scrPushPlayerYDown();

    void scrCheckGroundLevel();

    bool scrPlayerHandleBreakableTerrain();
    void scrPlayerHandleMonitors();

    void scrPlayerHandlePathSwitchers();

    void scrPlayerHandleObjects();
    void scrPlayerActionObjectGrab();

    void scrPlayerWallCollision();
    void scrPlayerSpeedMovement();
    void scrPlayerGravityMovement();

    void scrCreateRingLoss();
    void scrPlayerJump();
    void scrPlayerHurt(int x, bool spike);
    void scrPlayerDead();

    void scrPlayerActionPush();
    void scrPlayerActionNormal();
    void scrPlayerActionRolling();
    void scrPlayerActionSkid();
    void scrPlayerActionJumping();
    void scrPlayerActionCrouchDown();
    void scrPlayerActionSpindash();
    void scrPlayerActionSpring();
    void scrPlayerActionSlide();

    void scrPlayerActionGrab();
    void scrPlayerActionFly();

    void scrPlayerActionInStream();
    void scrPlayerActionInStreamPipe();
    void scrPlayerActionInStreamGrab();

    void scrPlayerActionRespawn();

    void handleAction();

    void step();
    void endStep();

    void update();
    void render(float camX, float camY);
    void doThing();
    void free();

    void Vibrate(int controller, double strength, int milliseconds);
};

#endif
