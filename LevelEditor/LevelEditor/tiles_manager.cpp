#include "tiles_manager.h"

//---------------Level tiles manager class---------------

// Constructor

lt_manager::lt_manager() {
	this->textures =						new texture_set();
	this->tiles.push_back(new tile_set(background_set, background_layer));
	this->tiles.push_back(new tile_set(objects_set, objects_layer));
	this->tiles.push_back(new tile_set(items_set, items_layer));
}

// Destructor

lt_manager::~lt_manager() {
	delete this->textures;
	for (unsigned short i = 0; i < this->tiles.size(); i++)
		delete this->tiles[i];
}

// Load textures

bool lt_manager::load_textures(const char *tex_dir) {
	this->textures->clear();
	this->textures->set_texture_dir(tex_dir);

	return this->textures->load();
}

// Load level tiles

bool lt_manager::load(const char *lt_file) {
	if (!strlen(lt_file)) return 0;

	bool error =							false;

	this->clear();

	TiXmlDocument doc(lt_file);

	if (doc.LoadFile()) {
		TiXmlElement *tset =				doc.FirstChildElement();

		while (tset) {
			tile_set *current_set =			0;

			for (unsigned short i = 0; i < this->tiles.size(); i++)
				if (string(tset->Value()) == string(this->tiles[i]->get_name())) {
					current_set =			this->tiles[i];
					break;
				} else current_set =		0;

			if (current_set) {
				TiXmlElement *tile =		tset->FirstChildElement(element_name);

				while (tile && !error) {
					if (current_set->add_tile()) {
						IND_Entity2d *current_tile =	current_set->get_tile(current_set->size() - 1);
						tile_info tinfo;

						tinfo.texture =		tile->Attribute(texture_a);
						tile->Attribute(x_a, &tinfo.pos_x);
						tile->Attribute(y_a, &tinfo.pos_y);
						tile->Attribute(z_a, &tinfo.pos_z);
						tile->Attribute(angle_a, &tinfo.angle);
						tile->Attribute(scale_a, &tinfo.scale);

						current_tile->SetSurface(this->textures->get_texture(tinfo.texture.c_str()));
						current_tile->SetBoundingRectangle("border", 0, 0, current_tile->GetSurface()->GetWidth(), current_tile->GetSurface()->GetHeight());
						current_tile->SetHotSpot(0.5f, 0.5f);
						current_tile->SetPosition((float) tinfo.pos_x, (float) tinfo.pos_y, tinfo.pos_z);
						current_tile->SetAngleXYZ(0, 0, (float) tinfo.angle);
						current_tile->SetScale((float) tinfo.scale, (float) tinfo.scale);

					} else error =			true;

					tile =					tile->NextSiblingElement(element_name);
				}

				tset =						tset->NextSiblingElement();

			} else error =					true;

		}

	} else error =							true;

	doc.Clear();

	if (error) {
		for (unsigned short i = 0; i < this->tiles.size(); i++)
			this->tiles[i]->clear();

		return 0;
	}

	this->lt_file =							lt_file;

	return 1;
}

// Save level tiles

bool lt_manager::save(const char *lt_file) {
	if (!strlen(lt_file)) return 0;

	TiXmlDocument doc(lt_file);

	TiXmlDeclaration *decl =				new TiXmlDeclaration("1.0", "", "");

	doc.LinkEndChild(decl);

	for (unsigned short t = 0; t < this->tiles.size(); t++) {
		TiXmlElement *tset =				new TiXmlElement(this->tiles[t]->get_name());
		
		for (unsigned short i = 0; i < this->tiles[t]->size(); i++) {
			TiXmlElement *tile =			new TiXmlElement(element_name);

			tile->SetAttribute(texture_a, this->textures->get_name(this->tiles[t]->get_tile(i)->GetSurface()));
			tile->SetAttribute(x_a, (int) this->tiles[t]->get_tile(i)->GetPosX());
			tile->SetAttribute(y_a, (int) this->tiles[t]->get_tile(i)->GetPosY());
			tile->SetAttribute(z_a, (int) this->tiles[t]->get_tile(i)->GetPosZ());
			tile->SetAttribute(angle_a, (int) this->tiles[t]->get_tile(i)->GetAngleZ());
			tile->SetDoubleAttribute(scale_a, this->tiles[t]->get_tile(i)->GetScaleX());

			tset->LinkEndChild(tile);
		}

		doc.LinkEndChild(tset);
	}

	bool error =							!doc.SaveFile();
	if (!error) this->lt_file =				lt_file;

	doc.Clear();

	return !error;
}

// Clear

void lt_manager::clear() {
	for (unsigned short i = 0; i < this->tiles.size(); i++)
		this->tiles[i]->clear();

	this->lt_file =							"";
}