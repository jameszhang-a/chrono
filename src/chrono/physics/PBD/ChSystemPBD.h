// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Simone Benatti
// =============================================================================
//
// Physical system in which the dyinamics step is performed using the Position Based method
// Matthias M�ller: Detailed Rigid Body Simulation using Extended Position Based Dynamics
//
// =============================================================================

#ifndef CH_SYSTEM_PBD_H
#define CH_SYSTEM_PBD_H

#include "chrono/physics/ChSystem.h"
#include "chrono/physics/PBD/ChPBDutils.h"

namespace chrono {

/// Class for a physical system in which contact is modeled using a non-smooth
/// (complementarity-based) method.
class ChApi ChSystemPBD : public ChSystem {

  public:


    /// Create a physical system.
    /// If init_sys is false, the collision system oand solver are not initialized.
	ChSystemPBD(bool init_sys = true);

    /// Copy constructor
	ChSystemPBD(const ChSystemPBD& other);

    /// Destructor
    virtual ~ChSystemPBD() {}

    /// "Virtual" copy constructor (covariant return type).
    virtual ChSystemPBD* Clone() const override { return new ChSystemPBD(*this); }

    /// Return the contact method supported by this system.
    virtual ChContactMethod GetContactMethod() const override final { return ChContactMethod::NSC; }

    /// Replace the contact container.
    virtual void SetContactContainer(std::shared_ptr<ChContactContainer> container) override;

	/// Performs a single dynamical simulation step, according to
	/// current values of:  Y, time, step  (and other minor settings)
	/// Depending on the integration type, it switches to one of the following:
	bool Integrate_Y() override;

    // SERIALIZATION

    /// Method to allow serialization of transient data to archives.
    virtual void ArchiveOUT(ChArchiveOut& marchive) override;

    /// Method to allow deserialization of transient data from archives.
    virtual void ArchiveIN(ChArchiveIn& marchive) override;

  private:
	/// convert the Chrono system to fit into PBD formulation
	void PBDSetup();

  protected:
    /// Setup the PBD system
	bool PBD_isSetup = false;
	/// Lists of links and contacts usable by PBD formulation
	std::vector<std::shared_ptr< ChLinkPBD> > linklistPBD;
	//std::vector<ChContactPBD> contactlistPBD;
};

CH_CLASS_VERSION(ChSystemPBD, 0)

}  // end namespace chrono

#endif
