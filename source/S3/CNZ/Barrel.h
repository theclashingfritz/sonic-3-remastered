#ifndef S3_BARREL_H
#define S3_BARREL_H

#include <S3/Object.h>
#include <LevelScene.h>

class IBarrel : public Object {
	
	public:
		bool CanFall = false;
		bool CanBeCollected = false;
		bool controlOnStand = true;
        bool meFirst = true;
        bool prior = true;
		int Lifespan = -1;
		int NonCollect = -1;

		void Create();
		void Update();
		void Render(int CamX, int CamY);

		int OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
		
	private:
		int f = 0;
		int pR = 0;
		int pX = -1;
		int currentPlayerID = -1;
		int lastHitFrom = -1;
};

#endif /* S3_BARREL_H */