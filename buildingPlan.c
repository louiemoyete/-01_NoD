








#include <stdio.h>
#include <stdlib.h>

/*************************************************************************************************
            BUILDING PLAN . H 

#include <stdio.h>
#include <stdlib.h>

#include "building.h"
************************************************************************************************/
#define WIDTH  21                 // width of floor plan grids
#define HEIGHT 21                 // height of floor plan grids
#define NUM_FLOORS 5            // Number of fllors in the building

#define DOOR '2'                // values representing doors 
#define WALL '1'                // Value representing walls 
#define EMPTYSPACE '0'          // Values representing empty spaces 
#define FLIP_TILE '3'           // Value to turn EMPTYSPACE values 
#define EMPTY 0                 // Initialise variables as 0


typedef struct RoomType {
  unsigned char     floor;        // Floor number (1 to NUM_FLOORS-1) that this room is on
  unsigned char     size;         // number of grid cells in the room
  unsigned char     numDoors;     // number of doors the room connects to
  struct RoomType  *nextRoom;     // The next room in the list
} Room;

typedef struct DoorType {
  unsigned char  row;             // grid location  that door lies at
  unsigned char  col;             
  Room           *openFromRoom;   // room on one side of the door
  Room           *openToRoom;     // room on other side of the door
} Door;

typedef struct FloorPlanType {
  char           grid[HEIGHT][WIDTH];  
  Door          *doors;           // Dynamically-Allocated array doors on this floor
  unsigned char  numDoors;        // Number of doors found on this floor
  Room          *roomList;        // Linked List of rooms on this floor
} FloorPlan;

typedef struct BuildingType {
  FloorPlan     floors[NUM_FLOORS];  // Individual Floors, statically-allocated
  Room         *offices;             // Linked-List of all offices in the building (includes all floors)
  Room         *meetingRooms;        // Linked-List of all meeting rooms in the building (includes all floors)
  Room         *labs;                // Linked-List of all labs in the building (includes all floors)
  Room         *hallways;            // Linked-List of all hallways in the building (includes all floors)
  Room         *storageRooms;        // Linked-List of all storageRooms in the building (includes all floors)
} Building;

// External functions used my the main program
extern void freeEverything(Building *);
extern void findRooms(Building *, int);
extern void findDoors(FloorPlan *);
extern void sortRooms(Building *);
extern void printRoomList(Room *, char *);

/*************************************************************************************************
            BUILDING PLAN . C



#include "building.h"
************************************************************************************************/

                            /** ** ** *
            Function : 
                findDoors 
            Parametres : 
                Pointer : FloorPlan ( Global Struct ) :  i.e.:
                {
                    char : grid[HEIGHT][WIDTH] : Array mimicking the size of the room
                    Door : *doors ( Global Struct ) : Dynamically-Allocated array of Doors on this particular floor : i.e.:
                    {
                            unsigned char : row :
                                grid row location that the door lies at
                            unsigned char : col : 
                                grid column location that the door lies at
                            Room : *openFromRoom : 
                                room on one side of the door
                            Room : *openToRoom : 
                                room on other side of the door
                    }
                    unsigned char : numDoors : Number of doors found on this floor
                    Room : *roomList : Linked List of rooms on this floor : i.e.:
                    {
                        unsigned char : floor :
                            Floor number (1 to NUM_FLOORS-1) that this room is on
                        unsigned char : size :
                            number of grid cells in the room
                        unsigned char : numDoors : 
                            number of doors the room connects to
                        struct RoomType : *nextRoom : 
                            The next room in the list
                    }
                }
            Return : 
                Void 
            Local Variables : 
                Door * : tempArray : 
                    Dynamically Allocated array, the size of all doors. Initialised with zero ( calloc )
                int : temp_numDoors
                    temporary variable to set row and col for Door inside doors ( second for loop )
            Functionality : 
                Iterates, find all doors. 
                Stores number in numDoors, inside fplan ( parametre ) 
                Creates a new array ( tempArray ), dynamically allocates enough space for all doors ( initialise it with zeroes )
                Equals it to fplan's ( parametre ) doors array 
                Traverses array again : 
                    for each door inside doors, sets value for variable row and col
                            * ** ** **/
