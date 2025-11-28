#ifndef CAMERA
#define CAMERA

#include "utils.h"
#include "hittable.h"

class camera {
public:
    double aspact_ratio = 16.0 / 9.0;
    int    image_width  = 720;

    void render(const hittable& world) {
        initialize();
        std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
        for (int j = 0; j < image_height; j++){
            std::clog << "\rScanline remaining: " << image_height - j << ' ' << std::flush; 
            for (int i = 0; i < image_width; i++){
                auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - camera_center;
                ray r(camera_center, ray_direction);
                color pixel_color = ray_color(r, world);
                write_color(std::cout, pixel_color);
            }
        }
        std::clog << "\rDone.               \n";
    }


private:
    int image_height;
    point3 camera_center;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    point3 pixel00_loc;

    void initialize() {
        // Image
        image_height = int(image_width / aspact_ratio);
        image_height = (image_height < 1) ? 1 : image_height; // ensure height at least 1

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

    color ray_color(const ray& r, const hittable& world) {
        hit_record rec;
        if (world.hit(r, interval(0, infinity), rec)){
            return 0.5 * color(rec.normal + color(1, 1, 1));
        }
        vec3 normalized_dirction = normalize(r.direction());
        auto a = 0.5 * (normalized_dirction.y() + 1);
        color white(1.0, 1.0, 1.0);
        color light_blue(0.5, 0.7, 1.0);
        return (1 - a) * white + a * light_blue;
    }

};


#endif