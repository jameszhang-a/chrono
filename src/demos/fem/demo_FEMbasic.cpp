///////////////////////////////////////////////////
//
//   Demos code about 
//
//     - FEM (basic introduction)
//
//	 CHRONO 
//   ------
//   Multibody dinamics engine
// 
// ------------------------------------------------
// 	 Copyright:Alessandro Tasora / DeltaKnowledge
//             www.deltaknowledge.com
// ------------------------------------------------
///////////////////////////////////////////////////
  
     
// Include some headers used by this tutorial...

#include "physics/ChApidll.h" 
#include "physics/ChSystem.h"
#include "fem/ChFem.h"
#include "fem/ChMesh.h"


// Remember to use the namespace 'chrono' because all classes 
// of Chrono::Engine belong to this namespace and its children...

using namespace chrono;
using namespace fem;




// Test 1
// First example: 

void test_1()
{
	GetLog() << "\n-------------------------------------------------\n";
	GetLog() << "TEST: spring element FEM  \n\n";

				// The physical system: it contains all physical objects.
	ChSystem my_system; 

				
				// Create some nodes. These are the classical point-like
				// nodes with x,y,z degrees of freedom, that can be used 
				// for many types of FEM elements in space.
	ChNodeFEMxyz mnodeA(ChVector<>(0,0,0));
	ChNodeFEMxyz mnodeB(ChVector<>(1,1,0));
				// For example, set an applied force to a node:


				// Create some elements of 'spring-damper' type, each connecting
				// two 3D nodes:
	ChElementSpring melementA;
	melementA.SetNodes(&mnodeA, &mnodeB);
	melementA.SetSpringK(100);
	
				// Create a mesh, that is a container for groups
				// of elements and their referenced nodes.
	ChSharedPtr<ChMesh> my_mesh(new ChMesh);

				// Remember to add nodes and elements to the mesh!
	my_mesh->AddNode(mnodeA);
	my_mesh->AddNode(mnodeB);

				// Remember to add the mesh to the system!
	my_system.Add(my_mesh);


	//***TO DO***

}



// Do some tests in a single run, inside the main() function.
// Results will be simply text-formatted outputs in the console..

int main(int argc, char* argv[])
{
	// In CHRONO engine, The DLL_CreateGlobals() - DLL_DeleteGlobals(); pair is needed if
	// global functions are needed. 
	DLL_CreateGlobals();



	GetLog() << " Example: the FEM techology for finite elements \n\n\n";

	// Test: an introductory problem:
	test_1();


	// Remember this at the end of the program, if you started
	// with DLL_CreateGlobals();
	DLL_DeleteGlobals();

	return 0;
}


