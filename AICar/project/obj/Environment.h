#include "Car.h"

#include <cstdlib>
#include <vector>
#include <chrono>
#include <ctime> 


class Environment
{

public:
    int X1; int X2; int Y1; int Y2;
    Params params;
    std::vector<Car> objects;
    std::vector<Vec> targets;
    time_t lastPhys;

    Logger log;

    Environment(int Width, int Height) {
        this->X1 = 0; this->Y1 = 0;
        this->X2 = Width; this->Y2 = Height;
        this->lastPhys = this->getTime();
        this->params = this->readParams();
    }
    //--------------------------------------------------------------------------
    Params readParams() { return Params(); }
    //--------------------------------------------------------------------------
    time_t getTime() {
        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            );
        return ms.count();
    }
    //--------------------------------------------------------------------------
    std::vector<JSON> step() {
		time_t t = this->getTime();
        std::vector<JSON> forceArr;
        forceArr = this->phys(t);
        return forceArr;
    }
    //--------------------------------------------------------------------------
    //                       Физика и принятие решения
    //--------------------------------------------------------------------------
    std::vector<JSON> phys(time_t t) {
        auto forceArr = std::vector<JSON>();
        for (int i = 0; i < this->objects.size(); i++) {
            Car* car = &this->objects[i];
			Vec target = this->targets[i];
            if (i > -1) {
				double rho = car->actualySpeed / this->params.car.max_speed_ue * this->params.force2.rho;
				double coeff = car->actualySpeed / this->params.car.max_speed_ue * this->params.force2.f3_p;
                Vec force1 = this->force_des1(i, this->params.force1.lm_r);
                Vec force2 = this->force_des2(i, this->params.force1.lm_v);
                Vec force3 = this->force_near(i, this->params.force2.lm_r, this->params.force2.lm_v, 
					this->params.force2.a, this->params.force2.mu, 
					(rho < this->params.force2.rhoMin) ? this->params.force2.rhoMin : rho);
                //force3 = force3.unit(force1.length()*coeff);
                Vec force = ((force1 + force2) + force3);
                forceArr.push_back(car->toJSON(force, car->pos|target, t, this->params.car));
            }
        }
        return forceArr;
    }
    //--------------------------------------------------------------------------
    std::vector<Vec> collision() {
        auto collisionArr = std::vector<Vec>();
        for (int i = 0; i < this->objects.size(); i++) {
            Car* car = &this->objects[i];
            for (int j = 0; j < this->objects.size(); j++) {
                if (i == j) continue;
                Vec x = car->find_collision(this->objects[j], this->params.collisions.elastic);
                collisionArr.push_back(x);
            }
        }
        return collisionArr;
    }
    //--------------------------------------------------------------------------
    Vec force_des1(int i, int lm, double eps = 1e-8) {
        auto pos = this->objects[i].pos;
        auto des = this->targets[i];
        pos = (pos - des);
        return pos.unit(-lm, eps);
    }
    //--------------------------------------------------------------------------
    Vec force_des2(int i, float lm, double eps = 1e-8) {
        Vec pos = this->objects[i].pos;
        Vec vel = this->objects[i].vel;
        Vec des = this->targets[i];

        pos = (pos - des);
        double vR = vel ^ pos;
        double v2 = vel ^ vel;
        double R = pos.length();
        Vec f = Vec(vel.x * vR - pos.x * v2, vel.y * vR - pos.y * v2);
        double mult = lm / (R + eps);
        f *= mult;
        return f;
    }
    //--------------------------------------------------------------------------
    Vec force_near(int i, int lm1, float lm2, int a, int mu, int R, double eps = 1e-8) {
        Car car = this->objects[i];
        Vec pi = car.pos;
        Vec sum = Vec();
        for (int j = 0; j < this->objects.size(); j++)
            if (j != i) {
                Vec pj = this->objects[j].pos;
                Vec pos = (pi - pj);
                Vec vel = (car.vel - this->objects[j].vel);
                double d2 = pos ^ pos;
                if (d2 < R * R) {
                    double r = sqrt(d2) / (2 * car.rad);
                    pos *= Vec(0.5 / car.rad);

                    double potential = 1.;
                    if (this->params.force2.kind == 1) {
                        potential = (1 + exp(mu * (1 - a))) / (1 + exp(mu * (r - a)));
                    }
                    else {
                        potential = exp(-mu * (r - 1)) / (pow(r, a) + eps);
                    }

                    double vR = vel ^ pos;
                    double v2 = vel ^ vel;
                    Vec f = Vec(vel.x * vR - pos.x * v2, vel.y * vR - pos.y * v2);

                    sum += pos * (lm1 * potential / (r + eps));
                    sum += f * (-lm2 * potential / (r + eps));
                }
            }
        return sum;
    }

    //--------------------------------------------------------------------------
    //                       Только локальная часть
    //--------------------------------------------------------------------------
    void initGame() {
        int N = this->params.num_cars;

        this->objects = {};
        this->targets = {};

        for (int i = 0; i < N; i++) {
            Vec pos = this->randPoint();
            Vec vel = Vec(2 * rand() - 1, 2 * rand() - 1);
            vel = vel.unit(0.5 * this->params.car.vel_max);
            if (i == 0) {
                vel = Vec();
                pos = Vec(0.5 * (this->X2 + this->X1), 0.5 * (this->Y2 + this->Y1));
            }
            Car car = Car(pos, vel, this->params.car.w, this->params.car.h, this->params.car.r,
                this->params.car.vel_max, this->params.car.acc_max);
            this->objects.push_back(car);
            this->targets.push_back(this->randPoint());
        }
    }
    //--------------------------------------------------------------------------
    double frand(int p = 100) { return float(rand() % p) / p; }
    //--------------------------------------------------------------------------
    Vec randPoint(int gap = 50) {
        double p = this->frand();
        int x = this->X1 + gap + (this->X2 - this->X1 - 2 * gap) * (this->frand());
        int y = this->Y1 + gap + (this->Y2 - this->Y1 - 2 * gap) * (this->frand());
        return Vec(x, y);
    }
    //--------------------------------------------------------------------------
    void Tick() {
        time_t t = this->getTime();
        if (t - this->lastPhys >= 1000 / this->params.fps_phys) {
            auto dt = float(t - this->lastPhys) / 1000.0f;
            auto forceArr = this->step();
            this->move(forceArr, dt); // двигаю машинки (для UE не нужно)
            this->check_targets(); // проверяю таргеты
            this->lastPhys = t;
        }
    }
    //--------------------------------------------------------------------------
    void move(std::vector<JSON> forces, double dt) {
        for (int i = 0; i < this->objects.size(); i++) {
            Car* car = &this->objects[i];
			Vec force = car->clipAcc(forces[i].force);
            car->velInc(force, dt);
            car->setDir();
            car->pos += (car->vel * dt);
            car->bounding_box(this->X1, this->Y1, this->X2, this->Y2, this->params.collisions.elastic, this->params.collisions.torus);
        }
    }
    //--------------------------------------------------------------------------
    void check_targets() {
        for (int i = 0; i < this->objects.size(); i++) {
            Car* car = &this->objects[i];
            Vec des = this->targets[i];
            if ((car->pos | des) < car->rad * car->rad) { // reach to destination
                this->targets[i] = this->randPoint();
            }
        }
    }
    //--------------------------------------------------------------------------
};