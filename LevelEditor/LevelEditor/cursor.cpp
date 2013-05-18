#include "cursor.h"

//---------------cursor class---------------

// Constructor

basic_cursor::basic_cursor() {
	this->engine =							0;
}

// Destructor

basic_cursor::~basic_cursor() {
	if (this->engine && this->engine->Entity2dManager->IsOk())
		this->engine->Entity2dManager->Delete(&this->entity);
}

// Initialize cursor

bool basic_cursor::init(CIndieLib &engine) {
		this->engine =						&engine;

		if (!this->engine->SurfaceManager->Add(&this->surface, "..\\res\\cursor.png", IND_ALPHA, IND_32)) return 0;
		if (!this->engine->Entity2dManager->Add(5, &this->entity)) return 0;

		this->entity.SetSurface(&this->surface);
		this->entity.SetRadius(cursor_radius + 1);
		this->entity.SetBoundingCircle("cursor", cursor_radius, cursor_radius, cursor_radius);
		this->entity.SetHotSpot(0.5f, 0.5f);
		this->entity.SetPosition((float) this->engine->Input->GetMouseX(), (float) this->engine->Input->GetMouseY(), 0);

		return 1;
}

//---------------tile cursor class---------------

// Destructor

editing_cursor::~editing_cursor() {
	if (this->a_tile && this->not_saved) {
		delete this->a_tile;
	}
}

// Get next texture in texture set

void tile_cursor::next_texture() {
	if (!(this->textures && this->textures->size())) return;

	if (this->a_tile && this->not_saved) {
		if (this->cur_texture == this->textures->size() - 1)
			this->cur_texture =				0;
		else this->cur_texture++;
		
		this->a_tile->DeleteBoundingAreas("*");
		this->a_tile->SetSurface(this->textures->get_texture(this->cur_texture));
		this->a_tile->SetHotSpot(0.5f, 0.5f);
		this->a_tile->SetPosition(this->entity.GetPosX(), this->entity.GetPosY(), this->a_tile->GetPosZ());
		this->a_tile->SetBoundingRectangle("border", 0, 0, this->a_tile->GetSurface()->GetWidth(), this->a_tile->GetSurface()->GetHeight());
	}
}

// Get previous texture in texture set

void tile_cursor::prev_texture() {
	if (!(this->textures && this->textures->size())) return;

	if (this->a_tile && this->not_saved) {
		if (this->cur_texture == 0)
			this->cur_texture =				this->textures->size() - 1;
		else this->cur_texture--;

		this->a_tile->DeleteBoundingAreas("*");
		this->a_tile->SetSurface(this->textures->get_texture(this->cur_texture));
		this->a_tile->SetHotSpot(0.5f, 0.5f);
		this->a_tile->SetPosition(this->entity.GetPosX(), this->entity.GetPosY(), this->a_tile->GetPosZ());
		this->a_tile->SetBoundingRectangle("border", 0, 0, this->a_tile->GetSurface()->GetWidth(), this->a_tile->GetSurface()->GetHeight());
	}
}

// Create new tile that will be added to the tile set

bool tile_cursor::add() {
	// If no tile_set or no texture_set - cannot create a new tile
	if (!this->tiles || !this->textures) return 0;
	// If no active tile - add a new one
	if (!this->a_tile) {
		this->a_tile =						new IND_Entity2d;
		
		if (this->a_tile) {
			this->not_saved =				true;
			/*
			if (this->tiles->get_name() == background_set)
				this->engine->Entity2dManager->Add(0, this->a_tile);
			else if (this->tiles->get_name() == objects_set)
				this->engine->Entity2dManager->Add(1, this->a_tile);
			*/
			if (!this->engine->Entity2dManager->Add(this->tiles->get_layer(), this->a_tile)) {
				delete this->a_tile;
				this->a_tile =				0;
				this->not_saved =			false;
				return 0;
			}
			
			this->a_tile->SetSurface(this->textures->get_texture(this->cur_texture));
			this->a_tile->SetBoundingRectangle("border", 0, 0, this->a_tile->GetSurface()->GetWidth(), this->a_tile->GetSurface()->GetHeight());
			this->a_tile->SetHotSpot(0.5f, 0.5f);
			this->a_tile->SetPosition(this->entity.GetPosX(), this->entity.GetPosY(), 0);
		} return 0;
	// If not saved active tile - save it
	} else return 0;
	return 1;
}

