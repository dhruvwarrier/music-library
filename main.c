//
// This is a program written to maintain a personal music library, 
// using a linked list to hold the songs in the library.
//
// Author: Dhruv Warrier
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

// A node in the linked list

// Each string in the node is declared as a character pointer variable,
// so they need to be dynamically allocated using the malloc() function,
// and deallocated using the free() function after use.

typedef struct node {
    char *artist ;
    char *songName ;
    char *genre ;
    struct node *nextNode ;
} Node;

// Declarations of linked list functions
//
void insertSong(char[], char[], char[], Node**);
void searchSong(Node*, char[]);
void deleteSong(Node**, char[]);
Node* rawInsertSong(char[], char[], char[], Node*); // inserts song/node after previous song/node and returns address of created node
Node* rawSearchSong(Node*, char[], Node**); // returns reference to song if found, else returns null
bool rawDeleteSong(Node**, char[]); // searches and deletes a node, returns true if successful
void freeNode(Node*); // frees the memory for a node
void printList(Node*);
bool songExists(Node*, char[]); // returns true if song found
void printNodeContents(Node*);

// Declarations of support functions

void inputStringFromUser( char prompt[], char s[], int arraySize ) ;
void songNameDuplicate( char songName[] ) ;
void songNameFound( char songName[] ) ;
void songNameNotFound( char songName[] ) ;
void songNameDeleted( char songName[] ) ;
void printMusicLibraryEmpty( void ) ;
void printMusicLibraryTitle( void ) ;

const int MAX_LENGTH = 1024;

int main( void ) {
    // Declare the head of the linked list.
	Node *headOfList = NULL;
	char *promptName = "Song name";
	char *promptArtist = "Artist";
	char *promptGenre = "Genre";
	
    // Announce the start of the program
    printf( "Personal Music Library.\n\n" ) ;
    printf( "%s", "Commands are I (insert), D (delete), S (search by song name),\n"
         "P (print), Q (quit).\n" ) ;
    
    char response ;
    char input[MAX_LENGTH + 1];
	char name[MAX_LENGTH + 1], artist[MAX_LENGTH + 1], genre[MAX_LENGTH + 1];
	
    do {
		inputStringFromUser( "\nCommand", input, MAX_LENGTH ) ;

        // Response is the first character entered by user.
        // Convert to uppercase to simplify later comparisons.
        response = toupper( input[0] ) ;

        if( response == 'I' ) {
            // Insert a song into the linked list.
            // Maintain the list in alphabetical order by song name.
			inputStringFromUser(promptName, name, MAX_LENGTH);
			inputStringFromUser(promptArtist, artist, MAX_LENGTH); // receives and inserts song in alphabetical order
			inputStringFromUser(promptGenre, genre, MAX_LENGTH);
			insertSong(name, artist, genre, &headOfList);
        }
        else if( response == 'D' ) {
            // Delete a song from the list.
            char *prompt = "\nEnter the name of the song to be deleted" ;
			inputStringFromUser(prompt, input, MAX_LENGTH); // receives and deletes song given by song name if found
			deleteSong(&headOfList, input);
        }
        else if( response == 'S' ) {
            // Search for a song by its name.
            char *prompt = "\nEnter the name of the song to search for" ;
			inputStringFromUser(prompt, input, MAX_LENGTH); // searches for song specified and returns song if found
			searchSong(headOfList, input);
        }
        else if( response == 'P' ) {
			printList(headOfList);
        }
        else if( response == 'Q' ) {
            ; // do nothing, we'll catch this below
        }
        else {
            // do this if no command matched ...
            printf( "\nInvalid command.\n" ) ;
        }
    } while( response != 'Q' ) ;
    // Delete the entire linked list.
    if (headOfList!= NULL) { // not an empty list
		Node* previousNode = headOfList;
		Node* nextNode = previousNode->nextNode;
		while (nextNode != NULL) { // iterate till end of linked list
			nextNode = previousNode->nextNode;
			if (nextNode == NULL) break; // prevents previous node from becoming new node
			deleteSong(&headOfList, previousNode->songName); // deletes song
			previousNode = nextNode;
		}
		deleteSong(&headOfList, previousNode->songName); // deletes head if the list only has one song
		headOfList = NULL;
	}
    // Print the linked list to confirm deletion.
    printList(headOfList);

    return 0 ;
}

// Support Function Definitions

// Prompt the user for a string safely, without buffer overflow
void inputStringFromUser(char prompt[], char s[], int maxStrLength) {
    int i = 0;
    char c;
    
    printf( "%s --> ", prompt ) ;
    while( i < maxStrLength && (c = getchar()) != '\n' )
        s[i++] = c ;
    s[i] = '\0' ;
}

