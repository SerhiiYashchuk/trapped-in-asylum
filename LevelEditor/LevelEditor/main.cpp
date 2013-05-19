#include <vector>
#include "CIndieLib_vc2008.h"
#include "tiles_manager.h"
#include "cursor.h"
#include "interface.h"
#include "objects_configuration.h"

//---------------update input---------------

void cursor_update(CIndieLib &engine, editing_cursor *cursor, float &dx, float &dy);
void tcursor_update(CIndieLib &engine, tile_cursor &tcursor);

void camera_update(CIndieLib &engine, IND_Camera2d &cam, IND_Camera2d &bcam, float &dx, float &dy, float &cam_x, float &cam_y, float dm);

void menu_update(CIndieLib &engine, char *m_str);

//---------------Exceptions---------------

class res_error {};
class tm_init_fail {};
class cursor_init_fail {};

//--------------------command line---------------------

bool perform_command(command &cmd, lt_manager &ltm, conf_manager &cm);

//--------------------main function--------------------

int IndieLib() {
	//-------------------Constants------------------
	enum emode {menu, create, config};

	const char FPS =								60;

	const float dcam =								0.05f;

	const char prompt[] =							">\t";
	
	//----------------Logic variables---------------

	bool show_b_areas =								true;

	emode mode =									create;
	emode prev_mode =								create;

	float dm =										0;

	//--------------------Engine--------------------

	CIndieLib *prog = CIndieLib::Instance();
	if (!prog->Init()) return 1;
	prog->Window->SetTitle("Level Editor 1.1-alpha");
	//prog->Window->Cursor(true);

	// Render timer

	IND_Timer render_timer;
	int ticks =										0;
	int last_frame =								0;
	render_timer.Start();

	// Camera

	float cam_x =									(float) prog->Window->GetWidth() / 2;
	float cam_y =									(float) prog->Window->GetHeight() / 2;
	IND_Camera2d cam((int) cam_x, (int) cam_y);
	IND_Camera2d bcam((int) cam_x, (int) cam_y);
	
	//-------------------Resources------------------

	IND_Font t_font;
	IND_Entity2d menu_e;

	try {
		if (!prog->FontManager->Add(&t_font, "..\\res\\font_small.png", "..\\res\\font_small.xml", IND_ALPHA, IND_32))
			throw res_error();
		if (!prog->Entity2dManager->Add(4, &menu_e)) throw res_error();
	}
	catch (res_error) {
		prog->End();
		return 2;
	}

	//-----------Menu and additional info-----------

	cmd_line cmd_menu;
	
	char cmd_str[520];
	char buf[10];

	strcpy(cmd_str, prompt);

	menu_e.SetFont(&t_font);
	menu_e.SetAlign(IND_LEFT);
	menu_e.SetCharSpacing(-8);
	menu_e.SetLineSpacing(18);
	menu_e.SetText(cmd_str);
	menu_e.SetPosition(5, (float) (prog->Window->GetHeight() - 25), 0);
	menu_e.SetShow(false);

	// Information
	
	info tile_info;

	prog->Entity2dManager->Add(4, &tile_info.get_entity());
	tile_info.set_font(t_font);

	info lvl_info;
	bool update_info =								true;

	prog->Entity2dManager->Add(4, &lvl_info.get_entity());
	lvl_info.set_font(t_font);
	lvl_info.set_position(prog->Render->GetViewPortX() + 5, prog->Render->GetViewPortY() + 5, 0);
	lvl_info.add_text(unnamed_lt_file);

	//------------------Tiles manager---------------

	lt_manager ltm;
	try {
		//if (!ltm.load_textures("..\\textures")) throw tm_init_fail();
		if (!ltm.load_textures("E:\\Documents\\Projects\\C++\\IndieLib\\PlatformerTest\\textures")) throw tm_init_fail();
	}
	catch (tm_init_fail) {
		prog->End();
		return 3;
	}

	//-------------Cofiguration manager-------------

	conf_manager conf_m;
	
	//-----------------Tile cursor------------------

	tile_cursor tcursor;

	float cur_dx = 0;
	float cur_dy = 0;

	try {
		if (!tcursor.init(*prog)) throw cursor_init_fail();
	}
	catch (cursor_init_fail) {
		prog->End();
		return 3;
	}
	tcursor.set_textures(ltm.get_textures());
	tcursor.set_tiles(ltm.get_tiles(0));

	//-----------------Config cursor----------------

	config_cursor ccursor(conf_m);

	try {
		if (!ccursor.init(*prog)) throw cursor_init_fail();
	}
	catch (cursor_init_fail) {
		prog->End();
		return 3;
	}
	ccursor.hide();

	editing_cursor *cursor;
	if (mode == create) cursor = &tcursor;

	lvl_info.add_text("\nTile set: ");
	lvl_info.add_text(tcursor.cur_tiles().get_name());
	
	//-------------------Main loop------------------

	while (!prog->Input->OnKeyPress(IND_ESCAPE) && !prog->Input->Quit()) {
		// Input update
		prog->Input->Update();

		// Count time for next rendering
		ticks = render_timer.GetTicks();
		
		// ~ button press (show command line)
		if (prog->Input->OnKeyPress(IND_BACKQUOTE) && !tcursor.active_tile()) {
			if (!menu_e.IsShow()) {
				prev_mode = mode;
				mode = menu;
			} else mode = prev_mode;

			menu_e.SetPosition((float) (cam_x - prog->Window->GetWidth() / 2 + 5),
				(float) (cam_y + prog->Window->GetHeight() / 2 - 25), 0);
			menu_e.SetShow(!menu_e.IsShow());
		}

		// Changing level info position
		lvl_info.set_position((int) cam_x - prog->Window->GetWidth() / 2 + 5,
			(int) cam_y - prog->Window->GetHeight() / 2 + 5, 0);
		
		// Changing active tile info position
		tile_info.set_position((int) cam_x + prog->Window->GetWidth() / 2 - 100,
			(int) cam_y - prog->Window->GetHeight() / 2 + 5, 0);

		//---------------Updating input due to active mode---------------
		
		switch (mode) {
		case menu: {
			// Menu input update
			char ch = '\0';

			for (char key = 97; key <= 122; key++)
				if (prog->Input->OnKeyPress(key - 97)) ch =			key;
			for (char key = 48; key <= 57; key++)
				if (prog->Input->OnKeyPress(key - 22)) ch =			key;
			if (prog->Input->OnKeyPress(IND_SPACE)) ch =			' ';
			if (prog->Input->OnKeyPress(IND_MINUS)) ch =			'-';
			//if (prog->Input->OnKeyPress(IND_AT)) ch =				':';
			//if (prog->Input->OnKeyPress(IND_CARET)) ch =			'_';
			if (prog->Input->OnKeyPress(IND_BACKSLASH)) ch =		'\\';
			if (prog->Input->OnKeyPress(IND_PERIOD)) ch =			'.';

			if (prog->Input->OnKeyPress(IND_BACKSPACE) || prog->Input->IsKeyPressed(IND_BACKSPACE, 100)) {
				cmd_menu.pop_back();
				strcpy(cmd_str, prompt);
				strcat(cmd_str, cmd_menu.get_string());
			}

			if (ch) {
				cmd_menu.push_back(ch);
				strcpy(cmd_str, prompt);
				strcat(cmd_str, cmd_menu.get_string());
			}

			// Performing command
			if (prog->Input->OnKeyPress(IND_RETURN)) {
				if (perform_command(cmd_menu.parse(), ltm, conf_m)) {
					strcpy(cmd_str, prompt);

					menu_e.SetShow(false);
					mode =											prev_mode;
					update_info =									true;

				} else {
					strcpy(cmd_str, prompt);
					strcat(cmd_str, "bad command");
				}
				cmd_menu.clear();
			}
				   } break;

		case create: {
			// Cursor update
			cursor_update(*prog, cursor, cur_dx, cur_dy);
			tcursor_update(*prog, tcursor);

			// 1 button press (set ground as current tile set)
			if (prog->Input->OnKeyPress(IND_1) && !tcursor.active_tile()) {
				tcursor.set_tiles(ltm.get_tiles(0));
				update_info =										true;
			}

			// 2 button press (set terrain as current tile set)
			if (prog->Input->OnKeyPress(IND_2) && !tcursor.active_tile()) {
				tcursor.set_tiles(ltm.get_tiles(1));
				update_info =										true;
			}

			// 3 button press (set items as current tile set)
			if (prog->Input->OnKeyPress(IND_3) && !tcursor.active_tile()) {
				tcursor.set_tiles(ltm.get_tiles(2));
				update_info =										true;
			}

					 } break;

		case config: {
			cursor_update(*prog, cursor, cur_dx, cur_dy);
					 } break;
		}

		// Non menu actions
		if (mode != menu) {
			
			// Tab button press (show/hide bounding areas OR switch working mode)
			if (prog->Input->OnKeyPress(IND_TAB))
				if (prog->Input->IsKeyPressed(IND_LCTRL)) {
					switch (mode) {
					case create:
						if (*ltm.get_lt_file() && !cursor->active_tile()) {
							mode =									config;
							cursor =								&ccursor;
							tcursor.hide();

							if (conf_m.get_doors_count())
								conf_m.update();
							else conf_m.create(ltm.get_textures(), ltm.get_tiles(1));
						}
						break;

					case config:
						mode =										create;
						cursor =									&tcursor;
						ccursor.hide();

						break;
					}
					cursor->show();
					update_info =									true;

				} else show_b_areas = !show_b_areas;

			// Camera update
			camera_update(*prog, cam, bcam, cur_dx, cur_dy, cam_x, cam_y, dm * dcam);
		}

		// Editor info
		if (update_info) {
			lvl_info.set_text("Mode: ");
			if (mode == create)
				lvl_info.add_text("create");
			else lvl_info.add_text("config");
			lvl_info.add_text("\nFile: ");
			if (*ltm.get_lt_file())
				lvl_info.add_text(ltm.get_lt_file());
			else lvl_info.add_text(unnamed_lt_file);
			lvl_info.add_text("\nTile set: ");
			lvl_info.add_text(tcursor.cur_tiles().get_name());

			update_info =											false;
		}

		// Tile info

		if (tcursor.active_tile()) {
			tile_info.set_text("Z pos: ");
			tile_info.add_text(itoa(tcursor.active_tile()->GetPosZ(), buf, 10));
			tile_info.add_text("\nZ angle: ");
			tile_info.add_text(itoa((int) tcursor.active_tile()->GetAngleZ(), buf, 10));
			tile_info.add_text("\nScale: ");
			tile_info.add_text(itoa((int) tcursor.active_tile()->GetScaleX(), buf, 10));
			tile_info.add_text(".");

			if ((int) (tcursor.active_tile()->GetScaleX() * 10) % 10 == 0)
				tile_info.add_text("0");
			if (tcursor.active_tile()->GetScaleX() * 100 < 100)
				tile_info.add_text(itoa((int) (tcursor.active_tile()->GetScaleX() * 100), buf, 10));
			else tile_info.add_text(itoa((int) (tcursor.active_tile()->GetScaleX() * 100) % 100, buf, 10));
		} else {
			tile_info.set_text("");
		}

		// Rendering

		if (ticks > last_frame + 1000/FPS) {
			
			// Viewport & camera
			prog->Render->ClearViewPort(0, 0, 0);
			prog->Render->SetViewPort2d(0, 0, prog->Window->GetWidth(), prog->Window->GetHeight());
			prog->Render->SetCamera2d(&cam);

			// Scene
			prog->Render->BeginScene();
			
			prog->Entity2dManager->RenderEntities2d(background_layer);
			prog->Entity2dManager->RenderEntities2d(objects_layer);
			prog->Entity2dManager->RenderEntities2d(items_layer);
			if (show_b_areas) {
				prog->Entity2dManager->RenderCollisionAreas(objects_layer, 255, 0, 0, 255);
				prog->Entity2dManager->RenderCollisionAreas(items_layer, 0, 0, 255, 255);
			}
			
			prog->Render->SetCamera2d(&bcam);
			
			prog->Entity2dManager->RenderEntities2d(4);
			prog->Entity2dManager->RenderEntities2d(5);
			prog->Entity2dManager->RenderCollisionAreas(5, 255, 0, 0, 255);
			if (mode == config)
				conf_m.render_links(*prog->Render);
			
			prog->Render->EndScene();

			dm = prog->Render->GetFrameTime() / 1000.0f;
			last_frame = ticks;
		}
	}

	// Finishing

	render_timer.Stop();
	prog->End();

	return 0;
}

