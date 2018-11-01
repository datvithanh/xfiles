#include <stdio.h>

typedef struct LineDequeueNode {
	struct LineDequeueNode* next;
	struct LineDequeueNode* prev;
	char* line;
} LineDequeueNode;

typedef struct LineDequeue {
	LineDequeueNode* first;
	LineDequeueNode* last;
	int size;
} LineDequeue;

LineDequeue* new_queue() {
	LineDequeue* q = (LineDequeue*) malloc (sizeof(LineDequeue));
	q->size = 0; 
	q->first = 0;
	q->last = 0;
	return q;
}

void enq(LineDequeue* queue, char* line) {
	LineDequeueNode* newNode = (LineDequeueNode*) malloc (sizeof(LineDequeueNode));
	newNode->next = queue->first;
	newNode->prev = 0;
	if (queue->first) {
		queue->first->prev = newNode;
	}
	queue->first = newNode;
	if (!queue->last) {
		queue->last = newNode;
	}
	queue->size++;
}

char* deq(LineDequeue* queue) {
	LineDequeueNode* last = queue->last;
	char* line = last->line;
	if (last->prev) {
		last->prev->next = 0;
	}
	queue->last = last->prev;
	if (queue->first == last) {
		queue->first = last->prev;
	}
	free(last);
	queue->size--;

	return line;
}

void delete_top(LineDequeue* queue) {
	
}
