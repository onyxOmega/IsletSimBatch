/*
	I don't know what this class does, and it's not currently implemented.
*/

class ChR2Current
{
	private:
		double O11;
		double O21;
		double C11;
		double C21;

		double Ga1;
		double Ga2;
		double dC1;
		double dC2;
		double dO1;
		double dO2;

	public:
		void calc_derivs(double,double,double,double,double,double,double,double,double);
};