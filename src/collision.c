#include "collision.h"

int intersect(obb_t *obb1, obb_t *obb2, vec3 axis) {
    float a_min = +HUGE;
    float a_max = -HUGE;
    float b_min = +HUGE;
    float b_max = -HUGE;

    for (int i = 0; i < 8; i++) {
        float dist;

        dist = vec3_mul_inner(obb1->corners[i], axis);
        a_min = fminf(a_min, dist);
        a_max = fmaxf(a_max, dist);

        dist = vec3_mul_inner(obb2->corners[i], axis);
        b_min = fminf(b_min, dist);
        b_max = fmaxf(b_max, dist);
    }

    float span = fmaxf(a_max, b_max) - fminf(a_min, b_min);
    float sum = (a_max - a_min) + (b_max - b_min);

    return span <= sum;
}

int check_collision(obb_t *obb1, obb_t *obb2) {
    for (int i = 0; i < 3; i++)
        if (!intersect(obb1, obb2, obb1->axis[i]))
            return 0;

    for (int i = 0; i < 3; i++)
        if (!intersect(obb1, obb2, obb2->axis[i]))
            return 0;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            vec3 axis;
            vec3_mul_cross(axis, obb1->axis[i], obb2->axis[j]);

            if (axis[0] == 0 && axis[1] == 0 && axis[2] == 0)
                continue;

            if (!intersect(obb1, obb2, axis))
                return 0;
        }

    return 1;
}
