#pragma once

struct NumThreads
{
    NumThreads(unsigned int x, unsigned int y, unsigned int z) :
        x_(x), y_(y), z_(z)
    {}

    unsigned int x_, y_, z_;
};