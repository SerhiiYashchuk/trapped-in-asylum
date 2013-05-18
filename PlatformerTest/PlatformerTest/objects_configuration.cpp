#include "objects_configuration.h"

//---------------constants---------------

const char door_keyword[] =						"door";

//---------------config manager class---------------

// Create configurations

bool conf_manager::create(texture_set &textures, tile_set &objects) {
	this->clear();
	
	this->textures =							&textures;
	this->objects =								&objects;
	
	// Creating doors configurations
	for (unsigned short i = 0; i < objects.size(); i++) {
		if (textures.get_name(objects.get_tile(i)->GetSurface()).find(door_keyword) != string::npos)
			this->doors.push_back(door_conf(*objects.get_tile(i)));
	}

	return 1;
}

// Update configurations

void conf_manager::update() {
	if (!this->textures || !this->objects) return;

	// Check existing configurations
	for (unsigned short i = 0; i < this->doors.size(); i++) {
		for (unsigned short j = 0; j < this->objects->size(); j++)
			if (this->doors[i].entity == this->objects->get_tile(j)) break;
			else if (j == this->objects->size() - 1) {
				if (this->doors[i].paired)
					this->doors[i].paired->paired = 0;
				this->doors.erase(this->doors.begin() + i);
			}
	}

	// Add new configurations
	for (unsigned short i = 0; i < this->objects->size(); i++) {
		if (this->textures->get_name(this->objects->get_tile(i)->GetSurface()).find(door_keyword) != string::npos)
			for (unsigned short j = 0; j < this->doors.size(); j++)
				if (this->objects->get_tile(i) == this->doors[j].entity) break;
				else if (j == this->doors.size() - 1)
					this->doors.push_back(door_conf(*this->objects->get_tile(i)));
	}
}

// Get door configuration by entity

door_conf *conf_manager::get_door_conf(IND_Entity2d &entity) {
	for (unsigned short i = 0; i < this->doors.size(); i++)
		if (&entity == this->doors[i].entity)
			return &this->doors[i];

	return 0;
}

// Render links between doors

void conf_manager::render_links(IND_Render &render) {
	for (unsigned short i = 0; i < this->doors.size(); i++)
		if (this->doors[i].paired)
			render.BlitLine((int) this->doors[i].entity->GetPosX(), (int) this->doors[i].entity->GetPosY(),
				(int) this->doors[i].paired->entity->GetPosX(), (int) this->doors[i].paired->entity->GetPosY(),
				0, 0, 255, 255);
}

// Save configurations to file

bool conf_manager::save(const char *conf_file) {
	if (!strlen(conf_file)) return 0;

	for (unsigned short i = 0, id = 0; i < this->doors.size(); i++)
		if (this->doors[i].paired)
			this->doors[i].ID =					(char) ++id;

	TiXmlDocument doc;

	TiXmlDeclaration *decl =					new TiXmlDeclaration("1.0", "", "");

	TiXmlElement *doors =						new TiXmlElement(configuration::doors_section);

	doc.LinkEndChild(decl);
	doc.LinkEndChild(doors);

	for (unsigned short i = 0; i < this->doors.size(); i++) {
		if (!this->doors[i].paired) continue;
		
		TiXmlElement *door =					new TiXmlElement(configuration::doors_element);

		door->SetAttribute(configuration::attr_id, this->doors[i].ID);
		door->SetAttribute(configuration::attr_pos_x, (int) this->doors[i].entity->GetPosX());
		door->SetAttribute(configuration::attr_pos_y, (int) this->doors[i].entity->GetPosY());
		door->SetAttribute(configuration::attr_paired, this->doors[i].paired->ID);

		doors->LinkEndChild(door);
	}

	return doc.SaveFile(conf_file);
}

// Load configurations from file

bool conf_manager::load(const char *conf_file) {
	if (!strlen(conf_file) || !this->textures || !this->objects) return 0;

	TiXmlDocument doc;

	if (doc.LoadFile(conf_file)) {
		TiXmlElement *doors =					doc.FirstChildElement(configuration::doors_section);

		if (doors) {
			TiXmlElement *door =				doors->FirstChildElement(configuration::doors_element);

			vector<tmp_conf> conf;
			tmp_conf tmp_c;

			while (door) {
				door->Attribute(configuration::attr_pos_x, &tmp_c.pos_x);
				door->Attribute(configuration::attr_pos_y, &tmp_c.pos_y);
				door->Attribute(configuration::attr_id, &tmp_c.ID);
				door->Attribute(configuration::attr_paired, &tmp_c.paired);

				conf.push_back(tmp_c);

				door =							door->NextSiblingElement(configuration::doors_element);
			}

			// Setting IDs for configured doors
			for (unsigned short i = 0; i < conf.size(); i++) {
				for (unsigned short j = 0; j < this->doors.size(); j++)
					if ((int) this->doors[j].entity->GetPosX() == conf[i].pos_x && (int) this->doors[j].entity->GetPosY() == conf[i].pos_y) {
						this->doors[j].ID =		conf[i].ID;
						conf[i].dc =			&this->doors[j];
						break;
					}
			}

			// Linking configured doors
			for (short i = 0; i < conf.size(); i++) {
				for (unsigned short j = i + 1; j < conf.size(); j++)
					if (conf[i].paired == conf[j].ID) {
						conf[i].dc->paired =	conf[j].dc;
						conf[j].dc->paired =	conf[i].dc;

						conf.erase(conf.begin() + j);
						conf.erase(conf.begin() + i--);

						break;
					}
			}

			conf.clear();

			return 1;

		} else return 0;

	} else return 0;
}

// Clear all configurations

void conf_manager::clear() {
	this->textures =							0;
	this->objects =								0;
	this->doors.clear();
}