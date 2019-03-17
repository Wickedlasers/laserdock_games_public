//
// Created by Darren Otgaar on 2018/09/10.
//

#ifndef LASERDOCKCORE_BOUND_H
#define LASERDOCKCORE_BOUND_H

#include "ldRazer.h"

namespace ldRazer
{

    enum BoundType
    {
        BT_POINT,
        BT_LINE,
        BT_DISC,
        BT_HULL,
        BT_LINEMESH
    };

    struct ldBound
    {
        BoundType type;

        union
        {
            vec2f point;
            line2f_t line;
            disc2f_t disc;
            hull2f_t hull;
        };

        // TODO: Might be preferable to use the mesh as vertex storage for all SAT-derived tests to simplify operation and
        // clean up code - using a unique_ptr to prevent allocating vectors in all bounds
        std::unique_ptr<lineMesh2f_t> meshPtr;

        ldBound() : type(BT_POINT), point(0.f, 0.f)
        {
        }

        explicit ldBound(const vec2f &point) : type(BT_POINT), point(point)
        {
        }

        explicit ldBound(const line2f_t &line) : type(BT_LINE), line(line)
        {
        }

        explicit ldBound(const disc2f_t &disc) : type(BT_DISC), disc(disc)
        {
        }

        explicit ldBound(const hull2f_t &hull) : type(BT_HULL), hull(hull)
        {
        }

        explicit ldBound(const lineMesh2f_t &mesh) : type(BT_LINEMESH), meshPtr(new lineMesh2f_t(mesh))
        {
        }

        ~ldBound() = default;

        bool isPoint() const
        {
            return type == BT_POINT;
        }

        bool isLine() const
        {
            return type == BT_LINE;
        }

        bool isDisc() const
        {
            return type == BT_DISC;
        }

        bool isHull() const
        {
            return type == BT_HULL;
        }

        bool isMesh() const
        {
            return type == BT_LINEMESH;
        }

        ldBound(const ldBound &rhs) : type(rhs.type)
        {
            switch (rhs.type) {
                case BT_POINT:
                    point = rhs.point;
                    break;
                case BT_LINE:
                    line = rhs.line;
                    break;
                case BT_DISC:
                    disc = rhs.disc;
                    break;
                case BT_HULL:
                    hull = rhs.hull;
                    break;
                case BT_LINEMESH:
                    meshPtr = std::unique_ptr<lineMesh2f_t>(new lineMesh2f_t(*rhs.meshPtr));
                    break;
            }
        }

        ldBound &operator=(const ldBound &rhs)
        {
            if (this != &rhs) {
                type = rhs.type;
                switch (rhs.type) {
                    case BT_POINT:
                        point = rhs.point;
                        break;
                    case BT_LINE:
                        line = rhs.line;
                        break;
                    case BT_DISC:
                        disc = rhs.disc;
                        break;
                    case BT_HULL:
                        hull = rhs.hull;
                        break;
                    case BT_LINEMESH:
                        meshPtr = std::unique_ptr<lineMesh2f_t>(new lineMesh2f_t(*rhs.meshPtr));
                        break;
                }
            }
            return *this;
        }

        ldBound &operator=(const vec2f &point)
        {
            type = BT_POINT;
            this->point = point;
            return *this;
        }

        ldBound &operator=(const line2f_t &line)
        {
            type = BT_LINE;
            this->line = line;
            return *this;
        }

        ldBound &operator=(const disc2f_t &disc)
        {
            type = BT_DISC;
            this->disc = disc;
            return *this;
        }

        ldBound &operator=(const hull2f_t &hull)
        {
            type = BT_HULL;
            this->hull = hull;
            return *this;
        }

        ldBound &operator=(const lineMesh2f_t &mesh)
        {
            type = BT_LINEMESH;
            this->meshPtr = std::unique_ptr<lineMesh2f_t>(new lineMesh2f_t(mesh));
            return *this;
        }
    };

}

#endif //LASERDOCKCORE_BOUND_H
