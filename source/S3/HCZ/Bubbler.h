#ifndef S3_BUBBLER_H
#define S3_BUBBLER_H

#include <S3/Object.h>

class IAirBubble;

#include <vector>

class IBubbler : public Object {
	
	public:
		bool CanFall = false;
		bool CanBeCollected = false;
		int Lifespan = -1;
		int NonCollect = -1;
		
		void Create();
		void Update();
		void Render(int CamX, int CamY);
		void RemoveBubble(const IAirBubble *bubble);
		
	private:
		std::vector<IAirBubble *> bubbles;
		int f = 0;
};

#endif /* S3_BUBBLER_H */
