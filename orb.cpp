#ifndef ORB_CPP
#define ORB_CPP

namespace aipfg {
	class orb {
	private:
		Sprite* sprite_{};
		Vector2 pos_{};
		float speed_{};
		int damage_{};
		Vector2 direction_{};
	public:
		void set_sprite(Sprite& sprite) { sprite_ = &sprite; }
		Sprite* get_sprite() { return sprite_; }
		void set_pos(Vector2 pos) { pos_ = pos; }
		Vector2 get_pos() { return pos_; }
		void set_speed(float speed) { speed_ = speed; }
		float get_speed() { return speed_; }
		int get_damage() { return damage_; }
		void set_damage(int damage) { damage_ = damage; }
		orb(Sprite* sprite, float speed, int damage, Vector2 direction, Vector2 pos) 
			: sprite_{ sprite },
				pos_{pos },
				speed_{ speed },
				damage_{ damage },
				direction_{direction}
			{ }
		
		void move() {
			set_pos({ get_pos().x + direction_.x * speed_, get_pos().y + direction_.y * speed_ });
			(*get_sprite()).set_posn(get_pos());
		}
		void draw() {
			(*get_sprite()).draw();
		}
		Vector2 get_pos_center() { return {get_pos().x + 16, get_pos().y + 16}; }
	};
}
#endif