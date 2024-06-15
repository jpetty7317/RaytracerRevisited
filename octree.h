#ifndef OCTREE_H
#define OCTREE_H

#include "aabb.h"
#include "model.h"
#include "utilities.h"

#include <tuple>
#include <array>

static const float MIN_CELL_SIZE = 155.0f;

class octree;

class node : public hittable
{
public:
    bool hasContentInChildren = false;

    node(){}

    node(const point3& c, const point3& s, std::vector<shared_ptr<model>>* gml) : center{c}, 
                                                                                        bounds{point3{c - (s * 0.5f)}, point3{c + (s * 0.5f)}}, 
                                                                                        nodeSize{s.x()},
                                                                                        globalModelList{gml} {}

    void addObject(shared_ptr<model> model, int modelListIndex)
    {
        if(bounds.intersects(model->bounds))
        {
            float quarter = nodeSize / 4.0f;
            float extent = nodeSize * 0.5f;

            if(canSplit(extent))
            {
                if(children.empty())
                    divide(quarter, extent);

                for(node& n : children)
                {
                    n.addObject(model, modelListIndex);
                    if(n.hasContentInChildren)
                        hasContentInChildren = true;
                }
            }
            else
            {
                for(int t = 0; t < model->triangles.size(); t++)
                {
                    if(model->triangles[t]->intersectsAABB(bounds))
                    {
                        modTriIndices.push_back(make_shared<interval>(modelListIndex, t));
                        hasContentInChildren = true;
                    }
                } 
            }
        }
    }

    bool hit(const ray& r, interval rayT, hitRecord& rec) const override
    {
        if(!hasContentInChildren)
            return false;

        if(!bounds.hit(r, rayT))
            return false;

        bool hitAnything = false;
        hitRecord tempRec;
        float closestSoFar = rayT.max;
        if(!children.empty())
        {
            for(const node& n : children)
            {
                if(n.hit(r, interval{rayT.min, closestSoFar}, tempRec))
                {
                    hitAnything = true;
                    if(tempRec.t <= closestSoFar)
                    {
                        closestSoFar = tempRec.t;
                        rec = tempRec;
                    }
                }
            }
        }
        else
        {
            if(!modTriIndices.empty())
            {
                for(shared_ptr<interval> i : modTriIndices)
                {
                    if((*globalModelList)[i->min]->triangles[i->max]->hit(r, interval{rayT.min, closestSoFar}, tempRec))
                    {
                        hitAnything = true;
                        if(tempRec.t <= closestSoFar)
                        {
                            closestSoFar = tempRec.t;
                            rec = tempRec;
                        }
                    }
                }
            }
        }

        return hitAnything;
    }

private:
    std::vector<shared_ptr<model>>* globalModelList;
    float nodeSize{};
    point3 center {};
    aabb bounds{};
    std::vector<node> children;
    std::vector<shared_ptr<interval>> modTriIndices;

    bool canSplit(float extent)
    {
        return extent > MIN_CELL_SIZE;
    }

    void divide(float quarter, float size)
    {
        vec3 size3 {size};
        children.push_back({center + vec3{-quarter, quarter, quarter}, size3, globalModelList});
        children.push_back({center + vec3{quarter, quarter, -quarter}, size3, globalModelList});
        children.push_back({center + vec3{-quarter, quarter, -quarter}, size3, globalModelList});
        children.push_back({center + vec3{quarter, quarter, quarter}, size3, globalModelList});
        children.push_back({center + vec3{-quarter, -quarter, quarter}, size3, globalModelList});
        children.push_back({center + vec3{quarter, -quarter, -quarter}, size3, globalModelList});
        children.push_back({center + vec3{-quarter, -quarter, -quarter}, size3, globalModelList});
        children.push_back({center + vec3{quarter, -quarter, quarter}, size3, globalModelList});
    }
};

class octree
{
public:
    octree(){}

    octree(const point3& center, float size, std::vector<shared_ptr<model>>* gml) : root { center, vec3{size}, gml } {}

    const node& getRoot() const { return root; }

    void addObject(shared_ptr<model> model, int index) { root.addObject(model, index); }

private:
    node root;
};

#endif
