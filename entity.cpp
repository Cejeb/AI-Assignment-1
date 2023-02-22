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
		void follow(entity entity, int distance, std::vector<Sprite> &vector) {
			if (get_pos().x + get_pos().y -
				(entity.get_pos().x + entity.get_pos().y) < distance) {
				if (abs(get_pos().x - entity.get_pos().x)
					>= abs(get_pos().y - entity.get_pos().y)) {

					if (get_pos().x - entity.get_pos().x >= 0) {

						set_pos({ get_pos().x - get_speed()
							, get_pos().y });
						set_sprite(vector.at(1));
					}
					else {
						set_pos({ get_pos().x + get_speed()
						   , get_pos().y });
						set_sprite(vector.at(2));
					}
				}
				else if (get_pos().y - entity.get_pos().y >= 0) {
					set_pos({ get_pos().x
										, get_pos().y - get_speed() });
					set_sprite(vector.at(3));
				}
				else {
					set_pos({ get_pos().x
										, get_pos().y + get_speed() });
					set_sprite(vector.at(0));
				}
				(*get_sprite()).set_posn(get_pos());
				//TO-DO: find out why animation causes a division by zero
				(*get_sprite()).set_animation(true);
			}
			else {
				(*get_sprite()).set_animation(false);
			}
		}
		Rectangle calculate_rectangle() {
			return { get_pos().x, get_pos().y, (float)(*get_sprite()).get_sprite_height(),(float)(*get_sprite()).get_sprite_width() };
		}
		void move(std::vector<Sprite> &sprite_vector) {
			if (IsKeyDown(KEY_DOWN))
			{
				set_pos({ get_pos().x 
						, get_pos().y + get_speed() });
				set_sprite(sprite_vector.at(0));
				(*get_sprite()).set_animation(true);
			}
			if (IsKeyDown(KEY_UP))
			{
				set_pos({ get_pos().x
				, get_pos().y - get_speed() });
				set_sprite(sprite_vector.at(3));
				(*get_sprite()).set_animation(true);
			}
			if (IsKeyDown(KEY_LEFT))
			{
				set_pos({ get_pos().x - get_speed()
				, get_pos().y  });
				set_sprite(sprite_vector.at(1));
				(*get_sprite()).set_animation(true);
			}
			if (IsKeyDown(KEY_RIGHT))
			{
				set_pos({ get_pos().x + get_speed()
				, get_pos().y });
				set_sprite(sprite_vector.at(2));
				(*get_sprite()).set_animation(true);
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