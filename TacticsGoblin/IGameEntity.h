#pragma once
#include <SDL_render.h>
#include <memory>

#include "ICollidable.h"

enum class EntityType;

class IGameEntityConfig
{
public:
	IGameEntityConfig() {};
	virtual ~IGameEntityConfig() = 0;
};

class IGameEntity
{

public:
	IGameEntity() = default;
	virtual ~IGameEntity() = default;
	IGameEntity(const IGameEntity&) = delete;
	IGameEntity(IGameEntity&&) = delete;
	IGameEntity& operator=(const IGameEntity&) = delete;

	virtual std::shared_ptr<IGameEntity> GetSharedPtr() = 0;

	virtual void Init(const IGameEntityConfig* config) = 0;
	virtual void Release() = 0;
	virtual EntityType GetEntityType() const = 0;

	virtual void Update(int deltaTime) = 0;
	virtual void Render(SDL_Renderer* screenRenderer) = 0;


};

