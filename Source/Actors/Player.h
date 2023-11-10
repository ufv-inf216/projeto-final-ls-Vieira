//
// Created by julio on 10/11/23.
//

#ifndef SUPER_CAPI_FIGHTERS_PLAYER_H
#define SUPER_CAPI_FIGHTERS_PLAYER_H

#pragma once
#include "Actor.h"

class Player: public Actor {
public:
    explicit Player(Game* game, Vector2 position, int playerNumber, float forwardSpeed = 2500.0f, float jumpSpeed = -5000.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;
    void OnCollision(std::unordered_map<CollisionSide, AABBColliderComponent::Overlap>& responses) override;

    void Kill() override;

private:
    void ManageAnimations();

    class RigidBodyComponent* mRigidBodyComponent;
    class DrawAnimatedComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    class DrawPolygonComponent* mDrawPolygonComponent;

    bool mIsMoving;
    bool mIsDead;
    float mForwardSpeed;
    float mJumpSpeed;

    int mPlayerNumber;
};


#endif //SUPER_CAPI_FIGHTERS_PLAYER_H
