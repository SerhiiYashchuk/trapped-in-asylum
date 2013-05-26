#include "level_manager.h"

//--------------------Level manager class--------------------

// Constructor

level_manager::level_manager(CIndieLib &engine) {
	this->engine =							&engine;

	this->ltm =								new lt_manager();
	this->cm =								new conf_manager;

	this->start_point.x =					0;
	this->start_point.y =					0;

	this->level_rect[0].x =					0;
	this->level_rect[0].y =					0;
	this->level_rect[1].x =					0;
	this->level_rect[1].y =					0;

	this->start_room =						0;
	this->finish_room =						0;
	this->key_need =						false;
}

// Destructor

level_manager::~level_manager() {
	delete this->ltm;
	delete this->cm;

	this->delete_rooms();
	for (unsigned short i = 0; i < this->items.size(); i++)
		if (this->items[i])
			delete this->items[i];
}

// Initialization

bool level_manager::init() {
	TiXmlDocument doc(levels_list);

	if (doc.LoadFile()) {
		TiXmlElement *level =				doc.FirstChildElement();

		while (level) {
			this->levels.push_back(string(levels_folder) + "\\" + level->Attribute(level_id_attr));

			level =							level->NextSiblingElement();
		}

	} else return false;

	doc.Clear();
	
	return this->ltm->load_textures("..\\textures");
}

// Define top left point for the level

void level_manager::define_level_rect() {
	if (!this->ltm->get_tiles(0).size()) {
		this->level_rect[0].x =				0;
		this->level_rect[0].y =				0;

		this->level_rect[1].x =				0;
		this->level_rect[1].y =				0;

	} else {
		IND_Entity2d *tmp_entity =			this->ltm->get_tiles(0).get_tile(0);

		this->level_rect[0].x =				(int) tmp_entity->GetPosX() - tmp_entity->GetSurface()->GetWidth() / 2;
		this->level_rect[0].y =				(int) tmp_entity->GetPosY() - tmp_entity->GetSurface()->GetHeight() / 2;
		this->level_rect[1].x =				this->level_rect[0].x + tmp_entity->GetSurface()->GetWidth();
		this->level_rect[1].y =				this->level_rect[0].y + tmp_entity->GetSurface()->GetHeight();
	
		for (unsigned short i = 1; i < this->ltm->get_tiles(0).size(); i++) {
			tmp_entity =					this->ltm->get_tiles(0).get_tile(i);

			if (this->level_rect[0].x > tmp_entity->GetPosX() - tmp_entity->GetSurface()->GetWidth() / 2)
				this->level_rect[0].x =		(int) tmp_entity->GetPosX() - tmp_entity->GetSurface()->GetWidth() / 2;
			if (this->level_rect[0].y > tmp_entity->GetPosY() - tmp_entity->GetSurface()->GetHeight() / 2)
				this->level_rect[0].y =		(int) tmp_entity->GetPosY() - tmp_entity->GetSurface()->GetHeight() / 2;

			if (this->level_rect[1].x < tmp_entity->GetPosX() + tmp_entity->GetSurface()->GetWidth() / 2)
				this->level_rect[1].x =		(int) (tmp_entity->GetPosX() + tmp_entity->GetSurface()->GetWidth() / 2);
			if (this->level_rect[1].y < tmp_entity->GetPosY() + tmp_entity->GetSurface()->GetHeight() / 2)
				this->level_rect[1].y =		(int) (tmp_entity->GetPosY() + tmp_entity->GetSurface()->GetHeight() / 2);
		}
	}
}

// Get set of linked floors

level_floor *level_manager::get_linked(level_floor *sender, level_floor *receiver, vector<level_floor *> &floor_set, vector<marked_floor> &floor_markup) {
	char *mark =						0;
	for (unsigned short i = 0; i < floor_markup.size(); i++) {
		if (floor_markup[i].floor == receiver) {
			mark =						&floor_markup[i].mark;
			break;
		}
	}

	if (!mark) return 0;

	if (*mark == 1)
		for (unsigned short i = 0; i < receiver->near_floor.size(); i++)
			if (receiver->near_floor[i] != sender) {
				level_floor *r_floors = get_linked(receiver, receiver->near_floor[i], floor_set, floor_markup);
				if (r_floors) floor_set.push_back(r_floors);
			}

	(*mark)--;
	return receiver;
}

