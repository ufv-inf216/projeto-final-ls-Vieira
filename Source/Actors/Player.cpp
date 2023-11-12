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
          mIsMoving(false),
          mFightStatus(FightStatus::Fight) {

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
    mDrawComponent->AddAnimation("jump_block", mCharacter->GetStateArray(mCharacterSelect, CharacterState::JumpBlock));
    mDrawComponent->AddAnimation("down_block", mCharacter->GetStateArray(mCharacterSelect, CharacterState::DownBlock));
    mDrawComponent->AddAnimation("punch", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Punch));
    mDrawComponent->AddAnimation("jump_punch", mCharacter->GetStateArray(mCharacterSelect, CharacterState::JumpPunch));
    mDrawComponent->AddAnimation("down_punch", mCharacter->GetStateArray(mCharacterSelect, CharacterState::DownPunch));
    mDrawComponent->AddAnimation("kick", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Kick));
    mDrawComponent->AddAnimation("jump_kick", mCharacter->GetStateArray(mCharacterSelect, CharacterState::JumpKick));
    mDrawComponent->AddAnimation("down_kick", mCharacter->GetStateArray(mCharacterSelect, CharacterState::DownKick));
    mDrawComponent->AddAnimation("dead", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Dead));
    mDrawComponent->AddAnimation("win", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Win));
    mDrawComponent->AddAnimation("lose", mCharacter->GetStateArray(mCharacterSelect, CharacterState::Lose));

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(7.0f);
}

void Player::OnProcessInput(const uint8_t *state) {
        if(state[SDL_SCANCODE_I]){ // FIXME Only to test animation
            mIsDead = true;
        }
        if(state[SDL_SCANCODE_O]){ // FIXME Only to test animation
            mFightStatus = FightStatus::Win;
        }
        if(state[SDL_SCANCODE_P]){ // FIXME Only to test animation
            mFightStatus = FightStatus::Lose;
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
        }

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

    if(mFightStatus == FightStatus::Win)
        mDrawComponent->SetAnimation("win");
    else if(mFightStatus == FightStatus::Lose)
        mDrawComponent->SetAnimation("lose");
    else {
        if(mIsDead) {
            mDrawComponent->SetAnimation("dead");
        } else if(mIsOnGround) {
            if(mIsMoving) {
                mDrawComponent->SetAnimation("move");
            } else if(mIsDown) {
                if(mIsBlocking){
                    mDrawComponent->SetAnimation("down_block");
                    mDrawComponent->SetIsPaused(true);
                } else if (mIsPunching){
                    mDrawComponent->SetAnimation("down_punch");
                } else if (mIsKicking){
                    mDrawComponent->SetAnimation("down_kick");
                } else {
                    mDrawComponent->SetAnimation("down");
                    mDrawComponent->SetIsPaused(true);
                }
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
                if(mIsBlocking){
                    mDrawComponent->SetAnimation("jump_block");
                } else if (mIsPunching){
                    mDrawComponent->SetAnimation("jump_punch");
                } else if (mIsKicking){
                    mDrawComponent->SetAnimation("jump_kick");
                } else {
                    mDrawComponent->SetAnimation("jump");
                }
            }
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