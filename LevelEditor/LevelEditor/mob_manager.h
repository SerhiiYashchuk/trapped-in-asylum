#ifndef MOB_MANAGER_H
#define MOB_MANAGER_H

#include "CIndieLib_vc2008.h"
#include <vector>

//---------------mob class---------------

struct mob {
	IND_Entity2d entity;
	vector<IND_Point> trj;
	string type;
};

//---------------mob manager class---------------

class mob_manager {
	CIndieLib *engine;
	vector<mob *> mobs;
public:
	mob_manager();
	~mob_manager();
	bool init(CIndieLib *engine);
	bool add(IND_Surface *surface, const char *type, int x, int y);
	bool add(mob *new_mob);
	void del(unsigned short pos);
	void del(mob *amob);
	mob *get(unsigned short pos);
	mob *get(IND_Entity2d *entity);
	unsigned short size();
	void render();
};

#endif