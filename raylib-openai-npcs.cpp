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
//uses AABB for collision detection
bool detectCollision(Rectangle rect1, Rectangle rect2) {
    if (rect1.x < rect2.x + rect2.width &&
        rect1.x + rect1.width > rect2.x &&
        rect1.y < rect2.y + rect2.height &&
        rect1.height + rect1.y > rect2.y)
        return true;
    else return false;
}
//TO-DO: Draw a game over screen
void drawGameOver(int x, int y) {
    DrawRectangle(0, 0, x, y, GRAY);
    DrawText("Game Over!", x / 3, y / 6, 120, BLACK);
}
void damage(aipfg::entity &knight, std::vector <aipfg::entity*> &enemies, Rectangle sword_rect, raylib::Sound &attacksound) {
    int i = 0;
    while (i < enemies.size())
    {
        Rectangle enemyrect = (*enemies.at(i)).calculate_rectangle();
        //slightly shifted as enemy and player cannot move into each other
        enemyrect.x -= 1;
        enemyrect.y -= 1;
        enemyrect.width += 2;
        enemyrect.height += 2;
        if (detectCollision(enemyrect, knight.calculate_rectangle())) {
            if ((unsigned int)(GetTime() * 1000.0) - knight.get_lastdamage() > 1000) {
                knight.set_hp(knight.get_hp() - (*enemies.at(i)).get_damage());
                attacksound.Play();
                knight.set_lastdamage((unsigned int)(GetTime() * 1000.0));
                if (knight.get_hp() <= 0) {
                    std::cout << "Dead";
                }
        }
        }
        if (detectCollision((*enemies.at(i)).calculate_rectangle(), sword_rect) &&
            (unsigned int)(GetTime() * 1000.0) - (*enemies.at(i)).get_lastdamage() > 1000) {

            (*enemies.at(i)).set_hp((*enemies.at(i)).get_hp() - knight.get_damage());
            (*enemies.at(i)).set_lastdamage((unsigned int)(GetTime() * 1000.0));
            if ((*enemies.at(i)).get_hp() <= 0) {
                delete enemies.at(i);
                enemies.erase(enemies.begin() + i);
                i--;
            }
        }
        i++;
    }
}

