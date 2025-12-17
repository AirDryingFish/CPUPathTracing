#ifndef CAMERA
#define CAMERA

#include "utils.h"
#include "hittable.h"
#include "material.h"

class camera {
public:
    double aspact_ratio      = 16.0 / 9.0;
    int    image_width       = 720;
    int    samples_per_pixel = 10;
    int    max_depth         = 10;
    double vfov              = 90;
    vec3   vup               = vec3(0, 1, 0);
    point3 look_from         = point3(0, 0, 0);
    point3 look_at           = point3(0, 0, -1);

    double focus_distance    = 10.0;
    double defocus_angle       = 0.0;

    void render(const hittable& world) {
        initialize();
        std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
        for (int j = 0; j < image_height; j++){
            std::clog << "\rScanline remaining: " << image_height - j << ' ' << std::flush; 
            for (int i = 0; i < image_width; i++){
                color pixel_color = color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_color * pixel_samples_scale);
            }
        }
        std::clog << "\rDone.               \n";
    }

private:
    int image_height;
    point3 camera_center;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    double pixel_samples_scale;
    point3 pixel00_loc;
    vec3 u, v, w;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    void initialize() {
        // Image
        image_height = int(image_width / aspact_ratio);
        image_height = (image_height < 1) ? 1 : image_height; // ensure height at least 1

        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Camera
        // auto focal_length = 1.0;
        camera_center = look_from;
        // auto focal_length = (look_at - look_from).length();

        auto theta = degree_to_radians(vfov);
        auto h = std::tan(theta / 2);
        
        // auto viewport_height = 2.0 * h * focal_length;
        auto viewport_height = 2.0 * h * focus_distance;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        w = normalize(look_from - look_at);
        u = normalize(cross(vup, w));
        v = cross(w, u);

        // auto viewport_u = vec3(viewport_width, 0, 0);
        // auto viewport_v = vec3(0, -viewport_height, 0);
        auto viewport_u = viewport_width * u;
        auto viewport_v = -viewport_height * v;

        // horizontal and vertical delta vectors between pixels
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        
        // Calculate the location of upper left pixel.
        auto viewport_upper_left = camera_center - focus_distance * w - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = std::tan(degree_to_radians(defocus_angle / 2)) * focus_distance;
        defocus_disk_u = defocus_radius * u;
        defocus_disk_v = defocus_radius * v;
    }

    ray get_ray(int i, int j) const {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
        // auto ray_origin = camera_center;
        auto ray_origin = defocus_angle <= 0 ? camera_center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double(); // time for motion blur
        return ray(ray_origin, ray_direction, ray_time);
    }


    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    vec3 defocus_disk_sample() const {
        auto p = random_in_unit_disk();
        return camera_center + p.x() * defocus_disk_u + p.y() * defocus_disk_v;
    }

    color ray_color(const ray& r, int depth, const hittable& world) {
        if (depth <= 0) {
            return color(0, 0, 0);
        }

        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)){
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            return color(0, 0, 0);
        }
        vec3 normalized_dirction = normalize(r.direction());
        auto a = 0.5 * (normalized_dirction.y() + 1);
        color white(1.0, 1.0, 1.0);
        color light_blue(0.5, 0.7, 1.0);
        return (1 - a) * white + a * light_blue;
    }

};


#endif