#ifndef INTERFACE_H
#define INTERFACE_H

#include <string>
#include "CIndieLib_vc2008.h"

const char cmdt_error =					'e';
const char cmdt_new =					'n';
const char cmdt_save =					's';
const char cmdt_load =					'l';

const char cmd_new[] =					"new-level";
const char cmd_save[] =					"save";
const char cmd_load[] =					"load";

//-------------------info class-------------------

class info {
	IND_Entity2d entity;
	char *text;

public:
	info() {this->text = 0;};

	~info() {if (this->text) delete this->text;};

	IND_Entity2d &get_entity() {return this->entity;};

	void set_position(int x, int y, short z) {this->entity.SetPosition((float) x, (float) y, z);};
	void set_font(IND_Font &font, char ch_spc = -8, char l_spc = 18, IND_Align align = IND_LEFT);
	void set_text(const char *text);
	void add_text(const char *text);
	const char *get_text() {return this->text;};
};

//---------------Command structure---------------

struct command {
	char type;
	std::string parameter;

	command() {
		this->type =					cmdt_error;
		this->parameter =				"";
	};
	command(char type, const char *parameter) {
		this->type =					type;
		this->parameter =				parameter;
	};
	command(char type, std::string parameter) {
		this->type =					type;
		this->parameter =				parameter;
	};
};

//---------------Command line class---------------

class cmd_line {
	char *cmd_str;
	short max_size;
	short cur_size;

public:
	cmd_line();
	~cmd_line();

	command parse();

	char *get_string();
	void push_back(const char *ch);
	void push_back(char ch);
	char pop_back();
	void clear();
};

#endif