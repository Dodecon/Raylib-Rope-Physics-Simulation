#include "RopeRenderer.h"

void RopeRenderer::DrawSquaresBatched(const std::vector<Vector2>& positions, float size, Color color) {
	if (positions.empty()) return;

	float half = size * 0.5f;

	rlSetTexture(0); // Use default white texture (color-only)
	rlBegin(RL_QUADS);
	rlColor4ub(color.r, color.g, color.b, color.a);

	for (const Vector2& pos : positions) {
		// Draw centered quad at pos
		rlVertex2f(pos.x - half, pos.y - half); // Top-left
		rlVertex2f(pos.x - half, pos.y + half); // Bottom-left
		rlVertex2f(pos.x + half, pos.y + half); // Bottom-right
		rlVertex2f(pos.x + half, pos.y - half); // Top-right
	}

	rlEnd();
}

void RopeRenderer::RenderRope(Camera2D& camera, Rope& rope) {

	if (rope.NodeAmount < 2) return;

	// lambda to check if a node is visible
	auto isVisible = [&](int i) {
		Vector2 nodeScreenPos = GetWorldToScreen2D(rope.Positions[i], camera);
		float nodeRadiusscreen = rope.Radius * camera.zoom;
		return (nodeScreenPos.x + nodeRadiusscreen >= 0 && nodeScreenPos.x - nodeRadiusscreen <= GetScreenWidth() &&
			nodeScreenPos.y + nodeRadiusscreen >= 0 && nodeScreenPos.y - nodeRadiusscreen <= GetScreenHeight());
	};

	// all visible nodes
	std::vector<Vector2> batch;
	batch.reserve(rope.NodeAmount);

	for (int i = 0; i < rope.NodeAmount; i++) {

		if (isVisible(i))  {

			// If entering screen from outside: include previous off-screen node to avoid gaps
			if (batch.empty() && i > 0) batch.push_back(rope.Positions[i - 1]); // Entering

			// // Add other visible node to batch
			batch.push_back(rope.Positions[i]);

		}
		else if (!batch.empty()) {

			//if the node is invisible and we're not at the start, add the last node, draw the rope, clear the batch
			batch.push_back(rope.Positions[i]); // Exiting
			DrawSplineLinear(batch.data(), batch.size(), rope.Radius, RED);
			DrawSquaresBatched(batch, rope.Radius * 2.0f, GREEN);
			batch.clear();
		}
	}

	// draw any remaining nodes
	if (!batch.empty()) {

		DrawSplineLinear(batch.data(), batch.size(), rope.Radius, RED);
		DrawSquaresBatched(batch, rope.Radius * 2.0f, GREEN);
	}
}