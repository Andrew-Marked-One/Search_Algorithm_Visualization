#pragma once
#include "EntityManager.h"
#include <algorithm>
#include <iostream>

EntityManager::EntityManager() {}
void EntityManager::update() {
	for (auto& e : m_entitiesToAdd) {
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
		m_entityUnorMap[e->id()] = e;
	}

	for (auto itA = m_entities.begin(); itA != m_entities.end(); ) {
		if (!(*itA)->isActive()) {
			size_t idToKill = (*itA)->id();
			auto& entityVec = m_entityMap[(*itA)->tag()];
			for (auto itB = entityVec.begin(); itB != entityVec.end(); itB++) {
				if ((*itB)->id() == idToKill) {
					itB = entityVec.erase(itB);
					break;
				}
			}
			itA = m_entities.erase(itA);

			m_entityUnorMap.erase((*itA)->id());
		}
		else {
			itA++;
		}
	}
	m_entitiesToAdd.clear();
}
std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
	auto e = std::make_shared<Entity>(m_totalEntities++, tag);
	m_entitiesToAdd.push_back(e);
	return e;
}
EntityVec& EntityManager::getEntities() {
	return m_entities;
}
EntityVec& EntityManager::getEntities(const std::string& tag) {
	return m_entityMap[tag];
}

bool compare(const std::shared_ptr<Entity>& e, const size_t b) {
	return e->id() < b;
}

std::shared_ptr<Entity> EntityManager::getEntity(const size_t id) {
	return m_entityUnorMap[id];
}

/*
std::shared_ptr<Entity> EntityManager::getEntity(const size_t id) {
	auto& entities = getEntities();
	auto it =  std::lower_bound(entities.begin(), entities.end(), id, [](const std::shared_ptr<Entity>& a, const size_t target) {
		return a->id() < target;
	});
	if (it != entities.end()) {
		return *it;
	}
	return nullptr;
}
*/