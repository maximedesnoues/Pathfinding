#pragma once

class CVect;

class ICollidable
{
public:
	ICollidable() = default;
	virtual ~ICollidable() = default;
	ICollidable(const ICollidable&) = delete;
	ICollidable(ICollidable&&) = delete;
	ICollidable& operator=(const ICollidable&) = delete;

	virtual int GetCollisionTag() const = 0;
	virtual float GetRadius() const = 0;
	virtual CVect GetPosition() const = 0;
};
