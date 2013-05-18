#include "tiles.h"

//---------------texture set class---------------

// Destructor

texture_set::~texture_set() {
	for (unsigned short i = 0; i < this->surface.size(); i++) {
		if (CIndieLib::Instance()->SurfaceManager->IsOk())
			CIndieLib::Instance()->SurfaceManager->Delete(this->surface[i]);
		delete this->surface[i];
	}
}

// Set path to the textures directory

void texture_set::set_texture_dir(const char *dir) {
	this->texture_dir = dir;
}

// Loading set of tiles from the list (*.txt file)

bool texture_set::load() {
	string buf;

	bool error =								false;

	if (!this->texture_dir.size()) return 0;
	string lfile =								this->texture_dir;
	lfile +=									"\\";
	lfile +=									lfile_name;

	IND_Surface *new_texture;
	char *texture_path =						new char[this->texture_dir.size() + 100];

	ifstream istream;
	istream.open(lfile, ios::in);
	if (!istream) return 0;
	while (!istream.eof() && !error) {
		if (istream >> buf) {
			strcpy(texture_path, this->texture_dir.c_str());
			strcat(texture_path, "\\");
			strcat(texture_path, buf.c_str());

			new_texture =						new IND_Surface;
			if (new_texture) {
				if (!CIndieLib::Instance()->SurfaceManager->Add(new_texture, texture_path, IND_ALPHA, IND_32)) {
					delete new_texture;
					error =						true;
				}
				name.push_back(buf);
				surface.push_back(new_texture);
			} else error =						true;
		}
	}
	istream.close();
	delete texture_path;

	if (error) {
		this->clear();
		return 0;
	}

	return 1;
}

// Free set

void texture_set::clear() {
	for (unsigned short i = 0; i < this->surface.size(); i++) {
		if (CIndieLib::Instance()->SurfaceManager->IsOk())
			CIndieLib::Instance()->SurfaceManager->Delete(this->surface[i]);
		delete this->surface[i];
	}
	
	this->surface.clear();
	this->name.clear();
}

// Get pointer to surface by its' position

IND_Surface *texture_set::get_texture(unsigned short pos) {
	if (pos < surface.size())
		return surface[pos];
	else return 0;
}

// Get pointer to surface by its' name

IND_Surface *texture_set::get_texture(const char *name) {
	for (unsigned short i = 0; i < this->surface.size(); i++)
		if (this->name[i] == name)
			return this->surface[i];
	return 0;
}

// Get name of exact surface by its' position

string texture_set::get_name(unsigned short pos) {
	if (pos < this->name.size())
		return this->name[pos];
	else return error_texture;
}

// Get name of exact surface by its' pointer

string texture_set::get_name(IND_Surface *surface) {
	if (surface && this->surface.size())
		for (unsigned short i = 0; i < this->surface.size(); i++)
			if (this->surface[i] == surface)
				return this->name[i];
	return error_texture;
}

// Get size of the set

unsigned short texture_set::size() {
	return this->surface.size();
}

//---------------tile set class---------------

// Destructor

tile_set::~tile_set() {
	for (unsigned short i = 0; i < this->entity.size(); i++) {
		if (CIndieLib::Instance()->Entity2dManager->IsOk())
			CIndieLib::Instance()->Entity2dManager->Delete(this->entity[i]);
		delete entity[i];
	}
}

// Clear set

void tile_set::clear() {
	for (unsigned short i = 0; i < this->entity.size(); i++) {
		CIndieLib::Instance()->Entity2dManager->Delete(this->entity[i]);
		delete this->entity[i];
	}
	
	this->entity.clear();
}

// Create and add tile to the set

bool tile_set::add_tile() {
	IND_Entity2d *new_tile =			new IND_Entity2d;
	if (new_tile) {
		if (!CIndieLib::Instance()->Entity2dManager->Add(this->layer, new_tile)) {
			delete new_tile;
			return 0;
		}
		entity.push_back(new_tile);
		return 1;
	} else return 0;
}

// Add existing tile to the set

void tile_set::add_tile(IND_Entity2d *tile) {
	CIndieLib::Instance()->Entity2dManager->Delete(tile);
	if (tile) {
		CIndieLib::Instance()->Entity2dManager->Add(this->layer, tile);
		entity.push_back(tile);
	}
}

// Delete tile by its' position

bool tile_set::del_tile(unsigned short pos) {
	if (pos < entity.size()) {
		CIndieLib::Instance()->Entity2dManager->Delete(this->entity[pos]);
		delete this->entity[pos];
		this->entity.erase(this->entity.begin() + pos);
		return 1;
	} else return 0;
}

// Delete tile by its' pointer

bool tile_set::del_tile(IND_Entity2d *entity) {
	for (unsigned short i = 0; i < this->entity.size(); i++)
		if (this->entity[i] == entity) {
			CIndieLib::Instance()->Entity2dManager->Delete(this->entity[i]);
			delete this->entity[i];
			this->entity.erase(this->entity.begin() + i);
			return 1;
		}

	return 0;
}

// Get pointer to entity by its' position

IND_Entity2d *tile_set::get_tile(unsigned short pos) {
	if (pos < this->entity.size())
		return this->entity[pos];
	else return 0;
}

// Get size of the set

unsigned short tile_set::size() {
	return this->entity.size();
}

// Get set name

const char *tile_set::get_name() {
	return this->tset_name.c_str();
}

// Set tile set name

void tile_set::reset_name(const char *name) {
	if (strlen(name)) this->tset_name = name;
}