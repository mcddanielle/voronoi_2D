//=================================
// include guard
#ifndef __MYCLASS_H_INCLUDED__
#define __MYCLASS_H_INCLUDED__
//=================================

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>

#define PI 3.14159265359
#define TWOPI 6.28318530718

#define SMOVIE 0
#define CMOVIE 1
#define JMOVIE 2

struct vortex{
  int id;        //count from 0 to N-1 particles in system
  int color;     //used to color shepherds/sheep fast/slow
  float x;       //x position of particle - converted to floats upon write time
  float y;       //y position of particle 
  float x_old;   //x position of particle in last timestep
  float y_old;   //y position of particle in last timestep; 
  float radius;  //
  float vx;
  float vy;
  float speed;
  float accum_dx_old;//keep a much older value of dx to measure ratcheting
  float accum_dx;    //measure the dx value over many timesteps

  int runtime;   // time between tumbles
};

struct syssize{
  double SX;    //DM Width of system
  double SY;    //DM Height of system
  double SX2;   //DM SX*SX  these get used often enough not to recalculate 
  double SY2;   //DM Sy*SY  every time
};


//DM check whether particles overlap, and are within the box
extern "C" void checkpos(struct vortex *vortex,int id,struct syssize syssize);

//DM calculate interparticle distances, including periodic boundary conditions
extern "C" void distance(float *dr,float *dx,float *dy,float x1,float y1,
	      float x2,float y2,struct syssize syssize);


//DM read in Pa0 
extern "C" void get_parameters_file(int *maxnum,struct syssize *syssize,double *radius,
			 int *runtime,double *runforce,double *density,
			 double *dt,int *maxtime,int *writemovietime,double *period);

//DM start the simulation
extern "C" void initialize(int *nV,struct vortex **vortex,struct syssize *syssize,
		double *dt,int *time,int *maxtime,int *writemovietime,
		double *period);

//send position data to be written as a frame
extern "C" void write_ascii_frame(struct vortex *vortex,
		       int nV,int time,
		       struct syssize syssize);

//read and parse movie
extern "C" void parse_movie(struct vortex *vortex,int *nV,int *time,double dt,
		 struct syssize syssize,int writemovietime,
		 int movie_type,double period);

//read movie frame
extern "C" void read_frame(struct vortex *vortex,int *nV,int time,double dt,
		 struct syssize syssize,int writemovietime,
		int movie_type,double period);

//determine what parameter file we have
extern "C" int doesFileExist(const char *filename);

//analyze the data using voro++
void analyze_voro(struct vortex *vortex,int *nV,int time,
		  struct syssize syssize, FILE *fp);


#endif // __MYCLASS_H_INCLUDED__ 

