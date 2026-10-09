#pragma once
#include "IGameEntity.h"
#include "CVect.h"
#include "EntityType.h"

#include <memory>
#include <queue>

class Sprite;
class Map;
class FlowFieldMap;

class  UnitConfig : public IGameEntityConfig
{
public:
    std::shared_ptr<Map> map;
};

class Unit :
    public IGameEntity, public ICollidable
{
protected:
    CVect position;

    float speed;

    bool isMoving;
    bool isSelected;

    CVect target;
    std::queue<CVect> path;
    std::shared_ptr<FlowFieldMap> flowFieldMap;

    std::weak_ptr<Unit> thisWeakPtr;
    std::shared_ptr<Sprite> sprite = nullptr;

    SDL_Rect rect = SDL_Rect{ 0 , 0 , 24 , 32 };
    const UnitConfig* config = nullptr;
    std::weak_ptr<Map> map;

    CVect forces;

    float jumpOnMoveHeight;
    float jumpOnMoveYDelta;
    float jumpOnMovePeriod;
    float jumpOnMoveTimer;

    int frameTimer;

    void SelectNextFlowFieldTile();
    void MoveToNextPosition();

    void UpdateAnimationFrame(int deltaTime);
    void JumpOnMove(int deltaTime);
    float Oscillate(float value, float length);
public:
    static std::shared_ptr<Unit> Create();

    // Hérité via IGameEntity
    virtual std::shared_ptr<IGameEntity> GetSharedPtr() override;
    virtual void Init(const IGameEntityConfig* config) override;
    virtual void Release() override;
    virtual void Update(int deltaTime) override;
    virtual void Render(SDL_Renderer* screenRenderer) override;
    virtual EntityType GetEntityType() const override;

    void SetPosition(int x, int y);

    void MoveTo(int x, int y);
    void AddToPath(int x, int y);
    void SetPath(const std::vector<CVect>& path);
    void SetFlowFieldMap(std::shared_ptr<FlowFieldMap> flowFieldMap);



    void SetSelected(bool selected);
    inline bool IsSelected() const { return isSelected; }
    
    inline bool IsOnTile(int x, int y)
    {
        return x == (int)position.x && y == (int)position.y;
    }

    bool IsInside(const SDL_Rect& rect);

    // Hérité via ICollidable
    virtual int GetCollisionTag() const override;
    virtual float GetRadius() const override;
    virtual CVect GetPosition() const override;
    CVect GetTile() const;

    void OnCollide(std::shared_ptr<ICollidable> other);
};

