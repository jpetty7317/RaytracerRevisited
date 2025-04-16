#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "triangle.h"

#include <stack>

#define BINS 8

class bvh
{
private:
    struct bin
    {
        aabb bounds{};
        int triCount = 0;
    };

    struct bvhNode
    {
        aabb bounds{};
        int leftFirst = 0;
        int triCount = 0;

        bool isLeaf() { return triCount > 0; }
    };

    std::vector<shared_ptr<triangle>>* triangles;
    std::vector<int> triIndices{};
    std::vector<bvhNode> bvhNodes{};
    int triCount = 0;
    int nodesUsed = 1;

    float findBestSplitPlane(const bvhNode& node, int& axis, float& splitPos)
    {
        float bestCost = infinity;
        axis = 0;
        splitPos = 0;
        for(int x = 0; x < 3; x++)
        {
            float boundsMin = infinity;
            float boundsMax = -infinity;
            for(int i = 0; i < node.triCount; i++)
            {
                shared_ptr<triangle> t = (*triangles)[triIndices[node.leftFirst + i]];
                boundsMin = std::min(boundsMin, t->centroid()[x]);
                boundsMax = std::max(boundsMax, t->centroid()[x]);
            }

            if(boundsMin == boundsMax)
                continue;

            bin bins[BINS];
            float scale = BINS / (boundsMax - boundsMin);
            for(int i = 0; i < node.triCount; i++)
            {
                shared_ptr<triangle> t = (*triangles)[triIndices[node.leftFirst + i]];
                int binIdx = std::min(BINS - 1, (int)((t->centroid()[x] - boundsMin) * scale));
                bins[binIdx].triCount++;
                bins[binIdx].bounds.grow(t->v0());
                bins[binIdx].bounds.grow(t->v1());
                bins[binIdx].bounds.grow(t->v2());
            }

            float leftArea[BINS - 1];
            float rightArea[BINS - 1];
            int leftCount[BINS - 1];
            int rightCount[BINS - 1];
            aabb leftBox{};
            aabb rightBox{};
            int leftSum = 0;
            int rightSum = 0;
            for(int i = 0; i < BINS - 1; i++)
            {
                leftSum += bins[i].triCount;
                leftCount[i] = leftSum;
                leftBox.grow(bins[i].bounds);
                leftArea[i] = leftBox.area();
                rightSum += bins[BINS - 1 - i].triCount;
                rightCount[BINS - 2 - i] = rightSum;
                rightBox.grow(bins[BINS - 1 - i].bounds);
                rightArea[BINS - 2 - i] = rightBox.area();
            }

            scale = (boundsMax - boundsMin) / BINS;
            for(int i = 0; i < BINS - 1; i++)
            {
                float planeCost = leftCount[i] * leftArea[i] + rightCount[i] * rightArea[i];
                if(planeCost < bestCost)
                {
                    splitPos = boundsMin + scale * (i + 1);
                    axis = x;
                    bestCost = planeCost;
                }
            }
        }
        return bestCost;
    }

    float calculateNodeCost(bvhNode& node)
    {
        vec3 e {node.bounds.max() - node.bounds.min()};
        float parentArea = e.x() * e.y() + e.y() * e.z() + e.z() * e.x();
        return node.triCount * parentArea;
    }

    void updateNodeBounds(int nodeIdx)
    {
        bvhNode& node = bvhNodes[nodeIdx];
        node.bounds.min() = vec3::posInf();
        node.bounds.max() = vec3::negInf();
        int first = node.leftFirst;
        for(int i = 0; i < node.triCount; i++)
        {
            int modIndex = triIndices[first + i];
            shared_ptr<triangle> t = (*triangles)[modIndex];
            node.bounds.min() = vmin(node.bounds.min(), t->v0());
            node.bounds.min() = vmin(node.bounds.min(), t->v1());
            node.bounds.min() = vmin(node.bounds.min(), t->v2());
            node.bounds.max() = vmax(node.bounds.max(), t->v0());
            node.bounds.max() = vmax(node.bounds.max(), t->v1());
            node.bounds.max() = vmax(node.bounds.max(), t->v2());
        }
    }

    void subdivide(int nodeIdx)
    {
        bvhNode& node = bvhNodes[nodeIdx];

        int axis{};
        float splitPos{};
        float splitCost = findBestSplitPlane(node, axis, splitPos);
        float noSplitCost = calculateNodeCost(node);

        if(splitCost >= noSplitCost)
            return;

        int i = node.leftFirst;
        int j = i + node.triCount - 1;
        while(i <= j)
        {
            if((*triangles)[triIndices[i]]->centroid()[axis] < splitPos)
            {
                i++;
            }
            else
            {
                std::swap(triIndices[i], triIndices[j]);
                j--;
            }
        }

        int leftCount = i - node.leftFirst;
        if(leftCount == 0 || leftCount == node.triCount)
            return;

        int leftChildIndex = nodesUsed++;
        int rightChildIndex = nodesUsed++;
        bvhNodes[leftChildIndex].leftFirst = node.leftFirst;
        bvhNodes[leftChildIndex].triCount = leftCount;
        bvhNodes[rightChildIndex].leftFirst = i;
        bvhNodes[rightChildIndex].triCount = node.triCount - leftCount;
        node.leftFirst = leftChildIndex;
        node.triCount = 0;

        updateNodeBounds(leftChildIndex);
        updateNodeBounds(rightChildIndex);

        subdivide(leftChildIndex);
        subdivide(rightChildIndex);
    }

    void build()
    {
        nodesUsed = 2;

        for(int i = 0; i < triCount; i++) 
            triIndices[i] = i;

        bvhNode& root = bvhNodes[0];
        root.leftFirst = 0;
        root.triCount = triCount;

        updateNodeBounds(0);
        subdivide(0);

        bvhBounds = {root.bounds.min(), root.bounds.max()};
    }
public:
    aabb bvhBounds{};

    bvh(){};

    bvh(std::vector<shared_ptr<triangle>>* t, int N) : triangles(t), triCount(N)
    {
        triIndices.resize(N);
        bvhNodes.resize(2 * N);
        build();
    }

    void hit(ray& r)
    {
        bvhNode* n = &bvhNodes[0];
        std::stack<bvhNode*> stack;
        while(true)
        {
            if(n->isLeaf())
            {
                for(int i = 0; i < n->triCount; i++)
                    (*triangles)[triIndices[n->leftFirst + i]]->hit(r);

                if(stack.size() > 0)
                {
                    n = stack.top();
                    stack.pop();
                }
                else
                    break;
                
                continue;
            }

            bvhNode* child1 = &bvhNodes[n->leftFirst];
            bvhNode* child2 = &bvhNodes[n->leftFirst + 1];

            float hit1, hit2;
            hit1 = child1->bounds.hit(r);
            hit2 = child2->bounds.hit(r);
            
            if(hit1 > hit2)
            {
                std::swap(hit1, hit2);
                std::swap(child1, child2);
            }

            if(hit1 == infinity)
            {
                if(stack.size() == 0)
                    break;

                n = stack.top();
                stack.pop();
            }
            else
            {
                n = child1;
                if(hit2 != infinity)
                    stack.push(child2);
            }
        }
    }
};

#endif
