// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   main.cpp                                           :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: ntibi <marvin@42.fr>                       +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2017/02/02 16:24:17 by ntibi             #+#    #+#             //
/*   Updated: 2017/03/03 17:31:22 by kperreau         ###   ########.fr       */
/*   Updated: 2017/03/03 17:23:13 by kperreau         ###   ########.fr       */
//                                                                            //
// ************************************************************************** //

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <sys/time.h>
#include "header.hpp"
#include "node.class.hpp"
#include "solver.hpp"

bool g_colors;

typedef struct s_opts
{
	bool verbose = false;
	uint greedyness = 1;
	bool step_by_step = false;
	bool step_by_step_key = false;
	bool random = false;
	bool ccolors = false;
	int seed;
	char *size = NULL;
	char *fn = NULL;
	int heuristic = 0;
	std::string algo = "A";
} t_opts;

void put_error(std::string error)
{
	std::cerr << "Error: " << error << std::endl;
	exit(1);
}

void usage(char *name)
{
	std::cout << "usage: " << name << " [rsvnf] [file]" << std::endl;
	std::cout << "'-r' to randomize a puzzle" << std::endl;
	std::cout << "'-s seed' to specify a seed for a random puzzle" << std::endl;
	std::cout << "'-c to activate colored ouput" << std::endl;
	std::cout << "'-k to activate step by step key" << std::endl;
	std::cout << "'-v' for a verbose output" << std::endl;
	std::cout << "'-V' for a step by step solution" << std::endl;
	std::cout << "'-n size' to specify a size for a random puzzle" << std::endl;
	std::cout << "'-f filename' to specify the file" << std::endl;
	std::cout << "'-a algorithm' to specify the solving algorithm (IDA, A)" << std::endl;
	std::cout << "'-H n' to specify the heuristic (0 = default, 1 = manhattan, 2 = manhattan + linear conflict, 3 = euclidean, 4 = euclidean + linear conflict)" << std::endl;
	std::cout << "'-g' set greedyness (0 = uniform cost, 1 = A* default, 2 = greedy)" << std::endl;
	std::cout << "'-h' to print this help" << std::endl;
	std::exit(0);
}

unsigned int get_ms(void)
{
	struct timeval tp;

	gettimeofday(&tp, NULL);
	return (tp.tv_usec);
}

t_opts options(int ac, char **av)
{
	static char default_stdin[] = "-";
	t_opts opts;
	int ch;

	while ((ch = getopt(ac, av, "rs:vn:f:ha:VH:g:ck")) != -1)
	{
		switch (ch)
		{
			case 'h':
				usage(av[0]);
			case 's':
				opts.random = true;
				opts.seed = atoi(optarg);
				break;
			case 'g':
				opts.greedyness = atoi(optarg);
				break;
			case 'r':
				opts.random = true;
				break;
			case 'n':
				opts.random = true;
				opts.size = optarg;
				break;
			case 'v':
				opts.verbose = true;
				break;
			case 'c':
				opts.ccolors = true;
				break;
			case 'V':
				opts.verbose = true;
				opts.step_by_step = true;
				break;
			case 'k':
				opts.verbose = true;
				opts.step_by_step = true;
				opts.step_by_step_key = true;
				break;
			case 'a':
				opts.algo = optarg;
				break;
			case 'H':
				opts.heuristic = atoi(optarg);
				break;
			case 'f':
				opts.fn = optarg;
				break;
			case '?':
			default:
				usage(av[0]);
				break;
		}
	}
	ac -= optind;
	av += optind;
	if (*av)
		opts.fn = *av;
	else if (!opts.size)
		opts.fn = default_stdin;
	if (!opts.fn && !opts.size)
		put_error("No puzzle");
	if (!opts.fn && opts.random && !opts.size)
		put_error("No size specified for the random generation");
	if (opts.algo != "A" && opts.algo != "IDA")
		put_error("Invalid solving algorithm");
	if (!(opts.greedyness <= 2))
		put_error("Invalid greedyness (0-2)");
	if (!(opts.heuristic >= 0 && opts.heuristic <= 4))
		put_error("Invalid heuristic (0-4)");
	if (!opts.fn && atoi(opts.size) <= 0)
		put_error("Invalid puzzle size");
	return (opts);
}

char _stdin[] = "/dev/fd/0";

void gen_puzzle_from_file(t_opts& opts, Node& start, Node& end)
{
	std::ifstream f;
	if (std::string(opts.fn) == "-")
		opts.fn = _stdin;
	try { f.open(opts.fn); }
	catch (std::exception& e) { put_error("No such file"); }
	if (errno)
		put_error("No such file");
	std::string line;

	bool size_set = false;
	uint j = 0;
	while (std::getline(f, line))
	{
		line = line.substr(0, line.find("#"));
		if (!line.size())
			continue;
		if (!size_set)
		{
			uint s;
			remove_if(line.begin(), line.end(), isspace);
			try { s = std::stoi(line); }
			catch (std::invalid_argument& e) { put_error("No size"); }
			catch (std::out_of_range& e) { put_error("Size ouf of range"); }
			Node::set_size(s);
			start = Node();
			end = Node();
			size_set = true;
		}
		else
		{
			uint i = 0;
			std::stringstream ss(line);
			while (ss.rdbuf()->in_avail())
			{
				uchar v;

				ss >> v;
				if (ss.fail())
					break;
				if (i >= Node::get_size() || j >= Node::get_size())
					put_error("Invalid puzzle");
				start.set_value(j, i, v);
				++i;
			}
			++j;
		}
	}
	if (!size_set)
		put_error("Invalid puzzle");
	if (!start.is_valid())
		put_error("Invalid puzzle");
	end.final_state();
}

void gen_puzzle(t_opts& opts, Node& start, Node& end)
{
	unsigned int size;

	size = atoi(opts.size);
	Node::set_size(size);

	start = Node();
	end = Node();
	end.final_state();
	start.final_state();
	start.randomize(10000);	
}

int main(int ac, char **av)
{
	Solver solver;
	t_opts opts;
	unsigned int seed;

	opts = options(ac, av);
	if (opts.random)
	{
		if (opts.seed)
			seed = opts.seed;
		else
			seed = get_ms();
		std::cout << "seed: " << seed << std::endl;
		std::srand(seed);
	}

	g_colors = opts.ccolors;
	solver.step_by_step = opts.step_by_step;
	solver.step_by_step_key = opts.step_by_step_key;
	solver.verbose = opts.verbose;
	solver.heuristics = opts.heuristic;
	solver.greedyness = opts.greedyness;

	Node end;
	Node start;

	if (opts.fn)
		gen_puzzle_from_file(opts, start, end);
	else
		gen_puzzle(opts, start, end);

	solver.set_final(end);

	start.print();

	if (!solver.is_solvable(start))
	{
		std::cout << "Not Solvable." << std::endl;
		return (0);
	}	

	if (opts.algo == "IDA")
		solver.solveIDA(start);
	if (opts.algo == "A")
		solver.solveA(start);
	return (0);
}
