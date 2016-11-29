#include "zone.h"

#include "Game.h"

zone::zone(float x, float y, float sizeX, float sizeY):
	objBasic(x, y, textureData(nullptr))
{
	zoneSize.x = sizeX;
	zoneSize.y = sizeY;
}

void zone::update(sf::Time elapsed)
{
	// keep collision boxes up to date
	if (collisionBoxes.size() != 1) {
		collisionBoxes.clear();
		collisionBoxes.push_back(sf::FloatRect());
	}

	collisionBoxes[0].left = getPosition().x;
	collisionBoxes[0].top = getPosition().y;
	collisionBoxes[0].width = zoneSize.x;
	collisionBoxes[0].height = zoneSize.y;

	// do search
	objectsInZone[1] = objectsInZone[0];
	objectsInZone[0] = theGame()->getCollidingObjects(this);

	for (unsigned int i = 0; i < objectsInZone[0].size(); i++) {
		if (!wasInZone(objectsInZone[0][i].get())) {
			objectEnterZone(objectsInZone[0][i]);
		}
	}

	for (unsigned int i = 0; i < objectsInZone[1].size(); i++) {
		if (wasInZone(objectsInZone[1][i].get())) {
			objectLeaveZone(objectsInZone[1][i]);
		}
	}
}

// by default these do nothing, derive the object
void zone::objectEnterZone(std::shared_ptr<objBasic> obj)
{
	onObjectEnterZone.notify(this, obj.get());
}

void zone::objectLeaveZone(std::shared_ptr<objBasic> obj)
{
	onObjectLeaveZone.notify(this, obj.get());
}

bool zone::isInZone(objBasic * obj)
{
	for (unsigned int i = 0; i < objectsInZone[0].size(); i++) {
		if (objectsInZone[0][i].get() == obj) {
			return true;
		}
	}

	return false;
}

bool zone::wasInZone(objBasic * obj)
{
	for (unsigned int i = 0; i < objectsInZone[1].size(); i++) {
		if (objectsInZone[1][i].get() == obj) {
			return true;
		}
	}

	return false;
}
