//
// Created by julio on 10/11/23.
//

#ifndef SUPER_CAPI_FIGHTERS_GROUND_H
#define SUPER_CAPI_FIGHTERS_GROUND_H

#pragma once
#include "Actor.h"


class Ground: public Actor {
public:
    explicit Ground(Game* game, Vector2 position = Vector2::Zero);

private:
    Vector2 mStartPosition;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class AABBColliderComponent* mColliderComponent;
    class RigidBodyComponent* mRigidBodyComponent;
};


#endif //SUPER_CAPI_FIGHTERS_GROUND_H
