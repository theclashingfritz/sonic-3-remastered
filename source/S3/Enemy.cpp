#include "Object.h"
#include <LevelScene.h>

int  Enemy::OnCollisionWithPlayer(int PlayerID, int HitFrom, int Data) {
    IPlayer* Player = Scene->GetPlayer(PlayerID);
    if ((Player->Action == ActionType::Jumping ||
        Player->Action == ActionType::Rolling ||
        Player->Action == ActionType::Spindash ||
        Player->Action == ActionType::Glide ||
        Player->Shield == ShieldType::Instashield ||
        Player->Invincibility == InvincibilityType::Full) && !Invincible &&
        (Player->Shield == ShieldType::None || (Player->Shield != ShieldType::None && !BounceOffShield))) {
        Player->EnemyCombo++;
        if (Player->Y < Y && Player->Gravity > 0) {
            Player->Gravity = -Player->Gravity;
            if (!Player->InputJumpHold)
                Player->Gravity = std::max(Player->Gravity, -1.0f);
        }
        else {
            Player->Gravity -= signum(Player->Gravity);
        }
        Player->Vibrate(0, 0.5, 500);
        Scene->givePoint(0, Player->EnemyCombo, X, Y);
        DoExplode();
    }
    else if (Player->Shield != ShieldType::None && BounceOffShield) {
        float angle = std::atan2(Player->Y - Y, X - Player->X);

        Speed   = 8 * std::cos(angle);
        Gravity = 8 * -std::sin(angle);
        Harmful = false;
    }
    else if (Harmful) {
        Player->Hurt(X, false);
    }
	return 1;
}
void Enemy::DoExplode() {
    Scene->aud_BadnikDestroyed->Play(3);

    IEnemyExplosion* obj = new IEnemyExplosion();
    obj->X = X;
    obj->Y = Y;
    obj->G = G;
    obj->App = App;
    obj->Scene = Scene;
    Scene->Particles[Scene->ParticleCount] = obj;
    Scene->ParticleCount = (Scene->ParticleCount + 1) % 50;
    obj->Create();

    Active = false;
}
