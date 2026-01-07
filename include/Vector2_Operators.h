#pragma once
#include "raylib.h"
inline Vector2 operator +(Vector2& a, Vector2& b) {
	return Vector2{ a.x + b.x, a.y + b.y };
}
inline Vector2 operator -(Vector2& a, Vector2& b) {
	return Vector2{ a.x - b.x, a.y - b.y };
}
inline Vector2 operator *(Vector2& a, Vector2& b) {        // custom vector2 operators for this project
	return Vector2{ a.x * b.x, a.y * b.y };
}
inline Vector2 operator / (Vector2& a, Vector2& b) {
	return Vector2{ a.x / b.x, a.y / b.y };
}

inline Vector2 operator * (Vector2& a, float& scalar) {
	return Vector2{ a.x * scalar, a.y * scalar };
}
inline Vector2 operator * (float& scalar, Vector2& a) {
	return Vector2{ a.x * scalar, a.y * scalar };
}