#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Camera.h"

#include <iostream>
using namespace std;

Camera::Camera()
{
    eye = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lookat = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    axis_n = vec4(0.0f);
    axis_u = vec4(0.0f);
    axis_v = vec4(0.0f);

    fovy = 0.0f;
    aspect = 0.0f;
    near_plane = 0.0f;
    far_plane = 0.0f;

    world_up = vec4(0.0f, 1.0f, 0.0f, 0.0f);

    mouse_pos = vec2(0.0f);
    mouse_pre_pos = vec2(0.0f);
    mouse_button = 0;

    key_pos = vec2(0.0f);
    key_pre_pos = vec2(0.0f, 0.0f);
    m_altKey = false;

    M_ZOOM_PAR = 0.03f;
    M_PAN_PAR = 50.0f;
}

Camera::~Camera()
{
}

void Camera::PrintProps()
{
    cout << "********* Camera **********" << endl;
    cout << "eye: " << eye.x << ", " << eye.y << ", " << eye.z << endl;
    cout << "lookat: " << lookat.x << ", " << lookat.y << ", " << lookat.z << endl;
    cout << "angle = " << fovy << endl;
    cout << "ratio = " << aspect << endl;
    cout << "near = " << near_plane << endl;
    cout << "far = " << far_plane << endl;
    cout << "n = " << axis_n.x << ", " << axis_n.y << ", " << axis_n.z << endl;
    cout << "u = " << axis_u.x << ", " << axis_u.y << ", " << axis_u.z << endl;
    cout << "v = " << axis_v.x << ", " << axis_v.y << ", " << axis_v.z << endl;
}

void Camera::Set(float eye_x, float eye_y, float eye_z, float lookat_x, float lookat_y, float lookat_z, int winW, int winH, float p_angle, float p_near, float p_far)
{
    eye = vec4(eye_x, eye_y, eye_z, 1.0f);
    lookat = vec4(lookat_x, lookat_y, lookat_z, 1.0f);

    fovy = p_angle;
    near_plane = p_near;
    far_plane = p_far;

    calibrate(winW, winH);
}

void Camera::SetProjectionMatrix(int winW, int winH)
{
    aspect = (float)winW / (float)winH;
    projMat = perspective(radians(fovy), aspect, near_plane, far_plane);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(value_ptr(projMat));
}

void Camera::SetViewMatrix()
{
    viewMat = lookAt(eye.xyz(), lookat.xyz(), world_up.xyz());
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(value_ptr(viewMat));
}

void Camera::MouseClick(int button, int state, int x, int y, int winW, int winH)
{
    if (x >= winW || y >= winH) return;

    mouse_button = (state == GLUT_DOWN) ? button : 0;

    mouse_pos = vec2(x / (float)winW - 1.0f, (winH - y) / (float)winH - 1.0f);
    mouse_pre_pos = mouse_pos;
    m_altKey = glutGetModifiers() == GLUT_ACTIVE_ALT;

    if (button == 3) CameraZoom(-1, winW, winH);
    if (button == 4) CameraZoom(1, winW, winH);
}

void Camera::MouseMotion(int x, int y, int winW, int winH)
{
    if (x >= winW || y >= winH) return;

    mouse_pos = vec2(x / (float)winW - 1.0f, (winH - y) / (float)winH - 1.0f);
    if (m_altKey && mouse_button == GLUT_LEFT_BUTTON)
    {
        CameraRotate();
    }
    else if (mouse_button == GLUT_MIDDLE_BUTTON) 
    {
        CameraPan();
    }

    mouse_pre_pos = mouse_pos;

    calibrate(winW, winH);
    glViewport(0, 0, winW, winH);

    glutPostRedisplay();
}

void Camera::CameraRotate()
{
    horizontalRotate();
    verticalRotate();
}

void Camera::CameraZoom(int dir, int winW, int winH)
{
    eye -= lookat;
    eye += dir > 0.0f ? eye * M_ZOOM_PAR : -eye * M_ZOOM_PAR;
    eye += lookat;

    calibrate(winW, winH);
    glViewport(0, 0, winW, winH);
    glutPostRedisplay();
}

void Camera::CameraPan()
{
    eye.x += M_PAN_PAR * (viewMat[0][0] * (mouse_pre_pos.x - mouse_pos.x) + viewMat[0][1] * (mouse_pre_pos.y - mouse_pos.y));
    eye.y += M_PAN_PAR * (viewMat[1][0] * (mouse_pre_pos.x - mouse_pos.x) + viewMat[1][1] * (mouse_pre_pos.y - mouse_pos.y));
    eye.z += M_PAN_PAR * (viewMat[2][0] * (mouse_pre_pos.x - mouse_pos.x) + viewMat[2][1] * (mouse_pre_pos.y - mouse_pos.y));

    lookat.x += M_PAN_PAR * (viewMat[0][0] * (mouse_pre_pos.x - mouse_pos.x) + viewMat[0][1] * (mouse_pre_pos.y - mouse_pos.y));
    lookat.y += M_PAN_PAR * (viewMat[1][0] * (mouse_pre_pos.x - mouse_pos.x) + viewMat[1][1] * (mouse_pre_pos.y - mouse_pos.y));
    lookat.z += M_PAN_PAR * (viewMat[2][0] * (mouse_pre_pos.x - mouse_pos.x) + viewMat[2][1] * (mouse_pre_pos.y - mouse_pos.y));
}

