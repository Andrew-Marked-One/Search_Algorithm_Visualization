#pragma once
#include "Vec2.h"
#include "EntityManager.h"
#include "SFML/Graphics.hpp"
#include <functional>
#include <chrono>
#include <unordered_set>
#include <queue>

class Algorithm {
	sf::RenderWindow m_window;

	EntityManager m_entityManager;

	//m_grid x and y must be even numbers
	Vec2 m_grid = { 10, 10 };
	Vec2 m_mPos;

	sf::VertexArray m_gridLines;

	int m_frameLimit       = 60;
	int m_frameCount       = 0;
	int m_iterationSpeed   = 10;
	int m_rightCount       = 0;
	int m_currentAlgorithm = 0;
	int m_cost             = 0;

	float m_AStartWeight = 1;

	bool m_running                 = true;
	bool m_leftPressed             = false;
	bool m_rightPressed            = false;
	bool m_C_KeyPressed            = false;
	bool m_A_KeyPressed            = false;
	bool m_D_KeyPressed            = false;
	bool m_I_KeyPressed            = false;
	bool m_R_KeyPressed            = false;
	bool m_Shift_KeyPressed        = false;
	bool m_allowDiaganalMove       = true;
	bool m_allowInstantCalculation = false;
	bool m_showGrid                = true;
	bool m_inProgress              = false;
	bool m_timerStarted            = false;

	sf::Color m_cellColor   = { 155, 155, 155 };
	sf::Color m_wallColor   = { 55, 55, 55 };
	sf::Color m_startColor  = { 0, 0, 255 };
	sf::Color m_endColor    = { 0, 255, 0 };
	sf::Color m_pathColor   = { 255, 255, 255 };
	sf::Color m_closedColor = { 255, 0, 0 };
	sf::Color m_openColor   = { 255, 255, 0 };
	sf::Color m_gridColor   = { 0, 0, 0 };

	std::chrono::steady_clock::time_point m_startTime;
	std::chrono::steady_clock::time_point m_endTime;

	std::vector<size_t> m_lastModEntID;
	std::vector<std::string> m_algorithmTypes = {
		"BREADTH FIRST SEARCH",
		"DEPTH FIRST SEARCH",
		"UNIFORM-COST SEARCH",
		"GREEDY BEST-FIRST SEARCH",
		"A* SEARCH",
		"WEIGHTED A* SEARCH"
	};
	std::vector<Vec2> m_path;
	std::vector<std::shared_ptr<Entity>> m_openList;
	std::unordered_set<size_t> m_closedList;

	std::shared_ptr<Entity> m_start;
	std::shared_ptr<Entity> m_end;
	//down-right, up-left, up-right, down-left, down, up, right, left
	std::vector<Vec2> m_action = {
		{ m_grid.x, m_grid.y },
		{ -m_grid.x, -m_grid.y },
		{ m_grid.x, -m_grid.y },
		{ -m_grid.x, m_grid.y },
		{ 0, m_grid.y },
		{ 0, -m_grid.y },
		{ m_grid.x, 0 },
		 {-m_grid.x, 0 },
	};
	std::vector<int> m_actionCost = { 141, 141, 141, 141, 100, 100, 100, 100 };
	std::vector<std::function<void(std::shared_ptr<Entity>&)>> m_strategies = {
		[this](std::shared_ptr<Entity>& e) { BreadthFirstSearch_strat(e); },
		[this](std::shared_ptr<Entity>& e) { DepthFirstSearch_strat(e); },
		[this](std::shared_ptr<Entity>& e) { UniformCostSearch_strat(e); },
		[this](std::shared_ptr<Entity>& e) { GreedyBestFirstSearch_strat(e); },
		[this](std::shared_ptr<Entity>& e) { AStarSearch_strat(e); },
		[this](std::shared_ptr<Entity>& e) { AStarSearch_strat(e); }
	};

	void startTimer();
	void endTimer();
	void createCell(const float posX, const float posY);
	void mouseInputResolution();
	std::shared_ptr<Entity> isInside(const Vec2& mPos);
	bool isInLastMod(size_t id);
	bool isOutOfBounds(const Vec2& pos) const;
	bool isLegalAction(const std::shared_ptr<Entity>& e, const Vec2& action) ;
	bool isInClosed(const size_t id);
	bool isInOpen(const size_t id) const;
	std::shared_ptr<Entity> findByPos(const Vec2& pos);
	void clearCanvas();
	void colorClosedAndOpen();
	void colorPath();
	void BreadthFirstSearch_strat(std::shared_ptr<Entity>& e);
	void DepthFirstSearch_strat(std::shared_ptr<Entity>& e);
	void UniformCostSearch_strat(std::shared_ptr<Entity>& e);
	void GreedyBestFirstSearch_strat(std::shared_ptr<Entity>& e);
	void AStarSearch_strat(std::shared_ptr<Entity>& e);
	void startSearch();
	void searchIteration();
	void recomputePath();
	void input();
	void render();
public:
	Algorithm();
	void run();
};