void findDoors( FloorPlan *fplan )
{
// Local Variables 
    int temp_numDoors = 0;
    
// Set fplan->numDoor to zero to start 
    fplan->numDoors = 0; 
    
// Traverse and count doors 
    for( int rows = 0; rows < HEIGHT; rows++ )
    {
        for( int columns = 0; columns < WIDTH; columns++ )
        {
            if( fplan->grid[ rows ][ columns ] == DOOR )
            {
                ++fplan->numDoors;
            }
        }
    }

// Create new array, allocate it dynamically 
    Door *tempArray;
    tempArray = calloc( fplan->numDoors, sizeof( Door ) );

// Check to see if everything went well, if so, equal dynamically allocated array 
    if ( tempArray == NULL ){ return; }
    fplan->doors = tempArray;

// Traverse again, fill each door in fplan->doors with the proper row and column 
    for( int rows = 0; rows < HEIGHT; rows++ )
    {
        for( int columns = 0; columns < WIDTH; columns++ )
        {
            if( fplan->grid[ rows ][ columns ] == DOOR )
            {
                fplan->doors[temp_numDoors].row = rows;
                fplan->doors[temp_numDoors].col = columns;
                ++temp_numDoors;
            }
        }
    }
}
                            /** ** ** *
            Function : 
                findRooms 
            Parametres : 
                Pointer : Building *build ( Global Struct ) : i.e.:
                    FloorPlan : floors[NUM_FLOORS] : Individual Floors, statically-allocated : i.e.:
                    {
                        char : grid[HEIGHT][WIDTH] : Array mimicking the size of the room
                        Door : *doors ( Global Struct ) : Dynamically-Allocated array of Doors on this particular floor : i.e.:
                        unsigned char : numDoors : Number of doors found on this floor
                        Room : *roomList : Linked List of rooms on this floor : i.e.:
                    }
                    Room : *offices : Linked-List of all offices in the building (includes all floors) : i.e.:
                    {
                        unsigned char : floor : Floor number (1 to NUM_FLOORS-1) that this room is on
                        unsigned char : size : number of grid cells in the room
                        unsigned char : numDoors : number of doors the room connects to
                        struct RoomType : *nextRoom : The next room in the list
                    }
                    Room : *meetingRooms : Linked-List of all meeting rooms in the building (includes all floors)
                    Room : *labs : Linked-List of all labs in the building (includes all floors)
                    Room : *hallways : Linked-List of all hallways in the building (includes all floors)
                    Room : *storageRooms : Linked-List of all storageRooms in the building (includes all floors)
                int : floorNum  
            Return : 
                Void 
            Local Variables : 
            Functionality : 
                Create an empty Room dummy, we call it Head 
                Iterate Building's floorPlan's Grid for a specific floor ( floorNum ( parametre )) to identify rooms 
                            Iterate grid left ro right, look for a 0 
                Search for OpenSpace ( Global ), dynamically allocate a new room
                Set variables 'size' and 'numDoors' to zero 
                Add said room to the Building's floorPlan's roomList ( linked list )
                Check if it's the first node of the list, 
                    if so: make it the Head 
                    if not: make its value next equal the Head, then make it the new Head. 
                            * ** ** **/
void findRooms(Building *build, int floorNum) 
{
// Create Head 
    Room *head;
    head = calloc( 1, sizeof( Room ));
// Iterate Building's FloorPlan's Grid from left to right 

    for( int rows = EMPTY; rows < HEIGHT; rows++ )
    {
        for( int columns = EMPTY; columns < WIDTH; columns++ )
        {
            if ( build->floors[ floorNum ].grid[ rows ][ columns ] == EMPTYSPACE )
            {
                Room *newRoom;
// If we find a 0 = Empty Space 
                newRoom = calloc( 1, sizeof( Room ));
// Call the Recursive Function in order to get the value for the size of the room
                traverseRecursively( &build, newRoom, floorNum, rows, columns ); 
// Add it to the list by checking if the head is null 
                if( !head ) 
                {
                    newRoom->nextRoom = NULL; 
                    head = newRoom; 
                }
// If the head is not empty, connect the new room to the previous one 
                else
                {
                    newRoom->nextRoom = head; 
                    head = newRoom;
                }
            }
        }
    }
                                /**** delete ****/
                                    printf("\n");
                                    while( head != NULL )
                                    {
                                        printf( " ^ ");
                                        printf("\n");
                                        printf( " | ");
                                        printf("\n");
                                        printf( " | ");
                                        printf("\n");
                                        printf( "[ Room's size          %d ]", head->size );
                                        printf("\n");
                                        printf( "[ Room's numDoor       %d ]", head->numDoors );
                                        printf("\n");
                                        if ( head->nextRoom != NULL ) { printf( "[ Room's nextRoom size %d ]", head->nextRoom->size ); }
                                        else { printf(" NULL "); }
                                        printf("\n");
                                        head = head->nextRoom;
                                    }
                                    printf("---------------------------------------------");
                                    printf("\n");
                                /**** delete ****/
}
                            /** ** ** *
            Function : 
                traverseRecursively 
            Parametres : 
                int : row : 
                    The row where the 0 was found 
                int : column : 
                    The column where the 0 was found 
                int : roomSize : 
                    The size of said room being traversed 
            Return :
                Void 
            Local Variables : 
            Functionality : 
                Taking the row and the column in the parametres, it recursively traverses the grid up, down, left, and right.  
                Base case : We stand in a space with surrounded by non zero spaces 
                Whatever else,
                    We mark locations already registerred already as FLIP_TILE ( global ) 
                        Increasing the counter for roomSize ( parametres ) as we go along
                    if a door is encountered : 
                        Increase door counter ( parametres ) 
                        Search within the doors array, within the building's floorplan, for that same door. 
                        Once found, change the values of openFromRoom and openToRoom by setting them to 
                        be the particular room being traversed, with a condition. 
                        If openFromRoom is NULL, the room becomes openFromRoom.
                        if openFromRoom is not NULL, the room becomes openToRoom
                    if a wall is encoutnered : 
                        Do nothing 
                            * ** ** **/