void Camera::DrawGrid()
{
    int size = 25;
    if (size % 2 != 0) ++size;

    glUseProgram(0);
    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(value_ptr(viewMat));

    glBegin(GL_LINES);
    for (int i = 0; i < size + 1; i++)
    {
        if ((float)i == size / 2.0f) glColor3f(0.0f, 0.0f, 0.0f);
        else glColor3f(0.8f, 0.8f, 0.8f);

        glVertex3f(-size / 2.0f, 0.0f, -size / 2.0f + i);
        glVertex3f(size / 2.0f, 0.0f, -size / 2.0f + i);
        glVertex3f(-size / 2.0f + i, 0.0f, -size / 2.0f);
        glVertex3f(-size / 2.0f + i, 0.0f, size / 2.0f);
    }
    glEnd();
    glPopMatrix();
}

//void Camera::drawCoordinateOnScreen(int winW, int winH)
//{
//}

void Camera::DrawFrustum()
{
    glUseProgram(0);
    glDisable(GL_LIGHTING);
    glLineWidth(1.5f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Near
    glColor3f(1.0f, 0.7f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(ntl.x, ntl.y, ntl.z);
    glVertex3f(ntr.x, ntr.y, ntr.z);
    glVertex3f(nbr.x, nbr.y, nbr.z);
    glVertex3f(nbl.x, nbl.y, nbl.z);
    glEnd();

    // Far
    glBegin(GL_LINE_LOOP);
    glVertex3f(ftr.x, ftr.y, ftr.z);
    glVertex3f(ftl.x, ftl.y, ftl.z);
    glVertex3f(fbl.x, fbl.y, fbl.z);
    glVertex3f(fbr.x, fbr.y, fbr.z);
    glEnd();

    // Bottom
    glBegin(GL_LINE_LOOP);
    glVertex3f(nbl.x, nbl.y, nbl.z);
    glVertex3f(nbr.x, nbr.y, nbr.z);
    glVertex3f(fbr.x, fbr.y, fbr.z);
    glVertex3f(fbl.x, fbl.y, fbl.z);
    glEnd();

    // Top
    glBegin(GL_LINE_LOOP);
    glVertex3f(ntr.x, ntr.y, ntr.z);
    glVertex3f(ntl.x, ntl.y, ntl.z);
    glVertex3f(ftl.x, ftl.y, ftl.z);
    glVertex3f(ftr.x, ftr.y, ftr.z);
    glEnd();

    // Left
    glBegin(GL_LINE_LOOP);
    glVertex3f(ntl.x, ntl.y, ntl.z);
    glVertex3f(nbl.x, nbl.y, nbl.z);
    glVertex3f(fbl.x, fbl.y, fbl.z);
    glVertex3f(ftl.x, ftl.y, ftl.z);
    glEnd();

    // Right
    glBegin(GL_LINE_LOOP);
    glVertex3f(nbr.x, nbr.y, nbr.z);
    glVertex3f(ntr.x, ntr.y, ntr.z);
    glVertex3f(ftr.x, ftr.y, ftr.z);
    glVertex3f(fbr.x, fbr.y, fbr.z);
    glEnd();

    glLineWidth(1);
    glPopMatrix();
}

void Camera::horizontalRotate()
{
    float theta = (-(mouse_pos.x - mouse_pre_pos.x) * 100.0f);
    mat4 mat = translate(mat4(1.0f), vec3(lookat));
    mat = rotate(mat, theta * pi<float>() / 180.0f, vec3(world_up));
    mat = translate(mat, -vec3(lookat));

    eye = mat * eye;
}

void Camera::verticalRotate()
{
    float theta = (mouse_pos.y - mouse_pre_pos.y) * 100.0f;
    mat4 mat = translate(mat4(1.0f), vec3(lookat));
    mat = rotate(mat, theta * pi<float>() / 180.0f, vec3(axis_u));
    mat = translate(mat, -vec3(lookat));

    eye = mat * eye;

    vec4 v1 = vec4(cross(vec3(world_up), vec3(axis_u)), 0.0f);
    vec4 v2 = eye - lookat;
    if (dot(v1, v2) > 0.0f) world_up.y = (world_up.y == 1.0f) ? -1.0f : 1.0f;
}

void Camera::getCamCS()
{
    axis_n = normalize(eye - lookat);
    axis_u = normalize(vec4(cross(vec3(world_up), vec3(axis_n)), 0.0f));
    axis_v = normalize(vec4(cross(vec3(axis_n), vec3(axis_u)), 0.0f));
}

void Camera::getViewFrustum()
{
    vec4 nc = eye - axis_n * near_plane;
    vec4 fc = eye - axis_n * far_plane;

    float tang = (float)tan(fovy * 3.14159265f / 180.0f * 0.5f);
    float nh = 2.0f * near_plane * tang;
    float nw = nh * aspect;

    float fh = 2.0f * far_plane * tang;
    float fw = fh * aspect;			    

    ntl = nc + axis_v * nh * 0.5f - axis_u * nw * 0.5f;
    ntr = nc + axis_v * nh * 0.5f + axis_u * nw * 0.5f;
    nbl = nc - axis_v * nh * 0.5f - axis_u * nw * 0.5f;
    nbr = nc - axis_v * nh * 0.5f + axis_u * nw * 0.5f;

    ftl = fc + axis_v * fh * 0.5f - axis_u * fw * 0.5f;
    fbr = fc - axis_v * fh * 0.5f + axis_u * fw * 0.5f;
    ftr = fc + axis_v * fh * 0.5f + axis_u * fw * 0.5f;
    fbl = fc - axis_v * fh * 0.5f - axis_u * fw * 0.5f;
}

void Camera::calibrate(int winW, int winH)
{
    SetProjectionMatrix(winW, winH);
    SetViewMatrix();
    getCamCS();
    getViewFrustum();
}
