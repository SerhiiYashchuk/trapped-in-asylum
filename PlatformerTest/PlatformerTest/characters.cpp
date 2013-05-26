#include "characters.h"

//--------------------Basic character class--------------------

// Constructor

character::character() {
	this->speed =							0;
	this->position.x =						0;
	this->position.y =						0;
	this->state =							stay;
	this->dRoute =							1;
}

// Set surface

void character::set_surface(IND_Surface &surface) {
	this->entity.SetSurface(&surface);
	this->entity.SetHotSpot(0.5f, 0.5f);

	this->entity.DeleteBoundingAreas("*");
	this->entity.SetBoundingRectangle("feet", surface.GetWidth() / 4, surface.GetHeight() - 5, surface.GetWidth() / 2, 1);
	this->entity.SetBoundingRectangle("lside", 0, 0, 1, surface.GetHeight() - 5);
	this->entity.SetBoundingRectangle("rside", surface.GetWidth() - 2, 0, 1, surface.GetHeight() - 5);
	this->entity.SetBoundingCircle("act_radius", surface.GetWidth() / 2, surface.GetHeight() / 2, surface.GetHeight() / 3);
}

// Move character in specified direction

void character::move(vector2d distance_v) {
	this->position.x +=						distance_v.x;
	this->position.y +=						distance_v.y;

	this->entity.SetPosition(this->position.x, this->position.y, this->entity.GetPosZ());
}

// Set specified animations

void character::set_animation(IND_Animation &animationMove, IND_Animation &animationStop){
	this->aMove = animationMove;
	this->aStop = animationStop;
}

// Set movement animation

void character::set_aMoveLeft_Right_Stay(IND_Animation &animation){
	this->entity.SetAnimation(&animation);
}

// Set position

void character::set_position(vector2d position) {
	this->position =						position;
	this->entity.SetPosition(position.x, position.y, this->entity.GetPosZ());
}

//-----------------------Main hero class-----------------------

// Change fear

void main_hero::change_fear(float fear_ch) {
	if (fear_ch > 0) {
		this->time.Stop();

		if (this->fear + fear_ch < this->max_fear) {
			this->fear +=					fear_ch;
			this->scream_energy +=			fear_ch;
		} else
			this->fear =					this->max_fear;

		if (this->scream_energy + fear_ch > this->max_fear)
			this->scream_energy =			this->max_fear;

		this->time.Start();
	} else {
		if (this->fear + fear_ch > 0)
			this->fear +=					fear_ch;
		else this->fear =					0;
	}
}

// Try to calm down

void main_hero::calmdown() {
	static bool change_max_calmdown =		true;
	
	if (this->fear && this->fear > this->max_calmdown && this->time.GetTicks() / 1000 > this->calmdown_time) {
		if (change_max_calmdown) {
			this->max_calmdown +=			(int) (this->fear - this->max_calmdown) / 2;
			change_max_calmdown =			false;
		}

		if (!this->calmdown_timer)
			this->calmdown_timer =			this->time.GetTicks();
		if ((this->time.GetTicks() - this->calmdown_timer) / 1000 == 1) {
			this->change_fear(-this->calmdown_speed);
			this->calmdown_timer =			0;
		}
	} else change_max_calmdown =			true;
}

// Set min fear that can be reached while calmdown is active

void main_hero::set_max_calmdown(int max_calmdowm) {
	if (max_calmdown < 0)
		this->max_calmdown =				0;
	else if (max_calmdown > this->max_fear)
		this->max_calmdown =				(int) this->max_fear;
	else
		this->max_calmdown =				max_calmdown;
}

// Scream

void main_hero::scream() {
	if (this->scream_energy >= characters::main_hero_scream_energy) {
		this->scream_skill->activate();
		this->scream_energy -=				characters::main_hero_scream_energy;
	}
}

// Reset all stats

void main_hero::reset_stats() {
	this->position.x =						0;
	this->position.y =						0;
	this->entity.SetPosition(0, 0, 0);

	this->fear =							0;
	this->max_fear =						characters::main_hero_max_fear;
	this->calmdown_speed =					characters::main_hero_calmdown_speed;
	this->max_calmdown =					0;
	this->calmdown_time =					characters::main_hero_calmdown_time;
	this->calmdown_timer =					0;
	this->scream_skill->deactivate();
	this->scream_energy =					0;
}

//--------------------------NPC class--------------------------

// Constructor

npc::npc() : character() {
	this->current_phrase =					-1;
	this->current_text =					0;
	this->text_e.SetShow(false);
}

// Destructor

npc::~npc() {
	if (this->current_text) delete this->current_text;
}

// Set font for phrases text

void npc::set_font(IND_Font &font, char ch_spc, char l_spc, IND_Align align) {
	this->text_e.SetFont(&font);
	this->text_e.SetCharSpacing(ch_spc);
	this->text_e.SetLineSpacing(l_spc);
	this->text_e.SetAlign(align);
}

