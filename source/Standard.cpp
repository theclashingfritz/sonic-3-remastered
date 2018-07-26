#include "Standard.h"
#include "TextureAudio.h"

// Global Functions
#define toStr(str) #str

float Math_PI = 3.14159265358979323846f;
static int pow10[10] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

int TenTo(int n) { return pow10[n]; }

char ZoneNameLetterWidth[27] = {
    10, 13, 13, 14, 10, // A B C D E
    10, 12, 10,  3,  7, // F G H I J
    10,  7, 24,  9, 24, // K L M N O
    11, 24, 11,  9, 10, // P Q R S T
    8, 13, 24, 14, 12, 11, 12 };
	
char getZoneNameLetterWidth(int f) { return ZoneNameLetterWidth[f]; }

int mod(int a, int n) { return a - (a / n) * n; }
int modulo(int a, int b) { return a >= 0 ? a % b : ( b - abs ( a%b ) ) % b; }

int HUD_PlaceOfNum(int num, int ind, int y) { return mod(num, pow10[y - ind]) / pow10[y - ind - 1]; }

float toRadians(float n) { return n * Math_PI / 180.0f; }
float toDegrees(float n) { return n * 180.0f / Math_PI; }

float wrapAngle(float angle) {
    while (angle < 0) angle += 360;
    while (angle >= 360) angle -= 360;
    return angle;
}

float wrapAngleRad(float angle) {
    while (angle < 0.0f) angle += Math_PI * 2.0f;
    while (angle >= Math_PI * 2.0f) angle -= Math_PI * 2.0f;
    return angle;
}

float clamp(float val, int lo, int hi) { return std::min(std::max(val, (float)lo), (float)hi); }

float signum(float n) {
    if (n > 0) return 1;
    if (n < 0) return -1;
    return 0;
}

int numPlaces(int n) {
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    return 10;
}

int zigzag(int f, int max) {
    if (f % (max * 2) < max) {
        return f % (max * 2);
    }
    return (max * 2) - f % (max * 2) - 1;
}

float zigzag(float f, float max) {
    if (fmod(f, (max * 2)) < max) {
        return fmod(f, (max * 2));
    }
    return (max * 2) - fmod(f, (max * 2)) - 1;
}

float ZigZagCosine(float x) {
    if (fmod(x, 2 * Math_PI) < Math_PI)
        return -2 * fmod(1 / Math_PI * x, 1) + 1;
    else
        return +2 * fmod(1 / Math_PI * x, 1) - 1;
}

float slope(float from, float to, float percent) {
    return std::max(from, std::min(from + (to - from) * percent, to));
}

size_t LoadRW(SDL_RWops *rw, void *ptr) {
    size_t len = (size_t)SDL_RWsize(rw);
    SDL_RWread(rw, ptr, len, 1);
    return len;
}

size_t LoadRWX(SDL_RWops *rw, void *ptr) {
    size_t len = (size_t)SDL_RWsize(rw);
    SDL_RWread(rw, ptr, len, 1);
    SDL_RWclose(rw); rw = NULL;
    return len;
}

int Log2(int x) {
    x &= 0xF;
    int res = 0;
    while (x >>= 1) res++;
    return res;
}