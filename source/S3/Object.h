#ifndef S3_OBJECT_H
#define S3_OBJECT_H

#include "../Application.h"

#include "../Engine/ITexture.h"
#include "../Engine/IGraphics.h"
#include "../Engine/IE.h"

#include "../S3/LevelScene.h"

class Object {
	public:
		IGraphics* G;
		SceneData* Data;
		Application* App;
		LevelScene* Scene;

		ISprite* Sprite = NULL;

		float X;
		float Y;
		float W;
		float H;
		float Speed;
		float Gravity;

		int ID;
		int SubType;
		bool Active;
		bool Priority;
		bool OnScreen = false;

		bool FlipX;
		bool FlipY;

		int Frame = 0;
		int Timer = -1;
		int Rotation = 0;

		bool Solid = false;
		bool SolidTop = false;
		bool SolidCustomized = false;

		bool Pushable = false;
		bool BreakableByRoll = false;
		bool BreakableByJump = false;
		bool BreakableByGlide = false;
		bool BreakableByKnuckles = false;
		bool CollidingWithPlayer = false;
		bool ControlOnStand = false;

		bool BounceOffShield = false;
		bool NegatedByFireShield = false;
		bool NegatedByElectricShield = false;
		bool NegatedByBubbleShield = false;

		virtual void Create() { };
		virtual void Update() { };
		virtual void Render(int CamX, int CamY) { };

		virtual int  OnTouchHitbox(int) { return 0; };

		virtual int  OnPush(int, int) { return 0; };
		virtual int  OnGrabbed(int) { return 0; };
		virtual int  OnBreakHorizontal(int, int) { return 0; };
		virtual int  OnBreakVertical(int, int) { return 0; };
		virtual int  OnCollisionWithPlayer(int, int, int) { return 0; };
		virtual int  OnStandControlled(int) { return 0; };
		virtual bool CustomSolidityCheck(int, int, int, bool) { return false; };
		virtual void OnLeaveScreen() { };
};

class Enemy : public Object {
	public:
		bool Harmful = true;
		bool Invincible = false;
		int  Radius = 1;
		int  HitCount = 1;

		void DoExplode();
		virtual int  OnCollisionWithPlayer(int, int, int);
};

#endif /* S3_OBJECT_H */