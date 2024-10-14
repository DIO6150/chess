#include "core/camera.h"

int mgeCameraInit(Camera** _out_camera, vec3 _in_origin)
{
    (*_out_camera) = malloc(sizeof(Camera));

    if (!(*_out_camera))
    {
        printf("%s(%s) : Camera memory could'nt be allocated.\n", __func__, __FILE__);
        return (0);
    }

    glm_vec3_copy(_in_origin, (*_out_camera)->camera_position);
    glm_vec3_copy((vec3) { 0.0f, 0.0f, -1.0f}, (*_out_camera)->camera_front);
    glm_vec3_copy((vec3) { 0.0f, 1.0f, 0.0f }, (*_out_camera)->camera_up);

    (*_out_camera)->yaw = -90.0f;
    (*_out_camera)->pitch = 0.0f;

    mgeCameraUpdate(*_out_camera);

    return (1);
}

int mgeCameraFree(Camera* _in_camera)
{
    if (!_in_camera)
    {
        printf("%s(%s) : Camera is NULL.\n", __func__, __FILE__);
        return (0);
    }

    free(_in_camera);

    return (1);
}

int mgeCameraMove(Camera* _in_camera, vec3 _in_pos)
{
    if (!_in_camera)
    {
        printf("%s(%s) : Camera is NULL.\n", __func__, __FILE__);
        return (0);
    }

    glm_vec3_add(_in_camera->camera_position, _in_pos, _in_camera->camera_position);
    return (1);
}

int mgeCameraMoveFromFront(Camera* _in_camera, vec3 _in_pos)
{
    if (!_in_camera)
    {
        printf("%s(%s) : Camera is NULL.\n", __func__, __FILE__);
        return (0);
    }

    vec3 temp;
    glm_vec3_mul(_in_pos, _in_camera->camera_front, temp);

    mgeCameraMove(_in_camera, temp);

    return (1);
}

int mgeCameraMoveFromRight(Camera* _in_camera, vec3 _in_pos)
{
    if (!_in_camera)
    {
        printf("%s(%s) : Camera is NULL.\n", __func__, __FILE__);
        return (0);
    }

    vec3 temp;
    glm_vec3_cross(_in_camera->camera_front, _in_camera->camera_up, temp);
    glm_vec3_normalize(temp);
    glm_vec3_mul(temp, _in_pos , temp);

    mgeCameraMove(_in_camera, temp);

    return (1);
}

int mgeCameraLook(Camera* _in_camera, float _in_yaw, float _in_pitch)
{
    if (!_in_camera)
    {
        printf("%s(%s) : Camera is NULL.\n", __func__, __FILE__);
        return (0);
    }

    _in_camera->yaw += _in_yaw;
    _in_camera->pitch += _in_pitch;

    if (_in_camera->yaw > 360.0f) _in_camera->yaw = 0.0f;
    if (_in_camera->yaw < 0.0f) _in_camera->yaw = 360.0f;

    if (_in_camera->pitch > 360.0f) _in_camera->pitch = 0.0f;
    if (_in_camera->pitch < 0.0f) _in_camera->pitch = 360.0f;

    return (1);
}

int mgeCameraSetPos(Camera* _in_camera, vec3 _in_pos)
{
    if (!_in_camera)
    {
        printf("%s(%s) : Camera is NULL.\n", __func__, __FILE__);
        return (0);
    }

    glm_vec3_copy(_in_pos, _in_camera->camera_position);
    return (1);
}
int mgeCameraSetLook(Camera* _in_camera, float _in_yaw, float _in_pitch)
{
    if (!_in_camera)
    {
        printf("%s(%s) : Camera is NULL.\n", __func__, __FILE__);
        return (0);
    }

    _in_camera->yaw = _in_yaw;
    _in_camera->pitch = _in_pitch;

    return (1);
}

int mgeCameraUpdate(Camera* _in_camera)
{
    if (!_in_camera)
    {
        printf("%s(%s) : Camera is NULL.\n", __func__, __FILE__);
        return (0);
    }

    _in_camera->camera_front[0] = cos(_in_camera->yaw * (3.14f / 180.0f)) * cos(_in_camera->pitch * (3.14f / 180.0f));
    _in_camera->camera_front[1] = sin(_in_camera->pitch * (3.14f / 180.0f));
    _in_camera->camera_front[2] = sin(_in_camera->yaw * (3.14f / 180.0f)) * cos(_in_camera->pitch * (3.14f / 180.0f));

    glm_normalize(_in_camera->camera_front);

    return (1);
}

int mgeCameraGetView(Camera* _in_camera, mat4 _out_view)
{
    if (!_in_camera)
    {
        printf("%s(%s) : Camera is NULL.\n", __func__, __FILE__);
        return (0);
    }

    mgeCameraUpdate(_in_camera);

    vec3 pos_front_sum;
    glm_vec3_add(_in_camera->camera_front, _in_camera->camera_position, pos_front_sum);
    glm_lookat(_in_camera->camera_position, pos_front_sum, _in_camera->camera_up, _out_view);

    return (1);
}