// Build room using floors, doors and walls

room *level_manager::build_room(vector<marked_floor> &floor_markup, vector<door_conf *> &doors, vector<IND_Entity2d *> &walls, char ID) {
	room *new_room =					new room;
	level_floor *receiver =				0;

	// Adding floors
	for (unsigned short i = 0; i < floor_markup.size(); i++) {
		if (floor_markup[i].mark == 1) {
			receiver =					floor_markup[i].floor;
			break;
		}
	}

	new_room->floor.push_back(this->get_linked(0, receiver, new_room->floor, floor_markup));

	new_room->ID =						ID;

	// Adding doors
	for (unsigned short i = 0; i < new_room->floor.size(); i++) {
		for (unsigned short j = 0; j < doors.size(); j++) {
			if (this->engine->Entity2dManager->IsCollision(new_room->floor[i]->entity, "*", doors[j]->entity, "*")) {
				if (doors[j]->ID >= 0)
					doors[j]->ID =		ID;
				new_room->doors.push_back(doors[j]);
				doors.erase(doors.begin() + j--);
			}
		}
	}

	// Adding walls
	for (unsigned short i = 0; i < new_room->floor.size(); i++) {
		for (unsigned short j = 0; j < walls.size(); j++)
			if (new_room->floor[i]->entity->GetPosY() > walls[j]->GetPosY() &&
				CIndieLib::Instance()->Entity2dManager->IsCollision(new_room->floor[i]->entity, "*", walls[j], "*")) {
					new_room->walls.push_back(walls[j]);
					break;
			}
	}

	this->remove_used_floors(floor_markup);
	return new_room;
}

// Create rooms for the current level

void level_manager::create_rooms() {
	// Creating sets of objects
	IND_Entity2d *obj_entity =			0;
	string tex_name;
	vector<marked_floor> floor_markup;
	vector<IND_Entity2d *> wall_set;

	for (unsigned short i = 0; i < this->ltm->get_tiles(1).size(); i++) {
		obj_entity =					ltm->get_tiles(1).get_tile(i);
		tex_name =						ltm->get_textures().get_name(obj_entity->GetSurface());

		// Floor
		if (tex_name.find(floor_keyword) != string::npos)
			floor_markup.push_back(marked_floor(new level_floor(*obj_entity), 0));

		// Wall
		else if (tex_name.find(wall_keyword) != string::npos)
			wall_set.push_back(obj_entity);
	}

	// Linking floors
	for (unsigned short i = 0; i < floor_markup.size() - 1; i++) {
		for (unsigned short j = i + 1; j < floor_markup.size(); j++) {
			if (engine->Entity2dManager->IsCollision(floor_markup[i].floor->entity, "*", floor_markup[j].floor->entity, "*")) {
				floor_markup[i].floor->near_floor.push_back(floor_markup[j].floor);
				floor_markup[j].floor->near_floor.push_back(floor_markup[i].floor);
			}
		}
	}
	// Marking floors
	for (unsigned short i = 0; i < floor_markup.size(); i++) {
		for (unsigned short k = 0; k < wall_set.size(); k++) {
			if (floor_markup[i].floor->entity->GetPosY() > wall_set[k]->GetPosY() &&
				CIndieLib::Instance()->Entity2dManager->IsCollision(floor_markup[i].floor->entity, "*", wall_set[k], "*")) {
					floor_markup[i].mark =	(floor_markup[i].floor->near_floor.size() == 1) ? (1) : (2);
					break;
			}
			if (k == wall_set.size() - 1)
				floor_markup[i].mark =	1;
		}
	}

	// Getting linked doors
	vector<door_conf *> tmp_door_set;
	char exit_door_id =				-1;

	for (unsigned short i = 0; i < this->cm->get_doors_count(); i++) {
		if (ltm->get_textures().get_name(this->cm->get_door_conf(i)->entity->GetSurface()).find(exit_door_keyword) != string::npos)
			this->cm->get_door_conf(i)->ID =	exit_door_id--;
		tmp_door_set.push_back(this->cm->get_door_conf(i));
	}
	
	// Creating rooms
	char ID =							0;
	while (floor_markup.size()) {
		this->rooms.push_back(this->build_room(floor_markup, tmp_door_set, wall_set, ++ID));
		if (!this->start_room || !this->finish_room) {
			bool found_room =			false;

			for (unsigned short i = 0; i < this->rooms.back()->doors.size(); i++) {
				switch (this->rooms.back()->doors[i]->ID) {
				case -1:
					this->start_room =	this->rooms.back();
					found_room =		true;
					break;

				case -2:
					this->finish_room =	this->rooms.back();
					found_room =		true;
					break;

				default:
					continue;
				}
				if (found_room) break;
			}
		}
	}
}