// Function to call when the user is trying to insert a song name 
// that is already in the personal music library.
void songNameDuplicate( char songName[] ) {
    printf( "\nA song with the name '%s' is already in the music library.\n"
            "No new song entered.\n", songName);
}

// Function to call when a song name was found in the personal music library.
void songNameFound( char songName[] ) {
    printf( "\nThe song name '%s' was found in the music library.\n",
           songName ) ;
}

// Function to call when a song name was not found in the personal music library.
void songNameNotFound( char songName[] ) {
    printf( "\nThe song name '%s' was not found in the music library.\n",
           songName);
}

// Function to call when a song name that is to be deleted
// was found in the personal music library.
void songNameDeleted (char songName[]) {
    printf( "\nDeleting a song with name '%s' from the music library.\n", songName);
}

// Function to call when printing an empty music library.
void printMusicLibraryEmpty(void) {
    printf( "\nThe music library is empty.\n" ) ;
}

// Function to call to print a title when the entire music library is printed.
void printMusicLibraryTitle(void) {
    printf( "\nMy Personal Music Library: \n" ) ;
}

// Linked list functions

void insertSong(char name[], char artist[], char genre[], Node **headRef) {
	// check for alphabetic order
	Node *head = *headRef;
	if (head == NULL) {
		head = rawInsertSong(name, artist, genre, NULL); // if no head exists, create a head node and point to it
	} else {
		Node *thisNode;
		Node *previousNode = head;
		
		if (strcmp(name, head->songName) < 0) {
			// if the new song comes alphabetically before the head, change the address of head to new song
			thisNode = rawInsertSong(name, artist, genre, NULL); 
			thisNode->nextNode = head;
			head = thisNode;
		} else if (strcmp(name, head->songName) == 0) {
			songNameDuplicate(name); // duplicate song as head node
		} else {
			// the new song comes alphabetically after the head node song
			Node *nextNode = previousNode->nextNode;
			
			while (nextNode != NULL && strcmp(name, nextNode->songName) > 0) { 
				// iterates till end of list or for as long as alphabetically after each new node
				nextNode = previousNode->nextNode;
				if (nextNode == NULL) break; // prevents previousNode from becoming nextNode
				previousNode = nextNode; 
			}
			if (nextNode != NULL && strcmp(nextNode->songName, name) != 0) {
				rawInsertSong(name, artist, genre, previousNode); // if not duplicate and not head node, insert song
			} else if (nextNode == NULL) {
				rawInsertSong(name, artist, genre, previousNode); // insert song if reached end of list
			} else {
				// nextNode is not NULL and is the same as this node
				songNameDuplicate(name);
			}
		}
	}
	*headRef = head; // update original head in main
}

Node* rawInsertSong(char name[], char artist[], char genre[], Node *previousNode) {
	//check if previousNode is NULL
	Node *thisNode = (Node*)malloc(sizeof(Node));
	if (previousNode == NULL) {
		thisNode->songName = (char*)malloc(strlen(name)*sizeof(char));
		strcpy(thisNode->songName, name);
		thisNode->artist = (char*)malloc(strlen(artist)*sizeof(char)); // allocates memory for the node and copies parameters
		strcpy(thisNode->artist, artist);
		thisNode->genre = (char*)malloc(strlen(genre)*sizeof(char));
		strcpy(thisNode->genre, genre);
		thisNode->nextNode = NULL; // if head node, previousNode cannot be re-routed and nextNode is NULL
	} else {
		// not a head node
		Node *nextNode = previousNode->nextNode;
		thisNode->songName = (char*)malloc(strlen(name)*sizeof(char));
		strcpy(thisNode->songName, name);
		thisNode->artist = (char*)malloc(strlen(artist)*sizeof(char)); // allocates memory for the node and copies parameters
		strcpy(thisNode->artist, artist);
		thisNode->genre = (char*)malloc(strlen(genre)*sizeof(char));
		strcpy(thisNode->genre, genre);
		thisNode->nextNode = nextNode; // make this node point to next node
		previousNode->nextNode = thisNode; // make previous node point to this node
	}
	return thisNode;
}

void searchSong(Node* head, char songToBeSearched[]) {
	Node* previousNode = head;
	Node* nextNode = previousNode->nextNode;
	bool found = false;

	while (nextNode != NULL) {
		nextNode = previousNode->nextNode;
		if (nextNode == NULL) break;
		if (strcmp(previousNode->songName, songToBeSearched) == 0) {// previousNode is found
			songNameFound(songToBeSearched);
			printNodeContents(previousNode);
			found = true;
			break;
		} else if (nextNode != NULL && (strcmp(nextNode->songName, songToBeSearched) == 0)) { // nextNode is found and this node is not the end of the list
			songNameFound(songToBeSearched);
			printNodeContents(nextNode);
			found = true;
			break;
		}
		previousNode = nextNode;
	}
	if (previousNode == head && (strcmp(head->songName, songToBeSearched) == 0) && nextNode == NULL) {
		found = true;
		songNameFound(songToBeSearched); // this makes sure head nodes are not skipped in searching
		printNodeContents(head);
	}
	if (!found) {
		songNameNotFound(songToBeSearched);
	}
}