//--------------------command line--------------------

bool perform_command(command &cmd, lt_manager &ltm, conf_manager &cm) {
	switch (cmd.type) {
	// New level
	case cmdt_new:
		ltm.clear();
		cm.clear();
		return 1;

	case cmdt_save:
		return (ltm.save((cmd.parameter + "\\tiles.xml").c_str()) &&
			cm.save((cmd.parameter + "\\config.xml").c_str()));

	case cmdt_load:
		if (ltm.load((cmd.parameter + "\\tiles.xml").c_str()))
			return (cm.create(ltm.get_textures(), ltm.get_tiles(1)) && cm.load((cmd.parameter + "\\config.xml").c_str()));
		else return 0;

	case cmdt_error:

	default:
		break;
	}
	return 0;
}

//----------------cursor update---------------

void cursor_update(CIndieLib &engine, editing_cursor *cursor, float &dx, float &dy) {
	// Moving cursor
	cursor->move(engine.Input->GetMouseX() + (int) dx, engine.Input->GetMouseY() + (int) dy);

	// Space button press
	if (engine.Input->OnKeyPress(IND_SPACE) && !cursor->active_tile())
		cursor->add();
	
	// Delete button press
	if (engine.Input->OnKeyPress(IND_DELETE))
		cursor->del();
	
	// Mouse left button click
	if (engine.Input->OnMouseButtonPress(IND_MBUTTON_LEFT) && !engine.Input->IsKeyPressed(IND_LSHIFT))
		cursor->check_click();
}

