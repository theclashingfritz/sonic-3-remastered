#ifndef S3_GiantHollowLog_H
#define S3_GiantHollowLog_H

#include <S3/Object.h>

class IGiantHollowLog : public Object {
	public:
		int FirstRun = -1;

		void Create();
		void Update();
		void Render(int CamX, int CamY);

		int  OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data);
	
	private:
		int PlayerID = -1;
};

#endif /* S3_GiantHollowLog_H */
