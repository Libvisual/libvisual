/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Antti Silvast <asilvast@iki.fi>
 *
 * Authors: Antti Silvast <asilvast@iki.fi>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: main.c,v 1.3 2006/01/22 13:25:26 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <config.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "main.h"

void set_vector (Vect v, float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

int init_flower(FlowerInternal *flower)
{
	Vect *v;

	flower->tension = 0.0;
	flower->continuity = 0.0;
	flower->bias = 0.0;
	flower->tension_new = 0.0;
	flower->continuity_new = 0.0;
	flower->bias_new = 0.0;

	flower->rotx = -40.0;
	flower->roty = 0.0;
	flower->spd = 1.0;
	flower->posz = 0.0;
	flower->posz_new = 0.0;
	flower->audio_strength = 30.0;
	
	v = flower->kukka;

	set_vector (v[0], 0.0, 0.5, 0.0);
	set_vector (v[1], 0.0, 0.0, 0.0);
	set_vector (v[2], 0.1,-0.2, 0.0);
	set_vector (v[3], 0.3,-0.3, 0.0);
	set_vector (v[4], 0.5,-0.3, 0.0);
	set_vector (v[5], 0.6,-0.2, 0.0);
	set_vector (v[6], 0.6, 0.5, 0.0);

	v = flower->kukka_morph;

	set_vector (v[0], 0.0, 0.5, 0.0);
	set_vector (v[1], 0.0, 0.0, 0.0);
	set_vector (v[2], 0.2,-0.2, 0.0);
	set_vector (v[3], 0.3,-0.15, 0.0);
	set_vector (v[4], 0.2, 0.0, 0.0);
	set_vector (v[5], 0.1, 0.0, 0.0);
	set_vector (v[6], 0.0, 0.1, 0.0);
	
	return 0;
}

void splineTCP(FlowerInternal *flower, float u, Vect * control, Vect * result)
{

	(*result)[0] = (2*u*u*u - 3*u*u + 1)*control[1][0]
		+ (-2*u*u*u + 3*u*u)*control[2][0]
		+ (u*u*u - 2*u*u + u)*(0.5*(1-flower->tension)*((1+flower->bias)*(1-flower->continuity)*(control[1][0]-control[0][0])
					+ (1-flower->bias)*(1+flower->continuity)*(control[2][0]-control[1][0])))
		+ (u*u*u - u*u)*(0.5*(1-flower->tension)*((1+flower->bias)*(1+flower->continuity)*(control[2][0]-control[1][0])
					+ (1-flower->bias)*(1-flower->continuity)*(control[3][0]-control[2][0])));

	(*result)[1]
		= (2*u*u*u - 3*u*u + 1)*control[1][1]
		+ (-2*u*u*u + 3*u*u)*control[2][1]
		+ (u*u*u - 2*u*u + u)*(0.5*(1-flower->tension)*((1+flower->bias)*(1-flower->continuity)*(control[1][1]-control[0][1])
					+ (1-flower->bias)*(1+flower->continuity)*(control[2][1]-control[1][1])))
		+ (u*u*u - u*u)*(0.5*(1-flower->tension)*((1+flower->bias)*(1+flower->continuity)*(control[2][1]-control[1][1])
					+ (1-flower->bias)*(1-flower->continuity)*(control[3][1]-control[2][1])));

//	printf ("FUCKERS %f %f %f %f %f, %f %f %f\n", *result[0], *result[1], control[1][0], control[1][1], u,
//			flower->tension, flower->bias, flower->continuity);
}

void lights(FlowerInternal *flower) {
	GLfloat light_position[]={10.0,9.0,-12.0,0.0};
	GLfloat light_diffuse[]={0.8,0.7,0.7,0.0};
	GLfloat light_specular[]={0.85,0.85,0.8,0.0};
	GLfloat light_ambient[]={0.0,0.0,0.0,0.0};

	GLfloat mat_diffuse[]={1.0,1.0,1.0,0.0};
	GLfloat mat_specular[]={1.0,1.0,1.0,0.0};
	GLfloat mat_ambient[]={0.48,0.45,0.4,0.0};
	GLfloat mat_emission[] = { 0.00, 0.00, 0.00, 1.0 };
	GLfloat mat_shininess[] = { 18.0 };

	glMaterialfv(GL_FRONT, GL_EMISSION,mat_emission);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glMaterialfv(GL_BACK, GL_EMISSION,mat_emission);
	glMaterialfv(GL_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_BACK, GL_SHININESS, mat_shininess);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glEnable(GL_LIGHT0);

}

void spline3DMorph(FlowerInternal *flower, float factor, float poikkeama) {
#define NBTW 12
	int i,j,k;
	int size=sizeof(flower->kukka)/sizeof(Vect);

	float vnyt,vnex;
	float l;
	float ti=visual_timer_elapsed_msecs(&flower->timer);
	Vect r,r_morph,n;
	float rf[17*NBTW*3];
	int c;
	c=0;
	for (i=0; i<size-3; i++) {
		for (j=0; j<NBTW; j++) {
			splineTCP(flower, (float)j/NBTW, &flower->kukka[i], &r);
			splineTCP(flower, (float)j/NBTW, &flower->kukka_morph[i], &r_morph);
			for (k=0; k<3; k++)
				rf[c*3+k]=(1.0-factor)*r[k]+factor*r_morph[k];

			rf[c*3+2]=sin(c*M_PI/(NBTW*4))*0.07;
			c+=1;

		}
	}
	for (i=0; i<(c-1); i++) {
		vnyt=(float)i/(float)(c-1)*(float)(size-3);
		vnex=(float)(i+1.0)/(float)(c-1)*(float)(size-3);

		rf[i*3]+=+sin((vnyt*2+ti*0.006))*0.02*poikkeama;
		rf[i*3+1]+=+sin((vnyt*2+ti*0.006))*0.02*poikkeama;
		rf[i*3+3]+=sin((vnex*2.1+ti*0.006))*0.02*poikkeama;
		rf[i*3+3+1]+=+sin((vnex*2.0+ti*0.006))*0.02*poikkeama;

		n[1]=-(rf[(i+1)*3]-rf[i*3]);
		n[0]=(rf[(i+1)*3+2]-rf[i*3+2]);
		n[2]=rf[(i+1)*3+1]-rf[i*3+1];
		l=sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
		n[0]/=l;
		n[1]/=l;
		n[2]/=l;

		glEnable(GL_LIGHTING);
		glColor3f(1.0,1.0,1.0);
		glPolygonOffset(3.0,2.0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glBegin(GL_POLYGON);
		glNormal3fv((GLfloat *)n);
		glVertex3f(1.0*rf[i*3],-rf[i*3+2],rf[i*3+1]);
		glNormal3fv((GLfloat *)n);
		glVertex3f(1.0*rf[i*3+3],-rf[i*3+3+2],rf[i*3+3+1]);
		glNormal3fv((GLfloat *)n);
		glVertex3f(1.0*rf[i*3+3],+rf[i*3+3+2],rf[i*3+3+1]);
		glNormal3fv((GLfloat *)n);
		glVertex3f(1.0*rf[i*3],+rf[i*3+2],rf[i*3+1]);
		glEnd();

		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
		glDisable(GL_BLEND);
		glPolygonOffset(-1.0,-5.0);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glLineWidth(2.0);
		glDisable(GL_LIGHTING);
		glColor3f(0.0,0.0,0.0);
		glBegin(GL_LINE_LOOP);
		glVertex3f(1.0*rf[i*3],-rf[i*3+2],rf[i*3+1]*1);
		glVertex3f(1.0*rf[i*3+3],-rf[i*3+3+2],rf[i*3+3+1]*1);
		glVertex3f(1.0*rf[i*3+3],+rf[i*3+3+2],rf[i*3+3+1]*1);
		glVertex3f(1.0*rf[i*3],+rf[i*3+2],rf[i*3+1]*1);
		glEnd();
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
	}
}


#define F 0.006
#define FR 0.006

void render_flower(FlowerInternal *flower) {
	int t;
	int time=visual_timer_elapsed_msecs(&flower->timer);

	flower->tension=F*flower->tension_new+(1.0-F)*flower->tension;
	flower->continuity=F*flower->continuity_new+(1.0-F)*flower->continuity;
	flower->bias=F*flower->bias_new+(1.0-F)*flower->bias;

	for (t=0; t<12; t++) {
		glRotatef(360.0/12.0,0.0,0.0,1.0);
		spline3DMorph(flower, sin(time*0.001*flower->spd)*0.5+0.5,
				6.0*flower->audio_bars[(t*8) % 32]*flower->audio_strength

			     );
	}


}

float col1[]={1,1,1};

void render_flower_effect(FlowerInternal *flower) {

	glClearColor(col1[0],col1[1],col1[2],0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(60.0, (float)flower->width/(float)flower->height, 0.01, 1135.0);


	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glTranslatef(0.0,0.0,-1.0);

	lights(flower);
	glPopMatrix();
	flower->posz=FR*flower->posz_new+(1.0-FR)*flower->posz;
	glTranslatef(0.0,0.0,-flower->posz);

	glRotatef(flower->rotx,1.0,0.0,0.0);
	glRotatef(flower->roty,0.0,1.0,0.0);
	glRotatef(visual_timer_elapsed_msecs(&flower->timer)*0.02,0.0,0.0,1.0);
	render_flower(flower);
}
