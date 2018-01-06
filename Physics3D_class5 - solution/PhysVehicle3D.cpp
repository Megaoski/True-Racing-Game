#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder wheel;

	wheel.color = Black;

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[0].radius;
		wheel.height = info.wheels[0].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.Render();
	}

	// Chassis

	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	chassis.color = Blue;
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();



	//Front of the car

	Cube front(info.front_size.x, info.front_size.y, info.front_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&front.transform);
	front.color = Blue;
	btQuaternion q_f = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset_f(info.front_offset.x, info.front_offset.y, info.front_offset.z);
	offset_f = offset_f.rotate(q_f.getAxis(), q_f.getAngle());
	
	front.transform.M[12] += offset_f.getX();
	front.transform.M[13] += offset_f.getY();
	front.transform.M[14] += offset_f.getZ();
	

	//left light

	Cube back1(info.left_backlight_size.x, info.left_backlight_size.y, info.left_backlight_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&back1.transform);
	back1.color = Red;
	btQuaternion q_b1 = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset_b1(info.left_backlight_offset.x, info.left_backlight_offset.y, info.left_backlight_offset.z);
	offset_b1 = offset_b1.rotate(q_b1.getAxis(), q_b1.getAngle());

	back1.transform.M[12] += offset_b1.getX();
	back1.transform.M[13] += offset_b1.getY();
	back1.transform.M[14] += offset_b1.getZ();


	//Right light

	Cube back2(info.right_backlight_size.x, info.right_backlight_size.y, info.right_backlight_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&back2.transform);
	back2.color = Red;
	btQuaternion q_b2 = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset_b2(info.right_backlight_offset.x, info.right_backlight_offset.y, info.right_backlight_offset.z);
	offset_b2 = offset_b2.rotate(q_b2.getAxis(), q_b2.getAngle());

	back2.transform.M[12] += offset_b2.getX();
	back2.transform.M[13] += offset_b2.getY();
	back2.transform.M[14] += offset_b2.getZ();

	if (lights_on) {
		
		back1.color = Yellow;
		back2.color = Yellow;
	}

	//Back chassis

	Cube back(info.back_size.x, info.back_size.y, info.back_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&back.transform);
	back.color = Blue;
	btQuaternion q_b = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset_b(info.back_offset.x, info.back_offset.y, info.back_offset.z);
	offset_b = offset_b.rotate(q_b.getAxis(), q_b.getAngle());

	back.transform.M[12] += offset_b.getX();
	back.transform.M[13] += offset_b.getY();
	back.transform.M[14] += offset_b.getZ();

	chassis.Render();
	front.Render();
	back.Render();
	back1.Render();
	back2.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}
