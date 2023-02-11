#include "sprite.hpp"
#include "openai-helper.hpp"
#include "raylib-cpp.hpp"
#include <string>
#include <optional>
#include <vector>
#include <numeric> // std::iota
#include <iostream>

void update_prompt(std::string& prompt, const std::string& str, const int font_size,
    const float max_text_width, int& tail_index_large,
    int& tail_index_small, int& nchars_entered);

int main(int argc, char* argv[])
{
    using namespace aipfg;

    openai_helper oai_help;
    if (!oai_help.init())
    {
        return -1;
    }

    raylib::Window window(1024, 576, "Raylib OpenAI NPCs");

    SetTargetFPS(60);            // Set our game to run at 60 frames-per-second

    raylib::AudioDevice audio{}; // necessary: initialises the audio
    raylib::Sound coin_sound{ "../resources/audio/coin.wav" };
    raylib::Music music{ "../resources/audio/Magic-Clock-Shop.mp3" };
    float music_volume_normal = 1.0f, music_volume_quiet = 0.4f;
    music.Play();

    raylib::Texture tex1{ "../resources/time_fantasy/reaper_blade_3.png" };
    Sprite reaper{ tex1, 3, 4, { 340, 192 }, { 0 } };

    raylib::Texture tex2{ "../resources/time_fantasy/knights_3x.png" };
    int ncols = 12, nrows = 8;
    int id = 3;
    Vector2 grey_posn{ 40.0f, 100.0f };
    Sprite grey_down{ tex2, ncols, nrows, grey_posn, { id, id + 1, id + 2 }, 6 };
    id += ncols;
    Sprite grey_left{ tex2, ncols, nrows, grey_posn, { id, id + 1, id + 2 }, 6 };
    id += ncols;
    Sprite grey_right{ tex2, ncols, nrows, grey_posn, { id, id + 1, id + 2 }, 6 };
    id += ncols;
    Sprite grey_up{ tex2, ncols, nrows, grey_posn, { id, id + 1, id + 2 }, 6 };

    raylib::Texture tex3{ "../resources/time_fantasy"
                          "/tf_ashlands/3x_RMMV/tf_A5_ashlands_3.png" };
    ncols = 8; nrows = 16;
    std::vector<int> frame_ids(ncols * nrows);
    std::iota(frame_ids.begin(), frame_ids.end(), 0);
    Sprite all_ground_cells{ tex3, ncols, nrows, { 0, 0 }, frame_ids, 5 };
    all_ground_cells.set_animation(true);
    Sprite grnd1{ tex3, ncols, nrows, { 50, 300 }, { 1, 2, 3, 4 } };

    Sprite* grey_knight = &grey_right;
    const float grey_speed = 2.5f;

    bool reaper_collision = false; // currently colliding with the reaper?
    const std::string reaper_nature =
        "The following is a conversation with the grim reaper. The grim reaper is a "
        "personified force. In some mythologies, the grim reaper causes the "
        "victim's death by coming to collect that person's soul.\n\n";

    const int font_size = 30; // n.b. "spacing" varies with the font & font size
    //bool display_text_box = true; SetExitKey(0); // debug
    bool display_text_box = false;
    const float border = 20;
    const float jump_fix = 4;
    const float box_width = (float)window.GetWidth() - (2 * border);
    const float average_word_size = MeasureText("Abcdefg", font_size);
    const float max_text_width = box_width - average_word_size;
    const float box_ypos = (float)window.GetHeight() - 200;
    const float box_height_small = (float)window.GetHeight() - box_ypos - border;
    const float box_height_large = (float)window.GetHeight() - (2 * border) + jump_fix;
    Rectangle text_box_small{ border, box_ypos, box_width, box_height_small };
    Rectangle text_box_large{ border, border - jump_fix,   box_width, box_height_large };
    Rectangle* text_box = &text_box_small;

    const std::string human_stop = "Human: ";
    const std::string reaper_stop = "Grim Reaper: ";
    const std::string new_lines = "\n\n\n\n\n\n\n\n\n"; // 9
    std::string reaper_prompt = new_lines + reaper_stop +
        "Why are you here?\n" + human_stop;
    int tail_index_large = 0;
    int tail_index_small = reaper_prompt.rfind('\n');
    tail_index_small = reaper_prompt.rfind('\n', tail_index_small - 1);
    int* tail_index = &tail_index_small;
    int nchars_entered = 0;

    while (!window.ShouldClose()) // Detect window close button or ESC key
    {
        music.Update();

        (*grey_knight).set_animation(false);

        if (display_text_box)
        {
            switch (GetKeyPressed())
            {
            case KEY_ESCAPE:
                if (text_box == &text_box_large) // esc resumes the small text box
                {
                    text_box = &text_box_small;
                    tail_index = &tail_index_small;
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
                std::string response_str{};
                update_prompt(reaper_prompt, '\n' + reaper_stop, font_size, max_text_width,
                    tail_index_large, tail_index_small, nchars_entered);
                const auto stop = std::optional{ std::vector{human_stop, reaper_stop} };
                oai_help.submit(reaper_nature + reaper_prompt, response_str, stop);
                for (auto& c : response_str)
                {
                    c = (c == '\n') ? ' ' : c; // replace newlines with spaces
                }
                response_str += '\n' + human_stop;
                update_prompt(reaper_prompt, response_str, font_size, max_text_width,
                    tail_index_large, tail_index_small, nchars_entered);
                nchars_entered = 0;
            }
            break;

            case KEY_UP:
                text_box = &text_box_large;
                tail_index = &tail_index_large;
                break;

            case KEY_BACKSPACE:
                if (nchars_entered > 0)
                {
                    bool reposition = reaper_prompt.back() == '\n'; // last char is newline
                    reaper_prompt.pop_back();
                    nchars_entered--;
                    if (reposition)
                    {
                        tail_index_large = reaper_prompt.rfind('\n', tail_index_large - 2) + 1;
                        tail_index_small = reaper_prompt.rfind('\n', tail_index_small - 2) + 1;
                    }
                }
                break;
            }

            while (int key = GetCharPressed())
            {
                if ((key >= 32) && (key <= 125)) // e.g. don't grab the ESC key
                {
                    update_prompt(reaper_prompt, std::string{ (char)key }, font_size, max_text_width,
                        tail_index_large, tail_index_small, nchars_entered);
                }
            }
        }
        else
        {
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

            if (Vector2Distance(grey_posn, reaper.get_posn()) < 30.0f)
            {
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
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        all_ground_cells.draw_cell(0, 0);
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                grnd1.draw_cell(2 + i, 2 + j, i % grnd1.get_frame_ids_size());
            }
        }

        // Drawn from back (-ve y coord) to front (+ve y coord)
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

        if (display_text_box)
        {
            Color light_gray_transparent{ 80, 80, 80, 192 }; // 192/256 nontransparent
            DrawRectangleRec(*text_box, light_gray_transparent);
            unsigned int milliseconds = (unsigned int)(GetTime() * 1000.0);
            std::string s = &reaper_prompt[*tail_index];
            if ((milliseconds % 1000) > 500)
            {
                s.push_back('_');
            }
            DrawText(s.c_str(), (*text_box).x + 12, (*text_box).y + 12, font_size, WHITE);
        }

        EndDrawing();
    }

    return 0;
}

void update_prompt(std::string& prompt, const std::string& str, const int font_size,
    const float max_text_width, int& tail_index_large,
    int& tail_index_small, int& nchars_entered)
{
    for (char c : str)
    {
        const char* psz = &prompt[prompt.rfind('\n') + 1];

        if (c == ' ' && MeasureText(psz, font_size) > max_text_width)
        {
            c = '\n';
        }

        if (c == '\n')
        {
            tail_index_large = prompt.find('\n', tail_index_large) + 1;
            tail_index_small = prompt.find('\n', tail_index_small) + 1;
        }

        prompt.push_back(c);
        nchars_entered++;
    }
}
