#ifndef LEVELS_H
#define LEVELS_H

#include "tiles_manager.h"
#include "objects_configuration.h"
#include "sup_structs_and_classes.h"

const char levels_folder[] =				"..\\levels";
const char levels_list[] =					"..\\levels\\levels.xml";
const char level_id_attr[] =				"id";

const char floor_keyword[] =				"floor";
const char wall_keyword[] =					"wall";
const char door_keyword[] =					"door";
const char exit_door_keyword[] =			"door_0";

const char item_key[] =						"key";
const char item_valeriana[] =				"valeriana";
const char item_candle[] =					"candle";
const float valeriana_fear_reduce =			40.0f;

//-------------------------Room class------------------------

struct room {
	vector<door_conf *> doors;
	vector<level_floor *> floor;
	vector<IND_Entity2d *> walls;
	char ID;

	room() : ID(0) {};
};

//--------------------Level manager class--------------------

class level_manager {
	struct marked_floor {
		level_floor *floor;
		char mark;

		marked_floor() : floor(0), mark(0) {};
		marked_floor(level_floor *l_floor, char f_mark = 0) : floor(l_floor), mark(f_mark) {};
	};

	CIndieLib *engine;
	
	vector<string> levels;

	lt_manager *ltm;
	conf_manager *cm;

	IND_Point level_rect[2];																		// Rectangle which defines level's edges
	IND_Point start_point;																			// Starting point. Camera will be setted there

	vector<room *> rooms;
	room *start_room;
	room *finish_room;
	vector<item *> items;
	bool key_need;

	void define_level_rect();																		// Defines rectangle for the level
	
	level_floor *get_linked(level_floor *sender, level_floor *receiver, vector<level_floor *> &floor_set, vector<marked_floor> &floor_markup);
	room *build_room(vector<marked_floor> &floor_markup, vector<door_conf *> &doors, vector<IND_Entity2d *> &walls, char ID);
	void create_rooms();
	void delete_rooms();
	void remove_used_floors(vector<marked_floor> &floor_markup);

public:
	level_manager(CIndieLib &engine);
	~level_manager();

	bool init();

	bool load_level(unsigned short ID);
	unsigned short get_levels_count() {return this->levels.size();};

	IND_Point get_start_point() {return this->start_point;};										// Returns starting point for the level. These are the coordinates for camera or something else
	IND_Point get_tl_point() {return this->level_rect[0];};											// Returns top left point of the level. This is the edge of the level
	IND_Point get_br_point() {return this->level_rect[1];};											// Returns bottom right point of the level. This is the edge of the level

	unsigned short get_rooms_count() {return this->rooms.size();};
	room &get_room(unsigned short pos) {return *this->rooms[pos];};
	room &get_start_room() {return *this->start_room;};
	room &get_finish_room() {return *this->finish_room;};

	void delete_item(unsigned short pos);
	item *get_item(unsigned short pos) {return this->items[pos];};
	unsigned short get_items_count() {return this->items.size();};

	void set_key_need(bool is_needed) {this->key_need = is_needed;};
	bool is_key_needed() {return this->key_need;};
};

#endif