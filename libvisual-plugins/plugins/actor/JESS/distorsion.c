#include <math.h>
#include <stdlib.h>
#include <stdio.h>  
#include <unistd.h>

#include "distorsion.h"
#include "def.h"
#include "jess.h"

void create_tables(JessPrivate *priv)
{
	int i, j, k, x, y;
	float n_fx, n_fy;
	int resy, resx;

	resy = priv->resy;
	resx = priv->resx;

	for (k=1; k<5; k++)
	{
		printf ("Computing table number %i\n", k);
		for (i = 0; i < priv->resy; i++)
		{
			for (j = 0; j < priv->resx; j++)
			{
				n_fx = (float) j - priv->xres2;
				n_fy = (float) i - priv->yres2;

				switch(k)
				{
					case 1:
						rot_hyperbolic_radial (&n_fx, &n_fy, -PI / 5, 0.001, 0,
								RESFACTY (50)) ;
						rot_hyperbolic_radial (&n_fx, &n_fy, PI / 2, 0.004,
								RESFACTX (200), RESFACTY (-30)) ;
						rot_hyperbolic_radial (&n_fx, &n_fy, PI / 5, 0.001,
								RESFACTX (-150), RESFACTY (-30)) ;
						rot_hyperbolic_radial (&n_fx, &n_fy, PI / 30, 0.0001, 0, 0) ;
						break;
					case 2:
						rot_cos_radial(&n_fx,&n_fy, 2*PI/75, 0.01,000,000) ; 
						break;
					case 3:
						homothetie_hyperbolic(&n_fx, &n_fy, 0.0005,0,0) ; 
						break;
					case 4:
						noize(&n_fx, &n_fy, 0*5.0);
						/*	  rot_hyperbolic_radial (&n_fx, &n_fy, PI / 30, 0.00010, 0, 0) ;  */
						/*	  homothetie_hyperbolic(&n_fx, &n_fy, -0.0002,0,0) ;  */
						/* 	  homothetie_cos_radial(&n_fx, &n_fy, 0.01,-10,10) ;  */
						break;
				}

				x = (int) (n_fx + priv->xres2);
				y = (int) (n_fy + priv->yres2);

				if (x < 0 || x >= priv->resx  || y < 0 || y >= priv->resy )
				{
					x = 0;
					y = 0;
				}

				switch(k)
				{
					case 1:
						priv->table1[i * resx + j] = x + y * resx;
						break;
					case 2:
						priv->table2[i * resx + j] = x + y * resx;
						break;
					case 3:
						priv->table3[i * resx + j] = x + y * resx;
						break;
					case 4:
						priv->table4[i * resx + j] = x + y * resx;
						break;
				}
			}
		}
	}
}

void rot_hyperbolic_radial(float *n_fx,float *n_fy,float d_alpha, float rad_factor, float cx, float cy)
{
	float r2,dx = *n_fx-cx, dy = *n_fy-cy,cosal, sinal;

	r2 = (dx)*(dx)+(dy)*(dy); 

	d_alpha*=1/(1+r2*rad_factor);

	cosal = cos(d_alpha) ;
	sinal = sin(d_alpha) ;

	*n_fx = cx + dx * cosal - dy * sinal;
	*n_fy = cy + dx * sinal + dy * cosal;
}

void rot_cos_radial( float *n_fx,float *n_fy,float d_alpha, float rad_factor, float cx, float cy)
{
	float r,dx = *n_fx-cx, dy = *n_fy-cy,cosal, sinal;
	r = sqrt(dx*dx+dy*dy); 

	d_alpha=d_alpha*cos(r*rad_factor); 

	cosal = cos(d_alpha) ;
	sinal = sin(d_alpha) ;

	*n_fx = cx + dx * cosal - dy * sinal;
	*n_fy = cy + dx * sinal + dy * cosal;
}

void homothetie_hyperbolic(float *n_fx,float *n_fy, float rad_factor, float cx, float cy)
{
	float r,dx = *n_fx-cx, dy = *n_fy-cy;
	r = sqrt(dx*dx+dy*dy); 

	*n_fx = cx + dx/(1+rad_factor*r);
	*n_fy = cy + dy/(1+rad_factor*r);  
}


void homothetie_cos_radial(float *n_fx,float *n_fy, float rad_factor, float cx, float cy)
{
	float r,cosrad, dx = *n_fx-cx, dy = *n_fy-cy;
	r = sqrt(dx*dx+dy*dy); 

	cosrad = cos(r*rad_factor);

	*n_fx = cx + dx*cosrad;
	*n_fy = cy + dy*cosrad;  
}

void noize(float *n_fx,float *n_fy, float intensity)
{
	*n_fx +=2*((float)rand()/RAND_MAX-0.5)*intensity;
	*n_fy +=2*((float)rand()/RAND_MAX-0.5)*intensity-5; 
}

