#include "CollisionManager.h"
#include "ICollidable.h"
#include "CVect.h"

void CollisionManager::AddCollider(std::shared_ptr<ICollidable> collidable)
{
	_allObjects.insert(collidable);
}

void CollisionManager::RemoveCollider(std::shared_ptr<ICollidable> collidable)
{
	_objectToRemove.insert(collidable);
}

void CollisionManager::AddCollisionCallback(std::shared_ptr<ICollidable> collider, int collisionTag, CollisionManagerCallback callback)
{
	auto& colliderCallbackList = _allCollisions[collider];
	colliderCallbackList.AddCollision(collisionTag, callback);
}

void CollisionManager::Tick()
{
	CheckCollisions();

    size_t size = _objectToRemove.size();
	if (size > 0)
	{
        std::unordered_set<std::shared_ptr<ICollidable>>::iterator it;
        for(it = _objectToRemove.begin(); it != _objectToRemove.end(); ++it)
		{
			_allObjects.erase(*it);
			_allCollisions.erase(*it);
		}
		_objectToRemove.clear();
	}
}

void CollisionManager::CheckCollisions()
{
	//a lot of for here... looping all registered callbacks, and for each parsing all known items to check for potential collisions
    std::map<std::shared_ptr<ICollidable>,CollisionConfig>::iterator collidableListIter;
    for (collidableListIter = _allCollisions.begin(); collidableListIter != _allCollisions.end(); ++collidableListIter)
	{
        std::unordered_set<std::shared_ptr<ICollidable>>::iterator allObjectIter;
        for (allObjectIter = _allObjects.begin(); allObjectIter != _allObjects.end(); ++allObjectIter)
		{
			if (collidableListIter->first != *allObjectIter)
			{
				int tag = (*allObjectIter)->GetCollisionTag();
				//check if the object can potentially collide: a bitset + test is way faster than crossing the vector content for no reason
				if ((tag & collidableListIter->second.collisionTags) == 0)
					continue;
                size_t size =  collidableListIter->second.collisions.size();
                for (size_t i = 0; i < size; ++i)
				{
					if ((tag & collidableListIter->second.collisions[i].collidableTag) != 0)
					{
						if (AreColliding(collidableListIter->first, *allObjectIter))
						{
                            collidableListIter->second.collisions[i].callback(*allObjectIter);
						}
					}
				}
			}
		}
	}
}

inline bool CollisionManager::AreColliding(const std::shared_ptr<ICollidable>& collider1,const std::shared_ptr<ICollidable>& collider2)
{
	CVect collider1Pos = collider1->GetPosition();
	CVect collider2Pos = collider2->GetPosition();
	CVect distance = collider1Pos - collider2Pos;
	float rad = collider1->GetRadius() + collider2->GetRadius();
	rad *= rad;
	return distance.getSquaredLength() <= rad;
}


void CollisionManager::Release()
{
	_allObjects.clear();
	_allCollisions.clear();
	_objectToRemove.clear();
}
