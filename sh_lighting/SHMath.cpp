#include "SHMath.h"

// Basic integer factorial
/*
const int factorial_tab[17] = 
{
	1,
	1,
	2,
	6,
	24,
	120,
	720,
	5040,
	40320,
	362880,
	3628800,
	39916800,
	479001600,
	6227020800,
	87178291200,
	1307674368000,
	19615115520000,
};
*/
const int factorial_tab[13] = 
{
	1,
	1,
	2,
	6,
	24,
	120,
	720,
	5040,
	40320,
	362880,
	3628800,
	39916800,
	479001600,
};

int factorial(int v)
{
	return factorial_tab[v];
}

// polynomial function
double P(int l, int m, double x)
{
	double pmm = 1;
	if(m>0)
	{
		double somx2 = sqrt(1-x*x);
		double fact = 1;
		for(int i=1; i<=m; i++)
		{
			pmm *= -1 * fact * somx2; 
			fact += 2; 
		} 
	} 
	
	if(l==m) return pmm; 
		
	double pmmp1 = x * (2*m+1) * pmm; 
	
	if(l==m+1) return pmmp1; 
		
	double pll = 0; 
	
	for(int ll=m+2; ll<=l; ll++) 
	{ 
		pll = ( (2.0*ll-1.0)*x*pmmp1-(ll+m-1.0)*pmm ) / (ll-m); 
		
		pmm = pmmp1; 
		pmmp1 = pll; 
	} 
	
	return pll; 
}

double K(int l, int m) 
{ 
 // renormalisation constant for SH function 
	double temp = ((2*l+1)*factorial(l-m)) / (4*PI*factorial(l+m));
 return (sqrt(temp)); 
}

 // return a point sample of a Spherical Harmonic basis function 
 // l is the band, range [0..N] 
 // m in the range [-l..l] 
 // theta in the range [0..Pi] 
 // phi in the range [0..2*Pi] 
double SHMath::SH(int l, int m, double theta, double phi) 
{ 
 const double sqrt2 = sqrt(2.0); 
	
 if(m==0) return K(l,0)*P(l,m,cos(theta));
	 
 else if(m>0) return sqrt2*K(l,m)*cos(m*phi)*P(l,m,cos(theta));
	 
 else return sqrt2*K(l,-m)*sin(-m*phi)*P(l,-m,cos(theta)); 
}
//:~