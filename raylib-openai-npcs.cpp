#include "sprite.hpp"
#include "openai-helper.hpp"
#include "raylib-cpp.hpp"
#include "entity.cpp"
#include "raylib-tileson.h"
#include <string>
#include <optional>
#include <vector>
#include <numeric> // std::iota
#include <iostream>
#include <algorithm>
#include <map> 
void update_prompt(std::string& prompt, char c, const int font_size,
    const float max_text_width, int& tail_index_large,
    int& tail_index_small, int& nchars_entered);

float randomFloat(float min, float max)
{
    return (min + 1) + (((float)rand()) / (float)RAND_MAX) * (max - (min + 1));
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
void damage(aipfg::entity& knight, std::vector <aipfg::entity*>& enemies, Rectangle sword_rect, raylib::Sound& attacksound, bool &isGameOver) {
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
                    isGameOver = true;
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

void generate_enemies(std::vector <aipfg::entity*>& enemies, int amount, aipfg::Sprite* sprite, int width, int height, int hp, float speed, int damage, int x_start, int y_start) {
    for (int i = 0; i < amount; i++) {
        float XE = randomFloat(x_start, width);
        float YE = randomFloat(y_start, height);
        aipfg::Sprite* localsprite = new aipfg::Sprite((*sprite));
        (*localsprite).set_posn({ XE, YE });
        aipfg::entity* entity = new aipfg::entity(localsprite, hp, speed, true, damage);
        (*entity).set_pos({ XE, YE });
        enemies.push_back(entity);
    }
}

bool attack(aipfg::entity &knight, unsigned int &last_sword, Rectangle &sword_rect, std::vector<aipfg::Sprite> &grey_vector, aipfg::Sprite &sword) {
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
                sword_rect = { sword_pos.x - sword_width - 5, sword_pos.y - sword_height - 12, (float)sword_width, (float)sword_height };
            }
            else if (knight.get_sprite() == &grey_vector.at(3)) {
                //up
                sword_pos.x = sword_pos.x + 5;
                sword_pos.y = sword_pos.y + 40;
                sword.set_angle(270);
                sword_rect = { sword_pos.x + 13, sword_pos.y - sword_width - 5, (float)sword_height, (float)sword_width };

            }
            else if (knight.get_sprite() == &grey_vector.at(2)) {
                //right
                sword.set_angle(0);
                sword_pos.x = sword_pos.x + 30;
                sword_pos.y = sword_pos.y + 40;
                sword_rect = { sword_pos.x + 5, sword_pos.y + 12, (float)sword_width, (float)sword_height };
            }
            sword.set_posn(sword_pos);
            if (!((unsigned int)(GetTime() * 1000.0) - last_sword <= 200)) {
                last_sword = (unsigned int)(GetTime() * 1000.0);
            }
            return true;
        }




    }
    return false;
}

void ranPosGen(float& d_gem_x, float& d_gem_y, float& e_gem_x, float& e_gem_y,  float& g_gem_x, float& g_gem_y, float fStart, float fEnd)
{
    srand(time(0));  // Initialize random number generator.
    float myNumbers[6];
    int n = 6;
    for (int i = 0; i < n; i++)
    {
        float numbr = randomFloat(fStart, fEnd);
        myNumbers[i] = numbr;
    }
     d_gem_x = myNumbers[n - 1];
     d_gem_y = myNumbers[n - 2];
     e_gem_x = myNumbers[n - 3];
     e_gem_y = myNumbers[n - 4];
     g_gem_x = myNumbers[n - 5];
     g_gem_y = myNumbers[n - 6];
}