// Delete current tile

void tile_cursor::del() {
	if (this->a_tile)
		if (this->not_saved) {
			this->engine->Entity2dManager->Delete(this->a_tile);
			delete this->a_tile;
			this->a_tile =					0;
		} else {
			this->tiles->del_tile(this->a_tile);
			this->a_tile =					0;
		}
}

// Get or put a tile by RMB click

void tile_cursor::check_click() {
	if (!this->a_tile) {
		for (unsigned short i = 0; i < this->tiles->size(); i++) {
			if (this->engine->Entity2dManager->IsCollision(&this->entity, "*", this->tiles->get_tile(i), "*")) {
				this->a_tile =				this->tiles->get_tile(i);
				break;
			}
		}
	// With active tile
	} else {
		if (this->not_saved) {
			this->tiles->add_tile(this->a_tile);
			this->not_saved =				false;
		}
		this->a_tile =						0;
	}
}

// Rotate active tile clockwise

void tile_cursor::rotate_right() {
	if (this->a_tile)
		if (this->a_tile->GetAngleZ() < 360 && this->a_tile->GetAngleZ() > -360)
			this->a_tile->SetAngleXYZ(0, 0, this->a_tile->GetAngleZ() + 0.00003f);
		else this->a_tile->SetAngleXYZ(0, 0, 0);
}

// Rotate active tile counterclockwise

void tile_cursor::rotate_left() {
	if (this->a_tile)
		if (this->a_tile->GetAngleZ() < 360 && this->a_tile->GetAngleZ() > -360)
			this->a_tile->SetAngleXYZ(0, 0, this->a_tile->GetAngleZ() - 0.00003f);
		else this->a_tile->SetAngleXYZ(0, 0, 0);
}

// Scale active tile in

void tile_cursor::scale_in() {
	if (this->a_tile)
		this->a_tile->SetScale(this->a_tile->GetScaleX() + 0.05f, this->a_tile->GetScaleY() + 0.05f);
}

// Scale active tile out

void tile_cursor::scale_out() {
	if (this->a_tile)
		if (this->a_tile->GetScaleX() > 0.05f)
			this->a_tile->SetScale(this->a_tile->GetScaleX() - 0.05f, this->a_tile->GetScaleY() - 0.05f);
}

