#include "Standard.h"
#include "Player.h"
#include "S3Object.h"

#define VIBRATE_FRICTION 0.25, 150
#define VIBRATE_IMPACT 0.25, 250
#define VIBRATE_HURT 0.75, 450

Player::Player() {
    this->w = 20;
    this->h = 40;
    for (int i = 0; i < 360; i++) {
        this->Sin[i] = sin(i * Math_PI / 180.0f);
        this->Cos[i] = cos(i * Math_PI / 180.0f);
    }
}

void Player::animExecute() {
    int animFrameCount = endAu - startAu + 1;
    // reset if animation change
    if (frm < 0 || frm >= animFrameCount) {
        fcountOffset = fcount;
    }


    if (frm < 0) {
        frm = 0;
    }
    if (frm >= animFrameCount) {
        if (stopAu) {
            frm = 0;
        }
        else {
            frm = animFrameCount - 1;
        }
    }

    if (stopAu) {
        if (speedAu > 0)
            frm = std::min(frm + 1.f / speedAu, (float)animFrameCount - 1);
        else
            frm = 0;//std::max(frm + speedAu, 0);
    }
    else {
        if (speedAu != 0)
            frm = fmod(frm + 1.f / speedAu + animFrameCount, animFrameCount);
        else
            frm = 0;
    }
}

void Player::anim(int start, int end, int speed, bool stop) {
    endAu = end;
    stopAu = stop;
    startAu = start;
    speedAu = speed;
}
void Player::create() {
    this->AnglePos[0][0] = this->x-7;
    this->AnglePos[0][1] = this->y+20;

    this->AnglePos[1][0] = this->x+7;
    this->AnglePos[1][1] = this->y+20;

    this->AnglePos[2][0] = this->x-9;
    this->AnglePos[2][1] = this->y+20;

    this->AnglePos[3][0] = this->x+9;
    this->AnglePos[3][1] = this->y+20;

    sc->cameraX[app->CurrentViewport] = this->x - app->renderWidth / 2.0f;
    sc->cameraY[app->CurrentViewport] = this->y - app->renderHeight / 2.0f;

    dispX = this->x;
    dispY = this->y;

    data3 = CharacterId;
}

int Player::mod(int a, int n) {
    return a - (a / n) * n;
}
int Player::angle_difference(int a, int b) {
    int j = b - a;
    return mod(j + 180, 360) - 180;
}

int Player::scrCheckAngle() {
    if (this->TerrainId.Active) {
        return scrWrapAngle(this->TerrainId.angle);
    }
    return this->Angle;
}
int Player::scrWrapAngle(int angle) {
    while (angle < 0.0) angle += 360;
    while (angle >= 360) angle -= 360;
    return angle;
}
float Player::scrWrapAngleRad(float angle) {
    while (angle < 0.0f) angle += Math_PI * 2.0f;
    while (angle >= Math_PI * 2.0f) angle -= Math_PI * 2.0f;
    return angle;
}
float Player::scrPlayerGetSlopeFactor() {
    if ((this->Angle < 45 || this->Angle > 315) && std::round(this->Speed) == 0 && this->SlopeFactorMode == 0)
        return 0;

    if ((this->Angle < 22 || this->Angle > 315) && std::round(this->Speed) == 0 && this->SlopeFactorMode == 1)
        return 0;

    if ((this->Angle < 22 || this->Angle > 337) && std::round(this->Speed) == 0 && this->SlopeFactorMode == 2)
        return 0;


    return 0.125f * this->Sin[this->Angle];
}

S3Object::Terrain Player::collision_line_terrain(float xr, float yr, float x2, float y2) {
    float X2 = std::round(xr);
    float Y2 = std::round(yr);
    float X3 = std::round(x2);
    float Y3 = std::round(y2);
    if (loop)
        Y2 = fmod((Y2 + sc->levelH * 128), (sc->levelH * 128));

    float angle = 0;
    float len = 0;

    len = std::sqrt((Y2 - Y3) * (Y2 - Y3) + (X3 - X2) * (X3 - X2));

    if (len > 0) {
        angle = std::atan2(Y2 - Y3, X3 - X2);
    }
    else
        angle = 0;

    angle = scrWrapAngleRad(angle);

    for (int n = 0; n <= len; n++) {
        if (Controllable) {
            if (X2 < sc->cameraX[app->CurrentViewport]) {
                //return S3Object::Terrain { true, this->Layer, false, 0, 16, (int)X2, (int)Y2, 270 };
            }
            if (X2 >= sc->levelW * 128) {
                return S3Object::Terrain { true, this->Layer, false, 0, 16, (int)X2, (int)Y2, 270 };
            }
        }

        if (Ground && std::abs(Speed) > 7 && Angle == 0 && Y2 >= sc->waterLevel && !Underwater && y < sc->waterLevel && (sc->act & 0xF0) == 0x20) {
            WaterRun = true;
            return S3Object::Terrain { true, -1, true, 0, 0, 0, 0, 0 };
        }

        for (unsigned int o = 0; o < sc->objects_count && objChek; o++) {
            if (sc->objects[o] != NULL) {
                bool noException = true;
                if (sc->objects[o]->data1 == 0x69 && (sc->objects[o]->data2 & 0xF0) == 0x00 && (sc->objects[o]->data2 & 0x0F) > 0x00) {
                    noException = false;
                }

                /*if (!(sc->objects[o]->x - sc->cameraX[app->CurrentViewport] >= 0 &&
                      sc->objects[o]->y - sc->cameraY[app->CurrentViewport] >= 0 &&
                      sc->objects[o]->x - sc->cameraX[app->CurrentViewport] < app->renderWidth &&
                      sc->objects[o]->y - sc->cameraY[app->CurrentViewport] < app->gameHeight) && noException) continue;*/

                if (X2 >= sc->objects[o]->x - sc->objects[o]->w / 2.f &&
                    Y2 >= sc->objects[o]->y - sc->objects[o]->h / 2.f &&
                    X2 < sc->objects[o]->x + sc->objects[o]->w / 2.f &&
                    Y2 < sc->objects[o]->y + sc->objects[o]->h / 2.f && sc->objects[o]->active && noException) {
                    if (sc->objects[o]->data1 == 0x01) {
                        if ((this->Action == Actions::Jumping && this->Gravity > 0 && !this->Ground) || this->Action == Actions::Rolling || sc->objects[o]->data2 == 0x0A)
                            continue;
                        else {
                            return S3Object::Terrain { true, -1, false, 0, 0, 0, 0, scrWrapAngle(this->AngleMode * 90), o };
                        }
                    }
                    else if (sc->objects[o]->solid) {
                        if ((sc->objects[o]->data1 == 0x0F ||
                             sc->objects[o]->data1 == 0x51 ||
                             sc->objects[o]->data1 == 0x11 ||
                             sc->objects[o]->data1 == 0x43 ||
                             sc->objects[o]->data1 == 0x51 ||
                             sc->objects[o]->data1 == 0x6C ||
                             sc->objects[o]->data1 == 0x71 ||
                             sc->objects[o]->data1 == 0xAF ||
                             sc->objects[o]->data1 == 0xB0 ||
                             sc->objects[o]->data1 == 0xB3) && Ground)
                            sc->objects[o]->doThing();
                        this->standSpdX = sc->objects[o]->standSpdX;
                        //this->standSpdY = sc->objects[o]->standSpdY;
                        return S3Object::Terrain { true, -1, false, (int)sc->objects[o]->Speed, 0, 0, 0, scrWrapAngle(this->AngleMode * 90), o };
                    }
                    else if (sc->objects[o]->solidTop && Gravity >= 0 && AngleMode == 0 && this->y + 12 <= sc->objects[o]->y + sc->objects[o]->h / 2.f) {
                        if ((sc->objects[o]->data1 == 0x0F ||
                             sc->objects[o]->data1 == 0x51 ||
                             sc->objects[o]->data1 == 0x11 ||
                             sc->objects[o]->data1 == 0x43 ||
                             sc->objects[o]->data1 == 0x51 ||
                             sc->objects[o]->data1 == 0x6C ||
                             sc->objects[o]->data1 == 0x71 ||
                             sc->objects[o]->data1 == 0xAF ||
                             sc->objects[o]->data1 == 0xB0 ||
                             sc->objects[o]->data1 == 0xB3) && Ground)
                            sc->objects[o]->doThing();
                        this->standSpdX = sc->objects[o]->standSpdX;
                        //this->standSpdY = sc->objects[o]->standSpdY;
                        return S3Object::Terrain { true, -1, true, 0, 0, 0, 0, scrWrapAngle(this->AngleMode * 90), o };
                    }
                    else
                        continue;
                }
            }
        }

        int chunkX = (int)(X2 / 128);
        int chunkY = (int)(Y2 / 128);
        if (Y2 < 0) {
            return S3Object::Terrain { false, -1, true };
        }
        int chunkID = sc->level[(sc->level[8] & 0xFF) * 0x100 + (sc->level[9] & 0xFF) - 0x8000 + chunkX + chunkY * sc->levelW] & 0xFF;

        int blockX = (int)(X2 / 16) - chunkX * 8;
        int blockY = (int)(Y2 / 16) - chunkY * 8;
        int blockData = sc->chunks[chunkID * 192 + blockX * 24 + blockY * 3] & 0xFF;

        int heightID1 = sc->chunks[chunkID * 192 + blockX * 24 + blockY * 3 + 1] & 0xFF;
        int heightID2 = sc->chunks[chunkID * 192 + blockX * 24 + blockY * 3 + 2] & 0xFF;

        int xToCheck = (int)X2 - chunkX * 128 - blockX * 16;
        int yToCheck = (int)Y2 - chunkY * 128 - blockY * 16;

        int solid1 = (blockData >> 6) & 0x3;
        int solid2 = (blockData >> 4) & 0x3;
        int flipY = (blockData >> 3) & 0x1;
        int flipX = (blockData >> 2) & 0x1;

        int height1 = 0;
        int height2 = 0;

        if (flipX == 1) {
            height1 = sc->heights[heightID1 * 16 + 15 - xToCheck] & 0xFF;
            height2 = sc->heights[heightID2 * 16 + 15 - xToCheck] & 0xFF;
        }
        else {
            height1 = sc->heights[heightID1 * 16 + xToCheck] & 0xFF;
            height2 = sc->heights[heightID2 * 16 + xToCheck] & 0xFF;
        }

        int angleFinal = -1;
        if (solid1 > 0 && this->Layer == 1) {
            angleFinal = sc->angles[heightID1] & 0xFF;
        }
        if (solid2 > 0 && this->Layer == 0) {
            angleFinal = sc->angles[heightID2] & 0xFF;
        }

        if (angleFinal != -1) {
            if ((solid1 > 0 ||
                solid2 > 0) && angleFinal != 0xFF) { // if not top-solid
                if (flipX == 0) {
                    angleFinal = 255 - angleFinal;
                }
                if (flipY == 1) {
                    angleFinal = (127 - angleFinal) % 255;
                }
            }

            //angleFinal = -angleFinal;

            //angleFinal = (int)std::round(angleFinal / 8.0f) * 8;

            if (angleFinal == 0xFF)
                angleFinal = this->AngleMode * 64;
        }

        if (solid1 > 0 && this->Layer == 1) {
            bool collided = false;
            if (height1 <= 16) {
                if ((yToCheck >= 16 - height1 && (flipY == 0 || solid1 == 1)) ||
                    (yToCheck <       height1 && flipY == 1 && solid1 != 1))
                    collided = true;
            }
            else {
                if ((yToCheck >= 16 - (255 - height1) && flipY == 1 && solid1 != 1) ||
                    (yToCheck <       (255 - height1) && (flipY == 0 || solid1 == 1)))
                    collided = true;
            }
            if (solid1 == 1) {
                if (collided && Sin[this->Angle] * signum(Speed) > 0.98) {
                    collided = false;
                }
            }
            if (collided) {
                return S3Object::Terrain { true, this->Layer, solid1 == 1, 0, height1, (int)X2, (int)Y2, scrWrapAngle((int)(angleFinal * 360.0f / 256.0f)) };
            }
        }
        if (solid2 > 0 && this->Layer == 0) {
            bool collided = false;
            if (height2 <= 16) {
                if ((yToCheck >= 16 - height2 && flipY == 0) ||
                    (yToCheck <       height2 && flipY == 1))
                    collided = true;
            }
            else {
                if ((yToCheck >= 16 - (255 - height2) && flipY == 1) ||
                    (yToCheck <       (255 - height2) && flipY == 0))
                    collided = true;
            }
            if (solid2 == 1) {
                if (collided && Sin[this->Angle] * signum(Speed) > 0.98) {
                    collided = false;
                }
            }
            if (collided) {
                return S3Object::Terrain { true, this->Layer, solid2 == 1, 0, height2, (int)X2, (int)Y2, scrWrapAngle((int)(angleFinal * 360.0f / 256.0f)) };
            }
        }

        X2 += this->Cos[(int)(angle * 180.0f / Math_PI + 360) % 360];
        Y2 -= this->Sin[(int)(angle * 180.0f / Math_PI + 360) % 360];

        if (loop)
            Y2 = fmod((Y2 + sc->levelH * 128), (sc->levelH * 128));
    }
    return S3Object::Terrain { false, -1, true };
}

// <editor-fold> COLLISION SENSORS
bool Player::scrPlayerCollisionSensor360(int obj, int len, int A, int no360) {
    int InitAngle = scrWrapAngle(std::round(this->Angle / 45.f) * 45.f);
    if (no360 == 2)
        InitAngle = scrWrapAngle(this->AngleMode * 90);
    int X2 = this->x - 9 * this->Cos[InitAngle];
    int Y2 = this->y + 9 * this->Sin[InitAngle];
    if (A == 1) {
        X2 = this->x;
        Y2 = this->y;
    }
    else if (A == 2) {
        X2 = this->x + 9 * this->Cos[InitAngle];
        Y2 = this->y - 9 * this->Sin[InitAngle];
    }

    int Height = 0;

    if (this->AngleMode == 0 && (this->Angle < 7 || this->Angle > 353) && no360 == 0)
        Height = this->GroundSensorHeightMin;
    else
        Height = this->GroundSensorHeight;

    if (no360 >= 1)
        Height = this->GroundSensorHeight;

    for (int i = 0; i <= Height + len; i++) {
        S3Object::Terrain Object = collision_line_terrain(X2, Y2, X2, Y2);

        if (Object.Active && no360 == 0 &&
            (Object.Layer == this->Layer || Object.Layer == -1) &&
            (Object.Platform == false || (Object.Platform == true /*&& this->AngleMode == 0 && i >= 20*/))) {
            return true;
        }
        else if (Object.Active && no360 > 0 &&
            (Object.Layer == this->Layer || Object.Layer == -1) &&
            (Object.Platform == false || (Object.Platform == true/* && this->AngleMode == 0 && i >= 20*/)/* ||
            (Object.Platform == 2 && this->Action != Actions::Jumping)*/)) {
            return true;
        }
        else {
            if (no360 == 2) {
                X2 += (int)this->Sin[InitAngle];
                Y2 += (int)this->Cos[InitAngle];
            }
            else {
                X2 += this->Sin[InitAngle];
                Y2 += this->Cos[InitAngle];
            }
        }
    }
    return false;
}
bool Player::scrPlayerCollisionASensor(int obj, int len) { return scrPlayerCollisionSensor360(obj, len, 0, 2); }
bool Player::scrPlayerCollisionBSensor(int obj, int len) { return scrPlayerCollisionSensor360(obj, len, 2, 2); }
bool Player::scrPlayerCollisionASensor360(int obj, int len) { return scrPlayerCollisionSensor360(obj, len, 0, 2); }
bool Player::scrPlayerCollisionBSensor360(int obj, int len) { return scrPlayerCollisionSensor360(obj, len, 2, 2); }
bool Player::scrPlayerCollisionMSensor360(int obj, int len) { return scrPlayerCollisionSensor360(obj, len, 1, 0); }

bool Player::scrPlayerCollisionLeftSensor(int obj) {
    int InitAngle = scrWrapAngle(this->AngleMode * 90); // Round angle to nearest 10.

    int X = this->x + 9 * this->Sin[InitAngle];
    int Y = this->y + 9 * this->Cos[InitAngle];

    int X2 = this->x - 12 * this->Cos[InitAngle] + 9 *this->Sin[InitAngle];
    int Y2 = this->y + 9 * this->Cos[InitAngle] + 12 *this->Sin[InitAngle];
    S3Object::Terrain Object = collision_line_terrain(X, Y, X2, Y2);

    if ((Object.Active && (Object.Layer == this->Layer || Object.Layer == -1) && Object.Platform != true))
        return true;
    return false;
}
bool Player::scrPlayerCollisionRightSensor(int obj) {
    int InitAngle = scrWrapAngle(this->AngleMode * 90); // Round angle to nearest 10.

    int X = this->x + 4.0f * this->Sin[InitAngle];
    int Y = this->y + 4.0f * this->Cos[InitAngle];

    int X2 = this->x + 12 * this->Cos[InitAngle] + 4 *this->Sin[InitAngle];
    int Y2 = this->y + 4 * this->Cos[InitAngle] - 12 *this->Sin[InitAngle];
    S3Object::Terrain Object = collision_line_terrain(X, Y, X2, Y2);

    if ((Object.Active && (Object.Layer == this->Layer || Object.Layer == -1) && Object.Platform != true))
        return true;
    return false;
}

bool Player::scrPlayerCollisionCSensor(int obj, int len) {
    int InitAngle = scrWrapAngle(this->AngleMode * 90);
    int Length = 0;

    if (this->Action == Actions::Jumping || this->Action == Actions::Rolling)
        Length = 10;
    else
        Length = 20;

    if (this->Action == Actions::Rolling) { return false; }

    int X = this->x - ((Length + len) * this->Sin[InitAngle]) - (10 * this->Cos[InitAngle]);
    int Y = this->y - ((Length + len) * this->Cos[InitAngle]) + (10 * this->Sin[InitAngle]);

    int X2 = this->x - (10 * this->Cos[InitAngle]);
    int Y2 = this->y + (10 * this->Sin[InitAngle]);

    S3Object::Terrain Object = collision_line_terrain(X2, Y2, X, Y);

    if (Object.Active && (Object.Layer == Layer || Object.Layer == -1))
        return true;

    return false;
}
bool Player::scrPlayerCollisionDSensor(int obj, int len) {
    int InitAngle = scrWrapAngle(this->AngleMode * 90);
    int Length = 0;

    if (this->Action == Actions::Jumping || this->Action == Actions::Rolling)
        Length = 10;
    else
        Length = 20;

    if (this->Action == Actions::Rolling) { return false; }

    int X = this->x - ((Length + len) * this->Sin[InitAngle]) + (10 * this->Cos[InitAngle]);
    int Y = this->y - ((Length + len) * this->Cos[InitAngle]) - (10 * this->Sin[InitAngle]);

    int X2 = this->x + (10 * this->Cos[InitAngle]);
    int Y2 = this->y - (10 * this->Sin[InitAngle]);

    S3Object::Terrain Object = collision_line_terrain(X2, Y2, X, Y);

    if (Object.Active && (Object.Layer == Layer || Object.Layer == -1))
        return true;

    return false;
}

bool Player::scrPlayerCollisionSquish() {
    S3Object::Terrain Object1 = collision_line_terrain(x - 9, y + 8, x - 2, y + 8);
    S3Object::Terrain Object2 = collision_line_terrain(x + 2, y + 8, x + 9, y + 8);
    if (Object1.Active && !Object1.Platform) {
        if (Object2.Active && !Object2.Platform) {
            return true;
        }
    }
    return false;
}
// </editor-fold>

void Player::bounceAwayFrom(float xx, float yy, float vel) {
    float angle = std::atan2(yy - y, x - xx);

    Speed = vel * std::cos(angle);
    Gravity = vel * -std::sin(angle);

    Vibrate(0, 0.15, 150);
}

