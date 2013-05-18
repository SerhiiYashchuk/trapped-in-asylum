#include "interface.h"

//-------------------info class-------------------

// Set font

void info::set_font(IND_Font &font, char ch_spc, char l_spc, IND_Align align) {
	this->entity.SetFont(&font);
	this->entity.SetCharSpacing(ch_spc);
	this->entity.SetLineSpacing(l_spc);
	this->entity.SetAlign(align);
}

// Set text

void info::set_text(const char *text) {
	if (this->text) delete this->text;
	this->text =							new char[strlen(text) + 1];
	strcpy(this->text, text);
	this->entity.SetText(this->text);
}

// Add text to existing

void info::add_text(const char *text) {
	if (!this->text) this->set_text(text);
	else {
		short len =								strlen(this->text);
		char *tmp =								new char[len + 1];
		strcpy(tmp, this->text);
		delete this->text;
		this->text =							new char[len + strlen(text) + 1];
		strcpy(this->text, tmp);
		strcat(this->text, text);
		delete tmp;
		this->entity.SetText(this->text);
	}
}

//---------------Command line class---------------

// Constructor

cmd_line::cmd_line() {
	this->max_size =						500;
	this->cmd_str =							new char[this->max_size];
	strcpy(this->cmd_str, "");

	this->cur_size =						0;
}

// Destructor

cmd_line::~cmd_line() {
	delete this->cmd_str;
}

// Parse command line string

command cmd_line::parse() {
	command cmd;
	std::string tmp;
	short i;

	// Searching for command
	for (i = 0; this->cmd_str[i]; i++)
		if (this->cmd_str[i] == ' ') {
			if (tmp.size()) break;
		} else tmp +=						this->cmd_str[i];

	if (tmp == cmd_new) cmd.type =			cmdt_new;
	else if (tmp == cmd_save) cmd.type =	cmdt_save;
	else if (tmp == cmd_load) cmd.type =	cmdt_load;
	else {
		cmd.type =							cmdt_error;
		return cmd;
	}

	// Searching for parameter
	for ( ; this->cmd_str[i]; i++) {
		if (this->cmd_str[i] == ' ') {
			if (cmd.parameter.size()) break;
		} else cmd.parameter +=				this->cmd_str[i];
	}

	return cmd;
}

// Get pointer to command line string

char *cmd_line::get_string() {
	return this->cmd_str;
}

// Add symbol to the end of the command line string

void cmd_line::push_back(char ch) {
	if (this->cur_size < this->max_size - 1) {
		this->cmd_str[(this->cur_size)++] =	ch;
		this->cmd_str[this->cur_size] =		'\0';
	}
}

// Add symbols to the end of the command line string

void cmd_line::push_back(const char *ch) {
	if (ch && this->cur_size + strlen(ch) < this->max_size - 1) {
		this->cur_size +=					strlen(ch);
		strcat(this->cmd_str, ch);
	}
}

// Delete and return last symbol from the command line string

char cmd_line::pop_back() {
	if (!this->cur_size) return '\0';

	char last =								this->cmd_str[--(this->cur_size)];
	this->cmd_str[this->cur_size] =			'\0';

	return last;
}

// Clear command line string

void cmd_line::clear() {
	strcpy(this->cmd_str, "");
	this->cur_size =						0;
}