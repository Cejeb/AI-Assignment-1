namespace aipfg {
	class entity {
	private:
		Sprite* sprite_;
		Vector2 pos_;
		//raylib::Texture tex_;
		float hp_;
		float speed_;
	public:
		//void set_sprite(Sprite sprite) { sprite_ = sprite; }
		Sprite get_sprite() { return *sprite_; }
		void set_pos(Vector2 pos) { pos_ = pos; }
		Vector2 get_pos() { return pos_; }
		void set_hp(float hp) { hp_ = hp; }
		//void set_tex(raylib::Texture tex) { tex_ = tex; }
		//raylib::Texture get_tex() { return tex_; }
		float get_hp() { return hp_; }
		void set_speed(float speed) { speed_ = speed; }
		float get_speed() { return speed_; }
		entity(Sprite* sprite, Vector2 pos, float hp, float speed)
			: sprite_{ sprite },
			pos_{ pos },
			hp_{ hp},
		speed_{ speed }
		{}

		
	};
}