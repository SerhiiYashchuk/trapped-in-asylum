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

MessageBox::MessageBox() {
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->entity);
	this->entity.SetShow(false);
}

// Set message image

void MessageBox::SetImage(IND_Surface &image) {
	this->entity.SetSurface(&image);
	this->entity.SetHotSpot(0.5f, 0.5f);
}

// Show message

void MessageBox::Show(int pos_x, int pos_y) {
	this->entity.SetPosition((float) pos_x, (float) pos_y, this->entity.GetPosZ());
	this->entity.SetShow(true);
	this->entity.SetTransparency(0);
}

//----------Quick MessageBox class---------

// Show message

void QMessageBox::Show(int pos_x, int pos_y) {
	MessageBox::Show(pos_x, pos_y);
	this->time.Start();
}

// Update

void QMessageBox::Update(int pos_x, int pos_y) {
	if (this->time.GetTicks() / 1000 >= this->show_time && this->entity.GetTransparency()) {
		this->entity.SetTransparency(this->entity.GetTransparency() - 5);
	} else if (this->entity.GetTransparency() < 255) {
		this->entity.SetTransparency(this->entity.GetTransparency() + 5);
	}

	if (!this->entity.GetTransparency()) {
		this->time.Stop();
		this->entity.SetShow(false);
	}

	this->entity.SetPosition((float) pos_x, (float) pos_y, this->entity.GetPosZ());
}

//----------ProgressBar----------

ProgressBar::ProgressBar(){
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->border);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->filler);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer, &this->text);

	this->value = 0;
	this->max_value = 0;
}
/*
int ProgressBar::load(){
	if(!CIndieLib::Instance()->SurfaceManager->Add(&this->surface, "..\\res\\progress_bar\\progress_bar_fear.png", IND_ALPHA, IND_32)) return 0;
	this->entity.SetSurface(&this->surface);

	if(!CIndieLib::Instance()->SurfaceManager->Add(&this->sBar, "..\\res\\progress_bar\\gradient.png", IND_ALPHA, IND_32)) return 0;
	this->eBar.SetSurface(&this->sBar);

	return 1;
}
*/
void ProgressBar::SetValue(float value){
	this->value = (value < this->max_value) ? (value) : (this->max_value);
	
	if (this->value == 0) 
		this->filler.SetShow(false);

	this->filler.SetShow(true);
	if (this->max_value)
		this->filler.SetScale(this->value / this->max_value, 1.0);
	else this->filler.SetScale(0, 1.0f);
}

void ProgressBar::SetPosition (float x, float y) {
	this->border.SetPosition(x, y, 1);
	this->filler.SetPosition(x+6, y+4, 0);

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

void ProgressBar::Show(bool flag) {
	this->border.SetShow(flag);
	this->filler.SetShow(flag);
}
//----------------------------main page-------------------

unsigned int main_page::load(){
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->main_pageSurface,"..\\res\\main_page\\mp.jpg", IND_ALPHA, IND_32))	return 0;
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->eyesSurface,"..\\res\\main_page\\peyes.png", IND_ALPHA, IND_32))		return 0;
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->playSurface, "..\\res\\main_page\\aplay.png", IND_ALPHA, IND_32))	return 0;
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->quitSurface, "..\\res\\main_page\\nquit.png", IND_ALPHA, IND_32))	return 0;

	CIndieLib::Instance()->Entity2dManager->Add(interface_layer,&this->main_pageEntity);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer,&this->playEntity);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer,&this->eyesEntity);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer,&this->quitEntity);

	this	->	main_pageEntity	.	SetSurface (&this->main_pageSurface);
	this	->	eyesEntity		.	SetSurface (&this->eyesSurface);
	this	->	playEntity		.	SetSurface (&this->playSurface);
	this	->	quitEntity		.	SetSurface (&this->quitSurface);

	this	->	main_pageEntity	.	SetHotSpot (0.5f,0.5f);
	this	->	playEntity		.	SetHotSpot (0.5f,0.5f);
	this	->	eyesEntity		.	SetHotSpot (0.5f,0.5f);
	this	->	quitEntity		.	SetHotSpot (0.5f,0.5f);
	this	->	main_pageEntity .	SetPosition(0,0,interface_layer);

	this->active = true;
	return 1;
}

