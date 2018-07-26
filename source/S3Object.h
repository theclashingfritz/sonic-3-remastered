#ifndef S3OBJECT_H
#define S3OBJECT_H

struct Actions {
    enum Type {
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
        Glide,
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
};

class S3Object {
public:
    struct Part { bool Active; float X; float Y; float Speed; float Gravity; int Frame; };
    Part parts[8];

	Application* app;
	LevelScene* sc;
    S3Object* player;

    bool OnScreen = false;

	float Speed = 0;
    float Gravity = 0;
    float GravityMaxi = 16.0f;
    float flip = 1.0f;
	int Shield = 0;
    Actions::Type Action = Actions::Normal;

    float $X = -1;
    float $Y = -1;

	float x = -1;
	float y = -1;
	float w = 1;
	float h = 1;
    float r = -1;
	float frame = 0.0;
	unsigned char data1 = 0x00;
	unsigned char data2 = 0x00;
	int data3 = 0x00;
	bool flipX = false;
	bool flipY = false;
	bool prior = false;

    float XPrevious = -1;
    float YPrevious = -1;

	bool Pushable = false;
    bool Grabbable = false;
	bool poppable = false;
	bool damaging = false;
	bool enemy = false;
	bool boss = false;
    bool breakable = false;
    bool breakableVert = false;
    bool Collidable = false;

    bool active = true;
	bool solid = false;
	bool solidTop = false;
    bool controlOnStand = false;
    bool controlOnTouch = false;

	bool overAll = false;
	bool overObj = false;
	bool underAll = false;
	bool meFirst = false;

	float standSpdX = 0;
	float standSpdY = 0;
    int frameExplode = -1;

    int index = -1;
	int Layer = 0;

	struct Terrain { bool Active; int Layer; bool Platform; int bbox_left; int bbox_right; int bbox_top; int bbox_bottom; int angle; unsigned int ObjectID; };

    virtual void update() { };
    virtual void render(float cameraX, float cameraY) { };
    virtual void doThing() { };
    virtual int doThing(int data) { return 0; };
    virtual int doThing2() { return 0; };

    virtual int  OnGrabbed(int data) { return 0; };
    virtual int  OnBreakHorizontal(int data) { return 0; };
    virtual int  OnBreakVertical(int data) { return 0; };
    virtual void OnLeaveScreen() { };
    virtual int  OnCollisionWithPlayer(int data1, int data2, int data3) { return 0; };

    virtual void create() { };
    void anim(float startAu, float endAu, float speedAu, bool stopAu) {
		if (speedAu > 0) {
            if (this->frame < startAu)
                this->frame = startAu;
            if (this->frame > endAu)
                this->frame = startAu;
        }
        else {
            if (this->frame > startAu)
                this->frame = startAu;
            if (this->frame < endAu)
                this->frame = startAu;
        }
        if (stopAu) {
            if (speedAu > 0)
                this->frame = std::min(this->frame + speedAu, endAu);
            else
                this->frame = std::max(this->frame + speedAu, endAu);
        }
        else {
            this->frame = fmod(this->frame - startAu + speedAu, endAu - startAu) + startAu;
        }
    }
	Terrain collision_line(float xr, float yr, float x2, float y2);
};

#endif
