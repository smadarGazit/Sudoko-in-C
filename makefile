CC = gcc
OBJS = main.o game.o board.o command_data.o main_aux.o cell.o point.o ilp_solver.o stack.o list.o list_node.o parser.o file_handler.o UI.o commands.o move.o game_flow.o
EXEC = sudoku-console
COMP_FLAG = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -IC:/gurobi811/win64/include
GUROBI_LIB = -LC:/gurobi811/win64/bin -lgurobi81

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm

.PHONY:all
all: $(EXEC)

main.o: main.c game_flow.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
main_aux.o: main_aux.c main_aux.h typedefs.h board.h cell.h stack.h UI.h commands.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
board.o: board.h board.c cell.h bool.h commands.h modes.h errors.h list.h list_node.h command_data.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c 
cell.o: cell.c cell.h point.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c 
point.o: point.c point.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c 
ilp_solver.o: ilp_solver.c point.h main_aux.h 
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
game.o: game.c game.h board.h cell.h point.h commands.h main_aux.h modes.h errors.h command_data.h move.h list_node.h UI.h file_handler.h ilp_solver.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
old_solver.o: old_solver.c bool.h board.h  cell.h point.h 
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
stack.o: stack.c stack.h bool.h board.h cell.h errors.h 
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
list.o: list.c list.h list_node.h command_data.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
list_node.o: list_node.c list_node.h command_data.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
command_data.o: command_data.c command_data.h board.h main_aux.h errors.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
parser.o: parser.c parser.h move.h errors.h 
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
file_handler.o: file_handler.c file_handler.h board.h cell.h errors.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
UI.o: UI.c UI.h errors.h board.h commands.h cell.h move.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
commands.o: commands.c commands.h modes.h bool.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
move.o: move.c move.h errors.h commands.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
game_flow.o: game_flow.c game_flow.h move.h game.h parser.h board.h commands.h main_aux.h modes.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
