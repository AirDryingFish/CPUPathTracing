#include "utils.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

int main(){

    // World Objs
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0));
    auto material_center_sphere = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    // auto material_left_sphere = make_shared<metal>(color(0.8, 0.8, 0.8), 0.5);
    auto material_left_sphere = make_shared<dielectric>(1.50);
    auto material_left_sphere_insidebubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right_sphere = make_shared<metal>(color(0.8, 0.6, 0.2), 0.2);

    world.add(make_shared<sphere>(point3(0, 0, -1.2), 0.5, material_center_sphere));
    world.add(make_shared<sphere>(point3(-1.0, 0, -1.0), 0.5, material_left_sphere));
    world.add(make_shared<sphere>(point3(-1.0, 0, -1.0), 0.4, material_left_sphere_insidebubble));
    world.add(make_shared<sphere>(point3(1.0, 0, -1.0), 0.5, material_right_sphere));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));

    camera cam;
    cam.image_width = 720;
    cam.aspact_ratio = 16.0 / 9.0;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.vfov = 20;
    cam.vup = vec3(0, 1, 0);
    cam.look_from = point3(-2, 2, 1);
    cam.look_at = point3(0, 0, -1);


    cam.render(world);

}
