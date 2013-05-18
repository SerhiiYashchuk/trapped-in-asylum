#ifndef SSC_H
#define SSC_H

const float pi =						3.14159265359f;
const int item_z_pos =					2;

//---------------2 dimensional vector structure----------------

struct vector2d {
	float x;
	float y;

	vector2d() : x(0), y(0) {};

	vector2d(float nx, float ny) : x(nx), y(ny) {};

	vector2d operator*(vector2d &rvalue) {
		vector2d result;

		result.x = this->x * rvalue.x;
		result.y = this->y * rvalue.y;

		return result;
	};

	vector2d operator*(int rvalue) {
		vector2d result;

		result.x = this->x * rvalue;
		result.y = this->y * rvalue;

		return result;
	};

	vector2d operator*(float rvalue) {
		vector2d result;

		result.x = this->x * rvalue;
		result.y = this->y * rvalue;

		return result;
	};
};

//---------------------Level floor class---------------------

class level_floor {
public:
	IND_Entity2d *entity;
	vector<level_floor *>near_floor;
	
	level_floor() {
		this->entity =						0;
	};

	level_floor(IND_Entity2d &entity) {
		this->entity =						&entity;
	};

	vector2d get_speed_vector(int speed) {
		float angle =						this->entity->GetAngleZ();
		if (angle < 0) angle =				abs(angle + 180);

		vector2d speed_vector;

		speed_vector.x =					abs(speed * cos(angle * pi / 180));
		speed_vector.y =					abs(speed * sin(angle * pi / 180));

		return speed_vector;
	};

	short get_coord_quarter() {
		float angle =						this->entity->GetAngleZ();
		if (angle < 0) angle =				abs(angle + 180);

		return (short) angle / 90 + 1;
	};
};

//---------------------Level door class----------------------

struct level_door {
	IND_Entity2d *entity;
	char roomID;
	level_door *paired;

	level_door() : entity(0), roomID(0), paired(0) {};
	level_door(IND_Entity2d &d_entity, char rID = 0) : entity(&d_entity), roomID(rID), paired(0) {};
	level_door(IND_Entity2d &d_entity, level_door &paired_d, char rID = 0) : entity(&d_entity), paired(&paired_d), roomID(rID) {}; 
};

//-----------------------Skill class-------------------------

class skill {
	IND_Timer time;
	unsigned char active_time;
public:
	skill() : active_time(0) {};
	skill(unsigned char seconds) : active_time(seconds) {};

	void activate() {this->time.Start();};
	void deactivate() {this->time.Stop();};
	bool is_active() {return this->time.IsStarted();};
	void set_time(unsigned char seconds) {this->active_time = seconds;};
	void update() {if (this->time.GetTicks() / 1000 == this->active_time) this->time.Stop();};
};

//-----------------------Item class-------------------------

class item {
	IND_Entity2d *entity;
	string name;

public:
	item(IND_Entity2d &item_entity, const char *item_name) : entity(&item_entity), name(item_name) {};
	item(IND_Entity2d &item_entity, string &item_name) : entity(&item_entity), name(item_name) {};

	IND_Entity2d &get_entity() {return *this->entity;};
	const string &get_name() {return this->name;};
};

#endif