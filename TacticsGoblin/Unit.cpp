#include "Unit.h"
#include "Sprite.h"
#include "Map.h"
#include "FlowFieldMap.h"
#include "GameConfig.h"

std::shared_ptr<IGameEntity> Unit::GetSharedPtr()
{
	return thisWeakPtr.lock();
}

void Unit::Init(const IGameEntityConfig* _config)
{
	config = dynamic_cast<const UnitConfig*>(_config);
	if (config != nullptr)
	{
		map = config->map;
	}
	sprite = SpriteFactory::GetSprite("char_1.png");
	speed = 4.0f;

	jumpOnMoveHeight = 0.2f;
	jumpOnMoveYDelta = 0.0f;
	jumpOnMovePeriod = 0.1f;
	jumpOnMoveTimer = 0.0f;
	frameTimer = rand() % 1000;
}

void Unit::Release()
{
}

void Unit::Update(int deltaTime)
{
	if (isMoving || forces.getSquaredLength() > 0.001f)
	{
		float deltat = (speed * deltaTime) / 1000;
		deltat /= map.lock()->GetTileCost((int)position.v[0], (int)position.v[1]);
		if (isMoving)
		{
			CVect delta = target - position;
			float sqrlen = delta.getSquaredLength();
			delta.normalize();
			delta *= deltat;
			if (delta.getSquaredLength() < sqrlen)
			{
				delta += position;
				if(map.lock()->GetTileCost((int)delta.v[0], (int)delta.v[1]) != 0xFFFFFFFF)
					position = delta;
				else
				{
					MoveToNextPosition();
					
				}
			}
			else
			{
				position = target;
				MoveToNextPosition();
			}
		}
		if (forces.getSquaredLength() > 0.001f)
		{
			forces.normalize();
			forces *= deltat;
			forces += position;
			if (map.lock()->GetTileCost((int)forces.v[0], (int)forces.v[1]) != 0xFFFFFFFF)
				position = forces;
			forces = CVect(0, 0, 0);

		}
	}
	JumpOnMove(deltaTime);
	UpdateAnimationFrame(deltaTime);
}

void Unit::MoveToNextPosition()
{
	if (flowFieldMap != nullptr)
	{
		SelectNextFlowFieldTile();
	}
	else if (path.empty())
	{
		isMoving = false;
	}
	else
	{
		target = path.front();
		path.pop();
	}
}

void Unit::Render(SDL_Renderer* screenRenderer)
{
	float unitPosX = position.v[0];
	float unitPosY = position.v[1] - jumpOnMoveYDelta;
	sprite->Render(screenRenderer, (int)(unitPosX * TILE_SIZE), (int)(unitPosY * TILE_SIZE), rect);
}

EntityType Unit::GetEntityType() const
{
	return EntityType::Unit;
}

void Unit::SetPosition(int x, int y)
{
	//shift to get center of tile by default
	position = CVect(x + 0.5f, y + 0.5f, 0);
}

void Unit::MoveTo(int x, int y)
{
	target = CVect(x + 0.5f, y + 0.5f, 0);
	isMoving = true;
}

void Unit::AddToPath(int x, int y)
{
	if (!isMoving)
		MoveTo(x, y);
	else
		path.push(CVect(x + 0.5f, y + 0.5f, 0));
}

void Unit::SetPath(const std::vector<CVect>& path)
{
	isMoving = false;
	this->path = std::queue<CVect>();
	for (auto& elt : path)
	{
		AddToPath((int)elt.x, (int)elt.y);
	}
}

void Unit::SetFlowFieldMap(std::shared_ptr<FlowFieldMap> flowFieldMap)
{
	this->flowFieldMap = flowFieldMap;
	SelectNextFlowFieldTile();
}

void Unit::SelectNextFlowFieldTile()
{
	if (flowFieldMap != nullptr)
	{
		target = flowFieldMap->GetNextTile(position);
		if(target != position)
			isMoving = true;
		else
		{
			isMoving = false;
			this->flowFieldMap = nullptr;
		}
	}
	else
		isMoving = false;
}


void Unit::SetSelected(bool selected)
{
	isSelected = selected;
	rect.y = selected ? 64 : 0;
}

bool Unit::IsInside(const SDL_Rect& rect)
{
	if(rect.x > position.v[0])
		return false;
	if (rect.w < position.v[0])
		return false;
	if (rect.y > position.v[1])
		return false;
	if (rect.h < position.v[1])
		return false;
	return true;
}

int Unit::GetCollisionTag() const
{
	return 1;
}

float Unit::GetRadius() const
{
	return 0.25f;
}

CVect Unit::GetPosition() const
{
	return position;
}

CVect Unit::GetTile() const
{
	//generates warning due to wanted cast, so ignore it
#pragma warning(suppress : 4244)
	return CVect((int)position.x, (int)position.y, (int)position.z);
#pragma warning(default : 4244)
}

void Unit::OnCollide(std::shared_ptr<ICollidable> other)
{
	if(isMoving)
		return;
	CVect delta = position - other->GetPosition();
	if (delta.getSquaredLength() < 0.0001f)
	{
		//on each other, randomize direction
		float angle  = rand() / (float)RAND_MAX * 2.0f * (float)M_PI;
		delta = CVect(cosf(angle), sinf(angle), 0);
	}
	delta.normalize();
	forces += delta;
}

std::shared_ptr<Unit> Unit::Create()
{
	std::shared_ptr<Unit> newShared = std::make_shared<Unit>();
	newShared->thisWeakPtr = newShared;
	return newShared;
}

void Unit::UpdateAnimationFrame(int deltaTime)
{

	frameTimer += deltaTime;
	frameTimer %= 1000;
	int frameindexes[] = {1, 0 ,1, 2};
	int curFrame = frameindexes[frameTimer / 250];
	rect.x = curFrame * 24;

}

void Unit::JumpOnMove(int deltaTime)
{
	if (isMoving)
	{
		jumpOnMoveTimer += (float)deltaTime / 1000;
		float jumpHeightPercent = Oscillate(jumpOnMoveTimer, jumpOnMovePeriod) / jumpOnMovePeriod;
		jumpOnMoveYDelta = jumpOnMoveHeight * jumpHeightPercent;
	}
	else
	{
		jumpOnMoveYDelta = 0.0f;
	}
}

float Unit::Oscillate(float value, float length)
{
	return (float)fabs(fmod(value + length, (length * 2.0f)) - length);
}