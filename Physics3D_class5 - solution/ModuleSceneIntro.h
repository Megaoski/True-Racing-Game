#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"




struct PhysBody3D;
struct PhysMotor3D;


class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	Cube CreateRamps(vec3 measures, vec3 position, float angle, const vec3 &u, Color color);
	Cube CreateRoads(vec3 measures, vec3 position, Color color);
	void CreateRoadSensors();
	Cube CreateExternalSensors(vec3 measures, vec3 position, Color color);
	void Turbo(int num);
	void VehicleHasFallen();
	void CreateMap();

public:


	PhysBody3D * pb_chassis;
	Cube p_chassis;

	PhysBody3D* pb_wheel;
	Cylinder p_wheel;

	PhysBody3D* pb_wheel2;
	Cylinder p_wheel2;

	PhysMotor3D* left_wheel;
	PhysMotor3D* right_wheel;


	p2List<Cube> parts;
	bool touched;

	bool turbo_on = false;


private:

	Timer timer;
	uint getTime;


};
