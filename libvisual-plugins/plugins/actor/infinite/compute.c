#include <config.h>

#include <math.h>
#include "renderer.h"
#include "compute.h"

#define PI 3.14159

static t_complex _inf_fct(InfinitePrivate *priv, t_complex a,int n,int p1,int p2)   //p1 et p2:0-4 
{
	t_complex b;
	float fact;
	float an;
	float circle_size;
	float speed;
	float co,si;
	
	a.x-=priv->plugwidth/2;
	a.y-=priv->plugheight/2;
	
	switch (n) {
		
	case 0:
		an=0.025*(p1-2)+0.002;
		co=cos(an);
		si=sin(an);
		circle_size=priv->plugheight*0.25;
		speed=2000+p2*500;
		b.x=(co*a.x-si*a.y);
		b.y=(si*a.x+co*a.y);
		fact=-(sqrt(b.x*b.x+b.y*b.y)-circle_size)/speed+1;
		b.x=(b.x*fact);
		b.y=(b.y*fact);
		break;
		
	case 1:
		an=0.015*(p1-2)+0.002;
		co=cos(an);
		si=sin(an);
		circle_size=priv->plugheight*0.45;
		speed=4000+p2*1000;
		b.x=(co*a.x-si*a.y);
		b.y=(si*a.x+co*a.y);
		fact=(sqrt(b.x*b.x+b.y*b.y)-circle_size)/speed+1;
		b.x=(b.x*fact);
		b.y=(b.y*fact);
		break;
		
	case 2:
		an=0.002;
		co=cos(an);
		si=sin(an);
		circle_size=priv->plugheight*0.25;
		speed=400+p2*100;
		b.x=(co*a.x-si*a.y);
		b.y=(si*a.x+co*a.y);  
		fact=-(sqrt(b.x*b.x+b.y*b.y)-circle_size)/speed+1;
		b.x=(b.x*fact);
		b.y=(b.y*fact);
		break;
		
	case 3:
		an=(sin(sqrt(a.x*a.x+a.y*a.y)/20)/20)+0.002;
		co=cos(an);
		si=sin(an);
		circle_size=priv->plugheight*0.25;
		speed=4000;
		b.x=(co*a.x-si*a.y);
		b.y=(si*a.x+co*a.y);
		fact=-(sqrt(b.x*b.x+b.y*b.y)-circle_size)/speed+1;
		b.x=(b.x*fact);
		b.y=(b.y*fact);
		break;
		
	case 4:
		an=0.002;
		co=cos(an);
		si=sin(an); 
		circle_size=priv->plugheight*0.25;
		speed=sin(sqrt(a.x*a.x+a.y*a.y)/5)*3000+4000;
		b.x=(co*a.x-si*a.y);
		b.y=(si*a.x+co*a.y);
		fact=-(sqrt(b.x*b.x+b.y*b.y)-circle_size)/speed+1;
		b.x=(b.x*fact);
		b.y=(b.y*fact);    
		break;
		
	case 5:
		b.x=a.x*1.02;
		b.y=a.y*1.02;
		break;

	case 6:
		an=0.002;
		co=cos(an);
		si=sin(an); 
		circle_size=priv->plugheight*0.25;
		fact=1+cos(atan(a.x/(a.y+0.00001))*6)*0.02;
		b.x=(co*a.x-si*a.y);
		b.y=(si*a.x+co*a.y);
		b.x=(b.x*fact);
		b.y=(b.y*fact);    
		break;	
		
	}  
 
	b.x+=priv->plugwidth/2;
	b.y+=priv->plugheight/2;  
	if (b.x<0)
		b.x=0;
	if (b.y<0)
		b.y=0;
	if (b.x>priv->plugwidth-1)
		b.x=priv->plugwidth-1;
	if (b.y>priv->plugheight-1)
		b.y=priv->plugheight-1;     
	
	return b;
}


static void _inf_generate_sector(InfinitePrivate *priv, int g,int f,int p1,int p2,int debut,int step,t_interpol* vector_field)
{
	int fin=debut+step;
	const int prop_transmitted=249;
	const int b_add=g*priv->plugwidth*priv->plugheight; 
	t_coord c;


	if (fin>priv->plugheight)
		fin=priv->plugheight;
	for (c.y=debut;c.y<fin;c.y++) 
		for (c.x=0;c.x<priv->plugwidth;c.x++) {
			t_complex a;
			float fpy;
			int rw,lw,add;
			uint32_t w1,w2,w3,w4;
			uint32_t x,y;

			a.x=(float)c.x;
			a.y=(float)c.y;
			a=_inf_fct(priv, a,f,p1,p2);
			add=c.x+c.y*priv->plugwidth;
			x=(int)(a.x);
			y=(int)(a.y);
			vector_field[b_add+add].coord=(x<<16)|y;

			fpy=a.y-floor(a.y);
			rw=(int)((a.x-floor(a.x))*prop_transmitted);
			lw=prop_transmitted-rw;
			w4=(int)(fpy*rw);
			w2=rw-w4;
			w3=(int)(fpy*lw);
			w1=lw-w3; 
			vector_field[b_add+add].weight=(w1<<24)|(w2<<16)|(w3<<8)|w4;
		}
}

void _inf_generate_vector_field(InfinitePrivate *priv, t_interpol* vector_field) 
{
	int f;
	int i,p1,p2;
    
	for (f=0;f<NB_FCT;f++) {
		p1=2;
		p2=2;
		for (i=0;i<priv->plugheight;i+=10)
			_inf_generate_sector(priv, f,f,p1,p2,i,10,vector_field);
	}
}

