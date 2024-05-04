/*
** EPITECH PROJECT, 2024
** B-OOP-400-TLS-4-1-raytracer-leo.wehrle [WSL: Ubuntu]
** File description:
** World
*/

#pragma once

#include <memory>
#include <vector>

#include "Lights/ILight.hpp"
#include "Primitives/IPrimitive.hpp"
#include "Ray.hpp"
#include "Interval.hpp"
#include "HitRecord.hpp"

class World : public IPrimitive {
public:
    World() = default;
    ~World() = default;

    std::vector<std::shared_ptr<IPrimitive>> primitives;

    void addPrimitive(std::shared_ptr<IPrimitive> primitive) { primitives.push_back(move(primitive)); }

    bool hit(const Ray& r, Interval ray_d, HitRecord& rec) const override
    {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_d.max;

        for (const auto& object : objects) {
            if (object->hit(r, interval(ray_d.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }


protected:
private:
};
