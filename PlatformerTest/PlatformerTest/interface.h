#ifndef INTERFACE_H
#define INTERFACE_H

#include "CIndieLib_vc2008.h"
#include "tiles.h"

const unsigned char interface_layer =				5;
const char messages_directory[] =					"..\\res\\messages";
const char message_image_modifier[] =				"_found.png";

//----------MessageBox class----------

class MessageBox {
protected:
	IND_Entity2d entity;

public:
	MessageBox();
	void SetImage(IND_Surface &image);
	virtual void Show(int pos_x, int pos_y);

	bool IsShow() {return this->entity.IsShow();};
};

//---------Quick MessageBox class---------

class QMessageBox : virtual public MessageBox {
	IND_Timer time;
	unsigned char show_time;

public:
	QMessageBox() : show_time(4) {};

	void SetTime(unsigned char seconds) {this->show_time = seconds;};
	unsigned char GetTime() {return this->show_time;};

	void Show(int pos_x, int pos_y);

	void Update(int pos_x, int pos_y);
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
	void SetMaxValue(float max_value) {this->max_value = max_value;};
	float GetMaxValue() {return this->max_value;};
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
protected:
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
	virtual unsigned int load();
	virtual void SetEyesPosition(float x, float y);
	virtual void SetPlayPosition(float x, float y);
	virtual void SetQuitPosition(float x, float y);
	virtual void SetActivePlay ();
	virtual void SetActiveQuit ();
	virtual bool PlayActive ();
	void Show (bool flag);
};

class pause : public main_page {
public:
	unsigned int load ();
	void SetPlayPosition (float x, float y);
	void SetQuitPosition (float x, float y);
	void SetActivePlay ();
	void SetActiveQuit ();
	bool PlayActive ();
	void SetmainPosition (float x, float y);
};

class Game_over : public main_page {
public:
	unsigned int load ();
	void SetPlayPosition (float x, float y);
	void SetmainPosition (float x, float y);
};

#endif