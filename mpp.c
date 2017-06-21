/* Read in Pa0
 * Read in smtest
 * many analysis techniques possible
 */

#include "mpp.h"

FILE *fp;
FILE *cmovie;          //DM color movie file pointer

int main(int argc, char **argv)
{
  struct vortex *vortex;      //DM declare a linked list pointer 
                              //which will contain particles
  int nV;                     //DM Declare number of variables integer
  struct syssize syssize;     //DM declare the system size struct
  int time,maxtime;           //DM time = running clock time of system
  double dt;                  //DM timestep to integrate over
  int writemovietime;         //DM integer that sends counts when to write frame
  double period;

  int movie_type = CMOVIE;    //DM hardwire for now
  //movie_type = SMOVIE;
  
  printf("The movie type is %d \n",movie_type);

  //DM call the subroutine to read the parameters in from Pa0
  initialize(&nV,&vortex,&syssize,&dt,&time,&maxtime,&writemovietime,&period);

  //---------------------------------------------------
  //analyze the data for number of voronoi faces
  //---------------------------------------------------
  //open a file
  if((fp = fopen("voro_stats.txt","wa"))==NULL)
    {printf("Cannot open file.\n");
      exit(1);
    }
  
  //loop through the smtest file frame-by-frame
  do{
    parse_movie(vortex,&nV,&time,dt,syssize,writemovietime,movie_type,period);
    
  }while(time<maxtime);


  fclose(cmovie);
  fclose(fp);

} 
//---------------------------------------------------------------------------
//DM end of main subroutine
//---------------------------------------------------------------------------

