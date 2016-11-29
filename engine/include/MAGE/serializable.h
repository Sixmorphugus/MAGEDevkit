#pragma once
#include "StdAfx.h"

// a serializable is an object that can save its state and reload it later.
// a serializable is NOT an object that can save its state and be entirely reconstructed from
// the saved state later.

// this is why you cannot save an object to a file and have it be a prefab.
// you CAN save a prefab to a file, though (as of build 366)

class MAGEDLL serializable {
public:
	virtual std::string serialize();
	virtual bool deserialize(std::string data);

	virtual void saveToFile(std::string file);
	virtual bool loadFromFile(std::string file);
};