Node* rawSearchSong(Node* head, char songToBeSearched[], Node** songBeforeSearchedSong) {
	Node* previousPreviousNode = NULL;
	Node* previousNode = head;
	Node* nextNode = previousNode->nextNode;

	while (nextNode != NULL) {
		nextNode = previousNode->nextNode;
		if (nextNode == NULL) break;
		if (strcmp(previousNode->songName, songToBeSearched) == 0) { // node before the node found, so that pointer can be re-routed
			*songBeforeSearchedSong = previousPreviousNode;  // previousPreviousNode will not exist if head is found
			return previousNode;
		} else if (nextNode != NULL && (strcmp(nextNode->songName, songToBeSearched) == 0)) {
			*songBeforeSearchedSong = previousNode; // node before the node found, so that pointer can be re-routed
			return nextNode; // nextNode is found and this node is not the end of the list
		}
		previousNode = nextNode;
		previousPreviousNode = previousNode;
	}
	if (previousNode == head && (strcmp(head->songName, songToBeSearched) == 0)  && nextNode == NULL) {
		*songBeforeSearchedSong = NULL;  // this makes sure head nodes are not skipped in searching
		return head;
	}
	*songBeforeSearchedSong = NULL;
	return NULL;
}

void deleteSong(Node** headRef, char songToBeDeleted[]) {
	Node *headOfList = *headRef;
	if (songExists(headOfList, songToBeDeleted)) {
		songNameDeleted(songToBeDeleted);
		rawDeleteSong(&headOfList, songToBeDeleted); // if songs exists delete song from list
	} else {
		songNameNotFound(songToBeDeleted);
	}
	*headRef = headOfList; // update original head in main
}

bool rawDeleteSong(Node** headRef, char songToBeDeleted[]) {
	
	Node *head = *headRef;
	bool found = false;
	Node** previousNodeRef = (Node**)malloc(sizeof(previousNodeRef));
	Node* songToBeDeletedRef = rawSearchSong(head, songToBeDeleted, previousNodeRef);
	Node* previousNode = *previousNodeRef;
	
	if (songToBeDeletedRef != NULL) { // song found
		found = true;
		if (previousNode == NULL) {
			// song to be deleted is head
			Node* newHead = songToBeDeletedRef->nextNode;
			*headRef = newHead;
			freeNode(head);
		} else {
			// re-route pointer of previous node to next node and free this node
			previousNode->nextNode = songToBeDeletedRef->nextNode;
			freeNode(songToBeDeletedRef);
		}
	} else {
		found = false;
	}
	free(previousNodeRef); // frees previousNodeRef whose size was not defined initially
	return found;
}

void freeNode(Node* nodeToBeFreed) {
	free(nodeToBeFreed->songName);
	free(nodeToBeFreed->artist); // frees all the memory for a node
	free(nodeToBeFreed->genre);
	free(nodeToBeFreed);
}

void printList(Node* head) {
	
	if (head == NULL) {
		printMusicLibraryEmpty();
	} else {
		printMusicLibraryTitle();
		Node* previousNode = head;
		Node* nextNode = previousNode->nextNode;
		while (nextNode != NULL) {
			nextNode = previousNode->nextNode;
			if (nextNode == NULL) break;
			printNodeContents(previousNode); // prints each individual element in the list
			previousNode = nextNode;
		}
		printNodeContents(previousNode); // prints last element
	}
}

bool songExists(Node* head, char songToBeSearched[]) {
	Node* previousNode = head;
	Node* nextNode = previousNode->nextNode;
	bool found = false;

	while (nextNode != NULL) {
		nextNode = previousNode->nextNode;
		if (nextNode == NULL) break;
		if (strcmp(previousNode->songName, songToBeSearched) == 0) {
			found = true; // checks previous node for song
			break;
		} else if (nextNode != NULL && (strcmp(nextNode->songName, songToBeSearched) == 0)) {
			found = true; // checks nextNode for song
			break;
		}
		previousNode = nextNode;
	}
	if (previousNode == head && (strcmp(head->songName, songToBeSearched) == 0) && nextNode == NULL) {
		found = true; // checks the head for existence as well
	}
	return found;
}

void printNodeContents(Node* nodeToBePrinted) {
	printf("\n%s\n", nodeToBePrinted->songName);
	printf("%s\n", nodeToBePrinted->artist); // prints the paramaters of a certain song in the list
	printf("%s\n", nodeToBePrinted->genre);
}