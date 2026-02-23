#ifndef HEADER_Vec2D
#define HEADER_Vec2D

#include <cmath>

/*
Basic vector class. Supports addition, subtraction, multiplication(returns (x1*x2, y1*y2))
and some basic functions
*/

class Vec2D{
    public:
    float X;
    float Y;

    Vec2D(float x, float y) {X=x;Y=y;}
    Vec2D() {X=0;Y=0;}

    float magnitude()
    {
        return std::sqrtf(X*X+Y*Y);
    }

    //Edits the vector
    void normalize()
    {
        float m = magnitude();
        X = X/m;
        Y = Y/m;
    }

    //Returns the nomralised vector. Does not edit the current vector
    Vec2D getNormal()
    {
        Vec2D v;
        float m = magnitude();
        v.X = X/m;
        v.Y = Y/m;
        return v;
    }

    Vec2D operator+(const Vec2D &v)
    {
        return Vec2D(X+v.X, Y+v.Y);
    }

    Vec2D operator-(const Vec2D &v)
    {
        return Vec2D(X-v.X, Y-v.Y);
    }

    Vec2D operator*(const Vec2D &v)
    {
        return Vec2D(X*v.X, Y*v.Y);
    }

    Vec2D operator+=(const Vec2D &v)
    {
        X += v.X;
        Y += v.Y;
        return *this;
    }
    Vec2D operator-=(const Vec2D &v)
    {
        X -= v.X;
        Y -= v.Y;
        return *this;
    }

    Vec2D operator*(const float num)
    {
        return Vec2D(X*num, Y*num);
    }

    Vec2D operator/(const float num)
    {
        return Vec2D(X/num, Y/num);
    }

    Vec2D operator-()
    {
        return Vec2D(-X, -Y);
    }

    static float DotProduct(const Vec2D &v1, const Vec2D &v2)
    {
        return v1.X*v2.X + v1.Y*v2.Y;
    }

    static float CrossProduct(const Vec2D &v1, const Vec2D &v2)
    {
        return v1.X*v2.Y - v1.Y*v2.X;
    }

    static Vec2D CrossProductZWithVector(float Z, const Vec2D &v1)
    {
        return Vec2D(-(Z*v1.Y), Z*v1.X);
    }

    static Vec2D GetPerpendicular(const Vec2D &v1)
    {
        return Vec2D(-v1.Y, v1.X);
    }

    static float Distance(const Vec2D &v1, const Vec2D &v2)
    {
        return std::sqrtf((v1.X-v2.X)*(v1.X-v2.X) + (v1.Y-v2.Y)*(v1.Y-v2.Y));
    }
};

#endif