void parse_movie(struct vortex *vortex,int *nV,int *time,double dt,
		 struct syssize syssize,int writemovietime,
		 int movie_type,double period)
{

  read_frame(vortex,nV,*time,dt,syssize,writemovietime,movie_type,period);

  //write_ascii_frame(vortex,nV,*time,syssize);
  analyze_voro(vortex,nV,*time,syssize,fp);
  
  //DM increment the time pointer
  (*time)+=writemovietime;
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------
void read_frame(struct vortex *vortex, int *nV, int time, double dt,
		struct syssize syssize,int writemovietime,int movie_type,
		double period)
{

  int check_nV;     //see if nV has changed
  int check_time;   //check it is predicted time

  int i;

  //float delta_t = writemovietime*dt;

  fread(&check_nV,  sizeof(int),1,cmovie); //get number particles and check
  fread(&check_time,sizeof(int),1,cmovie); //get time and check

  //update number of vortices
  (*nV) = check_nV;

 
  for(i=0;i<(*nV);i++){

    	//Save old particle positions if (time!=0)
        //before reading in the new frame
	if(check_time){
	  vortex[i].x_old = vortex[i].x;
	  vortex[i].y_old = vortex[i].y;

	}



	//read in the 'color' only if cmovie
	if(movie_type == CMOVIE){
	  fread(&(vortex[i].color), sizeof(int),  1,cmovie);
	}
	else{
	  //just hardwire to an integer value
	  vortex[i].color=1; 
	}

	fread(&(vortex[i].id),    sizeof(int),  1,cmovie); //all movies
	fread(&(vortex[i].x),     sizeof(float),1,cmovie); //all movies
	fread(&(vortex[i].y),     sizeof(float),1,cmovie); //all movies
	  
	//read in the 'radius' only if cmovie
	if(movie_type == CMOVIE){
	  fread(&(vortex[i].radius),sizeof(float),1,cmovie);
	}
	else{
	  //just hardwire to a float value
	  vortex[i].radius=1.0; 
	}

	//printf("%d %f %f %f\n", i, vortex[i].x,vortex[i].y,vortex[i].radius);
	
	/*
	if(check_time){

	  do_voronoi_analysis();
	  
	}
	*/


  }



  return;
}

//-----------------------------------------------------------------
void write_ascii_frame(struct vortex *vortex,int nV,int time,
		       struct syssize syssize)
{

  FILE *amovie;
  int i;


  //make a directory to place the files:
  mkdir("velocity_data",0700);

  //create file names using string methods of c
  char str_velocity[100]="velocity_data/XV_data_t=";
  char str_time[10];
  
  sprintf(str_time,"%08d",time); //convert current to a string   
  strcat(str_velocity,str_time);


   //open the velocity ascii file
   if( (amovie = fopen(str_velocity, "w")) == NULL){
     printf("Can't open %s.\n",str_velocity);
     exit(-1);
   }



   fprintf(amovie,"#Number of Vortices: %d\n",nV);
   fprintf(amovie,"#time frame: %d\n",time);
   fprintf(amovie,"#X Domain: %f\n",syssize.SX); 
   fprintf(amovie,"#Y Domain: %f\n",syssize.SY); 



  for(i=0;i<nV;i++){


    fprintf(amovie,"%5d ",i);
    fprintf(amovie,"%2d ",vortex[i].color);
    fprintf(amovie,"%-5.6f %-5.6f ",vortex[i].x,vortex[i].y);
    fprintf(amovie,"%-5.6f %-5.6f %-5.6f \n", vortex[i].vx,	\
	    vortex[i].vy,vortex[i].speed ); 



  }

  fflush(amovie);
  fclose(amovie);
  return;
}

//-----------------------------------------------------------------
void initialize(int *nV,struct vortex **vortex,struct syssize *syssize,
		double *dt,int *time,int *maxtime,int *writemovietime,
		double *period)
{
  int maxnum;
  double radius;
  int runtime;
  double runforce;
  double density;
  //int i,j;
  //int maxcell;
  
  //if Pa0 do this
  //read in Pa0 file
  get_parameters_file(&maxnum,syssize,&radius,&runtime,&runforce,&density,
		      dt,maxtime,writemovietime,period);

  *nV = 0;
  *time=0;  //first frame is written at this time

  //formally this was a movie for writing,
  //in this analysis code it is a movie for reading

  //hardcode 
  int get_data_smtest = 1;
  int get_data_smAtest0 = 0;

  if(get_data_smtest){
    cmovie=fopen("smtest","rb");
    printf("\nReading from binary file smtest\n");
  }
  else if(get_data_smAtest0){
    cmovie=fopen("smAtest0","rb");
    printf("\nReading from binary file smAtest0\n");
  }

  fflush(stdout);
  
  *vortex=(struct vortex*)malloc(maxnum*sizeof(struct vortex));

  //printf("malloc successful! maxnum=%d",maxnum);
  //fflush(stdout);

  return;

}

//-----------------------------------------------------------------
void get_parameters_file(int *maxnum,struct syssize *syssize,double *radius,
			 int *runtime,double *runforce,double *density,
			 double *dt,int *maxtime,int *writemovietime,
			 double *period)
{
  FILE *in;
  char trash[120];
  double cellsize;   //not used



  double kspring; //not used



  //following are 1 if file exists, and 0 otherwise
  int read_Pa0 = doesFileExist("input_files/Pa0");
  
  //int read_Pa0r = doesFileExist("Pa0r");
  //int read_generic = doesFileExist("generic_Px0");

  //------------------------------------------------try opening Pa0
  if(read_Pa0){
    if((in=fopen("input_files/Pa0","r"))==NULL){
      printf("Input file input_files/Pa0 not found\n");
      exit(-1);  // exit if Pa0 not opened
    }
    printf("\n reading in input_files/Pa0\n");
  }
  else{
    printf("What parameter files (Pa0) are you using?\n");
    exit(-1);  //no file found, game over
  }

  //for Pa0
  //------------------------------------------------try opening Pa0
  if(read_Pa0){

    if ( (fscanf(in,"%s %lf\n",trash,density)) != 2){
      printf("Density is: %lf\n",density);
      exit(-1);
    }
    printf("density is: %lf\n",density);

    if ( (fscanf(in,"%s %lf\n",trash,&((*syssize).SX))) != 2){
      printf("Couldn't load SX!\n");
      exit(-1);
    }
    printf("SX is: %lf\n",(*syssize).SX);

    (*syssize).SX2=(*syssize).SX*0.5;
    //--------------------------------------------
    if ( (fscanf(in,"%s %lf\n",trash,&((*syssize).SY))) != 2){
      printf("Couldn't load SY!\n");
      exit(-1);
    }
    printf("SY is: %lf\n",(*syssize).SY);
    //-----------------------------------------------
    (*syssize).SY2=(*syssize).SY*0.5;

    if ( (fscanf(in,"%s %lf\n",trash,radius)) != 2){
      printf("Couldn't load radius!\n");
      exit(-1);
    }
    printf("radius is: %lf\n",*radius);

    *maxnum=*density*(*syssize).SX*(*syssize).SY/(0.5*TWOPI*(*radius)*(*radius));
    //intialized on grid, so we don't get maxnum:
    int n_rows = (int) sqrt(*maxnum);
    int m_cols = ceil(*maxnum/n_rows);

    *maxnum=n_rows*m_cols;

    printf("maxnum is: %d\n",*maxnum);

    if ( (fscanf(in,"%s %d\n",trash,runtime)) != 2){
      printf("Couldn't load runtime!\n");
      exit(-1);
    }

    printf("runtime is: %d\n",*runtime);

    if ( (fscanf(in,"%s %lf\n",trash,runforce)) != 2){
      printf("Couldn't load runforce!\n");
      exit(-1);
    }
    printf("runforce is: %lf\n",*runforce);

    if ( (fscanf(in,"%s %lf\n",trash,dt)) != 2){
      printf("Couldn't load dt!\n");
      exit(-1);
    }
    printf("dt is: %lf\n",*dt);

    if ( (fscanf(in,"%s %d\n",trash,maxtime)) != 2){
      printf("Couldn't load maxtime!\n");
      exit(-1);
    }

    printf("maxtime is: %d\n",*maxtime);

    if ( (fscanf(in,"%s %d\n",trash,writemovietime)) != 2){
      printf("Couldn't load number of vortices or time 9!\n");
      exit(-1);
    }

    printf("writemovietime is: %d\n",*writemovietime);

    if ( (fscanf(in,"%s %lf\n",trash,&kspring)) != 2){
      printf("Couldn't load number of vortices or time 10!\n");
      exit(-1);
    }
    if ( (fscanf(in,"%s %lf\n",trash,&cellsize)) != 2){
      printf("Couldn't load number of vortices or time 11!\n");
      exit(-1);
    }
    // size of lookup cell
  }

  fclose(in);

  return;
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------

int doesFileExist(const char *filename) {
    struct stat st;
    int result = stat(filename, &st);
    return result == 0;  //if file does exist, this returns true
}
