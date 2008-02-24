
/// \file hatom_energy.cc
/// \brief Compute the energy of the hydrogen atom ground state

#include <mra/mra.h>

using namespace madness;


double psi(const Vector<double,3>& r) {
  return exp(-sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]+1e-6));
}

double V(const Vector<double,3>& r) {
  return -1.0/sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]+1e-8);
}

int main(int argc, char**argv) {
  // Initialize the parallel programming environment
  MPI::Init(argc, argv);
  World world(MPI::COMM_WORLD);
  
  // Load info for MADNESS numerical routines
  startup(world,argc,argv);
  
  // Setup defaults for numerical functions
  FunctionDefaults<3>::k = 7;                 // Wavelet order
  FunctionDefaults<3>::thresh = 1e-5;         // Accuracy
  FunctionDefaults<3>::refine = true;         // Enable adaptive refinement
  FunctionDefaults<3>::initial_level = 2;     // Initial projection level
  for (int i=0; i<3; i++) {
    FunctionDefaults<3>::cell(i,0) = -20.0;   // User-defined volume
    FunctionDefaults<3>::cell(i,1) =  20.0;
  }
    
  Function<double,3> u = FunctionFactory<double,3>(world).f(psi);
  Function<double,3> v = FunctionFactory<double,3>(world).f(V);
  Function<double,3> vu = v*u;
  Function<double,3> du = diff(u,0);
  double KE = 3*0.5*(du.inner(du));
  double PE = vu.inner(u);
  double S = u.inner(u);

  print("the overlap integral is",S);
  print("the kinetic energy integral",KE);
  print("the potential energy integral",PE);
  print("the total energy",(KE+PE)/S);
  
  MPI::Finalize();
  
  return 0;
}