//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawSpriteComponent::DrawSpriteComponent(class Actor *owner, const std::string &texturePath, const int width, const int height, const int drawOrder)
    : DrawComponent(owner, drawOrder), mWidth(width), mHeight(height) {
    mSpriteSheetSurface = mOwner->GetGame()->LoadTexture(texturePath);
}

void DrawSpriteComponent::Draw(SDL_Renderer *renderer) {
    Vector2 drawPosition = mOwner->GetPosition() - GetGame()->GetCameraPos();
    SDL_Rect dstRect = {int(drawPosition.x), int(drawPosition.y), mWidth, mHeight};
    SDL_RendererFlip flip = mOwner->GetRotation() == 0.0f ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, nullptr, &dstRect, 0.0, nullptr, flip);
}