int main(int argc, char* argv[])
{
    using namespace aipfg;
    openai_helper oai_help;
    if (!oai_help.init())
    {
        return -1;
    }
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
    raylib::Sound sword_sound{ "../resources/audio/sword.wav" };
    raylib::Music music{ "../resources/audio/Magic-Clock-Shop.mp3" };
    raylib::Sound zombie_sound{ "../resources/audio/zombie.wav" };
    raylib::Sound bat_sound{ "../resources/audio/bat.wav" };
    float music_volume_normal = 1.0f, music_volume_quiet = 0.4f;
    music.Play();

    raylib::Texture tex1{ "../resources/time_fantasy/reaper_blade_3.png" };
    Sprite reaper{ tex1, 3, 4, { 340, 192 }, { 0 } };

    //..................GEM......................
    float d_gem_x, d_gem_y, e_gem_x, e_gem_y, g_gem_x, g_gem_y = 0.0f;
    float d2_gem_x, d2_gem_y, e2_gem_x, e2_gem_y, g2_gem_x, g2_gem_y = 0.0f;
    float d3_gem_x, d3_gem_y, e3_gem_x, e3_gem_y, g3_gem_x, g3_gem_y ;
    float d4_gem_x, d4_gem_y, e4_gem_x, e4_gem_y, g4_gem_x, g4_gem_y ;

    //Initial x and y values for the 1st diamond, emerald and garnet.
    ranPosGen(d_gem_x, d_gem_y, e_gem_x, e_gem_y, g_gem_x, g_gem_y, 50.0f, 700.0f);
    //Initial x and y values for the 2nd diamond, emerald and garnet.
    ranPosGen(d2_gem_x, d2_gem_y, e2_gem_x, e2_gem_y, g2_gem_x, g2_gem_y, 820.0f, 1100.0f);//x50-1100  y820-1400
    //Initial x and y values for the 3rd diamond, emerald and garnet.
    ranPosGen(d3_gem_x, d3_gem_y, e3_gem_x, e3_gem_y, g3_gem_x, g3_gem_y, 50.0f, 1000.0f);//x1200-2300 y0-1000
    //Initial x and y values for the 4th diamond, emerald and garnet.
    ranPosGen(d4_gem_x, d4_gem_y, e4_gem_x, e4_gem_y, g4_gem_x, g4_gem_y, 1300.0f, 1800.0f);//x1300-2300 y1100-1800


    //variable to track the number of gems collected
    int gems_collected = 0;
    int diamond_collected = 0;
    int emerald_collected = 0;
    int garnet_collected = 0;
    //Sprite for diamond.
    raylib::Texture diamond_tex{ "../resources/time_fantasy/diamond.png" };
    int d_cols = 6, d_rows = 1;
    Vector2 d_gem_posn{ d_gem_x, d_gem_y };
    std::vector<int> frame_id_diamond(d_cols * d_rows);
    std::iota(frame_id_diamond.begin(), frame_id_diamond.end(), 0);
    Sprite dimond_gem{ diamond_tex, d_cols, d_rows, d_gem_posn, frame_id_diamond, 7 };
    dimond_gem.set_animation(true);

    Vector2 d2_gem_posn{ d2_gem_x, d2_gem_y };
    Sprite dimond2_gem{ diamond_tex, d_cols, d_rows, d2_gem_posn, frame_id_diamond, 7 };
    dimond2_gem.set_animation(true);

    Vector2 d3_gem_posn{ d3_gem_x+1250, d3_gem_y };
    Sprite dimond3_gem{ diamond_tex, d_cols, d_rows, d3_gem_posn, frame_id_diamond, 7 };
    dimond3_gem.set_animation(true);

    Vector2 d4_gem_posn{ d4_gem_x, d4_gem_y };
    Sprite dimond4_gem{ diamond_tex, d_cols, d_rows, d4_gem_posn, frame_id_diamond, 7 };
    dimond4_gem.set_animation(true);

    //Sprite for emerald.
    raylib::Texture emerald_tex{ "../resources/time_fantasy/emerald.png" };
    int e_cols = 6, e_rows = 1;
    Vector2 e_gem_posn{ e_gem_x, e_gem_y };
    std::vector<int> frame_id_emerald(e_cols * e_rows);
    std::iota(frame_id_emerald.begin(), frame_id_emerald.end(), 0);
    Sprite emerald_gem{ emerald_tex, e_cols, e_rows, e_gem_posn, frame_id_emerald, 7 };
    emerald_gem.set_animation(true);

    Vector2 e2_gem_posn{ e2_gem_x, e2_gem_y };
    Sprite emerald2_gem{ emerald_tex, e_cols, e_rows, e2_gem_posn, frame_id_emerald, 7 };
    emerald2_gem.set_animation(true);

    Vector2 e3_gem_posn{ e3_gem_x + 1250, e3_gem_y };
    Sprite emerald3_gem{ emerald_tex, e_cols, e_rows, e3_gem_posn, frame_id_emerald, 7 };
    emerald3_gem.set_animation(true);

    Vector2 e4_gem_posn{ e4_gem_x, e4_gem_y };
    Sprite emerald4_gem{ emerald_tex, e_cols, e_rows, e4_gem_posn, frame_id_emerald, 7 };
    emerald4_gem.set_animation(true);

    //Sprite for garnet.
    raylib::Texture garnet_tex{ "../resources/time_fantasy/garnet.png" };
    int g_cols = 6, g_rows = 1;
    Vector2 g_gem_posn{ g_gem_x, g_gem_y };
    std::vector<int> frame_id_garnet(g_cols * g_rows);
    std::iota(frame_id_garnet.begin(), frame_id_garnet.end(), 0);
    Sprite garnet_gem{ garnet_tex, g_cols, g_rows, g_gem_posn, frame_id_garnet, 7 };
    garnet_gem.set_animation(true);

    Vector2 g2_gem_posn{ g2_gem_x, g2_gem_y };
    Sprite garnet2_gem{ garnet_tex, g_cols, g_rows, g2_gem_posn, frame_id_garnet, 7 };
    garnet2_gem.set_animation(true);

    Vector2 g3_gem_posn{ g3_gem_x + 1250, g3_gem_y };
    Sprite garnet3_gem{ garnet_tex, g_cols, g_rows, g3_gem_posn, frame_id_garnet, 7 };
    garnet3_gem.set_animation(true);

    Vector2 g4_gem_posn{ g4_gem_x, g4_gem_y };
    Sprite garnet4_gem{ garnet_tex, g_cols, g_rows, g4_gem_posn, frame_id_garnet, 7 };
    garnet4_gem.set_animation(true);

    //loads the texture sheet and setup for the sprites the Knight uses
    raylib::Texture tex2{ "../resources/time_fantasy/knights_3x.png" };
    int ncols = 12, nrows = 8;
    int id = 3;

    raylib::Texture tex5{ "../resources/time_fantasy/sword.png" };
    Sprite sword{ tex5, 1, 1 , grey_posn, {1} , 1 };
    bool isSwordActive{ false };
    //Sets which sprite to use for each direction

    Sprite grey_down{ tex2, ncols, nrows, grey_posn, { id, id + 1, id + 2 }, 6 };
    id += ncols;
    Sprite grey_left{ tex2, ncols, nrows, grey_posn, { id, id + 1, id + 2 }, 6 };
    id += ncols;
    Sprite grey_right{ tex2, ncols, nrows, grey_posn, { id, id + 1, id + 2 }, 6 };
    id += ncols;
    Sprite grey_up{ tex2, ncols, nrows, grey_posn, { id, id + 1, id + 2 }, 6 };
    std::vector<Sprite> grey_vector = { grey_down, grey_left, grey_right, grey_up };
    raylib::Texture tex6{ "../resources/time_fantasy/ayy_gray_3x.png" };

    //entity zombie;
    Vector2 zombie_pos{ randomFloat(0, 20*48), randomFloat(-20*48, 20*48)};
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
    raylib::Texture tex8{ "../resources/time_fantasy/fairies_1x.png" };
    Vector2 fairy_pos = { grey_posn.x - 50, grey_posn.y };
    Sprite fairy_down{ tex8, 12, 8, fairy_pos, {0,1,2},6 };
    Sprite fairy_left{ tex8, 12, 8, fairy_pos, {12,13,14},6 };
    Sprite fairy_right{ tex8, 12, 8, fairy_pos, {24,25,26},6 };
    Sprite fairy_up{ tex8, 12, 8, fairy_pos, {36,37,38},6 };
    std::vector<Sprite*> fairy_vector = { &fairy_down, &fairy_left, &fairy_right, &fairy_up };
    raylib::Texture tex3{ "../resources/time_fantasy/tf_ashlands/3x_RMMV/tf_A5_ashlands_3.png" };

    raylib::Texture tex4{ "../resources/time_fantasy"
                        "/tf_ashlands/3x_RMMV/tf_B_ashlands_3.png" };
   //Map
    Map map = LoadTiled("../resources/New_World.tmj");
    //
    ncols = 8; nrows = 16;
    std::vector<int> frame_ids(ncols * nrows);
    std::iota(frame_ids.begin(), frame_ids.end(), 0);
    Sprite all_ground_cells{ tex3, ncols, nrows, { 10, 0 }, frame_ids, 5 };
    all_ground_cells.set_animation(true);
    Sprite grnd1{ tex3, ncols, nrows, { 50, 300 }, { 1, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29, } };
    Sprite grnd2{ tex3, ncols, nrows, { 50, 300 }, { 20, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29, } };
    Sprite grnd3{ tex3, ncols, nrows, { 50, 300 }, {22} };
    Sprite grnd4{ tex4, ncols, nrows, { 50, 300 }, { 15, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 28, 29, } };

    //sets the speed of the knight, and the default sprite to use.
    Sprite* fairy_sprite = &fairy_right;
    Sprite* grey_knight = &grey_right;
    Sprite* zombie_sprite = &zombie_down;
    Sprite* bat_sprite = &bat_down;
    //(*zombie_sprite).set_animation(false);
    std::vector <entity*> enemies;
    std::vector <entity*> enemies_bat;
    generate_enemies(enemies, 2, zombie_sprite, 20*48, 20*48, 100, 1.5, 15, 48, -(20*48));
    generate_enemies(enemies_bat, 2, bat_sprite, 20*48, 20*48, 100, 3, 10, 48 ,-(20*48));
    entity knight(grey_knight, 100, 2.6, false, 25);
    entity fairy(fairy_sprite, 0, 3, false, 0);
    const float grey_speed = 2.5f;
    Rectangle sword_rect{};
    //Variable to Check if the player is colliding with the reaper
    bool reaper_collision = false;

    // n.b. "spacing" varies with the font & font size
    const int font_size = 30;

    //bool display_text_box = true; SetExitKey(0); // debug
    bool reaper_display_text_box = false;
    const float border = 20;
    const float box_width = (float)window.GetWidth() - (2 * border);
    const float average_word_size = MeasureText("Abcdef", font_size);
    const float max_text_width = box_width - average_word_size;
    const float box_ypos = (float)window.GetHeight() - 200;
    const float box_height_small = (float)window.GetHeight() - box_ypos - border;
    const float box_height_large = (float)window.GetHeight() - (2 * border);
    Rectangle text_box_small{ border, box_ypos, box_width, box_height_small };
    Rectangle text_box_large{ border, border,   box_width, box_height_large };
    Rectangle* text_box = &text_box_small;
    const int lines_of_text_small = box_height_small / (30 + 15);
    const int lines_of_text_large = box_height_large / (30 + 15);
    int lines_of_text = lines_of_text_small;

    //sets up the strings used to split up sections of text with the AI
    bool QuestGiven = false;
    const std::string human_stop = "Human: ";
    const std::string reaper_stop = "Grim Reaper: ";
    const std::string new_lines = "\n\n\n\n\n\n\n\n\n"; // 9
    std::string questText = "Sup ";
    if (!QuestGiven)
    {
        questText = "I am here to reap your soul...unless you can collect a \ncollection of gems...perhaps 10 and you can live!\n";
    }
    std::string prompt = new_lines + reaper_stop +
        questText + human_stop;
    int tail_index_large = 0;
    int tail_index_small = prompt.find(reaper_stop) - 1;
    int* tail_index = &tail_index_small;
    int nchars_entered = 0;
    unsigned int last_sword = (unsigned int)(GetTime() * 1000.0);

    //The code for the text box when speaking to the fairy
    bool fairy_display_text_box = false;
    const float fairy_border = 20;
    const float fairy_box_width = (float)window.GetWidth() - (2 * fairy_border);
    const float fairy_average_word_size = MeasureText("Abcdef", font_size);
    const float fairy_max_text_width = fairy_box_width - fairy_average_word_size;
    const float fairy_box_ypos = (float)window.GetHeight() - 200;
    const float fairy_box_height_small = (float)window.GetHeight() - fairy_box_ypos - fairy_border;
    const float fairy_box_height_large = (float)window.GetHeight() - (2 * fairy_border);
    const int fairy_lines_of_text_small = box_height_small / (30 + 15);
    const int fairy_lines_of_text_large = box_height_large / (30 + 15);
    int fairy_lines_of_text = fairy_lines_of_text_small;
    Rectangle fairy_text_box_small{ fairy_border, fairy_box_ypos, fairy_box_width, fairy_box_height_small };
    Rectangle fairy_text_box_large{ fairy_border, fairy_border,   fairy_box_width, fairy_box_height_large };
    Rectangle* fairy_text_box = &fairy_text_box_small;
    //sets up the strings used to split up sections of text with the fairy AI.
    const std::string fairy_stop = "Navi: ";
    const std::string fairy_new_lines = "\n\n\n\n\n\n\n\n\n"; // 9
    std::string fairy_prompt = fairy_new_lines + fairy_stop +
        "I am Navi, your fairy! How can I help with your quest?\n" + human_stop;
    int fairy_tail_index_large = 0;
    int fairy_tail_index_small = fairy_prompt.find(fairy_stop) - 1;
    int* fairy_tail_index = &fairy_tail_index_small;
    int fairy_nchars_entered = 0;
    bool isGameOver = false;
    std::vector <Rectangle> walls{};
    walls.push_back({ 0, 0, 48, 58 * 48 });//0,0,48,2784                  left wall
    walls.push_back({ 50 * 48, 0, 48, 58 * 48 });//2400,0,48,2784         right wall
    walls.push_back({ 24 * 48,0, 48, 32 * 48 });//1152,0,48,1536          middle wall upper one
    walls.push_back({ 24 * 48, 36 * 48, 48, 2 * 48 });//1152,1728,48,96   middle wall lower one
    walls.push_back({ 12 * 48, 38 * 48, 48, 2 * 48 });//576,1824,48,96    the little wall of little room
    walls.push_back({ 12 * 48, 31 * 48, 48, 3 * 48 });//576,1488,48,144   on the little wall of little room

    walls.push_back({ 0, -1 * 48, 50 * 48, 48 });//0,-48,2400,48          upper wall
    //walls.push_back({ 0, -20 * 48, 20 * 48, 48 });//0,-960,960,48       ???????????
    walls.push_back({ 0, 16 * 48, 11 * 48, 48 });//0,768,528,48           2nd wall from x=0
    walls.push_back({ 24* 48, 22 * 48, 12 * 48, 48 });//1522,1056,576,48  upper-rigt room lower-left wall
    walls.push_back({ 40* 48, 22 * 48, 12 * 48, 48 });//1920,1056,576,48  upper-rigt room lower-right wall
    walls.push_back({ 0, 31 * 48, 13 * 48, 48 });//0,1488,624,48          3rd wall from x=0
    walls.push_back({ 0, 39 * 48, 50 * 48, 48 });//0,1872,2400,48         lower wall 
    walls.push_back({ 14 * 48, 16 * 48, 11 * 48, 48 });//672,768,528,48    wall with white spot
    //Detect window close button or ESC key
    while (!window.ShouldClose())
    {
        if (QuestGiven && gems_collected < 10)
        {
            questText = "Have you completed your quest? \nOr shall I take your soul now??\n";
    }
        else if(QuestGiven && gems_collected >=10)
        {
            questText = "Well...you completed your quest, so you may live this time. \nYou are free to leave!\n";
    }
    prompt = new_lines + reaper_stop +
        questText + human_stop;
        music.Update();

        (*knight.get_sprite()).set_animation(false);
        text_box_small.y = knight.get_pos().y + window.GetHeight() * 0.1;
        text_box_small.x = knight.get_pos().x - window.GetWidth() * 0.48;
        text_box_large.x = text_box_small.x;
        text_box_large.y = text_box_small.y - window.GetHeight() * 0.61;
        fairy_text_box_small.y = text_box_small.y;
        fairy_text_box_small.x = text_box_small.x;
        fairy_text_box_large.x = text_box_small.x;
        fairy_text_box_large.y = text_box_large.y;

        if (reaper_display_text_box)
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
                    reaper_display_text_box = false;
                    music.SetVolume(music_volume_normal);
                    QuestGiven = true;
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
                const auto stop = std::optional{ std::vector{human_stop, reaper_stop} };
                prompt.push_back('\n');
                std::cout << (reaper_nature + prompt + reaper_stop) << std::endl;

                oai_help.submit(reaper_nature + prompt + reaper_stop, response_str, stop);
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
        //This section activates when the player summons the fairy to speak to.
        else if (fairy_display_text_box)
        {
            switch (GetKeyPressed())
            {
            case KEY_ESCAPE:
                if (fairy_text_box == &fairy_text_box_large)
                {
                    fairy_text_box = &fairy_text_box_small;
                    fairy_tail_index = &fairy_tail_index_small;
                    fairy_lines_of_text = fairy_lines_of_text_small;
                }
                else
                {
                    SetExitKey(KEY_ESCAPE);
                    fairy_display_text_box = false;
                    music.SetVolume(music_volume_normal);
                }
                break;

            case KEY_ENTER:
            {
                std::cout << "KEY_ENTER PRESSED!\n";
                const std::string fairy_nature =
                    "The following is a conversation with Navi. Navi "
                    "is a personified force in a fairy body. Navi is a companion "
                    "who will be sarcastic and unhelpful to the player, often stating the obvious. "
                    "Navi knows the player is meant to be collecting gems, and is able to tell them this, but does not know how or why."
                    "Navi will also sometimes tell them random facts about the grim reaper, diamonds or emeralds.\n\n";

                std::string fairy_response_str{};
                const auto stop = std::optional{ std::vector{human_stop, fairy_stop} };
                fairy_prompt.push_back('\n');
                std::cout << (fairy_nature + fairy_prompt + fairy_stop) << std::endl;

                oai_help.submit(fairy_nature + fairy_prompt + fairy_stop, fairy_response_str, stop);
                fairy_response_str.push_back('\n');
                std::cout << fairy_response_str;
                fairy_response_str = fairy_stop + fairy_response_str;
                for (auto c : fairy_response_str)
                {
                    update_prompt(fairy_prompt, c, font_size, fairy_max_text_width,
                        fairy_tail_index_large, fairy_tail_index_small, fairy_nchars_entered);
                }
                for (auto d : human_stop) {
                    update_prompt(fairy_prompt, d, font_size, fairy_max_text_width,
                        fairy_tail_index_large, fairy_tail_index_small, fairy_nchars_entered);
                }
            }
            break;

            //Makes the text box larger so the player can more easily read their chat with the reaper
            case KEY_UP:
                fairy_text_box = &fairy_text_box_large;
                fairy_tail_index = &fairy_tail_index_large;
                fairy_lines_of_text = fairy_lines_of_text_large;
                break;

                //Allows the player to delete text from their current entry with the reaper.
            case KEY_BACKSPACE:
                if (fairy_nchars_entered > 0)
                {
                    bool reposition = fairy_prompt.back() == '\n'; // last char is newline
                    fairy_prompt.pop_back();
                    fairy_nchars_entered--;
                    if (reposition)
                    {
                        fairy_tail_index_large = fairy_prompt.rfind('\n', fairy_tail_index_large - 2) + 1;
                        fairy_tail_index_small = fairy_prompt.rfind('\n', fairy_tail_index_small - 2) + 1;
                    }
                }
                break;
            }

            while (int key = GetCharPressed())
            {
                //Sets which characters can be entered, so only normal letters and numbers, and not functional keys such as ESC
                if ((key >= 32) && (key <= 125))
                {
                    //update_prompt(prompt, key, font_size, max_text_width, tail_index_large,
                                 // tail_index_small, nchars_entered);
                    const char* psz = &fairy_prompt[fairy_prompt.rfind('\n') + 1];
                    std::cout << psz;
                    if ((char)key == ' ' && MeasureText(psz, font_size) > fairy_max_text_width)
                    {
                        fairy_prompt.push_back('\n');
                        fairy_tail_index_large = fairy_prompt.find('\n', fairy_tail_index_large) + 1;
                        fairy_tail_index_small = fairy_prompt.find('\n', fairy_tail_index_small) + 1;
                    }
                    else
                    {
                        fairy_prompt.push_back((char)key);
                    }

                    fairy_nchars_entered++;
                }
            }
        }
        else
        {
            //Changes the sprite and moves the character in the appropriate direction base on the characters input.
            knight.move(grey_vector, enemies, walls);
            //sword functionality
            sword_rect = {};
            if (attack(knight, last_sword, sword_rect, grey_vector, sword)) {
                sword_sound.Play();
                isSwordActive = true;
            }
            else {
                isSwordActive = false;
            }
           
            //Using the N key (Navi!) to detect when the player is speaking to the fairy
            if (IsKeyDown(KEY_N) && !reaper_display_text_box)
            {
                fairy_display_text_box = true;
                SetExitKey(0);
            }
            //Detects the player being close enough to the reaper to "collide"
            if (Vector2Distance(knight.get_pos(), reaper.get_posn()) < 30.0f)
            {
                //makes sure player is not already colliding with the reaper
                if (!reaper_collision)
                {
                    reaper_collision = true;
                    reaper_display_text_box = true;
                    SetExitKey(0);
                    coin_sound.Play();
                    music.SetVolume(music_volume_quiet);
                }
            }
            else
            {
                reaper_collision = false;
            }


            ////x1300-2300 y1100-1800 4th
            //Detects the player collecting a dimond and updates the dimonds collected variable.
            if (Vector2Distance(knight.get_pos(), dimond_gem.get_posn()) < 40.0f)
            {
                d_gem_x = randomFloat(50.0f, 1100.0f);
                d_gem_y = randomFloat(50.0f, 700.0f);
                d_gem_posn = { d_gem_x , d_gem_y };
                dimond_gem.set_posn(d_gem_posn);
                coin_sound.Play();
                diamond_collected++;
                gems_collected++;
            }

            //Detects the player collecting a dimond and updates the dimonds collected variable.
            if (Vector2Distance(knight.get_pos(), dimond2_gem.get_posn()) < 40.0f)
            {
                d2_gem_x = randomFloat(50.0f, 1100.0f);
                d2_gem_y = randomFloat(820.0f, 1400.0f);
                d2_gem_posn = { d2_gem_x , d2_gem_y };
                dimond2_gem.set_posn(d2_gem_posn);
                coin_sound.Play();
                diamond_collected++;
            }

            //Detects the player collecting a dimond and updates the dimonds collected variable.
            if (Vector2Distance(knight.get_pos(), dimond3_gem.get_posn()) < 40.0f)
            {
                d3_gem_x = randomFloat(1200.0f, 2300.0f);
                d3_gem_y = randomFloat(50.0f, 1000.0f);
                d3_gem_posn = { d3_gem_x , d3_gem_y };
                dimond3_gem.set_posn(d3_gem_posn);
                coin_sound.Play();
                diamond_collected++;
            }
            if (Vector2Distance(knight.get_pos(), dimond4_gem.get_posn()) < 40.0f)//x1300-2300 y1100-1800 4th
            {
                d4_gem_x = randomFloat(1300.0f, 2300.0f);
                d4_gem_y = randomFloat(1100.0f, 1800.0f);
                d4_gem_posn = { d4_gem_x , d4_gem_y };
                dimond4_gem.set_posn(d4_gem_posn);
                coin_sound.Play();
                diamond_collected++;
                gems_collected++;
            }

            //Detects the player collecting a emerald and updates the emeralds collected variable.
            if (Vector2Distance(knight.get_pos(), emerald_gem.get_posn()) < 40.0f)
            {
                e_gem_x = randomFloat(50.0f, 1100.0f);
                e_gem_y = randomFloat(50.0f, 700.0f);
                e_gem_posn = { e_gem_x , e_gem_y };
                emerald_gem.set_posn(e_gem_posn);
                coin_sound.Play();
                emerald_collected++;
                gems_collected++;
            }

            //Detects the player collecting a emerald and updates the emeralds collected variable.
            if (Vector2Distance(knight.get_pos(), emerald2_gem.get_posn()) < 40.0f)
            {
                e2_gem_x = randomFloat(50.0f, 1100.0f);
                e2_gem_y = randomFloat(820.0f, 1400.0f);
                e2_gem_posn = { e2_gem_x , e2_gem_y };
                emerald2_gem.set_posn(e2_gem_posn);
                coin_sound.Play();
                emerald_collected++;
            }

            //Detects the player collecting a dimond and updates the dimonds collected variable.
            if (Vector2Distance(knight.get_pos(), emerald3_gem.get_posn()) < 40.0f)
            {
                e3_gem_x = randomFloat(1200.0f, 2300.0f);
                e3_gem_y = randomFloat(50.0f, 1000.0f);
                e3_gem_posn = { e3_gem_x , d3_gem_y };
                emerald3_gem.set_posn(e3_gem_posn);
                coin_sound.Play();
                emerald_collected++;
            }
            if (Vector2Distance(knight.get_pos(), emerald4_gem.get_posn()) < 40.0f)//x1300-2300 y1100-1800 4th
            {
                e4_gem_x = randomFloat(1300.0f, 2300.0f);
                e4_gem_y = randomFloat(1100.0f, 1800.0f);
                e4_gem_posn = { e4_gem_x , e4_gem_y };
                emerald4_gem.set_posn(e4_gem_posn);
                coin_sound.Play();
                emerald_collected++;
                gems_collected++;
            }

            //Detects the player collecting a garnet and updates the garnets collected variable.
            if (Vector2Distance(knight.get_pos(), garnet_gem.get_posn()) < 40.0f)
            {
                g_gem_x = randomFloat(50.0f, 1100.0f);
                g_gem_y = randomFloat(50.0f, 700.0f);
                g_gem_posn = { g_gem_x , g_gem_y };
                garnet_gem.set_posn(g_gem_posn);
                coin_sound.Play();
                garnet_collected++;
                gems_collected++;
            }

            ////Detects the player collecting a garnet and updates the garnets collected variable.
            //if (Vector2Distance(knight.get_pos(), garnet2_gem.get_posn()) < 40.0f)
            //{
            //    g2_gem_x = randomFloat(50.0f, 1100.0f);
            //    g2_gem_y = randomFloat(820.0f, 1400.0f);
            //    g2_gem_posn = { g2_gem_x , g2_gem_y };
            //    garnet2_gem.set_posn(g2_gem_posn);
            //    coin_sound.Play();
            //    garnet_collected++;
            //}

            //Detects the player collecting a garnet and updates the garnets collected variable.
            if (Vector2Distance(knight.get_pos(), garnet3_gem.get_posn()) < 40.0f)
            {
                g3_gem_x = randomFloat(1200.0f, 2300.0f);
                g3_gem_y = randomFloat(50.0f, 1000.0f);
                g3_gem_posn = { g3_gem_x , g3_gem_y };
                garnet3_gem.set_posn(g3_gem_posn);
                coin_sound.Play();
                garnet_collected++;
            }
            if (Vector2Distance(knight.get_pos(), garnet4_gem.get_posn()) < 40.0f)//x1300-2300 y1100-1800 4th
            {
                g4_gem_x = randomFloat(1300.0f, 2300.0f);
                g4_gem_y = randomFloat(1100.0f, 1800.0f);
                g4_gem_posn = { g4_gem_x , g4_gem_y };
                garnet4_gem.set_posn(g4_gem_posn);
                coin_sound.Play();
                garnet_collected++;
                gems_collected++;
            }

        }


        //Converts the gems collected integer into a string that can be displayed
        std::string gem_string = "Total Score: " + std::to_string((diamond_collected * 10) + (emerald_collected * 5) + (garnet_collected * 5));
        std::string diamond_string = "Diamonds Collected: " + std::to_string(diamond_collected);
        std::string emerald_string = "Emeralds Collected: " + std::to_string(emerald_collected);
        std::string garnet_string = "Garnets Collected: " + std::to_string(garnet_collected);

        // Camera target follows player
        camera.target = Vector2{ knight.get_pos().x + 40, knight.get_pos().y + 40 };

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
        
         DrawTiled(map, 0, 0, WHITE);
      
        //Draws the characters and gems in the appropriate order for which is infront
        if (!(reaper_display_text_box || fairy_display_text_box)) {
            damage(knight, enemies, sword_rect, zombie_sound, isGameOver);
            damage(knight, enemies_bat, sword_rect, bat_sound, isGameOver);
        }
        for (int i = 0; i < enemies_bat.size(); i++) {
            (*enemies_bat.at(i)).follow(knight, 500, bat_vector, walls);
            (*enemies_bat.at(i)).draw();
            //DrawRectangle((*enemies.at(i)).calculate_rectangle().x, (*enemies.at(i)).calculate_rectangle().y, (*enemies.at(i)).calculate_rectangle().width, (*enemies.at(i)).calculate_rectangle().height ,BLACK);
        }
        for (int i = 0; i < enemies.size(); i++) {
            (*enemies.at(i)).follow(knight, 300, zombie_vector, walls);
            (*enemies.at(i)).draw();
            //DrawRectangle((*enemies.at(i)).calculate_rectangle().x, (*enemies.at(i)).calculate_rectangle().y, (*enemies.at(i)).calculate_rectangle().width, (*enemies.at(i)).calculate_rectangle().height ,BLACK);
        }
        fairy.follow(knight, 10000, fairy_vector, {});
        if (isSwordActive) sword.draw();
        knight.draw_health();

        std::vector<Sprite*> vsp{ knight.get_sprite(), &reaper, &dimond_gem, &emerald_gem, &garnet_gem, &dimond2_gem, &emerald2_gem, &garnet3_gem, &dimond3_gem, &emerald3_gem, &garnet4_gem, &dimond4_gem, &emerald4_gem, fairy.get_sprite() };//,&garnet3_gem
        std::sort(vsp.begin(), vsp.end(), [](Sprite* s1, Sprite* s2) {
            return s1->get_posn().y < s2->get_posn().y;
            }
        );

        for (const auto& s : vsp)
        {
            s->draw();
        }

        //Displays the text box for speaking to the fairy

        if (fairy_display_text_box && !reaper_display_text_box)
        {
            Color light_gray_transparent{ 80, 80, 80, 192 }; // 192/256 nontransparent
            DrawRectangleRec(*fairy_text_box, light_gray_transparent);
            unsigned int milliseconds = (unsigned int)(GetTime() * 1000.0);
            std::string s = &fairy_prompt[*fairy_tail_index];
            if ((milliseconds % 1000) > 500)
            {
                s.push_back('_');
            }
            std::string help_s{};
            int number_newlines = 0;
            //counts amount of new lines in string
            for (int i = 0; i < s.size(); i++)
            {
                if (s[i] == '\n')
                {
                    number_newlines++;
                }
            }
            if (number_newlines <= fairy_lines_of_text)
            {
                help_s = s;
            }
            else
            {
                for (int i = 0; i < fairy_lines_of_text; i++)
                {
                    if (s.find_last_of('\n'))
                    {
                        help_s = s.substr(s.find_last_of('\n'), s.size()) + help_s;
                        s.resize(s.find_last_of('\n'));
                    }
                }
            }
            if (help_s.at(0) == '\n')
            {
                help_s.erase(0, 1);
            }
            //std::cout << help_s;
            DrawText(help_s.c_str(), (*fairy_text_box).x + 12, (*fairy_text_box).y + 12, font_size, WHITE);
        }
        if (reaper_display_text_box && !fairy_display_text_box)
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
            int number_newlines = 0;
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
        
        DrawText(gem_string.c_str(), knight.get_pos().x - window.GetWidth() / 2 + 20, knight.get_pos().y - window.GetHeight() / 2 - 30, 20, BLACK);
        DrawText(diamond_string.c_str(), knight.get_pos().x - window.GetWidth() / 2 + 20, knight.get_pos().y - window.GetHeight() / 2 - 10, 20, BLACK);
        DrawText(emerald_string.c_str(), knight.get_pos().x - window.GetWidth() / 2 + 20, knight.get_pos().y - window.GetHeight() / 2 + 10, 20, BLACK);
        DrawText(garnet_string.c_str(), knight.get_pos().x - window.GetWidth() / 2 + 20, knight.get_pos().y - window.GetHeight() / 2 + 30, 20, BLACK);
        DrawText("N to talk to Navi", knight.get_pos().x - window.GetWidth() / 2 + 20, knight.get_pos().y - window.GetHeight() / 2 + 50, 20, BLACK);
        DrawText("Spacebar to attack", knight.get_pos().x - window.GetWidth() / 2 + 20, knight.get_pos().y - window.GetHeight() / 2 + 70, 20, BLACK);
        DrawText("arrow keys to move", knight.get_pos().x - window.GetWidth() / 2 + 20, knight.get_pos().y - window.GetHeight() / 2 + 90, 20, BLACK);

        //DrawLine((int)camera.target.x, -1280 * 10, (int)camera.target.x, 1280 * 10, GREEN);
        //DrawLine(-720 * 10, (int)camera.target.y, 720 * 10, (int)camera.target.y, GREEN);

        if (isGameOver) {
            DrawRectangle(knight.get_pos().x - window.GetWidth() / 2, knight.get_pos().y - window.GetHeight(),
                window.GetWidth(),  window.GetHeight() * 2, GRAY);
            DrawText("Game Over!", knight.get_pos().x - window.GetWidth() / 3, knight.get_pos().y - window.GetHeight() / 6, 120, BLACK);
        }
        //Draws map, for testing purposes
        //DrawTiled(map, 0, 0, WHITE);
        EndMode2D();

        EndDrawing();
    }
    //UnloadMap(map);
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