void main_page::SetEyesPosition (float x, float y){
	this->eyesEntity.SetPosition(x, y, interface_layer);
}
void main_page::SetPlayPosition (float x, float y){
	this->playEntity.SetPosition(x, y, interface_layer);
}
void main_page::SetQuitPosition (float x, float y) {
	this->quitEntity.SetPosition(x, y, interface_layer);
} 
void main_page::SetActivePlay () {
	float x, y;

	x = this->playEntity.GetPosX();
	y = this->playEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->playSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->playSurface, "..\\res\\main_page\\aplay.png", IND_ALPHA, IND_32)) return;
	this->playEntity.SetSurface(&this->playSurface);

	this->playEntity.SetHotSpot(0.5f,0.5f);

	this->playEntity.SetPosition(x, y, interface_layer);

	x = this->quitEntity.GetPosX();
	y = this->quitEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->quitSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->quitSurface, "..\\res\\main_page\\nquit.png", IND_ALPHA, IND_32)) return;
	this->quitEntity.SetSurface(&this->quitSurface);

	this->quitEntity.SetHotSpot(0.5f,0.5f);

	this->quitEntity.SetPosition(x, y, interface_layer);	

	x = this->eyesEntity.GetPosX();
	y = this->eyesEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->eyesSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->eyesSurface, "..\\res\\main_page\\peyes.png", IND_ALPHA, IND_32)) return;
	this->eyesEntity.SetSurface(&this->eyesSurface);

	this->eyesEntity.SetHotSpot(0.5f, 0.5f);

	this->eyesEntity.SetPosition(x, y, interface_layer);
	
	this->active = true;
}
void main_page::SetActiveQuit () {
	float x, y;

	x = this->playEntity.GetPosX();
	y = this->playEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->playSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->playSurface, "..\\res\\main_page\\nplay.png", IND_ALPHA, IND_32)) return;
	this->playEntity.SetSurface(&this->playSurface);

	this->playEntity.SetHotSpot(0.5f,0.5f);

	this->playEntity.SetPosition(x, y, interface_layer);

	x = this->quitEntity.GetPosX();
	y = this->quitEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->quitSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->quitSurface, "..\\res\\main_page\\aquit.png", IND_ALPHA, IND_32)) return;
	this->quitEntity.SetSurface(&this->quitSurface);

	this->quitEntity.SetHotSpot(0.5f,0.5f);

	this->quitEntity.SetPosition(x, y, interface_layer);

	x = this->eyesEntity.GetPosX();
	y = this->eyesEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->eyesSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->eyesSurface, "..\\res\\main_page\\qeyes.png", IND_ALPHA, IND_32)) return;
	this->eyesEntity.SetSurface(&this->eyesSurface);

	this->eyesEntity.SetHotSpot(0.5f, 0.5f);

	this->eyesEntity.SetPosition(x, y, interface_layer);

	this->active = false;
}
bool main_page::PlayActive () {
	return active;
}
void main_page::Show (bool flag) {
	this->main_pageEntity.SetShow(flag);
	this->eyesEntity.SetShow(flag);
	this->playEntity.SetShow(flag);
	this->quitEntity.SetShow(flag);
}

//--------------------------pause--------------------------------

unsigned int pause::load () {
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->main_pageSurface,"..\\res\\pause\\main.png", IND_ALPHA, IND_32))	return 0;
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->playSurface, "..\\res\\pause\\aresume.png", IND_ALPHA, IND_32))	return 0;
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->quitSurface, "..\\res\\pause\\nquit.png", IND_ALPHA, IND_32))	return 0;

	CIndieLib::Instance()->Entity2dManager->Add(interface_layer,&this->main_pageEntity);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer,&this->playEntity);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer,&this->quitEntity);

	this	->	main_pageEntity	.	SetSurface (&this->main_pageSurface);
	this	->	playEntity		.	SetSurface (&this->playSurface);
	this	->	quitEntity		.	SetSurface (&this->quitSurface);

	this	->	main_pageEntity	.	SetHotSpot (0.5f,0.5f);
	this	->	playEntity		.	SetHotSpot (0.5f,0.5f);
	this	->	quitEntity		.	SetHotSpot (0.5f,0.5f);
	this	->	main_pageEntity .	SetPosition(0,0,interface_layer);

	this->active = true;
	return 1;
}

