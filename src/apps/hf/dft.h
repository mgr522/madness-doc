#ifndef DFT_H_
#define DFT_H_

#include <mra/mra.h>
#include <world/world.h>
#include <vector>

#include "eigsolver.h"

namespace madness
{
    
  //***************************************************************************
  // TYPEDEFS
  typedef SharedPtr< FunctionFunctorInterface<double,3> > functorT;
  typedef Function<double,3> funcT;
  typedef Vector<double,3> coordT;
  //***************************************************************************

  //***************************************************************************
  class DFTNuclearPotentialOp : public EigSolverOp
  {
  public:
    //*************************************************************************
    // Constructor
    DFTNuclearPotentialOp(World& world, funcT V, double coeff, double thresh);
    //*************************************************************************

    //*************************************************************************
    // Is there an orbitally-dependent term?
    virtual bool is_od() {return false;}
    //*************************************************************************

    //*************************************************************************
    // Is there a density-dependent term?
    virtual bool is_rd() {return true;}
    //*************************************************************************

    //*************************************************************************
    virtual funcT op_o(const std::vector<funcT>& phis, const funcT& psi) {}
    //*************************************************************************

    //*************************************************************************
    virtual funcT op_r(const funcT& rho, const funcT& psi);
    //*************************************************************************

  private:
    //*************************************************************************
    funcT _V;
    //*************************************************************************

    //*************************************************************************
    functorT ones;
    functorT zeros;
    //*************************************************************************
  };
  //***************************************************************************

  //***************************************************************************
  class DFT : public IEigSolverObserver
  {
  public:
    //*************************************************************************
    // Constructor
    DFT(World& world, funcT V, std::vector<funcT> phis, 
      std::vector<double> eigs, double thresh);
    //*************************************************************************
  
    //*************************************************************************
    // Constructor for ground state only
    DFT(World& world, funcT V, funcT phi, double eig, double thresh);
    //*************************************************************************
  
  	//*************************************************************************
    DFT();
    //*************************************************************************

    //*************************************************************************
    virtual ~DFT();
    //*************************************************************************

    //*************************************************************************
     void solver(int maxits);
     //*************************************************************************
   
     //*************************************************************************
     virtual void iterateOutput(const std::vector<funcT>& phis,
         const std::vector<double>& eigs, const int& iter);
     //*************************************************************************

     //*************************************************************************
     double get_eig(int indx)
     {
       return _solver->get_eig(indx);
     }
     //*************************************************************************

     //*************************************************************************
     funcT get_phi(int indx)
     {
       return _solver->get_phi(indx);
     }
     //*************************************************************************
     
     //*************************************************************************
     double calculate_tot_exchange_energy(const std::vector<funcT>& phis);
     //*************************************************************************

  private:

      //*************************************************************************
      // Eigenvalue solver
      EigSolver* _solver;
      //*************************************************************************
      
      //*************************************************************************
      World& _world;
      //*************************************************************************

      //*************************************************************************
      funcT _V;
      //*************************************************************************

      //*************************************************************************
      double _thresh;
      //*************************************************************************
      
      //*************************************************************************
      functorT ones;
      functorT zeros;
      //*************************************************************************

      //*************************************************************************
      World& world() {return _world;}
      //*************************************************************************

      //*************************************************************************
      double thresh() {return _thresh;}
      //*************************************************************************

  };
  //***************************************************************************

}
#endif /*DFT_H_*/