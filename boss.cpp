#define _USE_MATH_DEFINES
#include <math.h>
#define ZERO_OFFSET   0
#define AMPLITUDE    0.5 

enum STAGE {
	STAGE1, STAGE2, STAGE3
};

namespace aipfg {
	class boss : public entity {
		using entity::entity;
	private:
		float maxhp{ hp_ };
		float offset;
		float t = 0;
		float starty = get_pos().y;
		STAGE currentStage{ STAGE1 };
	public:
		void hover() {
			offset = AMPLITUDE * sin(2 * M_PI * 0.15 * t + 0) + ZERO_OFFSET;
			set_pos({ get_pos().x, get_pos().y + offset });
			get_sprite()->set_posn(get_pos());
			t = t + 0.02;
		}
		void changeStage() {
			if (hp_ < 0.7 * maxhp) {
				currentStage = STAGE2;
				set_speed(0.6 * get_speed());
			}
			if (hp_ <= 0.3 * maxhp) {
				currentStage = STAGE3;
				set_speed(0.8 * get_speed());
			}

		}
		void draw_health() {
			int diff = (maxhp - get_sprite()->get_sprite_width()) / 2;
			DrawRectangle(get_pos().x - diff, get_pos().y - 25, maxhp, 25, GRAY);
			DrawRectangle(get_pos().x - diff, get_pos().y - 25, get_hp(), 25, DARKPURPLE);
			DrawText("Ophidian, the Orb Guardian", get_pos().x, get_pos().y - 50, 20, BLACK);

		}
		void draw() {
			(*get_sprite()).draw();
			draw_health();
		}
		Rectangle calculate_rectangle() {
			if (currentStage == STAGE1) {
				return { get_pos().x + 35, get_pos().y + 15,(float)(*get_sprite()).get_sprite_width() - 85, (float)(*get_sprite()).get_sprite_height() - 28 };
			}
			else if (currentStage == STAGE2) {

			}
			else {
				return { get_pos().x + 12, get_pos().y + 23,(float)(*get_sprite()).get_sprite_width() - 26, (float)(*get_sprite()).get_sprite_height() - 23 };
			}
		}
		
	};
}