#ifndef INTERFACE_H
#define INTERFACE_H

#include "CIndieLib_vc2008.h"

const unsigned char interface_layer =				5;

//----------MessageBox class----------

class Message_Box {
private:
	IND_Entity2d entity;
	IND_Surface surface;
public:
	Message_Box();
	void StartGameNotes();
	void FindKeyNotes();
	void Show(bool flag);
};

//----------ProgressBar class----------

class ProgressBar {
private:
	IND_Entity2d entity;
	IND_Surface surface;
	IND_Entity2d eBar;
	IND_Surface sBar;
public:
	ProgressBar();
	void SetValue(float fear);
};

//------------------------Info class-------------------------

class info {
	static const char num_size = 15;
	
	IND_Entity2d entity;
	char *text;
	//Message_Box messageBox;
	//ProgressBar progressBar;

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
	//Message_Box *GetMessageBox() {return &this->messageBox;}
	//ProgressBar *GetProgressBar() {return &this->progressBar;}
};

#endif