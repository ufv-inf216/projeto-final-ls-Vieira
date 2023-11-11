//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include <algorithm>
#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

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
   return mOwner->GetPosition() + mOffset;
}

Vector2 AABBColliderComponent::GetMax() const
{
    return GetMin() + Vector2(mWidth, mHeight);
}

Vector2 AABBColliderComponent::GetCenter() const
{
    return GetMin() + Vector2(mWidth/2, mHeight/2);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const
{
    bool notColliding = GetMax().x < b.GetMin().x || b.GetMax().x < GetMin().x || GetMax().y < b.GetMin().y || b.GetMax().y < GetMin().y;
    return !notColliding;
}

AABBColliderComponent::Overlap AABBColliderComponent::GetMinOverlap(AABBColliderComponent* b) const
{
    std::unordered_map<int, Overlap> overlaps;

    overlaps[0] = Overlap{b->GetMax().y - GetMin().y, CollisionSide::Top, b};
    overlaps[1] = Overlap{b->GetMin().y - GetMax().y, CollisionSide::Down, b};
    overlaps[2] = Overlap{b->GetMax().x - GetMin().x, CollisionSide::Left, b};
    overlaps[3] = Overlap{b->GetMin().x - GetMax().x, CollisionSide::Right, b};

    Overlap minOverlap = overlaps.begin()->second;

    for(auto overlap: overlaps)
        if(abs(overlap.second.amount) < abs(minOverlap.amount))
            minOverlap = overlap.second;

    return minOverlap;
}

void AABBColliderComponent::ResolveCollisions(RigidBodyComponent *rigidBody, const Overlap& minOverlap)
{
    bool collisionTop = minOverlap.side == CollisionSide::Top && rigidBody->GetVelocity().y < 0;
    bool collisionDown = minOverlap.side == CollisionSide::Down && rigidBody->GetVelocity().y > 0;
    if(collisionTop || collisionDown){
        mOwner->SetPosition(mOwner->GetPosition() + Vector2(0, minOverlap.amount));
        rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0));
    }

//    if(this->GetLayer() == ColliderLayer::Blocks) return;

    bool collisionLeft = minOverlap.side == CollisionSide::Left && rigidBody->GetVelocity().x < 0;
    bool collisionRight = minOverlap.side == CollisionSide::Right && rigidBody->GetVelocity().x > 0;
    if(collisionLeft || collisionRight){
        mOwner->SetPosition(mOwner->GetPosition() + Vector2(minOverlap.amount, 0));
        rigidBody->SetVelocity(Vector2(0, rigidBody->GetVelocity().y));
    }
}

void AABBColliderComponent::DetectCollision(RigidBodyComponent *rigidBody)
{
    if(!this->IsEnabled()) return;

    auto colliders = mOwner->GetGame()->GetColliders();

    std::sort(colliders.begin(), colliders.end(), [this](AABBColliderComponent* a, AABBColliderComponent* b) {
        return (a->GetCenter() - this->GetCenter()).Length() < (b->GetCenter() - this->GetCenter()).Length();
    });

    std::unordered_map<CollisionSide, Overlap> responses;

    for(auto c: colliders){
        if(c == this || !c->IsEnabled()) continue;


        if(Intersect(*c)){
            Overlap minOverlap = this->GetMinOverlap(c);
            this->ResolveCollisions(rigidBody, minOverlap);
            responses.insert(std::make_pair(minOverlap.side, minOverlap));
        }

        bool verticalCollision = responses.find(CollisionSide::Top) != responses.end() || responses.find(CollisionSide::Down) != responses.end();
        bool horizontalCollision = responses.find(CollisionSide::Left) != responses.end() || responses.find(CollisionSide::Right) != responses.end();
        if(verticalCollision && horizontalCollision) break;
    }

    mOwner->OnCollision(responses);
}