void Player::scrPushPlayerX_Id(S3Object::Terrain obj) {
    int InitAngle = scrWrapAngle(this->Angle);

    int hei = 8;

    float X = (int)this->x + hei * (int)this->Sin[InitAngle];
    float Y = (int)this->y + hei * (int)this->Cos[InitAngle];

    int len = 10;

    float X2 = X + len * (int)this->Cos[InitAngle];
    float Y2 = Y - len * (int)this->Sin[InitAngle];

    S3Object::Terrain Object = S3Object::Terrain { false, -1, true };

    for (int i = 0; i < 12 && Ground; i++) {
        Object = collision_line_terrain(X, Y, X2, Y2);
        if (Object.Active && (Object.Layer == this->Layer || Object.Layer == -1) && Object.Platform == false && this->Speed >= 0) {
            this->x -= this->Cos[InitAngle];
            this->y += this->Sin[InitAngle];
            this->x += Object.bbox_left * 1.5f;

            // recalculate x y
            X = (int)this->x + hei * (int)this->Sin[InitAngle];
            Y = (int)this->y + hei * (int)this->Cos[InitAngle];
            X2 = X + len * this->Cos[InitAngle];
            Y2 = Y - len * this->Sin[InitAngle];
        }
        else
            break;
    }

    X = (int)this->x + hei * (int)this->Sin[InitAngle];
    Y = (int)this->y + hei * (int)this->Cos[InitAngle];

    X2 = X - len * (int)this->Cos[InitAngle];
    Y2 = Y + len * (int)this->Sin[InitAngle];

    for (int i = 0; i < 12 && Ground; i++) {
        Object = collision_line_terrain(X, Y, X2, Y2);
        if (Object.Active && (Object.Layer == this->Layer || Object.Layer == -1) && Object.Platform == false && this->Speed <= 0) {
            this->x += (int)this->Cos[InitAngle];
            this->y -= (int)this->Sin[InitAngle];
            this->x += Object.bbox_left * 1.5f;

            X = (int)this->x + hei * (int)this->Sin[InitAngle];
            Y = (int)this->y + hei * (int)this->Cos[InitAngle];
            X2 = X - len * this->Cos[InitAngle];
            Y2 = Y + len * this->Sin[InitAngle];
        }
        else
            break;
    }

    if (!this->Ground) {
        hei = -3;

        len = 11;

        X = (int)this->x + hei * (int)this->Sin[InitAngle];
        Y = (int)this->y + hei * (int)this->Cos[InitAngle];

        X2 = X + len * (int)this->Cos[InitAngle];
        Y2 = Y - len * (int)this->Sin[InitAngle];

        S3Object::Terrain Object = S3Object::Terrain { false, -1, true };

        for (int i = 0; i < 12; i++) {
            Object = collision_line_terrain(X, Y, X2, Y2);
            if (Object.Active && (Object.Layer == this->Layer || Object.Layer == -1) && Object.Platform == false && this->Speed >= 0) {
                this->x -= this->Cos[InitAngle] + Object.bbox_left * 1.5f;
                this->y += this->Sin[InitAngle];

                X = (int)this->x + hei * (int)this->Sin[InitAngle];
                Y = (int)this->y + hei * (int)this->Cos[InitAngle];
                X2 = X + len * this->Cos[InitAngle];
                Y2 = Y - len * this->Sin[InitAngle];
            }
            else
                break;
        }

        X = (int)this->x + hei * (int)this->Sin[InitAngle];
        Y = (int)this->y + hei * (int)this->Cos[InitAngle];
        X2 = X - len * (int)this->Cos[InitAngle];
        Y2 = Y + len * (int)this->Sin[InitAngle];

        for (int i = 0; i < 12; i++) {
            Object = collision_line_terrain(X, Y, X2, Y2);
            if (Object.Active && (Object.Layer == this->Layer || Object.Layer == -1) && Object.Platform == false && this->Speed <= 0) {
                this->x += this->Cos[InitAngle] + Object.bbox_left * 1.5f;
                this->y -= this->Sin[InitAngle];

                X = (int)this->x + hei * (int)this->Sin[InitAngle];
                Y = (int)this->y + hei * (int)this->Cos[InitAngle];
                X2 = X - len * this->Cos[InitAngle];
                Y2 = Y + len * this->Sin[InitAngle];
            }
            else
                break;
        }
    }
}
void Player::scrPushPlayerX() { scrPushPlayerX_Id({ false, 0, true }); }
void Player::scrPushPlayerY() {
    int InitAngle = scrWrapAngle(this->AngleMode * 90);

    //if (ForceRoll) return;

    objChek = true;

    int pushOutNess = 128;
    for (int i = 0; i < pushOutNess; i++) {
        if (scrPlayerCollisionASensor(0, -17) && scrPlayerCollisionCSensor(0, 0) == false) {
            this->x -= (int)this->Sin[InitAngle];
            this->y -= (int)this->Cos[InitAngle];
        }
        else
            break;
    }
    for (int i = 0; i < pushOutNess; i++) {
        if (scrPlayerCollisionBSensor(0, -17) && scrPlayerCollisionDSensor(0, 0) == false) {
            this->x -= (int)this->Sin[InitAngle];
            this->y -= (int)this->Cos[InitAngle];
        }
        else
            break;
    }
}
void Player::scrPushPlayerYLand() {
    if (this->Ground == true && this->Landed == false)
        return;

    int InitAngle = scrWrapAngle(this->AngleMode * 90);

    for (int i = 0; i < 160; i++) {
        if (scrPlayerCollisionASensor(0, -17) && scrPlayerCollisionCSensor(0, 0) == false) {
            this->x -= this->Sin[InitAngle];
            this->y -= this->Cos[InitAngle];
        }
        else
            break;
    }
    for (int i = 0; i < 160; i++) {
        if (scrPlayerCollisionBSensor(0, -17) && scrPlayerCollisionDSensor(0, 0) == false) {
            this->x -= this->Sin[InitAngle];
            this->y -= this->Cos[InitAngle];
        }
        else
            break;
    }
}
void Player::scrPushPlayerYDown() {
    //int InitAngle = scrWrapAngle(this->AngleMode * 90);

    for (int i = 0; i < 40; i++) {
        S3Object::Terrain Object = collision_line_terrain(this->x - 9, this->y - 9, this->x - 9, this->y - 9);
        if (Object.Active && (Object.Layer == this->Layer || Object.Layer == -1) && (Object.Platform == false)) {
            //x -= dsin(InitAngle);
            Gravity = 0;
            y += 1;
        }
        Object = collision_line_terrain(this->x + 9, this->y - 9, this->x + 9, this->y - 9);
        if (Object.Active && (Object.Layer == this->Layer || Object.Layer == -1) && (Object.Platform == false)) {
            //x -= dsin(InitAngle);
            Gravity = 0;
            y += 1;
        }
        else {
            return;
        }
    }
}
void Player::scrCheckGroundLevel() {
    int InitAngle = scrWrapAngle(AngleMode * 90);

    float LeftX2   = (int)(this->x) - (9 * this->Cos[InitAngle]);
    float LeftY2   = (int)(this->y) + (9 * this->Sin[InitAngle]);
    float LeftX    = (int)(this->x) + (20 * this->Sin[InitAngle]);
    float LeftY    = (int)(this->y) + (20 * this->Cos[InitAngle]);
    //float LeftX2OG = LeftX2;
    //float LeftY2OG = LeftY2;

    float RightX2  = (int)(this->x) + (9 * this->Cos[InitAngle]);
    float RightY2  = (int)(this->y) - (9 * this->Sin[InitAngle]);
    float RightX   = (int)(this->x) + (20 * this->Sin[InitAngle]);
    float RightY   = (int)(this->y) + (20 * this->Cos[InitAngle]);
    //float RightX2OG = RightX2;
    //float RightY2OG = RightY2;

    float MiddleX2 = (int)(this->x);
    float MiddleY2 = (int)(this->y);
    float MiddleX  = (int)(this->x) + (20 * this->Sin[InitAngle]);
    float MiddleY  = (int)(this->y) + (20 * this->Cos[InitAngle]);
    //float MiddleX2OG = MiddleX2;
    //float MiddleY2OG = MiddleY2;

    int Add = 0;
    if (this->AngleMode != 0)
        Add = 0;

    bool found = false;

    S3Object::Terrain TerrainLeft {false};
    for (int i = 0; i < this->GroundSensorHeight + Add; i++) {
        TerrainLeft = collision_line_terrain((int)LeftX2, (int)LeftY2, (int)LeftX2, (int)LeftY2);
        //if (TerrainLeft.Active && instance_exists(TerrainLeft) == false)
        //    TerrainLeft = noone;

        if (TerrainLeft.Active && TerrainLeft.Platform == true) {
            //found = true;
        }

        if (TerrainLeft.Active &&
            (TerrainLeft.Layer == this->Layer || TerrainLeft.Layer == -1) &&
            (TerrainLeft.Platform == false ||
            (TerrainLeft.Platform == true))// ||
            /*(TerrainLeft.Platform == 2  && Action != ActionJumping))*/) {
            LeftX  = LeftX2;
            LeftY  = LeftY2;
            break;
        }
        else {
            LeftX2 += (int)this->Sin[InitAngle];
            LeftY2 += (int)this->Cos[InitAngle];
            TerrainLeft = { false, -1, true };
        }
        if (i == this->GroundSensorHeight + Add - 1) {
            //LeftX += this->Sin[InitAngle] * (this->GroundSensorHeight + Add + 3);
            //LeftY += this->Cos[InitAngle] * (this->GroundSensorHeight + Add + 3);
        }
    }

    found = false;

    S3Object::Terrain TerrainRight {false};
    for (int i = 0; i < this->GroundSensorHeight + Add; i++) {
        TerrainRight = collision_line_terrain((int)RightX2, (int)RightY2, (int)RightX2, (int)RightY2);
        //if (TerrainRight != noone && instance_exists(TerrainRight) == false)
        //    TerrainRight = noone;

        //if (TerrainRight.Active && TerrainRight.Platform == true)
            //found = true;

        if (TerrainRight.Active &&
            (TerrainRight.Layer == Layer || TerrainRight.Layer == -1) &&
            (TerrainRight.Platform == false ||
                (TerrainRight.Platform == true))
            /*|| (TerrainRight.Platform == 2  && Action != ActionJumping)*/) {
            RightX  = RightX2;
            RightY  = RightY2;
            break;
        }
        else {
            RightX2 += (int)this->Sin[InitAngle];
            RightY2 += (int)this->Cos[InitAngle];
            TerrainRight = { false, -1, true };
        }
        if (i == this->GroundSensorHeight + Add - 1) {
            //RightX += this->Sin[InitAngle] * (this->GroundSensorHeight + Add + 3);
            //RightY += this->Cos[InitAngle] * (this->GroundSensorHeight + Add + 3);
        }
    }

    /*if (TerrainLeft != noone && instance_exists(TerrainLeft) == false)
        TerrainLeft = noone;
    if (TerrainRight != noone && instance_exists(TerrainRight) == false)
        TerrainRight = noone;*/

    found = false;



    S3Object::Terrain TerrainMiddle {false};
    for (int i = 0; i < this->GroundSensorHeight; i++) {
        TerrainMiddle = collision_line_terrain((int)MiddleX2, (int)MiddleY2, (int)MiddleX2, (int)MiddleY2);

        //if (TerrainMiddle.Active && TerrainMiddle.Platform == true)
            //found = true;

        if (TerrainMiddle.Active) {
            S3Object::Terrain TerrainMiddle2 = collision_line_terrain(
                (int)MiddleX2 - (int)Sin[InitAngle],
                (int)MiddleY2 - (int)Cos[InitAngle],
                (int)MiddleX2 - (int)Sin[InitAngle],
                (int)MiddleY2 - (int)Cos[InitAngle]);
            if (TerrainMiddle2.Active) {
                if (((TerrainMiddle2.Layer == Layer || TerrainMiddle2.Layer == -1) &&

                (TerrainMiddle.Platform == false ||
                    (TerrainMiddle.Platform == true && !found))

        )) {
                    TerrainMiddle = { false, -1, true };
                    MiddleX2 = (int)this->x + 20 * (int)Sin[InitAngle];
                    MiddleY2 = (int)this->y + 20 * (int)Cos[InitAngle];
                    break;
                }
            }
        }
        if (TerrainMiddle.Active &&
            (TerrainMiddle.Layer == Layer || TerrainMiddle.Layer == -1) &&

            (TerrainMiddle.Platform == false ||
                (TerrainMiddle.Platform == true) /*|| (TerrainMiddle.Platform == 2  && Action != Actions::Jumping)*/)) {
            MiddleX  = MiddleX2;
            MiddleY  = MiddleY2;
            break;
        }
        else{
            TerrainMiddle = { false, -1, true, 0, 0, 0, 0, 0 };
            MiddleX2 += (int)Sin[InitAngle];
            MiddleY2 += (int)Cos[InitAngle];
        }
    }

    if (this->AngleMode == 0) {
        if (TerrainMiddle.Active && (MiddleY < LeftY && MiddleY < RightY)) {
            this->TerrainId = TerrainMiddle;
        }
        else if (RightY < LeftY/* || (TerrainLeft.Active && this->x > TerrainLeft.bbox_right)) &&(this->x >= TerrainRight.bbox_left)*/) {
            this->TerrainId = TerrainRight;
        }
        else if (LeftY < RightY/* || (TerrainRight.Active && this->x < TerrainRight.bbox_left)) && this->x <= TerrainLeft.bbox_right*/) {
            this->TerrainId = TerrainLeft;
        }
        else {
            if (TerrainMiddle.Active)
                this->TerrainId  = TerrainMiddle;
            else if (TerrainRight.Active)
                this->TerrainId  = TerrainRight;
            else if (TerrainLeft.Active)
                this->TerrainId  = TerrainLeft;
        }

        if (TerrainRight.Active && TerrainLeft.Active && TerrainMiddle.Active) {
            this->y = std::min(LeftY, std::min(RightY, MiddleY)) - 20.0f;
            /*printf("TerrainLeft's Angle: %d\n", TerrainLeft.angle);
            printf("TerrainMiddle's Angle: %d\n", TerrainMiddle.angle);
            printf("TerrainRight's Angle: %d\n", TerrainRight.angle);
            printf("Angle: %d\n", this->Angle);*/
        }
        else if (TerrainLeft.Active == false && (TerrainMiddle.Active || std::abs((RightY-20)-(int)(this->y)) <= 0 || TerrainRight.Active)) {
            this->y = RightY-20.0f;
            TerrainId = TerrainRight;
            //printf("TerrainRight's Angle: %d\n", TerrainRight.angle);
        }
        else if (TerrainRight.Active == false && (TerrainMiddle.Active || std::abs((LeftY-20)-(int)(this->y)) <= 0 || TerrainLeft.Active)) {
            this->y = LeftY-20.0f;
            TerrainId = TerrainLeft;
            //printf("TerrainLeft's Angle: %d\n", TerrainLeft.angle);
        }
    }
    else if (this->AngleMode == 1) {
        if (/* DISABLES CODE */ (false) && TerrainMiddle.Active && (MiddleX == LeftX && MiddleX == RightX)) {
            this->TerrainId = TerrainMiddle;
        }
        else if (RightX < LeftX/* ||(TerrainLeft.Active && this->y < TerrainLeft.bbox_top))&& this->y <= TerrainRight.bbox_bottom*/) {
            this->TerrainId = TerrainRight;
        }
        else if (LeftX < RightX/* ||(TerrainRight.Active && this->y > TerrainRight.bbox_bottom)) && (this->y >= TerrainLeft.bbox_top)*/) {
            this->TerrainId = TerrainLeft;
        }
        else {
            if (TerrainMiddle.Active)
                TerrainId  = TerrainMiddle;
            else if (TerrainRight.Active)
                TerrainId  = TerrainRight;
            else if (TerrainLeft.Active)
                TerrainId  = TerrainLeft;
        }
        if (TerrainRight.Active && TerrainLeft.Active && TerrainMiddle.Active) {
            this->x = std::min(LeftX, std::min(RightX, MiddleX))-20.0f;
        }
        else if (TerrainLeft.Active == false && (TerrainMiddle.Active || std::abs((RightX-20)-(int)(this->x)) <= 0 || TerrainRight.Active)) {
            this->x = RightX-20.0f;
            TerrainId = TerrainRight;
        }
        else if (TerrainRight.Active == false && (TerrainMiddle.Active || std::abs((LeftX-20)-(int)(this->x)) <= 0 || TerrainLeft.Active)) {
            this->x = LeftX-20.0f;
            TerrainId = TerrainLeft;
        }

    }
    else if (this->AngleMode == 2) {
        if (TerrainMiddle.Active && (MiddleY > LeftY && MiddleY > RightY)) {
                TerrainId = TerrainMiddle;
        }
        else if (TerrainRight.Active && (RightY > LeftY ||(TerrainLeft.Active && this->x < TerrainLeft.bbox_left))&& this->x <= TerrainRight.bbox_right) {
                TerrainId = TerrainRight;
        }else if (TerrainLeft.Active && (LeftY > RightY ||(TerrainRight.Active && this->x > TerrainRight.bbox_right)) &&  this->x >= TerrainLeft.bbox_left) {
                TerrainId = TerrainLeft;
        }else{
            if (TerrainMiddle.Active)
                TerrainId  = TerrainMiddle;
            else if (TerrainRight.Active)
                TerrainId  = TerrainRight;
            else if (TerrainLeft.Active)
                TerrainId  = TerrainLeft;
        }

        if (TerrainRight.Active && TerrainLeft.Active && TerrainMiddle.Active) {
            this->y = std::max(LeftY, std::max(RightY, MiddleY))+20;
        }
        else if (TerrainLeft.Active == false && (TerrainMiddle.Active || std::abs((RightY-20)-(int)this->y) <= 0)) {
            this->y = RightY+20;
            TerrainId = TerrainRight;
        }
        else if (TerrainRight.Active == false &&(TerrainMiddle.Active || std::abs((LeftY-20)-(int)this->y) <= 0)) {
            this->y = LeftY+20;
            TerrainId = TerrainLeft;
        }
    }
    else if (this->AngleMode == 3) {
        if (TerrainMiddle.Active && (MiddleX == LeftX && MiddleX == RightX)) {
                TerrainId = TerrainMiddle;
        }
        else if (RightX > LeftX) {//TerrainRight.Active && (RightX > LeftX ||(TerrainLeft.Active && this->y > TerrainLeft.bbox_bottom))&& this->y >= TerrainRight.bbox_top) {
                TerrainId = TerrainRight;
        }
        else if (LeftX > RightX) {//TerrainLeft.Active && (LeftX > RightX ||(TerrainRight.Active && this->y < TerrainRight.bbox_top)) && this->y <= TerrainLeft.bbox_bottom) {
                TerrainId = TerrainLeft;
        }
        else {
            if (TerrainMiddle.Active)
                TerrainId  = TerrainMiddle;
            else if (TerrainRight.Active)
                TerrainId  = TerrainRight;
            else if (TerrainLeft.Active)
                TerrainId  = TerrainLeft;
        }
        if (TerrainRight.Active && TerrainLeft.Active && TerrainMiddle.Active) {
            this->x = std::max(LeftX, std::max(RightX, MiddleX))+20;
        }
        else if (TerrainLeft.Active == false && (TerrainMiddle.Active || std::abs((RightX+20)-(int)this->x) <= 0 || TerrainRight.Active)) {
            this->x = RightX+20;
            TerrainId = TerrainRight;
        }
        else if (TerrainRight.Active == false && (TerrainMiddle.Active || std::abs((LeftX+20)-(int)this->x) <= 0 || TerrainLeft.Active)) {
            this->x = LeftX+20;
            TerrainId = TerrainLeft;
        }
    }

    AnglePos[2][0] = LeftX;
    AnglePos[2][1] = LeftY;

    AnglePos[3][0] = RightX;
    AnglePos[3][1] = RightY;
};
bool Player::scrPlayerHandleBreakableTerrain() {
    if ((Action == Actions::Jumping && Gravity >= 0.5f) || (!Ground && Gravity < -1.0f)) {
        // If "Gravity" >= 0
        // Check A sensor -16
            // If there, Gravity = -3; Destroy Object;
        // Check B sensor -16
            // If there, Gravity = -3; Destroy Object;
        for (unsigned int o = 0; o < sc->objects_count; o++) {
            if (sc->objects[o] != NULL) {
                if (sc->objects[o]->breakableVert && sc->objects[o]->active) {
                    if (this->x + 10 >= sc->objects[o]->x - sc->objects[o]->w / 2 &&
                        this->y + 25 >= sc->objects[o]->y - sc->objects[o]->h / 2 &&
                        this->x - 10 <= sc->objects[o]->x + sc->objects[o]->w / 2 &&
                        this->y - 25 <= sc->objects[o]->y + sc->objects[o]->h / 2) {

                        if ((Action == Actions::Jumping && Gravity >= 0.5f && y < sc->objects[o]->y) || (!Ground && Gravity < -1.0f && y > sc->objects[o]->y)) {
                            sc->objects[o]->OnBreakVertical(Gravity);
                            Gravity = -3;
                            Vibrate(0, VIBRATE_IMPACT);
                            return true;
                        }
                    }
                }
            }
        }

    }
    if ((Action == Actions::Rolling && std::abs(Speed) >= 4.5f) || SuperForm || CharacterId == consCharacterKnuckles) {
        for (unsigned int o = 0; o < sc->objects_count; o++) {
            if (sc->objects[o] != NULL) {
                if (sc->objects[o]->breakable && sc->objects[o]->active)
                    if (this->x + 24 >= sc->objects[o]->x - sc->objects[o]->w / 2 &&
                        this->y + 20 >= sc->objects[o]->y - sc->objects[o]->h / 2 &&
                        this->x - 24 <= sc->objects[o]->x + sc->objects[o]->w / 2 &&
                        this->y - 20 <= sc->objects[o]->y + sc->objects[o]->h / 2 && signum(this->Speed) == signum(sc->objects[o]->x - x) &&
                        std::abs(y + 20 - (sc->objects[o]->y + sc->objects[o]->h / 2)) < 5) {
                        sc->objects[o]->OnBreakHorizontal(Speed);
                        Vibrate(0, VIBRATE_IMPACT);
                        return true;
                    }
            }
        }

        // Continually look for sc->objects in a player-centered 40x40 box that are breakable to destroy
        /*_ObjectHandle = collision_rectangle(x-20, y-20, x+20, y+20, objBreakableTerrainRoll, false, true);
        while(_ObjectHandle != noone) {
            with(_ObjectHandle)
                instance_destroy();
            _ObjectHandle = collision_rectangle(x-20, y-20, x+20, y+20, objBreakableTerrainRoll, false, true)
        }*/
    }
    return false;
}
void Player::scrPlayerHandleMonitors() { }
void Player::scrPlayerHandlePathSwitchers() {
    //sc->pathswitchers_count
    for (unsigned int o = 0; o < sc->pathswitchers_count; o++) {
        S3Object* obj = sc->pathswitchers[o];
        if (obj != NULL) {
            if (this->x + 10 >= obj->x - obj->w / 2.f &&
                this->y + 22 >= obj->y - obj->h / 2.f &&
                this->x - 10 <  obj->x + obj->w / 2.f &&
                this->y - 22 <  obj->y + obj->h / 2.f) {
                if (obj->data1 == 0x02) {
                    int groundOnly = (obj->data2 >> 7) & 0x01;
                    int orientation = (obj->data2 >> 2) & 0x01;

                    int leftUpPath = (obj->data2 >> 4) & 0x01;
                    //int leftUpPriority = (sc->objects[o]->data2 >> 6) & 0x01; //1 is high, 0 is low
                    int rightDownPath = (obj->data2 >> 3) & 0x01;
                    //int rightDownPriority = (sc->objects[o]->data2 >> 5) & 0x01;

                    if (groundOnly == 1 && !this->Ground)
                        break;

                    if ((this->Gravity > 0 && orientation == 1) || (this->Speed * this->Sin[this->AngleMode * 90] < 0 && orientation == 1) || (this->Speed * this->Cos[this->AngleMode * 90] > 0 && orientation == 0)) {
                        this->Layer = rightDownPath;
                    }
                    if ((this->Gravity < 0 && orientation == 1) || (this->Speed * this->Sin[this->AngleMode * 90] > 0 && orientation == 1) || (this->Speed * this->Cos[this->AngleMode * 90] < 0 && orientation == 0)) {
                        this->Layer = leftUpPath;
                    }
                }
                else if (obj->Collidable) {
                    obj->OnCollisionWithPlayer(PlayerID, Speed, Gravity);
                }
            }
        }
    }
}
void Player::scrPlayerHandleObjects() {
    bool Grabbing = false;
    this->Fan = false;
    for (unsigned int o = 0; o < sc->objects_count && objChek; o++) {
        if (sc->objects[o] != NULL) {

            int wExtend = 0;
            if ((sc->objects[o]->data1 & 0xFF) == 0x07)
                wExtend = 3;
            if ((sc->objects[o]->data1 & 0xFF) == 0x08)
                wExtend = 10;

            if (sc->objects[o]->active && this->x + 9 + (Shield == 4 ? 11 : wExtend) + std::abs(Cos[AngleMode * 90])/* + (Speed * Cos[AngleMode * 90])*/ >= sc->objects[o]->x - sc->objects[o]->w / 2.f &&
                this->y + 24 >= sc->objects[o]->y - sc->objects[o]->h / 2.f &&
                this->x - 9 - (Shield == 4 ? 11 : wExtend) - std::abs(Cos[AngleMode * 90])/* + (Speed * Cos[AngleMode * 90])*/ <  sc->objects[o]->x + sc->objects[o]->w / 2.f &&
                this->y - 22 + (Action == Actions::CrouchDown || Action == Actions::Rolling ? 16 : 0) <  sc->objects[o]->y + sc->objects[o]->h / 2.f) {

                //float myW = 13 + (Shield == 4 ? 11 : 0) + std::abs(Cos[AngleMode * 90]) + (Speed * Cos[AngleMode * 90]);

                float ww = 0.5f * (20 + sc->objects[o]->w);
                float hh = 0.5f * (40 - (Action == Actions::CrouchDown || Action == Actions::Rolling ? 10 : 0) + sc->objects[o]->h);
                float dx = this->x - sc->objects[o]->x;
                float dy = this->y - sc->objects[o]->y;

                //dx = -dx;
                dy = -dy;

                float wy = ww * dy;
                float hx = hh * dx;


                if ((sc->objects[o]->data1 & 0xFF) == 0x00) { // Ring
                    sc->objects[o]->doThing();
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x01) { // Monitor
                    /*if (!AIControlled)
                        continue;*/

                    if (((this->Action == Actions::Jumping && this->Gravity > 0 && !this->Ground) || (this->Action == Actions::Rolling && this->Ground) || (Shield == 4 && Invincibility == 0) ) && sc->objects[o]->data2 != 0x0A) {
                            this->Gravity = this->Gravity * -1.0f;

                            $AudioPlay(aud_BadnikDestroyed, 3);
                            Vibrate(0, 0.25, 150);

                            sc->objects[o]->OnCollisionWithPlayer(PlayerID, Speed, Gravity);

                            sc->objects[o]->doThing();

                            this->Ground = false;
                    }
                    if (sc->objects[o]->data2 == 0x0A) {
                        continue;
                    }
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x02 && false) { // Path Switchers
                    /*if (this->x + 11 >= sc->objects[o]->x - sc->objects[o]->w / 2.f &&
                        this->x - 11 <  sc->objects[o]->x + sc->objects[o]->w / 2.f) {
                    } else continue;

                    int groundOnly = (sc->objects[o]->data2 >> 7) & 0x01;
                    int orientation = (sc->objects[o]->data2 >> 2) & 0x01;

                    int leftUpPath = (sc->objects[o]->data2 >> 4) & 0x01;
                    //int leftUpPriority = (sc->objects[o]->data2 >> 6) & 0x01; //1 is high, 0 is low
                    int rightDownPath = (sc->objects[o]->data2 >> 3) & 0x01;
                    //int rightDownPriority = (sc->objects[o]->data2 >> 5) & 0x01;

                    if (groundOnly == 1 && !this->Ground)
                        break;

                    if ((this->Gravity > 0 && orientation == 1) || (this->Speed * this->Sin[this->AngleMode * 90] < 0 && orientation == 1) || (this->Speed * this->Cos[this->AngleMode * 90] > 0 && orientation == 0)) {
                        this->Layer = rightDownPath;
                    }
                    if ((this->Gravity < 0 && orientation == 1) || (this->Speed * this->Sin[this->AngleMode * 90] > 0 && orientation == 1) || (this->Speed * this->Cos[this->AngleMode * 90] < 0 && orientation == 0)) {
                        this->Layer = leftUpPath;
                    }*/
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x03) { // AIZ_GiantLog
                    sc->objects[o]->doThing(PlayerID);
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x07) { // Springs
                    if (this->y + 20 >= sc->objects[o]->y - sc->objects[o]->h / 2.f + 3 &&
                        this->y - 20 <  sc->objects[o]->y + sc->objects[o]->h / 2.f - 3 && (sc->objects[o]->data2 & 0xF0) == 0x10) { }
                    else if (this->x + 12 >= sc->objects[o]->x - sc->objects[o]->w / 2.f + 3 &&
                        this->x - 12 <  sc->objects[o]->x + sc->objects[o]->w / 2.f - 3 && (sc->objects[o]->data2 & 0xF0) != 0x10) { }
                    else { continue; }

                    float num = 16;
                    int tempSpringFlip = 0;
                    if ((sc->objects[o]->data2 & 0x2) != 0x0) {
                        num = 10;
                        tempSpringFlip = (sc->objects[o]->data2 & 0x1);
                    }
                    if (Underwater)
                        tempSpringFlip = 1 - tempSpringFlip;

                    if ((sc->objects[o]->data2 & 0xF0) == 0x00) { // up spring
                        if (this->AngleMode == 0 && y + 20 < sc->objects[o]->y) {
                            this->Gravity = -num;
                            this->Ground = false;
                            this->InputAlarm = 16;
                            SpringFlip = tempSpringFlip;
                            Action = Actions::Spring;
                            sc->objects[o]->doThing();
                            $AudioPlay(aud_Spring, 1);
                            Vibrate(0, 0.25, 150);
                        }
                        else {
                            if (this->AngleMode == 1 || this->AngleMode == 3) {
                                this->Speed = num * (int)this->Sin[this->AngleMode * 90];
                                this->flip = (int)this->Sin[this->AngleMode * 90];
                                this->InputAlarm = 16;
                                Action = Actions::Spring;
                                sc->objects[o]->doThing();
                                $AudioPlay(aud_Spring, 1);
                                Vibrate(0, 0.25, 150);
                            }
                        }
                    }
                    else if ((sc->objects[o]->data2 & 0xF0) == 0x20) { // down spring
                        this->Gravity = num;
                        if (this->AngleMode == 1 || this->AngleMode == 3) {
                            this->Speed = -num * (int)this->Sin[this->AngleMode * 90];
                        }
                        this->Layer = 1 - ((sc->objects[o]->data2 >> 2) & 0x1);
                        sc->objects[o]->doThing();
                        $AudioPlay(aud_Spring, 1);
                        Vibrate(0, 0.25, 150);
                    }
                    else if ((sc->objects[o]->data2 & 0xF0) == 0x10 && sc->objects[o]->flipX && Speed >= 0) { // right spring
                        if (this->AngleMode == 0 || this->AngleMode == 2) {
                            this->Speed = num * (int)this->Cos[this->AngleMode * 90];
                        }
                        if (((sc->objects[o]->data2 >> 2) & 0x1) != ((sc->objects[o]->data2 >> 3) & 0x1)) // Turns out, the 3rd bit controls which layer the player is switched to, but only if the 4th bit is the opposite of it.
                            this->Layer = 1 - ((sc->objects[o]->data2 >> 2) & 0x1);
                        this->flip = 1;

                        sc->objects[o]->doThing();
                        $AudioPlay(aud_Spring, 1);
                        Vibrate(0, 0.25, 150);
                    }
                    else if ((sc->objects[o]->data2 & 0xF0) == 0x10 && !sc->objects[o]->flipX && Speed <= 0) { // left spring
                        if (this->AngleMode == 0 || this->AngleMode == 2) {
                            this->Speed = -num * (int)this->Cos[this->AngleMode * 90];
                        }
                        if (((sc->objects[o]->data2 >> 2) & 0x1) != ((sc->objects[o]->data2 >> 3) & 0x1)) // Turns out, the 3rd bit controls which layer the player is switched to, but only if the 4th bit is the opposite of it.
                            this->Layer = 1 - ((sc->objects[o]->data2 >> 2) & 0x1);
                        this->flip = -1;

                        sc->objects[o]->doThing();
                        $AudioPlay(aud_Spring, 1);
                        Vibrate(0, 0.25, 150);
                    }
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x08) { // Spikes
                    Spikes* obj = (Spikes*)sc->objects[o];

                    if (obj->rot % 180 == 90) {
                        //ww += 1;
                        hh -= 1;
                    }
                    else {
                        //hh += 1;
                        ww -= 1;
                    }
                    if (obj->rot == 270) {
                        ww += 2;
                    }
                    wy = ww * dy;
                    hx = hh * dx;

                    if (std::abs(dx) <= ww && std::abs(dy) <= hh) {
                        if (wy > hx) {
                            if (wy > -hx && (obj->rot == 0)) {
                                /* collision at the top */
                                scrPlayerHurt((int)sc->objects[o]->x, true);
                                //app->print(0, "Top");
                            }
                            else {
                                /* on the right */
                                if (obj->rot == 270) {
                                    scrPlayerHurt((int)sc->objects[o]->x, true);
                                    app->print(0, "OUCH");
                                }
                                app->print(0, "Left %d", obj->rot);
                            }
                        }
                        else {
                            if (wy > -hx) {
                                /* on the left */
                                if (obj->rot == 90) {
                                    scrPlayerHurt((int)sc->objects[o]->x, true);
                                }
                                //app->print(0, "Right %d", obj->rot);
                            }
                            else if (obj->rot == 180) {
                                /* at the bottom */
                                scrPlayerHurt((int)sc->objects[o]->x, true);
                                //app->print(0, "Bottom");
                            }
                        }
                    }
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x0E) { // Ridge
                    if (Ground && Speed >= 4 && x >= sc->objects[o]->x + 8 && x <= sc->objects[o]->x + 0x20 && y >= sc->objects[o]->y - 0x20 && y <= sc->objects[o]->y + 0x20 && flip == 1) {
                        SpringFlip = 2;
                        Action = Actions::Spring;

                        Speed = 12;
                        Gravity = -7;
                        Ground = false;
                        InputAlarm = 30;
                    }
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x6A) { // InvisibleSpikes (copy of Spikes)
                    InvisibleSpikes* obj = (InvisibleSpikes*)sc->objects[o];

                    if (obj->rot % 180 == 90) {
                        ww += 1;
                        hh -= 1;
                    }
                    else {
                        hh += 1;
                        ww -= 1;
                    }
                    wy = ww * dy;
                    hx = hh * dx;

                    if (std::abs(dx) <= ww && std::abs(dy) <= hh){
                        if (wy > hx) {
                            if (wy > -hx && (obj->rot == 0)) {
                                /* collision at the top */
                                scrPlayerHurt((int)sc->objects[o]->x, true);
                                //app->print(0, "Top");
                            }
                            else {
                                /* on the right */
                                if (obj->rot == 270) {
                                    scrPlayerHurt((int)sc->objects[o]->x, true);
                                }
                                //app->print(0, "Left %d", obj->rot);
                            }
                        }
                        else {
                            if (wy > -hx) {
                                /* on the left */
                                if (obj->rot == 90) {
                                    scrPlayerHurt((int)sc->objects[o]->x, true);
                                }
                                //app->print(0, "Right %d", obj->rot);
                            }
                            else if (obj->rot == 180) {
                                /* at the bottom */
                                scrPlayerHurt((int)sc->objects[o]->x, true);
                                //app->print(0, "Bottom");
                            }
                        }
                    }

                    obj = NULL;
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x26) { // Roll Tunnels
                    float flip = sc->objects[o]->flipX ? -1 : 1;
                    if (Speed * flip * Cos[Angle] > 0) {
                        this->ForceRoll = true;
                        $AudioPlay(aud_Roll, 1);
                    }
                    else
                        this->ForceRoll = false;
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x34) { // Starpole
                    sc->objects[o]->doThing();
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x33) { // Button
                    if (Gravity > 1) {
                        sc->objects[o]->doThing();
                        sc->objects[o]->data2 = 0x0;
                    }
                    else {
                        Button* b = (Button*)sc->objects[o];
                        if (b->b == -1) {
                            b->doThing();
                            b->b = 4;
                        }
                        sc->objects[o]->data2 = 0x2;
                    }
                }

                else if ((sc->objects[o]->data1 & 0xFF) == 0x54) { // Air Bubble
                    if (Shield != 3) {
                        sc->objects[o]->doThing();
                        Speed = 0;
                        Gravity = 0;
                        UnderwaterTimer = 1200;
                        animOverrideFrame = 52;
                        animOverride = 32;
                        $AudioPlay(aud_BubbleGet, 3);
                    }
                }

                // CNZ
                else if ((sc->objects[o]->data1 & 0xFF) == 0x41) { // Balloon
                    if (sc->objects[o]->doThing2() == 1) {
                        Gravity = -7;
                        JumpVariable = 0;
                        ShieldUsable = false;
                        Ground = false;
                    }
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x4A) { // Bumper
                    int dis = (int)std::sqrt((this->x - sc->objects[o]->x) * (this->x - sc->objects[o]->x) + (this->y - sc->objects[o]->y) * (this->y - sc->objects[o]->y));
                    if (dis < 16 + sc->objects[o]->r) {
                        sc->objects[o]->doThing();

                        bounceAwayFrom(sc->objects[o]->x, sc->objects[o]->y, 7);
                    }
                }

                else if ((sc->objects[o]->data1 & 0xFF) == 0x3B) { // Gusher
                    sc->objects[o]->doThing();
                }
                else if ((sc->objects[o]->data1 & 0xFF) == 0x3E) { // Conveyor
                    if (this->y + 4 >= sc->objects[o]->y - sc->objects[o]->h / 2.f &&
                        this->y - 4 <  sc->objects[o]->y + sc->objects[o]->h / 2.f) {
                        if (sc->objects[o]->w > 16 && LastGrabY != sc->objects[o]->y && (Gravity > 2 || Gravity == 0 || Action == Actions::Fan || (Underwater && Gravity >= 0))) {
                            if (GrabY != sc->objects[o]->y) {
                                LastGrabY = GrabY;
                            }
                            ConveyorSpeed = 2;
                            if (sc->objects[o]->flipX) {
                                ConveyorSpeed = -2;
                            }
                            Action = Actions::Grab;
                            GrabY = sc->objects[o]->y;
                            Grabbing = true;
                        }
                    }
                }
                else if (sc->objects[o]->controlOnStand) {
                    if (sc->objects[o]->solid || sc->objects[o]->solidTop) {
                        if (Ground && !jump && y < sc->objects[o]->y - sc->objects[o]->h / 2) {
                            int oc = sc->objects[o]->doThing(PlayerID);
                            if (oc > 0)
                                ObjectControlled = oc;
                        }
                    }
                }
                else if (sc->objects[o]->Grabbable) {
                    if (!Ground && !jump && Gravity > 2) {
                        int oc = sc->objects[o]->OnGrabbed(PlayerID);
                        if (oc > 0)
                            ObjectControlled = oc;
                    }
                }
                else if (sc->objects[o]->controlOnTouch) {
                    ObjectControlled = sc->objects[o]->doThing(PlayerID);
                }
                else if (sc->objects[o]->boss) {
                    if (sc->objects[o]->Collidable) {
                        // Getting hurt
                        if (((!(Action == Actions::Jumping || Action == Actions::Rolling || Action == Actions::Spindash))) &&
                            (Invincibility == 0 && Shield != 4) ) {
                            int dis = (int)std::sqrt((this->x - sc->objects[o]->x) * (this->x - sc->objects[o]->x) + (this->y - sc->objects[o]->y) * (this->y - sc->objects[o]->y));
                            if (dis < 16 + 24) {
                                scrPlayerHurt(sc->objects[o]->x, false);
                            }
                        }
                        // Hitting an enemy
                        if (((sc->objects[o]->data1 & 0xFF) != 0x01 && !sc->objects[o]->damaging && (Action == Actions::Jumping || Action == Actions::Rolling || Action == Actions::Spindash)) ||
                            (Invincibility > 1) ||
                            (Shield == 4)) {
                            int dis = (int)std::sqrt((this->x - sc->objects[o]->x) * (this->x - sc->objects[o]->x) + (this->y - sc->objects[o]->y) * (this->y - sc->objects[o]->y));

                            if ((dis < 16 + 24 && Shield != 4) ||
                                (dis < 16 + 24 + 8 && Shield == 4)) {
                                sc->objects[o]->doThing();

                                if (!Ground) {
                                    bounceAwayFrom(sc->objects[o]->x, sc->objects[o]->y, 3);
                                }
                                else {
                                    this->Speed = this->Speed * -1.0f;
                                    if (std::abs(this->Speed) < 1) {
                                        this->Speed = signum(this->Speed) * 1;
                                    }
                                }

                                /*if (!Ground) {
                                    //this->Gravity = this->Gravity * -1.0f;
                                    this->Gravity = std::abs(this->Gravity) * signum(this->y - sc->objects[o]->y);
                                    this->Speed = this->Speed * -1.0f;
                                    if (std::abs(this->Speed) < 1) {
                                        this->Speed = signum(this->Speed) * 1;
                                    }
                                    if (!jumpHold)
                                        this->Gravity = std::max(this->Gravity, -4.0f);
                                }
                                else {
                                    this->Speed = this->Speed * -1.0f;
                                    if (std::abs(this->Speed) < 1) {
                                        this->Speed = signum(this->Speed) * 1;
                                    }
                                }*/

                                Vibrate(0, VIBRATE_IMPACT);
                            }
                        }
                    }
                }
                else if (sc->objects[o]->enemy) {
                    if (!sc->objects[o]->Collidable)
                        continue;

                    // Getting hurt
                    if ((sc->objects[o]->damaging ||
                        (sc->objects[o]->enemy && !(Action == Actions::Jumping || Action == Actions::Rolling || Action == Actions::Spindash))) &&
                        (Invincibility == 0 && Shield != 4) ) {
                        if (this->x + 16 >= sc->objects[o]->x - sc->objects[o]->w / 2.f &&
                            this->y + 16 >= sc->objects[o]->y - sc->objects[o]->h / 2.f &&
                            this->x - 16 <  sc->objects[o]->x + sc->objects[o]->w / 2.f &&
                            this->y - 16 <  sc->objects[o]->y + sc->objects[o]->h / 2.f) {
                            scrPlayerHurt(sc->objects[o]->x, false);
                            sc->objects[o]->doThing();
                        }
                    }
                    // Hitting an enemy
                    if ((sc->objects[o]->poppable && (sc->objects[o]->data1 & 0xFF) != 0x01 && !sc->objects[o]->damaging && (Action == Actions::Jumping || Action == Actions::Rolling || Action == Actions::Spindash)) ||
                        (sc->objects[o]->enemy && Invincibility > 1) ||
                        (sc->objects[o]->enemy && Shield == 4)) {
                        if ((this->x + 16 >= sc->objects[o]->x - sc->objects[o]->w / 2.f &&
                            this->y + 16 >= sc->objects[o]->y - sc->objects[o]->h / 2.f &&
                            this->x - 16 <  sc->objects[o]->x + sc->objects[o]->w / 2.f &&
                            this->y - 16 <  sc->objects[o]->y + sc->objects[o]->h / 2.f && Shield != 4) ||
                            (this->x + 24 >= sc->objects[o]->x - sc->objects[o]->w / 2.f &&
                            this->y + 24 >= sc->objects[o]->y - sc->objects[o]->h / 2.f &&
                            this->x - 24 <  sc->objects[o]->x + sc->objects[o]->w / 2.f &&
                            this->y - 24 <  sc->objects[o]->y + sc->objects[o]->h / 2.f && Shield == 4)) {
                            sc->objects[o]->doThing();

                            if (this->y < sc->objects[o]->y) {
                                this->Gravity = this->Gravity * -1.0f;
                                if (!jumpHold)
                                    this->Gravity = std::max(this->Gravity, -1.0f);
                            }
                            else
                                this->Gravity -= signum(this->Gravity);


                            enemyCombo++;
                            sc->givePoint(0, enemyCombo, sc->objects[o]->x, sc->objects[o]->y);


                            $AudioPlay(aud_BadnikDestroyed, 3);
                            Vibrate(0, VIBRATE_IMPACT);
                        }
                    }
                }
                else if (sc->objects[o]->Collidable) {
                    sc->objects[o]->OnCollisionWithPlayer(PlayerID, Speed, Gravity);
                }
                else {
                    sc->objects[o]->doThing(PlayerID);
                }
            }

            if ((sc->objects[o]->data1 & 0xFF) == 0x37) {
                Objectx37* obj = (Objectx37*)sc->objects[o];
                if (!obj->solid) {
                    /*if (this->x > obj->x - 192 && this->x < obj->x + 64 && this->y > obj->y - 128 && this->y < obj->y + 128 &&
                        this->Action != Actions::Hurt && this->Action != Actions::InStreamGrab && this->Action != Actions::InStream) {
                        this->Action = Actions::InStream;
                    }
                    if (this->x >= obj->x + 32 && this->x <= obj->x + 64 + 64 && this->y > obj->y - 128 && this->y < obj->y + 128 &&
                        this->Action == Actions::InStream) {
                        //this->Action = Actions::Hurt;
                    }*/
                }
            }
            else if ((sc->objects[o]->data1 & 0xFF) == 0x3B) {
                /*Objectx3B* obj = (Objectx3B*)sc->objects[o];
                if (this->x > obj->x - 48 && this->x < obj->x + 48 && this->y > obj->y - 624 && this->y < obj->y + 624 && obj->data2 == 0x01 && this->Action == Actions::InStream) {
                    this->Action = Actions::InStreamPipe;
                }
                if (this->x > obj->x - 48 && this->x < obj->x + 48 && this->y > obj->y && this->y < obj->y + 624 && obj->data2 == 0x01 && this->Action == Actions::InStreamPipe) {
                    this->x = obj->x;
                }
                if (this->x >= obj->x - 48 && this->x <= obj->x + 48 && this->y > obj->y - 624 - 64 && this->y < obj->y - 624 && this->Action == Actions::InStreamPipe) {
                    //this->Action == Actions::InStream) {
                    this->Action = Actions::Spring;
                    this->Gravity = -16;
                }*/
            }
            else if ((sc->objects[o]->data1 & 0xFF) == 0x38) { // Fan
                int h = (sc->objects[o]->data2 & 0x0F) * 0x10 + 128;

                class Fan* objFan = (class Fan*)sc->objects[o];

                if (objFan->on) {
                    if (this->y <=  sc->objects[o]->y) {
                        objFan->out = true;
                    }
                }
                else {
                    if (this->y <=  sc->objects[o]->y - 32) {
                        objFan->out = true;
                    }
                }

                if (this->Action != Actions::Hurt && objFan->on &&
                    this->x + 10 >= sc->objects[o]->x - sc->objects[o]->w / 2.f &&
                    this->y + 00 >= sc->objects[o]->y - h - 12 &&
                    this->x - 10 <  sc->objects[o]->x + sc->objects[o]->w / 2.f &&
                    this->y - 00 <=  sc->objects[o]->y - 16) {

                    if (Action != Actions::Fan)
                        SpringFlip = 1;
                    if (this->Action != Actions::Grab) {
                        if (!Underwater)
                            this->Gravity = std::max(this->Gravity - 2.f, -3.0f);
                        else
                            this->Gravity = std::max(this->Gravity - 0.25f, -1.0f);
                        this->Ground = false;
                        if (Action != Actions::Fan)
                            SpringFlip = 0;
                        this->Action = Actions::Fan;
                    }
                    this->Fan = true;
                }
            }
            else if ((sc->objects[o]->data1 & 0xFF) == 0x46) { // CNZ_Fan
                int h = 16;

                //CNZ_Fan* objFan = (CNZ_Fan*)sc->objects[o];

                if (this->x + 10 >= sc->objects[o]->x - sc->objects[o]->w / 2.f &&
                    this->y + 00 >= sc->objects[o]->y - h - 16 &&
                    this->x - 10 <  sc->objects[o]->x + sc->objects[o]->w / 2.f &&
                    this->y - 00 <=  sc->objects[o]->y + 16) {

                    if (Action != Actions::Fan)
                        SpringFlip = 1;
                    if (this->Action != Actions::Grab) {
                        if (!Underwater)
                            this->Gravity = std::max(this->Gravity - 2.f, -3.0f);
                        else
                            this->Gravity = std::max(this->Gravity - 0.25f, -1.0f);
                        this->Ground = false;
                        if (Action != Actions::Fan)
                            SpringFlip = 0;
                        this->Action = Actions::Fan;
                    }
                    this->Fan = true;
                }
            }
            else if ((sc->objects[o]->data1 & 0xFF) == 0x39) { // LargeFan
                LargeFan* objFan = (LargeFan*)sc->objects[o];
                if (this->x >= objFan->x - 48) {
                    objFan->out = true;
                }
                else {
                    objFan->out = false;
                }
            }
        }
    }
    for (unsigned int d = 0; d < 64; d++) {
        if ((Invincibility == 0 && Shield != 4) && sc->damages[d].Active && sc->damages[d].Radius < 128 &&
            this->x + 13 + std::abs(Cos[AngleMode * 90]) + (Speed * Cos[AngleMode * 90]) >= sc->damages[d].X - sc->damages[d].Radius &&
            this->y + 22 >= sc->damages[d].Y - sc->damages[d].Radius &&
            this->x - 13 - std::abs(Cos[AngleMode * 90]) + (Speed * Cos[AngleMode * 90]) <  sc->damages[d].X + sc->damages[d].Radius &&
            this->y - 22 + (Action == Actions::CrouchDown ? 16 : 0) <  sc->damages[d].Y + sc->damages[d].Radius) {
            scrPlayerHurt(sc->damages[d].X, false);
        }
    }

    if (!Grabbing && Action == Actions::Grab && ObjectControlled == 0) {
        LastGrabY = GrabY;
        Speed = 0;
        Action = Actions::Jumping;
        JumpVariable = 1;
        GrabTimer = -1;
    }
}
void Player::scrPlayerWallCollision() {
    int InitAngle = scrWrapAngle(this->AngleMode * 90);

    int len = 12;

    int hei = 8;

    float X = this->x + hei * this->Sin[InitAngle];
    float Y = this->y + hei * this->Cos[InitAngle];

    float X2 = X + len * this->Cos[InitAngle];
    float Y2 = Y - len * this->Sin[InitAngle];

    S3Object::Terrain Collide = collision_line_terrain(X, Y, X2, Y2);
    if ((Collide.Active && (Collide.Layer == this->Layer || Collide.Layer == -1) && Collide.Platform != true) && this->Speed >= 0) {
        scrPushPlayerX_Id(Collide);
        this->Speed = 0;
    }

    X = this->x + hei * this->Sin[InitAngle];
    Y = this->y + hei * this->Cos[InitAngle];

    X2 = X - len * this->Cos[InitAngle];
    Y2 = Y + len * this->Sin[InitAngle];
    Collide = collision_line_terrain(X, Y, X2, Y2);
    if ((Collide.Active && (Collide.Layer == this->Layer || Collide.Layer == -1) && Collide.Platform != true) && this->Speed <= 0) {
        scrPushPlayerX_Id(Collide);
        this->Speed = 0;
    }


    if (this->Ground == false) {
        X = this->x - 7 * this->Sin[InitAngle];
        Y = this->y - 7 * this->Cos[InitAngle];

        X2 = X + len * this->Cos[InitAngle];
        Y2 = Y - len * this->Sin[InitAngle];
        Collide = collision_line_terrain(X, Y, X2, Y2);
        if ((Collide.Active && (Collide.Layer == this->Layer || Collide.Layer == -1) && Collide.Platform != true) && this->Speed >= 0) {
            scrPushPlayerX_Id(Collide);
            this->Speed = 0;
        }

        X = this->x - 7 * this->Sin[InitAngle];
        Y = this->y - 7 * this->Cos[InitAngle];

        X2 = X - len * this->Cos[InitAngle];
        Y2 = Y + len * this->Sin[InitAngle];
        Collide = collision_line_terrain(X, Y, X2, Y2);
        if ((Collide.Active && (Collide.Layer == this->Layer || Collide.Layer == -1) && Collide.Platform != true) && this->Speed <= 0) {
            scrPushPlayerX_Id(Collide);
            this->Speed = 0;
            //app->print(0, "Speeeeeed %f", Speed);
        }
    }
}
void Player::scrPlayerSpeedMovement() {
    float SpeedSplit = 1;
    float SpeedValue = 0;

    int objParentTerrain = 0;

    Speed = clamp(Speed, -128, 128);
    SplitSpeedAmount = 8;

    SpeedSplit = std::floor(std::max(std::abs((Speed * SpeedMultiplier) / SplitSpeedAmount), 1.0f));

    if (SpeedSplit > 0)
        SpeedValue = (Speed * SpeedMultiplier) / SpeedSplit;
    else
        SpeedValue = 0;

    float SpeedMax = std::abs(Speed) * SpeedMultiplier;

	for (int i = 0; i < std::ceil(SpeedSplit); i++) {
		if (Speed == 0)
            SpeedValue = 0;

        SpeedValue = std::min(SpeedMax, std::abs(SpeedValue)) * signum(SpeedValue);
        SpeedMax -= SpeedValue;

        if (scrPlayerHandleBreakableTerrain())
            break;

        x += SpeedValue * Cos[Angle];
        y -= SpeedValue * Sin[Angle];

        if (Action == Actions::InStreamPipe) continue;

        scrPlayerHandlePathSwitchers();

        if (this->CollisionPushMode == 0) {
            scrPushPlayerX_Id(S3Object::Terrain { false, -1, true });
            scrPushPlayerY();
        }
        else {
            //scrPushPlayerSafe();
        }


        if (this->Ground == true) {
            this->TerrainIdLast = this->TerrainId;
            scrCheckGroundLevel(); // - Check ground position.
            scrPushPlayerY();
            this->AngleLast2 = this->AngleLast;
            this->AngleLast = this->Angle;

            this->Angle = scrCheckAngle();

            if ((this->Angle > 315 || this->Angle < 45) && this->AngleMode != 0) {
                this->AngleModeLast = this->AngleMode;
                this->AngleMode = 0;

                if (this->Angle360Detection == false &&
                    this->AngleModeLast != 0 &&
                    (!scrPlayerCollisionASensor(objParentTerrain, 0) || !scrPlayerCollisionBSensor(objParentTerrain, 0)))
                    this->AngleMode = this->AngleModeLast;
                else if (this->AngleBothSensors == false ||
                    std::abs(angle_difference(this->AngleMode * 90, this->Angle)) > 10 ||
                    (scrPlayerCollisionASensor360(objParentTerrain, 0) && scrPlayerCollisionBSensor360(objParentTerrain, 0)))
                    this->Angle = scrCheckAngle();
            }
            else if ((this->Angle > 45 && this->Angle < 135) && this->AngleMode != 1) {
                this->AngleModeLast = this->AngleMode;
                this->AngleMode = 1;
                if (this->Angle360Detection == false && (!scrPlayerCollisionASensor(objParentTerrain, 0)) && this->AngleModeLast != 1) {
                    this->AngleMode = this->AngleModeLast;
                    this->Angle = this->AngleLast;
                }
                else if (this->Speed < 0 && this->AngleMode == 0 && this->AngleModeSwitchMode == 1) {
                    this->x -= 12;
                    this->x = std::floor(this->x);
                }
                else if ((this->Speed > 0 || this->AngleMode != 0 || this->AngleModeSwitchMode == 1 || (this->AngleLast < 45 && this->AngleLast > 0))) {
                    this->AngleMode = 1;
                    if (this->AngleBothSensors == false || std::abs(angle_difference(this->AngleMode * 90, this->Angle)) > 10 ||
                    (scrPlayerCollisionASensor360(objParentTerrain, 0) && scrPlayerCollisionBSensor360(objParentTerrain, 0)))
                        this->Angle = scrCheckAngle();
                }
                else {
                    this->AngleMode = this->AngleModeLast;
                    this->Angle = this->AngleLast;
                }

            }
            else if ((this->Angle > 135 && this->Angle < 225) && this->AngleMode != 2) {
                this->AngleModeLast = this->AngleMode;
                this->AngleMode = 2;
                if (this->Angle360Detection == false && this->AngleModeLast != 2 &&
                    (!scrPlayerCollisionASensor(objParentTerrain, 0) || !scrPlayerCollisionBSensor(objParentTerrain, 0)))
                    this->AngleMode = this->AngleModeLast;
                else if (this->AngleBothSensors == false ||
                    std::abs(angle_difference(this->AngleMode * 90, this->Angle)) > 10 ||
                    (scrPlayerCollisionASensor360(objParentTerrain, 0) && scrPlayerCollisionBSensor360(objParentTerrain, 0)))
                    this->Angle = scrCheckAngle();
            }
            else if ((this->Angle > 225 && this->Angle < 315) && this->AngleMode != 3) {
                this->AngleModeLast = this->AngleMode;
                this->AngleMode = 3;
                if (this->Angle360Detection == false && this->AngleModeLast != 3 &&
                    (!scrPlayerCollisionASensor(objParentTerrain, 0) || !scrPlayerCollisionBSensor(objParentTerrain, 0)))
                    this->AngleMode = this->AngleModeLast;
                else if (this->Speed > 0 && this->AngleMode == 0 && this->AngleModeSwitchMode == 1) {
                    this->x += 12;
                    this->x = std::ceil(this->x);
                    this->y += 12;
                }
                else if (this->Speed < 0 || this->AngleMode != 0 || this->AngleModeSwitchMode == 1 || ((this->AngleLast > 315 && this->AngleLast < 360))) {
                    this->AngleMode = 3;
                    if (this->AngleBothSensors == false ||
                        std::abs(angle_difference(this->AngleMode * 90, this->Angle)) > 10 ||
                        (scrPlayerCollisionASensor360(objParentTerrain, 0) && scrPlayerCollisionBSensor360(objParentTerrain, 0)))
                        this->Angle = scrCheckAngle();
                }
                else
                    this->AngleMode = this->AngleModeLast;
            }
        }

        scrPlayerWallCollision();


        if (Ground &&
            (!scrPlayerCollisionASensor(objParentTerrain, 0) && !scrPlayerCollisionBSensor(objParentTerrain, 0))
            && this->ForceRoll == false) {
            //if (this->TerrainId.Active && this->TerrainId.angle != 0 && (this->AngleMode == 1 || this->AngleMode == 3))
                //this->Angle = scrWrapAngle(this->TerrainId.angle);

            //app->print(0, "No ground detected, falling off. Angle: %d, AngleMode: %d", (int)this->Angle, this->AngleMode);
            Gravity = -Speed * Sin[Angle];
            Speed = Speed * Cos[Angle];
            if (this->Action == Actions::Rolling || down) {
                this->x += (4 * Sin[Angle]);
                //this->y += (4 * Cos[Angle]);
            }
            Angle = 0;
            AngleMode = 0;
            Ground = false;
            TerrainId = { false };
       }

       //if (i == 0)
           //scrPlayerHandleObjects();
    }

    if ((this->Angle >= 45 && this->Angle <= 315) && std::abs(this->Speed) < 2.5 && this->ForceRoll == false) {
        if (this->Angle < 90 || this->Angle > 270) {
            this->InputAlarm  = 30;
        }
        else {
            this->Gravity = -this->Sin[this->Angle] * this->Speed;
            this->Speed = this->Cos[this->Angle] * this->Speed;
            this->Ground = false;
            if (this->Action == Actions::Rolling) {
                this->x += (4 * this->Sin[this->Angle]);
                this->y += (4 * this->Cos[this->Angle]);
            }
            this->Angle = 0;
            this->TerrainId = {false};
            this->AngleMode = 0;
            this->InputAlarm = 30;
        }
    }
}
void Player::scrPlayerGravityMovement() {
    //this->Gravity = std::round(this->Gravity * 16) / 16;
    //this->Gravity += standSpdY;
    this->standSpdY = 0;

    float GravitySplit = 1.0f;
    float GravityValue = 0.0f;
    S3Object::Terrain Object;
    S3Object::Terrain Object2;
    S3Object::Terrain Object3;
    if (this->Ground)
        return;

    float split = 2.0f;
    if (app->platform == Application::P_Vita) {
        split = 12.0f;
    }

    this->Gravity = clamp(this->Gravity, -this->GravityMaxi, this->GravityMaxi);
    GravitySplit = std::floor(std::max(std::abs((this->Gravity * this->SpeedMultiplier)) / split, 1.0f));

    if (GravitySplit > 0)
        GravityValue = (this->Gravity * this->SpeedMultiplier) / (GravitySplit); // - Split speed so we can check collisions more precisely.
    else
        GravityValue = 0;
    float GravityMax = this->Gravity * this->SpeedMultiplier;

    int Length = 0;
    int objParentTerrain = 0;

    for (int i = 0; i < std::ceil(GravitySplit); i++) {
        if (GravityMax == 0 && this->Gravity != 0)
            break;
        GravityValue = std::min(std::abs(GravityMax), std::abs(GravityValue)) * signum(GravityValue);
        GravityMax = std::max(std::abs(GravityMax) - std::abs(GravityValue), 0.0f) * signum(GravityValue);

        this->y += GravityValue; // - Increase y by speed and angle.
        //scrPlayerHandleMonitors();

        if (Action == Actions::InStreamPipe) continue;

        if (Action != Actions::InStreamPipe) {
            //scrPlayerHandleBreakableTerrain();
            scrPlayerWallCollision();

            scrPushPlayerX_Id({ false, 0, true });
            scrPushPlayerYDown();
        }


        if (Action == Actions::Jumping)
            Length = 10;
        else
            Length = 20;

        Object = collision_line_terrain(this->x - 9, this->y - Length, this->x - 9, this->y);
        if ((!Object.Active || (Object.Layer != this->Layer && Object.Layer != -1) || (Object.Platform == true)))
            Object2 = collision_line_terrain(this->x + 9, this->y - Length, this->x + 9, this->y);
        if ((!Object2.Active || (Object2.Layer != this->Layer && Object2.Layer != -1) || (Object2.Platform == true)))
            Object3 = collision_line_terrain(this->x, this->y - Length, this->x, this->y);

        if (this->Gravity >= 0 &&
            (scrPlayerCollisionASensor(objParentTerrain, -16) || scrPlayerCollisionBSensor(objParentTerrain, -16)) &&
            !Object.Active && !Object2.Active && !Object3.Active) {
            if (this->ShieldAction == true && this->Shield == 3) {
                this->Gravity = -7.5;
                this->Ground = false;
                this->ShieldAction = false;
                this->ShieldUsable = true;
                Vibrate(0, 0.5, 250);
                scrPushPlayerY();
                return;
            }
            else {
                //scrPlayerResetTrail();
                scrPushPlayerYLand();
                this->Ground = true;
                this->EnemyChain = 0;
                this->Landed = true;
                this->Angle = 0;
                this->ShieldAction = false;
                this->AngleMode = 0;
                scrCheckGroundLevel();
                scrPushPlayerX();

                this->ShieldUsable = true;


                this->Angle = scrCheckAngle();

                if ((this->Angle > 315 || this->Angle < 45))
                    this->AngleMode = 0;
                else if ((this->Angle > 45 && this->Angle < 135))
                    this->AngleMode = 1;
                else if ((this->Angle > 135 && this->Angle < 225))
                    this->AngleMode = 2;
                else if ((this->Angle > 225 && this->Angle < 315))
                    this->AngleMode = 3;

                if (std::abs(this->Speed) <= std::abs(this->Gravity) && this->Angle >= 22.5f && this->Angle <= 337.5f/* && TerrainId.object_index != objObjectSeesaw*/) {
                    this->Speed = -this->Gravity * signum(this->Sin[this->Angle]);
                    if (this->Angle < 45 || this->Angle > 315)
                        this->Speed *= 0.5f;
                    /*else if (TerrainId.object_index == objObjectSeesaw)
                        this->Angle = 0;*/
                }
                if (ObjectControlled == 0 && std::abs(this->Speed) > 0.5f && down && this->Ground &&
                    (this->RollMoveLock == false ||(right == false && left == false)) &&
                    (this->Action == Actions::Normal || this->Action == Actions::Jumping)) {
                    //scrPlayerResetTrail();
                    //audio_play_sound(sndPlayerRoll, 1, false);
                    //this->Action = Actions::Rolling;
                }
                if (this->Action == Actions::Hurt) {
                    this->Invincibility = 1;
                    this->InvincibilityTimer = 120;
                    this->Action = Actions::Normal;
                }
                if (this->Action == Actions::Peril) {
                    this->Action = Actions::Normal;
                }

                this->Gravity = 0;
                GravityMax = 0;

                return;
            }
        }

        if ((Object.Active && (Object.Layer == this->Layer || Object.Layer == -1) && (Object.Platform != true)) ||
            (Object2.Active && (Object2.Layer == this->Layer || Object2.Layer == -1) && (Object2.Platform != true)) ||
            (Object3.Active && (Object3.Layer == this->Layer || Object3.Layer == -1) && (Object3.Platform != true))) {
            float Y = this->y;
            this->AngleMode = 2;

            if (this->Gravity < -0.5f) {
                this->Angle = 180;
                this->Ground = true;
                this->Landed = true;
                scrCheckGroundLevel();
                scrPushPlayerX_Id({false,0,true});
                scrPushPlayerY();
                this->Angle = scrCheckAngle();

                if ((
                    (this->Angle > 90 && this->Angle <= 135) ||
                    (this->Angle >= 225 && this->Angle < 270)) &&
                    this->Gravity < -1.5f && /*TerrainId != noone &&*/
                    (this->Action == Actions::Normal ||
                    this->Action == Actions::Jumping ||
                    this->Action == Actions::Spring ||
                    this->Action == Actions::Fan ||
                    this->Action == Actions::Rolling)) {
                    //scrPlayerResetTrail();
                    if ((this->Angle > 45 && this->Angle < 135))
                        this->AngleMode = 1;
                    else if ((this->Angle > 135 && this->Angle < 225))
                        this->AngleMode = 2;
                    else if ((this->Angle > 225 && this->Angle < 315))
                        this->AngleMode = 3;

                    this->Ground = true;
                    this->Speed = -this->Gravity * signum(this->Sin[this->Angle]);

                    this->Gravity = 0;
                    this->EnemyChain = 0;

                    GravityMax = 0;

                    if (down == true && (this->Action == Actions::Normal || this->Action == Actions::Jumping)) {
                        //scrPlayerResetTrail();
                        $AudioPlay(aud_Roll, 1);
                        Action = Actions::Rolling;
                    }
                    return;
                }
                else {
                    if (this->Gravity < 0)
                        this->Gravity = 0;

                    this->TerrainId     = {false};
                    this->Ground = false;
                    this->Angle = 0;
                    this->Landed = false;
                    this->AngleMode = 0;

                    scrPushPlayerX_Id({false, 0, true});

                    this->y = Y;
                    scrPushPlayerYDown();

                    this->TerrainId     = {false};
                    GravityMax = 0;
                    return;
                }
            }
            else {
                if (this->Gravity < 0)
                    this->Gravity = 0;

                this->TerrainId     = {false};
                this->Ground = false;
                this->Angle = 0;
                this->Landed = false;
                this->AngleMode = 0;

                scrPushPlayerX_Id({false, 0, true});
                scrPushPlayerYDown();
                this->TerrainId     = {false};
                GravityMax = 0;
                return;
            }
        }

        //scrPlayerHandleSprings();
        //scrPlayerHandleMonitors();
    }
}