void pause::SetPlayPosition (float x, float y) {
	this->playEntity.SetPosition(x, y, interface_layer);
} 
void pause::SetQuitPosition (float x, float y) {
	this->quitEntity.SetPosition (x, y, interface_layer);
}
void pause::SetActivePlay () {
	float x, y;

	x = this->playEntity.GetPosX();
	y = this->playEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->playSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->playSurface, "..\\res\\pause\\aresume.png", IND_ALPHA, IND_32)) return;
	this->playEntity.SetSurface(&this->playSurface);

	this->playEntity.SetHotSpot(0.5f,0.5f);

	this->playEntity.SetPosition(x, y, interface_layer);

	x = this->quitEntity.GetPosX();
	y = this->quitEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->quitSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->quitSurface, "..\\res\\pause\\nquit.png", IND_ALPHA, IND_32)) return;
	this->quitEntity.SetSurface(&this->quitSurface);

	this->quitEntity.SetHotSpot(0.5f,0.5f);

	this->quitEntity.SetPosition(x, y, interface_layer);	
	
	this->active = true;
}
void pause::SetActiveQuit () {
	float x, y;

	x = this->playEntity.GetPosX();
	y = this->playEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->playSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->playSurface, "..\\res\\pause\\nresume.png", IND_ALPHA, IND_32)) return;
	this->playEntity.SetSurface(&this->playSurface);

	this->playEntity.SetHotSpot(0.5f,0.5f);

	this->playEntity.SetPosition(x, y, interface_layer);

	x = this->quitEntity.GetPosX();
	y = this->quitEntity.GetPosY();

	CIndieLib::Instance()->SurfaceManager->Delete(&this->quitSurface);
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->quitSurface, "..\\res\\pause\\aquit.png", IND_ALPHA, IND_32)) return;
	this->quitEntity.SetSurface(&this->quitSurface);

	this->quitEntity.SetHotSpot(0.5f,0.5f);

	this->quitEntity.SetPosition(x, y, interface_layer);

	this->active = false;
}
bool pause::PlayActive () {
	return active;
}
void pause::SetmainPosition (float x, float y) {
	this	->	main_pageEntity	.	SetHotSpot (0.5f,0.5f);
	this->main_pageEntity.SetPosition (x, y, interface_layer);
}

//-------------------------------game_over----------------------

unsigned int Game_over::load () {
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->main_pageSurface,"..\\res\\game_over\\main.png", IND_ALPHA, IND_32))	return 0;
	if (!CIndieLib::Instance()->SurfaceManager->Add(&this->playSurface, "..\\res\\game_over\\agame_over.png", IND_ALPHA, IND_32))	return 0;
	
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer,&this->main_pageEntity);
	CIndieLib::Instance()->Entity2dManager->Add(interface_layer,&this->playEntity);

	this	->	main_pageEntity	.	SetSurface (&this->main_pageSurface);
	this	->	playEntity		.	SetSurface (&this->playSurface);

	
	this	->	main_pageEntity	.	SetHotSpot (0.5f,0.5f);
	this	->	playEntity		.	SetHotSpot (0.5f,0.5f);

	return 1;
}

void Game_over::SetmainPosition (float x, float y) {
	this	->	main_pageEntity	.	SetHotSpot (0.5f,0.5f);
	this->main_pageEntity.SetPosition (x, y, interface_layer);
}

void Game_over::SetPlayPosition (float x, float y) {
	this->playEntity.SetPosition(x, y, interface_layer);
}
