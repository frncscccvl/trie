/*
@  Trie.c
@  COP3502C
@
@  Created by September Caccavale on 7/11/13.
@  Copyright (c) 2013 September Caccavale. All rights reserved.
*/

#define STRINGSIZE 1024
#define WordPerSent 21

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "Trie.h"


TrieNode *createTrieNode(TrieNode *newLeaf)
{
    /* (Dynamically allocate space for a new TrieNode struct. Initialize all the struct members appropriately.) */
    if (newLeaf == NULL) {
        
        TrieNode *newLeaf = malloc(sizeof(TrieNode));
        
        int i = 0;
        newLeaf->count = 0;
        
        for (i = 0; i < 26; i++)
            newLeaf->children[i] = NULL;
        
        newLeaf->subtrie = NULL;
        
        return newLeaf;
    }
}

void *convertToLower(char a[]) {
    /* (takes a string array and loops the characters to transform each into a lowercase letter.) */
    int i = 0;
    
    for(i = 0; a[i] != '\0'; i++)
        a[i] = (char)tolower(a[i]);
}

TrieNode *insertString(TrieNode *root, char *str)
{
    /* (Inserts the string str into the trie. Since it has no return value, it assumes the root already exists (i.e., root is not NULL). If str is already represented in the trie, simply increment its count member.) */
    
    if (str[0] == '\0')
        root->count = root->count+1;
    else {
        char m = str[0];
        str++;
        int index = m - 'a';
        if (root->children[index] == NULL) {
            root->children[index] = createTrieNode(root->children[index]);
        }
        /* (recursive act.) */
        root->children[index] = insertString(root->children[index], str);
    }
    
    return root;
}

TrieNode *getNode(TrieNode *root, char *str)
{
    /* (Searches the trie for the specified string, str.) */
    
    if (root == NULL)
        return NULL;
    
    else if (str[0] == '\0' && root->count > 0)
        return root;
    
    else if (str[0] == '\0' && root->count == 0)
        return NULL;
    
    else {
        char n = str[0];
        str++;
        int index = n - 'a';
        if (root->children[index] == NULL) {
            return NULL;
        }
        /* (recursive act.) */
        return getNode(root->children[index], str);
    }
}

void *auxiliarySubTrieFormatting(TrieNode *trie, char *SUBTRIE[], int wordAmount)
{
    /* (auxiliary subtrie function meant specifically for creating subtries.)*/
    
    int k = 0, j = 0;
    TrieNode *n = NULL;
    
    for (k = 0; k < wordAmount - 1; k++) {
        n = getNode(trie, SUBTRIE[k]);
        n->subtrie = createTrieNode(n->subtrie);
        /* (check to see if a subtrie exists, if not just set the subtrie equal to NULL and return.) */
        if (*SUBTRIE[1] == '.') {
            n->subtrie = NULL;
            return;
        }
        
        for (j = 0; j < wordAmount - 1; j++) {
            /* (disregard duplicate word inside subtrie with next two lines below.) */
            if (strcmp(SUBTRIE[k], SUBTRIE[j]) == 0)
                continue;
            
            n->subtrie = insertString(n->subtrie, SUBTRIE[j]);
        }
    }
}

TrieNode *buildTrie(char *filename)
{
    /* (filename is the name of a corpus text file to process. Open the file and create a
     trie (including all its appropriate subtries) as described above.) */
    
    int i = 0;
    char fileContent[STRINGSIZE];
    
    TrieNode *trie = NULL;
    trie = createTrieNode(trie);
    
    char **SUBTRIE = calloc(WordPerSent, sizeof(char*));
    for (i = 0; i < WordPerSent; i++)
        SUBTRIE[i] = calloc(STRINGSIZE, sizeof(char));
    
    FILE *fileCORPUS = fopen(filename, "rb");
    if (fileCORPUS == 0)
        return NULL;
    
    i = 0;
    while (fscanf(fileCORPUS, " %s", fileContent) != EOF) {
        
        convertToLower(fileContent);
        
        strcpy(SUBTRIE[i], fileContent);
        /* (increment 'i' to count number of words in one sentence) */
        i++;
        
        if (*fileContent == '.') {
            /* (build subtries with line below.) */
            auxiliarySubTrieFormatting(trie, SUBTRIE, i);
            /* (reset 'i' for next sentence word count.) */
            i = 0;
            continue;
        }
        
        trie = insertString(trie, fileContent);
    }
    /* (free malloc-ed two-dimensional char array to prevent memory leak.) */
    for (i = 0; i < WordPerSent; i++)
        free(SUBTRIE[i]);
    free(SUBTRIE);
    
    return trie;
}

void printTrieHelper(TrieNode *root, char *buffer, int k)
{
	int i;
    
	if (root == NULL)
		return;
    
	if (root->count > 0)
		printf("%s (%d)\n", buffer, root->count);
    
	buffer[k + 1] = '\0';
    
	for (i = 0; i < 26; i++)
	{
		buffer[k] = 'a' + i;
        
		printTrieHelper(root->children[i], buffer, k + 1);
	}
    
	buffer[k] = '\0';
}

void printTrie(TrieNode *root, int useSubtrieFormatting)
{
	char buffer[1026];
    
	if (useSubtrieFormatting)
	{
		strcpy(buffer, "- ");
		printTrieHelper(root, buffer, 2);
	}
	else
	{
		strcpy(buffer, "");
		printTrieHelper(root, buffer, 0);
	}
}


int main(int argc, char **argv)
{
    char fileContent[STRINGSIZE], functionContent[STRINGSIZE];
    FILE *fileINPUT = fopen(argv[2], "rb");
    
    if (fileINPUT == 0)
        return 0;
    
    TrieNode *trie = NULL;
    trie = createTrieNode(trie);
    
    TrieNode *endNode = NULL;
    /* (build main trie/subtries with line below.) */
    trie = buildTrie(argv[1]);
    
    while (fscanf(fileINPUT, " %s", fileContent) != EOF) {
        
        strcpy(functionContent, fileContent);
        convertToLower(fileContent);
        
        if (*fileContent == '!') {
            printTrie(trie, 0);
        }
        else if (getNode(trie, fileContent) != NULL) {
            printf("%s\n", functionContent);
            endNode = getNode(trie, fileContent);
            if (endNode->subtrie == NULL)
                printf("(EMPTY)\n");
            else
                printTrie(endNode->subtrie, 1);
        }
        else if (getNode(trie, fileContent) == NULL) {
            printf("%s\n", functionContent);
            printf("(INVALID STRING)\n");
        }
    }
    
    return 0;
}
