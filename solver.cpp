// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   solver.cpp                                         :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: Nico <marvin@42.fr>                        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2015/08/14 02:06:33 by Nico              #+#    #+#             //
//   Updated: 2017/03/03 17:49:11 by ntibi            ###   ########.fr       //
/*   Updated: 2017/03/03 17:27:30 by kperreau         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#include "solver.hpp"

Solver::Solver() : found(false), verbose(0), step_by_step(0) {}
Solver::~Solver() {}

void Solver::set_final(Node& final) { this->final = final; fill_index(); }

int Solver::valid(Node& n) { return (final == n); }

std::vector<Node> Solver::get_neighbours(Node& n)
{
	std::vector<Node> out;

	if (n.try_move_left() && n.lastmov != Node::RIGHT)
		out.push_back(n.move_left());
	if (n.try_move_down() && n.lastmov != Node::UP)
		out.push_back(n.move_down());
	if (n.try_move_right() && n.lastmov != Node::LEFT)
		out.push_back(n.move_right());
	if (n.try_move_up() && n.lastmov != Node::DOWN)
		out.push_back(n.move_up()); 

	return (out);
}

Node* Solver::is_in_closed(Node& n)
{
	auto matches = closed_list.equal_range(n._hash);
	for (auto it = matches.first; it != matches.second; ++it)
	{
		if (n == it->second)
			return (&(it->second));
	}
	return (nullptr);
}

uint Solver::linear_conflict_h(int tk_x, int tk_y, int tk_x_goal, int tk_y_goal, Node& n)
{
	int coord, tj_y, tj_x, tj_x_goal, tj_y_goal;
	int size = (int)Node::get_size();

	tj_y = tk_y;
	for (tj_x = tk_x + 1; tj_x < size; ++tj_x)
	{
		coord = tj_x + tj_y * size;
		if (!n.values[coord])
			continue ;
		tj_x_goal = index_of_value[n.values[coord]] % size;
		tj_y_goal = index_of_value[n.values[coord]] / size;
		if (tj_y_goal == tk_y_goal && tj_x_goal < tk_x_goal)
			return (2);

	}
	return (0);

}

uint Solver::linear_conflict_v(int tk_x, int tk_y, int tk_x_goal, int tk_y_goal, Node& n)
{
	int coord, tj_y, tj_x, tj_x_goal, tj_y_goal;
	int size = (int)Node::get_size();

	tj_x = tk_x;
	for (tj_y = tk_y + 1; tj_y < size; ++tj_y)
	{
		coord = tj_x + tj_y * size;
		if (!n.values[coord])
			continue ;
		tj_x_goal = index_of_value[n.values[coord]] % size;
		tj_y_goal = index_of_value[n.values[coord]] / size;
		if (tj_x_goal == tk_x_goal && tj_y_goal < tk_y_goal)
			return (2);

	}
	return (0);

}


uint Solver::mdist(uint x1, uint y1, uint x2, uint y2)
{
	return (std::abs((int)x1 - (int)x2) + std::abs((int)y1 - (int)y2));
}


double Solver::mdist_eucli(uint x1, uint y1, uint x2, uint y2)
{
	return (std::sqrt(std::pow((int)x1 - (int)x2, 2) + std::pow((int)y1 - (int)y2, 2)));
}

uint Solver::heuristic(Node& n)
{
	int x, y, x2, y2;
	int size = Node::get_size();
	int coord;
	uint h = 0;

	for (y = 0; y < (int)Node::get_size(); ++y)
	{
		for (x = 0; x < (int)Node::get_size(); ++x)
		{
			coord = x + y * size;
			if (!n.values[coord])
				continue ;
			x2 = index_of_value[n.values[coord]] % size;
			y2 = index_of_value[n.values[coord]] / size;
			if (this->heuristics == 0 || this->heuristics == 1 || this->heuristics == 2)
				h += mdist(x, y, x2, y2);
			if (this->heuristics == 3 || this->heuristics == 4)
				h += mdist_eucli(x, y, x2, y2);
			if (this->heuristics == 0 || this->heuristics == 2 || this->heuristics == 4)
			{
				if (y2 == y)
					h += linear_conflict_h(x, y, x2, y2, n);
				if (x2 == x)
					h += linear_conflict_v(x, y, x2, y2, n);
			}
		}
	}
	return (h);
}

Node Solver::get_best_from_open()
{
	Node out = this->open_list.top();
	this->open_list.pop();
	return (out);
}

void Solver::fill_index(void)
{
	index_of_value = std::vector<uchar>();
	index_of_value.resize(Node::get_size() * Node::get_size());
	for (uint i = 0; i < final.values.size(); ++i)
		index_of_value[final.values[i]] = i;
}

bool Solver::is_solvable(Node& n)
{
	int		total = 0;
	int		tmp_h = this->heuristics;
	int		manhattan;

	this->heuristics = 1;
	manhattan = heuristic(n);
	for (int j = 0; j < Node::get_size() * Node::get_size(); ++j)
	{
		auto it = std::find(n.values.begin(), n.values.end(), final.values[j]);
		auto index = std::distance(n.values.begin(), it);
		for (int i = j + 1; i < Node::get_size() * Node::get_size(); ++i)
		{
			it = std::find(n.values.begin(), n.values.end(), final.values[i]);
			auto index2 = std::distance(n.values.begin(), it);
			if (index > index2)
				++total;
		}
	}

	this->heuristics = tmp_h;
	return (total % 2 == manhattan % 2);
}

void Solver::solveA(Node& start)
{
	Node current;
	Node *neighbor;
	Node *neighbor_from_closed;
	int success = 0;
	std::vector<Node> neighbours;
	Node *closed;

	open_list = std::priority_queue<Node>();
	closed_list = std::unordered_multimap<uint, Node> ();

	start.g_cost = 0;
	start.f_cost = heuristic(start);
	open_list.push(start);
	if (this->verbose)
		std::cout << "A*: "  << "Estimated moves: " << start.f_cost << "\n";
	while (!success)
	{
		if (!open_list.empty())
			current = get_best_from_open();
		else
			break;
		if (valid(current))
		{
			success = 1;
			break;
		}
		else
		{
			closed = &(closed_list.insert(std::unordered_multimap<uint, Node>::value_type(current._hash, current)))->second;
			neighbours = get_neighbours(current);
			for (uint i = 0; i < neighbours.size(); ++i)
			{
				neighbor = &neighbours[i];
				neighbor_from_closed = is_in_closed(*neighbor);
				if (!neighbor_from_closed)
				{
					neighbor->g_cost = current.g_cost + 1;
					switch (this->greedyness)
					{
						case 0:
							neighbor->f_cost = neighbor->g_cost;
							break;
						case 2:
							neighbor->f_cost = heuristic(*neighbor);
							break;
						default:	// case 1:
							neighbor->f_cost = neighbor->g_cost + heuristic(*neighbor);
							break;
					}
					neighbor->pred = closed;
					this->open_list.push(*neighbor);
				}
			}
		}
	}
	if (this->verbose)
	{
		std::cout << "A*: " << Node::nodes_currently_open_max << " complexity in size\n";
		std::cout << "A*: " << Node::nodes_opened << " complexity in time\n";
		std::cout << "A*: "  << open_list.size() << " nodes in open list\n";
		std::cout << "A*: "  << closed_list.size() << " nodes in closed list\n";
	}
	if (success)
	{
		this->solution = std::vector<Node>();
		Node* tmp = &current;
		while (tmp)
		{
			this->solution.push_back(*tmp);
			tmp = tmp->pred;
		}
		construct_solution();
	}
	else
	{
		std::cout << "A*: "  << "No solution found :/\n";
	}
}

void Solver::solveIDA(Node& start)
{
	uint bound;
	uint t;
	this->solution = std::vector<Node>();

	bound = heuristic(start);
	if (this->verbose)
		std::cout << "IDA*: Esitmated : " << bound << std::endl;
	while (1)
	{
		this->solution.clear();
		this->solution.reserve(bound);
		if (this->verbose)
			std::cout << "IDA*: searching at depth " << bound << std::endl;
		t = search(start, 0, bound);
		if (this->found)
			break ;
		if (t == INT_MAX)
			break;
		bound = t;
	}
	if (this->verbose)
	{
		std::cout << "IDA*: " << Node::nodes_currently_open_max << " complexity in size\n";
		std::cout << "IDA*: " << Node::nodes_opened << " complexity in time\n";
	}
	if (this->found)
	{
		this->solution = std::vector<Node>(this->solution.rbegin(), this->solution.rend());
		construct_solution();
	}
}

int Solver::search(Node n, uint g, uint bound)
{
	uint f = g + heuristic(n);
	uint min;
	uint t;
	std::vector<Node> neighbours;

	if (f > bound)
		return (f);
	this->solution.push_back(n);
	if (valid(n))
	{
		this->found = true;
		return (0);
	}
	min = UINT_MAX;
	neighbours = get_neighbours(n);
	for (auto it = neighbours.begin(); it != neighbours.end(); ++it)
	{
		t = search(*it, g + 1, bound);
		if (this->found)
			return (0);
		if (t < min)
			min = t;
	}
	this->solution.pop_back();
	return (min);
}

void Solver::construct_solution()
{
	std::cout << this->solution.size() - 1 << " moves !\n";

	if (this->step_by_step)
	{
		char res;

		for (auto rit = this->solution.rbegin(); rit != this->solution.rend(); ++rit)
		{
			if (this->step_by_step_key)
				std::cin.get();
			rit->print(true);
		}
		if (this->step_by_step_key)
			std::cin.get();
	}
}