void Player::scrCreateRingLoss() {
    if (!Controllable) return;

    int RingTotal = std::min((int)sc->rings, 32);
    float RingAngle = 101.25f;
    float RingSpeed = 4;

    $AudioPlay(aud_C_RingLoss, 3);

    while (RingTotal > 0) {
        Obj_Ring* ring = new Obj_Ring(0);
        ring->x = x - 8;
        ring->y = y - 16;
        ring->data1 = 00;
        ring->sc = sc;
        ring->app = app;
        ring->prior = true;
        //Ring.Alarm = 64;

        ring->Gravity    = -std::sin(toRadians(std::round(RingAngle))) * RingSpeed;
        ring->Speed      =  std::cos(toRadians(std::round(RingAngle))) * RingSpeed;
        //Ring.Motion     = true;
        if (RingTotal % 2 != 0) {
            RingAngle += 22.5;
            ring->Speed *= -1;
        }
        RingTotal -= 1;
        if (RingTotal == 16) {
            RingSpeed = 2;
            RingAngle = 101.25;
        }

        sc->objects[sc->objects_count] = ring;
        sc->objects_count++;
    }
    sc->rings = 0;
}
void Player::scrPlayerJump() {
    float GSpeed = this->Speed;
    this->Speed    = (this->Cos[this->Angle] * GSpeed) + (this->Sin[this->Angle] * (this->JumpHeight + (0.5 * (CharacterId == consCharacterKnuckles))));
    this->Gravity = -(this->Sin[this->Angle] * GSpeed) + (this->Cos[this->Angle] * (this->JumpHeight + (0.5 * (CharacterId == consCharacterKnuckles))));
    this->Ground = false;
    this->JumpVariable = 1;
    this->Action = Actions::Jumping;

    $AudioPlay(aud_Jump, 1);

    this->Angle = 0;
    this->FloorAngle  = 0;
    this->AngleMode   = 0;
    this->InputAlarm  = 0;
    this->JumpAnimSpeed = 1.0f / std::max((5.0f - std::abs(this->Speed)), 1.0f);
}
void Player::scrPlayerHurt(int x, bool spike) {
    if (Action == Actions::Hurt || Action == Actions::Dead || Invincibility > 0 || SuperForm || Action == Actions::Transform)
        return;

    Vibrate(0, 0.75, 450);
    ObjectControlled = 0;

    if (Shield <= 0 || Shield == 4) {
        if (sc->rings > 0) {
            Action = Actions::Hurt;
            if (scrPlayerCollisionCSensor(objParentTerrain, 6) == false && scrPlayerCollisionDSensor(objParentTerrain, 6) == false)
                Gravity = -4;
            else
                Gravity = 0;
            Ground = false;
            Speed = 2 * signum(this->x - x);
            if (Speed == 0) Speed = -2;

            scrCreateRingLoss();
            if (!spike)
                $AudioPlay(aud_Spiked, 1); //$AudioPlay(aud_RingLoss, 1);
            else
                $AudioPlay(aud_Spiked, 1);
        }
        else {
            scrPlayerDead();
            $AudioPlay(aud_Die, 1);
        }
    }
    else {
        Shield = 0;
        Action = Actions::Hurt;
        if (!spike)
            $AudioPlay(aud_Spiked, 1); //$AudioPlay(aud_Death, 1);
        else
            $AudioPlay(aud_Spiked, 1);
        Gravity = -4;
        Ground = false;
        Speed = 2 * signum(this->x - x);
    }
}
void Player::scrPlayerDead() {
    Action             = Actions::Dead;
    Speed              = 0;
    Angle              = 0;
    AngleMode          = 0;
    Gravity            = -7;

    if (!Controllable) return;

    sc->lives--;
}

