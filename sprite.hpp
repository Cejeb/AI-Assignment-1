#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include "raylib-cpp.hpp"
#include <vector>

namespace aipfg {

struct Sprite
{
  Sprite(const raylib::Texture& tex,
         const int ncols, const int nrows, const Vector2 posn,
         const std::vector<int> frame_ids = { 0 }, const int sprite_fps = 0)
    : tex_{ tex },
      ncols_{ ncols },
      nrows_{ nrows },
      posn_{ posn },
      sprite_fps_{ sprite_fps },
      sprite_width_{ tex_.width / ncols_ },
      sprite_height_{ tex_.height / nrows_ },
      frame_ids_{ frame_ids },
      frame_rec_{ calc_frame_rect(frame_ids_[0]) },
      prev_time_{ GetTime() }
      { }

  void set_posn(const Vector2 posn) { posn_ = posn; }
  Vector2 get_posn() { return posn_; }
  void set_angle(const float angle) { angle_ = angle; }
  float get_angle() { return angle_; }
  void set_rotation(const float delta) { delta_ = delta; }
  float get_rotation() { return delta_; }
  void set_animation(bool on) { animation_on_ = on; }
  bool get_animation() { return animation_on_; }
  void set_origin(const Vector2 origin) { origin_ = origin; }
  Vector2 get_origin() { return origin_; }
  auto get_frame_ids_size() { return frame_ids_.size(); }

  void draw()
  {
    if (animation_on_)
    {
      update_animation();
    }
    const Rectangle dest{ posn_.x, posn_.y,
                          (float)sprite_width_, (float)sprite_height_ };
    Vector2 origin{ sprite_width_ * origin_.x, sprite_height_ * origin_.y };
    DrawTexturePro(tex_, frame_rec_, dest, origin, angle_ += delta_, WHITE);
  }
  void draw(const Vector2 posn) { set_posn(posn); draw(); }
  void draw_cell(const int x, const int y)
  {
    set_posn({(float)x * sprite_width_, (float)y * sprite_height_});
    draw();
  }
  void draw_cell(const int x, const int y, const int frame_id)
  {
    calc_frame_rect(frame_ids_[frame_id]);
    draw_cell(x, y);
  }

private:

  Rectangle calc_frame_rect(const int frame_id)
  {
    const int frame_x = frame_id % ncols_;
    const int frame_y = frame_id / ncols_;
    return { (float)frame_x * sprite_width_, (float)frame_y * sprite_height_,
             (float)sprite_width_, (float)sprite_height_ };
  }

  void update_animation()
  {
    const double t = GetTime();
    const double time_diff = t - prev_time_;
    const double one_frame = 1.0 / sprite_fps_;
    const int nframes = static_cast<int>(time_diff / one_frame);
    const int temp_frame = (frame_ + nframes) % frame_ids_.size(); // consider ALT+TAB

    frame_rec_ = calc_frame_rect(frame_ids_[temp_frame]);
    const double anim_cycle_dur = frame_ids_.size() / (double)sprite_fps_;
    if (time_diff > anim_cycle_dur)
    {
      frame_ = temp_frame;
      prev_time_ = t;
    }
  }

  const raylib::Texture& tex_;
  const int ncols_, nrows_;
  const int sprite_width_, sprite_height_;
  Vector2 posn_{ 0, 0 };
  float angle_{ 0 }, delta_{ 0 }; // delta is the change in rotation each frame
  Vector2 origin_{ 0.0f, 0.0f };  // normalised [0-1] "centre" of the sprite
  int sprite_fps_;
  double prev_time_;
  std::vector<int> frame_ids_; // flattened coordinate of each frame
  int frame_{ 0 }; // zero-based frame index for frame_ids_
  Rectangle frame_rec_;
  bool animation_on_{ false };
};

} // namespace aipfg

#endif // _SPRITE_HPP_
