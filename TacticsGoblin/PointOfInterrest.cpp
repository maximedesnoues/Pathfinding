#include "PointOfInterrest.h"
#include "SpriteFactory.h"
#include "GameConfig.h"
#include "Sprite.h"

std::shared_ptr<PointOfInterrest> PointOfInterrest::Create()
{
    std::shared_ptr<PointOfInterrest> newShared = std::make_shared<PointOfInterrest>();
    newShared->thisWeakPtr = newShared;
    return newShared;
}

std::shared_ptr<IGameEntity> PointOfInterrest::GetSharedPtr()
{
    return thisWeakPtr.lock();
}

void PointOfInterrest::Init(const IGameEntityConfig* _config)
{
    auto entityConig = dynamic_cast<const PointOfInterrestConfig*>(_config);
    entityType = EntityType::None;
    if (entityConig)
        entityType = entityConig->type;
    sprite = SpriteFactory::GetSprite("Basis.png");
}

void PointOfInterrest::Release()
{
}

EntityType PointOfInterrest::GetEntityType() const
{
    return entityType;
}

void PointOfInterrest::Update(int deltaTime)
{
}

void PointOfInterrest::Render(SDL_Renderer* screenRenderer)
{
    SDL_Rect source;

    switch (entityType)
    {
    case EntityType::Village:
            source = { 22 * 32, 8 * 32, 32, 32 };
            break;
    case EntityType::GoblinCamp:
        source = { 28 * 32, 2 * 32, 32, 32 };
        break;
    default:
        source = { 0, 0, 32, 32 };
    }

    int px = (int)position.x * TILE_SIZE + TILE_SIZE / 2;
    int py = (int)position.y * TILE_SIZE + TILE_SIZE / 2;

    sprite->Render(screenRenderer, px, py, source);
}

CVect PointOfInterrest::GetPosition() const
{
    return position;
}

void PointOfInterrest::SetPosition(int x, int y)
{
    position.x = 1.0f * x;
    position.y = 1.0f * y;
}
