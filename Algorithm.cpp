#pragma once
#include "Algorithm.h"
#include <iostream>
#include <algorithm>

Algorithm::Algorithm()
	: m_gridLines(sf::Lines) {
	int width = round(ceil(1200 / m_grid.x) * m_grid.x);
	int height = round(ceil(700 / m_grid.y) * m_grid.y);
	m_window.create(sf::VideoMode(width, height), "BFS algorithm");
	m_window.setFramerateLimit(m_frameLimit);
	Vec2 gridHalfSize = m_grid / 2;
	int anountX = m_window.getSize().x / m_grid.x;
	int anountY = m_window.getSize().y / m_grid.y;
	for (size_t i = 0; i < anountX; i++) {
		for (size_t j = 0; j < anountY; j++) {
			createCell(i * m_grid.x + gridHalfSize.x, j * m_grid.y + gridHalfSize.y);
		}
	}
	m_entityManager.update();
	for (auto& e : m_entityManager.getEntities()) {
		auto& eT = e->getComponent<CTransform>().pos;
		e->getComponent<CShape>().shape.setPosition(eT.x, eT.y);
	}

	for (int i = 0; i < width / m_grid.x + 1; i++) {
		m_gridLines.append(sf::Vertex(sf::Vector2f(m_grid.x * i, 0), m_gridColor));
		m_gridLines.append(sf::Vertex(sf::Vector2f(m_grid.x * i, height), m_gridColor));
	}
	for (int i = 0; i < height / m_grid.y + 1; i++) {
		m_gridLines.append(sf::Vertex(sf::Vector2f(0, m_grid.y * i), m_gridColor));
		m_gridLines.append(sf::Vertex(sf::Vector2f(width, m_grid.y * i), m_gridColor));
	}
	std::cout << "Current algorithm is: " << m_algorithmTypes[m_currentAlgorithm] << '\n';
	std::cout << "8 directions are legal" << '\n';
}

void Algorithm::startTimer() {
	m_startTime = std::chrono::steady_clock::now();
	m_timerStarted = true;
}

void Algorithm::endTimer() {
	m_endTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = m_endTime - m_startTime;
	std::cout << "Search time: " << elapsed_seconds << '\n';
	m_timerStarted = false;
}

void Algorithm::createCell(const float posX, const float posY) {
	std::shared_ptr<Entity> cell = m_entityManager.addEntity("cell");
	cell->addComponent<CTransform>(Vec2(posX, posY));
	cell->addComponent<CShape>(m_grid.x, m_grid.y, m_cellColor, sf::Color(0, 0, 0), 0.f);
}

void Algorithm::mouseInputResolution() {
	std::shared_ptr<Entity> e = isInside(m_mPos);
	if (!e) {
		return;
	}
	size_t eID = e->id();
	if (m_leftPressed && !isInOpen(eID) && !isInClosed(eID) && !isInLastMod(eID)) {
		if (e->type == 0) {
			e->type = 1;
			e->getComponent<CShape>().shape.setFillColor(m_wallColor);
		}
		else if (e->type == 1) {
			e->type = 0;
			e->getComponent<CShape>().shape.setFillColor(m_cellColor);
		}
		m_lastModEntID.push_back(e->id());
	}
	else if (m_rightPressed) {
		if (m_rightCount % 2 == 0) {
			clearCanvas();
			e->type = 2;
			m_start = e;
			m_start->getComponent<CShape>().shape.setFillColor(m_startColor);
		}
		else if (m_rightCount % 2 == 1) {
			e->type = 3;
			m_end = e;
			m_end->getComponent<CShape>().shape.setFillColor(m_endColor);
			startSearch();
		}
		m_rightCount++;
	}
}

std::shared_ptr<Entity> Algorithm::isInside(const Vec2& mPos) {
	Vec2 gridHalfSize = m_grid / 2;
	Vec2 newMPos = ((mPos - gridHalfSize) / m_grid).round() * m_grid + gridHalfSize;
	return findByPos(newMPos);
}

bool Algorithm::isInLastMod(const size_t id) {
	return std::find(m_lastModEntID.begin(), m_lastModEntID.end(), id) != m_lastModEntID.end();
}

bool Algorithm::isOutOfBounds(const Vec2& pos) const{
	Vec2 offsetPos = pos - m_grid / 2;
	Vec2 windowSize = m_window.getSize();
	return offsetPos.x < 0 || offsetPos.x >= windowSize.x || offsetPos.y < 0 || offsetPos.y >= windowSize.y;
}

