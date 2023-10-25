//
// Created by Lucas N. Ferreira on 08/09/23.
//

#include <SDL.h>
#include "../Actors/Actor.h"
#include "../Game.h"
#include "RigidBodyComponent.h"
#include "ColliderComponents/AABBColliderComponent.h"

const float MAX_SPEED_X = 300.0f;
const float MAX_SPEED_Y = 700.0f;
const float GRAVITY = 2000.0f;

RigidBodyComponent::RigidBodyComponent(class Actor* owner, float mass, float friction, int updateOrder, bool isStatic)
        :Component(owner, updateOrder)
        ,mMass(mass)
        ,mFrictionCoefficient(friction)
        ,mVelocity(Vector2::Zero)
        ,mAcceleration(Vector2::Zero),
        mIsStatic(isStatic)
{

}

void RigidBodyComponent::ApplyForce(const Vector2 &force) {
    mAcceleration += force * (1.f/mMass);
}

void RigidBodyComponent::Update(float deltaTime)
{

    // Apply gravity acceleration
    if(!mIsStatic){
        ApplyForce(Vector2::UnitY * GRAVITY);
    }

    // Apply friction
    if(mVelocity.x != 0.0f) {
        ApplyForce(Vector2::UnitX * -mFrictionCoefficient * mVelocity.x);
    }

    // Euler Integration
    mVelocity += mAcceleration * deltaTime;

    mVelocity.x = Math::Clamp<float>(mVelocity.x, -MAX_SPEED_X, MAX_SPEED_X);
    mVelocity.y = Math::Clamp<float>(mVelocity.y, -MAX_SPEED_Y, MAX_SPEED_Y);

    if(Math::NearZero(mVelocity.x, 1.0f)) {
        mVelocity.x = 0.f;
    }

    Vector2 position = mOwner->GetPosition();
    mOwner->SetPosition(position + mVelocity * deltaTime);

    auto collider = mOwner->GetComponent<AABBColliderComponent>();
    if (collider) {
        collider->DetectCollision(this);
    }

    mAcceleration.Set(0.f, 0.f);
}

Vector2 RigidBodyComponent::GetMaxSpeed() const {
    return Vector2(MAX_SPEED_X, MAX_SPEED_Y);
}
