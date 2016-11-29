#pragma once
#include "basic.h"

// zone object - keeps track of objects inside it, and can do stuff to them.

class MAGEDLL zone : public objBasic {
public:
	zone(float x, float y, float sizeX, float sizeY);

	virtual void update(sf::Time elapsed);

	virtual void objectEnterZone(std::shared_ptr<objBasic> obj);
	virtual void objectLeaveZone(std::shared_ptr<objBasic> obj);

	bool isInZone(objBasic* obj);
	bool wasInZone(objBasic* obj);

	CLONEABLE(zone);

public:
	sf::Vector2f zoneSize;

	hook<basic*, basic*> onObjectEnterZone;
	hook<basic*, basic*> onObjectLeaveZone;

private:
	std::vector<std::shared_ptr<objBasic>> objectsInZone[2];
};
