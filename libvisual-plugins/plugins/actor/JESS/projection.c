#include <math.h>

void rotation_3d (float *x, float *y, float *z, float alpha, float beta,
		float gamma)
{
	float nx, ny;
	float cosga = cos (gamma), singa = sin (gamma), cosal = cos (alpha), sinal =
		sin (alpha);


	nx = cosga * *x - singa * *y;
	ny = cosal * (singa * *x + cosga * *y) - sinal * *z;
	*z = sinal * (singa * *x + cosga * *y) + cosal * *z;

	*x = nx;
	*y = ny;

}

void perspective (float *x, float *y, float *z, int persp, int dist_cam)
{
	float foc_cam = 100,aux;
	aux = (foc_cam + persp) / (dist_cam + persp - *z);
	*x = *x * aux;
	*y = *y * aux;
}

