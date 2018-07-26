#ifndef S3_RideVine_H
#define S3_RideVine_H

#include <S3/Object.h>
#include <Player.h>

class IRideVine : public Object {
	
	public:
		float StartX;
		float StartY;
		float BaseX;
		float BaseY;
		float Angle;
		float PotentialEnergy;

		void Create();
		void Update();
		void Render(int CamX, int CamY);

		int OnGrabbed(int PlayerID);
		int OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
    
	private:
		int PlayerID = -1;
		int t = 0;
};

class IZiplinePeg : public Object {
	
	public:
		void Create();
		void Render(int CamX, int CamY);
};

#endif /* S3_RideVine_H */
