#ifndef _JESS_ANALYSER_STRUCT_H
#define _JESS_ANALYSER_STRUCT_H

#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5

#define OUI 1
#define NON 0

#define BPM_MAX 400
#define LEVEL_ONE 3 
#define TOTAL_E 0
#define BASS_E 1
#define MEDIUM_E 2
#define HIGH_E 3

#define T_AVERAGE_SPECTRAL 100
#define T_AVERAGE_DEDT 100
#define T_DEDT 10
#define T_AVERAGE_E 130

struct analyser_struct
{
	float E;
	float E_moyen;
	float dEdt;
	float dEdt_moyen;

	float Ed_moyen[256];
	float dt;

	char dbeat[256]; 

	int reprise;
	int montee;
	int beat;
	int last_time[10];
	int conteur[100]; /* ZERO : sert pour l'affichage de appel JESS */
};

#endif /* _JESS_ANALYSER_STRUCT_H */
