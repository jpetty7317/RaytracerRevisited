#ifndef OCTREE_H
#define OCTREE_H

#include "aabb.h"
#include "model.h"
#include "utilities.h"

#include <tuple>
#include <array>

static const float MIN_CELL_SIZE = 1.0f;

class node : public hittable
{
public:
    node(){}

    node(const point3& c, const point3& s) : center{center}, 
                                                bounds{point3{c - (s * 0.5f)}, point3{c + (s * 0.5f)}}, 
                                                nodeSize{s.x()}{}

    void addObject(shared_ptr<model> model)
    {
        if(bounds.intersects(model->bounds))
        {
            float quarter = nodeSize / 4.0f;
            float extent = nodeSize * 0.5f;

            float bMax = std::max({model->bounds.size().x(), model->bounds.size().y(), model->bounds.size().z()});

            if(canSplit(extent, bMax))
            {
                if(children.empty())
                    divide(quarter, extent);

                for(node& n : children)
                    n.addObject(model);
            }
            else
            {   
                containedModels.push_back(model);
            }
        }
    }

    bool hit(const ray& r, interval rayT, hitRecord& rec) const override
    {
        if(!bounds.hit(r, rayT))
            return false;

        bool hitMesh = false;
        if(!containedModels.empty())
        {
            for(auto m : containedModels)
            {
                if(m->hit(r, rayT, rec))
                    hitMesh = true;
            }
        }

        if(!hitMesh && !children.empty())
        {
            for(const node& n : children)
            {
                n.hit(r, rayT, rec);
            }
        }

        return hitMesh;
    }

private:
    float nodeSize{};
    point3 center {};
    aabb bounds{};
    std::vector<node> children;
    std::vector<shared_ptr<model>> containedModels;

    bool canSplit(float extent, float objectSize)
    {
        return (extent > objectSize) && (extent > MIN_CELL_SIZE);
    }

    void divide(float quarter, float size)
    {
        vec3 size3 {size};
        children.push_back({center + vec3{-quarter, quarter, quarter}, size3});
        children.push_back({center + vec3{quarter, quarter, -quarter}, size3});
        children.push_back({center + vec3{-quarter, quarter, -quarter}, size3});
        children.push_back({center + vec3{quarter, quarter, quarter}, size3});
        children.push_back({center + vec3{-quarter, -quarter, quarter}, size3});
        children.push_back({center + vec3{quarter, -quarter, -quarter}, size3});
        children.push_back({center + vec3{-quarter, -quarter, -quarter}, size3});
        children.push_back({center + vec3{quarter, -quarter, quarter}, size3});
    }
};

class octree
{
public:
    octree(){}

    octree(const point3& center, float size) : root { center, vec3{size} } {}

    const node& getRoot() const { return root; }

    void addObject(shared_ptr<model> model) { root.addObject(model); }

private:
    node root;
};

#endif