#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ModulePhysics3D.h"
#include "ModulePlayer.h"
#include "PhysVehicle3D.h"
#include "ModuleAudio.h"


#define SIZE_ARRAY(_A_) (sizeof(_A_)/sizeof(_a_[0]))


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	
	CreateMap();
	
	for (p2List_item<Cube>* item = parts.getFirst(); item; item = item->next)
	{
		App->physics->AddBody(item->data, 0.0f, false);
	}

	
	
	if (App->player->endmusic == false || App->player->winmusic == false)
	{
		App->audio->PlayMusic("music/freestyla.ogg");
	}
	else
		App->audio->PlayMusic("music/freestyla.ogg", 0.0f);//play fx and stop actual audio
	
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{

	for (p2List_item<Cube>* item = parts.getFirst(); item; item = item->next)
	{
		item->data.Render();
		/*LOG("PINTA LAS ROADS");*/
	}
		
		
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

	if (body1 == App->player->FirstTurbo.body)
	{
		Turbo(1);
	}

	if (body1 == App->player->SecondTurbo.body)
	{
		Turbo(2);
	}

	if (body1 == App->player->DeadFloor.body)
	{
		VehicleHasFallen();
	}
	
}


Cube ModuleSceneIntro::CreateRamps(vec3 measures, vec3 position, float angle, const vec3 &u, Color color)
{	
	Cube example(measures.x, measures.y, measures.z);
	example.SetPos(position.x, position.y, position.z);
	example.SetRotation(angle, u);
	example.color = color;
	parts.add(example);
	/*LOG("CREA LA RAMPA");*/
	

	return example;
	
	
}


void ModuleSceneIntro::CreateRoadSensors()
{
	Cube turbo1(40, 5, 20);
	turbo1.SetPos(-110, 5, 165);
	turbo1.color = Green;
	

	App->player->FirstTurbo.cube = turbo1;
	App->player->FirstTurbo.body = App->physics->AddBody(turbo1, 0, true);
	App->player->FirstTurbo.body->collision_listeners.add(this);
	App->player->turbos.add(App->player->FirstTurbo);

	Cube turbo2(20, 5, 40);
	turbo2.SetPos(-200, 5, 222.5f);
	turbo2.color = Green;

	App->player->SecondTurbo.cube = turbo2;
	App->player->SecondTurbo.body = App->physics->AddBody(turbo2, 0, true);
	App->player->SecondTurbo.body->collision_listeners.add(this);
	App->player->turbos.add(App->player->SecondTurbo);

	Cube goalzone(20, 5, 40);
	goalzone.SetPos(0, 0, 0);
	goalzone.color = Black;

	App->player->goal.goalZone = goalzone;
	App->player->goal.goalzone_body = App->physics->AddBody(turbo2, 0, true);
	App->player->goal.goalzone_body->collision_listeners.add(this);
	App->player->goals.add(App->player->goal);
	timer.Start();
	getTime = 0;
	
}

Cube ModuleSceneIntro::CreateRoads(vec3 measures, vec3 position, Color color)
{
	Cube example(measures.x, measures.y, measures.z);
	example.SetPos(position.x, position.y,position.z);
	example.color = color;
	parts.add(example);

	return example;
}

Cube ModuleSceneIntro::CreateExternalSensors(vec3 measures, vec3 position, Color color)
{

	Cube example(measures.x, measures.y, measures.z);
	example.SetPos(position.x, position.y, position.z);
	example.color = color;


	App->player->DeadFloor.cube = example;
	App->player->DeadFloor.body = App->physics->AddBody(example, 0, true);
	App->player->DeadFloor.body->collision_listeners.add(this);
	App->player->turbos.add(App->player->DeadFloor);

	

	return example;
}

void ModuleSceneIntro::Turbo(int num)
{
	if (num == 1)
	{
		App->player->vehicle->body->setLinearVelocity(btVector3(-35, 10, 0));
		App->player->vehicle->body->setAngularVelocity(btVector3(0, 0, 0));
	}
	else if (num == 2)
	{
		App->player->vehicle->body->setLinearVelocity(btVector3(0, 15, -50));
		App->player->vehicle->body->setAngularVelocity(btVector3(0, 0, 0));
	}
}


void ModuleSceneIntro::VehicleHasFallen()
{
	App->player->vehicle->SetTransform(IdentityMatrix.M);
	App->player->vehicle->SetPos(App->player->initial_pos.x, App->player->initial_pos.y, App->player->initial_pos.z);
	
	App->player->vehicle->body->setLinearVelocity(btVector3(0,0,0));
	App->player->vehicle->body->setAngularVelocity(btVector3(0, 0, 0));
	App->player->brake = BRAKE_POWER;

	App->player->lives--;
	
	
}



void ModuleSceneIntro::CreateMap()
{
	
	Cube bigsensor = CreateExternalSensors({ 2000, 1, 1600 }, { 22, 0, 60 }, Red);// Use NoColor in case we want to hide it
	CreateRoadSensors();
	

	

	Cube ramp1 = CreateRamps({ 20, 3, 20 }, { 20, 10, 20 }, -12, { 1, 0, 0 }, Blue);//just in case we need ramps
	

	
	
	Cube road1 = CreateRoads({ 20, 5, 270 }, { 0, 5, 20 }, Grey);
	Cube road2 = CreateRoads({ 100, 5, 20 }, { -40, 5, 165 }, Grey);
	Cube road3 = CreateRoads({ 160, 5, 20 }, { -230, 5, 165 }, Grey);
	Cube road4 = CreateRoads({ 20, 5, 120 }, { -320, 5, 115 }, Grey);
	Cube road5 = CreateRoads({ 160, 5, 20 }, { -410, 5, 65 }, Grey);
	Cube road6 = CreateRoads({ 20, 5, 300 }, { -500, 5, 205 }, Grey);
	Cube road7 = CreateRoads({ 300, 5, 20 }, { -360, 5, 365 }, Grey);
	Cube road8 = CreateRoads({ 20, 5, 135 }, { -200, 5, 310}, Grey);

	Cube road9 = CreateRoads({ 20, 5, 250 }, { -200, 5, -10 }, Grey);
	Cube road10 = CreateRoads({ 230, 5, 20 }, { -105, 5, -125 }, Grey);

}