void traverseRecursively( Building **build, Room *newRoom, int floorNum, int rows, int columns )
{
// Increase size counter for the room
    ++newRoom->size;
// Change the value of the EMPTYSPACE to FLIP_TILE
    (*build)->floors[ floorNum ].grid[ rows ][ columns ] = FLIP_TILE; 
// Move up. Check
    if( ( (*build)->floors[ floorNum ].grid[ ( rows-1 ) ][ columns ] ) == EMPTYSPACE )
    {
// if EMPTYSPACE : Repeat 
        traverseRecursively( build, newRoom, floorNum, ( rows-1 ), columns );
    }
// if DOOR :
    else if( ( (*build)->floors[ floorNum ].grid[ ( rows-1 ) ][ columns ] ) == DOOR )
    {
// Increase counter 
        ++newRoom->numDoors;
// Check array of doors 
        for( int i = 0; i < (*build)->floors[ floorNum ].numDoors; i++ )
        {
            if( ((*build)->floors[ floorNum ].doors[ i ].row == ( rows-1 )) && ((*build)->floors[ floorNum ].doors[ i ].col == ( columns )) )
            {
// If the match have an openFromRoom value of NULL 
                if ( ((*build)->floors[ floorNum ].doors[ i ].openFromRoom) == NULL )
                {
// openFromRoom is set to be the newRoom 
                    (*build)->floors[ floorNum ].doors[ i ].openFromRoom = newRoom;
                }
// if openFromRoom is not NULL 
                else 
                {
// openToRoom is set to be the new Room 
                    (*build)->floors[ floorNum ].doors[ i ].openToRoom = newRoom;
                }
            }
        }
    }
//Repeat this process three more times, for directions : Down, Left, Right, respectively
// Move down 
    if( ( (*build)->floors[ floorNum ].grid[ ( rows+1 ) ][ columns ] ) == EMPTYSPACE )
    {
        traverseRecursively( build, newRoom, floorNum, ( rows+1 ), columns );
    }
    else if( ( (*build)->floors[ floorNum ].grid[ ( rows+1 ) ][ columns ] ) == DOOR )
    {
        ++newRoom->numDoors;
        for( int i = 0; i < (*build)->floors[ floorNum ].numDoors; i++ )
        {
            if( ((*build)->floors[ floorNum ].doors[ i ].row == ( rows+1 )) && ((*build)->floors[ floorNum ].doors[ i ].col == ( columns )) )
            {
                if ( ((*build)->floors[ floorNum ].doors[ i ].openFromRoom) == NULL )
                {
                    (*build)->floors[ floorNum ].doors[ i ].openFromRoom = newRoom;
                }
                else 
                {
                    (*build)->floors[ floorNum ].doors[ i ].openToRoom = newRoom;
                }
            }
        }
    }
// Go left 
    if( ( (*build)->floors[ floorNum ].grid[ rows ][ ( columns-1 ) ] ) == EMPTYSPACE )
    {
        traverseRecursively( build, newRoom, floorNum, rows, ( columns-1 ) );
    }
    else if( ( (*build)->floors[ floorNum ].grid[ rows ][ ( columns-1 ) ] ) == DOOR )
    {
        ++newRoom->numDoors;
        for( int i = 0; i < (*build)->floors[ floorNum ].numDoors; i++ )
        {
            if( ((*build)->floors[ floorNum ].doors[ i ].row == ( rows )) && ((*build)->floors[ floorNum ].doors[ i ].col == ( columns-1 )) )
            {
                if ( ((*build)->floors[ floorNum ].doors[ i ].openFromRoom) == NULL )
                {
                    (*build)->floors[ floorNum ].doors[ i ].openFromRoom = newRoom;
                }
                else 
                {
                    (*build)->floors[ floorNum ].doors[ i ].openToRoom = newRoom;
                }
            }
        }
    }
// Go right 
    if( ( (*build)->floors[ floorNum ].grid[ rows ][ ( columns+1 ) ] ) == EMPTYSPACE )
    {
        traverseRecursively( build, newRoom, floorNum, rows, ( columns+1 ) ); 
    }
    else if( ( (*build)->floors[ floorNum ].grid[ rows ][ ( columns+1 ) ] ) == DOOR )
    {
        ++newRoom->numDoors;
        for( int i = 0; i < (*build)->floors[ floorNum ].numDoors; i++ )
        {
            if( ((*build)->floors[ floorNum ].doors[ i ].row == ( rows )) && ((*build)->floors[ floorNum ].doors[ i ].col == ( columns+1 )) )
            {
                if ( ((*build)->floors[ floorNum ].doors[ i ].openFromRoom) == NULL )
                {
                    (*build)->floors[ floorNum ].doors[ i ].openFromRoom = newRoom;
                }
                else 
                {
                    (*build)->floors[ floorNum ].doors[ i ].openToRoom = newRoom;
                }
            }
        }
    }
// Base case for recursion : if the spaces in every direction are WALL, DOOR, or FLIP_TILE, then Return
    else
    {
        return;
    }
}
                            /** ** ** *
    Function : 
        sort Rooms 
    Parametres : 
        Pointer : Building : *build 
    Local Variables : 
    Funcitonality : 
        Goes through all the rooms in the Building ( Parametre ) and classifies them as 
            A storage room, if size <= 6 
            An office, if size > 6 and it has exactly one door 
            
        Bulding the linked list for each 
            
            IT MUS TGO THROUGH ROOMLSIT 
                            ** ** ** */