void Player::scrPlayerActionPush() {
    if (
        (this->flip == 1.0f && (right == false || scrPlayerCollisionRightSensor(objParentTerrain) == false)) ||
        (this->flip == -1.0f && (left == false || scrPlayerCollisionLeftSensor(objParentTerrain) == false)) || this->Ground == false) {
        //audio_stop_sound(sndPush1);
        this->Action = Actions::Normal;
    }
    else if (jump && this->Ground == true) {
        //audio_stop_sound(sndPush1);
        scrPlayerJump();
    }
    else {
        S3Object::Terrain _ObjectHandle = collision_line_terrain(this->x, this->y + 10, this->x + (14 * flip), this->y + 10);
        if (_ObjectHandle.Active && PlayerID == 0) {
            if (sc->objects[_ObjectHandle.ObjectID] != NULL) {
                if (sc->objects[_ObjectHandle.ObjectID]->Pushable) {
                    this->x += flip * 0.25f;
                    sc->objects[_ObjectHandle.ObjectID]->x += flip * 0.25f;
                }
            }
            /*with (_ObjectHandle) {
                if (other.image_xscale == 1 && collision_point(bbox_right, other.y+4, objParentTerrain, true, true).Active == false && Ground == true) {
                    if (audio_is_playing(sndPush1) == false)
                        audio_play_sound(sndPush1, 1, true);
                    x    += 0.25;
                    other.x           = floor(x-11);
                }else if (other.image_xscale == -1 && collision_point(bbox_left-1, other.y+4, objParentTerrain, true, true).Active == false && Ground == true) {
                    if (audio_is_playing(sndPush1) == false)
                        audio_play_sound(sndPush1, 1, true);
                    x    -= 0.25;
                    other.x           = floor(x+sprite_width+11);
                }else
                    audio_stop_sound(sndPush1);
            }*/
        }
    }
}
void Player::scrPlayerActionNormal() {
    if (jump && this->Ground == true) {
        scrPlayerJump();
    }

    if (ObjectControlled > 0) return;

    if (this->Ground == true &&
        this->Speed == 0 &&
        this->AngleMode == 0 &&
        (scrPlayerCollisionRightSensor(objParentTerrain) == true && right))
        this->Action = Actions::Push;
    else if (this->Ground == true &&
        this->Speed == 0 &&
        this->AngleMode == 0 &&
        (scrPlayerCollisionLeftSensor(objParentTerrain) == true && left))
        this->Action = Actions::Push;

    else if (this->Ground == true && this->AngleMode == 0 && this->InputAlarm <= 0 && ((this->Speed >= 4.5 && left) || (this->Speed <= -4.5 && right))) {
        this->Action = Actions::Skid;
        $AudioPlay(aud_Skid, 0);
        //audio_play_sound(sndPlayerSkid, 1, false);
    }
    else if (this->Ground == true && this->InputAlarm <= 0 && ((this->Speed < 0 && right) || (this->Speed > 0 && left))) {
            this->Speed = std::max(std::abs(this->Speed)- this->SkidRate, 0.0f) * signum(this->Speed);
    }
    else if (std::abs(this->Speed) <= (this->Sonic3RollReq ? 1.03125f : 0.53125f) && down && this->Ground) {
        this->Speed = 0;
        this->LookTimer = 0;
        this->Action = Actions::CrouchDown;
    }
    else if (std::abs(this->Speed) > (this->Sonic3RollReq ? 1.03125f : 0.53125f) && down && this->Ground && (!this->RollMoveLock || (right == false && left == false))) {
        $AudioPlay(aud_Roll, 1);
        this->Action = Actions::Rolling;
    }
}
void Player::scrPlayerActionRolling() {
    if (ObjectControlled > 0) return;

    if (this->Speed > 0)
        this->Speed = std::max(0.0f, this->Speed - (AccelRate / 2.0f));
    else if (this->Speed < 0)
        this->Speed = std::min(0.0f, this->Speed + (AccelRate / 2.0f));

    if (this->CharacterId != consCharacterTails && this->ForceRoll == false && this->InputAlarm <= 0) {
        if (left && this->Speed > 0)
           this->Speed = std::max(0.0f, this->Speed - 0.125f);
        if (right && this->Speed < 0)
           this->Speed = std::min(0.0f, this->Speed + 0.125f);
    }
    else if (InputAlarm <= 0) {
        if (left && Speed > 0)
           this->Speed = std::max(0.0f, this->Speed - (this->SkidRate / 4.0f));

        if (right && Speed < 0)
           this->Speed = std::min(0.0f, this->Speed + (this->SkidRate / 4.0f));
    }
    if (this->Ground == false && this->ForceRoll == false) {
        this->Action         = Actions::Jumping;
        this->JumpAnimSpeed  = 1.0f / std::max((5.0f - std::abs(this->Speed)), 1.0f);
        this->JumpVariable   = false;
    }
    if (jump && this->Ground == true && this->ForceRoll == false) {
        scrPlayerJump();
        this->JumpVariable = -1;
    }
    scrPlayerHandleMonitors();
    if (this->Speed == 0 && this->Ground) {
        if (this->ForceRoll == false)
            this->Action = Actions::Normal;
        else {
            $AudioPlay(aud_Roll, 1);
            this->Speed = 4.0f;
        }
    }
    if (this->FlagRollSpeedCap == true || true)
        this->Speed = std::min(std::abs(this->Speed), 32.0f) * signum(this->Speed);

}
void Player::scrPlayerActionSkid() {
    if (this->Speed > 0 && left) {
        this->Speed = std::max(this->Speed - this->SkidRate, 0.0f);

        if (fcount % 5 == 0) {
            sc->particles[sc->part_next++ % sc->part_max] = new Particle(tex_SkidDust, x, y + 13, 2 * 4, 2 * 4, 4, true);
            Vibrate(0, 0.25, 50);
        }
    }
    else if (this->Speed < 0 && right) {
        this->Speed = std::min(this->Speed + this->SkidRate, 0.0f);

        if (fcount % 5 == 0) {
            sc->particles[sc->part_next++ % sc->part_max] = new Particle(tex_SkidDust, x, y + 13, 2 * 4, 2 * 4, 4, true);
            Vibrate(0, 0.25, 50);
        }
    }
    else if (this->Speed != 0)
        this->Speed -= std::min(std::abs(this->Speed), this->Acceleration) * signum(this->Speed);

    if ((left && this->Speed < 0) || (right && this->Speed > 0) || this->Speed == 0) {
        Action = Actions::Normal;
    }
    else if (jump && this->Ground == true) {
        scrPlayerJump();
    }
}
void Player::scrPlayerActionJumping() {
    if (jumpHold == false && this->Gravity < this->JumpRelease && this->JumpVariable == 1)
        this->Gravity = this->JumpRelease;

    scrPlayerHandleMonitors();
    if (this->Ground == true) {
        this->ShieldUsable = true;
        this->ShieldAction = false;
        if (DropDashRev >= 21 && jumpHold) {
            $AudioPlay(aud_Roll, 1);
            Action = Actions::Rolling;
            if (SuperForm)
                Speed = flip * 12;
            else
                Speed = flip * 8;
        }
        else {
            Action = Actions::Normal;
        }
        DropDashRev = 0;
    }
    else if (jump && ShieldUsable == true && Shield != 5 && CharacterId == consCharacterSonic && (!SuperForm || UseShieldSuper || DropDashEnabled)) {
        ShieldAnimation = 0;
        switch (Shield) {
            case 0:
                if (DropDashEnabled) {
                    if (this->JumpVariable < 0)
                        this->JumpVariable = 0;

                    if (DropDashRev == 0)
                        DropDashRev = 1;
                    $AudioPlay(aud_SpinRev, 1);
                }
                else if (this->SuperForm == false) {
                    $AudioPlay(aud[0x42], 1);
                    if (this->JumpVariable < 0)
                        this->JumpVariable = 0;

                    this->Shield          = 4;
                }
                break;
            case 3:
                if (!SuperForm || UseShieldSuper) {
                    this->ShieldAction = true;
                    this->Speed        = 0.0f;
                    this->Gravity      = 8.0f;
                    $AudioPlay(aud[0x44], 1);
                }
                break;
            case 2:
                if (!SuperForm || UseShieldSuper) {
                    this->Gravity = -5.5f;
                    this->JumpVariable = 0;
                    $AudioPlay(aud_ShieldElectric, 1);
                    Vibrate(0, 0.5, 250);
                    /*Spark       = instance_create(x, y-10, objShieldSpark);
                    Spark.Dir   = 0;
                    Spark       = instance_create(x, y-10, objShieldSpark);
                    Spark.Dir   = 180;
                    Spark       = instance_create(x, y-10, objShieldSpark);
                    Spark.Dir   = 225;
                    Spark       = instance_create(x, y-10, objShieldSpark);
                    Spark.Dir   = 315;*/
                    $AudioPlay(aud[0x45], 1);
                    break;
                }
            case 1:
                if (!SuperForm || UseShieldSuper) {
                    this->ShieldAction = true;
                    this->Speed        = 8.0f * this->flip;
                    this->Gravity      = 0.0f;
                    Vibrate(0, 0.5, 250);
                    $AudioPlay(aud[0x43], 1);

                    if (CameraLag == true) {
                        CameraLock = 16;
                        //objCamera.CameraLock   = true;
                        //objCamera.alarm[0]     = 16;
                    }
                }
                break;
        }
        this->ShieldUsable = false;
    }
    else if (jump && this->CharacterId == consCharacterTails && Action != Actions::Rolling) {
        this->Action     = Actions::Fly;
        this->FlyFlag    = 1;
        if (this->Underwater == false) {
            //audio_play_sound(sndFly, 1, true);
        }
        jump = false;
    }
    else if (jump && CharacterId == consCharacterKnuckles) {
        this->Action = Actions::Glide;
        if (this->Gravity < 0)
            this->Gravity = 0;
        this->Speed = 4.0f * flip;
        this->TurnSpeed = 0;
    }
    else if (jump && up && this->HyperForm == true && this->ShieldUsable == true) {
        if (up == true) {
            this->Speed       = 0.0f;
            this->Gravity     = -8.0f;
        }
        else {
            this->Speed       = 8.0f * flip;
            this->Gravity     = 0;
        }
        this->ShieldUsable = false;
        this->JumpVariable = 0;
        if (CameraLag == true) {
            CameraLock = 16;
            //objCamera.CameraLock   = true;
            //objCamera.alarm[0]     = 16;
        }
        //audio_play_sound(sndPlayerSpindash, 1, false);
    }
    if (jump && up && !SuperForm && !Ground && sc->rings >= 50/* && global.ChaosEmeralds == 127 && */ && CharacterId == consCharacterSonic && UseSuperForms && !DisableSuper && !sc->levelComplete) {
        Speed       = 0;
        Gravity     = 0;
        //this->Action      = Actions::Transform;
        SuperForm = true;
        SuperFlight = false;
        formingType = 1;
        formingTimer = 4 * 10;
        Shield = 0;
        sc->aud[0x9F]->Play(2);
        //audio_pause_sound(global.CurrentSong);
        //audio_play_sound(sndSuperSonic, 1, true);
        //global.CurrentSong = sndSuperSonic;
    }

    if (DropDashEnabled && jumpHold && DropDashRev > 0) {
        DropDashRev++;
    }
}
void Player::scrPlayerActionCrouchDown() {
    if (down == false)
        this->Action = Actions::Normal;

    if (jump == true && this->Spindash > 0) {
        this->SpindashRev = 0;
        //SpindashFrame = global.Frame;
        if (this->Spindash == 1) {
            $AudioPlay(aud_SpinRev, 1);
        }
        else if (this->Spindash == 2) {
            //audio_stop_sound(sndPeelout);
            //audio_play_sound(sndPeelout, 1, false);
        }
        this->Action = Actions::Spindash;
    }
    else if (jump && this->Ground == true)
        scrPlayerJump();
}
void Player::scrPlayerActionSpindash() {
    if (this->Delay == true) {
        /*this->SpindashRev = this->SpindashRev;
        image_xscale   = global.Player[0].image_xscale;
        if (global.Player[0].Action != ActionSpindash) {
            Speed = (SpindashMin+SpindashRev)*image_xscale;
            audio_play_sound(sndPlayerSpindash, 1, false);
            scrPlayerResetTrail();
            Action = ActionRolling;
            scrPlayerHandleMonitors();
        }
        exit;*/
    }

    if (this->Spindash == 1) {
        if (jump) {
            this->SpindashRev = std::min(this->SpindashRev + this->SpindashRevAmount, this->SpindashMax - 8.0f);
            $AudioPlay(aud_SpinRev, 1);
            frame = 91;

            Vibrate(0, 0.25, 150);
        }
        this->SpindashRev -= (this->SpindashRev / 0.125f) / 256.0f;

        S3Object::Terrain _ObjectHandle = collision_line_terrain(this->x, this->y + 10, this->x + (12 * flip), this->y + 10);
        if (_ObjectHandle.Active && PlayerID == 0) {
            if (sc->objects[_ObjectHandle.ObjectID] != NULL) {
                if (sc->objects[_ObjectHandle.ObjectID]->Pushable) {
                    this->x += flip * 0.25f;
                    sc->objects[_ObjectHandle.ObjectID]->x += flip * 0.25f;
                }
            }
        }

        if (down == false) {
            this->Speed = (((this->SpindashMin + this->SpindashRev)) + (3.0f * (this->SuperForm ? 1 : 0))) * this->flip;
            //scrPlayerResetTrail();
            //$AudioPlay(aud_Roll, 1);
            this->Action = Actions::Rolling;
            this->SpindashRev = 0;

            if (CameraLag == true) {
                CameraLock = 16;
            }

            scrPlayerHandleMonitors();
        }
    }
    /*else if (KeyDown == false && Spindash == 2) {
        if (global.Frame >= SpindashFrame+45) {
            Speed = (12*image_xscale);
            audio_play_sound(sndPlayerSpindash, 1, false);
            scrPlayerResetTrail();
            Action = ActionRolling;
            scrPlayerHandleMonitors();
        }else{
            Action = Actions::Normal;
        }
    }*/
}
void Player::scrPlayerActionSpring() {
    if (Ground == true) {
        Action = Actions::Normal;
        this->Fan = false;
    }
    else if (jump && Ground == false && UseCurlButton == true && Fan == false && Action == Actions::Spring) {
        Action = Actions::Jumping;
        JumpAnimSpeed  = 1.0f / std::max((5.0f - std::abs(Speed)), 1.0f);
        //audio_play_sound(sndShieldInstashield, 1, false);
        JumpVariable = 0;
    }
    else if (Fan == false && Gravity > 0 && Action == Actions::Spring) {
        Action = Actions::Normal;
        anim(0, 7, std::max(8 - (int)std::abs(this->Speed), 1), false);
        if (SpringFlip > 0) {
            SpringFlip--;
        }
    }
}
void Player::scrPlayerActionSlide() {
    if (Ground == false)
        Action = Actions::Normal;
    else if (jump)
        scrPlayerJump();

    if (signum(Speed) != 0)
        flip = signum(Speed);
}
void Player::scrPlayerActionGrab() {
    this->y = GrabY;
    Gravity = 0;
    Speed = ConveyorSpeed;
    if (left) {
        Speed = ConveyorSpeed - 1;
    }
    if (right) {
        Speed = ConveyorSpeed + 1;
    }
    if (jump) {
        LastGrabY = GrabY;
        GrabTimer = 60;
        scrPlayerJump();
        Gravity = -5;
        HangingBar = false;
        ObjectControlled = 0;
    }
}
void Player::scrPlayerActionObjectGrab() {
    if (jump) {
        scrPlayerJump();
        Gravity = -5;
        HangingBar = false;
        ObjectControlled = 0;
    }
}
void Player::scrPlayerActionFly() {
    FlyTimer    -= 1;
    GravityRate  = 0;

    InputAlarm = 0;

    if (FlyTimer == 0 && Underwater == false) {
        //audio_stop_sound(sndFly);
        //audio_play_sound(sndFlyFall, 1, true);
    }
    if (Gravity <= -1)
        FlyFlag = 1;

    if (FlyFlag > 1) {
        Gravity -= 0.125f;
        FlyFlag++;
        if (FlyFlag == 32)
            FlyFlag = 1;
    }
    else {
       if (FlyTimer > 0 && Gravity > -1 && jump)
           FlyFlag = 2;
       Gravity += 0.03125f;
    }

    TopSpeed = 6.f / (Underwater ? 2 : 1);

    if (Ground) {
        FlyTimer    = 480;
        FlyFlag     = 0;
        Action      = Actions::Normal;
    }
}
void Player::scrPlayerActionInStream() {
    //Speed = std::max(Speed, 7.f);
    Ground = false;
    Angle = 0;
    AngleMode = 0;
    if (up) Gravity = -1;
    else if (down) Gravity = 1;
    else Gravity = 0;

    Vibrate(0, 0.10, 50);
}
void Player::scrPlayerActionInStreamPipe() {
    Ground = false;
    Angle = 0;
    AngleMode = 0;

    Vibrate(0, 0.10, 50);
}
void Player::scrPlayerActionInStreamGrab() {
    Speed = 0;
    Ground = false;
    Angle = 0;
    AngleMode = 0;
    if (up) Gravity = -2;
    else if (down) Gravity = 2;
    else Gravity = 0;
}
void Player::scrPlayerActionRespawn() {
    Angle = 0;
    AngleMode  = 0;

    Player *player1 = sc->playerBuffer[0];

    switch (CharacterId) {
        case 1: // Tails
            x        += signum(player1->x - x) * 3;
            y        += signum(player1->y - y) * 3;

            FlagsAllowXMovement = false;
            FlagsAllowYMovement = false;
            FlagsAllowObjectHandle = false;

            if (signum(player1->x - x) != 0)
                flip = signum(player1->x - x);

            Speed = 0;
            Gravity = 0;
            if ((player1->x - x) * (player1->x - x) + (player1->y - y) * (player1->y - y) < 5 * 5) {
                Action = Actions::Fly;
                FlagsAllowXMovement = true;
                FlagsAllowYMovement = true;
                FlagsAllowObjectHandle = true;
            }
            break;
        /*case consCharacterKnuckles:
            Dir       = point_direction(x, y, player1->x, player1->y);
            x        += lengthdir_x(3, Dir);
            y        += lengthdir_y(3, Dir);

            FlagsAllowXMovement = false;
            FlagsAllowYMovement = false;
            FlagsAllowObjectHandle = false;

            if (sign(lengthdir_x(1, Dir)) != 0)
                flip = sign(lengthdir_x(1, Dir))

            Speed = 0;
            Gravity = 0;
            if (distance_to_object(player1) < 5) {
                Action = ActionGlideFall;
                Speed = 0.75;
                FlagsAllowXMovement = true;
                FlagsAllowYMovement = true;
                FlagsAllowObjectHandle = true;
            }
            break;
        default:
            Dir       = point_direction(x, y, player1->x, player1->y-4);
            //x        += lengthdir_x(max(min(abs(x-player1->x)/8, 16), 4), Dir);
            x = player1->x-20;
            y        -= 6;
            RespawnTimer++;
           // Speed     = max(min(Speed+lengthdir_x(0.09375, Dir), 4), -4);
            //Gravity   = -5;

            FlagsAllowXMovement = false;
            FlagsAllowYMovement = false;
            FlagsAllowObjectHandle = false;

            if(sign(lengthdir_x(1, Dir)) != 0)
                image_xscale = sign(lengthdir_x(1, Dir))

            if(y <= player1->y &&(distance_to_object(Player1) < 5 || y < player1->y-40 || RespawnTimer > 60)){
                Action                  = ActionJumping;
                Gravity                 = -3;
                Speed                   = max(min(distance_to_object(Player1)/8, Player1.Speed, 16), 2);
                JumpVariable            = true;

                FlagsAllowXMovement     = true;
                FlagsAllowYMovement     = true;
                FlagsAllowObjectHandle  = true;
            }
            break;*/
    }
}

