export module core_util;

export import vec3;
export import color;
export import core_constructs;
export import image;
export import ray;

export import <SFML/Graphics.hpp>;

// Type check asserts.
static_assert(std::is_default_constructible<Vec3>::value, "Vec3 should be default constructible");
static_assert(std::is_copy_constructible<Vec3>::value, "Vec3 should be copy constructible");
static_assert(std::is_copy_assignable<Vec3>::value, "Vec3 should be copy assignable");
static_assert(std::is_move_constructible<Vec3>::value, "Vec3 should be move constructible");
static_assert(std::is_move_assignable<Vec3>::value, "Vec3 should be move assignable");
static_assert(std::is_destructible<Vec3>::value, "Vec3 should be destructible");

static_assert(std::is_default_constructible<Color>::value, "Color should be default constructible");
static_assert(std::is_copy_constructible<Color>::value, "Color should be copy constructible");
static_assert(std::is_copy_assignable<Color>::value, "Color should be copy assignable");
static_assert(std::is_move_constructible<Color>::value, "Color should be move constructible");
static_assert(std::is_move_assignable<Color>::value, "Color should be move assignable");
static_assert(std::is_destructible<Color>::value, "Color should be destructible");

static_assert(std::is_default_constructible<PNGImage>::value, "PNGImage should be default constructible");
static_assert(std::is_copy_constructible<PNGImage>::value, "PNGImage should be copy constructible");
static_assert(std::is_copy_assignable<PNGImage>::value, "PNGImage should be copy assignable");
static_assert(std::is_move_constructible<PNGImage>::value, "PNGImage should be move constructible");
static_assert(std::is_move_assignable<PNGImage>::value, "PNGImage should be move assignable");
static_assert(std::is_destructible<PNGImage>::value, "PNGImage should be destructible");

