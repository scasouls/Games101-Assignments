// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(int x, int y, const Vector3f* _v)
{
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]

    //定义三角形的三个顶点
    Vector3f A = _v[0];
    Vector3f B = _v[1];
    Vector3f C = _v[2];

    //定义三角形的三条边
    Vector3f AB = B - A;
    Vector3f BC = C - B;
    Vector3f CA = A - C;

    Vector3f P;
    P << x, y, A[2];
    //检测点的三条边
    Vector3f AP = P - A;
    Vector3f BP = P - B;
    Vector3f CP = P - C;

    //点积
    Vector3f crossAP = AB.cross(AP);
    Vector3f crossBP = BC.cross(BP);
    Vector3f crossCP = CA.cross(CP);

    //是否位于三角形内
    if (crossAP.dot(crossBP) > 0 && crossAP.dot(crossCP) > 0 && crossBP.dot(crossCP))
        return 1;
    else
        return 0;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    //获取当前帧的数据
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    //
    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    
    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle
    float xmin = std::floor(std::min(v[0].x(), std::min(v[1].x(), v[2].x())));
    float xmax = std::ceil(std::max(v[0].x(), std::max(v[1].x(), v[2].x())));
    float ymin = std::floor(std::min(v[0].y(), std::min(v[1].y(), v[2].y())));
    float ymax = std::ceil(std::max(v[0].y(), std::max(v[1].y(), v[2].y())));

    std::vector<std::vector<bool>> image(xmax, std::vector<bool>(ymax, 0));
    for (int x = xmin; x < xmax; ++x) {
        for (int y = ymin; y < ymax; ++y) {
            float min_dep = FLT_MAX;
            
            bool SSAA = false;//开启反走样

            //4×MSAA的实现
            if (SSAA) {
                float pixel4 = 0;
                float count = 0;
                for (int i = 0.25; i < 1.0; i+=0.5) {
                    for (int j = 0.25; j < 1.0; j+=0.5) {
                        float sample_x = float(x) + i;
                        float sample_y = float(y) + j;
                        if (insideTriangle(sample_x, sample_y, t.v)) {
                            auto [alpha, beta, gamma] = computeBarycentric2D(sample_x, sample_x, t.v);
                            float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                            float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                            z_interpolated *= w_reciprocal;
                            pixel4 += z_interpolated;
                            count++;
                        }
                    }
                }
                if(count > 0){
                    min_dep = std::min(min_dep, pixel4 / count);
                    if (depth_buf[get_index(x, y)] > min_dep) {
                        depth_buf[get_index(x, y)] = min_dep;
                        Eigen::Vector3f point(x, y, 1.0f);
                        set_pixel(point, t.getColor()*(count/4.0f));
                    }
                }
            }

            //查找位于三角形内部的点并进行插值
            else {
                if (insideTriangle(x + 0.5, y + 0.5, t.v)) {

                    auto [alpha, beta, gamma] = computeBarycentric2D(x + 0.5, y + 0.5, t.v);
                    float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    min_dep = std::min(min_dep, z_interpolated);

                    if (depth_buf[get_index(x, y)] > min_dep) {
                        depth_buf[get_index(x, y)] = min_dep;
                        Eigen::Vector3f point(x, y, 1.0f);
                        set_pixel(point, t.getColor());
                    }
                }

            }
        }
    }
    // If so, use the following code to get the interpolated z value.
    

    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();

    //更新帧缓存，设置颜色
    frame_buf[ind] = color;

}

// clang-format on