void Player::handleAction() {
    switch (this->Action) {
        case Actions::Normal:
            scrPlayerActionNormal();
            break;
        case Actions::Skid:
            scrPlayerActionSkid();
            break;
        case Actions::Jumping:
            scrPlayerActionJumping();
            break;
        case Actions::Push:
            scrPlayerActionPush();
            break;
        case Actions::Rolling:
            scrPlayerActionRolling();
            break;
        case Actions::CrouchDown:
            scrPlayerActionCrouchDown();
            break;
        case Actions::Spindash:
            scrPlayerActionSpindash();
            break;
        case Actions::Spring:
        case Actions::Fan:
            scrPlayerActionSpring();
            break;
        case Actions::Slide:
            scrPlayerActionSlide();
            break;

        case Actions::Fly:
            scrPlayerActionFly();
            break;

        case Actions::Grab:
            scrPlayerActionGrab();
            break;

        case Actions::ObjectGrab:
            scrPlayerActionObjectGrab();
            break;

        case Actions::InStream:
            scrPlayerActionInStream();
            break;
        case Actions::InStreamGrab:
            scrPlayerActionInStreamGrab();
            break;
        case Actions::InStreamPipe:
            scrPlayerActionInStreamPipe();
            break;

        case Actions::Respawn:
            scrPlayerActionRespawn();
            break;

        default:
            scrPlayerActionNormal();
            break;
    }
}

