export module core_util;

import <type_traits>;

export import vec3;
export import color;
export import core_constructs;
export import image;
export import ray;
export import camera; 
export import renderer;
export import timer;
export import indus;

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

static_assert(std::is_default_constructible<Ray>::value, "Ray should be default constructible");
static_assert(std::is_copy_constructible<Ray>::value, "Ray should be copy constructible");
static_assert(std::is_copy_assignable<Ray>::value, "Ray should be copy assignable");
static_assert(std::is_move_constructible<Ray>::value, "Ray should be move constructible");
static_assert(std::is_move_assignable<Ray>::value, "Ray should be move assignable");
static_assert(std::is_destructible<Ray>::value, "Ray should be destructible");

static_assert(std::is_default_constructible<Camera>::value, "Camera should be default constructible");
static_assert(std::is_copy_constructible<Camera>::value, "Camera should be copy constructible");
static_assert(std::is_copy_assignable<Camera>::value, "Camera should be copy assignable");
static_assert(std::is_move_constructible<Camera>::value, "Camera should be move constructible");
static_assert(std::is_move_assignable<Camera>::value, "Camera should be move assignable");
static_assert(std::is_destructible<Camera>::value, "Camera should be destructible");

static_assert(std::is_default_constructible<Renderer>::value, "Renderer should be default constructible");
//static_assert(std::is_copy_constructible<Renderer>::value, "Renderer should be copy constructible");
//static_assert(std::is_copy_assignable<Renderer>::value, "Renderer should be copy assignable");
//static_assert(std::is_move_constructible<Renderer>::value, "Renderer should be move constructible");
//static_assert(std::is_move_assignable<Renderer>::value, "Renderer should be move assignable");
static_assert(std::is_destructible<Renderer>::value, "Renderer should be destructible");

//static_assert(std::is_default_constructible<Timer>::value, "Timer should be default constructible");
//static_assert(std::is_copy_constructible<Timer>::value, "Timer should be copy constructible");
//static_assert(std::is_copy_assignable<Timer>::value, "Timer should be copy assignable");
//static_assert(std::is_move_constructible<Timer>::value, "Timer should be move constructible");
//static_assert(std::is_move_assignable<Timer>::value, "Timer should be move assignable");
//static_assert(std::is_destructible<Timer>::value, "Timer should be destructible");





