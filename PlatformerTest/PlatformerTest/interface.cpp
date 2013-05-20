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

Message_Box::Message_Box(IND_Camera2d &camera) {
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->entity);
	this->camera = &camera;
}


int Message_Box::SetImage(char *name) {

	if (!surface.IsHaveSurface()) return 0;
	else {
		this->entity.SetSurface(set->get_texture(name));
		this->entity.SetHotSpot(0.5f,0.5f);
	}
	
	if(this->surface.IsHaveSurface())
		return 1;
	return 0;
}

void Message_Box::ShowMessageBox(bool flag) {
	this->entity.SetShow(flag);
	this->entity.SetPosition(camera->GetPosX(), camera->GetPosY(), interface_layer);
}

//----------ProgressBar----------

ProgressBar::ProgressBar(){
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->entity);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->eBar);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->text);

}

int ProgressBar::load(){
	if(!CIndieLib::Instance()->SurfaceManager->Add(&this->surface, "..\\res\\progress_bar\\progress_bar_fear.png", IND_ALPHA, IND_32)) return 0;
	this->entity.SetSurface(&this->surface);

	if(!CIndieLib::Instance()->SurfaceManager->Add(&this->sBar, "..\\res\\progress_bar\\gradient.png", IND_ALPHA, IND_32)) return 0;
	this->eBar.SetSurface(&this->sBar);

	return 1;
}

void ProgressBar::SetValue(float fear){
	if (fear == 0) 
		this->eBar.SetShow(false);

	if (fear < 2) return;
	this->eBar.SetShow(true);
	this->eBar.SetScale(fear/2, 1.0);
}

void ProgressBar::SetPosition (float x, float y) {
	this->entity.SetPosition(x, y, 0);
	this->eBar.SetPosition(x+2, y+2, 0);

	this->text.SetPosition(x-40, y-8, 1);
	
}

void ProgressBar::SetFont (IND_Font &font) {
	this->small_text = font;
	this->text.SetFont(&font);
	this->text.SetShow(true);
	this->text.SetAlign(IND_LEFT);
	this->text.SetCharSpacing(-8);
	this->text.SetTint(255,0,0);
	this->text.SetText("Fear ");
} 
