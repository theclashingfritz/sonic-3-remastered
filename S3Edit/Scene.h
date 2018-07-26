#ifndef SCENE_H
#define SCENE_H

#include "Standard.h"

class Scene {
public:
	Application* app;

    // Global Audio
### SCENE_H_AUTOFILL ###

	void drawText(Texture* texture, const char* text, float x, float y, int xa, int ya, int type);
	virtual void Load() { }
	virtual void Update() { }
    virtual void Render() { }
    virtual void Free() { }
	void FreeAudio();
};

#endif
