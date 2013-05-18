#ifndef TILES_H
#define TILES_H

#include "CIndieLib_vc2008.h"
#include <string>
#include <fstream>

// Constants

const char background_set[] =						"BACKGROUND";
const char objects_set[] =							"OBJECTS";
const char items_set[] =							"ITEMS";

const char unnamed_lt_file[] =						"Unnamed file";

const char error_texture[] =						"ERROR_TEXTURE";

const char lfile_name[] =							"!list.txt";
const char basic_tset_name[] =						"New tile set";

//---------------texture set class---------------

class texture_set {
	vector<IND_Surface *> surface;
	vector<string> name;

	string texture_dir;
	
public:
	texture_set() {};
	~texture_set();

	void set_texture_dir(const char *dir);
	bool load();
	void clear();

	IND_Surface *get_texture(unsigned short pos);
	IND_Surface *get_texture(const char *name);

	string get_name(unsigned short pos);
	string get_name(IND_Surface *surface);

	unsigned short size();
};

//---------------tile set class---------------

class tile_set {
	vector<IND_Entity2d *> entity;
	unsigned char layer;

	string tset_name;

public:
	tile_set(const char *set_name, unsigned char entity_layer) : tset_name(set_name), layer(entity_layer) {};
	~tile_set();

	bool add_tile();
	void add_tile(IND_Entity2d *tile);
	bool del_tile(unsigned short pos);
	bool del_tile(IND_Entity2d *entity);
	IND_Entity2d *get_tile(unsigned short pos);
	void clear();
	
	unsigned short size();
	const char *get_name();
	void reset_name(const char *name);

	unsigned char get_layer() {return this->layer;};
};

#endif