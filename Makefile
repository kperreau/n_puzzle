NAME = npuzzle

SRCDIR = ./

CFILES = 	node.class.cpp\
			solver.cpp

SRC = $(patsubst %, $(SRCDIR)%, $(CFILES))

OBJ = $(SRC:.cpp=.o)

INCLUDEDIR = .

HEADERS = puzzle.class.hpp


# COMPILATION
ERRORFLAGS = # -Wall -Werror -Wextra

INCLUDEFLAGS = -I $(INCLUDEDIR)

LIBFLAGS = 

FLAGS = $(ERRORFLAGS) $(INCLUDEFLAGS) $(LIBFLAGS) -std=c++11 -O2

CC = c++

# Colors vars
DEF_C = '\033[0m'
OK_C = '\033[32m'
ERR_C = '\033[31m'
WRN_C = '\033[33m'
INF_C = '\033[35m'

OK = $(OK_C)[OK]$(DEF_C)
ERR = $(ERR_C)[Error]$(DEF_C)
WAR = $(WRN_C)[Warning]$(DEF_C)
INF = $(INF_C)[Info]$(DEF_C)

# RULES

all: $(NAME)

$(NAME): $(OBJ) main.cpp
		$(CC) $(FLAGS) main.cpp $(OBJ) -o $(NAME)

re: fclean all

%.o: %.cpp
		$(CC) $(FLAGS) -o $@ -c $<

clean:
		rm -f $(OBJ)

fclean: clean
		rm -f $(NAME)
