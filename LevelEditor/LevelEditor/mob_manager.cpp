#include "mob_manager.h"

#define TPOINT_RADIUS 8

//---------------mob manager class---------------

// Constructor

mob_manager::mob_manager() {
	this->engine = 0;
}

// Destructor

mob_manager::~mob_manager() {
	for (unsigned short i = 0; i < this->mobs.size(); i++) {
		if (this->engine->Entity2dManager->IsOk())
			this->engine->Entity2dManager->Delete(&this->mobs[i]->entity);
		delete this->mobs[i];
	}
}

// Initialize

bool mob_manager::init(CIndieLib *engine) {
	if (!engine) return 0;
	this->engine = engine;
	this->mobs.clear();

	return 1;
}

// Create and add new mob to the manager

bool mob_manager::add(IND_Surface *surface, const char *type, int x, int y) {
	if (!surface || !type) return 0;
	mob *new_mob = new mob;
	if (new_mob) {
		if (this->engine->Entity2dManager->Add(&new_mob->entity)) {
			new_mob->entity.SetSurface(surface);
			new_mob->entity.SetHotSpot(0.5f, 0.5f);
			new_mob->entity.SetBoundingRectangle("border", 0, 0, surface->GetWidth(), surface->GetHeight());
			new_mob->entity.SetPosition((float) x, (float) y, 0);
			new_mob->type = type;
			IND_Point new_point;
			new_point.x = x;
			new_point.y = y;
			new_mob->trj.push_back(new_point);

			return 1;
		} else delete new_mob;
	}

	return 0;
}

// Add existing mob to the manager

bool mob_manager::add(mob *new_mob) {
	if (new_mob) {
		this->mobs.push_back(new_mob);

		return 1;
	} else return 0;
}

// Delete mob by position

void mob_manager::del(unsigned short pos) {
	if (pos < this->mobs.size()) {
		this->engine->Entity2dManager->Delete(&this->mobs[pos]->entity);
		delete this->mobs[pos];
		this->mobs.erase(this->mobs.begin() + pos);
	}
}

// Delete mon by pointer

void mob_manager::del(mob *amob) {
	for (unsigned short i = 0; i < this->mobs.size(); i++)
		if (this->mobs[i] == amob) {
			this->engine->Entity2dManager->Delete(&this->mobs[i]->entity);
			delete this->mobs[i];
			this->mobs.erase(this->mobs.begin() + i);
		}
}

// Get pointer to mob by it's position

mob *mob_manager::get(unsigned short pos) {
	if (pos < this->mobs.size())
		return this->mobs[pos];
	else return 0;
}

// Get pointer to mob by it's entity

mob *mob_manager::get(IND_Entity2d *entity) {
	for (unsigned short i = 0; i < this->mobs.size(); i++)
		if (&this->mobs[i]->entity == entity)
			return this->mobs[i];
	
	return 0;
}

// Get mob's count

unsigned short mob_manager::size() {
	return this->mobs.size();
}

// Render trajectories of all mobs

void mob_manager::render() {
	for (unsigned short i = 0; i < this->mobs.size(); i++)
		if (this->mobs[i]->trj.size() > 1)
			for (unsigned short j = 0; j < this->mobs[i]->trj.size() - 1; j++) {
				this->engine->Render->BlitRegularPoly(this->mobs[i]->trj[j + 1].x, this->mobs[i]->trj[j + 1].y, TPOINT_RADIUS, 5, -90, 0, 0, 255, 255);
				this->engine->Render->BlitLine(this->mobs[i]->trj[j].x, this->mobs[i]->trj[j].y, this->mobs[i]->trj[j + 1].x, this->mobs[i]->trj[j + 1].y, 0, 0, 255, 255);
			}
}