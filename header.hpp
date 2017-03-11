// ************************************************************************** //
//                                                                            //
//                                                        :::      ::::::::   //
//   header.hpp                                         :+:      :+:    :+:   //
//                                                    +:+ +:+         +:+     //
//   By: Nico <marvin@42.fr>                        +#+  +:+       +#+        //
//                                                +#+#+#+#+#+   +#+           //
//   Created: 2015/08/13 18:08:29 by Nico              #+#    #+#             //
//   Updated: 2017/02/16 18:21:12 by ntibi            ###   ########.fr       //
//                                                                            //
// ************************************************************************** //

#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>

typedef unsigned int uint;
// typedef unsigned char uchar;
typedef unsigned int uchar;

enum Movement {None, Up, Down, Left, Right};

void put_error(std::string error);

extern bool g_colors;

#define C_GREEN  "\x1B[32m"
#define C_YELL  "\x1B[33m"
#define C_DEF "\x1B[0m"

#endif /* HEADER_HPP */
