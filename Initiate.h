#pragma once

class hgeResourceManager;
class World;
class Entity;

hgeResourceManager* CreateResourceManager();
World* InitiateWorld();

void InitiatePlayer(int id);

void RefreshPlayer(Entity *guy);

void UpdatePlayerPos();