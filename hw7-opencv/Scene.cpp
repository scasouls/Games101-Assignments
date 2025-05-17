//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

//用来获得光线与场景模型的交点信息
Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

//对所有光源进行随机采样
void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    //第一个接触点
    Intersection Point_inter = Scene::intersect(ray);

    //线有接触到场景
    if (Point_inter.happened) {
        Material* m = Point_inter.m;

        //如果接触的是光源，则直接返回自发光强度
        if (m->hasEmission()) {
            return m->getEmission();
        }

        //接触点位置和法向量
        Vector3f Point = Point_inter.coords;
        Vector3f N = Point_inter.normal.normalized();

        //从所有光源中随机采样，产生位置和概率密度
        float Light_Pdf = 0.0;
        Intersection Light_inter;
        sampleLight(Light_inter, Light_Pdf);

        //光源位置，方向，法向量，自发光
        Vector3f x = Light_inter.coords;
        Vector3f ws = normalize(x - Point);
        Vector3f NN = Light_inter.normal.normalized();
        Vector3f Light_emit = Light_inter.emit;

        //向光源方向射线
        Ray ray2(Point, ws);
        Intersection Ray2_inter = Scene::intersect(ray2);

        //直接光
        Vector3f L_dir(0);
        //判断遮挡
        if (Ray2_inter.happened && (Ray2_inter.coords - Light_inter.coords).norm() <= EPSILON) {

            L_dir = Light_emit * m->eval(ray.direction, ws, N) * std::max(0.f, dotProduct(ws, N)) * std::max(0.f, dotProduct(-ws, NN)) / std::pow((x - Point).norm(), 2) / Light_Pdf;

        }

        //间接光
        Vector3f L_indir(0);

        //根据RussianRoulette判断是否继续递归
        float random = get_random_float();
        if (random > RussianRoulette)
        {
            return L_indir + L_dir;
        }

        //计算并发射反射光线
        Vector3f wi = m->sample(ray.direction, N).normalized();
        Ray ray_re(Point, wi);

        //如果接触到一个非光源物体
        Intersection reflect_inter = intersect(ray_re);
        if (reflect_inter.happened && !reflect_inter.m->hasEmission()) {

            L_indir = castRay(ray_re, depth + 1) * m->eval(ray.direction, wi, N) * dotProduct(wi, N) / m->pdf(ray.direction, wi, N) / RussianRoulette;

        }
        //累加直接光和间接光
        return L_indir + L_dir;
    }
    else return {};
}