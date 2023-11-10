//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include <algorithm>

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h, ColliderLayer layer, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(Vector2((float)dx, (float)dy))
        ,mWidth(w)
        ,mHeight(h)
        ,mLayer(layer)
{
    mOwner->GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
    mOwner->GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
    return this->mOffset + this->mOwner->GetPosition();
}

Vector2 AABBColliderComponent::GetMax() const
{
    return Vector2(this->mWidth + this->GetMin().x, this->mHeight + this->GetMin().y);
}

Vector2 AABBColliderComponent::GetCenter() const
{
    return Vector2(this->mWidth/2 + this->GetMin().x, this->mHeight/2 + this->GetMin().y);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const
{
    // Calculate the min and max values for this AABB
    Vector2 thisMin = GetMin();
    Vector2 thisMax = GetMax();

    // Calculate the min and max values for the AABB b
    Vector2 bMin = b.GetMin();
    Vector2 bMax = b.GetMax();

    // Check for intersection
    if (thisMin.x <= bMax.x && thisMax.x >= bMin.x &&
        thisMin.y <= bMax.y && thisMax.y >= bMin.y) {
        return true; // AABBs are intersecting
    }

    return false;
}

AABBColliderComponent::Overlap AABBColliderComponent::GetMinOverlap(AABBColliderComponent* b) const
{
    std::unordered_map<int, Overlap> overlaps;

    Vector2 thisMin = GetMin();
    Vector2 thisMax = GetMax();
    Vector2 bMin = b->GetMin();
    Vector2 bMax = b->GetMax();
    Overlap left;
    Overlap rigth;
    Overlap top;
    Overlap down;

    left.amount = bMax.x - thisMin.x;
    left.side = CollisionSide::Left;
    left.target = const_cast<AABBColliderComponent*>(this);

    rigth.amount = bMin.x - thisMax.x;
    rigth.side = CollisionSide::Right;
    rigth.target = const_cast<AABBColliderComponent*>(this);

    top.amount = bMax.y - thisMin.y;
    top.side = CollisionSide::Top;
    top.target = const_cast<AABBColliderComponent*>(this);

    down.amount =  bMin.y - thisMax.y;
    down.side = CollisionSide::Down;
    down.target = const_cast<AABBColliderComponent*>(this);

    overlaps[0] = left;
    overlaps[1] = rigth;
    overlaps[2] = top;
    overlaps[3] = down;

    Overlap minOverlap = overlaps.begin()->second;
    for (const auto& overlap : overlaps) {
        if (std::abs(overlap.second.amount) < std::abs(minOverlap.amount)) {
            minOverlap = overlap.second;
        }
    }

    return minOverlap;
}

void AABBColliderComponent::ResolveCollisions(RigidBodyComponent *rigidBody, const Overlap& minOverlap)
{
    if (minOverlap.side == CollisionSide::Top && rigidBody->GetVelocity().y < 0) {
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x, mOwner->GetPosition().y + minOverlap.amount));
        rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0));
    } else if (minOverlap.side == CollisionSide::Down && rigidBody->GetVelocity().y > 0) {
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x, mOwner->GetPosition().y + minOverlap.amount));
        rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0));
    } else if (minOverlap.side == CollisionSide::Left && rigidBody->GetVelocity().x < 0) {
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x + minOverlap.amount, mOwner->GetPosition().y));
        rigidBody->SetVelocity(Vector2(0, rigidBody->GetVelocity().y));
    } else if (minOverlap.side == CollisionSide::Right && rigidBody->GetVelocity().x > 0) {
        mOwner->SetPosition(Vector2(mOwner->GetPosition().x + minOverlap.amount, mOwner->GetPosition().y));
        rigidBody->SetVelocity(Vector2(0, rigidBody->GetVelocity().y));
    }

    return;
}

void AABBColliderComponent::DetectCollision(RigidBodyComponent *rigidBody)
{

    auto colliders = mOwner->GetGame()->GetColliders();


    std::sort(colliders.begin(), colliders.end(), [this](const AABBColliderComponent* a, const AABBColliderComponent* b) {
        float distanceA = (a->GetCenter() - this->GetCenter()).LengthSq();
        float distanceB = (b->GetCenter() - this->GetCenter()).LengthSq();
        return distanceA < distanceB;
    });
    std::unordered_map<CollisionSide, Overlap> responses;
    bool verticalCollision = false;
    bool horizontalCollision = false;

    for (const auto& collider : colliders) {
        if (collider == this || !collider->IsEnabled()) {
            continue;
        }

        if(Intersect(*collider)){
            auto minOverlap = GetMinOverlap(collider);
            ResolveCollisions(rigidBody, minOverlap);
            responses.insert(std::make_pair(minOverlap.side, minOverlap));

            if (minOverlap.side == CollisionSide::Top || minOverlap.side == CollisionSide::Down) {
                verticalCollision = true;
            } else if (minOverlap.side == CollisionSide::Left || minOverlap.side == CollisionSide::Right) {
                horizontalCollision = true;
            }
        }



        // Check if both vertical and horizontal collisions occurred
        if (verticalCollision && horizontalCollision) {
            break;
        }
    }

    // Callback only for closest (first) collision
    mOwner->OnCollision(responses);
}