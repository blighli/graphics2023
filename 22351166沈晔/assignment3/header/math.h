#pragma once

#include "vulkan/vulkan.hpp"
#include <initializer_list>

namespace app {

// struct Vec final {
//   union {
//     struct {
//       float x, y;
//     };
//     struct {
//       float w, h;
//     };
//   };

//   static auto GetAttributeDescription()
//       ->
//       std::vector<vk::VertexInputAttributeDescription>;
//   static auto GetBindingDescription()
//       -> std::vector<vk::VertexInputBindingDescription>;
// };

// struct Vertex final {
//   Vec position;
//   Vec texcoord;
// };

// struct Color final {
//   float r, g, b;
// };

// using Size = Vec;

// class Mat4 {
// public:
//   static auto CreateIdentity() -> Mat4;
//   static auto CreateOnes() -> Mat4;
//   static auto CreateOrtho(int left, int right, int top,
//   int bottom, int near,
//                           int far) -> Mat4;
//   static auto CreateTranslate(const Vec &) -> Mat4;
//   static auto CreateScale(const Vec &) -> Mat4;
//   static auto Create(const std::initializer_list<float>
//   &) -> Mat4;

//   Mat4();
//   [[nodiscard]] auto GetData() const -> const float * {
//   return data_; } void Set(int x, int y, float value) {
//   data_[x * 4 + y] = value; }
//   [[nodiscard]] auto Get(int x, int y) const -> float {
//     return data_[x * 4 + y];
//   }

//   [[nodiscard]] auto Mul(const Mat4 &m) const -> Mat4;

// private:
//   float data_[4 * 4];
// };

// struct Rect {
//   Vec position;
//   Size size;
// };

} // namespace app