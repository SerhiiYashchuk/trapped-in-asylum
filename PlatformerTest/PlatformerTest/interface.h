#ifndef INTERFACE_H
#define INTERFACE_H

#include "CIndieLib_vc2008.h"
#include "tiles.h"
const unsigned char interface_layer =				5;
const char PathToMessageBoxImage[] = "..\\res\\notebook";

//----------MessageBox class----------

class Message_Box {
private:
	texture_set *set;
	IND_Entity2d entity;
	IND_Surface surface;
	char dir_path[50];
	IND_Camera2d *camera;
public:
	Message_Box(IND_Camera2d &camera);
	int SetImage(char *name);
	void ShowMessageBox(bool flag);
	void SetTexture_set(texture_set &texture) {this->set = &texture;}
};

//----------ProgressBar class----------

class ProgressBar {
private:
	IND_Entity2d border;
	//IND_Surface *bsurface;
	IND_Entity2d filler;
	//IND_Surface *fsurface;
	IND_Font small_text;
	IND_Entity2d text;

	float value;
	float max_value;

public:
	ProgressBar();
	void SetFont(IND_Font &font);
	void SetBorder(IND_Surface &border) {this->border.SetSurface(&border);};
	void SetFiller(IND_Surface &filler) {this->filler.SetSurface(&filler);};
	void SetValue(float fear);
	float GetValue() {return this->value;};
	void SetPosition(float x, float y);
	//int load();
	void Show (bool flag);
};

//------------------------Info class-------------------------

class info {
	static const char num_size = 15;
	
	IND_Entity2d entity;
	char *text;

public:
	info() : text(0) {CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->entity);};

	~info() {if (this->text) delete this->text;};

	IND_Entity2d &get_entity() {return this->entity;};

	void set_position(int x, int y, short z = 0) {this->entity.SetPosition((float) x, (float) y, z);};
	void set_font(IND_Font &font, unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
	void set_font_params(char ch_spc = -8, char l_spc = 18, IND_Align align = IND_LEFT);
	void set_text(const char *text);
	void set_text(int num);
	void add_text(const char *text);
	void add_text(int num);
	const char *get_text() {return this->text;};
};

//---------------------------Main_Page---------------------------

class main_page {
	IND_Surface eyesSurface;
	IND_Entity2d eyesEntity;

	IND_Surface main_pageSurface;
	IND_Entity2d main_pageEntity;

	IND_Surface playSurface;
	IND_Entity2d playEntity;

	IND_Surface quitSurface;
	IND_Entity2d quitEntity;

	bool active;
public:
	unsigned int load();
	void SetEyesPosition(float x, float y);
	void SetPlayPosition(float x, float y);
	void SetQuitPosition(float x, float y);
	void SetActivePlay ();
	void SetActiveQuit ();
	bool PlayActive ();
	void Show (bool flag);
};

#endif