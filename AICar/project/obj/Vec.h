#include <math.h>

class Vec {
    
    public:
        double x;
        double y;

    /* -----------------------------------------------------------------
     * constructors
     * -------------------------------------------------------------- */

    // ->zero()
    Vec(): Vec(0.) { }
    Vec(double p): Vec(p, p) { }
    Vec(double x, double y) { this->x = x; this->y = y;}

    /* -----------------------------------------------------------------
     * functions
     * -------------------------------------------------------------- */

	Vec fromAngle(double a, double len) { this->x = len * cos(a); this->y = len * sin(a); return *this; }

    Vec clip(double val) { return (this->length() > val) ? this->unit(val) : Vec(*this); }

    double length() { return sqrt(this->x*this->x + this->y*this->y); }

    bool is_zero() { return this->x == 0 && this->y == 0; }

    Vec unit(int len=1, double eps=1e-8) { 
        double cur_len = this->length()+eps; 
        return Vec(this->x*len/cur_len, this->y*len/cur_len); 
    }

	double angle() { return atan2(this->y, this->x); }

    double angle(Vec p) {
        double s = (this->x*p.x + this->y*p.y);
        double a = this->length();
        double b = p.length();
        double res = s/(a*b);
        return acos((res > 1) ? 1.0f : res);
    }

    void rotate(double a) {
		auto X = this->x; auto Y = this->y;
		this->x = X * cos(a) - Y * sin(a);
		this->y = Y * cos(a) + X * sin(a);
	}

    /* -----------------------------------------------------------------
     * operators overloading
     * -------------------------------------------------------------- */

    Vec operator+(Vec p) { return Vec(this->x+p.x, this->y+p.y); }
    void operator+=(Vec p) { this->x += p.x; this->y+= p.y; }

    Vec operator-(Vec p) { return Vec(this->x-p.x, this->y-p.y); }
    void operator-=(Vec p) { this->x -= p.x; this->y-= p.y; }

    void operator-() { this->x = -this->x; this->y = -this->y; }

    Vec operator*(double s) { return Vec(this->x*s, this->y*s); }
    void operator*=(double s) { this->x *= s; this->y*= s; }

    Vec operator*(Vec p) { return Vec(this->x*p.x, this->y*p.y); }
    void operator*=(Vec p) { this->x *= p.x; this->y *= p.y; }

    Vec operator/(double s) { return Vec(this->x/s, this->y/s); }
    void operator/=(double s) { this->x /= s; this->y/= s; }

    // ->dot(Vec p)
    double operator^(Vec p) { return this->x*p.x + this->y*p.y; }

    // ->dist(Vec p)
    double operator|(Vec p) { return sqrt(pow(this->x-p.x, 2)+pow(this->y-p.y, 2)); }

    /* -----------------------------------------------------------------
     * deprecated
     * -------------------------------------------------------------- */

    //double dot(Vec p) { return this->x*p.x + this->y*p.y; }
    //void inverse() { this->x = -this->x; this->y = -this->y; }
    //double dist2(Vec p1, Vec p2){ return pow(p1.x-p2.x, 2)+pow(p1.y-p2.y, 2); }
    //double dist(Vec p) { return pow(this->x-p.x, 2)+pow(this->y-p.y, 2); }
    //void inc(Vec p, double mul=1.) { this->x += p.x * mul; this->y += p.y * mul; } Vec += Vec p * mul
    //void scale(double sx, double sy) { this->x *= sx;  this->y *= sy; } -> Vec * Vec

};