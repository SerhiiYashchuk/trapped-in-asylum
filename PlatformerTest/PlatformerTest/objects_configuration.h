#ifndef OBJECTS_CONFIGURATION_H
#define OBJECTS_CONFIGURATION_H

#include "CIndieLib_vc2008.h"
#include "tiles.h"
#include "tinyxml\tinyxml.h"

//---------------constants---------------

namespace configuration {

const char doors_section[] =		"DOORS";
const char doors_element[] =		"door";

const char attr_id[] =				"id";
const char attr_pos_x[] =			"x";
const char attr_pos_y[] =			"y";
const char attr_paired[] =			"paired-id";

}

//---------------door config structure---------------

struct door_conf {
	IND_Entity2d *entity;
	door_conf *paired;
	char ID;

	door_conf() {
		this->entity =				0;
		this->paired =				0;
		this->ID =					0;
	};

	door_conf(IND_Entity2d &entity, char ID = 0) {
		this->entity =				&entity;
		this->paired =				0;
		this->ID =					ID;
	};

	door_conf(IND_Entity2d &entity, door_conf &paired_door, char ID = 0) {
		this->entity =				&entity;
		this->paired =				&paired_door;
		this->ID =					ID;
	};
};

//---------------config manager class---------------

class conf_manager {
	struct tmp_conf {
		int pos_x;
		int pos_y;
		int ID;
		int paired;
		door_conf *dc;

		tmp_conf() {
			this->pos_x =			0;
			this->pos_y =			0;
			this->ID =				0;
			this->paired =			0;
			this->dc =				0;
		};

		tmp_conf(int x, int y, int ID, int paired = 0) {
			this->pos_x =			x;
			this->pos_y =			y;
			this->ID =				ID;
			this->paired =			paired;
			this->dc =				0;
		};
	};
	
	texture_set *textures;
	tile_set *objects;
	
	vector<door_conf> doors;

public:
	conf_manager() {
		this->textures =			0;
		this->objects =				0;
	};

	bool create(texture_set &textures, tile_set &objects);
	void update();

	door_conf *get_door_conf(unsigned short pos) {return &this->doors[pos];};
	door_conf *get_door_conf(IND_Entity2d &entity);
	unsigned short get_doors_count() {return this->doors.size();};

	void render_links(IND_Render &render);

	bool save(const char *conf_file);
	bool load(const char *conf_file);

	void clear();
};

#endif