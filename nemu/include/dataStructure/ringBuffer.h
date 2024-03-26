#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char** buffer;
    int size; // the nums of blocks
    int front; // point to the front block;
    int rear; // point to the next block after the last block
    int count; // the count is the blocks which using in the buffer
} RingBuffer;

RingBuffer* createRingBuffer(int size) {
    RingBuffer* rb = (RingBuffer*)malloc(sizeof(RingBuffer));
    rb->buffer = (char**)malloc(size * sizeof(char*)); 
    for (int i = 0; i < size; i++) {
        rb->buffer[i] = (char*)malloc(128 * sizeof(char)); 
    }
    rb->size = size;
    rb->front = 0;
    rb->rear = 0;
    rb->count = 0;
    return rb;
}

void destroyRingBuffer(RingBuffer* rb) {
    for (int i = 0; i < rb->size; i++) {
        free(rb->buffer[i]);
    }
    free(rb->buffer); 
    free(rb);
}

bool isRingBufferEmpty(RingBuffer* rb) {
    return rb->count == 0;
}

bool isRingBufferFull(RingBuffer* rb) {
    return rb->count == rb->size;
}

void enqueue(RingBuffer* rb, char* data) {
    if(isRingBufferFull(rb)) {
        memset(rb->buffer[rb->front], 0, 128);
        rb->front = (rb->front + 1) % rb->size;
        rb->count--;
    }
    memcpy(rb->buffer[rb->rear], data, 128);
    rb->rear = (rb->rear + 1) % rb->size;
    rb->count ++;
}

void printRingBuffer(RingBuffer* rb) {
    for(int i = 0, ptr = rb->front; i < rb->count; i ++) {
        printf("%s\n", rb->buffer[ptr]);
        ptr = (ptr + 1) % rb->size;
    }
}