// Show one of the phrases

void npc::start_talking() {
	if (this->current_phrase != -1) {
		if (this->current_text) delete this->current_text;
		this->current_text =				new char[this->speech[this->current_phrase].size() + 1];
		strcpy(this->current_text, this->speech[this->current_phrase].c_str());
		
		this->text_e.SetText(this->current_text);
		this->text_e.SetPosition(this->entity.GetPosX() + this->entity.GetSurface()->GetWidth() / 2 - 20,
			this->entity.GetPosY() - this->entity.GetSurface()->GetHeight() / 2 - 20, 0);
		this->text_e.SetShow(true);
	}
}

// Hide phrases text and set the next one as a current

void npc::stop_talking() {
	this->text_e.SetShow(false);
	if (this->current_phrase != this->speech.size() - 1 && this->current_phrase != -1)
		this->current_phrase++;
	else this->current_phrase =				-1;
	if (this->current_text) {
		delete this->current_text;
		this->current_text =				0;
	}
}

//-------------------------Mob class---------------------------

// Scare the hero

void mob::scare(main_hero &hero) {
	if (!this->scare_timer)
		this->scare_timer =					this->time.GetTicks();
	if (this->time.GetTicks() - this->scare_timer > 750) {
		hero.change_fear(this->_scare);
		this->scare_timer =					0;
	}
}

void mob::set_animation() {
	this->entity.SetAnimation(&this->aMove);
}

//---------------------Characters manager----------------------

// Constructor

chmanager::chmanager() {
	this->info.push_back(chinfo(&this->hero));
	CIndieLib::Instance()->Entity2dManager->Add(4, &this->hero.get_entity());
}

// Destructor

chmanager::~chmanager() {
	if (CIndieLib::Instance()->Entity2dManager->IsOk())
		CIndieLib::Instance()->Entity2dManager->Delete(&this->hero.get_entity());
	this->clear();
}

// Delete character info

void chmanager::del_info(character *_char) {
	for (unsigned short i = 0; i < this->info.size(); i++)
		if (this->info[i]._char == _char) {
			this->info.erase(this->info.begin() + i);
			break;
		}
}

// Add NPC to the NPC set

void chmanager::add_npc() {
	npc *new_npc =							new npc;
	
	CIndieLib::Instance()->Entity2dManager->Add(3, &new_npc->get_entity());
	new_npc->set_surface(*this->skin[0]);
	this->npc_set.push_back(new_npc);
	this->info.push_back(chinfo(new_npc));
}

// Delete NPC from the NPC set

void chmanager::del_npc(unsigned short pos) {
	this->del_info(this->npc_set[pos]);
	if (CIndieLib::Instance()->Entity2dManager->IsOk())
		CIndieLib::Instance()->Entity2dManager->Delete(&this->npc_set[pos]->get_entity());
	delete this->npc_set[pos];
	this->npc_set.erase(this->npc_set.begin() + pos);
}

// Add mob to the mob set

void chmanager::add_mob() {
	mob *new_mob =							new mob;

	CIndieLib::Instance()->Entity2dManager->Add(3, &new_mob->get_entity());
	new_mob->set_animation();
	this->mob_set.push_back(new_mob);
	this->info.push_back(chinfo(new_mob));
}

// Delete mob from the mob set

void chmanager::del_mob(unsigned short pos) {
	this->del_info(this->mob_set[pos]);
	if (CIndieLib::Instance()->Entity2dManager->IsOk())
		CIndieLib::Instance()->Entity2dManager->Delete(&this->mob_set[pos]->get_entity());
	delete this->mob_set[pos];
	this->mob_set.erase(this->mob_set.begin() + pos);
}

// Get character info

chinfo &chmanager::get_info(character *_char) {
	if (_char)
		for (unsigned short i = 0; i < this->info.size(); i++)
			if (this->info[i]._char == _char) return this->info[i];

	return chinfo(0);
}

// Clear all

void chmanager::clear() {
	for (unsigned short i = 0; i < this->npc_set.size(); i++) {
		if (CIndieLib::Instance()->Entity2dManager->IsOk())
			CIndieLib::Instance()->Entity2dManager->Delete(&this->npc_set[i]->get_entity());
		delete this->npc_set[i];
	}
	for (unsigned short i = 0; i < this->mob_set.size(); i++) {
		if (CIndieLib::Instance()->Entity2dManager->IsOk())
			CIndieLib::Instance()->Entity2dManager->Delete(&this->mob_set[i]->get_entity());
		delete this->mob_set[i];
	}

	this->mob_set.clear();
	this->npc_set.clear();
	this->info.clear();
	this->info.push_back(&this->hero);
}