void Player::beginStep() {
    WaterRun = false;

    XPrevious = x;
    YPrevious = y;

    if (CameraLag && CameraLock > 0) {
        CameraLock--;
    }

    // Input Management
    this->Landed = false;
    if (Action == Actions::Dead || DisableSuper || sc->levelComplete) {
        if (SuperForm) {
            SuperForm = false;

            formingType = -1;
            formingTimer = 4 * 10;
        }
    }

    if (this->Action == Actions::Dead) {
        app->Stack->Remove("Drowning");
        return;
    }

    if (Ground) {
        enemyCombo = 0;
        SpringFlip = 0;
    }

    // Timers
    if (this->Invincibility > 0) {
        this->InvincibilityTimer -= 1;
        if (this->Invincibility > 1)
            UnderwaterTimer = 1800;

        if (this->InvincibilityTimer % 8 == 0 && this->Invincibility == 2 && this->SuperForm == false) {
            //this->InvAngle = scrWrapAngle(this->InvAngle + 45);
            //float X = this->Cos[this->InvAngle]*16;
            //float Y = this->Sin[this->InvAngle]*16;
            //instance_create(x+X, y+Y, objInvSparkle)
            //instance_create(x-X, y-Y, objInvSparkle)
        }

        //instance_create(x-X, y-Y, objInvSparkle);
        if (this->InvincibilityTimer <= 0) {
            /*if (audio_is_playing(sndInvincibility) && Invincibility == 2 && audio_is_playing(sndSuperSonic) == false) {
                audio_stop_sound(sndInvincibility);
                global.CurrentSong = ds_map_find_value(LvlMusic, room);
                audio_resume_sound(global.CurrentSong);
            }*/


            if (Invincibility == 2) {
                app->Stack->Remove("Invincibility");
            }

            this->Invincibility = 0;
        }
    }
    if (this->SpeedSneakers == true) {
        this->SpeedSneakersTimer -= 1;
        if (this->SpeedSneakersTimer <= 0) {
            /*if (audio_is_playing(sndSpeedSneakers) && audio_is_playing(sndSuperSonic) == false) {
                audio_stop_sound(sndSpeedSneakers);
                global.CurrentSong = ds_map_find_value(LvlMusic, room);
                audio_resume_sound(global.CurrentSong);
            }*/
            this->SpeedSneakers = false;
        }
    }
    if (this->SuperForm) {
        this->SuperFormTimer += 1;
        if (this->HyperForm == false)
            this->CurrentPalette += 0.1f;
        else{
            this->CurrentPalette += 0.3f;

            /*if (global.Frame mod 10) {
                InvAngle   = scrWrapAngle(InvAngle+45);
                Ins        = instance_create(x, y, objHyperSparkle);
                Ins.Angle  = InvAngle;
            }*/
        }
        /*if ((CurrentPalette > sprite_get_number(SuperPalette)-1 && HyperForm == false)||(CurrentPalette > sprite_get_number(HyperPalette)-1 && HyperForm == true))
            CurrentPalette = 1;*/
        if (SuperFormTimer % 60 == 0) {
            sc->rings -= 1;
            if (sc->rings == 0) {
                SuperForm = false;

                formingType = -1;
                formingTimer = 4 * 10;
                //scrPlayerDeform();
            }
        }
    }
    if (this->Deform == true) {
        this->CurrentPalette -= 0.4f;
        if (this->CurrentPalette < 0)
            this->Deform = false;
    }
    if (this->ShieldUsable == false && this->Action != Actions::Jumping) {
        this->ShieldUsable = true;
        this->ShieldAction = false;
    }



    // Underwater
    if (Action == Actions::Pipe) {
        Underwater = false;
    }
    if (Underwater == true) {
        if (Shield != 3 && Action != Actions::Pipe)
            UnderwaterTimer -= 1;
        else
            UnderwaterTimer = 1800;

        if (UnderwaterTimer == 660) {// && UnderwaterTimer > 500) {
            app->Stack->Push("Drowning", sc->aud_BGM_Drowning, 11, 0, 0, false, true);
        }
        else if (UnderwaterTimer > 660) {
            //stop drowning music
            app->Stack->Remove("Drowning");
        }
        if (UnderwaterTimer == 0) {
            scrPlayerDead();
            $AudioPlay(aud_C_Drown, 3);
            //UnderwaterTimer = 1800;
        }
    }
    else {
        app->Stack->Remove("Drowning");
    }


    // Physics
    if (this->Underwater == false) {
        this->UnderwaterTimer = 1800;

        if (this->Action != Actions::Fly)
            this->GravityRate = 0.21875f;

        this->JumpRelease = this->JumpReleaseValue;

        if (!this->SuperForm || this->SpeedSneakers) {
            this->JumpHeight = this->JumpValue;
            this->SkidRate = 0.5f;
        }

        if (!this->SpeedSneakers) {
            if (!this->SuperForm) {
                this->Acceleration = this->AccelRate;
                if (this->Action != Actions::Fly)
                    this->TopSpeed = 6;
                this->RollingFriction = 0.0234375f;
            }
            else {
                this->JumpHeight = -8;
                this->SkidRate = 1;
                if (this->Action != Actions::Fly)
                    this->TopSpeed = 10;
                RollingFriction = 0.0234375f;
                Acceleration = AccelRate * 4;
            }
        }
        else {
            this->Acceleration = this->AccelRate * 2;
            if (this->Action != Actions::Fly)
                this->TopSpeed = 12;
            this->RollingFriction = 0.046875f;
        }
    }
    else {
        // Underwater
        this->JumpHeight = this->JumpValueWater;
        this->JumpRelease = this->JumpReleaseWater;

        if (this->Action != Actions::Fly)
            this->GravityRate = 0.0625f;

        if (this->Shield == 2 || this->Shield == 1)
            this->Shield = 0;

        if (!this->SuperForm) {
            if (this->Action != Actions::Fly)
                this->TopSpeed = 3;

            this->Acceleration = this->AccelRate / 2.0f;
            this->RollingFriction = 0.01171875f;
            this->SkidRate = 0.25f;
        }
        else {
            if (this->Action != Actions::Fly)
                this->TopSpeed = 5;

            this->Acceleration = this->AccelRate * 2;
            this->RollingFriction = 0.046875f;
            this->SkidRate = 0.5f;
        }
    }

    if (this->Ground == false && this->Action != Actions::Fly) {
        this->Acceleration *= 2;
    }

    if (SuperFlight && SuperForm) {
        GravityRate = 0;
        JumpHeight = 0;
        if (up) {
            this->Gravity -= this->Acceleration * this->SpeedMultiplier / 2.f;
        }
        else if (down) {
            this->Gravity += this->Acceleration * this->SpeedMultiplier / 2.f;
        }
        else if (left || right) {

        }
        else {
            this->Speed -= std::min(std::abs(this->Speed), this->Acceleration * this->SpeedMultiplier / 6.f) * signum(this->Speed);
            this->Gravity -= std::min(std::abs(this->Gravity), this->Acceleration * this->SpeedMultiplier / 6.f) * signum(this->Gravity);
        }
        Ground = false;
    }

    if (scrPlayerCollisionSquish()) {
        scrPlayerDead();
    }
}
void Player::step() {
    if (this->Action == Actions::Dead) {
        return;
    }

    if (this->Ground == true &&
        this->Action != Actions::Rolling &&
        this->Action != Actions::Pipe &&
        this->Action != Actions::CrouchDown &&
        this->Action != Actions::Spindash &&
        this->Action != Actions::LookUp)
        this->Speed -= (0.125f * this->Sin[this->Angle]) / this->SpeedMultiplier;

    if (std::abs(this->Speed) < 2 && this->Shield == 1 && this->ShieldAction == true)
        this->ShieldAction = false;

    if (Action == Actions::Rolling || Action == Actions::Slide) {
        if (signum(this->Speed) == signum(this->Sin[this->Angle])) {
            this->Speed -= (0.078125f * this->Sin[this->Angle]) / this->SpeedMultiplier;
        }
        else {
            this->Speed -= (0.3125f * this->Sin[this->Angle]) / this->SpeedMultiplier;
        }
    }

    if (ForceRoll) Action = Actions::Rolling;

    // -- Accelerate when holding LEFT/RIGHT keys, and decelerate when hold neither. --
    if (this->InputAlarm <= 0 &&
        (this->ShieldAction == false || Shield == 3) &&
        (this->Action == Actions::Normal ||
            (this->Action == Actions::Corkscrew/* && this->Animation != "Rolling"*/) ||
            (this->Action == Actions::Jumping && (this->JumpVariable >= 0 || this->RollJumpLock == false)) ||
            this->Action == Actions::Fly ||
            this->Action == Actions::Spring ||
            this->Action == Actions::Fan ||
            this->Action == Actions::Peril ||
            this->Action == Actions::Peril ||
            this->Action == Actions::Peril ||
            this->Action == Actions::Peril) && ObjectControlled == 0) {
        if (left && this->Speed > -this->TopSpeed)
            this->Speed = std::max(this->Speed - this->Acceleration * this->SpeedMultiplier, -this->TopSpeed);
        else if (right && this->Speed < this->TopSpeed)
            this->Speed = std::min(this->Speed + this->Acceleration * this->SpeedMultiplier, this->TopSpeed);

        if (right == false && left == false && this->Speed != 0 && (this->Ground == true || this->Action == Actions::Corkscrew) && ObjectControlled == 0 && (!SuperForm || (SuperForm && !SuperFlight)))
            this->Speed -= std::min(std::abs(this->Speed), AccelRate * this->SpeedMultiplier) * signum(this->Speed);

    }
    // -- Handle Input Alarm.
    if (this->InputAlarm > 0 && ((this->Ground == true) || this->InputAlarm < 30)) {
        this->InputAlarm -= 1;
    }

    if (ObjectControlled > 0 && ObjectControlled < 4 && jump) {
        ObjectControlled = 0;
        Gravity = -3;
    }

    if (ObjectControlled % 4 == 0) {
        FlagsAllowXMovement = true;
        FlagsAllowYMovement = true;
    }
    else if (ObjectControlled % 4 == 1) {
        FlagsAllowXMovement = false;
        FlagsAllowYMovement = false;
    }
    else if (ObjectControlled % 4 == 2) {
        FlagsAllowXMovement = false;
        FlagsAllowYMovement = true;
    }
    else if (ObjectControlled % 4 == 3) {
        FlagsAllowXMovement = true;
        FlagsAllowYMovement = false;
    }

    // Movement
    if (this->Action == Actions::Dead) {
        return;
    }

    if (this->FlagsAllowXMovement) {
        scrPlayerHandleBreakableTerrain();
        scrPlayerWallCollision();
    }

    if (this->FlagsAllowXMovement)
        scrPlayerSpeedMovement();

    if (this->FlagsAllowYMovement) {
        scrPlayerHandleBreakableTerrain();

        if (!this->Ground) {
            if (this->Gravity < 0 && this->Gravity > -4 && !this->Ground)
                this->Speed -= (this->Speed / 0.125f) / 256.0f;

            scrPlayerGravityMovement();

            if (!this->Ground &&
                this->Action != Actions::Glide &&
                this->Action != Actions::Climb &&
                this->Action != Actions::ClimbRise &&
                this->Action != Actions::Transform &&
                this->Action != Actions::Grab &&
                this->Action != Actions::Quicksand)
                this->Gravity += this->GravityRate * this->SpeedMultiplier;
        }
    }



    // Object Handle
    if (this->FlagsAllowObjectHandle) {
        scrPlayerHandleObjects();
        scrPlayerHandleBreakableTerrain();
        /*scrPlayerHandleRings();
        scrPlayerHandleSprings();
        scrPlayerHandleBumpers();
        scrPlayerHandleBalloons();
        scrPlayerHandleDashers();
        scrPlayerHandleSpikes();
        scrPlayerHandleEnemies();
        scrPlayerHandleMonitors();
        scrPlayerHandlePipes();
        scrPlayerHandleCheckpoint();
        scrPlayerHandleBubbles();
        scrPlayerHandleHarmful();
        scrPlayerHandleCollapsingPlatforms();
        scrPlayerHandleSignpost();
        scrPlayerHandleChaosEmeralds();
        scrPlayerHandleGrabPoles();
        scrPlayerHandleCorkscrews();
        scrPlayerHandleQuicksand();
        scrPlayerHandleLayers();
        scrPlayerHandleConveyers();
        scrPlayerHandleMisc();
        scrPlayerHandleCutscenes();*/
    }



    // Actions
    handleAction();



    // Misc
    /*if Sonic falls past ground bounds,
        scrPlayerDead(sndPlayerDead);
    if (Action != ActionFly &&(audio_is_playing(sndFly) || audio_is_playing(sndFlyFall))) {
        audio_stop_sound(sndFly);
        audio_stop_sound(sndFlyFall);
    }*/
    if (this->Action != Actions::Fly) {
        this->FlyTimer = 480;
        this->FlyFlag = 0;
    }

    if (this->Shield == 4 && ShieldAnimation >= 7) {
        this->Shield = 0;
        this->ShieldAnimation = 0;
    }

    if (this->AirDash == true && this->Action != Actions::Jumping)
        this->AirDash = false;


    // Speed Caps
    if (this->FlagSpeedCap == 1) {
        if (this->Action != Actions::Rolling &&
            (this->Action != Actions::Jumping || this->JumpVariable > 0) && this->InputAlarm <= 0) {
            if (right)
                this->Speed = std::min(this->Speed, this->TopSpeed);
            if (left)
                this->Speed = std::max(this->Speed, -this->TopSpeed);
        }
    }
    else if (this->FlagSpeedCap == 2) {
        if (this->Ground == false && (this->Action != Actions::Jumping || this->JumpVariable > 0) && this->InputAlarm <= 0) {

            if (right)
                this->Speed = std::min(this->Speed, this->TopSpeed);
            if (left)
                this->Speed = std::max(this->Speed, -this->TopSpeed);
        }
    }
}
void Player::endStep() {
    if (this->Action == Actions::Dead) {
        this->Gravity += 0.21875f;
        this->y += this->Gravity;
    }

    if (ObjectControlled > 0) return;

    if ((this->Speed <= 0 || this->Ground == false) && left && this->InputAlarm <= 0 &&
        (this->Action == Actions::Normal ||
        this->Action == Actions::Spring ||
        this->Action == Actions::Fan ||
        this->Action == Actions::Fly ||
        this->Action == Actions::Jumping ||
        this->Action == Actions::Quicksand ||
        this->Action == Actions::Peril))
        this->flip = -1;
    if ((this->Speed >= 0 || this->Ground == false) && right && this->InputAlarm <= 0 &&
        (this->Action == Actions::Normal ||
        this->Action == Actions::Spring ||
        this->Action == Actions::Fan ||
        this->Action == Actions::Fly ||
        this->Action == Actions::Jumping ||
        this->Action == Actions::Quicksand ||
        this->Action == Actions::Peril))
        this->flip = 1;

    if (this->Speed * Cos[this->AngleMode * 90] < -6 && left) {
        //camShiftX += (-80.0f - camShiftX) / 20.0f;
    }
    else if (this->Speed * Cos[this->AngleMode * 90] > 6 && right) {
        //camShiftX += (80.0f - camShiftX) / 20.0f;
    }
    else if (this->AngleMode == 0 && ObjectControlled == 0) {
        camShiftX += (0.0f - camShiftX) / 20.0f;
    }
    if (Action == Actions::Spindash) {
        camShiftX += (80.0f * flip - camShiftX) / 20.0f;
    }

    if (GrabTimer > -1) {
        GrabTimer--;
    }
    if (GrabTimer == 0) {
        GrabY = -1;
        LastGrabY = -1;
    }

    collision_line_terrain(x, y + 20, x, y + 20);
    this->x += standSpdX;
    this->standSpdX = 0;

    if (Ground)
        GroundSpeed = Speed;


    if (WaterRun && sc->frameAnim % 16 == 0) {
        sc->aud[0xDB]->Play(3);
    }
}

