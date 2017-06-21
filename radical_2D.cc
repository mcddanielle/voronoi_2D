// Radical Voronoi tessellation example code
//
// Original Author   : Chris H. Rycroft (LBL / UC Berkeley)
// Email    : chr@alum.mit.edu
// Date     : August 30th 2011

// Modifications to 2D, PBC: Danielle McDermott
// Email    : mcd.danielle@gmail.com
// Date     : June 19th, 2016

#include <vector>
using namespace std;

#include "voro++.hh"
using namespace voro;

#include "mpp.h"


/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
void analyze_voro(struct vortex *vortex,int *nV,int time,
		  struct syssize syssize, FILE *fp) {

  // Set up constants for the container geometry
  const double x_min= 0.0, x_max=syssize.SX;
  const double y_min= 0.0, y_max=syssize.SY;
  const double z_min=-0.5, z_max=0.5;

  // Set up the number of blocks that the container is divided
  // into.
  // NOT optimized!
  const int n_x=20,n_y=20,n_z=1;

  double x,y,z,radius;
  voronoicell_neighbor c;
  vector<int> neigh;
  int i;
	
  // Create a container for polydisperse particles using the same
  // geometry as above. Import the polydisperse test packing and
  // output the Voronoi radical tessellation in gnuplot and POV-Ray
  // formats.
  container_poly conp(x_min,x_max,            //container size
		      y_min,y_max,  
		      z_min,z_max,  
		      n_x,n_y,n_z,            //number threads
		      true,true,false,        //pbc in x/y
		      16);                    //number particles/block
  
  /*get data alternative:
   * import() if you have a plain text file containing
   *id x y z radius
   */
  //conp.import("poly_output_voro");

  //for smtest loop through vortex struct adding particles
  for(i=0;i<(*nV);i++){

    x=vortex[i].x;
    y=vortex[i].y;
    z=0.0;

    if(vortex[i].color == 1 ){
      radius = 0.7;
    }
    else{
      radius = 0.5;
    }

    //add the individual particle to the container
    conp.put(i,x,y,z,radius);

  }


  /*Output the tessellation data for plotting*/
  
  //  conp.draw_cells_gnuplot("poly.gnu");   //gnuplot
  //  conp.draw_cells_pov("poly_v.pov");     //pov ray
  //  conp.draw_particles_pov("poly_p.pov");
  
  //array for storing voronoi stats
  double pN[4];
  
  //initialize values
  pN[0]=0.0;
  pN[1]=0.0;
  pN[2]=0.0;
  pN[3]=0.0;

  int n_neigh;
  int normalize = 0; // = conp.number_of_partcilcsl;
	  
  //from http://math.lbl.gov/voro++/examples/polygons/
  // Loop over all particles in the container
  // and compute each Voronoi cell

	
  c_loop_all cl(conp);
  if(cl.start()) do if(conp.compute_cell(c,cl)) {
	cl.pos(x,y,z);
	//id=cl.pid();

	normalize += 1;
	      
	// Gather information about the computed Voronoi cell
	c.neighbors(neigh);
	n_neigh = neigh.size()-2;  //subtract off the z-faces
	      
	if(n_neigh >3 && n_neigh<8){
	  pN[n_neigh-4] += 1.0;
	  //printf("not done! norm = %d ", normalize);
	  
	}

      } while (cl.inc());


  //divide by the total number of particles
  for(i=0;i<4;i++){
    pN[i]  /= (float)normalize;
  }

  //print the normalized values to the output file
  fprintf(fp,"%f  %f  %f  %f \n", pN[0], pN[1], pN[2], pN[3]);
  fflush(fp);


  return;
}
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
/*--------------------------------------------------------------------*/
