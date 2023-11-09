struct CollisionsParams {
    public: 
        bool enable;
        float elastic;
        bool torus;
    
    CollisionsParams(bool enable, float elastic, bool torus) {
        this->enable = enable;
        this->elastic = elastic;
        this->torus = torus;
    }
};

struct CarParams {
    public:
        int w; int h; int m; int r;
        int vel_max; int acc_max;
		double vel_p; 
        double check_time;
        double back_time;
		double distance;
		double senst; double gas;
		double max_speed_ue;
		double rot_on_speed;
    
    CarParams(int w, int h, int m, int r, int max_vel, int max_acc, 
		double vel_p, double check_time, double back_time, double distance,
		double sensitivity, double max_speed_ue, double rot_on_speed) {
        this->w = w;
        this->h = h;
        this->m = m;
        this->r = r;
        this->vel_max = max_vel;
        this->acc_max = max_acc;
		this->vel_p = vel_p;
		this->check_time = check_time;
		this->back_time = back_time;
		this->distance = distance;
		this->senst = sensitivity;
		this->max_speed_ue = max_speed_ue;
		this->rot_on_speed = rot_on_speed;
    }

};

struct Force1Params {
    public:
        int lm_r; float lm_v;

        Force1Params(int lm_r, float lm_v) {
            this->lm_r = lm_r;
            this->lm_v = lm_v;
        }
};

struct Force2Params {
    public:
        int lm_r; float lm_v;
        int kind;
		int mu; int a; int rho; double rhoMin;
		double f3_p;

        Force2Params(int lm_r, float lm_v, int kind, int mu, int a, int rho, double rhoMin, double f3_p) {
            this->lm_r = lm_r;
            this->lm_v = lm_v;
            this->kind = kind;
            this->mu = mu;
            this->a = a;
			this->rho = rho;
			this->rhoMin = rhoMin;
			this->f3_p = f3_p;
        }
};

struct Params {
    
    public:
        int fps_draw = 60;    // drawing
        int fps_phys = 100;   // physics
        int num_cars = 1;    // number of cars
        CarParams car = CarParams(
                    5,      // w - half-sizes
                    8,      // h - collisions
                    1,      // m - mass
                    5,      // r - rad
                    100,   // max_vel - px/sec
                    200,	// max_acc - px/sec^2
					100.00,	// vel_p
					2.00,	// check_time
					2.00,	// back_time
                    150.,    // distance
					1.,		// sensitivity
					1500.,	// max speed UE
					.4		// rotate on speed
			);   
        CollisionsParams collisions = CollisionsParams(
                    false,   // enable - car-car
                    0.2,    // elastic - 
                    false); // torus - periodic border
        Force1Params force1 = Force1Params(
                    10,   // lm_r - pos to dest
                    0.1);   // lm_v - vel to dest
        Force2Params force2 = Force2Params(
                    10,     // lm_r - coupling const 
                    0.1,    // lm_v - coupling const
                    1,      // kind - sigmoid or Yukawa
                    1,      // mu - 
                    3,      // a - 
                    4000,   // rho max - visibility radius
					500.,	// rhoMin
					2. // f3_p
			);

        Params() {}
};