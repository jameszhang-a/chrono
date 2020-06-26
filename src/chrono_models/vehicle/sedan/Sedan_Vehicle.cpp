// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All right reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Radu Serban, Asher Elmquist
// =============================================================================
//
// Sedan full vehicle model.
//
// =============================================================================

#include "chrono/assets/ChSphereShape.h"
#include "chrono/assets/ChTriangleMeshShape.h"
#include "chrono/utils/ChUtilsInputOutput.h"

#include "chrono_vehicle/ChVehicleModelData.h"

#include "chrono_models/vehicle/sedan/Sedan_Vehicle.h"

namespace chrono {
namespace vehicle {
namespace sedan {

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Sedan_Vehicle::Sedan_Vehicle(const bool fixed,
                             ChContactMethod contact_method,
                             ChassisCollisionType chassis_collision_type)
    : ChWheeledVehicle("Sedan", contact_method), m_omega({0, 0, 0, 0}) {
    Create(fixed, chassis_collision_type);
}

Sedan_Vehicle::Sedan_Vehicle(ChSystem* system, const bool fixed, ChassisCollisionType chassis_collision_type)
    : ChWheeledVehicle("Sedan", system), m_omega({0, 0, 0, 0}) {
    Create(fixed, chassis_collision_type);
}

void Sedan_Vehicle::Create(bool fixed, ChassisCollisionType chassis_collision_type) {
    // Create the chassis subsystem
    m_chassis = chrono_types::make_shared<Sedan_Chassis>("Chassis", fixed, chassis_collision_type);

    // Create the axle subsystems
    m_axles.resize(2);
    m_axles[0] = chrono_types::make_shared<ChAxle>();
    m_axles[1] = chrono_types::make_shared<ChAxle>();

    m_axles[0]->m_suspension = chrono_types::make_shared<Sedan_DoubleWishbone>("FrontSusp");
    m_axles[1]->m_suspension = chrono_types::make_shared<Sedan_MultiLink>("RearSusp");

    m_axles[0]->m_wheels.resize(2);
    m_axles[0]->m_wheels[0] = chrono_types::make_shared<Sedan_Wheel>("Wheel_FL");
    m_axles[0]->m_wheels[1] = chrono_types::make_shared<Sedan_Wheel>("Wheel_FR");
    m_axles[1]->m_wheels.resize(2);
    m_axles[1]->m_wheels[0] = chrono_types::make_shared<Sedan_Wheel>("Wheel_RL");
    m_axles[1]->m_wheels[1] = chrono_types::make_shared<Sedan_Wheel>("Wheel_RR");

    m_axles[0]->m_brake_left = chrono_types::make_shared<Sedan_BrakeSimple>("Brake_FL");
    m_axles[0]->m_brake_right = chrono_types::make_shared<Sedan_BrakeSimple>("Brake_FR");
    m_axles[1]->m_brake_left = chrono_types::make_shared<Sedan_BrakeSimple>("Brake_RL");
    m_axles[1]->m_brake_right = chrono_types::make_shared<Sedan_BrakeSimple>("Brake_RR");

    // Create the steering subsystem
    m_steerings.resize(1);
    m_steerings[0] = chrono_types::make_shared<Sedan_RackPinion>("Steering");

    // Create the driveline
    m_driveline = chrono_types::make_shared<Sedan_Driveline2WD>("Driveline");
}

Sedan_Vehicle::~Sedan_Vehicle() {}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void Sedan_Vehicle::Initialize(const ChCoordsys<>& chassisPos, double chassisFwdVel) {
    // Initialize the chassis subsystem.
    m_chassis->Initialize(m_system, chassisPos, chassisFwdVel, WheeledCollisionFamily::CHASSIS);

    // Initialize the steering subsystem (specify the steering subsystem's frame relative to the chassis reference
    // frame).
    ChVector<> offset = ChVector<>(1.2, 0, 0.14);
    ChQuaternion<> rotation = ChQuaternion<>(1, 0, 0, 0);
    m_steerings[0]->Initialize(m_chassis->GetBody(), offset, rotation);

    // Initialize the axle subsystems.
    m_axles[0]->Initialize(m_chassis, nullptr, m_steerings[0], ChVector<>(1.388, 0, .25), ChVector<>(0), 0.0,
                           m_omega[0], m_omega[1]);
    m_axles[1]->Initialize(m_chassis, nullptr, nullptr, ChVector<>(-1.388, 0, .125), ChVector<>(0), 0.0, m_omega[2],
                           m_omega[3]);

    // Initialize the driveline subsystem (FWD)
    std::vector<int> driven_susp_indexes = {0};
    m_driveline->Initialize(m_chassis->GetBody(), m_axles, driven_susp_indexes);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
double Sedan_Vehicle::GetSpringForce(int axle, VehicleSide side) const {
    return std::static_pointer_cast<ChDoubleWishbone>(m_axles[axle]->m_suspension)->GetSpringForce(side);
}

double Sedan_Vehicle::GetSpringLength(int axle, VehicleSide side) const {
    return std::static_pointer_cast<ChDoubleWishbone>(m_axles[axle]->m_suspension)->GetSpringLength(side);
}

double Sedan_Vehicle::GetSpringDeformation(int axle, VehicleSide side) const {
    return std::static_pointer_cast<ChDoubleWishbone>(m_axles[axle]->m_suspension)->GetSpringDeformation(side);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
double Sedan_Vehicle::GetShockForce(int axle, VehicleSide side) const {
    return std::static_pointer_cast<ChDoubleWishbone>(m_axles[axle]->m_suspension)->GetShockForce(side);
}

double Sedan_Vehicle::GetShockLength(int axle, VehicleSide side) const {
    return std::static_pointer_cast<ChDoubleWishbone>(m_axles[axle]->m_suspension)->GetShockLength(side);
}

double Sedan_Vehicle::GetShockVelocity(int axle, VehicleSide side) const {
    return std::static_pointer_cast<ChDoubleWishbone>(m_axles[axle]->m_suspension)->GetShockVelocity(side);
}

// -----------------------------------------------------------------------------
// Log the hardpoint locations for the front-right and rear-right suspension
// subsystems (display in inches)
// -----------------------------------------------------------------------------
void Sedan_Vehicle::LogHardpointLocations() {
    GetLog().SetNumFormat("%7.3f");

    GetLog() << "\n---- FRONT suspension hardpoint locations (LEFT side)\n";
    std::static_pointer_cast<ChDoubleWishbone>(m_axles[0]->m_suspension)
        ->LogHardpointLocations(ChVector<>(0, 0, 0), false);

    GetLog() << "\n---- REAR suspension hardpoint locations (LEFT side)\n";
    std::static_pointer_cast<ChDoubleWishbone>(m_axles[1]->m_suspension)
        ->LogHardpointLocations(ChVector<>(0, 0, 0), false);

    GetLog() << "\n\n";

    GetLog().SetNumFormat("%g");
}

// -----------------------------------------------------------------------------
// Log the spring length, deformation, and force.
// Log the shock length, velocity, and force.
// Log constraint violations of suspension joints.
//
// Lengths are reported in inches, velocities in inches/s, and forces in lbf
// -----------------------------------------------------------------------------
void Sedan_Vehicle::DebugLog(int what) {
    GetLog().SetNumFormat("%10.2f");

    if (what & OUT_SPRINGS) {
        GetLog() << "\n---- Spring (front-left, front-right, rear-left, rear-right)\n";
        GetLog() << "Length [m]       " << GetSpringLength(0, LEFT) << "  " << GetSpringLength(0, RIGHT) << "  "
                 << GetSpringLength(1, LEFT) << "  " << GetSpringLength(1, RIGHT) << "\n";
        GetLog() << "Deformation [m]  " << GetSpringDeformation(0, LEFT) << "  " << GetSpringDeformation(0, RIGHT)
                 << "  " << GetSpringDeformation(1, LEFT) << "  " << GetSpringDeformation(1, RIGHT) << "\n";
        GetLog() << "Force [N]         " << GetSpringForce(0, LEFT) << "  " << GetSpringForce(0, RIGHT) << "  "
                 << GetSpringForce(1, LEFT) << "  " << GetSpringForce(1, RIGHT) << "\n";
    }

    if (what & OUT_SHOCKS) {
        GetLog() << "\n---- Shock (front-left, front-right, rear-left, rear-right)\n";
        GetLog() << "Length [m]       " << GetShockLength(0, LEFT) << "  " << GetShockLength(0, RIGHT) << "  "
                 << GetShockLength(1, LEFT) << "  " << GetShockLength(1, RIGHT) << "\n";
        GetLog() << "Velocity [m/s]   " << GetShockVelocity(0, LEFT) << "  " << GetShockVelocity(0, RIGHT) << "  "
                 << GetShockVelocity(1, LEFT) << "  " << GetShockVelocity(1, RIGHT) << "\n";
        GetLog() << "Force [N]         " << GetShockForce(0, LEFT) << "  " << GetShockForce(0, RIGHT) << "  "
                 << GetShockForce(1, LEFT) << "  " << GetShockForce(1, RIGHT) << "\n";
    }

    if (what & OUT_CONSTRAINTS) {
        // Report constraint violations for all joints
        LogConstraintViolations();
    }

    GetLog().SetNumFormat("%g");
}

}  // end namespace sedan
}  // end namespace vehicle
}  // end namespace chrono
