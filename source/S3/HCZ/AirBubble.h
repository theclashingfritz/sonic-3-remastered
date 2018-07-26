#ifndef S3_AIR_BUBBLE_H
#define S3_AIR_BUBBLE_H

#include <S3/Object.h>

class IBubbler;

class IAirBubble : public Object {
	
	public:
		bool CanFall = false;
		bool CanBeCollected = false;
		int Lifespan = -1;
		int NonCollect = -1;
		
		void Create();
		void Update();
		void Render(int CamX, int CamY);
		
		virtual void SetBubbler(IBubbler *bubbler);
		
		virtual void OnLeaveScreen();
		
	private:
		bool willGrow = false;
		
		IBubbler *hostBubbler;
};
#endif /* S3_AIR_BUBBLE_H */