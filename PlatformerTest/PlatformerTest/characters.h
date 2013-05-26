#ifndef CHARACTERS_H
#define CHARACTERS_H

#include "CIndieLib_vc2008.h"
#include "sup_structs_and_classes.h"
#include "Sound.h"

enum chstate {stay, shocked, go, on_stairs, atack};
enum Direction{right = 1 , left = -1};

//--------------------------Constants--------------------------

const unsigned char mobs_layer =								3;
const unsigned char hero_layer =								4;

namespace characters {
	const unsigned short main_hero_speed =						200;
	const float main_hero_max_fear =							200.0f;
	const unsigned char main_hero_calmdown_time =				7;
	const float main_hero_calmdown_speed =						2.5f;
	const unsigned char main_hero_skill_duration =				2;
	const float main_hero_scream_energy =		 				60.0f;
	const unsigned char main_hero_scream_limit =				3;

	const unsigned short mob_speed =							170;
	const float mob_scare_speed =								5.0f;
}

//--------------------Basic character class--------------------

class character {
protected:
	IND_Entity2d entity;
	IND_Animation aMove;
	IND_Animation aStop;
	chstate state;
	vector2d position;

private:
	unsigned short speed;

public:
	character();
	int dRoute;

	void set_animation (IND_Animation &animationMove, IND_Animation &animationStop);
	IND_Animation &get_aMoveLeft_Right() {return this->aMove;}
	IND_Animation &get_aStay() {return this->aStop;}

	void set_aMoveLeft_Right_Stay(IND_Animation &animation);

	void set_surface(IND_Surface &surface);
	IND_Entity2d &get_entity() {return this->entity;};

	virtual void move(vector2d distance_v);
	void set_position(vector2d position);
	vector2d get_position() {return this->position;};

	void set_speed(unsigned short speed) {this->speed = speed;};
	unsigned short get_speed() {return this->speed;};

	void set_state(chstate state) {this->state = state;};
	chstate get_state() {return this->state;};
};

//-----------------------Main hero class-----------------------

class main_hero : virtual public character {
	float fear;
	float max_fear;

	IND_Timer time;
	unsigned char calmdown_time;
	float calmdown_speed;
	int max_calmdown;
	unsigned int calmdown_timer;

	skill *scream_skill;
	float scream_energy;

	sSound *scream_sound;
	sSound *footstep_sound;

	IND_Animation ScreamAnimation;

public:
	main_hero() : character(),
		fear(0),
		max_fear(characters::main_hero_max_fear),
		calmdown_time(characters::main_hero_calmdown_time),
		calmdown_speed(characters::main_hero_calmdown_speed),
		max_calmdown(0),
		calmdown_timer(0),
		scream_energy(0) {
			this->scream_skill = new skill(characters::main_hero_skill_duration);
			this->set_speed(characters::main_hero_speed);
	};

	~main_hero() {delete this->scream_skill;};

	float get_fear() {return this->fear;};
	void change_fear(float fear_ch);

	void set_max_fear(float max_fear) {this->max_fear = max_fear;};
	float get_max_fear() {return this->max_fear;};

	void set_calmdown_time(unsigned char seconds) {this->calmdown_time = seconds;};
	unsigned char get_calmdown_time() {return this->calmdown_time;};

	void set_calmdown_speed(float fear_per_second) {this->calmdown_speed = fear_per_second;};
	float get_calmdown_speed() {return this->calmdown_speed;};

	void set_max_calmdown(int max_calmdown);
	int get_max_calmdown() {return this->max_calmdown;};

	void calmdown();
	void scream();
	bool is_screaming() {this->scream_skill->update(); return this->scream_skill->is_active();};
	float get_scream_energy() {return this->scream_energy;};

	void reset_stats();

	void SetScreamSound(sSound *scream) {this->scream_sound = scream; this->scream_sound->SetVolume(100);}
	sSound *GetScreamSound(){return this->scream_sound;}

	void SetFootstepSound(sSound *footstep) {this->footstep_sound = footstep;}
	sSound *GetFootstepSound() {return this->footstep_sound;}
	
	void SetScreamAnimation (IND_Animation &animation) {this->ScreamAnimation = animation;}
	IND_Animation &GetScreamAnimation() {return this->ScreamAnimation;}
};

//--------------------------NPC class--------------------------

class npc : virtual public character {
	IND_Entity2d text_e;
	vector<string> speech;
	short current_phrase;
	char *current_text;

public:
	npc();
	~npc();

	void set_font(IND_Font &font, char ch_spc = -8, char l_spc = 18, IND_Align align = IND_CENTER);
	IND_Entity2d &get_text_entity() {return this->text_e;};

	void start_talking();
	void stop_talking();
	void add_phrase(const char *phrase) {if (!this->speech.size()) this->current_phrase = 0; this->speech.push_back(phrase);};
};

//-------------------------Mob class---------------------------

class mob : virtual public character {
	IND_Timer time;
	float _scare;
	unsigned int scare_timer;

public:
	mob() : character(),
		_scare(characters::mob_scare_speed),
		scare_timer(0) {
			this->set_speed(characters::mob_speed);
			this->time.Start();
			CIndieLib::Instance()->AnimationManager->AddToSurface(&this->aMove, "..\\res\\animations\\mob\\move\\Move.xml", IND_ALPHA, IND_32);
	};
	~mob() {
		if (CIndieLib::Instance()->AnimationManager->IsOk())
			CIndieLib::Instance()->AnimationManager->Delete(&this->aMove);
	}
	void set_scare(float fear_per_second) {this->_scare = fear_per_second;};
	float get_scare() {return this->_scare;};
	void set_animation();
	void scare(main_hero &hero);
};

//-----------------Characters info structure-------------------

struct chinfo {
	character *_char;
	char current_room;
	bool lcollision;
	bool rcollision;
	level_floor *current_floor;
	level_floor *possible_floor;
	vector2d direction;

	chinfo() : _char(0), current_room(0), lcollision(0), rcollision(0), current_floor(0), possible_floor(0) {};

	chinfo(character *Char) : _char(Char), current_room(0), lcollision(0), rcollision(0), current_floor(0), possible_floor(0) {};
};

//---------------------Characters manager----------------------

class chmanager {
	main_hero hero;
	vector<npc *> npc_set;
	vector<mob *> mob_set;

	vector<chinfo> info;
	vector<IND_Surface *> skin;

	void del_info(character *_char);

public:
	IND_Timer time;

	chmanager();
	~chmanager();

	main_hero &get_hero() {return this->hero;};
	npc &get_npc(unsigned short pos) {return *this->npc_set[pos];};
	unsigned short get_npc_count() {return this->npc_set.size();};
	mob &get_mob(unsigned short pos) {return *this->mob_set[pos];};
	unsigned short get_mob_count() {return this->mob_set.size();};

	void add_npc();
	void del_npc(unsigned short pos);
	void add_mob();
	void del_mob(unsigned short pos);

	chinfo &get_info(character *_char);
	chinfo &get_info(unsigned short pos) {return this->info[pos];};
	unsigned short get_info_count() {return this->info.size();};

	void add_skin(IND_Surface &skin) {this->skin.push_back(&skin);};

	void clear();
};

#endif