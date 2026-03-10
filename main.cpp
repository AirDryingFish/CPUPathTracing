#include "utils.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "constant_medium.h"

void bouncing_spheres()
{
    // World Objs
    hittable_list world;
    auto checker_tex = make_shared<checker_texture>(0.2, color(0.8, 0.8, 0.8), color(0.6, 0.1, 0.8));
    auto material_ground = make_shared<lambertian>(checker_tex);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, material_ground));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<material> shared_material;
                if (choose_mat < 0.8)
                {

                    auto albedo = color::random() * color::random();
                    shared_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, shared_material));
                }
                else if (choose_mat < 0.95)
                {
                    auto albedo = color::random(0.5, 1.0);
                    auto fuzz = random_double(0, 0.5);
                    shared_material = make_shared<metal>(albedo, fuzz);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, shared_material));
                }
                else
                {
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
    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;
    cam.image_width = 1200;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.samples_per_pixel = 250;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.vup = vec3(0, 1, 0);
    cam.look_from = point3(13, 2, 3);
    cam.look_at = point3(0, 0, 0);

    cam.defocus_angle = 0.6;
    cam.focus_distance = 10;

    cam.render(world);
}

void checkered_spheres()
{
    hittable_list world;

    auto checker1_tex = make_shared<checker_texture>(1.0, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    auto checker2_tex = make_shared<checker_texture>(0.2, color(0.8, 0.8, 0.8), color(0.6, 0.1, 0.8));
    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker1_tex)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker2_tex)));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.look_from = point3(13, 2, 3);
    cam.look_at = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.0;

    cam.render(world);
}

void draw_earth()
{
    auto earth_tex = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_tex);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.look_from = point3(13, 2, 3);
    cam.look_at = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.0;

    cam.render(hittable_list(globe));
}

void draw_perlin_spheres()
{
    hittable_list world;
    auto perlin_tex = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(perlin_tex)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(perlin_tex)));

    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 20;
    cam.look_from = point3(13, 2, 3);
    cam.look_at = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.0;
    cam.render(world);
}

void draw_quads()
{
    hittable_list world;
    // Materials
    auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

    camera cam;
    cam.aspect_ratio = 1.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0.70, 0.80, 1.00);

    cam.vfov = 80;
    cam.look_from = point3(0, 0, 8);
    cam.look_at = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void draw_simple_light()
{
    hittable_list world;
    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    world.add(make_shared<quad>(point3(3, 1, -5), vec3(4, 0, 0), vec3(0, 4, 0), difflight));
    world.add(make_shared<sphere>(point3(0, 10, 0), 4, difflight));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 20;
    cam.look_from = point3(26, 3, 6);
    cam.look_at = point3(0, 2, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void cornell_box()
{
    hittable_list world;

    // Materials
    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    // auto isotropic = make_shared<isotropic>(color(0.5, 0.5, 0.5));

    // Entites
    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green)); // left
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(box1);

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    world.add(box2);

    // Light
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));

    camera cam;
    cam.aspect_ratio = 1.0;
    cam.image_width = 200;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.look_from = point3(278, 278, -800);
    cam.look_at = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.render(world);
}

void cornell_box_frog()
{
    hittable_list world;

    // Materials
    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    // auto isotropic = make_shared<isotropic>(color(0.5, 0.5, 0.5));

    // Entites
    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green)); // left
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    // wrapping the whole box in a foggy medium
    world.add(make_shared<constant_medium>(box1, 0.01, color(0.0, 0.0, 0.0)));
    world.add(make_shared<constant_medium>(box2, 0.01, color(1.0, 0.0, 0.0)));

    // Light
    world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));

    camera cam;
    cam.aspect_ratio = 1.0;
    cam.image_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.look_from = point3(278, 278, -800);
    cam.look_at = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.render(world);
}

void book_two_final_scene(int image_width, int samples_per_pixel, int max_depth)
{
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++)
    {
        for (int j = 0; j < boxes_per_side; j++)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    // Light
    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quad>(point3(123, 554, 147), vec3(330, 0, 0), vec3(0, 0, 305), light));

    // moving sphere
    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, 50, sphere_material));

    // dielectric sphere
    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    // metal sphere
    world.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));

    // foggy boundary
    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

    // textured sphere
    auto earth_texture = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    world.add(make_shared<sphere>(point3(400, 200, 400), 100, earth_texture));

    // perlin noise textured sphere
    auto perlin_texture = make_shared<noise_texture>(0.3);
    world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(perlin_texture)));

    hittable_list boxes2;
    auto while_mat = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    int ns = 1500;
    for (int j = 0; j < ns; j++)
    {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, while_mat));
    }

    world.add(make_shared<translate>(make_shared<rotate_y>(make_shared<bvh_node>(boxes2), 15), vec3(-100, 270, 395)));

    camera cam;

    cam.aspect_ratio = 1.0;
    cam.image_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.look_from = point3(478, 278, -600);
    cam.look_at = point3(278, 278, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main()
{
    // checkered_spheres();
    // draw_earth();
    switch (9)
    {
    case 1:
        bouncing_spheres();
        break;
    case 2:
        checkered_spheres();
        break;
    case 3:
        draw_earth();
        break;
    case 4:
        draw_perlin_spheres();
        break;
    case 5:
        draw_quads();
        break;
    case 6:
        draw_simple_light();
        break;

    case 7:
        cornell_box();
        break;
    case 8:
        cornell_box_frog();
        break;

    case 9:
        book_two_final_scene(800, 10000, 40);
        break;
    default:
        book_two_final_scene(400, 250, 4);
        break;
    }
}

// #include "utils.h"
// #include <iostream>
// #include <iomanip>

// int main()
// {
//     std::cout << std::fixed << std::setprecision(12);

//     // int inside_circle = 0;
//     // int n_count = 0;

//     // for (int i = 0; i < n_count; i++)
//     // {
//     //     auto x = random_double(-1, 1);
//     //     auto y = random_double(-1, 1);

//     //     if (x * x + y * y < 1.0)
//     //         inside_circle++;
//     // }

//     // while (true)
//     // {
//     //     n_count++;
//     //     auto x = random_double(-1, 1);
//     //     auto y = random_double(-1, 1);
//     //     if (x * x + y * y < 1.0)
//     //         inside_circle++;
//     //     if (n_count % 100000 == 0)
//     //         std::cout << "Estimate of PI = " << (4.0 * inside_circle) / n_count << std::endl;
//     // }

//     // std::cout << "Estimate of PI = " << (4.0 * inside_circle) / n_count << std::endl;

//     int inside_circle = 0;
//     int inside_circle_stratified = 0;
//     int sqrt_N = 1000;

//     for (int i = 0; i < sqrt_N; i++)
//     {
//         for (int j = 0; j < sqrt_N; j++)
//         {
//             auto x = random_double(-1, 1);
//             auto y = random_double(-1, 1);
//             if (x * x + y * y < 1.0)
//             {
//                 inside_circle++;
//             }

//             x = 2 * (i + random_double()) / sqrt_N - 1;
//             y = 2 * (j + random_double()) / sqrt_N - 1;
//             if (x * x + y * y < 1.0)
//             {
//                 inside_circle_stratified++;
//             }
//         }
//     }

//     std::cout
//         << "Regular    Estimate of Pi = "
//         << (4.0 * inside_circle) / (sqrt_N * sqrt_N) << '\n'
//         << "Stratified Estimate of Pi = "
//         << (4.0 * inside_circle_stratified) / (sqrt_N * sqrt_N) << '\n';
// }