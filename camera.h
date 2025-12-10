#ifndef CAMERA
#define CAMERA

#include "utils.h"
#include "hittable.h"

class camera {
public:
    double aspact_ratio = 16.0 / 9.0;
    int    image_width  = 720;
    int    samples_per_pixel = 10;
    int    max_depth = 10;

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

    void initialize() {
        // Image
        image_height = int(image_width / aspact_ratio);
        image_height = (image_height < 1) ? 1 : image_height; // ensure height at least 1

        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Camera
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width) / image_height);
        camera_center = point3(0, 0, 0);

        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        // horizontal and vertical delta vectors between pixels
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;
        
        // Calculate the location of upper left pixel.
        auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    ray get_ray(int i, int j) const {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
        auto ray_origin = camera_center;
        auto ray_direction = pixel_sample - ray_origin;
        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    color ray_color(const ray& r, int depth, const hittable& world) {
        if (depth <= 0) {
            return color(0, 0, 0);
        }

        hit_record rec;
        if (world.hit(r, interval(0.001, infinity), rec)){
            // return 0.5 * color(rec.normal + color(1, 1, 1));
            // vec3 direction = random_on_hemisphere(rec.normal);
            vec3 direction = rec.normal + random_unit_vector();
            return 0.5 * ray_color(ray(rec.p, direction), depth - 1, world);
        }
        vec3 normalized_dirction = normalize(r.direction());
        auto a = 0.5 * (normalized_dirction.y() + 1);
        color white(1.0, 1.0, 1.0);
        color light_blue(0.5, 0.7, 1.0);
        return (1 - a) * white + a * light_blue;
    }

};


#endif