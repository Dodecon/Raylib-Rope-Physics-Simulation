#pragma once
#include "raylib.h"
#include "rlgl.h"
#include "Rope.h"

class RopeRenderer
{
public:
	RopeRenderer() = default;
	~RopeRenderer() = default;

	static void DrawSquaresBatched(const std::vector<Vector2>& positions, float size, Color color);
	static void RenderRopes(Camera2D& camera, Rope& ropes, std::vector<RopeNode>& nodes);

};
