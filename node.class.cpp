// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   node.class.cpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: ntibi <marvin@42.fr>                       +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2017/02/02 16:23:50 by ntibi             #+#    #+#             //
/*   Updated: 2017/02/18 18:57:03 by kperreau         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#include "node.class.hpp"
#include <iostream>
#include <iomanip>

uint Node::size = 0;

void Node::set_size(uint size) { if (size <= 0 || size > 10 ) put_error("Invalid puzzle size {1..10}"); Node::size = size; }
uint Node::get_size(void) { return Node::size; }

void Node::on_new_node(void)
{
	++Node::nodes_currently_open;
	++Node::nodes_opened;
	if (Node::nodes_currently_open > Node::nodes_currently_open_max)
		 Node::nodes_currently_open_max = Node::nodes_currently_open;
}

Node::Node(void) : values(Node::size * Node::size, 0), g_cost(0), f_cost(0), pred(nullptr), lastmov(NONE) { on_new_node(); }
Node::Node(std::vector<uchar> values) : values(values), g_cost(0), f_cost(0), pred(nullptr), lastmov(NONE) { hash(); get_pos(); on_new_node(); }

Node::Node(const Node& n) { *this = n; on_new_node(); }

Node& Node::operator=(const Node& n)
{
	this->values = n.values;
	this->g_cost = n.g_cost;
	this->f_cost = n.f_cost;
	this->pos = n.pos;
	this->lastmov = n.lastmov;
	this->pred = n.pred;
	this->_hash = n._hash;
	return (*this);
}

Node::~Node(void) { --Node::nodes_currently_open; }

void Node::print(bool verbose) const
{
	std::string line(Node::size * 5 - 1, '-');

	std::cout << " " << line << std::endl;
	for (uint i = 0; i < this->values.size(); ++i)
	{
		if (g_colors)
		{
			std::string color = (this->values[i]) ? C_GREEN : C_YELL;
			std::cout << "|" << std::string(3 - std::to_string(this->values[i]).size(), ' ') << color << (this->values[i]) << C_DEF << " ";
		}
		else
		{
			std::cout << "|" << std::string(3 - std::to_string(this->values[i]).size(), ' ') << (this->values[i]) << " ";
		}
		if (!((i + 1) % this->size))
			std::cout << "|\n " << line << std::endl;
	}
}

uchar & Node::operator[](int i) { return (this->values[i]); }

uchar Node::get_value(uint y, uint x) { return (this->values[y * Node::size + x]); }
void Node::set_value(uint y, uint x, uchar v) { this->values[y * Node::size + x] = v; this->get_pos(); this->hash(); }

bool Node::is_valid(void)
{
	std::vector<uchar> verif;

	for(auto it = this->values.begin(); it != this->values.end(); ++it)
	{
		if (std::find(verif.begin(), verif.end(), *it) != verif.end())
			return false;
		verif.push_back(*it);
	}
	std::sort(verif.begin(), verif.end());
	if (verif[this->values.size() - 1] != this->values.size() - 1)
		return false;
	return true;
}

void Node::get_pos(void)
{
	this->pos = 0;
	while(this->values[this->pos]) ++this->pos;
}

int Node::try_move_right(void)
{
	return ((this->pos + 1) % this->size);
}

Node Node::move_right(void)
{
	Node out(*this);

	std::swap(out.values[this->pos], out.values[this->pos + 1]);
	++out.pos;
	out.hash();
	out.lastmov = RIGHT;
	// out.pred = this;
	return (out);
}

int Node::try_move_left(void)
{
	return (this->pos % this->size);
}

Node Node::move_left(void)
{
	Node out(*this);

	std::swap(out.values[this->pos], out.values[this->pos - 1]);
	--out.pos;
	out.hash();
	out.lastmov = LEFT;
	// out.pred = this;
	return (out);
}

int Node::try_move_up(void)
{
	return ((this->pos) >= this->size);
}

Node Node::move_up(void)
{
	Node out(*this);

	std::swap(out.values[this->pos], out.values[this->pos - this->size]);
	out.pos -= this->size;
	out.hash();
	out.lastmov = UP;
	// out.pred = this;
	return (out);
}

int Node::try_move_down(void)
{
	return ((this->pos) < this->size * (this->size - 1));
}

Node Node::move_down(void)
{
	Node out(*this);

	std::swap(out.values[this->pos], out.values[this->pos + this->size]);
	out.pos += this->size;
	out.hash();
	out.lastmov = DOWN;
	// out.pred = this;
	return (out);
}

void Node::move_right_inplace(void)
{
	std::swap(this->values[this->pos], this->values[this->pos + 1]);
	++this->pos;
}

void Node::move_left_inplace(void)
{
	std::swap(this->values[this->pos], this->values[this->pos - 1]);
	--this->pos;
}

void Node::move_up_inplace(void)
{
	std::swap(this->values[this->pos], this->values[this->pos - this->size]);
	this->pos -= this->size;
}

void Node::move_down_inplace(void)
{
	std::swap(this->values[this->pos], this->values[this->pos + this->size]);
	this->pos += this->size;
}

void Node::final_state(void)
{
	int d = 0;
	uint v = 1;
	uint i, j;

	i = 0;
	j = 0;
	while (v < Node::get_size() * Node::get_size())
	{
		if (d == 0 && (i == Node::get_size() - 1 || this->get_value(j, i + 1))) d = 1;
		if (d == 1 && (j == Node::get_size() - 1 || this->get_value(j + 1, i))) d = 2;
		if (d == 2 && (i == 0 || this->get_value(j, i - 1))) d = 3;
		if (d == 3 && (j == 0 || this->get_value(j - 1, i))) d = 0;
		this->set_value(j, i, v++);
		if (d == 0) i++;
		if (d == 1) j++;
		if (d == 2) i--;
		if (d == 3) j--;
	}
	get_pos();
	hash();
}

void Node::randomize(uint i)
{
	int r;

	while (i--)
	{
		r = std::rand() % 4;
		if (r == 0)
		{
			if (try_move_right())
				move_right_inplace();
		}
		if (r == 1)
		{
			if (try_move_left())
				move_left_inplace();
		}
		if (r == 2)
		{
			if (try_move_up())
				move_up_inplace();
		}
		if (r == 3)
		{
			if (try_move_down())
				move_down_inplace();
		}
	}
	get_pos();
	hash();
}

uint Node::hash(void)
{
	this->_hash = 0;
	for (uint i = 0; i < values.size(); ++i)
	{
		this->_hash += values[i];
		this->_hash += this->_hash << 10;
		this->_hash ^= this->_hash >> 6;
	}
	return (this->_hash);
}

uint Node::nodes_currently_open = 0;
uint Node::nodes_currently_open_max = 0;
uint Node::nodes_opened = 0;

bool Node::operator>(const Node & n) const { return (this->f_cost < n.f_cost); }
bool Node::operator<(const Node & n) const { return (this->f_cost > n.f_cost); }

bool Node::operator==(const Node & n) const { return (this->_hash == n._hash && this->values == n.values); }
bool Node::operator!=(const Node & n) const { return (this->_hash != n._hash && this->values != n.values); }
