#pragma once
#include "basic.h"

// can slide across the ground, bounce off other objects etc.
class MAGEDLL physicsObject : public objBasic
{
public:
	physicsObject(float posX, float posY, textureData sprite);
	
	void init();

	void preUpdate(sf::Time elapsed);

	void update(sf::Time elapsed);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	virtual void impact(collision cDat);
	virtual void impulse(sf::Vector2f position, float force, float radius);

	virtual void registerProperties();

	CLONEABLE(physicsObject);
public:
	sf::Vector2f velocity; // movement per frame
	float density; // how quickly the object slows down per frame (0-1)
	float weight; // how well the object responds to attempts to move it, where 1.f is normal weight

	hook<basic*, basic*> onImpact;
	hook<basic*> onImpulse;
};
