#ifndef S3_PLAYER_H
#define S3_PLAYER_H

class IPlayer;

#include "../Engine/ITexture.h"
#include "../Engine/IGraphics.h"
#include "../Engine/IE.h"

#include <LevelScene.h>
#include "Object.h"

#include "../TextureAudio.h"


/*
 * Player Enums
 */

// <editor-fold> Player Enums
enum class SpindashType {
    None,
    Standard,
    SCD,
};
enum class InvincibilityType {
    None,
    Temporary,
    Instant,
    Full,
};
enum class ShieldType {
    None,
    Basic,
    Fire,
    Electric,
    Bubble,
    Instashield,
    Invincibility,
};
enum class SuperFormAnimType {
    None,
    Transforming,
    Super,
    Deforming,
};
enum class SpeedCapType {
    None,
    S1,
    SCD,
};
enum class CharacterType {
    Sonic,
    Tails,
    Knuckles,
};
enum class ActionType {
    Normal,
    Hurt,
    Peril,
    Dead,
    Jumping,
    Fly,
    Rolling,
    Pipe,
    CrouchDown,
    Spindash,
    LookUp,
    Corkscrew,
    Spring,
    Twirl,

    Glide,
    GlideFall,
    GlideSlide,

    Climb,
    ClimbRise,

    Transform,
    Grab,
    GrabFall,
    Quicksand,
    Skid,
    Push,
    Slide,

    ObjectGrab,

    Fan,
    Conveyor,

    InStream,
    InStreamGrab,
    InStreamPipe,

    Respawn,
};
// </editor-fold> Player Enums

class IPlayer : public Object {
	public:
		ISprite* Sprite = NULL;
		ISprite* SpriteDashDust = NULL;
		ISprite* SpriteWaterRun = NULL;

		ISprite* SpriteShieldFire = NULL;
		ISprite* SpriteShieldElectric = NULL;
		ISprite* SpriteShieldBubble = NULL;
		ISprite* SpriteShieldInsta = NULL;

		ITexture* Palette1 = NULL;
		ITexture* Palette2 = NULL;

		LevelScene* Scene = NULL;

		/*
		 * Player Variables
		 */

		// <editor-fold> Player Variables
		ActionType Action = ActionType::Normal;

		CharacterType Character = CharacterType::Knuckles;

		bool HyperRings = false;

		bool ObjectCheck = true;
		Object* LastObject = NULL;
		Object* LastGrab = NULL;
		bool CheckJumpThrough = false;
		int PlayerID = 0;

		float Flip = 1;
		float DisplayX = 0;
		float DisplayY = 0;
		float CameraX = 0;
		float CameraY = 0;
		float DisplayFlip = 1;
		float DisplayAngle = 0;
		bool SlowRotationReset = false;
		float StoredRotation = 0;

		float Speed = 0.0f;
		float TopSpeed = 6.0f;
		float LastSpeed = 0.0f;
		float GroundSpeed = 0.0f;
		int SpeedMultiplier = 1;
		int SpeedSplitAmount = 4;
		float Acceleration = 0.046875f;
		float AccelRate = 0.046875f;
		float SkidRate = 0.5f;
		float RollingFriction = 0.046875f;
		float RollingFrictionValue = 0.046875f;

		float Gravity = 0.0f;
		float GravityMaxFull = 128.0f;
		float GravityRate = 0.21875f;
		float GravityQuicksandRate = 0.35f;

		int JumpVariable = 0;
		float JumpHeight = -6.5f;
		float JumpRelease = -4.0f;
		float JumpValue = -6.5f;
		float JumpReleaseValue = -4.0f;
		float JumpValueWater = -3.5f;
		float JumpReleaseValueWater = -2.0f;
		float JumpAnimSpeed = 1.0f;

		int SpindashMin = 8;
		int SpindashMax = 12;
		float SpindashRev = 0.0f;
		int SpindashRevAmount = 2;
		SpindashType Spindash = SpindashType::Standard;

		bool DropDashEnabled = true;
		int DropDashRev = 0;

		int InvincibilityTimer = 0;
		InvincibilityType Invincibility = InvincibilityType::None;

		int SpeedSneakersTimer = 0;
		bool SpeedSneakersActive = false;

		int ShieldAnimation = 0;
		bool ShieldUsable = true;
		bool ShieldAction = false;
		ShieldType Shield = ShieldType::None;

		int FlyFlag = 0;
		int FlyTimer = 480;
		int FlyTimerMax = 480;

		float GlideTurnSpeed = 0.0f;
		int GlideTurnCos = 0;
		bool GlideTurnFlag = false;

		int Angle = 0;
		int AngleMode = 0;
		int AngleModeLast = 0;
		int AngleLast = 0;
		int AngleLast2 = 0;
		bool Angle360Detection = false; // - If set to true angles will be detected smoother, but using a different method then Sonic 2.
		bool AngleBothSensors = false;  // - If set to true, both sensors must return a terrain object to check for angles.
		int AngleModeSwitchMode = 0;     // - If set to 0, it will follow a more accurate angle mode switch(Reccomended).
		int AngleSensorHeight = 90;
		int AngleSensorWidth = 6;
		int GroundSensorHeight = 35;
		int GroundSensorHeightMin = 29;
		int TerrainAngle = 0;

		bool Ground = false;
		bool Landed = false;
		bool WaterRunning = false;
		
