#include <stdio.h>

typedef struct LineStackNode {
	struct LineStackNode* next;
	char* line;
} LineStackNode;

typedef struct LineStack {
	LineStackNode* top;
} LineStack;

LineStack* new_stack() {
	LineStack* stack = (LineStack*) malloc(sizeof(LineStack));
	stack->top = 0;
	return stack;
}

void push(LineStack *stack, char* line) {
	LineStackNode* newNode = (LineStackNode*) malloc(sizeof(LineStackNode));
	newNode->line = line;
	newNode->next = stack->top;
	stack->top = newNode;
}

int is_empty(LineStack* stack) {
	return !stack->top;
}

char* pop(LineStack* stack) {
	if (is_empty(stack)) {
		return 0;
	}

	LineStackNode* top = stack->top;
	char* line = top->line;
	stack->top = top->next;
	free(top);
	return line;
}
