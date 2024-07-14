#pragma once
#include <vector>
#include <map>
#include <unordered_map>
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;

class EntityManager {
	EntityVec                        m_entities;
	EntityVec                        m_entitiesToAdd;
	std::map<std::string, EntityVec> m_entityMap;
	std::unordered_map<size_t, std::shared_ptr<Entity>> m_entityUnorMap;
	size_t                           m_totalEntities = 0;
public:
	EntityManager();
	void update();
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	EntityVec& getEntities();
	EntityVec& getEntities(const std::string& tag);
	std::shared_ptr<Entity> getEntity(const size_t id);
};