void Player::update() {
    if (Controllable) {
        up = app->input->up[0];
        down = app->input->down[0];
        left = app->input->left[0];
        right = app->input->right[0];
        jump = app->input->jumpPressed[0] || app->input->jump2Pressed[0];
        jumpHold = app->input->jump[0] || app->input->jump2[0];
    }
    else {
        if (PlayerID == 1) {
            Player *FirstPlayer = sc->playerBuffer[0];
            if ((x < FirstPlayer->x - 10 || (FirstPlayer->y < y - 30 && FirstPlayer->Ground && FirstPlayer->Speed > 0))
                 && (FirstPlayer->y >= y - 30 || !FirstPlayer->Ground || FirstPlayer->Speed >= 0))
                right = true;
            else
                right = false;

            if ((x > FirstPlayer->x + 10 || (FirstPlayer->y < y - 30 && FirstPlayer->Ground && FirstPlayer->Speed < 0))
                 && (FirstPlayer->y >= y - 30 || !FirstPlayer->Ground || FirstPlayer->Speed <= 0))
                left = true;
            else
                left = false;

            if (Ground && FirstPlayer->y < y - 30 && FirstPlayer->Ground && Action != Actions::Rolling)
                jump = true;

            if ((x < sc->cameraX[0] - 30 ||
                x > sc->cameraX[0] + app->renderWidth + 30 ||
                y < sc->cameraY[0] - 30 ||
                y > sc->cameraY[0] + app->renderHeight + 30) && Action != Actions::Respawn && std::abs(FirstPlayer->Speed) < 1) {
                Action = Actions::Respawn;
                x = FirstPlayer->x;
                y = FirstPlayer->y - 320;
            }

            /*if (Action == Actions::Jumping)
                jumpHold = true;
            else
                jumpHold = false;*/
        }
    }

    if (ControlLock) {
        up = false;
        down = false;
        left = false;
        right = false;
        jump = false;
        jumpHold = false;
    }

    if (left && right) {
        left = false;
    }

    if (Controllable) {
        for (int i = 0; i < 4; i++) {
            if (app->input->touchDown[i]) {
                int X2 = 48;
                int Y2 = app->gameHeight - 48;

                int X3 = app->input->touchX[i];
                int Y3 = app->input->touchY[i];

                float angle = toDegrees(scrWrapAngleRad(std::atan2(Y2 - Y3, X3 - X2)));
                float len = std::sqrt((Y2 - Y3) * (Y2 - Y3) + (X3 - X2) * (X3 - X2));

                //printf("Angle: %d Distance: %d\n", (int)angle, (int)len);

                if (len < 100) {
                    if (angle >= 45 && angle < 135) {
                        up = true;
                    }
                    if (angle < 45 || angle >= 315) {
                        right = true;
                    }
                    if (angle >= 135 && angle < 225) {
                        left = true;
                    }
                    if (angle >= 225 && angle < 315) {
                        down = true;
                    }
                }

                X2 = app->renderWidth - 48 + 8;
                Y2 = app->renderHeight - 48 + 8;
                len = std::sqrt((Y2 - Y3) * (Y2 - Y3) + (X3 - X2) * (X3 - X2));

                if (len < 75) {
                    jump = app->input->touchPressed[i];
                    jumpHold = app->input->touchDown[i];
                }

                if (Controllable)
                    app->input->jump3[0] = jumpHold && up;
            }
        }
    }

    if (up || down || left || right || jump || jumpHold) {
        waitClock = 0;
    }

    if (ObjectControlled >= 4) {
        up = false;
        down = false;
        left = false;
        right = false;
        jump = false;
        jumpHold = false;
    }

    if (!sc->DebugMode) {
        beginStep();
        step();
        endStep();

        offY = 0;

        if (SuperForm && formingType != 1) {
            formingType = 2;
            formingTimer = 2;
        }
        if (formingTimer > 0)
            formingTimer--;

        if (formingType != 0 && formingType != 2) {
            if (formingTimer == 0) {
                if (formingType == 1)
                    formingType = 2;
                else
                    formingType = 0;
                formingTimer = 4 * 10;
            }
        }

        // Set displayed angles
        if ((false)) { // angletype == 0
            float a = Angle;
            if (a < 30 || a > 330) {
                a = 0;
            }
            a = ((a * Math_PI / 180.0f - dispAng) + Math_PI);
            float n = (Math_PI * 2.0);

            if (!OverrideRotation)
                dispAng += ((a - std::floor(a / n) * n) - Math_PI) / 2.0f;
        }
        else {
            float a = Angle;
            if (a < 30 || a > 330) {
                a = 0;
            }

            a = std::round(a / 45.f) * 45;

            a = ((a * Math_PI / 180.0f - dispAng) + Math_PI);
            float n = (Math_PI * 2.0);

            if (!OverrideRotation)
                dispAng += ((a - std::floor(a / n) * n) - Math_PI) / 1.5f;

            n = Math_PI / 4;
            dispAng = std::round(dispAng / n) * n;
        }

        int sonicPoses[2 * 40] = {
            142, 142, // Standing
            0, 7, // Walking
            8, 11, // Running
            12, 23, // Air Flipping
            24, 35, // Air Flipping (Standing)
            36, 47, // Running in Cylinder
            48, 54, // Spinning Standing
            55, 60, // Swinging around a vertical bar
            61, 61, // YEAH!
            62, 67, // Swinging around a horizontal bar
            68, 82, // Running up a Cylinder
            83, 83, // Hanging on Swing bar
            84, 89, // Monkey Bars (Moving)
            91, 95, // Spindash
            97, 97, // Hurt
            98, 98, // Spring
            99, 99, // Hugging Wall
            100, 103, // Monkey Bars (Standing)
            104, 105, // Conveyor (Moving)
            106, 110, // Rolling
            111, 112, // Crouch
            113, 116, // Skidding
            120, 122, // Tipping
            117, 119, // Tipping Further
            123, 125, // Dead
            126, 127, // Hanging on to vertical bar
            128, 128, // Bubble Get
            129, 135, // Level Complete
            136, 137, // Hanging on to horizontal bar
            138, 141, // Pushing
            142, 150, // Waiting
            151, 152, // Looking up
            153, 155, // Whistling
            156, 163, // Air Spinning
            164, 165, // Begin Super
            166, 167, // Loop  Super
        };
        int tailsPoses[2 * 44] = {
            129, 129, // Standing
            0, 7, // Walking
            151, 152, // Running
            12, 23, // Air Flipping
            24, 35, // Air Flipping (Standing)
            36, 47, // Running in Cylinder
            48, 54, // Spinning Standing
            55, 60, // Swinging around a vertical bar
            61, 61, // YEAH!
            62, 67, // Swinging around a horizontal bar
            68, 82, // Running up a Cylinder
            83, 83, // Hanging on Swing bar
            84, 89, // Monkey Bars (Moving)
            90, 92, // Spindash
            93, 94, // Hurt
            95, 96, // Spring
            97, 97, // Hugging Wall
            100, 103, // Monkey Bars (Standing)
            104, 105, // Conveyor (Moving)
            106, 108, // Rolling
            109, 109, // Crouch
            98, 99, // Skidding
            110, 111, // Tipping
            110, 111, // Tipping Further
            112, 112, // Dead
            113, 114, // Hanging on to vertical bar
            115, 115, // Bubble Get
            121, 122, // Level Complete
            123, 124, // Hanging on to horizontal bar
            125, 128, // Pushing
            129, 136, // Waiting
            137, 144, // Looking up
            153, 155, // Whistling
            137, 144, // Air Spinning
            164, 165, // Begin Super
            166, 167, // Loop  Super
            116, 116, // Flying (Downwards)
            117, 117, // Flying (Upwards)
            118, 120, // Flying (Tired)
            145, 149, // Swimming
            155, 156, // Swimming (Tored)
        };
        int ani_stand = 0;
        int ani_walk = 2;
        int ani_run = 4;
        int ani_airFlipping = 6;
        /*int ani_airFlippingStanding = 8;*/
        //int ani_runningInCylinder = 10;
        int ani_spinningStanding = 12;
        /*int ani_swingingAroundAVertBar = 14;
        int ani_YEAH = 16;*/
        int ani_swingingAroundAHoriBar = 18;
        /*int ani_runningUpACylinder = 20;*/
        //int ani_hangingOnSwingBar = 22;
        //int ani_monkeyBars = 24;
        int ani_spindash = 26;
        int ani_hurt = 28;
        int ani_spring = 30;
        /*int ani_huggingWall = 32;*/
        int ani_monkeyBarsStanding = 34;
        int ani_conveyorMoving = 36;
        int ani_rolling = 38;
        int ani_crouch = 40;
        int ani_skidding = 42;
        int ani_tipping = 44;
        int ani_tippingFurther = 46;
        int ani_dead = 48;
        int ani_hangingOnVertBar = 50;
        /*int ani_bubbleGet = 52;
        int ani_levelComplete = 54;
        int ani_hangingOnHoriBar = 56;*/
        int ani_pushing = 58;
        /*int ani_waiting = 60;
        int ani_lookingUp = 62;
        int ani_whistling = 64;*/
        int ani_airSpinning = 66;
        /*int ani_beginSuper = 68;
        int ani_loopSuper = 70;*/
        int ani_flyingDownwards = 72;
        //int ani_flyingUpwards = 74;
        int ani_flyingTired = 76;
        int ani_swimming = 78;
        int ani_swimmingTired = 80;

        int* c = sonicPoses;
        if (CharacterId == 1)
            c = tailsPoses;

        if (Shield == 4 && ((!SuperForm && Invincibility <= 1 && Action != Actions::Transform))) {
            ShieldAnimation++;
        }

        dispFlip = flip;

        if (Ground) {
            if (Action == Actions::Rolling) {
                anim(c[ani_rolling], c[ani_rolling + 1], std::max(5 - (int)std::abs(GroundSpeed), 1), false);
                offY = 5;
                dispAng = 0;
            }
            else if (Action == Actions::Slide) {
                anim(c[ani_hurt], c[ani_hurt + 1], 10, false);
                dispAng = 0;
            }
            else {
                if (this->Action == Actions::CrouchDown) {
                    anim(c[ani_crouch], c[ani_crouch + 1], 4, true);
                    dispAng = 0;
                }
                else if (this->Action == Actions::Spindash) {
                    anim(c[ani_spindash], c[ani_spindash + 1], 2, false);
                    dispAng = 0;
                    offY = -1;
                }
                else {
                    if (std::abs(this->Speed) > 0) {
                        if (std::abs(this->Speed) >= 6 || (SuperForm && SuperFlight))
                            anim(c[ani_run], c[ani_run + 1], std::max(8 - (int)std::abs(this->Speed), 1), false);
                        else
                            anim(c[ani_walk], c[ani_walk + 1], std::max(8 - (int)std::abs(this->Speed), 1), false);

                        if (this->Speed < 0 && right) {
                            anim(c[ani_skidding], c[ani_skidding + 1], 4, true);
                        }
                        if (this->Speed > 0 && left) {
                            anim(c[ani_skidding], c[ani_skidding + 1], 4, true);
                        }
                    }
                    else {
                        if (Action == Actions::Push) {
                            anim(c[ani_pushing + 0], c[ani_pushing + 1], 32, false);
                        }
                        else {
                            int searchOffY = 24;
                            bool middle = collision_line_terrain((int)x, (int)y + 20, (int)x, (int)y + searchOffY).Active;
                            bool leftSide = collision_line_terrain((int)x - 6, (int)y + 20, (int)x - 6, (int)y + searchOffY).Active;
                            bool rightSide = collision_line_terrain((int)x + 6, (int)y + 20, (int)x + 6, (int)y + searchOffY).Active;
                            bool leftSideFar = collision_line_terrain((int)x - 10, (int)y + 20, (int)x - 10, (int)y + searchOffY).Active;
                            //bool rightSideFar = collision_line_terrain((int)x + 10, (int)y + 20, (int)x + 10, (int)y + searchOffY).Active;
                            if (!middle && Angle == 0) {
                                if (leftSide != rightSide) {
                                    anim(c[ani_tipping + 0], c[ani_tipping + 1], 8, false);

                                    if (leftSide)
                                        dispFlip = 1;
                                    else
                                        dispFlip = -1;
                                }
                                else {
                                    anim(c[ani_tippingFurther + 0], c[ani_tippingFurther + 1], 6, false);

                                    if (leftSideFar)
                                        dispFlip = 1;
                                    else
                                        dispFlip = -1;
                                }
                            }
                            else {
                                anim(c[ani_stand + 0], c[ani_stand + 1], 0, true);
                                this->frame = c[ani_stand + 0];
                            }
                        }
                    }
                }
            }

        }
        else {
            if (this->Action == Actions::Jumping) {
                anim(c[ani_rolling], c[ani_rolling + 1], std::max(5 - (int)std::abs(GroundSpeed), 1), false);
                dispAng = 0;
                offY = 4;
                if (DropDashRev) {
                    anim(c[ani_rolling + 0], c[ani_rolling + 0], 0, true);
                }
            }
            if (Action == Actions::Spring) {
                if (Fan) {
                    anim(c[ani_airFlipping], c[ani_airFlipping + 1], 2, false);
                }
                else {
                    if (SpringFlip > 0) {
                        anim(c[ani_airFlipping], c[ani_airFlipping + 1], 4, false);
                    }
                    else {
                        anim(c[ani_spring], c[ani_spring + 1], 0, true);
                        this->frame = c[ani_spring];
                    }
                }
            }
            if (Action == Actions::Fan) {
                //if (SpringFlip > 0) {
                    anim(c[ani_airFlipping], c[ani_airFlipping + 1], 3, false);
                /*}
                else {
                    anim(c[ani_airSpinning], c[ani_airSpinning + 1], 3, false);
                //}*/
            }
            if (SpringFlip > 0) {
                anim(c[ani_airFlipping], c[ani_airFlipping + 1], 4, false);
            }
        }
        if (Action == Actions::Grab || Action == Actions::ObjectGrab) {
            if (Fan) {
                anim(c[ani_swingingAroundAHoriBar], c[ani_swingingAroundAHoriBar + 1] - 2, 4, true);
            }
            else {
                if (std::abs(Speed) != 2) {
                    anim(c[ani_conveyorMoving], c[ani_conveyorMoving + 1], 8, false);
                }
                else {
                    /*if ((int)this->frame > c[ani_swingingAroundAHoriBar + 1] - 2 ||
                        (int)this->frame < c[ani_swingingAroundAHoriBar]) {
                        anim(c[ani_swingingAroundAHoriBar], c[ani_swingingAroundAHoriBar], 0, true);
                        this->frame = c[ani_swingingAroundAHoriBar];
                    }*/
                    //anim(c[ani_swingingAroundAHoriBar + 1] - 2 + 0.75, c[ani_swingingAroundAHoriBar], -0.25, true);
                }
            }
            if (HangingBar) {
                anim(c[ani_monkeyBarsStanding], c[ani_monkeyBarsStanding + 1], 0, true);
            }
        }
        if (Action == Actions::Hurt || Action == Actions::Peril) {
            anim(c[ani_hurt], c[ani_hurt + 1], 0, false);
            offY = -4;
        }
        if (Action == Actions::InStream) {
            flip = 1;
            anim(c[ani_airSpinning], c[ani_airSpinning + 1], 8, false);
            offY = 2;
        }
        if (Action == Actions::InStreamGrab) {
            flip = 1;
            anim(c[ani_hangingOnVertBar], c[ani_hangingOnVertBar + 1], 2, false);
            offY = 4;
        }

        if (Action == Actions::Dead) {
            anim(c[ani_dead], c[ani_dead + 1], 0, true);
            offY = 0;
            flip = 1;
        }

        if (CharacterId == 1)
            offY += 4;

        if (SuperForm && SuperFlight)
            anim(c[ani_run], c[ani_run + 1], 1, false);

        if (Action == Actions::Fly) {
            if (Underwater) {
                if (FlyTimer > 0) {
                    anim(c[ani_swimming], c[ani_swimming + 1], 2 * (signum(Gravity) + 3), false);
                }
                else {
                    anim(c[ani_swimmingTired], c[ani_swimmingTired + 1], 16, false);
                }
            }
            else {
                if (FlyTimer > 0) {
                    anim(c[ani_flyingDownwards], c[ani_flyingDownwards + 1], 4, true);
                }
                else {
                    anim(c[ani_flyingTired], c[ani_flyingTired + 1], 16, false);
                }
            }
        }

        if (ObjectControlled == 0) {
            overrideFrame = -1;
        }
        if (ObjectControlled > 0 && Rot3D != -1) {
            switch (Rot3D) {
                case 0:
                    frame = c[ani_spinningStanding] + 0;
                    flip = 1;
                    break;
                case 1:
                    frame = c[ani_spinningStanding] + 1;
                    flip = 1;
                    break;
                case 2:
                    frame = c[ani_spinningStanding] + 2;
                    flip = 1;
                    break;
                case 3:
                    frame = c[ani_spinningStanding] + 3;
                    flip = 1;
                    break;
                case 4:
                    frame = c[ani_spinningStanding] + 2;
                    flip = -1;
                    break;
                case 5:
                    frame = c[ani_spinningStanding] + 1;
                    flip = -1;
                    break;
                case 6:
                    frame = c[ani_spinningStanding] + 0;
                    flip = -1;
                    break;
                case 7:
                    frame = c[ani_spinningStanding] + 4;
                    flip = 1;
                    break;
                case 8:
                    frame = c[ani_spinningStanding] + 5;
                    flip = 1;
                    break;
                case 9:
                    frame = c[ani_spinningStanding] + 6;
                    flip = -1;
                    break;
                case 10:
                    frame = c[ani_spinningStanding] + 5;
                    flip = -1;
                    break;
                case 11:
                    frame = c[ani_spinningStanding] + 4;
                    flip = -1;
                    break;
            }
            if (Rot3D >= 12) {
                frame = c[ani_airSpinning] + (Rot3D - 12) * 8 / 12;
            }
            anim(frame, frame, 0, true);
            offY = 0;
        }

        if (animOverride > 0) {
            anim(c[animOverrideFrame], c[animOverrideFrame + 1], 16, true);
            animOverride--;
        }

        animExecute();

        if (overrideFrame > -1) {
            startAu = 0;
            frm = overrideFrame;
        }

        if (Action == Actions::Grab) {
            if ((int)this->frame == 62) {
                offY = 20;
            }
            else if ((int)this->frame == 63) {
                offY = 11;
            }
            else if ((int)this->frame == 64) {
                offY = -15;
            }
            else if ((int)this->frame == 65) {
                offY = -20;
            }
            else {
                offY = 20;
            }
        }

        if (Underwater && Shield != 3) {
            if (fcount % 60 == 0) {
                sc->particles[sc->part_next++] = new Particle(tex_BubbleFX, x, y, 10 * 16, 10 * 16, 16, true, -1/16.0f, -1, 2, true);
                sc->part_next = (sc->part_next + 1) % sc->part_max;
            }
            if (fcount % 60 == 20) {
                sc->particles[sc->part_next] = new Particle(tex_BubbleFX, x, y, 10 * 16, 10 * 16, 16, true, -1/16.0f, -1, 2, true);
                sc->part_next = (sc->part_next + 1) % sc->part_max;
            }
        }

        fcount++;
    }
    else if (Controllable) {
        if (jumpHold) {
            x = (int)x + (app->input->right[0] - app->input->left[0]) * (app->input->jump[0] + app->input->jump2[0]) * 6;
            y = (int)y + (app->input->down[0]  -   app->input->up[0]) * (app->input->jump[0] + app->input->jump2[0]) * 6;
        }
        else {
            x = (int)x + (app->input->rightPressed[0] - app->input->leftPressed[0]);
            y = (int)y + (app->input->downPressed[0]  -   app->input->upPressed[0]);
        }
        Speed = 0;
        Gravity = 0;
    }
    else if (sc->DebugMode) {
        Player *FirstPlayer = sc->playerBuffer[0];
        x = FirstPlayer->x;
        y = FirstPlayer->y;
        Speed = 0;
        Gravity = 0;
    }

    dispX = (int)x + (float)std::sin(toRadians(Angle)) * offY;
    dispY = y + std::min(0.f, (float)std::cos(toRadians(Angle)) * offY);

    Trail[TrailIndex] = {
        x + (float)std::sin(toRadians(Angle)) * offY,
        y + (float)std::cos(toRadians(Angle)) * offY,
        dispAng,
        dispFlip,
        (int)frm + startAu + AnimationOffset
    };
    TrailIndex = (TrailIndex - 1 + 60) % 60;

    if (Controllable) {
        up = app->input->up[0];
        down = app->input->down[0];
        left = app->input->left[0];
        right = app->input->right[0];
        jump = app->input->jumpPressed[0] || app->input->jump2Pressed[0];
        jumpHold = app->input->jump[0] || app->input->jump2[0];
    }
    if (!Controllable) {
        up = false;
        down = false;
        left = false;
        right = false;
        jump = false;
        jumpHold = false;
    }
}
void Player::render(float camX, float camY) {
    if (!sc->DebugMode || !Controllable) {
        float dZ = app->drawZ;

        if (overAll)
            app->drawZ = -0.82f;

        if ((Rot3D == 12 || Rot3D >= 12 + 6) && ObjectControlled) {
            app->drawZ -= 0.3;
        }
        //if (y - camY > app->gameHeight && !Ground)
            //Action = Actions::Dead;
        if (Action == Actions::Dead)
            app->drawZ = -0.8f;

        if (!Controllable)
            app->drawZ += 0.003f;

        if (Action == Actions::Spindash) {
            app->drawSprite(tex_DashDust,
                (float)ceil(x) - camX,
                (float)ceil(y) - camY, 419 - dispFlip,
                ((fcount / 2) % 7), 1, 1);
        }

        if (WaterRun) {
            app->drawSpriteR(tex_WaterRun,
                ceil(x) - camX,
                ceil(y) - camY + 20,
                ((fcount / 4) % 5),
                0,
                2, 2,
                signum(Speed), 1.0f,
                0);
        }

        if (Underwater) {
            int secondsLeft = UnderwaterTimer / 100;
            int frame = 0;
            if (secondsLeft == 0)
                frame = 4;
            if (secondsLeft == 1)
                frame = 9;
            if (secondsLeft == 2)
                frame = 8;
            if (secondsLeft == 3)
                frame = 7;
            if (secondsLeft == 4)
                frame = 6;
            if (secondsLeft == 5)
                frame = 5;

            if (frame > 0 && UnderwaterTimer > 0) {
                app->drawSpriteR(sc->tex_AirCountdown,
                    (float)ceil(dispX) + (float)std::sin(dispAng) * offY + 16 - camX,
                    (float)ceil(dispY) + (float)std::cos(dispAng) * offY - 32 - camY,
                    frame % 8,
                    frame / 8,
                    1, 1,
                    1.0f, 1.0f,
                    0.0f);
            }
        }

        if (this->Invincibility != 1 || (this->Invincibility == 1 && fcount % 8 < 4)) {
            int ff = -1;

            if (tex_Tails != NULL) {
                int offX = 0;
                float rot = 0;
                bool needrot = false;

                if ((Action == Actions::Normal || Action == Actions::CrouchDown || Action == Actions::Spindash || Action == Actions::Skid)) {
                    if ((Speed == 0 && Ground) || Action == Actions::Skid) {
                        ff = (fcount / 6) % 5 + 20;
                        offX = -5;
                        needrot = false;
                        if (Action == Actions::CrouchDown || Action == Actions::Spindash) {
                            offY = 4;
                        }
                    }
                }
                else if (Action == Actions::Fly && !Underwater) {
                    ff = fcount % 2 + 38;
                    needrot = false;
                }
                else if (Action == Actions::Fly) {
                    ff = -1;
                    needrot = false;
                }
                else if (Action == Actions::Rolling || Action == Actions::Jumping) {
                    ff = fcount % 8;
                    rot = (std::max(-1.0f, std::min(1.0f, Gravity)) + 0.f) * 90.0f;
                    needrot = true;

                    if (Ground) {
                        needrot = false;
                        rot = -toRadians(Angle);
                    }
                }
                else if (
                    Action == Actions::Grab ||
                    Action == Actions::Peril ||
                    Action == Actions::Hurt ||
                    Action == Actions::Spring ||
                    false) {

                }
                else {
                    ff = (fcount / 6) % 5 + 20;
                    offX = -5;
                    needrot = false;
                }

                if (needrot)
                    rot = std::atan2(Gravity * dispFlip, Speed * dispFlip);

                if (ff > -1) {
                    app->drawSpriteR(tex_Tails,
                        (float)ceil(x) + (float)std::sin(dispAng) * offY - offX * dispFlip - camX,
                        (float)ceil(y) + (float)std::cos(dispAng) * offY - camY,
                        ff % 8,
                        ff / 8,
                        1, 1,
                        dispFlip, 1.0f,
                        rot);
                }
            }

            ff = (int)frm + startAu + AnimationOffset;

            if (SuperForm) {
                app->drawSpriteR(tex_Super,
                    (x + (float)std::sin(toRadians(Angle)) * offY - camX),
                    (int)(y) + (float)std::cos(toRadians(Angle)) * offY - camY,
                    ff % 13,
                    ff / 13,
                    1, 1,
                    dispFlip, 1.0f,
                    (-(float)dispAng) * (this->Action == Actions::Rolling ? 0.f : 1.0f) );

                for (int t = 1; t < 8 - 1; t += 2) {
                    TrailData trailPoint = Trail[(TrailIndex + t + 60) % 60];
                    if (TrailSameFrame) trailPoint.frame = ff;
                    app->SetBlend(app->Blend[0], app->Blend[1], app->Blend[2], std::max(0.f, (20 - t) / 40.f + 0.5f));
                    app->drawZ += 0.003f;
                    app->drawSpriteR(tex_Super,
                        trailPoint.x - camX,
                        (int)(trailPoint.y) - camY,
                        trailPoint.frame % 13,
                        trailPoint.frame / 13,
                        1, 1,
                        trailPoint.flip, 1.0f,
                        (-(float)trailPoint.angle) * (Action == Actions::Rolling ? 0.f : 1.0f) );
                }
                app->SetBlend(app->Blend[0], app->Blend[1], app->Blend[2], 1.f);
            }
            else {
                app->drawSpriteR(tex,
                    (x + (float)std::sin(toRadians(Angle)) * offY - camX),
                    (int)(y) + (float)std::cos(toRadians(Angle)) * offY - camY,
                    ff % 13,
                    ff / 13,
                    1, 1,
                    dispFlip, 1.0f,
                    (-(float)dispAng) * (this->Action == Actions::Rolling ? 0.f : 1.0f) );
                if (SpeedSneakers) {
                    for (int t = 1; t < 8 - 1; t += 2) {
                        TrailData trailPoint = Trail[(TrailIndex + t + 60) % 60];
                        if (TrailSameFrame) trailPoint.frame = ff;
                        app->SetBlend(app->Blend[0], app->Blend[1], app->Blend[2], std::max(0.f, (20 - t) / 40.f + 0.5f));
                        app->drawZ += 0.003f;
                        app->drawSpriteR(tex,
                            trailPoint.x - camX,
                            (int)(trailPoint.y) - camY,
                            trailPoint.frame % 13,
                            trailPoint.frame / 13,
                            1, 1,
                            trailPoint.flip, 1.0f,
                            (-(float)trailPoint.angle) * (Action == Actions::Rolling ? 0.f : 1.0f) );
                    }
                    app->SetBlend(app->Blend[0], app->Blend[1], app->Blend[2], 1.f);
                }
            }
        }

        if (Action != Actions::Dead && !SuperForm) {
            if (Invincibility != 2) {
                if (Shield == 1) {
                    fcount %= 16 * 4;
                    int fr = fcount / 4;

                    int ffiv = fr % 16;
                    if (ffiv >= 8) {
                        ffiv += 8;
                    }

                    if (ShieldAction) {
                        fcount %= 16 * 2;
                        fr = fcount / 2;

                        ffiv = fr % 16;
                        ffiv = ffiv % 8 + 8;
                    }

                    if (ffiv >= 16) {
                        app->drawZ = dZ + 0.4;
                    }

                    app->drawSpriteR(tex_Fire,
                        (float)ceil(dispX) + (float)std::sin(dispAng) * offY - camX,
                        (float)ceil(dispY) + (float)std::cos(dispAng) * offY - camY,
                        (ffiv) % 8,
                        (ffiv) / 8,
                        1, 1,
                        dispFlip, 1.0f,
                        0);

                    app->drawZ = dZ;
                }
                else if (Shield == 2) {
                    fcount %= 43 * 2;
                    int fr = electricShieldAnim1[fcount / 2];
                    app->drawSpriteR(tex_Lightning,
                        (float)ceil(dispX) + (float)std::sin(dispAng) * offY - camX,
                        (float)ceil(dispY) + (float)std::cos(dispAng) * offY - camY,
                        (fr % 12) % 8,
                        (fr % 12) / 8,
                        1, 1,
                        1.0f, 1.0f,
                        0);
                }
                else if (Shield == 3) {
                    fcount %= 55 * 2;
                    int fr = bubbleShieldAnim[fcount / 2];
                    if (!ShieldAction) {
                        app->drawSpriteR(tex_Bubble,
                            (float)ceil(dispX) + (float)std::sin(dispAng) * offY - camX,
                            (float)ceil(dispY) + (float)std::cos(dispAng) * offY - camY,
                            (fr % 11) % 8,
                            (fr % 11) / 8,
                            1, 1,
                            1.0f, 1.0f,
                            0);
                    }
                    else {
                        app->drawSpriteR(tex_Bubble,
                            (float)ceil(dispX) + (float)std::sin(dispAng) * offY - camX,
                            (float)ceil(dispY) + (float)std::cos(dispAng) * offY - camY,
                            3,
                            1,
                            1, 1,
                            1.0f, 1.0f,
                            0);
                    }
                }
                else if (Shield == 4) {
                    app->drawSprite(tex_Insta, (float)ceil(dispX) - camX, (float)ceil(dispY) + (float)std::cos(dispAng) * offY - camY, 0, ShieldAnimation, 1, 1);
                }
            }
            else {
                //tex_Invinc
                //zigzag(fcount,8)
                for (int n = 0; n < 6; n++)
                    app->drawSprite(tex_Invinc,
                        (float)ceil(dispX) + (float)std::sin(dispAng) * offY - camX + 16 * std::cos(toRadians((int)(fcount * 6 + n * 60.f) % 360)),
                        (float)ceil(dispY) + (float)std::cos(dispAng) * offY - camY - 16 * std::sin(toRadians((int)(fcount * 6 + n * 60.f) % 360)),
                        0,
                        zigzag(fcount + n * 3, 8), 1, 1);

                for (int n = 0; n < 2; n++)
                    app->drawSprite(tex_Invinc,
                        (float)ceil(dispX) + (float)std::sin(dispAng) * offY - camX + 16 * std::cos(toRadians((int)(fcount * 78.75f + n * 180.f) % 360)),
                        (float)ceil(dispY) + (float)std::cos(dispAng) * offY - camY - 16 * std::sin(toRadians((int)(fcount * 78.75f + n * 180.f) % 360)),
                        0,
                        zigzag(fcount + n * 3, 8), 1, 1);
            }
        }

        app->drawZ = dZ;
    }
    else {
        float dZ = app->drawZ;
        app->drawZ = -0.9f;
        //app->drawRectangle(x - camX - 8, y - camY, 16, 1,    1, 0, 0, 0.5f);
        //app->drawRectangle(x - camX, y - camY - 8, 1, 16,    0, 1, 1, 0.5f);
        app->drawZ = dZ;
    }
}
void Player::doThing() { }
void Player::free() { }