// Delete rooms for the current level

void level_manager::delete_rooms() {
	if (this->rooms.empty()) return;
	
	for (unsigned short i = 0; i < this->rooms.size() - 1; i++) {
		for (unsigned short j = 0; j < this->rooms[i]->floor.size(); j++) {
			for (unsigned short i1 = i + 1; i1 < this->rooms.size(); i1++) {
				for (unsigned short j1 = 0; j1 < this->rooms[i1]->floor.size(); j1++) {
					if (this->rooms[i]->floor[j] == this->rooms[i1]->floor[j1]) {
						this->rooms[i1]->floor[j1] = 0;
						break;
					}
				}
			}
		}
	}

	for (unsigned short i = 0; i < this->rooms.size(); i++) {
		for (unsigned short j = 0; j < this->rooms[i]->floor.size(); j++) {
			if (this->rooms[i]->floor[j]) {
				delete this->rooms[i]->floor[j];
				this->rooms[i]->floor[j] =	0;
			}
		}
		delete this->rooms[i];
		this->rooms[i] =				0;
	}
}

// Remove the saim rooms from source set

void level_manager::remove_used_floors(vector<marked_floor> &floor_markup) {
	for (unsigned short i = 0; i < floor_markup.size(); i++) {
		if (!floor_markup[i].mark) {
			floor_markup.erase(floor_markup.begin() + i);
			--i;
		}
	}
}

// Load level

bool level_manager::load_level(unsigned short ID) {
	if (ID < this->levels.size()) {
		// Loading
		if (!this->ltm->load((this->levels[ID] + "\\tiles.xml").c_str())) return 0;
		if (!this->cm->create(this->ltm->get_textures(), this->ltm->get_tiles(objects_layer)) ||
			!this->cm->load((this->levels[ID] + "\\config.xml").c_str())) {
			this->ltm->clear();
			return 0;
		}
		this->define_level_rect();

		for (unsigned short i = 0; i < this->ltm->get_tiles(0).size(); i++)
			this->ltm->get_tiles(0).get_tile(i)->DeleteBoundingAreas("*");

		// Clearing
		this->delete_rooms();
		this->rooms.clear();
		this->start_room =						0;
		this->finish_room =						0;

		for (unsigned short i = 0; i < this->items.size(); i++)
			if (this->items[i])
				this->delete_item(i);
		this->items.clear();
		this->key_need =						false;

		// Creating rooms
		this->create_rooms();

		// Creating items
		IND_Entity2d *item_entity =				0;
		string item_name;
		for (unsigned short i = 0; i < this->ltm->get_tiles(items_layer).size(); i++) {
			item_entity =						this->ltm->get_tiles(items_layer).get_tile(i);
			string &item_texture =				this->ltm->get_textures().get_name(this->ltm->get_tiles(items_layer).get_tile(i)->GetSurface());
			item_name =							item_texture.substr(0, item_texture.find('.'));

			this->items.push_back(new item(*item_entity, item_name));

			if (item_name == item_key)
				this->key_need =				true;

			if (item_name == item_candle)
				item_entity->SetTransparency(0);
		}

		return 1;

	} else return 0;
}

// Delete item from the current level

void level_manager::delete_item(unsigned short pos) {
	this->ltm->get_tiles(items_layer).del_tile(&this->items[pos]->get_entity());
	delete this->items[pos];
	this->items[pos] =								0;
}