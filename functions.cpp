#include "functions.h"
#include "CImg.h"
#include "Object.h"
/*Vec3f cast_ray(const Ray& r, const AABBox& box) {
 float box_dist = std::numeric_limits<float>::max();
 if (!box.intersect(r, box_dist)) {
     return Vec3f(255, 255, 255); // background color
 }
 return Vec3f(0.4, 0.4, 0.3);
}*/
Vec3f normalize(const Vec3f& v)
{
    float mag2 = v.x * v.x + v.y * v.y + v.z * v.z;
    if (mag2 > 0) {
        float invMag = 1 / sqrtf(mag2);
        return Vec3f(v.x * invMag, v.y * invMag, v.z * invMag);
    }
    return v;
}

Vec3f crossProduct(const Vec3f& a, const Vec3f& b)
{
    return Vec3f(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}
Options GetSettings(std::string FileIn, std::string FileOut) {

    Options options;

    std::ifstream in(FileIn);
    if (!in)
    {
        std::cout << "Settings file doesn't exist" << std::endl;
        return options;
    }

    for (std::string line; std::getline(in, line); )
    {
        std::istringstream in(line);
        std::string option;
        float x, y, z, val;
        in >> option;

        if (option == "cam") {
            in >> x >> y >> z;
            options.camera = Vec3f(x, y, z);
        }
        else if (option == "target") {
            in >> x >> y >> z;
            options.target = Vec3f(x, y, z);
        }
        else if (option == "up") {
            in >> x >> y >> z;
            options.vup = normalize(Vec3f(x, y, z));
        }
        else if (option == "distance") {
            in >> val;
            options.dist_to_screen = val;
        }
        else if (option == "limit") {
            in >> val;
            options.max_dist = val;
        }
        else if (option == "alpha") {
            in >> val;
            options.fov = val;
        }
        else if (option == "width") {
            in >> val;
            options.width = val;
        }
        else if (option == "height") {
            in >> val;
            options.height = val;
        }
        else {
            std::cout << "Smth wrong in 'settings.txt'" << std::endl;
        }
    }

    options.output_file = FileOut;
    options.backgroundColor = Vec3f(252, 255, 255);
    options.normal = normalize((options.camera - options.target));
    options.right = normalize(crossProduct(options.normal, options.vup));
    options.vup = normalize(crossProduct(options.right, options.normal));
    options.cameraToWorld = Matrix44(options.right[0], options.right[1], options.right[2], 0,
        options.vup[0], options.vup[1], options.vup[2], 0,
        options.normal[0], options.normal[1], options.normal[2], 0,
        options.camera[0], options.camera[1], options.camera[2], 1);
    in.close();
    return options;

}
float deg2rad(const float& deg)
{
    return deg * M_PI / 180;
}

bool trace(
    const Ray& r,
    const std::vector<Object*>& objects,
    float& tNear, Object** hitObject, Options options)
{
    *hitObject = nullptr;
#pragma omp parallel for
    for (int k = 0; k < objects.size(); ++k) {
        float tNearK = options.max_dist;
        if (objects[k]->intersect(r,tNearK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
        }
    }
    return (*hitObject != nullptr);
}

Vec3f castRay(
    const Ray&r,
    const std::vector<Object*>& objects,
    const Options& options)
{
    Vec3f hitColor = options.backgroundColor;
    float tnear = options.max_dist;
    Object* hitObject = nullptr;
    if (trace(r, objects, tnear, &hitObject, options)) {
        if (options.num_objects == 1) {
            hitColor = Vec3f(64, 64, 64);
        }
        else {
            hitColor = Vec3f(64, 64, 64) + Vec3f(static_cast<int>(127 / (options.num_objects - 1))) * (hitObject->pos - 1);
        }
    }
    return hitColor;
}

void render(
    const Options& options,
    const std::vector<Object*>& objects)
{
    float scale = tan(deg2rad(options.fov * 0.5)) * options.dist_to_screen;
    float imageAspectRatio = options.width / (float)options.height;
    Vec3f orig, tmp;
    float color[3];
    options.cameraToWorld.multVecMatrix(Vec3f(0), orig);//aaaaaaaaaaaaaaa
    cimg_library::CImg<float> img(options.width, options.height, 1, 3);
#pragma omp parallel for private(color,tmp)
    for (int j = 0; j < options.height; ++j) {
        #pragma omp parallel for
        for (int i = 0; i < options.width; ++i) {
            float x = (2 * (i + 0.5) / (float)options.width - 1) * imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)options.height) * scale;
            Vec3f dir;
            options.cameraToWorld.multDirMatrix(Vec3f(x, y, -1), dir);
            normalize(dir);
            Ray r(orig, dir);
            tmp = castRay(r, objects, options);
            color[0] = tmp[0];
            color[1] = tmp[1];
            color[2] = tmp[2];
            img.draw_point(i, j, color);
        }
    }
    img.save(options.output_file.c_str());
}
std::vector<Object*> GetFigures(std::string FileName, Options& options) {

    std::vector< Object*> objects;
    std::ifstream in(FileName);
    //in.open(R"(полный путь к файлу)");
    if (!in)
    {
        std::cout << "Figures file doesn't exist!" << std::endl;
        return objects;
    }
    float tmp[6];
    options.num_objects = 0;

    for (std::string line; std::getline(in, line); )
    {
        std::istringstream in(line);
        std::string type;
        in >> type;
        if (type == "box") {
            options.num_objects++;
            in >> tmp[0] >> tmp[1] >> tmp[2] >>
                tmp[3] >> tmp[4] >> tmp[5];
            Vec3f A = (tmp[0], tmp[1], tmp[2]);
            Vec3f B = (tmp[3], tmp[4], tmp[5]);
            Object* a = new AABBox(A, B);
            objects.push_back(a);
            float dist = dotProduct((objects.back()->getMassCenter()-options.camera), (objects.back()->getMassCenter() - options.camera));
            std::cout << dist << std::endl;
            options.distances.insert(std::make_pair(dist, objects.back()));
        }
        else {
            std::cout << "Wrong info in 'figures.txt'" << std::endl;
            return objects;
        }
    }
    std::map<int, Object*> tmp1;
    std::map<Object*, int> poss;
    int i = 1;
    for (std::map<float, Object*>::const_iterator it = options.distances.begin();
        it != options.distances.end(); ++it) {
        tmp1.insert(std::make_pair(i, it->second));
        i++;
    }

    for (std::map<int, Object*>::const_iterator it = tmp1.begin();
        it != tmp1.end(); ++it) {
        poss.insert(std::make_pair(it->second, it->first));
    }

    for (std::vector<Object*>::iterator it = objects.begin(); it != objects.end(); ++it) {
        (*it)->pos = poss.find(*it)->second;
    }
    in.close();
    return objects;
}
void autotest1() { // Front view
    Options options = GetSettings("settings1.txt", "view1.bmp");
    std::vector<Object*> objects = GetFigures("figures.txt", options);
    if (objects.size() == 0) {
        return;
    }
    render(options, objects);

    for (int i = 0; i < objects.size(); i++) {
        delete objects[i];
    }

    std::cout << "Test1: OK (view 1) " << std::endl;
}
void autotest2() { // Side view

    Options options = GetSettings("settings2.txt", "view2.bmp");
    std::vector<Object*> objects = GetFigures("figures.txt", options);
    if (objects.size() == 0) {
        return;
    }
    render(options, objects);

    for (int i = 0; i < objects.size(); i++) {
        delete objects[i];
    }

    std::cout << "Test2: OK (view 2)" << std::endl;
}

void autotest3() { // Back view

    Options options = GetSettings("settings3.txt", "view3.bmp");
    std::vector<Object*> objects = GetFigures("figures.txt", options);
    if (objects.size() == 0) {
        return;
    }
    render(options, objects);

    for (int i = 0; i < objects.size(); i++) {
        delete objects[i];
    }

    std::cout << "Test3: OK (view 3)" << std::endl;
}