bool Algorithm::isLegalAction(const std::shared_ptr<Entity>& e, const Vec2& action) {
	auto& eT = e->getComponent<CTransform>().pos;
	if (e->type == 1 || isOutOfBounds(eT)) {
		return false;
	}
	if (m_allowDiaganalMove) {
		Vec2 cell1 = { eT.x - action.x, eT.y };
		Vec2 cell2 = { eT.x, eT.y - action.y };
		if (findByPos(cell1)->type == 1 || findByPos(cell2)->type == 1) {
			return false;
		}
	}
	return true;
}

bool Algorithm::isInClosed(const size_t id) {
	return m_closedList.find(id) != m_closedList.end();
}

bool Algorithm::isInOpen(const size_t id) const {
	return std::find_if(m_openList.begin(), m_openList.end(), 
		[id](const std::shared_ptr<Entity>& e) {
			return e->id() == id;
		}) != m_openList.end();
}

std::shared_ptr<Entity> Algorithm::findByPos(const Vec2& pos) {
	if (isOutOfBounds(pos)) {
		return nullptr;
	}
	Vec2 gridHalfSize = m_grid / 2;
	unsigned int yAmount = m_window.getSize().y / m_grid.y;
	unsigned int indexX = pos.x - gridHalfSize.x == 0 ? 0 : ((pos.x - gridHalfSize.x) / m_grid.x);
	unsigned int indexY = pos.y - gridHalfSize.y == 0 ? 0 : ((pos.y - gridHalfSize.y) / m_grid.y);
	unsigned int index = yAmount * indexX + indexY;
	return m_entityManager.getEntity(static_cast<size_t>(index));
}

void Algorithm::clearCanvas() {
	//0 - cell, 1 - wall, 2 - start, 3 - end, 4 - open, 5 - closed, 6 - path;
	bool m_inProgress = false;
	m_path.clear();
	m_openList.clear();
	m_closedList.clear();
	m_cost = 0;
	for (auto& e : m_entityManager.getEntities()) {
		if (e->type != 1 || m_C_KeyPressed) {
			e->type = 0;
			e->getComponent<CShape>().shape.setFillColor(m_cellColor);
		}
		if (e->hasComponent<CNode>()) {
			e->removeComponent<CNode>();
		}
	}
	if (m_start && !m_end) {
		m_rightCount--;
	}
	if (m_start) {
		m_start = nullptr;
	}
	if (m_end) {
		m_end = nullptr;
	}
}

void Algorithm::colorClosedAndOpen() {
	for (auto& closed : m_closedList) {
		auto e = m_entityManager.getEntity(closed);
		if (e->hasChanged) {
			e->getComponent<CShape>().shape.setFillColor(m_closedColor);
			e->hasChanged = false;
		}
	}
	for (auto& open : m_openList) {
		if (open->hasChanged) {
			open->getComponent<CShape>().shape.setFillColor(m_openColor);
			open->hasChanged = false;
		}
	}
}

void Algorithm::colorPath() {
	Vec2 prevPos = m_start->getComponent<CTransform>().pos;
	for (auto& path : m_path) {
		findByPos(prevPos)->getComponent<CShape>().shape.setFillColor(m_pathColor);
		prevPos += path;
	}
}

void Algorithm::BreadthFirstSearch_strat(std::shared_ptr<Entity>& e) {
	e = m_openList[0];
	m_openList.erase(m_openList.begin());
}

void Algorithm::DepthFirstSearch_strat(std::shared_ptr<Entity>& e) {
	e = m_openList.back();
	m_openList.pop_back();
}

void Algorithm::UniformCostSearch_strat(std::shared_ptr<Entity>& e) {
	auto it = std::min_element(m_openList.begin(), m_openList.end(), 
		[](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
			int aCost = a->getComponent<CNode>().cost;
			int bCost = b->getComponent<CNode>().cost;
			return aCost < bCost;
		});
	e = *it;
	m_openList.erase(it);
}

void Algorithm::GreedyBestFirstSearch_strat(std::shared_ptr<Entity>& e) {
	auto it = std::min_element(m_openList.begin(), m_openList.end(),
		[&](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
			int distCostA = a->getComponent<CNode>().distCost;
			int distCostB = b->getComponent<CNode>().distCost;
			return distCostA < distCostB;
		});
	e = *it;
	m_openList.erase(it);
}

void Algorithm::AStarSearch_strat(std::shared_ptr<Entity>& e) {
	auto it = std::min_element(m_openList.begin(), m_openList.end(),
		[&](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
			int aCost = a->getComponent<CNode>().cost;
			int bCost = b->getComponent<CNode>().cost;
			int distCostA = a->getComponent<CNode>().distCost;
			int distCostB = b->getComponent<CNode>().distCost;

			int evaluationA;
			int evaluationB;
			if (m_currentAlgorithm == 5) {
				evaluationA = aCost + m_AStartWeight * distCostA;
				evaluationB = bCost + m_AStartWeight * distCostB;
			}
			else {
				evaluationA = aCost + distCostA;
				evaluationB = bCost + distCostB;
			}

			return evaluationA == evaluationB ? distCostA < distCostB : evaluationA < evaluationB;
		});
	e = *it;
	m_openList.erase(it);
}

