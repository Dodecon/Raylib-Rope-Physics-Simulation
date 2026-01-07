#pragma once
#include<vector>
#include "raylib.h"
#include "RopeNode.h"

void CameraMove(Camera2D& mainCamera);
void MoveRopeNode(std::vector<RopeNode>& ropenodes, const Camera2D& mainCamera);
void DrawFixedBackground(Texture2D background, int screenWidth, int screenHeight);