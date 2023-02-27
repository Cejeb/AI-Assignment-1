#include <vector>
namespace aipfg {
	class entity {
	private:
		Sprite* sprite_{};
		Vector2 pos_{};
		//raylib::Texture tex_;
		float hp_{};
		float speed_{};
		bool isHostile_{};
		int damage_{};
		unsigned int lastdamage_{0};
	public:
		void set_sprite(Sprite& sprite) { sprite_ = &sprite; }
		Sprite* get_sprite() { return sprite_; }
		void set_pos(Vector2 pos) { pos_ = pos; }
		Vector2 get_pos() { return pos_; }
		void set_hp(float hp) { hp_ = hp; }
		//void set_tex(raylib::Texture tex) { tex_ = tex; }
		//raylib::Texture get_tex() { return tex_; }
		float get_hp() { return hp_; }
		void set_speed(float speed) { speed_ = speed; }
		float get_speed() { return speed_; }
		bool get_isHostile() { return isHostile_; }
		int get_damage() { return damage_; }
		void set_damage(int damage) { damage_ = damage; }
		void set_lastdamage(unsigned int lastdamage) { lastdamage_ = lastdamage; }
		unsigned int get_lastdamage() { return lastdamage_; }
		entity(Sprite* sprite, float hp, float speed, bool isHostile, int damage)
			: sprite_{ sprite },
			pos_{ (*sprite).get_posn()},
			hp_{ hp},
		speed_{ speed },
			isHostile_{isHostile},
			damage_{damage}
		{ }
		void follow(entity entity, int distance, std::vector<Sprite*> &vector, std::vector<Rectangle> walls) {
			Rectangle entityrect = entity.calculate_rectangle();
			Rectangle selfrect = calculate_rectangle();
			Vector2 entitycenter = { entityrect.x+ entityrect.width /2, entityrect.y + entityrect.height /2 };
			bool collision = false;
				if (CheckCollisionCircleRec(entitycenter, (float)distance, calculate_rectangle())){
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
								set_sprite((*vector.at(1)));
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
								set_sprite((*vector.at(2)));
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
							set_sprite((*vector.at(3)));
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
							set_sprite((*vector.at(0)));
						}
					
				}
				
				(*get_sprite()).set_posn(get_pos());
				(*get_sprite()).set_animation(true);
			}
			else {
				(*get_sprite()).set_posn(get_pos());
				(*get_sprite()).set_animation(false);
			}
			
		}

		Rectangle calculate_rectangle() {
			return { get_pos().x + 12, get_pos().y + 23,(float)(*get_sprite()).get_sprite_width()-26, (float)(*get_sprite()).get_sprite_height() - 23 };
		}
		void move(std::vector<Sprite> &sprite_vector, std::vector<entity*> enemies, std::vector<Rectangle> walls) {
			Rectangle selfrect = calculate_rectangle();

			if (IsKeyDown(KEY_DOWN))
			{
				selfrect.y += get_speed();
				bool collision = false;
				for (int i = 0; i < enemies.size(); i++) {
					Rectangle entityrect = (*enemies.at(i)).calculate_rectangle();
					if (CheckCollisionRecs(entityrect, selfrect)) {
						collision = true;
						break;
					}
				}
				for (int i = 0; i < walls.size(); i++) {
					if (CheckCollisionRecs(walls.at(i), selfrect)) {
						collision = true;
						break;
					}
				}
				set_sprite(sprite_vector.at(0));
				(*get_sprite()).set_animation(true);
				if (!collision) {
					set_pos({ get_pos().x
							, get_pos().y + get_speed() });
				}
			}
				
			
			if (IsKeyDown(KEY_UP))
			{
				selfrect.y -= get_speed();
				bool collision = false;
				for (int i = 0; i < enemies.size(); i++) {
					Rectangle entityrect = (*enemies.at(i)).calculate_rectangle();
					if (CheckCollisionRecs(entityrect, selfrect)) {
						collision = true;
						break;
					}
				}
				for (int i = 0; i < walls.size(); i++) {
					if (CheckCollisionRecs(walls.at(i), selfrect)) {
						collision = true;
						break;
					}
				}
				set_sprite(sprite_vector.at(3));
				(*get_sprite()).set_animation(true);
				if (!collision) {
						set_pos({ get_pos().x
						, get_pos().y - get_speed() });
					}
				}
			
			if (IsKeyDown(KEY_LEFT))
			{
				selfrect.x -= get_speed();
				bool collision = false;
				for (int i = 0; i < enemies.size(); i++) {
					Rectangle entityrect = (*enemies.at(i)).calculate_rectangle();
					if (CheckCollisionRecs(entityrect, selfrect)) {
						collision = true;
						break;
					}
				}
				for (int i = 0; i < walls.size(); i++) {
					if (CheckCollisionRecs(walls.at(i), selfrect)) {
						collision = true;
						break;
					}
				}
				set_sprite(sprite_vector.at(1));
				(*get_sprite()).set_animation(true);
				if (!collision) {
						set_pos({ get_pos().x - get_speed()
						, get_pos().y });
					}
				}
			
			if (IsKeyDown(KEY_RIGHT))
			{
				selfrect.x += get_speed();
				bool collision = false;
				for (int i = 0; i < enemies.size(); i++) {
					Rectangle entityrect = (*enemies.at(i)).calculate_rectangle();
					if (CheckCollisionRecs(entityrect, selfrect)) {
						collision = true;
						break;
					}
				}
				for (int i = 0; i < walls.size(); i++) {
					if (CheckCollisionRecs(walls.at(i), selfrect)) {
						collision = true;
						break;
					}
				}
				set_sprite(sprite_vector.at(2));
				(*get_sprite()).set_animation(true);
				if (!collision) {
						set_pos({ get_pos().x + get_speed()
						, get_pos().y });
					}
				}
			

			(*get_sprite()).set_posn(get_pos());
					}
		void draw_health() {
			int diff = (100 -get_sprite()->get_sprite_width()  ) / 2;
			DrawRectangle(get_pos().x -diff, get_pos().y , 100, 10, GRAY);
			if (get_isHostile())
				DrawRectangle(get_pos().x - diff, get_pos().y , get_hp(), 10, RED);
			else
				DrawRectangle(get_pos().x - diff, get_pos().y, get_hp(), 10, GREEN);
		}
		void draw(){
			(*get_sprite()).draw();
			draw_health();
		}
	};
}