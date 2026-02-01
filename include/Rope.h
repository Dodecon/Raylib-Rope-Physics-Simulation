#pragma once
#include <vector>
#include "raymath.h"

// stores all nodes and other data of a rope
struct Rope
{
private:

	struct alignas(32) Node {
		Vector2 Position;
		Vector2 OldPosition;
		Vector2 Acceleration;
		float IsAnchored;
	};

	// all data of all nodes are stored in a single buffer
	std::vector<Node>DataBuffer;


		// Proxy struct for easy access to DataBuffer's data
		template <typename Member, typename MemberType>
		struct MemberAccessor {

			std::vector<Member>& data;
			MemberType Member::* memberPtr;	// pointer to the member's data

			MemberAccessor(std::vector<Member>& vec, MemberType Member::* member) : data(vec), memberPtr(member) {}


			// overload the [] opeartor to allow easy access to data
			MemberType& operator[] (int index) {

				return data[index].*memberPtr;
			}
			//the same for const
			const MemberType& operator[] (int index) const {

				return data[index].*memberPtr;
			}
		};

public:

	// data thats stored per rope
	int NodeAmount;
	float TargetLength;	// max length between 2 nodes, NOT an entire rope
	float Radius;  // radius of eaach node (for rendering, damping, collisions in the future)

	// objects to access node data
	MemberAccessor<Node, Vector2> Positions;
	MemberAccessor<Node, Vector2> OldPositions;
	MemberAccessor<Node, Vector2> Accelerations;
	MemberAccessor<Node, float> IsAnchored;


	// create an empty rope of known size
	Rope(int nodeAmount, float radius, float targetLength)

		:
		NodeAmount(nodeAmount),
		Radius(radius),
		TargetLength(targetLength),

		//setup rope data
		DataBuffer(nodeAmount),
		Positions(DataBuffer, &Node::Position),
		OldPositions(DataBuffer, &Node::OldPosition),
		Accelerations(DataBuffer, &Node::Acceleration),
		IsAnchored(DataBuffer, &Node::IsAnchored)
	{};

	// copy constructor
	Rope(const Rope& other)
		:
		NodeAmount(other.NodeAmount),
		Radius(other.Radius),
		TargetLength(other.TargetLength),

		//setup rope data
		DataBuffer(other.DataBuffer),
		Positions(DataBuffer, &Node::Position),
		OldPositions(DataBuffer, &Node::OldPosition),
		Accelerations(DataBuffer, &Node::Acceleration),
		IsAnchored(DataBuffer, &Node::IsAnchored)
	{
	};

	//move constructor
	Rope(Rope&& other) noexcept
		:
		NodeAmount(other.NodeAmount),
		Radius(other.Radius),
		TargetLength(other.TargetLength),

		//setup rope data
		DataBuffer(std::move(other.DataBuffer)),

		Positions(DataBuffer, &Node::Position),
		OldPositions(DataBuffer, &Node::OldPosition),
		Accelerations(DataBuffer, &Node::Acceleration),
		IsAnchored(DataBuffer, &Node::IsAnchored)
	{
		//reset everything in other to avoid dangling pointers
		other.NodeAmount = 0;
		other.TargetLength = 0;
		other.Radius = 0;
	};

	// overload the "=" operator so the copy/move constructor can work properly
	Rope& operator=(Rope&& other) noexcept {
		if (this != &other) {

			NodeAmount = other.NodeAmount;
			TargetLength = other.TargetLength;
			Radius = other.Radius;
			DataBuffer = std::move(other.DataBuffer); // Steal vector

			// Clean up the other rope
			other.NodeAmount = 0;
			other.TargetLength = 0;
			other.Radius = 0;
		}
		//return this new rope
		return *this;
	}

	~Rope() = default;

	// fills the rope with positions and initial Accelerations, every node's position offset to the right of a previous one
	// this is how it should look when rthe rope is created:[ N1------N2------N3 ] and so on
	void FillRope(Vector2 position, Vector2 initialAcceleration) {

		for (int i = 0; i < NodeAmount; i++) {

			Vector2 offset = Vector2{ TargetLength * i, 0 };
			Vector2 currentPos = Vector2{ position.x + offset.x, position.y + offset.y };


			Positions[i] = currentPos;
			OldPositions[i] = currentPos - initialAcceleration;
			Accelerations[i] = Vector2Zero();
			IsAnchored[i] = 0.0f;
		}
	}
};