//---------------mob cursor class---------------
/*
// Constructor

mob_cursor::mob_cursor() : cursor() {
	this->mm = 0;
	this->a_point = 0;
	this->a_mob = 0;
	this->edit_trj = false;
}

// Destructor

mob_cursor::~mob_cursor() {

}

// Check collision between cursor and trajectory point

bool mob_cursor::tpoint_collision(IND_Point *tpoint) {
	if (sqrt(pow((float) this->engine->Input->GetMouseX() - tpoint->x, 2) +
		pow((float) this->engine->Input->GetMouseY() - tpoint->y, 2)) < cursor_radius + tpoint_radius)
		return 1;
	else return 0;
}

// Move cursor

void mob_cursor::move(int x, int y) {
	this->entity.SetPosition((float) x, (float) y, 0);
	if (this->a_tile && !this->edit_trj)
		this->a_tile->SetPosition((float) x, (float) y, this->a_tile->GetPosZ());
	if (this->a_point) {
		this->a_point->x = x;
		this->a_point->y = y;
	}
}

// Add new mob

bool mob_cursor::add() {
	if (this->a_mob || !this->textures) return 0;
	else {
		this->a_mob = new mob;
		
		this->a_tile = &this->a_mob->entity;
		this->engine->Entity2dManager->Add(this->a_tile);
		this->a_tile->SetSurface(this->textures->get_texture(this->cur_texture));
		this->a_tile->SetHotSpot(0.5f, 0.5f);
		this->a_tile->SetPosition(this->entity.GetPosX(), this->entity.GetPosY(), 0);
		this->a_tile->SetBoundingRectangle("border", 0, 0, this->a_tile->GetSurface()->GetWidth(), this->a_tile->GetSurface()->GetHeight());
		
		this->a_mob->type = this->textures->get_name(this->cur_texture);

		this->not_saved = true;
		this->edit_trj = false;

		return 1;
	}
	return 0;
}

// Delete current mob

void mob_cursor::del() {
	if (this->a_mob) {
		if (this->a_point)
			for (unsigned short i = 0; i < this->a_mob->trj.size(); i++)
				if (&this->a_mob->trj[i] == this->a_point) {
					this->a_mob->trj.erase(this->a_mob->trj.begin() + i);
					this->a_point = 0;
					return;
				}
		if (this->not_saved) {
			this->engine->Entity2dManager->Delete(this->a_tile);
			delete this->a_mob;
		} else {
			this->mm->del(this->a_mob);
		}
		this->a_mob = 0;
		this->a_tile = 0;
		this->a_point = 0;
	}
}

// Managing mob by RMB click

void mob_cursor::check_click(bool move) {
	// Have active mob
	if (this->a_mob) {
		// Move mob
		if (move) {
			if (this->engine->Entity2dManager->IsCollision(&this->entity, "*", this->a_tile, "*"))
				this->edit_trj = false;
			this->a_point = &this->a_mob->trj[0];
		} else {
		// Edit trajectory
			// Add mob to the manager
			if (this->not_saved) {
				IND_Point new_point;
				new_point.x = this->engine->Input->GetMouseX();
				new_point.y = this->engine->Input->GetMouseY();
				this->a_mob->trj.push_back(new_point);
				this->mm->add(this->a_mob);
				this->not_saved = false;
			}
			// Put down active point
			if (this->a_point) {
				this->a_point = 0;
				if (this->edit_trj) return;
			}
			// Put down active tile
			if (!this->edit_trj) {
				this->edit_trj = true;
				return;
			}
			// Check collision with point
			for (unsigned short i = 0; i < this->a_mob->trj.size(); i++) {
				if (this->tpoint_collision(&this->a_mob->trj[i])) {
					this->a_point = &this->a_mob->trj[i];
					return;
				}
			}
			// Create new trajectory point
			IND_Point new_point;
			new_point.x = this->engine->Input->GetMouseX();
			new_point.y = this->engine->Input->GetMouseY();
			this->a_mob->trj.push_back(new_point);

		}
	} else {
	// Check collision for activating mob
		for (unsigned short i = 0; i < this->mm->size(); i++)
			if (this->engine->Entity2dManager->IsCollision(&this->entity, "*", &this->mm->get(i)->entity, "*")) {
				this->a_mob = this->mm->get(this->a_tile);
				this->a_tile = &this->a_mob->entity;
				//this->a_point = &this->a_mob->trj[0];
				this->edit_trj = !move;
				break;
			}
	}
}

// Set mob manager pointer

void mob_cursor::set_manager(mob_manager *mm) {
	if (mm) this->mm = mm;
}

// Get mob manager pointer

mob_manager *mob_cursor::manager() {
	return this->mm;
}
*/
//---------------config cursor class---------------

// Check click

void config_cursor::check_click() {
	// Look for a door
	for (unsigned short i = 0; i < this->conf_m->get_doors_count(); i++) {
		if (this->engine->Entity2dManager->IsCollision(&this->entity, "*", this->conf_m->get_door_conf(i)->entity, "*")) {
			// Set as active
			if (!this->a_door) {
				this->a_door =						this->conf_m->get_door_conf(i);
				break;
			// Pair with
			} else {
				if (!this->a_door->paired && !this->conf_m->get_door_conf(i)->paired) {
					this->a_door->paired =			this->conf_m->get_door_conf(i);
					this->conf_m->get_door_conf(i)->paired =	this->a_door;
					this->a_door =					0;
				}
				break;
			}
		}

		if (i == this->conf_m->get_doors_count() - 1)
			this->a_door =							0;
	}
}

// Add new configuration

bool config_cursor::add() {
	return 1;
}

// Delete configuration

void config_cursor::del() {
	if (this->a_door) {
		if (this->a_door->paired)
			this->a_door->paired->paired =			0;
		this->a_door->paired =						0;
	}
}