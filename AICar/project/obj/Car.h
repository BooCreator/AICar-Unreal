#include "JSON.h"
#include "Params.h"
#include <algorithm>
#include <iostream>

#define M_PI           3.14159265358979323846  /* pi */

class Car {

public:
    Vec pos; Vec vel; Vec dir;
    int width; int height; int rad;
    int weight;
    int maxVel; int maxAcc;
	double vel_coef; double actualySpeed;

    time_t check_c=0; time_t back_c=0; Vec lastPos; bool back = false;
    /* -----------------------------------------------------------------
     * constructors
     * -------------------------------------------------------------- */
    Car(Vec pos, Vec vel, int w = 8, int h = 12, int r = 6, int maxVel = 100, int maxAcc = 100, Vec dir = Vec(), double mass = 1) {
        this->width = w;        // половина ширины (поперёк dir)
        this->height = h;       // половина длины  (вдоль dir)
        this->rad = r;          // радиус сферы столкновения c центром в pos
        this->weight = mass;    // масса машины для колизий
        this->maxVel = maxVel;  // максимальная скорость машины
        this->maxAcc = maxAcc;  // максимальное ускорение машины

        this->pos = pos;        // вектор положения центра масс машины
        this->lastPos = pos;    // вектор положения центра масс машины
		this->initVel(vel);		// вектор скорости машины

        this->dir = dir;        // вектор продольной оси машины (см. метод setDir)
		if (this->dir.is_zero())
			this->setDir();
    }
    //---------------------------------------------------------------------------
    void init(Vec nPos, Vec nVel, Vec nDir) {
        this->pos = nPos;        // вектор положения центра масс машины
		this->initVel(nVel);		// вектор скорости машины

        this->dir = nDir;        // вектор продольной оси машины (см. метод setDir)
		if (this->dir.is_zero())
			this->setDir();
    }
    //---------------------------------------------------------------------------
    void initVel(Vec newVel) {
		//this->maxAcc = newVel.length();
		this->actualySpeed = newVel.length();
        this->vel = newVel;
    }
    //---------------------------------------------------------------------------
    // Обрезать скорость на максимальном значении
    // 
    void velInc(Vec p, double mul = 1.) { // local only
        this->vel += p * mul;
        this->vel = this->vel.clip(this->maxVel);
    }
    Vec clipAcc(Vec a) { return a.clip(this->maxAcc); }
	Vec clipVel(Vec a) { return a.clip(this->maxVel); }
    //---------------------------------------------------------------------------
    // Установить ориентацию машины в соответствии с её скоростью
    // ----------------------------------------------------------------------------
    void setDir() {
        if (this->dir.is_zero()) {
            if (this->vel.is_zero()) {
                this->dir = Vec(0, -1);
                return;
            }
        }
        else {
            if (this->vel.is_zero())
                return;               // не меняем направления
        }
        this->dir = this->vel.unit();
    }
    //---------------------------------------------------------------------------
    // Отражение от стенок ящика (x1,y1)-(x2,y2) или телепортация по тору
    //
    void bounding_box(int x1, int y1, int x2, int y2, double elast = 1., bool torus = false, float push = 1.5) {
        if (this->pos.x <= x1 + this->rad) {
            if (torus) {
                this->pos.x = x2 - push * this->rad;
            }
            else {
                this->pos.x = x1 + push * this->rad;
                this->vel.x = -this->vel.x * elast;
            }
        }
        else if (this->pos.x >= x2 - this->rad) {
            if (torus) {
                this->pos.x = x1 + push * this->rad;
            }
            else {
                this->pos.x = x2 - push * this->rad;
                this->vel.x = -this->vel.x * elast;
            }
        }
        if (this->pos.y <= y1 + this->rad) {
            if (torus) {
                this->pos.y = y2 - push * this->rad;
            }
            else {
                this->pos.y = y1 + push * this->rad;
                this->vel.y = -this->vel.y * elast;
            }
        }
        else if (this->pos.y >= y2 - this->rad) {
            if (torus) {
                this->pos.y = y1 + push * this->rad;
            }
            else {
                this->pos.y = y2 - push * this->rad;
                this->vel.y = -this->vel.y * elast;
            }
        }
    }
    //------------------------------------------------------
    // Столкновение двух машин
    //
    double collision(Car p, double elast = 1., float push = 1.5) {
        Vec r = Vec(p.pos.x - this->pos.x, p.pos.y - this->pos.y);
        double r2 = r ^ r;
        if (r2 > (this->rad + p.rad) * (this->rad + p.rad))
            return 0;                          // шарики не столкнулись

        // разводятся шарики
        Vec n = r * (1 / sqrt(r2));
        Vec c = (((this->pos + p.pos) * 0.5) + (n * 0.5 * (this->rad - p.rad)));
        this->pos = (c + (n * -this->rad * push));  // сильно разводим это всё жн не шары!
        p.pos = (c + (n * p.rad * push));

        r = Vec(p.pos.x - this->pos.x, p.pos.y - this->pos.y);
        r2 = r ^ r;

        // меняются скорости
        Vec v = Vec(this->vel.x - p.vel.x, this->vel.y - p.vel.y);
        Vec dv = (r * (2 / (this->weight + p.weight)) * (r ^ v / r2));

        this->vel += (dv * -p.weight);
        p.vel += (dv * this->weight);
        this->vel.x *= elast;  this->vel.y *= elast;
        p.vel.x *= elast;  p.vel.y *= elast;
        return dv.length();
    }

