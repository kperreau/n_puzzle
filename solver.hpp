// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   solver.hpp                                         :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: Nico <marvin@42.fr>                        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2015/08/14 02:06:21 by Nico              #+#    #+#             //
/*   Updated: 2017/03/03 17:19:40 by kperreau         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <queue>
#include <stack>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <climits>
#include <cmath>
#include "header.hpp"
#include "node.class.hpp"

class Solver
{
public:
	Solver();
	virtual ~Solver();
	void solveIDA(Node& start);
	void solveA(Node& start);
	void set_final(Node& final);
	bool is_solvable(Node& n);

private:
	int valid(Node& n);
	void fill_index(void);
	std::vector<Node> get_neighbours(Node& n);
	void construct_solution(void);

	uint mdist(uint x1, uint y1, uint x2, uint y2);
	double mdist_eucli(uint x1, uint y1, uint x2, uint y2);
	uint heuristic(Node& n);
	uint linear_conflict(uint tj_x, uint tj_y, uint tj_x_goal, uint tj_y_goal, Node& n);
	uint linear_conflict_h(int tk_x, int tk_y, int tk_x_goal, int tk_y_goal, Node& n);
	uint linear_conflict_v(int tk_x, int tk_y, int tk_x_goal, int tk_y_goal, Node& n);

	Node* is_in_closed(Node& n);
	Node get_best_from_open();
	// Node* get_from_open_list(Node& n);

	int search(Node n, uint g, uint bound); // IDA*

	Node final;
	bool found;
	std::vector<uchar> index_of_value; // final[i] = x; index[x] = i
	std::vector<Node> solution;

	std::priority_queue<Node> open_list;
	std::unordered_multimap<uint, Node> closed_list;

public:
	uint heuristics;
	uint limit;
	bool verbose;
	uint greedyness;
	bool step_by_step;
	bool step_by_step_key;
};

#endif /* SOLVER_HPP */
