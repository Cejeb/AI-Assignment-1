#include <string>
#include <future>
#include "openai-helper.hpp"
#include <iostream>
#include <vector>
#include <chrono>
namespace aipfg {
	class textbox {
	private:
        std::string new_lines{};
        std::string prompt_{};
        const float jump_fix{ 4 };
        float box_width{};
		float box_ypos{};
		float box_height_small{};
		float box_height_large{};
		Rectangle text_box_small_{};
		Rectangle text_box_large_{};
		float average_word_size{};
		float max_text_width_{};
		float tail_index_small_{};
        float tail_index_large_{};
		const std::string human_stop_str = "Human: ";
		const float border_ = 20;
		Rectangle* text_box{};
        const int font_size_ = 30;
        float* tail_index_{};
        int nchars_entered_{};
        openai_helper* oai_help_{};
        std::string name_str{};
        bool active_{ false };
        int windowx{};
        int windowy{};
        int lines_of_text_small{};
        int lines_of_text_large{};
        int lines_of_text{};
        std::future<void> response_future_{};
        std::string response_{};
        std::string nature_;
        std::string nature_prompt_;
        bool custom_prompt_done_{ false };
        const std::optional<std::vector<std::string>> stop_ = std::optional{ std::vector{name_str, human_stop_str} };

	public:
		textbox(raylib::Window& window, std::string nature, std::string gambit, std::string name, openai_helper &oai_help) {
			new_lines = "\n\n\n\n\n\n\n\n\n";
            nature_ = nature;
			prompt_ = new_lines + name + gambit + '\n' + human_stop_str;
            windowx = window.GetWidth();
            windowy = window.GetHeight();
			box_width = (float)windowx - (2 * border_);
			box_ypos = (float)windowy - 200;
			box_height_large = (float)windowy - (2 * border_);
			box_height_small = (float)windowy - box_ypos - border_;
			text_box_small_= { border_, box_ypos, box_width, box_height_small };
			text_box_large_= { border_, border_,   box_width, box_height_large };
			text_box = &text_box_small_;
			average_word_size = MeasureText("Abcdefg", font_size_);
			max_text_width_ = box_width - average_word_size;
			tail_index_small_ = prompt_.rfind('\n');
            tail_index_large_ = prompt_.rfind('\n', tail_index_small_ - 1);
            tail_index_ = &tail_index_small_;
            nchars_entered_ = 0;
            oai_help_ = &oai_help;
            name_str = name;
            lines_of_text_small = box_height_small / (30 + 15);
            lines_of_text_large = box_height_large / (30 + 15);
            lines_of_text = lines_of_text_small;
            if (response_future_.valid()) {
                auto text = "std::future has no copy constructor."
                    "We can only handle the member when in an invalid state.";
                throw std::runtime_error{ text };
            }
		}
		void update(Vector2 pos, std::string custom_prompt_, raylib::Sound& music) {

            bool waiting{ response_future_.valid() };
            if (waiting &&
                std::future_status::ready ==
                response_future_.wait_for(std::chrono::seconds(0)))
            {
                response_future_.get(); // invalidates the future
                for (auto& c : response_)
                {
                    c = (c == '\n') ? ' ' : c; // replace newlines with spaces
                }
                response_ += '\n' + human_stop_str;
                update_prompt(response_);
                nchars_entered_ = 0;
            }
            if (!custom_prompt_.empty() && !custom_prompt_done_) {
                if (!waiting)
                {
                    prompt_ += custom_prompt_;
                    update_prompt('\n' + name_str);
                    nature_prompt_ = nature_ + prompt_;
                    response_future_ = std::async(std::launch::async, &openai_helper::submit, std::ref((*oai_help_)),
                        std::cref(nature_prompt_), std::ref(response_), std::cref(stop_));
                    custom_prompt_done_ = true;

                }
            }
            static bool once;
            if (!once) {
                    text_box_small_.y = pos.y + windowy * 0.30;
                    text_box_small_.x = pos.x - windowx * 0.51;
                    text_box_large_.x = text_box_small_.x;
                    text_box_large_.y = text_box_small_.y - windowy * 0.72;
                    if (text_box = &text_box_small_) {
                        text_box = &text_box_small_;
                    }
                    else
                    {
                        text_box = &text_box_large_;
                    }
                once = true;
            }
            switch (GetKeyPressed())
            {
            case KEY_ESCAPE:
                if (text_box == &text_box_large_)
                {
                    text_box = &text_box_small_;
                    tail_index_ = &tail_index_small_;
                    lines_of_text = lines_of_text_small;
                    
                }
                else
                {
                    SetExitKey(KEY_ESCAPE);
                    once = false;
                    active_ = false;
                    music.SetVolume(1.0f);
                }
                break;

            case KEY_ENTER:
            {
                if (waiting)
                {
                    break;
                }
                update_prompt('\n' + name_str);
                nature_prompt_ = nature_ + prompt_;
                response_future_ = std::async(std::launch::async, &openai_helper::submit, std::ref((*oai_help_)),
                    std::cref(nature_prompt_), std::ref(response_), std::cref(stop_));

            }
            break;

            //Makes the text box larger so the player can more easily read their chat with the reaper
            case KEY_UP:
                text_box = &text_box_large_;
                tail_index_ = &tail_index_large_;
                lines_of_text = lines_of_text_large;
                break;

                //Allows the player to delete text from their current entry with the reaper.
            case KEY_BACKSPACE:
                if (waiting)
                {
                    break;
                }
                if (nchars_entered_ > 0)
                {
                    bool reposition = prompt_.back() == '\n'; // last char is newline
                    prompt_.pop_back();
                    nchars_entered_--;
                    if (reposition)
                    {
                        tail_index_large_ = prompt_.rfind('\n', tail_index_large_ - 2) + 1;
                        tail_index_small_ = prompt_.rfind('\n', tail_index_small_ - 2) + 1;
                    }
                }
                break;
            }

            while (int key = GetCharPressed())
            {
                if (waiting)
                {
                    continue;
                }
                //Sets which characters can be entered, so only normal letters and numbers, and not functional keys such as ESC
                if ((key >= 32) && (key <= 125)) // e.g. don't grab the ESC key
                {
                    update_prompt(std::string{ (char)key });
                }
            }
        }
        void update_prompt(const std::string& str) {
            for (char c : str)
            {
                const char* psz = &prompt_[prompt_.rfind('\n') + 1];

                if (c == ' ' && MeasureText(psz, font_size_) > max_text_width_)
                {
                    c = '\n';
                }

                if (c == '\n')
                {
                    tail_index_large_ = prompt_.find('\n', tail_index_large_) + 1;
                    tail_index_small_ = prompt_.find('\n', tail_index_small_) + 1;
                }

                prompt_.push_back(c);
                nchars_entered_++;
            }
        }
        void draw()
        {
            if (active_) {
                Color light_gray_transparent{ 80, 80, 80, 192 }; // 192/256 nontransparent
                DrawRectangleRec(*text_box, light_gray_transparent);
                unsigned int milliseconds = (unsigned int)(GetTime() * 1000.0);
                //std::string s = &prompt_[*tail_index_];
                std::string s = prompt_;
                std::string help_s;
                if ((milliseconds % 1000) > 500)
                {
                    s.push_back('_');
                }
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
                DrawText(help_s.c_str(), (*text_box).x + 12, (*text_box).y + 12, font_size_, WHITE);
            }
        }
        bool getActive() {
            return active_;
        }
        void setActive(bool active) {
            active_ = active;
        }
	};


}