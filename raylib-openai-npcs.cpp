#include "sprite.hpp"
#include "openai-helper.hpp"
#include "raylib-cpp.hpp"
#include "entity.cpp"
#include <string>
#include <optional>
#include <vector>
#include <numeric> // std::iota
#include <iostream>

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

  //Initial x and y values for the first gem.
  

  

  //variable to track the number of gems collected
  int gems_collected = 0;

  //sets the window size of the game
  int window_x = 1800;
  int window_y = 1000;
  float gem_x = randomFloat(100.0f, (float)window_x - 100);
  float gem_y = randomFloat(100.0f, (float)window_y - 100);
  raylib::Window window(window_x, window_y, "Raylib OpenAI NPCs");

  SetTargetFPS(60);            // Set our game to run at 60 frames-per-second

  raylib::AudioDevice audio{}; // necessary: initialises the audio
  raylib::Sound coin_sound{ "../resources/audio/coin.wav" };
  raylib::Sound sword_sound{ "../resources/audio/sword.wav" };
  raylib::Music music{ "../resources/audio/Magic-Clock-Shop.mp3" };
  float music_volume_normal = 1.0f, music_volume_quiet = 0.4f;
  music.Play();

  raylib::Texture tex1{ "../resources/time_fantasy/reaper_blade_3.png" };
  Sprite reaper{ tex1, 3, 4, { 340, 192 }, { 0 } };

  //Sprites for Gems. Each gem has 6 different rotations, and there are 3 gems.
  raylib::Texture tex4{ "../resources/time_fantasy/gems_cut.png" };
  int gcols = 6, grows = 3;
  int gid = 1;
  Vector2 gem_posn{ gem_x, gem_y };
  Sprite gem{ tex4, gcols, grows, gem_posn, {0,1,2,3,4,5 }, 6 };
  
  //loads the texture sheet and setup for the sprites the Knight uses
  raylib::Texture tex2{ "../resources/time_fantasy/knights_3x.png" };
  int ncols = 12, nrows = 8;
  int id = 3;
  Vector2 grey_posn{ 40.0f, 100.0f };
  raylib::Texture tex5{ "../resources/time_fantasy/sword.png" };
  Sprite sword{ tex5, 1, 1 , grey_posn, {1} , 1};

  //Sets which sprite to use for each direction
  Sprite grey_down { tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  id += ncols;
  Sprite grey_left { tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  id += ncols;
  Sprite grey_right{ tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  id += ncols;
  Sprite grey_up   { tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  raylib::Texture tex6{ "../resources/time_fantasy/ayy_gray_3x.png" };
  //entity zombie;
  Vector2 zombie_pos{ randomFloat(100.0f, window_x - 100), randomFloat(100.0f, window_y - 100) };
      Sprite zombie_down{ tex6, 3, 4, 
          zombie_pos,{0,1,2},6 };
      Sprite zombie_left{ tex6, 3, 4,
          zombie_pos,{3,4,5},6 };
      Sprite zombie_right{ tex6, 3, 4,
    zombie_pos,{6,7,8},6 };
      Sprite zombie_up{ tex6, 3, 4,
    zombie_pos,{9,10,11},6 };
  //Texture used for the ground material
  raylib::Texture tex3{ "../resources/time_fantasy/tf_ashlands/3x_RMMV/tf_A5_ashlands_3.png" };
  ncols = 8; nrows = 16;
  std::vector<int> frame_ids(ncols*nrows);
  std::iota(frame_ids.begin(), frame_ids.end(), 0);
  Sprite all_ground_cells { tex3, ncols, nrows, { 0, 0 }, frame_ids, 5 };
  all_ground_cells.set_animation(true);
  Sprite grnd1 { tex3, ncols, nrows, { 50, 300 }, { 1, 2, 3, 4 } };

  //sets the speed of the knight, and the default sprite to use.
  Sprite* grey_knight = &grey_right;
  Sprite* zombie_sprite = &zombie_down;
  entity zombie(zombie_sprite, zombie_pos, 100, 1.5, true);
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
  unsigned int last_sword = (unsigned int)(GetTime() * 1000.0);
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
        if (Vector2Distance(grey_posn, reaper.get_posn()) < 30.0f)
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

        //Detects the player collecting a gem and updates the gems collected variable.
        if (Vector2Distance(grey_posn, gem.get_posn()) < 30.0f)
        {
            gem_x = randomFloat(100.0f, window_x -100);
            gem_y = randomFloat(100.0f, window_y -100);
            gem_posn = { gem_x , gem_y };
            gem.set_posn(gem_posn);
            coin_sound.Play();
            gems_collected++;
        }
    }

    if (zombie.get_pos().x + zombie.get_pos().y -
        (grey_knight->get_posn().x + grey_knight->get_posn().y) < 500) {
        if (abs(zombie.get_pos().x - grey_knight->get_posn().x)
            >= abs(zombie.get_pos().y - grey_knight->get_posn().y)) {

            if (zombie.get_pos().x - grey_knight->get_posn().x >= 0) {

                zombie.set_pos({ zombie.get_pos().x - zombie.get_speed()
                    , zombie.get_pos().y });
                zombie.set_sprite(zombie_left);
            }
            else {
                zombie.set_pos({ zombie.get_pos().x + zombie.get_speed()
                   , zombie.get_pos().y });
                zombie.set_sprite(zombie_right);
            }
        }
            else if (zombie.get_pos().y - grey_knight->get_posn().y >= 0) {
                zombie.set_pos({ zombie.get_pos().x
                                    , zombie.get_pos().y - zombie.get_speed() });
                zombie.set_sprite(zombie_up);
            }
            else {
                zombie.set_pos({ zombie.get_pos().x
                                    , zombie.get_pos().y + zombie.get_speed() });
                zombie.set_sprite(zombie_down);
            }
            (*zombie.get_sprite()).set_posn(zombie.get_pos());
            //TO-DO: find out why animation causes a division by zero
            //(*zombie.get_sprite()).set_animation(true);
        }

    
    //Converts the gems collected integer into a string that can be displayed
    std::string gem_string = "Gems Collected: " + std::to_string(gems_collected);

    //begins drawing the sprites and text onto the screen
    BeginDrawing();
    //makes a white background behind everything
    ClearBackground(RAYWHITE);

    //draws the ground using a selection of sprites from the ground texture file.
    all_ground_cells.draw_cell(0, 0);
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            grnd1.draw_cell(2 + i, 2 + j, i % grnd1.get_frame_ids_size());
        }
    }

    if (IsKeyPressed(KEY_SPACE) || (unsigned int)(GetTime() * 1000.0) - last_sword <= 200) {
        unsigned int milliseconds = (unsigned int)(GetTime() * 1000.0);
        Vector2 sword_pos = grey_knight->get_posn();

        if (last_sword == 0 || milliseconds - last_sword >= 500 || (unsigned int)(GetTime() * 1000.0) - last_sword <= 200) {
            if (grey_knight == &grey_down) {
                sword.set_angle(90);
                sword_pos.x = sword_pos.x + 70;
                sword_pos.y = sword_pos.y + 80;
            }
            else if (grey_knight == &grey_left) {
                sword_pos.x = sword_pos.x + 30;
                sword_pos.y = sword_pos.y + 100;
                sword.set_angle(180);
            }
            else if (grey_knight == &grey_up) {
                sword_pos.x = sword_pos.x + 5;
                sword_pos.y = sword_pos.y + 40;
                sword.set_angle(270);

            }
            if (grey_knight == &grey_right) {
                sword.set_angle(0);
                sword_pos.x = sword_pos.x + 30;
                sword_pos.y = sword_pos.y + 40;
            }
            sword_sound.Play();
            sword.set_posn(sword_pos);
            sword.draw();
            if (!((unsigned int)(GetTime() * 1000.0) - last_sword <= 200)) {
                last_sword = (unsigned int)(GetTime() * 1000.0);
            }
        }
 
        

    }
   (*zombie.get_sprite()).draw();
    //Draws the reaper and character in the appropriate order for which is infront
    if (grey_posn.y < reaper.get_posn().y)
    {
      (*grey_knight).draw();
      reaper.draw();
    }
    else
    {
      reaper.draw();
      (*grey_knight).draw();
    }
    
    //Draws the gem and character in the appropriate order for which is infront
    gem.set_animation(true);
    if (grey_posn.y < gem.get_posn().y)
    {
        (*grey_knight).draw();
        gem.draw();
    }
    else
    {
        gem.draw();
        (*grey_knight).draw();
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
    DrawText(gem_string.c_str(), 50, 50, 30, BLACK);

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
