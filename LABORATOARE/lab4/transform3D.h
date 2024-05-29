#pragma once

#include "utils/glm_utils.h"


namespace transform3D
{
    // Translate matrix
    inline glm::mat4 Translate(float translateX, float translateY, float translateZ)
    {
        // TODO(student): Implement the translation matrix
        return glm::mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            translateX, translateY, translateZ, 1
        );

    }

    // Scale matrix
    inline glm::mat4 Scale(float scaleX, float scaleY, float scaleZ)
    {
        // TODO(student): Implement the scaling matrix
        return glm::mat4(
            scaleX, 0, 0, 0,
            0, scaleY, 0 ,0,
            0, 0, scaleZ, 0,
            0, 0, 0, 1
        );

    }

   


    // Rotate matrix relative to the OZ axis
    inline glm::mat4 RotateOZ(float radians)
    {
        GLfloat c = cos(radians);
        GLfloat s = sin(radians);
        // TODO(student): Implement the rotation matrix
        return glm::mat4(
            c, s, 0, 0,
            -s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        );

    }

    // Rotate matrix relative to the OY axis
    inline glm::mat4 RotateOY(float radians)
    {
        GLfloat c = cos(radians);
        GLfloat s = sin(radians);

        // TODO(student): Implement the rotation matrix
        return glm::mat4(
            c, 0, -s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1
        );

    }

    // Rotate matrix relative to the OX axis
    inline glm::mat4 RotateOX(float radians)
    {
        GLfloat c = cos(radians);
        GLfloat s = sin(radians);
        // TODO(student): Implement the rotation matrix
        return glm::mat4(
            1, 0, 0, 0,
            0, c, s, 0,
            0, -s, c, 0,
            0, 0, 0, 1
        );

    }
}   // namespace transform3D
