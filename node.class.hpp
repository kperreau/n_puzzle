// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   node.class.hpp                                     :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: Nico <marvin@42.fr>                        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2015/08/13 18:03:38 by Nico              #+#    #+#             //
/*   Updated: 2017/02/18 18:57:40 by kperreau         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#ifndef NODE_CLASS_HPP
#define NODE_CLASS_HPP

#include "header.hpp"
#include <vector>
#include <algorithm>
#include <string>

class Node
{
public:
	enum Mov
	{
		NONE = 0,
		UP = 1,
		DOWN = -1,
		LEFT = 2,
		RIGHT = -2
	};
	
	Node(void);
	Node(std::vector<uchar> values);
	Node(const Node& n);
	Node& operator=(const Node& n);
	~Node(void);
	static void set_size(uint size);
	static uint get_size(void);

	void print(bool verbose = false) const;
	uchar& operator[](int i);

	std::vector<uchar> values;
	uchar get_value(uint x, uint y);
	void get_pos(void);
	void set_value(uint x, uint y, uchar v);
	bool is_valid(void);

	int try_move_right(void);
	Node move_right(void);
	int try_move_left(void);
	Node move_left(void);
	int try_move_up(void);
	Node move_up(void);
	int try_move_down(void);
	Node move_down(void);

	void final_state(void);
	void randomize(uint i);

	uint hash(void);
		
	uint g_cost;
	uint f_cost;
	uint _hash;
	uint pos;
	Node* pred;
	Mov lastmov;
		
	bool operator<(const Node & n) const;
	bool operator>(const Node & n) const;
	bool operator==(const Node & n) const;
	bool operator!=(const Node & n) const;

	static uint nodes_currently_open;
	static uint nodes_currently_open_max;
	static uint nodes_opened;

private:
	void move_right_inplace(void); // these don't re-hash
	void move_left_inplace(void);
	void move_up_inplace(void);
	void move_down_inplace(void);

	void on_new_node(void);

	static uint size;
};

#endif /* NODE_CLASS_HPP */
