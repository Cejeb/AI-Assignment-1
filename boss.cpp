#define _USE_MATH_DEFINES
#include <math.h>
#define ZERO_OFFSET   0
#define AMPLITUDE    0.5 
#include "orb.cpp"
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
		int lastspawn{};
	public:
		void hover() {
			offset = AMPLITUDE * sin(2 * M_PI * 0.15 * t + 0) + ZERO_OFFSET;
			set_pos({ get_pos().x, get_pos().y + offset });
			get_sprite()->set_posn(get_pos());
			t = t + 0.02;
		}
		void changeStage(std::vector<Sprite>& vector) {
			if (hp_ < 0.7 * maxhp) {
				currentStage = STAGE2;
				set_speed(0.6 * get_speed());
				set_sprite(vector.at(2));
			}
			if (hp_ <= 0.3 * maxhp) {
				currentStage = STAGE3;
				set_speed(0.8 * get_speed());
				set_sprite(vector.at(4));
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
				return { get_pos().x + 35, get_pos().y + 15,(float)(*get_sprite()).get_sprite_width() - 85, (float)(*get_sprite()).get_sprite_height() - 28 };
			}
			else {
				return { get_pos().x + 35, get_pos().y + 15,(float)(*get_sprite()).get_sprite_width() - 85, (float)(*get_sprite()).get_sprite_height() - 28 };
			}
		}
		void follow(entity entity, int distance, std::vector<Sprite>& vector, std::vector<Rectangle> walls) {
			Rectangle entityrect = entity.calculate_rectangle();
			Rectangle selfrect = calculate_rectangle();
			Vector2 entitycenter = { entityrect.x + entityrect.width / 2, entityrect.y + entityrect.height / 2 };
			bool collision = false;
			if (CheckCollisionCircleRec(entitycenter, (float)distance, calculate_rectangle())) {
				if (abs(get_pos().x - entity.get_pos().x)
					>= abs(get_pos().y - entity.get_pos().y)) {

					if (get_pos().x - entity.get_pos().x >= 0) {
						selfrect.x -= get_speed();
						for (int i = 0; i < walls.size(); i++) {
							if (CheckCollisionRecs(walls.at(i), selfrect)) {
								collision = true;
							}
						}
						if (!CheckCollisionRecs(entityrect, selfrect) && !collision) {
							set_pos({ get_pos().x - get_speed()
								, get_pos().y });
							if (currentStage == STAGE1) {
								set_sprite(vector.at(1));
							}
							else if (currentStage == STAGE2) {
								set_sprite(vector.at(3));
							}
							else {
								set_sprite(vector.at(5));
							}
							
						}
					}
					else {
						selfrect.x += get_speed();
						for (int i = 0; i < walls.size(); i++) {
							if (CheckCollisionRecs(walls.at(i), selfrect)) {
								collision = true;
							}
						}
						if (!CheckCollisionRecs(entityrect, selfrect) && !collision) {
							set_pos({ get_pos().x + get_speed()
							   , get_pos().y });
							if (currentStage == STAGE1) {
								set_sprite(vector.at(0));
							}
							else if (currentStage == STAGE2) {
								set_sprite(vector.at(2));
							}
							else {
								set_sprite(vector.at(4));
							}
						}
					}

				}
				else if (get_pos().y - entity.get_pos().y >= 0) {
					selfrect.y -= get_speed();
					for (int i = 0; i < walls.size(); i++) {
						if (CheckCollisionRecs(walls.at(i), selfrect)) {
							collision = true;
						}
					}
					if (!CheckCollisionRecs(entityrect, selfrect) && !collision) {
						set_pos({ get_pos().x
											, get_pos().y - get_speed() });
					}

				}
				else {
					selfrect.y += get_speed();
					for (int i = 0; i < walls.size(); i++) {
						if (CheckCollisionRecs(walls.at(i), selfrect)) {
							collision = true;
						}
					}
					if (!CheckCollisionRecs(entityrect, selfrect) && !collision) {
						set_pos({ get_pos().x
											, get_pos().y + get_speed() });
					}

				}
			}
			(*get_sprite()).set_posn(get_pos());
		}
		void attack(entity knight, std::vector<orb*>& vector, std::vector<Rectangle> walls, std::vector <Sprite>& orb_sprite, raylib::Sound& attacksound){
			int spawntime = 3000;
			Sprite* currentSprite= (&orb_sprite.at(0));
			int orbspeed = 3;
			int orbdamage = damage_;
			switch (currentStage) {
			case STAGE1:
				break;
			case STAGE2:
				spawntime = 1500;
				currentSprite = (&orb_sprite.at(1));
				orbspeed = 5;
				orbdamage = 1.5 * damage_;
				break;
			case STAGE3:
				spawntime = 750;
				currentSprite = (&orb_sprite.at(2));
				orbspeed = 6;
				orbdamage = 2 * damage_;
				break;
			}
			if ((unsigned int)(GetTime() * 1000.0) - lastspawn >= spawntime) {
				Vector2 unit_vector = {knight.get_pos().x+ knight.calculate_rectangle().width/2 - (get_pos().x + calculate_rectangle().width / 2)
					, knight.get_pos().y + knight.calculate_rectangle().height / 2 - (get_pos().y + calculate_rectangle().height / 2)};
				double magnitude = sqrt(pow(unit_vector.x, 2) + pow(unit_vector.y, 2));
				unit_vector.x = unit_vector.x * (1 / magnitude);
				unit_vector.y = unit_vector.y * (1 / magnitude);
				vector.push_back(new orb(currentSprite, orbspeed, orbdamage, unit_vector, { get_pos().x + calculate_rectangle().width /2, get_pos().y + calculate_rectangle().height /2}));
				attacksound.Play();
				lastspawn = (unsigned int)(GetTime() * 1000.0);
			}
		}
	};
}