void Algorithm::startSearch(){
	if (m_start) {
		m_inProgress = true;
		m_openList.emplace_back(m_start);
		m_start->addComponent<CNode>(Vec2(0, 0), Vec2(0, 0), 0);
	}
}

void Algorithm::searchIteration() {
	if (!m_inProgress) {
		return;
	}

	if (!m_timerStarted) {
		startTimer();
	}

	if (m_openList.empty()) {
		m_inProgress = false;
		m_cost = -1;
		endTimer();
		std::cout << "Cost is: " << m_cost << '\n';
		return;
	}
	std::shared_ptr<Entity> node;
	m_strategies[m_currentAlgorithm](node);
	auto& nodeT = node->getComponent<CTransform>().pos;
	auto& nodeN = node->getComponent<CNode>();
	size_t nodeID = node->id();
	if (nodeID == m_end->id()) {
		size_t startID = m_start->id();
		std::cout << "Cost is: " << nodeN.cost << '\n';
		while (nodeID != startID) {
			m_path.emplace_back(nodeN.action);
			node = findByPos(nodeN.parent);
			nodeN = node->getComponent<CNode>();
			nodeID = node->id();
		}
		std::reverse(m_path.begin(), m_path.end());
		endTimer();
		m_inProgress = false;
		return;
	}
	if (isInClosed(nodeID)) {
		return;
	}
	node->hasChanged = true;
	m_closedList.insert(nodeID);
	auto& endT = m_end->getComponent<CTransform>().pos;
	for (size_t i = 0; i < m_action.size(); i++) {
		if (!m_allowDiaganalMove && i < 3) {
			i = 4;
		}
		Vec2 newNodePos = nodeT + m_action[i];
		std::shared_ptr<Entity> newNode = findByPos(newNodePos);
		if (!newNode) {
			continue;
		}
		size_t newNodeID = newNode->id();
		if (newNodePos != nodeN.parent && !isInClosed(newNodeID) && !isInOpen(newNodeID) && isLegalAction(newNode, m_action[i])) {
			int newNodeCost = nodeN.cost + m_actionCost[i];
			newNode->addComponent<CNode>(m_action[i], nodeT, newNodeCost);
			newNode->hasChanged = true;
			auto& newNodeT = newNode->getComponent<CTransform>().pos;
			Vec2 dist = (endT - newNodeT).abs() / m_grid;
			int distCost;
			if (m_allowDiaganalMove) {
				distCost = dist.min() * m_actionCost[0] + (dist.max() - dist.min()) * m_actionCost[4];
			}
			else {
				distCost = (dist.x + dist.y) * m_actionCost[4];
			}
			newNode->getComponent<CNode>().distCost = distCost;
			m_openList.emplace_back(newNode);
		}
	}
}

void Algorithm::recomputePath() {
	if (m_start && m_end) {
		std::shared_ptr<Entity> tempStart = m_start;
		std::shared_ptr<Entity> tempEnd = m_end;
		clearCanvas();
		m_start = tempStart;
		m_end = tempEnd;
		startSearch();
	}
}

