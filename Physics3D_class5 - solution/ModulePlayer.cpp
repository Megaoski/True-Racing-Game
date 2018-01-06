#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
	min = 0;
	lives = 3;
	winmusic = false;
	endmusic = false;
	deadplayer = false;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	initial_pos = { 0, 17, 0};
	runtime.Start();
	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2, 2, 5);
	car.chassis_offset.Set(0, 1, 0);
	car.front_size.Set(1.6f, 1.6f, 1.2f);
	car.front_offset.Set(0, 1, 4);
	car.back_size.Set(2, 1, 2.2f);
	car.back_offset.Set(0, 1, -2);
	car.left_backlight_size.Set(0.5f, 0.4f, 0.4f);
	car.left_backlight_offset.Set(0.75f, 0.8f, -3);
	car.right_backlight_size.Set(0.5f, 0.4f, 0.4f);
	car.right_backlight_offset.Set(-0.75f, 0.8f, -3);

	car.mass = 700.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 500.0f;
	car.frictionSlip = 50.0f;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.7f;
	float wheel_width = 1.1f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;
	
	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(initial_pos.x,initial_pos.y, initial_pos.z);
	memset(car_transformed_matrix, 0, sizeof(car_transformed_matrix));
	vehicle->GetTransform(car_transformed_matrix);
	vehicle->collision_listeners.add(this);

	best_time = 0;
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

void ModulePlayer::RestartPlayer() {

	vehicle->SetPos(initial_pos.x, initial_pos.y, initial_pos.z);

	vehicle->SetTransform(car_transformed_matrix);
	App->player->vehicle->body->setLinearVelocity(btVector3(0, 0, 0));
	App->player->vehicle->body->setAngularVelocity(btVector3(0, 0, 0));

	App->player->lives = 3;
	App->player->timer = 0;

}
// Update: draw background
update_status ModulePlayer::Update(float dt)
{

	p2List_item<Turbos>* item = turbos.getFirst();
	while (item)
	{
		item->data.cube.Render();
		item = item->next;
	}

	turn = acceleration = brake = 0.0f;

	if(!deadplayer)

		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) {

			RestartPlayer();
		}
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			if (vehicle->GetKmh() < 50.0f) {
				acceleration = MAX_ACCELERATION * 2;
			}
			else if (vehicle->GetKmh() > 50.0f) {
				acceleration = MAX_ACCELERATION;
			}
			if (vehicle->GetKmh() < 0) {
				brake = BRAKE_POWER / 4;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (turn < TURN_DEGREES)
				turn += TURN_DEGREES;

			if (vehicle->left_light == false) vehicle->left_light = true;
			else if (vehicle->left_light == true) vehicle->left_light = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (turn > -TURN_DEGREES)
				turn -= TURN_DEGREES;

			if (vehicle->right_light == false) vehicle->right_light = true;
			else if (vehicle->right_light == true) vehicle->right_light = false;
			
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			if (vehicle->GetKmh() > -30.0f) {
				acceleration = -MAX_ACCELERATION;
				if (vehicle->lights_on == false) vehicle->lights_on = true;
				else if (vehicle->lights_on == true) vehicle->lights_on = false;
			}
			else if (vehicle->GetKmh() > 0) {
				brake = BRAKE_POWER / 4;
			}

			
		}


		if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN) //changing camera view
		{

			App->camera->test = false;
			cameracounter++;

			if (cameracounter == 2)
			{
				App->camera->test = true;
				cameracounter = 0;
			}
		}

	}
	
	

	if (vehicle->GetKmh() > 0) {
		vehicle->ApplyEngineForce(-150.0f);
	}
	else if (vehicle->GetKmh() < 0) {
		vehicle->ApplyEngineForce(150.0f);

		if (vehicle->GetKmh() > 0 && App->input->GetKey(SDL_SCANCODE_UP) != KEY_REPEAT) {
			acceleration = -200.0f;
		}
		if (vehicle->GetKmh() < 0 && App->input->GetKey(SDL_SCANCODE_DOWN) != KEY_REPEAT) {
			acceleration = 200.0f;
		}
	}
	

	if ((int)runtime.ReadSec() == 60)
	{
		min++;
		runtime.Start();
	}

	if (lives == 0)
	{
		deadplayer = true;
		EndRun();
	}

	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	char title[80];

	if (!deadplayer)
	{
		velocity = vehicle->GetKmh();
		sprintf_s(title, "%.1f Km/h      Time: %i : %i       HP: %i  ", vehicle->GetKmh(), min, (int)runtime.ReadSec(), lives);
		App->window->SetTitle(title);
	}
	
	else
	{
		runtime.Stop(); //porque no para?
		velocity = 0;
		sprintf_s(title, "%.1f Km/h      Total Time: %i : %i       HP: %i  ", vehicle->GetKmh(), min, total_time, lives);
		App->window->SetTitle(title);
	}
	

	return UPDATE_CONTINUE;
}


void ModulePlayer::EndRun()
{
	char gameover[50];
	deadplayer = true;


	sprintf_s(gameover, "You lost! --------> To play again press R");
	App->window->SetTitle(gameover);

	
	/*endmusic = true;*/
	
}

void ModulePlayer::NewRun()
{
	lives = 3;

}



