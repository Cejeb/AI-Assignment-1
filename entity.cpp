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
		entity(Sprite* sprite, float hp, float speed, bool isHostile, int damage)
			: sprite_{ sprite },
			pos_{ (*sprite).get_posn()},
			hp_{ hp},
		speed_{ speed },
			isHostile_{isHostile},
			damage_{damage}
		{ }
		void follow(Sprite* sprite, int distance, std::vector<Sprite> &vector) {
			if (get_pos().x + get_pos().y -
				(sprite->get_posn().x + sprite->get_posn().y) < distance) {
				if (abs(get_pos().x - sprite->get_posn().x)
					>= abs(get_pos().y - sprite->get_posn().y)) {

					if (get_pos().x - sprite->get_posn().x >= 0) {

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
				else if (get_pos().y - sprite->get_posn().y >= 0) {
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

		
	};
}