#include "sprite.hpp"
#include "openai-helper.hpp"
#include "raylib-cpp.hpp"
#include <string>
#include <optional>
#include <vector>
#include <numeric> // std::iota
#include <iostream>
#include <algorithm>

void update_prompt(std::string& prompt, char c, const int font_size,
                   const float max_text_width, int& tail_index_large,
                   int& tail_index_small, int& nchars_entered);

float randomFloat(float min, float max)    
{    
    return (min + 1) + (((float) rand()) / (float) RAND_MAX) * (max - (min + 1));    
}

int main(int argc, char *argv[])
{
  using namespace aipfg;

  openai_helper oai_help;
  if (!oai_help.init())
  {
    return -1;
  }

  //Initial x and y values for the diamond.
  float d_gem_x = randomFloat(100.0f, 900.0f);
  float d_gem_y = randomFloat(100.0f, 500.0f);

  //Initial x and y values for the emerald.
  float e_gem_x = randomFloat(150.0f, 950.0f);
  float e_gem_y = randomFloat(150.0f, 550.0f);
  //Initial x and y values for the garnet.
  float g_gem_x = randomFloat(200.0f, 1000.0f);
  float g_gem_y = randomFloat(200.0f, 600.0f);

  

  //variable to track the number of gems collected
  int gems_collected = 0;
  int diamond_collected = 0;
  int emerald_collected = 0;
  int garnet_collected = 0;

  //sets the window size of the game
  raylib::Window window(1200, 570, "Raylib OpenAI NPCs");

  Camera2D camera = { 0 };
  Vector2 grey_posn{ 40.0f, 100.0f };

  camera.target = Vector2{ grey_posn.x + 20.0f, grey_posn.y + 20.0f };
  camera.offset = Vector2{ 1280 / 2.0f, 720 / 2.0f };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  SetTargetFPS(60);            // Set our game to run at 60 frames-per-second

  raylib::AudioDevice audio{}; // necessary: initialises the audio
  raylib::Sound coin_sound{ "../resources/audio/coin.wav" };
  raylib::Music music{ "../resources/audio/Magic-Clock-Shop.mp3" };
  float music_volume_normal = 1.0f, music_volume_quiet = 0.4f;
  music.Play();

  raylib::Texture tex1{ "../resources/time_fantasy/reaper_blade_3.png" };
  Sprite reaper{ tex1, 3, 4, { 340, 192 }, { 0 } };

  //Sprite for diamond.
  raylib::Texture diamond_tex{ "../resources/time_fantasy/diamond.png" };
  int d_cols = 6, d_rows = 1;
  int d_id = 1;
  Vector2 d_gem_posn{ d_gem_x, d_gem_y };
  //Sprite dimond_gem{ diamond_tex, d_cols, d_rows, d_gem_posn, { d_id }, 1 };
  std::vector<int> frame_id_diamond(d_cols * d_rows);
  std::iota(frame_id_diamond.begin(), frame_id_diamond.end(), 0);
  Sprite dimond_gem{ diamond_tex, d_cols, d_rows, d_gem_posn, frame_id_diamond, 7 };
  dimond_gem.set_animation(true);

  //Vector2 position = { 350.0f, 280.0f };
  //Rectangle frameRec = { 0.0f, 0.0f, (float)diamond_tex.width / 6, (float)diamond_tex.height };

  //Sprite for emerald.
  raylib::Texture emerald_tex{ "../resources/time_fantasy/emerald.png" };
  int e_cols = 6, e_rows = 1;
  //int e_id = 5;
  Vector2 e_gem_posn{ e_gem_x, e_gem_y };
  //Sprite emerald_gem{ emerald_tex, e_cols, e_rows, e_gem_posn, { e_id }, 1 };
  std::vector<int> frame_id_emerald(e_cols * e_rows);
  std::iota(frame_id_emerald.begin(), frame_id_emerald.end(), 0);
  Sprite emerald_gem{ emerald_tex, e_cols, e_rows, e_gem_posn, frame_id_emerald, 7 };
  emerald_gem.set_animation(true);

  //Sprite for garnet.
  raylib::Texture garnet_tex{ "../resources/time_fantasy/garnet.png" };
  int g_cols = 6, g_rows = 1;
  //int g_id = 5;
  Vector2 g_gem_posn{ g_gem_x, g_gem_y };
  //Sprite emerald_gem{ garnet_tex, e_cols, e_rows, e_gem_posn, { e_id }, 1 };
  std::vector<int> frame_id_garnet(g_cols * g_rows);
  std::iota(frame_id_garnet.begin(), frame_id_garnet.end(), 0);
  Sprite garnet_gem{ garnet_tex, g_cols, g_rows, g_gem_posn, frame_id_garnet, 7 };
  garnet_gem.set_animation(true);

 
  //loads the texture sheet and setup for the sprites the Knight uses
  raylib::Texture tex2{ "../resources/time_fantasy/knights_3x.png" };
  int ncols = 12, nrows = 8;
  int id = 3;

  Sprite grey_down { tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  id += ncols;
  Sprite grey_left { tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  id += ncols;
  Sprite grey_right{ tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  id += ncols;
  Sprite grey_up   { tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };

  //Texture used for the ground material
  raylib::Texture tex3{ "../resources/time_fantasy/tf_ashlands/3x_RMMV/tf_A5_ashlands_3.png" };
  
   raylib::Texture tex4{ "../resources/time_fantasy"
                       "/tf_ashlands/3x_RMMV/tf_B_ashlands_3.png" };

  ncols = 8; nrows = 16;
  std::vector<int> frame_ids(ncols*nrows);
  std::iota(frame_ids.begin(), frame_ids.end(), 0);
  Sprite all_ground_cells { tex3, ncols, nrows, { 10, 0 }, frame_ids, 5 };
  all_ground_cells.set_animation(true);
  Sprite grnd1 { tex3, ncols, nrows, { 50, 300 }, { 1, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29, } };
  Sprite grnd2{ tex3, ncols, nrows, { 50, 300 }, { 20, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29, } };
  Sprite grnd3{ tex3, ncols, nrows, { 50, 300 }, {22} };
  Sprite grnd4{ tex4, ncols, nrows, { 50, 300 }, { 15, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29, } };

  //sets the speed of the knight, and the default sprite to use.
  Sprite* grey_knight = &grey_right;
  const float grey_speed = 2.5f;

  //Variable to Check if the player is colliding with the reaper
  bool reaper_collision = false; 

  // n.b. "spacing" varies with the font & font size
  const int font_size = 30; 

  //bool display_text_box = true; SetExitKey(0); // debug
  bool display_text_box = false;
  const float border = 20;
  const float box_width = (float)window.GetWidth() - (2 * border);
  const float average_word_size = MeasureText("Abcdef", font_size);
  const float max_text_width = box_width - average_word_size;
  const float box_ypos = (float)window.GetHeight() - 200;
  const float box_height_small = (float)window.GetHeight() - box_ypos - border;
  const float box_height_large = (float)window.GetHeight() - (2 * border);
  const int lines_of_text_small = 4;
  const int lines_of_text_large = 15;
  int lines_of_text = lines_of_text_small;
  Rectangle text_box_small{ border, box_ypos, box_width, box_height_small };
  Rectangle text_box_large{ border, border,   box_width, box_height_large };
  Rectangle* text_box = &text_box_small;

  //sets up the strings used to split up sections of text with the AI
  const std::string human_stop = "Human: ";
  const std::string reaper_stop = "Grim Reaper: ";
  const std::string new_lines = "\n\n\n\n\n\n\n\n\n"; // 9
  std::string prompt = new_lines + reaper_stop +
                       "Why are you here?\n" + human_stop;
  int tail_index_large = 0;
  int tail_index_small = prompt.find(reaper_stop) - 1;
  int* tail_index = &tail_index_small;
  int nchars_entered = 0;

  //Detect window close button or ESC key
  while (!window.ShouldClose()) 
  {
    music.Update();

    (*grey_knight).set_animation(false);

    if (display_text_box)
    {
      switch (GetKeyPressed())
      {
        case KEY_ESCAPE:
          if (text_box == &text_box_large)
          {
            text_box = &text_box_small;
            tail_index = &tail_index_small;
            lines_of_text = lines_of_text_small;
          }
          else
          {
            SetExitKey(KEY_ESCAPE);
            display_text_box = false;
            music.SetVolume(music_volume_normal);
          }
          break;

        case KEY_ENTER:
          {
          std::cout << "KEY_ENTER PRESSED!\n";
          const std::string reaper_nature =
            "The following is a conversation with the grim reaper. The grim "
            "reaper is a personified force. In some mythologies, the grim "
            "reaper causes the victim's death by coming to collect that "
            " person's soul.\n\n";

          std::string response_str{};
          const auto stop = std::optional{std::vector{human_stop, reaper_stop}};
          prompt.push_back('\n');
          std::cout << (reaper_nature + prompt + reaper_stop) << std::endl;
            
          oai_help.submit(reaper_nature + prompt +  reaper_stop, response_str, stop);
          response_str.push_back('\n');
          std::cout << response_str;
          response_str = reaper_stop + response_str;
          for (auto c : response_str)
          {
            update_prompt(prompt, c, font_size, max_text_width,
                          tail_index_large, tail_index_small, nchars_entered);
          }
          for (auto d : human_stop) {
              update_prompt(prompt, d, font_size, max_text_width,
                  tail_index_large, tail_index_small, nchars_entered);
          }

          }
          break;

        //Makes the text box larger so the player can more easily read their chat with the reaper
        case KEY_UP:
          text_box = &text_box_large;
          tail_index = &tail_index_large;
          lines_of_text = lines_of_text_large;
          break;
       
        //Allows the player to delete text from their current entry with the reaper.
        case KEY_BACKSPACE:
          if (nchars_entered > 0)
          {
            bool reposition = prompt.back() == '\n'; // last char is newline
            prompt.pop_back();
            nchars_entered--;
            if (reposition)
            {
              tail_index_large = prompt.rfind('\n', tail_index_large - 2) + 1;
              tail_index_small = prompt.rfind('\n', tail_index_small - 2) + 1;
            }
          }
          break;
      }

      while (int key = GetCharPressed())
      {
        //Sets which characters can be entered, so only normal letters and numbers, and not functional keys such as ESC
        if ((key >= 32) && (key <= 125)) 
        {
          /*update_prompt(prompt, key, font_size, max_text_width, tail_index_large,
                        tail_index_small, nchars_entered);*/
          const char* psz = &prompt[prompt.rfind('\n') + 1];
          std::cout << psz;
          if ((char)key == ' ' && MeasureText(psz, font_size) > max_text_width)
          {
            prompt.push_back('\n');
            tail_index_large = prompt.find('\n', tail_index_large) + 1;
            tail_index_small = prompt.find('\n', tail_index_small) + 1;
          }
          else
          {
            prompt.push_back((char)key);
          }

          nchars_entered++;
        }
      }
    }
    else
    {
      //Changes the sprite and moves the character in the appropriate direction base on the characters input.
      if (IsKeyDown(KEY_DOWN))
      {
        grey_posn.y += grey_speed;
        grey_knight = &grey_down;
        (*grey_knight).set_animation(true);
      }
      if (IsKeyDown(KEY_UP))
      {
        grey_posn.y -= grey_speed;
        grey_knight = &grey_up;
        (*grey_knight).set_animation(true);
      }
      if (IsKeyDown(KEY_LEFT))
      {
        grey_posn.x -= grey_speed;
        grey_knight = &grey_left;
        (*grey_knight).set_animation(true);
      }
      if (IsKeyDown(KEY_RIGHT))
      {
        grey_posn.x += grey_speed;
        grey_knight = &grey_right;
        (*grey_knight).set_animation(true);
      }

      (*grey_knight).set_posn(grey_posn);

      //Detects the player being close enough to the reaper to "collide"
      if (Vector2Distance(grey_posn, reaper.get_posn()) < 40.0f)
      {
        //makes sure player is not already colliding with the reaper
        if (!reaper_collision)
        {
          reaper_collision = true;
          display_text_box = true;
          SetExitKey(0);
          coin_sound.Play();
          music.SetVolume(music_volume_quiet);
        }
      }
      else
      {
        reaper_collision = false;
      }

      //Detects the player collecting a dimond and updates the dimonds collected variable.
      if (Vector2Distance(grey_posn, dimond_gem.get_posn()) < 40.0f)
      {
          d_gem_x = randomFloat(100.0f, 900.0f);
          d_gem_y = randomFloat(100.0f, 500.0f);
          d_gem_posn = { d_gem_x , d_gem_y };
          dimond_gem.set_posn(d_gem_posn);
          coin_sound.Play();
          diamond_collected++;
      }

        //Detects the player collecting a emerald and updates the emeralds collected variable.
      if (Vector2Distance(grey_posn, emerald_gem.get_posn()) < 40.0f)
      {
          e_gem_x = randomFloat(150.0f, 950.0f);
          e_gem_y = randomFloat(150.0f, 550.0f);
          e_gem_posn = { e_gem_x , e_gem_y };
          emerald_gem.set_posn(e_gem_posn);
          coin_sound.Play();
          emerald_collected++;
      }

      //Detects the player collecting a garnet and updates the garnets collected variable.
      if (Vector2Distance(grey_posn, garnet_gem.get_posn()) < 40.0f)
      {
          g_gem_x = randomFloat(150.0f, 950.0f);
          g_gem_y = randomFloat(150.0f, 550.0f);
          g_gem_posn = { g_gem_x , g_gem_y };
          garnet_gem.set_posn(g_gem_posn);
          coin_sound.Play();
          garnet_collected++;
      }
    }
    
    //Converts the gems collected integer into a string that can be displayed
    std::string gem_string = "Total Score: " + std::to_string((diamond_collected*10)+ (emerald_collected*5)+ (garnet_collected*5));
    std::string diamond_string = "Diamonds Collected: " + std::to_string(diamond_collected);
    std::string emerald_string = "Emeralds Collected: " + std::to_string(emerald_collected);
    std::string garnet_string = "Garnets Collected: " + std::to_string(garnet_collected);
    
       // Camera target follows player
    camera.target = Vector2{ grey_posn.x + 40, grey_posn.y + 40 };

    // Camera zoom controls
    camera.zoom += ((float)GetMouseWheelMove() * 0.05f);

    if (camera.zoom > 3.0f) camera.zoom = 3.0f;
    else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

    // Camera reset (zoom and rotation)
    if (IsKeyPressed(KEY_R))
    {
        camera.zoom = 1.0f;
        camera.rotation = 0.0f;
    }

    //begins drawing the sprites and text onto the screen

    BeginDrawing();

    //makes a white background behind everything
    ClearBackground(RAYWHITE);


    //draws the ground using a selection of sprites from the ground texture file.
    BeginMode2D(camera);

    all_ground_cells.draw_cell(0, 0); // ROOM1/ LEFT, RIGHT & BOTTOM WALLS
    for (int i = 0; i < 20; i++)
    {
      for (int j = 0; j < 1; j++)
      {
        grnd1.draw_cell( j, 1+i % grnd1.get_frame_ids_size());
        grnd1.draw_cell( 20+j, 1+i % grnd1.get_frame_ids_size());
        grnd1.draw_cell( i, 20-j% grnd1.get_frame_ids_size());
      }
    }

   //draws the ground using a selection of sprites from the ground texture file.
    all_ground_cells.draw_cell(0, 0); //ROOM1/ GROUND
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            grnd2.draw_cell( 1 + i, 1 + j, i % grnd2.get_frame_ids_size());

        }
    }  

    all_ground_cells.draw_cell(0, 0); //RO0M1/ TOP WALL
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            grnd1.draw_cell(i, j % grnd1.get_frame_ids_size());
            grnd1.draw_cell(11+i, j% grnd1.get_frame_ids_size());
        }
    }

    all_ground_cells.draw_cell(0, 0); //ROOM1, DOOR
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            grnd3.draw_cell(10+i, j % grnd1.get_frame_ids_size());
        }
    }

    all_ground_cells.draw_cell(0, 0); //ROOM2/ GROUND
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            grnd2.draw_cell(1 + i, -20 + j, i % grnd2.get_frame_ids_size());

        }
    }

    all_ground_cells.draw_cell(0, 0); // ROOM2/ LEFT, RIGHT & TOP WALLS
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            grnd1.draw_cell(j, -20 + i % grnd1.get_frame_ids_size());
            grnd1.draw_cell(20 + j, -20 + i % grnd1.get_frame_ids_size());
            grnd1.draw_cell(i, -20 - j % grnd1.get_frame_ids_size());
        }
    }

    all_ground_cells.draw_cell(0, 0); //RO0M2/ TOP WALL
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            grnd1.draw_cell(i, -1 + j % grnd1.get_frame_ids_size());
            grnd1.draw_cell(11 + i, -1 + j % grnd1.get_frame_ids_size());
        }
    }

    all_ground_cells.draw_cell(0, 0); //ROOM2, DOOR
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            grnd3.draw_cell(10 + i, -1+j % grnd1.get_frame_ids_size());
            grnd4.draw_cell(8 + i, -2+j % grnd4.get_frame_ids_size());

        }
    }


    /*for (int j = 0; j < 12; j++)
    {
        grnd1.draw_cell(2 + j, 2, (2+ j) % grnd1.get_frame_ids_size());
        grnd2.draw_cell(2 + j, 3, (5+ j) % grnd2.get_frame_ids_size());
    }
    */
    //Draws the characters and gems in the appropriate order for which is infront
    std::vector<Sprite*> vsp{ grey_knight, &reaper, &dimond_gem, &emerald_gem, &garnet_gem };
    std::sort(vsp.begin(), vsp.end(), [](Sprite* s1, Sprite* s2) {
        return s1->get_posn().y < s2->get_posn().y;
        }
    );

    for (const auto& s : vsp)
    {
        s->draw();     
    }


    if (display_text_box)
    {
      Color light_gray_transparent{ 80, 80, 80, 192 }; // 192/256 nontransparent
      DrawRectangleRec(*text_box, light_gray_transparent);
      unsigned int milliseconds = (unsigned int)(GetTime() * 1000.0);
      std::string s = &prompt[*tail_index];
      if ((milliseconds % 1000) > 500)
      {
        s.push_back('_');
      }
      std::string help_s{};
      int number_newlines =0;
      //counts amount of new lines in string
      for (int i = 0; i < s.size(); i++) {
          if (s[i] == '\n') {
              number_newlines++;
          }
      }

      if (number_newlines <= lines_of_text) {
          help_s = s;
      }
      else {
          for (int i = 0; i < lines_of_text; i++) {
              if (s.find_last_of('\n')) {
                  help_s = s.substr(s.find_last_of('\n'), s.size()) + help_s;
                  s.resize(s.find_last_of('\n'));
              }
          }
      }
      if (help_s.at(0) == '\n') {
          help_s.erase(0, 1);
      }
      //std::cout << help_s;
      DrawText(help_s.c_str(), (*text_box).x + 12, (*text_box).y + 12, font_size, WHITE);
    }

    //Draws text onto the screen displaying how many gems have been collected.
    DrawText(gem_string.c_str(), 50, 10, 20, BLACK);
    DrawText(diamond_string.c_str(), 50, 30, 20, BLACK);
    DrawText(emerald_string.c_str(), 50, 50, 20, BLACK);
    DrawText(garnet_string.c_str(), 50, 70, 20, BLACK);
    
    //DrawLine((int)camera.target.x, -1280 * 10, (int)camera.target.x, 1280 * 10, GREEN);
    //DrawLine(-720 * 10, (int)camera.target.y, 720 * 10, (int)camera.target.y, GREEN);

    EndMode2D();

    EndDrawing();
  }

  return 0;
}

void update_prompt(std::string& prompt, char c, const int font_size,
                   const float max_text_width, int& tail_index_large,
                   int& tail_index_small, int& nchars_entered)
{
  const char* psz = &prompt[prompt.rfind('\n') + 1];
  if (c == ' ' && MeasureText(psz, font_size) > max_text_width)
  {
    prompt.push_back('\n');
    tail_index_large = prompt.find_last_of('\n', tail_index_large) + 1;
    tail_index_small = prompt.find_last_of('\n', tail_index_small) + 1;
  }
  else
  {
    prompt.push_back(c);
  }

  nchars_entered++;
}
