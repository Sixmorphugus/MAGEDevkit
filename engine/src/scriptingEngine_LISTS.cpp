#include "scriptingEngine.h"

#include "Game.h"

// chaiscript lists
void scriptingEngine::bindLists()
{
	// primitive lists
	chai->add(vector_type<std::vector<unsigned int> >("UIntVector"));
	chai->add(vector_type<std::vector<int> >("IntVector"));
	chai->add(vector_type<std::vector<float> >("FloatVector"));
	chai->add(vector_type<std::vector<std::string> >("StringVector"));
	//chai->add(vector_type<std::vector<bool> >("BoolVector"));

	// sfml type lists
	chai->add(vector_type<std::vector<sf::Texture*> >("textureVector")); // forgot why we need this?
	chai->add(vector_type<std::vector<sf::IntRect> >("frameVector"));
	chai->add(vector_type<std::vector<sf::FloatRect> >("rectangleVector"));

	// mage type lists
	chai->add(vector_type<std::vector<basic*> >("objectVector"));
	chai->add(vector_type<std::vector<objBasic*> >("worldObjectVector"));
	chai->add(vector_type<std::vector<uiBasic*> >("uiObjectVector"));
	chai->add(vector_type<std::vector<objBasic::spriteData> >("objectSpriteVector"));
}
