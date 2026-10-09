#pragma once
#include "IGameEntity.h"
#include "CVect.h"
#include "EntityType.h"

class Sprite;

class  PointOfInterrestConfig : public IGameEntityConfig
{
public:
    EntityType type;
};

class PointOfInterrest : public IGameEntity
{
protected:
    EntityType entityType;
    
    std::weak_ptr<PointOfInterrest> thisWeakPtr;
    std::shared_ptr<Sprite> sprite = nullptr;

    CVect position;
    bool looted = false;

    SDL_Rect spriteRect;
    EntityType type;

public:
    static std::shared_ptr<PointOfInterrest> Create();

    // Hérité via IGameEntity
    std::shared_ptr<IGameEntity> GetSharedPtr() override;

    void Init(const IGameEntityConfig* config) override;

    void Release() override;

    EntityType GetEntityType() const override;

    void Update(int deltaTime) override;

    void Render(SDL_Renderer* screenRenderer) override;


    virtual CVect GetPosition() const;
    void SetPosition(int x, int y);

};