class Logger {

	public:
		double angle = 0;
		double lenght = 0;
		double dt = 0;
		double rot = 0;
		double newAngle = 0;
		double newAngleLimit = 0;

		Vec force1;
		Vec force2;
		Vec force3;

		Logger() {}
		Logger(double a, double b, double d, double e, double f) {
			this->angle = a;
			this->lenght = b;
			this->rot = d;
			this->newAngle = e;
			this->newAngleLimit = f;
		};
		Logger(Vec a) {
			this->force3 = a;
		}
		Logger(Vec a, Vec b, Vec c) {
			this->force1 = a;
			this->force2 = b;
			this->force3 = c;
		}
};