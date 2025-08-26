//
// Created by jpett on 7/30/2025.
//

#include "color.h"
#include "rtw_stb_image.h"

#ifndef TEXTURE_H
#define TEXTURE_H

class texture {
public:
    texture(const char* filename) : tex(filename) {}

    glm::vec3 value(float u, float v, const glm::vec3& p) const {
        //if (tex.height() <= 0) return color{0,1,1};

        u = interval(0,1).clamp(u);
        v = 1.0f - interval(0,1).clamp(v);

        int i = int(u * tex.width());
        int j = int(v * tex.height());
        auto pixel = tex.pixel_data(i,j);

        float colorScale = 1.0f / 255.0f;
        return glm::vec3 {colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]};
    }

private:
    rtw_image tex;
};
#endif //TEXTURE_H