		bool LimitRingRoss = true; // - If set to true, loss of Rings will be limited to 32 and rings won't be set to 0 and instead caculate the actual loss.

		int InputAlarm = 0;

		int Layer = 0;
		int VisualLayer = 0;
		int ForegroundLayer = 2;
		int EnemyCombo = 0;

		bool Underwater = false;
		int UnderwaterTimer = 1200;

		bool SuperForm = false;
		bool HyperForm = false;
		bool SuperFlight = false;
		bool SuperEnabled = true;
		int SuperFormTimer = 0;
		SuperFormAnimType   SuperFormAnim = SuperFormAnimType::None;
		int SuperFormAnimTimer = -1;
		int SuperFormAnimTimerMax = 20;

		bool ForceRoll = false;
		int ForceRollBond = -1;
		bool RollSpeedCap = true;
		bool RollJumpLock = false;
		SpeedCapType SpeedCap = SpeedCapType::None;

		int CollisionPushMode = 0; // - 0: Legacy, 1: Safe
		int PushPrecision = 2; // - If the number is lower, it will be more accurate, but use more CPU.

		int SlopeFactorMode = 0; // - 0: Sonic 1 & 2, 1: Sonic 3, 2: Sonic 3 and Knuckles?

		bool UseWorldsRotation = true; // - If set to true, the player will only rotate if angle is greater than 45 degrees(Only applies to 360 rotation).
		bool Use360SpriteRotation = true; // - If set to true characters will use 360 degree sprite rotations, false for 8 degree rotations.

		bool FlagsAllowXMovement = true;
		bool FlagsAllowYMovement = true;
		bool FlagsAllowObjectHandle = true;

		bool CameraLag = true;
		int CameraLockTimer = 0;
		int CameraLookTimer = 0;
		int SpringFlip = 0;

		float Frame = 0;
		int AnimationStart = 0;
		int AnimationEnd = 0;
		int AnimationSpeed = 0;
		bool AnimationStop = false;

		float Sin[360];
		float Cos[360];

		int ObjectControlled = 0;

		bool InputUp = false;
		bool InputDown = false;
		bool InputLeft = false;
		bool InputRight = false;
		bool InputJump = false;
		bool InputJumpHold = false;
		// </editor-fold> Player Variables

		// <editor-fold> Object-Use Variables
		int Data1 = 0;
		int Data2 = 0;
		int Data3 = 0;
		int Data4 = 0;
		int Data5 = 0;
		// </editor-fold> Object-Use Variables

		/*
		 * Basic functions
		 */

		// <editor-fold> Basic functions
		IPlayer();

		void Create();
		void Update();
		void LateUpdate();
		void Render(int CamX, int CamY);

		void AnimSet(int Start, int End, int Speed, bool Stop);
		void AnimExecute();

		void StepBegin();
		void Step();
		void StepEnd();

		void Vibrate(int controller, double strength, int milliseconds);
		// </editor-fold> Basic functions

		/*
		 * Math functions
		 */

		int AngleDifference(int a, int b);
		int SpecialModulo(int a, int b);

		/*
		 * Collision functions
		 */

		// <editor-fold> Collision functions
		bool (*CollisionAt)(int, int, int, int*, bool, Object**) = NULL;

		bool CollisionSensor360(int obj, int len, int A, int no360);
		bool CollisionASensor(int obj, int len);
		bool CollisionBSensor(int obj, int len);
		bool CollisionASensor360(int obj, int len);
		bool CollisionBSensor360(int obj, int len);
		bool CollisionMSensor360(int obj, int len);

		bool CollisionLeftSensor(int obj);
		bool CollisionRightSensor(int obj);

		bool CollisionCSensor(int obj, int len);
		bool CollisionDSensor(int obj, int len);

		bool CollisionSquish();

		bool CollisionCheckLine(float xr, float yr, float x2, float y2, int* angleOut);
		// </editor-fold> Collision functions

		/*
		 * Physics & Movement functions
		 */

		// <editor-fold> Physics & Movement functions
		int CheckAngle();
		void CheckGroundLevel();
		void BounceAwayFrom(float xx, float yy, float vel);
		float GetSlopeFactor();

		bool PushPlayerX_Id(int obj);
		bool PushPlayerX();
		void PushPlayerY();
		void PushPlayerYLand();
		void PushPlayerYDown();

		bool HandleSprings();
		void HandleEnemies();
		void HandleMonitors();
		void HandlePathSwitchers();
		bool HandleBreakableTerrain();

		void HandleObjects();
		void ActionObjectGrab();

		void WallCollision();
		void SpeedMovement();
		void GravityMovement();

		void HandleAction();
		void ActionPush();
		void ActionNormal();
		void ActionLookUp();
		void ActionRolling();
		void ActionSkid();
		void ActionJumping();
		void ActionCrouchDown();
		void ActionSpindash();
		void ActionSpring();
		void ActionSlide();
		void ActionGrab();
		void ActionFly();
		void ActionInStream();
		void ActionInStreamPipe();
		void ActionInStreamGrab();
		void ActionRespawn();
		void ActionGlide();
		void ActionGlideFall();
		void ActionGlideSlide();
		void ActionClimb();
		void ActionClimbRise();
		// </editor-fold> Physics & Movement functions

		/*
		 * Miscellaneous functions
		 */

		void Die();
		void Jump();
		void CreateRingLoss();
		void Hurt(int x, bool spike);
		void Deform();
};

#endif /* S3_PLAYER_H */