void generate_enemies(std::vector <aipfg::entity*> &enemies, int amount, aipfg::Sprite* sprite, int x, int y, int hp, float speed, int damage) {
    for (int i = 0; i < amount; i++) {
        float XE = randomFloat(100.0f, x - 100);
        float YE = randomFloat(100.0f, y - 100);
        aipfg::Sprite* localsprite = new aipfg::Sprite((*sprite));
        (*localsprite).set_posn({ XE, YE });
        aipfg::entity* entity = new aipfg::entity(localsprite, hp, speed, true, damage);
        (*entity).set_pos({ XE, YE});
        enemies.push_back(entity);
    }
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


  raylib::Window window(1800, 1000, "Raylib OpenAI NPCs");
  float gem_x = randomFloat(100.0f, (float)window.GetWidth() - 100);
  float gem_y = randomFloat(100.0f, (float)window.GetHeight() - 100);
  SetTargetFPS(60);            // Set our game to run at 60 frames-per-second

  raylib::AudioDevice audio{}; // necessary: initialises the audio
  raylib::Sound coin_sound{ "../resources/audio/coin.wav" };
  raylib::Sound sword_sound{ "../resources/audio/sword.wav" };
  raylib::Music music{ "../resources/audio/Magic-Clock-Shop.mp3" };
  raylib::Sound zombie_sound{ "../resources/audio/zombie.wav" };
  raylib::Sound bat_sound{ "../resources/audio/bat.wav" };
  float music_volume_normal = 1.0f, music_volume_quiet = 0.4f;
  music.Play();

  raylib::Texture tex1{ "../resources/time_fantasy/reaper_blade_3.png" };
  Sprite reaper{ tex1, 3, 4, { 340, 192 }, { 0 } };

  //Sprites for Gems. Each gem has 6 different rotations, and there are 3 gems.
  raylib::Texture tex4{ "../resources/time_fantasy/gems_cut.png" };
  int gcols = 6, grows = 3;
  int gid = 1;
  Vector2 gem_posn{ gem_x, gem_y };
  Sprite gem{ tex4, gcols, grows, gem_posn, {0,1,2,3,4,5}, 6 };
  
  //loads the texture sheet and setup for the sprites the Knight uses
  raylib::Texture tex2{ "../resources/time_fantasy/knights_3x.png" };
  int ncols = 12, nrows = 8;
  int id = 3;
  Vector2 grey_posn{ 40.0f, 100.0f };
  raylib::Texture tex5{ "../resources/time_fantasy/sword.png" };
  Sprite sword{ tex5, 1, 1 , grey_posn, {1} , 1};
  bool isSwordActive{ false };
  //Sets which sprite to use for each direction
  Sprite grey_down { tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  id += ncols;
  Sprite grey_left { tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  id += ncols;
  Sprite grey_right{ tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  id += ncols;
  Sprite grey_up   { tex2, ncols, nrows, grey_posn, { id, id+1, id+2 }, 6 };
  std::vector<Sprite> grey_vector = { grey_down, grey_left, grey_right, grey_up };
  raylib::Texture tex6{ "../resources/time_fantasy/ayy_gray_3x.png" };
  
  //entity zombie;
  Vector2 zombie_pos{ randomFloat(100.0f, window.GetWidth() - 100), randomFloat(100.0f, window.GetHeight() - 100) };
      Sprite zombie_down{ tex6, 3, 4, 
          zombie_pos,{0,1,2},6 };
      Sprite zombie_left{ tex6, 3, 4,
          zombie_pos,{3,4,5},6 };
      Sprite zombie_right{ tex6, 3, 4,
    zombie_pos,{6,7,8},6 };
      Sprite zombie_up{ tex6, 3, 4,
    zombie_pos,{9,10,11},6 };
      std::vector<Sprite*> zombie_vector = { &zombie_down, &zombie_left, &zombie_right, &zombie_up };
 //bat
      raylib::Texture tex7{ "../resources/time_fantasy/vampire_fly.png" };
      Sprite bat_down{ tex7, 3, 4, zombie_pos, {0,1,2},6 };
      Sprite bat_left{ tex7, 3, 4, zombie_pos, {3,4,5},6 };
      Sprite bat_right{ tex7, 3, 4, zombie_pos, {6,7,8},6 };
      Sprite bat_up{ tex7, 3, 4, zombie_pos, {9,10,11},6 };
      std::vector<Sprite*> bat_vector = { &bat_down, &bat_left, &bat_right, &bat_up };
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
  Sprite* bat_sprite = &bat_down;
  //(*zombie_sprite).set_animation(false);
  std::vector <entity*> enemies;
  std::vector <entity*> enemies_bat;
  generate_enemies(enemies, 2, zombie_sprite, window.GetWidth(), window.GetHeight(), 100, 1.5, 15);
  generate_enemies(enemies_bat, 2, bat_sprite, window.GetWidth(), window.GetHeight(), 100, 3, 10);
  entity knight(grey_knight, 100, 2.6, false, 25);
  const float grey_speed = 2.5f;
  Rectangle sword_rect{};
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

    (*knight.get_sprite()).set_animation(false);

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
        knight.move(grey_vector, enemies);
        //sword functionality
        isSwordActive = false;
        sword_rect={};
        if (IsKeyPressed(KEY_SPACE) || (unsigned int)(GetTime() * 1000.0) - last_sword <= 200) {
            unsigned int milliseconds = (unsigned int)(GetTime() * 1000.0);
            Vector2 sword_pos = knight.get_pos();

            if (last_sword == 0 || milliseconds - last_sword >= 500 || (unsigned int)(GetTime() * 1000.0) - last_sword <= 200) {
                int sword_height = 40;
                int sword_width = 114;
                if (knight.get_sprite() == &grey_vector.at(0)) {
                   //down
                    sword.set_angle(90);
                    sword_pos.x = sword_pos.x + 70;
                    sword_pos.y = sword_pos.y + 80;
                    sword_rect = { sword_pos.x - sword_height - 12, sword_pos.y + 5, (float)sword_height, (float)sword_width };
                }
                else if (knight.get_sprite() == &grey_vector.at(1)) {
                   //left
                    sword_pos.x = sword_pos.x + 30;
                    sword_pos.y = sword_pos.y + 100;
                    sword.set_angle(180);
                    sword_rect = { sword_pos.x- sword_width-5, sword_pos.y- sword_height-12, (float)sword_width, (float)sword_height };
                }
                else if (knight.get_sprite() == &grey_vector.at(3)) {
                    //up
                    sword_pos.x = sword_pos.x + 5;
                    sword_pos.y = sword_pos.y + 40;
                    sword.set_angle(270);
                    sword_rect = { sword_pos.x+ 13, sword_pos.y - sword_width - 5, (float)sword_height, (float)sword_width };

                }
                else if (knight.get_sprite() == &grey_vector.at(2)) {
                   //right
                    sword.set_angle(0);
                    sword_pos.x = sword_pos.x + 30;
                    sword_pos.y = sword_pos.y + 40;
                    sword_rect = { sword_pos.x + 5, sword_pos.y+ 12, (float)sword_width, (float)sword_height };
                }
                sword_sound.Play();
                sword.set_posn(sword_pos);
                isSwordActive = true;
                if (!((unsigned int)(GetTime() * 1000.0) - last_sword <= 200)) {
                    last_sword = (unsigned int)(GetTime() * 1000.0);
                }
            }



        }
        //Detects the player being close enough to the reaper to "collide"
        if (Vector2Distance(knight.get_pos(), reaper.get_posn()) < 30.0f)
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
        if (Vector2Distance(knight.get_pos(), gem.get_posn()) < 30.0f)
        {
            gem_x = randomFloat(100.0f, window.GetWidth() -100);
            gem_y = randomFloat(100.0f, window.GetHeight() -100);
            gem_posn = { gem_x , gem_y };
            gem.set_posn(gem_posn);
            coin_sound.Play();
            gems_collected++;
        }
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
 
    damage(knight, enemies, sword_rect, zombie_sound);
    damage(knight, enemies_bat, sword_rect, bat_sound);
    for (int i = 0; i < enemies_bat.size(); i++) {
        (*enemies_bat.at(i)).follow(knight, 500, bat_vector);
        (*enemies_bat.at(i)).draw();
        //DrawRectangle((*enemies.at(i)).calculate_rectangle().x, (*enemies.at(i)).calculate_rectangle().y, (*enemies.at(i)).calculate_rectangle().width, (*enemies.at(i)).calculate_rectangle().height ,BLACK);
    }
    for (int i = 0; i < enemies.size(); i++) {
        (*enemies.at(i)).follow(knight, 300, zombie_vector);
        (*enemies.at(i)).draw();
        //DrawRectangle((*enemies.at(i)).calculate_rectangle().x, (*enemies.at(i)).calculate_rectangle().y, (*enemies.at(i)).calculate_rectangle().width, (*enemies.at(i)).calculate_rectangle().height ,BLACK);
    }

   if (isSwordActive) sword.draw();
   
    //Draws the reaper and character in the appropriate order for which is infront
    if (grey_posn.y < reaper.get_posn().y)
    {
      knight.draw();

      reaper.draw();
    }
    else
    {
      reaper.draw();
      knight.draw();
    }
    
    //Draws the gem and character in the appropriate order for which is infront
    gem.set_animation(true);
    if (knight.get_pos().y < gem.get_posn().y)
    {
        knight.draw();
        gem.draw();
    }
    else
    {
        gem.draw();
        knight.draw();
    }
    //DrawRectangle(knight.calculate_rectangle().x, knight.calculate_rectangle().y, knight.calculate_rectangle().width, knight.calculate_rectangle().height, BLACK);
    Vector2 knightcenter = { knight.calculate_rectangle().x + knight.calculate_rectangle().width /2, knight.calculate_rectangle().y + knight.calculate_rectangle().height/2 };
    //DrawCircleLines(knightcenter.x, knightcenter.y, 300, BLACK);
    //drawGameOver(1800, 1000);
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