    Vec find_collision(Car p, double elast = 1., float push = 1.5) {
        Vec r = Vec(p.pos.x - this->pos.x, p.pos.y - this->pos.y);
        double r2 = r ^ r;
        if (r2 > (this->rad + p.rad) * (this->rad + p.rad))
            return 0;                          // шарики не столкнулись

        // разводятся шарики
        Vec n = r * (1 / sqrt(r2));
        Vec c = (((this->pos + p.pos) * 0.5) + (n * 0.5 * (this->rad - p.rad)));
        this->pos = (c + (n * -this->rad * push));  // сильно разводим это всё жн не шары!
        p.pos = (c + (n * p.rad * push));

        r = Vec(p.pos.x - this->pos.x, p.pos.y - this->pos.y);
        r2 = r ^ r;

        // меняются скорости
        Vec v = Vec(this->vel.x - p.vel.x, this->vel.y - p.vel.y);
        Vec dv = (r * (2 / (this->weight + p.weight)) * (r ^ v / r2));

        //this->vel += (dv * -p.weight);
        //this->vel.x *= elast;  this->vel.y *= elast;
        Vec res = (this->vel + (dv * -p.weight)) * elast;
        return res;
    }

    double _r(double p) { return p * 180 / (atan(1) * 4); }

    //------------------------------------------------------
       // Столкновение двух машин
       //

	double rotateOld(Vec force) {
		double c = 0.6981317008;
		double angle = atan2(force.y, force.x);

		double rot = atan2(this->dir.y, this->dir.x);
		double newAngle = angle - rot;
		if (newAngle > M_PI) newAngle = -(newAngle - M_PI);
		double newAngleLimit = (((-c) > (newAngle)) ? (-c) : (newAngle));
		newAngleLimit = (((c) < (newAngleLimit)) ? (c) : (newAngleLimit));
		return newAngleLimit / c;
	}

	double rotate(Vec force, double p=1.) {
		double c = M_PI/p;

		double rot = atan2(this->dir.y, this->dir.x);
		force.rotate(-rot);
		double angle = atan2(force.y, force.x);

		double newAngleLimit = (((-c) > (angle)) ? (-c) : (angle));
		newAngleLimit = (((c) < (newAngleLimit)) ? (c) : (newAngleLimit));
		newAngleLimit = newAngleLimit / c;
		return newAngleLimit;
	}

    double toOnes(double v) {
        if (v < -1.) return -1.;
        if (v > 1.) return 1.;
        return v;
    }

	double frand(double val, int p = 100) { return float(rand() % int(val*p)) / float(p); }

	double getRand(double val, double scale=1.) { return this->frand(val) * scale; }

	JSON toJSON(Vec force, double dist, time_t t, CarParams params) {
		//double mappedValue = this->rotateOld(force);
		force = (this->vel + force).clip(this->maxAcc);

		//force = force.clip(this->maxAcc);

		double mappedValue = this->rotate(force, params.senst);
		double len = (this->pos | this->lastPos);
        double acc = 0.;
		Vec force1 = Vec();
		Vec force2 = this->lastPos;
		Vec force3 = Vec(len, t - this->check_c);
        if (t - this->check_c >= 1000 * (params.check_time-this->getRand(params.check_time, .15))) {
            if (len > 0 && len < params.distance) {
                this->back = true;
				this->back_c = t;
			} else {
				this->lastPos = this->pos;
			}
			this->check_c = t;
        }
		
        if (this->back) { 
            acc = -1.;
			mappedValue = -mappedValue;
			if (t - this->back_c >= 1000 * (params.back_time - this->getRand(params.back_time, .5))) {
				this->back_c = t;
				this->back = false;
				this->lastPos = this->pos;
				force1 = Vec(t, this->back_c);
			}
        } else {
            double speed = this->vel.length();
            double split = (dist - speed);
            if (speed > split) {
                acc = (split - speed) / dist + 0.2;
            } else if (speed < split - params.vel_p) {
                acc = 1.;
            } else {
                acc = (split - speed) / params.vel_p;
            }
			if (abs(mappedValue) > params.rot_on_speed && speed > params.vel_p) acc *= 1. - abs(mappedValue);
        }
        return JSON(force, toOnes(acc), mappedValue, Logger(force1, force2, force3));
	}

    //--------------------------------------------------------------------------
    //                       Только локальная часть
    //--------------------------------------------------------------------------

};