//---------------tcursor update---------------

void tcursor_update(CIndieLib &engine, tile_cursor &tcursor) {
	// Left shift is pressed (rotating and scaling)
	if (engine.Input->IsKeyPressed(IND_LSHIFT) && tcursor.active_tile()) {
		// Rotating counterclockwise
		if (engine.Input->IsMouseButtonPressed(IND_MBUTTON_LEFT))
			tcursor.rotate_left();
		// Rotating clockwise
		if (engine.Input->IsMouseButtonPressed(IND_MBUTTON_RIGHT))
			tcursor.rotate_right();
		// Return to basic position
		if (engine.Input->OnMouseButtonPress(IND_MBUTTON_MIDDLE))
			tcursor.set_basic_rotation();
		// Scaling out
		if (engine.Input->OnMouseButtonPress(IND_MBUTTON_WHEELUP))
			tcursor.scale_out();
		// Scaling in
		if (engine.Input->OnMouseButtonPress(IND_MBUTTON_WHEELDOWN))
			tcursor.scale_in();
	}

	// Mouse wheel up (get previous texture)
	if (engine.Input->OnMouseButtonPress(IND_MBUTTON_WHEELUP)
		&& !engine.Input->IsKeyPressed(IND_LSHIFT)
		&& !engine.Input->IsKeyPressed(IND_LCTRL))
			tcursor.prev_texture();
	
	// Mouse wheel down (get next texture)
	if (engine.Input->OnMouseButtonPress(IND_MBUTTON_WHEELDOWN)
		&& !engine.Input->IsKeyPressed(IND_LSHIFT)
		&& !engine.Input->IsKeyPressed(IND_LCTRL))
			tcursor.next_texture();

	// Increase Z position
	if (engine.Input->OnKeyPress(IND_KEYDOWN))
		tcursor.set_zpos(tcursor.tile_zpos() - 1);
	
	// Decrease Z position
	if (engine.Input->OnKeyPress(IND_KEYUP))
		tcursor.set_zpos(tcursor.tile_zpos() + 1);
}

//---------------camera update---------------

void camera_update(CIndieLib &engine, IND_Camera2d &cam, IND_Camera2d &bcam, float &dx, float &dy, float &cam_x, float &cam_y, float d) {
	

	// Move up
	if (engine.Input->IsKeyPressed(IND_W)) {
		cam_y -=		d;
		dy -=			d;
	}

	// Move down
	if (engine.Input->IsKeyPressed(IND_S)) {
		cam_y +=		d;
		dy +=			d;
	}

	// Move right
	if (engine.Input->IsKeyPressed(IND_D)) {
		cam_x +=		d;
		dx +=			d;
	}

	// Move left
	if (engine.Input->IsKeyPressed(IND_A)) {
		cam_x -=		d;
		dx -=			d;
	}

	// Zoom in
	if (engine.Input->OnKeyPress(IND_E) && cam.GetZoom() < 1)
		cam.SetZoom(cam.GetZoom() + 0.05f);

	// Zoom out
	if (engine.Input->OnKeyPress(IND_Q) && cam.GetZoom() > 0.35)
		cam.SetZoom(cam.GetZoom() - 0.05f);

	cam.SetPosition((int) cam_x, (int) cam_y);
	bcam.SetPosition((int) cam_x, (int) cam_y);
}