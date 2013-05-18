#ifndef CURSOR_H
#define CURSOR_H

#include "tiles.h"
#include "objects_configuration.h"

const char cursor_radius =					7;
const char tpoint_radius =					8;

//---------------cursor class---------------

class basic_cursor {
	
protected:
	CIndieLib *engine;
	IND_Entity2d entity;
	IND_Surface surface;

public:
	basic_cursor();
	~basic_cursor();

	bool init(CIndieLib &engine);

	virtual void move(int x, int y) {
		this->entity.SetPosition((float) x, (float) y, this->entity.GetPosZ());
	};

	void show() {this->entity.SetShow(true);};
	void hide() {this->entity.SetShow(false);};

	virtual void check_click() =	0;
};

//---------------editing cursor class---------------

class editing_cursor : virtual public basic_cursor {
protected:
	IND_Entity2d *a_tile;
	bool not_saved;

public:
	editing_cursor() : basic_cursor() {
		this->a_tile =				0;
		this->not_saved =			false;
	};
	~editing_cursor();
	
	IND_Entity2d *active_tile() {return this->a_tile;};

	virtual bool add() =			0;
	virtual void del() =			0;
};

//---------------tile cursor class---------------

class tile_cursor : virtual public editing_cursor {
	tile_set *tiles;
	texture_set *textures;
	unsigned short cur_texture;

public:
	tile_cursor() : editing_cursor() {
		this->tiles =				0;
		this->textures =			0;
		this->cur_texture =			0;
	};

	tile_set &cur_tiles() {return *this->tiles;};
	void set_tiles(tile_set &tiles) {this->tiles = &tiles;};

	void set_textures(texture_set &textures) {
		this->textures = &textures; this->cur_texture = 0;
	};

	void next_texture();
	void prev_texture();

	bool add();
	void del();
	
	void move(int x, int y) {
		this->entity.SetPosition((float) x, (float) y, this->entity.GetPosZ());
		if (this->a_tile)
			this->a_tile->SetPosition((float) this->entity.GetPosX(), (float) this->entity.GetPosY(), this->a_tile->GetPosZ());
	};

	void check_click();

	void rotate_right();
	void rotate_left();
	void set_basic_rotation() {if (this->a_tile) this->a_tile->SetAngleXYZ(0, 0, 0);};

	void scale_in();
	void scale_out();

	unsigned short tile_zpos() {
		if (this->a_tile) return this->a_tile->GetPosZ();
		else return 0;
	};
	void set_zpos(unsigned short z) {
		if (this->a_tile && z < 64)
			this->a_tile->SetPosition(this->a_tile->GetPosX(), this->a_tile->GetPosY(), z);
	};
};

//---------------config cursor class---------------

class config_cursor : virtual public editing_cursor {
	conf_manager *conf_m;
	door_conf *a_door;

public:
	config_cursor(conf_manager &cm) : editing_cursor() {
		this->conf_m =				&cm;
		this->a_door =				0;
	};

	void check_click();

	bool add();
	void del();
};

#endif