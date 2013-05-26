#include "CIndieLib_vc2008.h"
#include "game_control_and_logic.h"
#include <SFML\Audio\Music.hpp>
#include <SFML\Audio.hpp>
#include "interface.h"
#include <vector>

using namespace game_control;
using namespace game_logic;

int IndieLib() {
	//--------------------Engine--------------------

	CIndieLib *engine = CIndieLib::Instance();
	if (!engine->Init()) return 1;

	float df =								0;

	const float no_angle =					360.0f;

	const char floors_to_go[] =				"\nFloors to go: ";

	//----------------Rendering timer---------------

	IND_Timer r_timer;

	const char FPS =						60;

	int last_frame =						0;
	int ticks =								0;

	//------------------Resources-------------------
	
	IND_Surface char_s;
	IND_Surface npc_s;
	IND_Surface pb_border;
	IND_Surface pb_sborder;
	IND_Surface pb_rfiller;
	IND_Surface pb_bfiller;

	IND_Animation aMove;
	IND_Animation aStop;
	IND_Animation Scream;

	if (!engine->AnimationManager->AddToSurface(&aMove,"..\\res\\animations\\hero\\MoveRightVariant_4\\Move.xml", IND_ALPHA, IND_32)) return 2;
	if (!engine->AnimationManager->AddToSurface(&Scream,"..\\res\\animations\\hero\\Scream\\Scream.xml", IND_ALPHA, IND_32)) return 2;
	if (!engine->AnimationManager->AddToSurface(&aStop, "..\\res\\animations\\hero\\Stop\\Stop.xml", IND_ALPHA, IND_32)) return 2;

	if (!engine->SurfaceManager->Add(&char_s, "..\\res\\character.png", IND_ALPHA, IND_32)) return 2;
	if (!engine->SurfaceManager->Add(&npc_s, "..\\res\\character_1.png", IND_ALPHA, IND_32)) return 2;
	if (!engine->SurfaceManager->Add(&pb_border, "..\\res\\progress_bar\\border.png", IND_ALPHA, IND_32)) return 2;
	if (!engine->SurfaceManager->Add(&pb_sborder, "..\\res\\progress_bar\\separated_border.png", IND_ALPHA, IND_32)) return 2;
	if (!engine->SurfaceManager->Add(&pb_rfiller, "..\\res\\progress_bar\\red_filler.png", IND_ALPHA, IND_32)) return 2;
	if (!engine->SurfaceManager->Add(&pb_bfiller, "..\\res\\progress_bar\\blue_filler.png", IND_ALPHA, IND_32)) return 2;

	IND_Font font;

	if (!engine->FontManager->Add(&font, "..\\res\\font_small.png", "..\\res\\font_small.xml", IND_ALPHA, IND_32)) return 2;

	//-----------------Level manager----------------

	level_manager lm(*engine);

	if (!lm.init()) {
		engine->End();
		return 3;
	}

	//-------------------Characters-----------------

	chmanager chm;

	chm.add_skin(npc_s);

	chm.get_hero().set_aMoveLeft_Right_Stay(aStop);
	chm.get_hero().set_animation(aMove, aStop);
	chm.get_hero().SetScreamAnimation(Scream);

	//----------------------NPCs--------------------
	/*
	chm.add_npc();

	engine->Entity2dManager->Add(2, &chm.get_npc(0).get_entity());
	engine->Entity2dManager->Add(3, &chm.get_npc(0).get_text_entity());

	chm.get_npc(0).set_surface(npc_s);
	chm.get_npc(0).set_font(font);

	chm.get_npc(0).add_phrase("Hi, dude!");

	if (lm.get_rooms_count()) {
		room *rand_room =					&lm.get_room((unsigned short) engine->Math->Rand(1, lm.get_rooms_count() - 2));
		IND_Entity2d *rand_floor =			rand_room->floor[(unsigned short) engine->Math->Rand(0, rand_room->floor.size() - 1)]->entity;
		chm.get_npc(0).move(vector2d(rand_floor->GetPosX(),
			rand_floor->GetPosY() - rand_floor->GetSurface()->GetHeight() / 2 - chm.get_npc(0).get_entity().GetSurface()->GetHeight() / 2 + 5));
		chm.get_info(1).current_room =		rand_room->ID;
		chm.get_info(1).direction.x =		1;
	}
	*/

	//--------------------Camera--------------------
	
	IND_Camera2d camera(0, 0);

	//-------------------Main loop------------------

	//r_timer.Start();
	//chm.time.Start();

	//------------------Background music-------------
	
	sf::Music music;
	sf::Music pandoraMusic;
	
	if (!pandoraMusic.openFromFile("..\\res\\Music\\background\\pandora.ogg")) return 3;
	if (!music.openFromFile("..\\res\\Music\\background\\back.ogg")) return 3;
	
	music.setVolume	(70);
	music.setLoop	(true);

	pandoraMusic.setVolume(100);
	pandoraMusic.setLoop(true);
	pandoraMusic.play();
	
	//-------------------Sound------------------------
	sSound scream;
	sSound footstep;

	if (!scream.SetSound("..\\res\\Music\\scream\\scream.ogg"))	return 3;
	if (!footstep.SetSound("..\\res\\Music\\step\\step.ogg"))	return 3;

	scream.SetVolume(100);
	footstep.SetVolume(100);

	chm.get_hero().	SetScreamSound	(&scream);
	chm.get_hero().	SetFootstepSound(&footstep);
	chm.get_hero().	GetFootstepSound()->SetLoop(true);
	chm.get_hero(). GetFootstepSound()->SetSpeed(2);

	//-------------ProgressBar------------------------
	ProgressBar fear_bar, scream_bar;

	fear_bar.SetBorder(pb_border);
	fear_bar.SetFiller(pb_rfiller);
	fear_bar.SetMaxValue(characters::main_hero_max_fear);

	scream_bar.SetBorder(pb_sborder);
	scream_bar.SetFiller(pb_bfiller);
	scream_bar.SetMaxValue(characters::main_hero_scream_limit * characters::main_hero_scream_energy);

	//--------------MessageBox------------------------
	texture_set set;

	set.set_texture_dir(PathToMessageBoxImage);
	if (!set.load()) return 3;

	Message_Box messageBox(camera);

	messageBox.SetTexture_set(set);

	//------------------------------------------------

	//-----------------Main menu----------------------

	main_page MainPage;

	if (!MainPage.load()) return 3;

	MainPage.SetEyesPosition(-80,-100);
	MainPage.SetPlayPosition(300,150);
	MainPage.SetQuitPosition(300,200);

	//-------------------------------------------------
	//gstate = show_menu;
	//------------------Pause-------------------------
	pause Pause;

	if (!Pause.load()) return 3;

	Pause.SetPlayPosition(-100,-100);
	Pause.SetQuitPosition(-100,-50);
	Pause.SetmainPosition(0,0);
	Pause.Show(false);
	//------------------------------------------------
	//-------------------Game over-------------------
	Game_over Game_Over;
	IND_Timer timer;
	if (!Game_Over.load()) return 3;

	Game_Over.Show(false);
	//-----------------------------------------------
	while (!engine->Input->Quit()) {

		ticks =								r_timer.GetTicks();

		// Input update
		engine->Input->Update();

		switch (gstate) {
		case show_menu:
			main_page_interface(MainPage);
			fear_bar.Show(false);
			scream_bar.Show(false);
			if (gstate == change_level) music.play ();
			break;

		case play_game:
			Game_Over.Show(false);
			Pause.Show(false);
			if (CIndieLib::Instance()->Input->OnKeyPress(IND_ESCAPE)){
				gstate = pause_game;
				pandoraMusic.stop();
			}

			play(lm, chm, camera);
			fear_bar.SetPosition((float) (camera.GetPosX() +40 - engine->Window->GetWidth() / 2), (float) (camera.GetPosY() +10 - engine->Window->GetHeight() / 2));
			fear_bar.Show(true);
			fear_bar.SetValue(chm.get_hero().get_fear());

			scream_bar.SetPosition((float) (camera.GetPosX() + engine->Window->GetWidth() / 2 - 240), (float) (camera.GetPosY() - engine->Window->GetHeight() / 2 + 10));
			scream_bar.Show(true);
			scream_bar.SetValue(chm.get_hero().get_scream_energy());

			break;

		case change_level:
			complete_level(lm);
			if (gstate == quit) break;
			load_level(lm, chm);
			update_camera(camera, IND_Point((int) chm.get_hero().get_position().x, (int) chm.get_hero().get_position().y),
				lm.get_tl_point(), lm.get_br_point(), engine->Window->GetWidth(), engine->Window->GetHeight());
			gstate =						play_game;
			break;
		case pause_game:
			Pause.Show(true);
			Pause.SetPlayPosition((float) (camera.GetPosX() +400 - engine->Window->GetWidth() / 2), (float) (camera.GetPosY() +400 - engine->Window->GetHeight() / 2));
			Pause.SetQuitPosition((float) (camera.GetPosX() +400 - engine->Window->GetWidth() / 2), (float) (camera.GetPosY() +450 - engine->Window->GetHeight() / 2));
			Pause.SetmainPosition ((float)camera.GetPosX(),(float)camera.GetPosY());
			fear_bar.Show(false);
			scream_bar.Show(false);
			pause_screen(Pause);
			if (gstate == play_game)
				pandoraMusic.play();
			break;
		case _game_over:
			fear_bar.Show(false);
			scream_bar.Show(false);
			Game_Over.Show(true);
			Game_Over.SetPlayPosition((float) (camera.GetPosX() +400 - engine->Window->GetWidth() / 2), (float) (camera.GetPosY() +200 - engine->Window->GetHeight() / 2));
			Game_Over.SetmainPosition((float)camera.GetPosX(),(float)camera.GetPosY());

			if (engine->Input->OnKeyPress(IND_RETURN)) gstate = change_level;

			break;
		case quit:
			engine->End();
			return 0;
			break;
		}
		
		

		// Viewport & camera
		engine->Render->ClearViewPort(0, 0, 0);
		engine->Render->SetViewPort2d(0, 0, engine->Window->GetWidth(), engine->Window->GetHeight());
		engine->Render->SetCamera2d(&camera);
		
		// Rendering scene
		//if (ticks > last_frame + 1000 / FPS) {
			engine->Render->BeginScene();

			if (gstate == play_game || gstate == pause_game || gstate == _game_over) {
			engine->Entity2dManager->RenderEntities2d(background_layer);
			engine->Entity2dManager->RenderEntities2d(objects_layer);
			engine->Entity2dManager->RenderEntities2d(items_layer);
			engine->Entity2dManager->RenderEntities2d(mobs_layer);
			engine->Entity2dManager->RenderEntities2d(hero_layer);
			}
			engine->Entity2dManager->RenderEntities2d(interface_layer);

			if (game_control::show_bareas) {
				engine->Entity2dManager->RenderCollisionAreas(objects_layer, 255, 0, 0, 255);
				engine->Entity2dManager->RenderCollisionAreas(items_layer, 0, 0, 255, 255);
				engine->Entity2dManager->RenderCollisionAreas(hero_layer, 0, 255, 0, 255);
			}

			engine->Render->EndScene();

			// Frame info
			//last_frame =					ticks;
			df =							engine->Render->GetFrameTime() / 1000.0f;
			engine->Render->ShowFpsInWindowTitle();

			
		//}

	}

	//---------------------End----------------------

	//r_timer.Stop();
	//chm.time.Stop();
	engine->End();

	return 0;
}