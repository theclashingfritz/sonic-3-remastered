#ifndef S3_BARREL_H
#define S3_BARREL_H

#include <S3/Object.h>
#include <LevelScene.h>

class IBarrel : public Object {
	
	public:
		bool CanFall = false;
		bool CanBeCollected = false;
		bool ControlOnStand = true;
		bool Solid = true;
		int Lifespan = -1;
		int NonCollect = -1;

		void Create();
		void Update();
		void Render(int CamX, int CamY);

		virtual int OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
		virtual int OnStandControlled(int);
		
	private:
		int f = 0;
		int pR = 0;
		int pX = -1;
		int $X = -1;
		int $Y = -1;
		int PlayerID = -1;
		int Data = -1;
};

#endif /* S3_BARREL_H */