void sortRooms( Building *build)
{
    
}



/*************************************************************************************************
            BUILDING TEST . C 

#include <stdio.h>
#include <stdlib.h>

#include "building.h"
************************************************************************************************/
// This function creates the floors of a Building
void createFloors(Building *aBuilding) {
  // Add the 1st floor plan
  char  temp1[HEIGHT][WIDTH+1] = 
    {"111111111111111111111",
     "100000100000100000001",
     "100001110000100000001",
     "100002010000100000001",
     "111111011121121111111",
     "100010000000000020001",
     "100010112111111110001",
     "100020100000100010001",
     "100010100000100010001",
     "111110100000100010001",
     "100010100000100111211",
     "100010100000100200001",
     "100010111121111101211",
     "100020000000000001001",
     "111110000000001121111",
     "100020000000001000001",
     "100010000000001000001",
     "100012110000001000001",
     "100010010000001000001",
     "100010010000001000001",
     "111111111122111111111"};
  aBuilding->floors[0].numDoors = 0;
  aBuilding->floors[0].roomList = NULL;
  aBuilding->floors[0].doors = NULL;
  for (int r=0;r<HEIGHT; r++)
    for (int c=0; c<WIDTH; c++)
      aBuilding->floors[0].grid[r][c] = temp1[r][c];
    
  // Add the 2nd floor plan
  char temp2[HEIGHT][WIDTH+1] = 
    {"111111111111111111111",
     "100010000000000010001",
     "100020000000000020001",
     "100010000000000011211",
     "100011111121111110001",
     "100010001000100010001",
     "100010001000100011111",
     "100011211111112110001",
     "100010000000000010001",
     "100010000000000020001",
     "100011211000112110001",
     "100010001000100011111",
     "100010001000100010001",
     "100011111000111110001",
     "100010000000000020001",
     "100010000000000010001",
     "100011211000111111111",
     "100010001000100010001",
     "100020001000200020001",
     "100010001000100010001",
     "111111111121111111111"};
  aBuilding->floors[1].numDoors = 0;
  aBuilding->floors[1].roomList = NULL;
  aBuilding->floors[1].doors = NULL;
  for (int r=0;r<HEIGHT; r++)
    for (int c=0; c<WIDTH; c++)
      aBuilding->floors[1].grid[r][c] = temp2[r][c];
  
  // Add the 3rd floor plan
  char temp3[HEIGHT][WIDTH+1] = 
    {"111111111111111111111",
     "100010000000000010001",
     "100020000000000020001",
     "111110011121110011111",
     "100010010000010010001",
     "100020011111110020001",
     "111110020010020011111",
     "100010010010010010001",
     "100020011111110020001",
     "111110020010020011111",
     "200000010010010000002",
     "111110011111110011111",
     "100010020010020010001",
     "100020010010010020001",
     "111110011111110011111",
     "100010020010020010001",
     "100020010010010020001",
     "111110011111110011111",
     "100010000000000010001",
     "100020000000000020001",
     "111111111121111111111"};
  aBuilding->floors[2].numDoors = 0;
  aBuilding->floors[2].roomList = NULL;
  aBuilding->floors[2].doors = NULL;
  for (int r=0;r<HEIGHT; r++)
    for (int c=0; c<WIDTH; c++)
      aBuilding->floors[2].grid[r][c] = temp3[r][c];
    
  // Add the 4th floor plan
  char temp4[HEIGHT][WIDTH+1] = 
    {"111111111111111111111",
     "100010000010000010001",
     "100020000020000020001",
     "100010000010000010001",
     "112111121111121111211",
     "100010000010000010001",
     "100010000010000010001",
     "100010000010000010001",
     "112110000010000011211",
     "100010000010000010001",
     "100010000010000010001",
     "100011121111121110001",
     "112110000010000011211",
     "100010000010000010001",
     "100010000020000010001",
     "100010000010000010001",
     "112110000010000011211",
     "100011111111111110001",
     "100010000000000010001",
     "100020000000000020001",
     "111111111121111111111"};
  aBuilding->floors[3].numDoors = 0;
  aBuilding->floors[3].roomList = NULL;
  aBuilding->floors[3].doors = NULL;
  for (int r=0;r<HEIGHT; r++)
    for (int c=0; c<WIDTH; c++)
      aBuilding->floors[3].grid[r][c] = temp4[r][c];
  
  // Add the 5th floor plan
  char temp5[HEIGHT][WIDTH+1] = 
    {"111111111111111111111",
     "100010000010000010001",
     "100010000010000010001",
     "100010001111100010001",
     "100011211000112110001",
     "100010001000100010001",
     "100020002000200020001",
     "100010001000100010001",
     "111110001000100011211",
     "100010001000100010001",
     "100010001121100010001",
     "100010001000100010001",
     "100010001000100010001",
     "100020001111100020001",
     "100010000000000010001",
     "111110000000000011111",
     "100010000000000010001",
     "100012111000111210001",
     "100000001000100000001",
     "100000001000100000001",
     "111111111121111111111"};
  aBuilding->floors[4].numDoors = 0;
  aBuilding->floors[4].roomList = NULL;
  aBuilding->floors[4].doors = NULL;
  for (int r=0;r<HEIGHT; r++)
    for (int c=0; c<WIDTH; c++)
      aBuilding->floors[4].grid[r][c] = temp5[r][c];
}



// This is where it all begins
int main() {
  Building    building;

  // Initialize the building to have no lists of rooms to begin
  building.offices = NULL;
  building.meetingRooms = NULL;
  building.labs = NULL;
  building.hallways = NULL;
  building.storageRooms = NULL;
  
  // Create the floors for the building
  createFloors(&building);



  // Compute the graphs for each building plan and add them to a Graph Set
  for (int i=0; i<NUM_FLOORS; i++) {
    findDoors(&(building.floors[i]));
    findRooms(&building, i);
    }

  // Sort out all the rooms and display the results
//  sortRooms(&building);
  
                            /** ** ** *
  // Display the room lists
  printRoomList(building.offices, "Office");
  printRoomList(building.meetingRooms, "Meeting Room");
  printRoomList(building.labs, "Lab");
  printRoomList(building.storageRooms, "Storage Room");
  printRoomList(building.hallways, "Hallway");

  // Free all allocated memory
  freeEverything(&building);
  
                            * ** ** **/
}