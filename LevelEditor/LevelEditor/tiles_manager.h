#ifndef TILES_MANAGER_H
#define TILES_MANAGER_H

#include "tiles.h"
#include "tinyxml\tinyxml.h"
#include <fstream>

const unsigned char background_layer =			0;
const unsigned char objects_layer =				1;
const unsigned char items_layer =				2;

const char element_name[] =						"tile";

const char texture_a[] =						"texture";
const char x_a[] =								"x";
const char y_a[] =								"y";
const char z_a[] =								"z";
const char angle_a[] =							"angle";
const char scale_a[] =							"scale";

#define entity_vector vector<IND_Entity2d *>

//---------------tile info struct---------------

struct tile_info {
	int pos_x;
	int pos_y;
	int pos_z;
	int angle;
	double scale;
	string texture;
	tile_info() {
		this->pos_x =							0;
		this->pos_y =							0;
		this->pos_z =							0;
		this->angle =							0;
		this->scale =							0;
	};
};

//---------------level tiles manager class---------------

class lt_manager {
	texture_set *textures;																			// Pointer to set of textures
	vector<tile_set *> tiles;

	string lt_file;																					// Path to level tiles' file

public:
	lt_manager();																					// Constructor with a pointer to CIndieLib engine as a parameter
	~lt_manager();
	
	bool load_textures(const char *tex_dir);														// Loads textures that are needed for tiles. Returns 1 if textures are loaded; 0 if some error occurred
	
	bool load(const char *lt_file);																	// Loads tiles from the given file. Returns 1 if tiles are loaded; 0 if some error occurred
	bool save(const char *lt_file);																	// Saves tiles to the given file. Returns 1 if tiles are saved; 0 if some error occurred
	void clear();																					// Clears the current tiles
	const char *get_lt_file() {return this->lt_file.c_str();};										// Returns a path to level tiles' file

	texture_set &get_textures() {return *this->textures;};											// Returns texture set

	tile_set &get_tiles(unsigned short num) {return *this->tiles[num];};
	//tile_set &get_background() {return *this->background;};											// Returns set of entities for ground layer
	//tile_set &get_objects() {return *this->objects;};												// Returns set of entities for terrain layer
	//tile_set &get_items() {return *this->items;};
};

#endif