#ifndef TLAS_H
#define TLAS_H

#include "model.h"

#include <vector>
#include <stack>

class tlas
{
private:
    struct tlasNode
    {
        aabb bounds{};
        int leftRight = 0;
        int blas = 0;

        bool isLeaf() { return leftRight == 0; }
    };

    std::vector<tlasNode> tlasNodes;
    std::vector<shared_ptr<model>>* blas;
    int nodesUsed;
    int blasCount;

    int findBestMatch(int* nodeIdx, int N, int A)
    {
        float smallest = infinity;
        int bestB = -1;

        for(int B = 0; B < N; B++)
        {
            if(B != A)
            {
                tlasNode& nodeA = tlasNodes[nodeIdx[A]];
                tlasNode& nodeB = tlasNodes[nodeIdx[B]];

                vec3 bmax { vmax(nodeA.bounds.max(), nodeB.bounds.max()) };
                vec3 bmin { vmin(nodeA.bounds.min(), nodeB.bounds.min()) };
                vec3 e = bmax - bmin;
                float surfaceArea = e.x() * e.y() + e.y() * e.z() + e.z() * e.x();
                if(surfaceArea < smallest)
                {
                    smallest = surfaceArea;
                    bestB = B;
                }
            }
        }

        return bestB;
    }

    void build()
    {
        int nodeIdx[blasCount];
        int nodeIndices = blasCount;

        nodesUsed = 1;
        for(int i = 0; i < blasCount; i++)
        {
            nodeIdx[i] = nodesUsed;
            tlasNodes[nodesUsed].bounds.min() = (*blas)[i]->mbvh.bvhBounds.min();
            tlasNodes[nodesUsed].bounds.max() = (*blas)[i]->mbvh.bvhBounds.max();
            tlasNodes[nodesUsed].blas = i;
            tlasNodes[nodesUsed].leftRight = 0;
            nodesUsed++;
        }

        int A = 0;
        int B = findBestMatch(nodeIdx, nodeIndices, A);
        while(nodeIndices > 1)
        {
            int C = findBestMatch(nodeIdx, nodeIndices, B);
            if(A == C)
            {
                int nodeIdxA = nodeIdx[A];
                int nodeIdxB = nodeIdx[B];
                tlasNode& nodeA = tlasNodes[nodeIdxA];
                tlasNode& nodeB = tlasNodes[nodeIdxB];
                tlasNodes[nodesUsed].leftRight = nodeIdxA + (nodeIdxB << 16);
                tlasNodes[nodesUsed].bounds.min() = vmin(nodeA.bounds.min(), nodeB.bounds.min());
                tlasNodes[nodesUsed].bounds.max() = vmax(nodeA.bounds.max(), nodeB.bounds.max());
                nodeIdx[A] = nodesUsed++;
                nodeIdx[B] = nodeIdx[nodeIndices - 1];
                B = findBestMatch(nodeIdx, --nodeIndices, A);
            }
            else
            {
                A = B;
                B = C;
            }
        }
        tlasNodes[0] = tlasNodes[nodeIdx[A]];
    }

public:
    tlas (){}

    tlas (std::vector<shared_ptr<model>>* b, int N) : blas(b), blasCount(N)
    {
        tlasNodes.resize(2 * blasCount);
        build();
    }

    void hit(ray& r)
    {
        tlasNode* n = &tlasNodes[0];
        std::stack<tlasNode*> stack;
        while(true)
        {
            if(n->isLeaf())
            {
                (*blas)[n->blas]->mbvh.hit(r);
                
                if(stack.size() == 0)
                    break;

                n = stack.top();
                stack.pop();
                
                continue;
            }

            tlasNode* child1 = &tlasNodes[n->leftRight & 0xffff];
            tlasNode* child2 = &tlasNodes[n->leftRight >> 16];

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