void Player::Vibrate(int controller, double strength, int milliseconds) {
    if (Controllable)
        app->input->Vibrate(controller, strength, milliseconds);
}


S3Object::Terrain S3Object::collision_line(float xr, float yr, float x2, float y2) {
    float X2 = std::round(xr);
    float Y2 = std::round(yr);
    float X3 = std::round(x2);
    float Y3 = std::round(y2);
    if (sc->loop)
        Y2 = fmod((Y2 + sc->levelH * 128), (sc->levelH * 128));

    float angle = 0;
    float len = 0;

    len = std::sqrt((Y2 - Y3) * (Y2 - Y3) + (X3 - X2) * (X3 - X2));

    if (len > 0) {
        angle = std::atan2(Y2 - Y3, X3 - X2);
    }
    else
        angle = 0;

    //angle = scrWrapAngleRad(angle);

    for (int n = 0; n <= len; n++) {
        int chunkX = (int)(X2 / 128);
        int chunkY = (int)(Y2 / 128);
        if (Y2 < 0) {
            return S3Object::Terrain { false, -1, true };
        }
        int chunkID = sc->level[(sc->level[8] & 0xFF) * 0x100 + (sc->level[9] & 0xFF) - 0x8000 + chunkX + chunkY * sc->levelW] & 0xFF;

        int blockX = (int)(X2 / 16) - chunkX * 8;
        int blockY = (int)(Y2 / 16) - chunkY * 8;
        int blockData = sc->chunks[chunkID * 192 + blockX * 24 + blockY * 3] & 0xFF;

        int heightID1 = sc->chunks[chunkID * 192 + blockX * 24 + blockY * 3 + 1] & 0xFF;
        int heightID2 = sc->chunks[chunkID * 192 + blockX * 24 + blockY * 3 + 2] & 0xFF;

        int xToCheck = (int)X2 - chunkX * 128 - blockX * 16;
        int yToCheck = (int)Y2 - chunkY * 128 - blockY * 16;

        int solid1 = (blockData >> 6) & 0x3;
        int solid2 = (blockData >> 4) & 0x3;
        int flipY = (blockData >> 3) & 0x1;
        int flipX = (blockData >> 2) & 0x1;

        int height1 = 0;
        int height2 = 0;

        if (flipX == 1) {
            height1 = sc->heights[heightID1 * 16 + 15 - xToCheck] & 0xFF;
            height2 = sc->heights[heightID2 * 16 + 15 - xToCheck] & 0xFF;
        }
        else {
            height1 = sc->heights[heightID1 * 16 + xToCheck] & 0xFF;
            height2 = sc->heights[heightID2 * 16 + xToCheck] & 0xFF;
        }

        int angleFinal = -1;
        if (solid1 > 0 && this->Layer == 1) {
            angleFinal = sc->angles[heightID1] & 0xFF;
        }
        if (solid2 > 0 && this->Layer == 0) {
            angleFinal = sc->angles[heightID2] & 0xFF;
        }

        if (angleFinal != -1) {
            if ((solid1 > 0 ||
                solid2 > 0) && angleFinal != 0xFF) { // if not top-solid
                if (flipX == 0) {
                    angleFinal = 255 - angleFinal;
                }
                if (flipY == 1) {
                    angleFinal = (127 - angleFinal) % 255;
                }
            }

            if (angleFinal == 0xFF)
                angleFinal = 0;
        }

        if (solid1 > 0 && this->Layer == 1) {
            bool collided = false;
            if (height1 <= 16) {
                if ((yToCheck >= 16 - height1 && (flipY == 0 || solid1 == 1)) ||
                    (yToCheck <       height1 && flipY == 1 && solid1 != 1))
                    collided = true;
            }
            else {
                if ((yToCheck >= 16 - (255 - height1) && flipY == 1 && solid1 != 1) ||
                    (yToCheck <       (255 - height1) && (flipY == 0 || solid1 == 1)))
                    collided = true;
            }
            if (solid1 == 1) {
                if (collided && std::sin(angle * Math_PI / 180.f) * signum(Speed) > 0.98) {
                    collided = false;
                }
            }
            if (collided) {
                return S3Object::Terrain { true, this->Layer, solid1 == 1, 0, height1, (int)X2, (int)Y2, 0 /*scrWrapAngle((int)(angleFinal * 360.0f / 256.0f))*/ };
            }
        }
        if (solid2 > 0 && this->Layer == 0) {
            bool collided = false;
            if (height2 <= 16) {
                if ((yToCheck >= 16 - height2 && flipY == 0) ||
                    (yToCheck <       height2 && flipY == 1))
                    collided = true;
            }
            else {
                if ((yToCheck >= 16 - (255 - height2) && flipY == 1) ||
                    (yToCheck <       (255 - height2) && flipY == 0))
                    collided = true;
            }
            if (solid2 == 1) {
                if (collided && std::sin(angle * Math_PI / 180.f) * signum(Speed) > 0.98) {
                    collided = false;
                }
            }
            if (collided) {
                return S3Object::Terrain { true, this->Layer, solid2 == 1, 0, height2, (int)X2, (int)Y2, 0 /*scrWrapAngle((int)(angleFinal * 360.0f / 256.0f))*/ };
            }
        }

        X2 += std::cos(angle);
        Y2 -= std::sin(angle);

        if (sc->loop)
            Y2 = fmod((Y2 + sc->levelH * 128), (sc->levelH * 128));
    }
    return S3Object::Terrain { false, -1, true };
}
