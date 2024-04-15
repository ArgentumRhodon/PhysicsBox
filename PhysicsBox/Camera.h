#pragma once
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/constants.hpp>

using namespace glm;

class Camera
{
public:
	vec4 eye;
	vec4 lookat;
	mat4 viewMat;
	mat4 projMat;

private:
	float fovy;
	float aspect;
	float near_plane;
	float far_plane;

	vec4 axis_n;
	vec4 axis_u;
	vec4 axis_v;
	vec4 world_up;

	vec2 mouse_pos;
	vec2 mouse_pre_pos;
	unsigned short mouse_button;

	vec2 key_pos;
	vec2 key_pre_pos;
	bool m_altKey;

	// View Frustum
	vec4 ntl;
	vec4 ntr;
	vec4 nbl;
	vec4 nbr;
	vec4 ftl;
	vec4 fbr;
	vec4 ftr;
	vec4 fbl;

	// Scale Parameters
	float M_ZOOM_PAR;
	float M_PAN_PAR;

public:
	Camera();
	~Camera();
	void PrintProps();
	void Set(
		float eye_x, float eye_y, float eye_z,
		float lookat_x, float lookat_y, float lookat_z,
		int winW, int winH,
		float p_angle = 45.0f, float p_near = 0.1f, float p_far = 10000.0f
	);
	void SetProjectionMatrix(int winW, int winH);
	void SetViewMatrix();
	void MouseClick(int button, int state, int x, int y, int winW, int winH);
	void MouseMotion(int x, int y, int winW, int winH);

	// Focus Cam
	void CameraRotate();
	void CameraZoom(int dir, int winW, int winH);
	void CameraPan();

	// First Person Cam
	void CameraPanFP();
	void CameraRotateFP(int winW, int winH);

	void DrawGrid();

	void DrawFrustum();

private:
	void horizontalRotate();
	void verticalRotate();
	void getCamCS();
	void getViewFrustum();
	void calibrate(int winW, int winH);
};

