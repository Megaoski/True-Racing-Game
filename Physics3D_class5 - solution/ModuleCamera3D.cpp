#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "PhysVehicle3D.h"
#include "ModulePlayer.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

	test = true;
	x = 0;
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	if(test)//camera locket
	{
		Position.x = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() - 15 * App->player->vehicle->vehicle->getForwardVector().getX();
		Position.y = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getY() + 10 * App->player->vehicle->vehicle->getUpAxis();
		Position.z = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() - 15 * App->player->vehicle->vehicle->getForwardVector().getZ();

		float p_x = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() + 15 * App->player->vehicle->vehicle->getForwardVector().getX();
		float p_z = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() + 15 * App->player->vehicle->vehicle->getForwardVector().getZ();
		LookAt(vec3(p_x, 1, p_z));
	}

	if (!test)
	{
		Position.x = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() - 15 * App->player->vehicle->vehicle->getForwardVector().getX();
		Position.y = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getY() + 20 * App->player->vehicle->vehicle->getUpAxis();
		Position.z = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() - 15 * App->player->vehicle->vehicle->getForwardVector().getZ();

		float p_x = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getX() + 10 * App->player->vehicle->vehicle->getForwardVector().getX();
		float p_z = App->player->vehicle->vehicle->getChassisWorldTransform().getOrigin().getZ() + 10 * App->player->vehicle->vehicle->getForwardVector().getZ();
		LookAt(vec3(p_x, 1, p_z));
	}

	
	

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}