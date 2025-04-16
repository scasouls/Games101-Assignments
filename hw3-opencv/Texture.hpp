//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;//纹理图

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);//获取纹理图
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    //从纹理图中获得颜色数据
    Eigen::Vector3f getColor(float u, float v)
    {
        //防止数据越出范围
        if (u < 0) u = 0.0f;
        if (u > 1) u = 0.999f;
        if (v < 0) v = 0.0f;
        if (v > 1) v = 0.999f;
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

};
#endif //RASTERIZER_TEXTURE_H
