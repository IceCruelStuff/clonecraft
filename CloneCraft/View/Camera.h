#pragma once

#include <glad\glad.h>
#include "GlmCommon.h"

class Camera
{
private:

	// Default values
	static const float SPEED, SENSITIVTY, SCROLLSPEED, ZOOM, YAW, PITCH;
	static const float NEARPLANE, FARPLANE;
	static const vec3 POSITION, FRONT, WORLDUP;

	vec3 m_position;
	vec2 m_chunk;
	vec3 m_front{ FRONT };
	vec3 m_up;
	vec3 m_right;

	float m_yaw;
	float m_pitch;

	float m_speed{ SPEED };
	float m_sensitivity{ SENSITIVTY };
	float m_zoom{ ZOOM };

	bool newChunk{ false };
	void updateFromEuler();

public:

	enum Direction {
		FORWARD,
		BACKWARD,
		RIGHT,
		LEFT,
		UP,
		DOWN
	};

	Camera(vec3 position = vec3{ 0.0f, 0.0f, 0.0f }, float yaw = YAW, float pitch = PITCH);

	mat4 getViewMatrix();
	mat4 getProjectionMatrix();
	vec3 getPosition();
	float getYaw();
	float getPitch();

	void move(Direction direction, float deltaTime);
	void move(vec3 offset);
	void setPosition(vec3 position);
	void processMouse(vec2 offset);
	void processMouseScroll(float yOffset);

	bool isInNewChunk();
};