void Algorithm::input() {
	sf::Event event;
	if (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			m_running = false;
		}

		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape) {
				m_running = false;
			}
			else if (event.key.code == sf::Keyboard::Up) {
				if (!m_Shift_KeyPressed) {
					m_iterationSpeed -= 2;
					if (m_iterationSpeed < 1) {
						m_iterationSpeed = 1;
					}
				}
				else {
					m_AStartWeight *= 1.1;
					if (m_AStartWeight < 0.05) {
						m_AStartWeight = 0.05;
					}
					std::cout << m_AStartWeight << '\n';
				}
			}
			else if (event.key.code == sf::Keyboard::Down) {
				if (!m_Shift_KeyPressed) {
					m_iterationSpeed += 2;
				}
				else {
					m_AStartWeight *= 0.9;
					if (m_AStartWeight < 0.05) {
						m_AStartWeight = 0;
					}
					std::cout << m_AStartWeight << '\n';

				}
			}
			else if (event.key.code == sf::Keyboard::G) {
				m_showGrid = !m_showGrid;
			}
			else if (event.key.code == sf::Keyboard::A) {
				if (!m_A_KeyPressed) {
					m_currentAlgorithm++;
					if (m_currentAlgorithm >= m_algorithmTypes.size()) {
						m_currentAlgorithm = 0;
					}
					std::cout << "Current algorithm is: " << m_algorithmTypes[m_currentAlgorithm] << '\n';
					recomputePath();
				}
				m_A_KeyPressed = true;
			}
			else if (event.key.code == sf::Keyboard::C) {
				m_C_KeyPressed = true;
				clearCanvas();
			}
			else if (event.key.code == sf::Keyboard::D) {
				if (!m_D_KeyPressed) {
					m_allowDiaganalMove = !m_allowDiaganalMove;
					recomputePath();
				}
				if (m_allowDiaganalMove) {
					std::cout << "8 directions are legal" << '\n';
				}
				else {
					std::cout << "4 directions are legal" << '\n';
				}
				m_D_KeyPressed = true;
			}
			else if (event.key.code == sf::Keyboard::I) {
				if (!m_I_KeyPressed) {
					m_allowInstantCalculation = !m_allowInstantCalculation;
				}
				if (m_allowInstantCalculation) {
					std::cout << "Instant path calculation is ENABLED" << '\n';
				}
				else {
					std::cout << "Instant path calculation is DISABLED" << '\n';
				}
				m_I_KeyPressed = true;
			}
			else if (event.key.code == sf::Keyboard::R) {
				if (!m_R_KeyPressed) {
					recomputePath();
				}
				m_R_KeyPressed = true;
			}
			else if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
				m_Shift_KeyPressed = true;
			}
		}
		else if (event.type == sf::Event::KeyReleased) {
			if (event.key.code == sf::Keyboard::A) {
				m_A_KeyPressed = false;
			}
			else if (event.key.code == sf::Keyboard::C) {
				m_C_KeyPressed = false;
			}
			else if (event.key.code == sf::Keyboard::D) {
				m_D_KeyPressed = false;
			}
			else if (event.key.code == sf::Keyboard::I) {
				m_I_KeyPressed = false;
			}
			else if (event.key.code == sf::Keyboard::R) {
				m_R_KeyPressed = false;
			}
			else if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift) {
				m_Shift_KeyPressed = false;
			}
		}
		else if (event.type == sf::Event::MouseMoved) {
			m_mPos = sf::Mouse::getPosition(m_window);
			if (m_leftPressed) {
				mouseInputResolution();
			}
		}
		else if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left && !m_rightPressed) {
				m_leftPressed = true;
				mouseInputResolution();
			}
			else if (event.mouseButton.button == sf::Mouse::Right && !m_leftPressed) {
				m_rightPressed = true;
				mouseInputResolution();
			}
		}
		else if (event.type == sf::Event::MouseButtonReleased) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				m_leftPressed = false;
				m_lastModEntID.clear();
			}
			else if (event.mouseButton.button == sf::Mouse::Right) {
				m_rightPressed = false;
			}
		}
	}
}

void Algorithm::render() {
	m_window.clear();

	sf::VertexArray vertexArray(sf::Quads);
	auto& entities = m_entityManager.getEntities();
	vertexArray.resize(entities.size() * 4);
	size_t vertexIndex = 0;
	Vec2 gridHalfSize = m_grid / 2;
	sf::Color color;
	for (auto& e : entities) {
		auto& eSh = e->getComponent<CShape>().shape;
		auto& eT = e->getComponent<CTransform>().pos;
		color = eSh.getFillColor();
		vertexArray[vertexIndex++] = sf::Vertex(sf::Vector2f(eT.x - gridHalfSize.x, eT.y - gridHalfSize.y), color);
		vertexArray[vertexIndex++] = sf::Vertex(sf::Vector2f(eT.x + gridHalfSize.x, eT.y - gridHalfSize.y), color);
		vertexArray[vertexIndex++] = sf::Vertex(sf::Vector2f(eT.x + gridHalfSize.x, eT.y + gridHalfSize.y), color);
		vertexArray[vertexIndex++] = sf::Vertex(sf::Vector2f(eT.x - gridHalfSize.x, eT.y + gridHalfSize.y), color);
	}
	m_window.draw(vertexArray);

	if (m_showGrid) {
		m_window.draw(m_gridLines);
	}

	m_window.display();
}

void Algorithm::run() {
	while (m_running) {
		input();
		if (m_inProgress && m_frameCount % m_iterationSpeed == 0 && m_start && m_end) {
			if (m_allowInstantCalculation) {
				while (m_inProgress) {
					searchIteration();
				}
			}
			else {
				searchIteration();
			}
			colorClosedAndOpen();
			colorPath();
			m_start->getComponent<CShape>().shape.setFillColor(m_startColor);
			m_end->getComponent<CShape>().shape.setFillColor(m_endColor);
		}
		render();
		m_frameCount++;
	}
}