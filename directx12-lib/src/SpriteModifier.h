#pragma once
#include <concepts>

template <class T>
concept Modifier = requires(T & obj)
{
    {obj.init()} -> std::convertible_to<void>;
    {obj.update()} -> std::convertible_to<void>;
};

template <Modifier T>
class SpriteModifier
{
public:
	SpriteModifier(T mod) : mod(mod) {}

	void init() { mod.init(); }
	void update() { mod.update(); }
private:
	T mod;
};
