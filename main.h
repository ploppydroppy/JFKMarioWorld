#pragma once

void player_code()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError(); return;
	}


	//Initialize ZSNES UI
	load_zsnes_font();
	zsnes_ui.add_button("SINGLEPLAYER", 4, 2, 82, 13);
	zsnes_ui.add_button("MULTIPLAYER", 84, 2, 84 + 82, 13);
	zsnes_ui.add_button("RELOAD", 84 + 84, 2, 84 + 84 + 48, 13);
	zsnes_ui.add_button("X",  256-13, 2, 256-2, 13);

	//Load SDL part
	screen(resolution_x, resolution_y);
	init_input();
	init_audio();
	Mix_HaltMusic();
	string file = path + "Sounds/music/_boot.ogg"; music = Mix_LoadMUS(file.c_str()); Mix_PlayMusic(music, 1);

	string level = "";
	global_frame_counter = 0;

	zsnes_ui.message = "First boot";

	while (true)
	{
		LevelSprites.clear();
		zsnes_ui.hint = "";
		disconnected = false;
		PlayerAmount = 0; SelfPlayerNumber = 1; CheckForPlayers();
		quit = false;

		/* Options Loop */
		string s_or_c;
		while (true)
		{
			
			quit = false;
			if (done(true))
			{
				return;
			}
			global_frame_counter += 1;

			cls();

			if (zsnes_ui.hint != "")
			{
				if (zsnes_ui.button_pressed == "SINGLEPLAYER" || state[SDL_SCANCODE_Q]) {
					s_or_c = "t";
					level = zsnes_ui.hint;
					zsnes_ui.message = "Loading level " + level + "..";
					midway_activated = false;
				}
				if (zsnes_ui.button_pressed == "MULTIPLAYER" || state[SDL_SCANCODE_W]) {
#if not defined(DISABLE_NETWORK)
					ip = zsnes_ui.hint; PORT = 25500;
					zsnes_ui.message = "Connecting to " + ip + ":" + to_string(PORT);
					s_or_c = "c";
					midway_activated = false;
#else
					zsnes_ui.message = "MP Not supported";
#endif
				}
			}
			else
			{
				if (zsnes_ui.button_pressed != "none" || ((state[SDL_SCANCODE_Q] || state[SDL_SCANCODE_R]) || state[SDL_SCANCODE_W]))
				{
					zsnes_ui.message = "bruh type something";
				}
			}

			if ((zsnes_ui.button_pressed == "RELOAD" || state[SDL_SCANCODE_R]) || SDL_GameControllerGetButton(gGameController, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
			{
				if (level != "")
				{
					zsnes_ui.message = "Reloading";
					s_or_c = "t";
				}
			}


			zsnes_ui.process();
			zsnes_ui.finish_processing(ren);


			//we copy it to the renderer, for your program, if you want to have a ingame thing (variable or w/e), you just simply don't do this and render what your game has instead.
			SDL_Rect DestR;
			DestR.x = sp_offset_x;
			DestR.y = sp_offset_y;
			DestR.w = int_res_x * scale;
			DestR.h = int_res_y * scale;

			//Copied from renderer.h
			for (int x = 0; x < 2; x++)
			{
				for (int y = 0; y < 2; y++)
				{
					RenderBackground(
						(-int(double(CameraX) * (double(ServerRAM.RAM[0x3F06]) / 16.0)) % 512) + x * 512,
						-272 + (int_res_y - 224) +(int(double(CameraY) * (double(ServerRAM.RAM[0x3F07]) / 16.0)) % 512) + y * -512);
				}
			}
			//Copied from renderer.h
			SDL_RenderCopy(ren, screen_t_l1, nullptr, &DestR);

			DestR.x = sp_offset_x + ((int_res_x - 256) * scale) / 2;
			DestR.y = sp_offset_y + ((int_res_y - 224) * scale) / 2;
			DestR.w = 256 * scale;
			DestR.h = 224 * scale;

			SDL_RenderCopy(ren, screen_t_l2, nullptr, &DestR);
			SDL_RenderCopy(ren, zsnes_ui.texture, NULL, &DestR);

			redraw87();
			check_input();

			if (zsnes_ui.button_pressed == "X")
			{
				return;
			}

			if (s_or_c != "")
			{
				break;
			}
		}
		global_frame_counter = 0;
		/* Load Shit */

		networking = s_or_c != "t";
		initialize_map16();

		if (s_or_c == "c")
		{
			isClient = true;

#if not defined(DISABLE_NETWORK)

			if (!ConnectClient()) {
				cout << red << "[Network] Failed to connect. Falling back to normal mode." << white << endl;
				networking = false;
				isClient = false;
				disconnected = false;
				zsnes_ui.message = "Failed to connect";
				continue;
			}
#else
			cout << "Multiplayer is not supported in this build!" << endl;
			networking = false;
			isClient = false;
			s_or_c = "t";
			zsnes_ui.message = "Not supported";
#endif
		}
		
		if (s_or_c == "t")
		{
			isClient = false;
			if (level == "")
			{
				cout << "Enter a level : "; cin >> level;
			}
			LevelManager.LoadLevel(stoi(level, nullptr, 16));
		}

		if (!isClient)
		{
			game_init();
		}

		//Initialize Singleplayer
		if (s_or_c == "t") {
			PlayerAmount = 1; SelfPlayerNumber = 1; CheckForPlayers();
		}

#if not defined(DISABLE_NETWORK)
		//Initialize Multiplayer Client
		if (networking)
		{
			thread = new sf::Thread(&NetWorkLoop); thread->launch();
		}
#endif

		cout << yellow << "[JFKMW] Waiting for player..." << white << endl;
		while (Mario.size() == 0) {
			Sleep(16);
		}
		
#if defined(EXPERIMENTAL)
		Launch3D();
#endif
		while (!done(true))
		{
			while (doing_read) {
				Sleep(1);
			}

			zsnes_ui.message = "Idle";

			doing_write = true;
			chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
			check_input(); game_loop(); SoundLoop();

			render();
			chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
#if defined(EXPERIMENTAL)
			DrawExperimental3D();
#endif
			redraw(); cls();
			
			total_time_ticks = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
			int fps = int(1.0 / (total_time_ticks.count() / 1.0));
			if (force_sleep)
			{
				while (fps > 75)
				{
					Sleep(1);
					t2 = chrono::high_resolution_clock::now();
					fps = int(1.0 / ((chrono::duration_cast<chrono::duration<double>>(t2 - t1)).count() / 1.0));
				}
			}
			doing_write = false;

			if (disconnected) {
				quit = true; cout << red << "[Network] Disconnected." << white << endl; zsnes_ui.message = "Disconnected from server"; break;
			}
		}

		//We quit the game go back to the ZSNES ui

#if not defined(DISABLE_NETWORK)
		if (networking) { socketG.disconnect();  thread->terminate(); }
#endif
		cout << yellow << "[JFKMW] Returning to main screen.." << white << endl;
		quit = true;
	}
}