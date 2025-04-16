//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_SHADER_H
#define RASTERIZER_SHADER_H
#include <Eigen/Eigen>
#include "Texture.hpp"

//片着色器
struct fragment_shader_payload
{
    fragment_shader_payload()
    {
        texture = nullptr;
    }

    fragment_shader_payload(const Eigen::Vector3f& col, const Eigen::Vector3f& nor,const Eigen::Vector2f& tc, Texture* tex) :
         color(col), normal(nor), tex_coords(tc), texture(tex) {}


    Eigen::Vector3f view_pos;//视角位置
    Eigen::Vector3f color;//颜色
    Eigen::Vector3f normal;//法向量
    Eigen::Vector2f tex_coords;//纹理坐标
    Texture* texture;//纹理图
};

//顶点着色器
struct vertex_shader_payload
{
    Eigen::Vector3f position;
};

#endif //RASTERIZER_SHADER_H
