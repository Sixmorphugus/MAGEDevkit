#include "physicsObject.h"
#include "Game.h"
#include "helpers.h"

#include "sfResources.h"

#include "shaders.h"

physicsObject::physicsObject(float posX, float posY, textureData sprite)
	: objBasic(posX, posY, sprite)
{
	init();
}

void physicsObject::init()
{
	density = 0.1f;
	weight = 1.f;
}

void physicsObject::preUpdate(sf::Time elapsed)
{
	objBasic::preUpdate(elapsed);
}

void physicsObject::update(sf::Time elapsed)
{
	objBasic::update(elapsed);

	// velocity
	if (density > 1.f)
		density = 1.f; // NOT A GOOD IDEA TO HAVE DRAG OVER 1.f

	if (density < 0.f)
		density = 0.f; // NOT A GOOD IDEA TO HAVE DRAG UNDER 0.f

	float drag = 1.f - density;

	if ((velocity.x != 0 || velocity.y != 0)) {
		collision cDat = resolveMovement(velocity.x, velocity.y);

		if (cDat.hitX || cDat.hitY) {
			impact(cDat);
		}

		velocity.x *= drag;
		velocity.y *= drag;
	}
}

void physicsObject::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	applyShader(states, getTexSizeF());

	// we can use the delta number and the velocity to "predict" where this object will be next.
	sf::Sprite fakeSprite(sfSprite);

	fakeSprite.move(velocity.x * theGame()->getDelta(), velocity.y * theGame()->getDelta());

	if (sfSprite.getTexture())
		target.draw(fakeSprite, states);
}

void physicsObject::impact(collision cDat)
{
	if (cDat.main == this) {
		// we hit something
		for (unsigned int i = 0; i < cDat.involved.size(); i++) {
			objBasic* ob = cDat.involved[i].get();
			physicsObject* po = dynamic_cast<physicsObject*>(ob);

			if (po) {
				po->impact(cDat); // notify any physics objects we hit
			}
			else {
				// solid object, stop dead
				velocity.x = 0.f;
				velocity.y = 0.f;
			}
		}
	}
	else {
		physicsObject* po = dynamic_cast<physicsObject*>(cDat.main);

		float devisor = 2.f * weight;

		velocity.x += po->velocity.x / devisor;
		velocity.y += po->velocity.y / devisor;

		po->velocity.x -= po->velocity.x / devisor;
		po->velocity.y -= po->velocity.y / devisor;

		onImpact.notify(this, po);
		po->onImpact.notify(po, this);
	}

	// derive to do more
}

void physicsObject::impulse(sf::Vector2f position, float force, float radius)
{
	// calculate distance and directional angle
	float dir = directionToVector(getCenter(), position);
	float dis = distanceBetweenVectors(getCenter(), position);

	float d = dis / radius;
	float f = force / d;

	// move away
	velocity += generateVector(dir, f);

	onImpulse.notify(this);
}

void physicsObject::registerProperties()
{
	objBasic::registerProperties();

	registerProperty("xVel", prop(velocity.x));
	registerProperty("yVel", prop(velocity.y));

	registerProperty("drag", prop(density));

	registerProperty("weight", prop(weight));
}

// SE
DeclareScriptingCustom(user_type<physicsObject>(), "physicsObject");
DeclareScriptingCustom(base_class<objBasic, physicsObject>());
DeclareScriptingBaseClass(basic, physicsObject);
DeclareScriptingBaseClass(sf::Transformable, physicsObject);
DeclareScriptingCopyOperator(physicsObject);
DeclareScriptingCustom(constructor<physicsObject(float, float, basic::textureData)>(), "physicsObject");
DeclareScriptingCustom(fun(&physicsObject::density), "density");
DeclareScriptingCustom(fun(&physicsObject::impact), "impact");
DeclareScriptingCustom(fun(&physicsObject::impulse), "impulse");
DeclareScriptingCustom(fun(&physicsObject::velocity), "velocity");
DeclareScriptingCustom(fun(&physicsObject::weight), "weight");