#include "Sprite.h"
#include <SDL.h>

#define GLOBAL_ZOOM 1

Sprite::Sprite(SDL_Texture* spriteImage): _spriteImage(spriteImage)
{
    SDL_QueryTexture(spriteImage, NULL, NULL, &_spriteRect.w, &_spriteRect.h);
}

Sprite::~Sprite()
{
    SDL_DestroyTexture(_spriteImage);
}

void Sprite::Render(SDL_Renderer* screenRenderer, int posX, int posY)
{
    SDL_Rect rect = { (posX - (_spriteRect.w >> 1)) * GLOBAL_ZOOM, (posY - (_spriteRect.h >> 1)) * GLOBAL_ZOOM, _spriteRect.w * GLOBAL_ZOOM, _spriteRect.h * GLOBAL_ZOOM };
    SDL_RenderCopy(screenRenderer, _spriteImage, NULL, &rect);
}

void Sprite::Render(SDL_Renderer* screenRenderer, int posX, int posY, double orientation)
{
    SDL_Rect rect = { (posX - (_spriteRect.w >> 1)) * GLOBAL_ZOOM, (posY - (_spriteRect.h >> 1)) * GLOBAL_ZOOM, _spriteRect.w * GLOBAL_ZOOM, _spriteRect.h * GLOBAL_ZOOM };
    SDL_RenderCopyEx(screenRenderer, _spriteImage, NULL, &rect, orientation, NULL, SDL_RendererFlip::SDL_FLIP_NONE);
}

void Sprite::RenderSlice(SDL_Renderer* screenRenderer, int posX, int posY, int slice, int sliceCount)
{
    int sliceSize = _spriteRect.w / sliceCount;
    SDL_Rect srcrect = { sliceSize * slice, 0, sliceSize, _spriteRect.h };
    SDL_Rect dstrect = { (posX - (sliceSize >> 1)) * GLOBAL_ZOOM, (posY - (_spriteRect.h >> 1)) * GLOBAL_ZOOM, sliceSize * GLOBAL_ZOOM, _spriteRect.h * GLOBAL_ZOOM };

    SDL_RenderCopy(screenRenderer, _spriteImage, &srcrect, &dstrect);
}

void Sprite::Render(SDL_Renderer* screenRenderer, int posX, int posY, const SDL_Rect& srcrect)
{
    SDL_Rect dstrect = { (posX - (srcrect.w >> 1)) * GLOBAL_ZOOM, (posY - (srcrect.h >> 1)) * GLOBAL_ZOOM, srcrect.w * GLOBAL_ZOOM, srcrect.h * GLOBAL_ZOOM };

    SDL_RenderCopy(screenRenderer, _spriteImage, &srcrect, &dstrect);
}

int Sprite::GetWidth() const
{
    return _spriteRect.w;
}

int Sprite::GetHeight() const
{
    return _spriteRect.h;
}
