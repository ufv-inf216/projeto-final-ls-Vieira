//
// Created by julio on 10/11/23.
//

#include "Player.h"
#include "../Components/ColliderComponents/AABBColliderComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Game.h"

#define FLOOR_HEIGHT 50.0f

Player::Player(Game *game, Vector2 position, int playerNumber, CharacterSelect characterSelect, float forwardSpeed, float jumpSpeed)
        : Actor(game),
          mPlayerNumber(playerNumber),
          mCharacterSelect(characterSelect),
          mForwardSpeed(forwardSpeed),
          mJumpSpeed(jumpSpeed),
          mIsDead(false),
          mIsOnGround(true),
          mIsJumping(false),
          mIsDown(false),
          mIsBlocking(false),
          mIsPunching(false),
          mIsKicking(false),
          mIsMoving(false) {

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 2.5f);
    const float width = 330.0f;
    const float height = 330.0f;
    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height, ColliderLayer::Player);

    mDrawPolygonComponent = new DrawPolygonComponent(this, width, height);

    mRotation = mPlayerNumber == 1 ? Math::Pi : 0.0f;

    mCharacter = new Character();

    mDrawComponent = new DrawAnimatedComponent(this, mCharacter->GetCharacterSpriteSheetPath(mCharacterSelect), mCharacter->GetCharacterSpriteSheetJSON(mCharacterSelect));

    mDrawComponent->AddAnimation("idle", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Idle));
    mDrawComponent->AddAnimation("move", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Move));
    mDrawComponent->AddAnimation("jump", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Jump));
    mDrawComponent->AddAnimation("down", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Down));
    mDrawComponent->AddAnimation("block", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Block));
    mDrawComponent->AddAnimation("punch", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Punch));
    mDrawComponent->AddAnimation("kick", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Kick));
    mDrawComponent->AddAnimation("dead", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Dead));

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(7.0f);
}

void Player::OnProcessInput(const uint8_t *state) {
        if(state[SDL_SCANCODE_ESCAPE]){
            mIsDead = true;
        }

        if (mPlayerNumber == 1 && state[SDL_SCANCODE_D] || mPlayerNumber == 2 && state[SDL_SCANCODE_RIGHT]) {
            mRigidBodyComponent->ApplyForce(Vector2(mForwardSpeed, 0.0f));
            mIsMoving = true;
        }

        if (mPlayerNumber == 1 && state[SDL_SCANCODE_A] || mPlayerNumber == 2 && state[SDL_SCANCODE_LEFT]) {
            mRigidBodyComponent->ApplyForce(Vector2(-mForwardSpeed, 0.0f));
            mIsMoving = true;
        }

        if (mPlayerNumber == 1 && !state[SDL_SCANCODE_D] && !state[SDL_SCANCODE_A] || mPlayerNumber == 2 && !state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LEFT]) {
            mIsMoving = false;
        }

        if (mPlayerNumber == 1 && state[SDL_SCANCODE_W] || mPlayerNumber == 2 && state[SDL_SCANCODE_UP]) {
            if(mIsOnGround){
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpSpeed));
                mIsOnGround = false;
                mIsJumping = true;
            }
        } else {
            mIsJumping = false;

            if(mPlayerNumber == 1 && state[SDL_SCANCODE_E] || mPlayerNumber == 2 && state[SDL_SCANCODE_KP_1]) {
                mIsBlocking = true;
                mIsPunching = false;
                mIsKicking = false;
            } else {
                mIsBlocking = false;
            }

            if(mPlayerNumber == 1 && state[SDL_SCANCODE_R] || mPlayerNumber == 2 && state[SDL_SCANCODE_KP_2]) {
                mIsPunching = true;
                mIsBlocking = false;
                mIsKicking = false;
            } else {
                mIsPunching = false;
            }

            if(mPlayerNumber == 1 && state[SDL_SCANCODE_T] || mPlayerNumber == 2 && state[SDL_SCANCODE_KP_3]) {
                mIsKicking = true;
                mIsBlocking = false;
                mIsPunching = false;
            } else {
                mIsKicking = false;
            }
        }

        if(mPlayerNumber == 1 && state[SDL_SCANCODE_S] || mPlayerNumber == 2 && state[SDL_SCANCODE_DOWN]) {
            mIsDown = true;
        } else {
            mIsDown = false;
        }
}

void Player::OnUpdate(float deltaTime) {
    if (mRigidBodyComponent->GetOwner()->GetPosition().x < mGame->GetCameraPos().x)
        mRigidBodyComponent->GetOwner()->SetPosition(Vector2(mGame->GetCameraPos().x, mRigidBodyComponent->GetOwner()->GetPosition().y));

    if (mRigidBodyComponent->GetOwner()->GetPosition().x + mColliderComponent->GetWidth()> mGame->GetWindowWidth())
        mRigidBodyComponent->GetOwner()->SetPosition(Vector2(mGame->GetWindowWidth() - mColliderComponent->GetWidth(), mRigidBodyComponent->GetOwner()->GetPosition().y));

    if (mRigidBodyComponent->GetOwner()->GetPosition().y + mColliderComponent->GetHeight() > mGame->GetWindowHeight() - FLOOR_HEIGHT){
        mRigidBodyComponent->GetOwner()->SetPosition(Vector2(mRigidBodyComponent->GetOwner()->GetPosition().x, mGame->GetWindowHeight() - mColliderComponent->GetHeight() - FLOOR_HEIGHT));
        mIsOnGround = true;
        mIsJumping = false;
    }

    ManageAnimations();
}

void Player::ManageAnimations() {
    mDrawComponent->SetIsPaused(false);

    if(mIsDead) {
        mDrawComponent->SetAnimation("dead");
    } else if(mIsOnGround) {
        if(mIsMoving) {
            mDrawComponent->SetAnimation("move");
        } else if(mIsDown) {
            mDrawComponent->SetAnimation("down");
        } else if (mIsBlocking) {
            mDrawComponent->SetAnimation("block");
            mDrawComponent->SetIsPaused(true);
        } else if (mIsPunching){
            mDrawComponent->SetAnimation("punch");
        } else if (mIsKicking){
            mDrawComponent->SetAnimation("kick");
        } else {
            mDrawComponent->SetAnimation("idle");
        }
    } else {
        if(mIsJumping) {
            mDrawComponent->SetAnimation("jump");
        }
    }
}

void Player::Kill() {
    mDrawComponent->SetAnimation("dead");
    mIsDead = true;
    mColliderComponent->SetEnabled(false);
}

void Player::OnCollision(std::unordered_map<CollisionSide, AABBColliderComponent::Overlap> &responses) {
    for (auto &response : responses) {
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