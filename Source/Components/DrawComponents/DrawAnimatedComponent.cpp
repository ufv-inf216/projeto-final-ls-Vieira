//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawAnimatedComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "DrawPolygonComponent.h"
#include <fstream>

DrawAnimatedComponent::DrawAnimatedComponent(class Actor *owner, const std::string &spriteSheetPath, const std::string &spriteSheetData, int drawOrder)
    : DrawSpriteComponent(owner, spriteSheetPath, 0, 0, drawOrder) {
    LoadSpriteSheet(spriteSheetPath, spriteSheetData);
}

DrawAnimatedComponent::~DrawAnimatedComponent() {
    for (const auto &rect : mSpriteSheetData) {
        delete rect;
    }
    mSpriteSheetData.clear();
}

void DrawAnimatedComponent::LoadSpriteSheet(const std::string &texturePath, const std::string &dataPath) {
    // Load sprite sheet texture
    mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);

    // Load sprite sheet data
    std::ifstream spriteSheetFile(dataPath);
    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    SDL_Rect *rect = nullptr;
    for (const auto &frame : spriteSheetData) {

        int x = frame["x"].get<int>();
        int y = frame["y"].get<int>();
        int w = frame["width"].get<int>();
        int h = frame["height"].get<int>();
        rect = new SDL_Rect({x, y, w, h});

        mSpriteSheetData.emplace_back(rect);
    }
}

void DrawAnimatedComponent::Draw(SDL_Renderer *renderer) {
    int spriteIdx = mAnimations[mAnimName][int(mAnimTimer)];

    Vector2 drawPosition = mOwner->GetPosition() - GetGame()->GetCameraPos();

    int width = mSpriteSheetData[spriteIdx]->w * 3;
    int heigth = mSpriteSheetData[spriteIdx]->h * 3;

    mOwner->GetComponent<AABBColliderComponent>()->SetSize(width, heigth);
    mOwner->GetComponent<DrawPolygonComponent>()->Update(width, heigth);

    SDL_Rect mDrawRect = {int(drawPosition.x), int(drawPosition.y), width, heigth};
    SDL_RendererFlip flip = mOwner->GetRotation() == 0.0f ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, mSpriteSheetData[spriteIdx], &mDrawRect, mOwner->GetRotation(), nullptr, flip);
}

void DrawAnimatedComponent::Update(float deltaTime) {
    if (mIsPaused)
        return;

    mAnimTimer += mAnimFPS * deltaTime;

    while (mAnimTimer >= mAnimations[mAnimName].size()) {
        mAnimTimer = int(mAnimTimer) - mAnimations[mAnimName].size();
    }
}

void DrawAnimatedComponent::SetAnimation(const std::string &name) {
    mAnimName = name;
    Update(0.0f);
}

void DrawAnimatedComponent::AddAnimation(const std::string &name, const std::vector<int> &spriteNums) {
    mAnimations.emplace(name, spriteNums);
}
