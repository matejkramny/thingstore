/*
 
 Redis-like 'thing' store
 Description: An experiment I did to revive my knowledge of C. Inspired by redis
 
 By: Matej Kramny
 Date: 24/9/2014
 
 License: WTFPL (wftpl.net)
 
 How to: `make thingstore && ./thingstore`
 
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define kBufSize 5

struct Memory {
    char **mem;
    long size;
    long maxSize;
};

void parse_command (struct Memory *memory, char *cmd);

void checkMemSize (struct Memory *memory) {
    // Check if memory store over limit
    if (memory->size > memory->maxSize -1) {
        // Reallocate some memory
        memory->maxSize += kBufSize;
        
        char **mem = (char **) realloc(memory->mem, sizeof(char *) * memory->maxSize);
        assert(mem != NULL);
        
        memory->mem = mem;
    }
}

void repl (struct Memory *memory) {
    while (1) {
        // Initial command size
        int maxSize = kBufSize;
        // Allocate command
        char *cmd = (char *) malloc(maxSize * sizeof(char));
        
        // Prompt
        printf("> ");
        
        int i = 0;
        while (1) {
            // Block until gets a character from STDIN
            int c = getchar();
            
            // EOF || newline signifies end of input
            if (c == EOF || c == '\n') {
                cmd[i] = 0;
                break;
            }
            
            // Store character
            cmd[i] = c;
            
            // Check if we're over limit
            if (i == maxSize - 1) {
                // Expand max size of command
                maxSize += kBufSize * sizeof(char);
                
                // Reallocate memory
                cmd = (char *) realloc(cmd, maxSize * sizeof(char));
            }
            
            i++;
        }
        
        // Parse the command
        parse_command(memory, cmd);
        
        // Free memory
        free(cmd);
    }
}

void parse_command (struct Memory *memory, char *cmd) {
    long size = strlen(cmd);
    
    // List command
    if (size >= 1 && cmd[0] == 'l') {
        printf(" %ld things stored\n", memory->size);
        
        // Get list
        for (long i = 0; i < memory->size; i++) {
            printf(" [%ld/%ld]: %s\n", i, memory->size, memory->mem[i]);
        }
        
        return;
    }
    
    // Store-a-thing command
    if (size > 1 && cmd[0] == 's') {
        // Store something
        
        checkMemSize(memory);
        
        // Allocate new memory for string
        memory->mem[memory->size] = malloc(sizeof(char) * strlen(cmd));
        
        // Copy string to its space
        strcpy(memory->mem[memory->size], cmd);
        
        memory->size++;
        
        printf("+'%s'. %ld things now\n", cmd, memory->size);
        
        return;
    }
    
    // Some kind of help
    printf("\nFuq. Dunno? 'l' and 's x' work.\n");
}

int main (int argc, char *argv[]) {
    // Init storage
    struct Memory *memory = (struct Memory *) malloc(sizeof(struct Memory));
    assert(memory != NULL);
    
    // Init memory store
    memory->size = 0;
    memory->maxSize = kBufSize;
    
    memory->mem = (char **) malloc(kBufSize * sizeof(char *));
    assert(memory->mem != NULL);
    
    repl(memory);
    
    // Free in-memory store
    for (long i = 0; i < memory->size; i++) {
        // Free allocated memory
        if (memory->mem[i] != NULL)
            free(memory->mem[i]);
    }
    
    free(memory->mem);
    free(memory);
}
