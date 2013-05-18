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
	IND_Animation aMove;
	IND_Animation aStop;
	IND_Animation Scream;

	if (!engine->AnimationManager->AddToSurface(&aMove,"..\\res\\animations\\hero\\MoveRightVariant_4\\Move.xml", IND_ALPHA, IND_32)) return 2;
	if (!engine->AnimationManager->AddToSurface(&Scream,"..\\res\\animations\\hero\\Scream\\Scream.xml", IND_ALPHA, IND_32)) return 2;
	if (!engine->AnimationManager->AddToSurface(&aStop, "..\\res\\animations\\hero\\Stop\\Stop.xml", IND_ALPHA, IND_32)) return 2;
	if (!engine->SurfaceManager->Add(&char_s, "..\\res\\character.png", IND_ALPHA, IND_32)) return 2;
	if (!engine->SurfaceManager->Add(&npc_s, "..\\res\\character_1.png", IND_ALPHA, IND_32)) return 2;

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

	//----------------------Info--------------------

	info hero_info;

	hero_info.set_font(font, 255, 0, 0);
	hero_info.set_font_params();
	hero_info.set_text("Fear: ");
	hero_info.add_text((int) chm.get_hero().get_fear());
	hero_info.set_position(lm.get_tl_point().x + level_border, lm.get_tl_point().y + level_border, 0);

	//--------------------Camera--------------------
	
	IND_Camera2d camera(0, 0);

	// Start game

	gstate =								change_level;

	//-------------------Main loop------------------

	//r_timer.Start();
	//chm.time.Start();

	//------------------Background music-------------
	
	sf::Music music;
	sf::Music pandoraMusic;
	
	if (!pandoraMusic.openFromFile("..\\res\\Music\\background\\pandora.ogg")) return 3;
	if (!music.openFromFile("..\\res\\Music\\background\\back.ogg")) return 3;
	
	music.setVolume	(70);
	music.play		();
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
	
	//----------

	while (!engine->Input->Quit() && !engine->Input->OnKeyPress(IND_ESCAPE)) {

		ticks =								r_timer.GetTicks();

		// Input update
		engine->Input->Update();

		switch (gstate) {
		case show_menu:

			break;

		case play_game:
			play(lm, chm, camera);
			update_ingame_interface(camera.GetPosX() - engine->Window->GetWidth() / 2, camera.GetPosY() - engine->Window->GetHeight() / 2, chm.get_hero(), hero_info);
			break;
		case change_level:
			complete_level(lm);
			if (gstate == quit) break;
			load_level(lm, chm);
			update_camera(camera, IND_Point((int) chm.get_hero().get_position().x, (int) chm.get_hero().get_position().y),
				lm.get_tl_point(), lm.get_br_point(), engine->Window->GetWidth(), engine->Window->GetHeight());
			gstate =						play_game;
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

			engine->Entity2dManager->RenderEntities2d(background_layer);
			engine->Entity2dManager->RenderEntities2d(objects_layer);
			engine->Entity2dManager->RenderEntities2d(items_layer);
			engine->Entity2dManager->RenderEntities2d(3);
			engine->Entity2dManager->RenderEntities2d(interface_layer);

			if (game_control::show_bareas) {
				engine->Entity2dManager->RenderCollisionAreas(objects_layer, 255, 0, 0, 255);
				engine->Entity2dManager->RenderCollisionAreas(2, 0, 255, 0, 255);
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