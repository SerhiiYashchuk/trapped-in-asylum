#include "interface.h"

// My comment

//------------------------Info class------------------------

// Set font and tint

void info::set_font(IND_Font &font, unsigned char r, unsigned char g, unsigned char b) {
	this->entity.SetFont(&font);
	this->entity.SetTint(r, g, b);
}

// Set font params

void info::set_font_params(char ch_spc, char l_spc, IND_Align align) {
	this->entity.SetCharSpacing(ch_spc);
	this->entity.SetLineSpacing(l_spc);
	this->entity.SetAlign(align);
}

// Set text

void info::set_text(const char *text) {
	if (this->text) delete this->text;
	this->text =								new char[strlen(text) + 1];
	strcpy(this->text, text);
	this->entity.SetText(this->text);
}

// Set number as text

void info::set_text(int num) {
	if (this->text) delete this->text;
	this->text =								new char[this->num_size + 1];
	//char buf[20];
	itoa(num, this->text, 10);
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

// Add number to existing text

void info::add_text(int num) {
	if (!this->text) this->set_text(num);
	else {
		short len =								strlen(this->text);
		char *tmp =								new char[len + 1];
		strcpy(tmp, this->text);
		delete this->text;
		this->text =							new char[len + this->num_size + 1];
		strcpy(this->text, tmp);
		strcat(this->text, itoa(num, tmp, 10));
		delete tmp;
		this->entity.SetText(this->text);
	}
}

//----------MessageBox class----------

// Constructor

Message_Box::Message_Box() {
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->entity);
	strcpy(dir_path,"..\\res\\notebook\\");
}


int Message_Box::SetImage(char *name) {
	strcat(dir_path,name);

	if (!surface.IsHaveSurface())
		CIndieLib::Instance()->SurfaceManager->Delete(&this->surface);
	else {
		CIndieLib::Instance()->SurfaceManager->Add(&this->surface, dir_path, IND_ALPHA, IND_32);
		this->entity.SetSurface(&this->surface);
		this->entity.SetPosition((float)CIndieLib::Instance()->Window->GetWidth()/2 - (float)this->entity.GetSurface()->GetWidth()/2, (float)CIndieLib::Instance()->Window->GetHeight()/2+(float)this->entity.GetSurface()->GetHeight()/2, 5);
	}

	strcpy(dir_path, "..\\res\\notebook\\");
	
	if(this->surface.IsHaveSurface())
		return 1;
	return 0;
}

void Message_Box::ShowMessageBox(bool flag) {
	this->entity.SetShow(flag);
}

//----------ProgressBar----------

ProgressBar::ProgressBar(){
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->entity);
	CIndieLib::Instance()->SurfaceManager->Add(&this->surface, "..\\res\\progress bar\\progress_bar_fear.png", IND_ALPHA, IND_32);
	this->entity.SetSurface(&this->surface);
	this->entity.SetPosition(10,10,0);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->eBar);
	CIndieLib::Instance()->SurfaceManager->Add(&this->sBar, "..\\res\\progress bar\\gradient.png", IND_ALPHA, IND_32);
	this->eBar.SetSurface(&this->sBar);
	this->eBar.SetPosition(12,12,0);
}

void ProgressBar::SetValue(float fear){
	if (fear < 2) return;
	this->eBar.SetScale(fear/2, 1.0);
}