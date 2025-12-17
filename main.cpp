#include "utils.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

int main(){

    // World Objs
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, material_ground));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> shared_material;
                if (choose_mat < 0.8) {

                    auto albedo = color::random() * color::random();
                    shared_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, shared_material));
                }
                else if (choose_mat < 0.95) {
                    auto albedo = color::random(0.5, 1.0);
                    auto fuzz = random_double(0, 0.5);
                    shared_material = make_shared<metal>(albedo, fuzz);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, shared_material));
                }
                else {
                    shared_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, shared_material));
                }

            }
        }
    }

    auto material1 = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    world.add(make_shared<sphere>(point3(-4.0, 1.0, 0), 1.0, material1));


    auto material2 = make_shared<dielectric>(1.50);
    auto material2insidebubble = make_shared<dielectric>(1.00 / 1.50);
    world.add(make_shared<sphere>(point3(0, 1.0, 0), 1.0, material2));
    world.add(make_shared<sphere>(point3(0, 0.9, 0), 0.9, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4.0, 1.0, 0), 1.0, material3));
    // world.add(make_shared<sphere>(point3(0, 0, -1.2), 0.5, material_center_sphere));
    // world.add(make_shared<sphere>(point3(-1.0, 0, -1.0), 0.5, material_left_sphere));
    // world.add(make_shared<sphere>(point3(-1.0, 0, -1.0), 0.4, material_left_sphere_insidebubble));
    // world.add(make_shared<sphere>(point3(1.0, 0, -1.0), 0.5, material_right_sphere));
    

    camera cam;
    cam.image_width = 400;
    cam.aspact_ratio = 16.0 / 9.0;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.vup = vec3(0, 1, 0);
    cam.look_from = point3(13, 2, 3);
    cam.look_at = point3(0, 0, 0);

    cam.defocus_angle = 0.6;
    cam.focus_distance = 10;


    cam.render(world);

}
