//
// Created by julio on 10/11/23.
//

#include "Player.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Game.h"

Player::Player(Game *game, Vector2 position, int playerNumber, float forwardSpeed, float jumpSpeed)
        : Actor(game),
          mIsMoving(false),
          mPlayerNumber(playerNumber),
          mIsDead(false),
          mForwardSpeed(forwardSpeed),
          mJumpSpeed(jumpSpeed) {

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 2.5f);
    const float width = 330.0f;
    const float height = 330.0f;
    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height, ColliderLayer::Player);

    mDrawPolygonComponent = new DrawPolygonComponent(this, width, height);

    mRotation = mPlayerNumber == 1 ? Math::Pi : 0.0f;

    if(mPlayerNumber == 1) {
        mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Goku/Goku.png", "../Assets/Sprites/Goku/Goku.json");
    } else if (mPlayerNumber == 2) {
        mDrawComponent = new DrawAnimatedComponent(this, "../Assets/Sprites/Vegeta/Vegeta.png", "../Assets/Sprites/Vegeta/Vegeta.json");
    }

    mDrawComponent->AddAnimation("idle", {14, 15, 16});
    mDrawComponent->AddAnimation("move", {18, 19, 20, 21, 22});
    mDrawComponent->AddAnimation("jump", {31, 32, 33, 34, 35, 36, 37, 38});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(9.0f);


}

void Player::OnProcessInput(const uint8_t *state) {
    if(mPlayerNumber == 1) {
        if (state[SDL_SCANCODE_D]) {
            mRigidBodyComponent->ApplyForce(Vector2(mForwardSpeed, 0.0f));
            mIsMoving = true;
        }

        if (state[SDL_SCANCODE_A]) {
            mRigidBodyComponent->ApplyForce(Vector2(-mForwardSpeed, 0.0f));
            mIsMoving = true;
        }

        if (!state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A]) {
            mIsMoving = false;
        }

        if (state[SDL_SCANCODE_W]) {
            if(mIsOnGround){
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
                SetOffGround();
            }
        }
    } else if (mPlayerNumber == 2) {
        if (state[SDL_SCANCODE_RIGHT]) {
            mRigidBodyComponent->ApplyForce(Vector2(mForwardSpeed, 0.0f));
            mIsMoving = true;
        }

        if (state[SDL_SCANCODE_LEFT]) {
            mRigidBodyComponent->ApplyForce(Vector2(-mForwardSpeed, 0.0f));
            mIsMoving = true;
        }

        if (!state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LEFT]) {
            mIsMoving = false;
        }

        if (state[SDL_SCANCODE_UP]) {
            if(mIsOnGround){
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
                SetOffGround();
            }
        }
    }
}

void Player::OnUpdate(float deltaTime) {
    if (mRigidBodyComponent->GetVelocity().x < 0 && mRigidBodyComponent->GetOwner()->GetPosition().x < mGame->GetCameraPos().x)
        mRigidBodyComponent->GetOwner()->SetPosition(Vector2(mGame->GetCameraPos().x, mRigidBodyComponent->GetOwner()->GetPosition().y));

    if (mRigidBodyComponent->GetVelocity().x > 0 && mRigidBodyComponent->GetOwner()->GetPosition().x > mGame->GetWindowWidth())
        mRigidBodyComponent->GetOwner()->SetPosition(Vector2(mGame->GetWindowWidth(), mRigidBodyComponent->GetOwner()->GetPosition().y));

    ManageAnimations();
}

void Player::ManageAnimations() {
    if(mIsDead) {
//        mDrawComponent->SetAnimation("dead");
    } else if(mIsOnGround && !mIsMoving) {
        mDrawComponent->SetAnimation("idle");
    } else if(mIsOnGround && mIsMoving) {
        mDrawComponent->SetAnimation("move");
    } else if(!mIsOnGround) {
        mDrawComponent->SetAnimation("jump");
    }
}

void Player::Kill() {
    mDrawComponent->SetAnimation("dead");
    mIsDead = true;
    mColliderComponent->SetEnabled(false);
}

void Player::OnCollision(std::unordered_map<CollisionSide, AABBColliderComponent::Overlap> &responses) {
//    if(responses.empty())
//        SetOffGround();

    for (auto &response : responses) {
        if (response.second.side == CollisionSide::Down) {
            SetOnGround();
            mIsOnGround = true;
        }
//
//        if(response.second.target->GetLayer() == ColliderLayer::Enemy){
//            if (response.second.side == CollisionSide::Down) {
//                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed/1.5f));
//                response.second.target->GetOwner()->Kill();
//            } else {
//                if(mIsOnGround)
//                    Kill();
//                else {
//                    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed/1.5f));
//                    response.second.target->GetOwner()->Kill();
//                }
//            }
//        }
    }
}