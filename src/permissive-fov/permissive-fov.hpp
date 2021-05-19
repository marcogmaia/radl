// permissive-fov-cpp.h


/* Copyright (c) 2007, Jonathon Duerig. Licensed under the BSD
   license. See LICENSE.txt for details. */

#ifndef PERMISSIVE_FOV_CPP_H_DUERIG
#define PERMISSIVE_FOV_CPP_H_DUERIG

#include "permissive-fov.h"

namespace permissive {

// A C++ extension to the C interface in permissive-fov.h which
// yields better type-safety.
//
// void permissiveFovPlus(int sourceX, int sourceY,
//                        int radius, char * mask,
//                        T & context)
//
// Calculate precise permissive field of view sourced from the point
// (sourceX, sourceY).
//
// sourceX, sourceY, radius, mask are as described in permissive-fov.h
//
// context is of a templated type T.
// T as defined in this function must define the following two
// public methods:
//
// class T
// {
// public:
//   bool isBlocked(int destX, int destY);
//   void visit(int destX, int destY);
// };

template <class T>
class fovPrivateT {
public:
    static int isBlocked(int destX, int destY, void* context) {
        T* typedContext = reinterpret_cast<T*>(context);
        return typedContext->isBlocked(destX, destY);
    }

    static void visit(int destX, int destY, void* context) {
        T* typedContext = reinterpret_cast<T*>(context);
        typedContext->visit(destX, destY);
    }
};

class maskT {
public:
    maskT(int north = 0, int south = 0, int east = 0, int west = 0) {
        initPermissiveMask(&mask, north, south, east, west);
    }

    maskT(char const* fileName) {
        loadPermissiveMask(&mask, fileName);
    }

    ~maskT() {
        cleanupPermissiveMask(&mask);
    }

    void saveMask(char const* fileName) {
        savePermissiveMask(&mask, fileName);
    }

    void set(int x, int y) {
        setPermissiveVisit(&mask, x, y);
    }

    void clear(int x, int y) {
        clearPermissiveVisit(&mask, x, y);
    }

    bool doesVisit(int x, int y) {
        return doesPermissiveVisit(&mask, x, y) == 1;
    }

    permissiveMaskT* getMask(void) {
        return &mask;
    }

private:
    permissiveMaskT mask;
};

template <class T>
void squareFov(int sourceX, int sourceY, int radius, T& context) {
    permissiveSquareFov(sourceX, sourceY, radius, fovPrivateT<T>::isBlocked,
                        fovPrivateT<T>::visit,
                        reinterpret_cast<void*>(&context));
}

template <class T>
void fov(int sourceX, int sourceY, maskT& mask, T& context) {
    permissiveFov(sourceX, sourceY, mask.getMask(), fovPrivateT<T>::isBlocked,
                  fovPrivateT<T>::visit, reinterpret_cast<void*>(&context));
}
}  // namespace permissive

#endif
