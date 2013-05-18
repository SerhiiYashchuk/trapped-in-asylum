#include "Sound.h"

bool sSound::SetSound(char *path){
	if (this->ScreamBuffer.loadFromFile(path)){
			this->ScreamSound.setBuffer(this->ScreamBuffer);
			this->ScreamSound.setLoop(false);
			this->ScreamSound.setVolume(50);
			return true;
	}
	return false;
}

void sSound::play (){
	this->ScreamSound.play();
}

void sSound::stop (){
	this->ScreamSound.stop();
}

void sSound::SetLoop (bool flag) {
	this->ScreamSound.setLoop(flag);
}

void sSound::SetSpeed (int value) {
	this->ScreamSound.setPitch((float) value);
}

void sSound::SetVolume (int value) {
	this->ScreamSound.setVolume((float) value);
}