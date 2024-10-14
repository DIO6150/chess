#ifndef MGE_CAMERA_H
#define MGE_CAMERA_H

#include "cglm/cglm.h"

typedef struct
{
    vec3 camera_up;
    vec3 camera_front;
    vec3 camera_position;

    float yaw;
    float pitch;
} Camera;

int mgeCameraInit(Camera** _out_camera, vec3 _in_origin);
int mgeCameraFree(Camera* _in_camera);

int mgeCameraMove(Camera* _in_camera, vec3 _in_pos);
int mgeCameraMoveFromFront(Camera* _in_camera, vec3 _in_pos);
int mgeCameraMoveFromRight(Camera* _in_camera, vec3 _in_pos);

int mgeCameraLook(Camera* _in_camera, float _in_yaw, float _in_pitch);

int mgeCameraSetPos(Camera* _in_camera, vec3 _in_pos);
int mgeCameraSetLook(Camera* _in_camera, float _in_yaw, float _in_pitch);

int mgeCameraUpdate(Camera* _in_camera);
int mgeCameraGetView(Camera* _in_camera, mat4 _out_view);


#endif