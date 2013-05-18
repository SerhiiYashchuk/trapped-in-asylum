#include "CIndieLib_vc2008.h"
#include <SFML\Audio.hpp>

class sSound{
protected:
	sf::SoundBuffer		ScreamBuffer;
	sf::Sound			ScreamSound;
public:
	bool SetSound		(char *path);
	void play			();
	void stop			();
	void SetLoop		(bool flag);
	void SetSpeed		(int value);
	void SetVolume		(int value);
	int GetStatus		() {return this->ScreamSound.getStatus();}
};
