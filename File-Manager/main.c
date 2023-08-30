#include "libs.h"
#include "header.h"

#define SLEEP_TIME_MS 100
#define MAX_PATH_LENGTH 255

#define KEY_ESC 0
#define KEY_ARROW_UP 1
#define KEY_ARROW_RIGHT 2
#define KEY_ARROW_DOWN 3
#define KEY_ARROW_LEFT 4
#define KEY_SPACEBAR 5
#define KEY_X 6
#define KEY_C 7
#define KEY_V 8
#define KEY_ENTER 9 
#define KEY_DELETE 10

#define WINDOW_WIDTH 44
#define WINDOW_HEIGHT 42

const int MAX_FILES_ON_SCREEN = 37;
const int BUFFER_STATUS_LINE = 38;
const int FILE_COUNT_INFO_LINE = 39;
const int KEYS_INFO_LINE = 40; // uses 2 lines 

enum bufferState
{
    BUFFER_EMPTY,
    BUFFER_COPYING,
    BUFFER_MOVING
};

int dirSize; // Number of files and directories in the current directory
enum bufferState currentBufferState = BUFFER_EMPTY; // 0 for empty, 1 for not empty

// Clear screen, print error message and last error code, then stop the program
void printErrorMessage(char* errorMessage)
{
    clrscr();
    gotoxy(1, 1);
    printf("%s. Error code: %ld.\n", errorMessage, GetLastError());
    system("pause");
}

void printKeysInfo()
{
    gotoxy(0, KEYS_INFO_LINE);
    textcolor(DARKGRAY);
    printf("[C] copy, [X] move, [V] paste,\n[DEL] delete, [ENTER] open file, [ESC] exit");
}

// Return a pressed key as a value from 0 to 4
// Arrow up = 1, Arrow down = 3, Arrow right = 2, Arrow left = 4, ESC = 0
int getKey_Browse()
{
    while(1) {
        // Check if the window is focused
        if(GetForegroundWindow() != GetConsoleWindow())
            continue;
        
        if (GetAsyncKeyState(VK_ESCAPE) & 0x07)
            return KEY_ESC;
        else if (GetAsyncKeyState(VK_UP) & 0x07)
            return KEY_ARROW_UP;
        else if (GetAsyncKeyState(VK_RIGHT) & 0x07)
            return KEY_ARROW_RIGHT;
        else if (GetAsyncKeyState(VK_DOWN) & 0x07)
            return KEY_ARROW_DOWN;
        else if (GetAsyncKeyState(VK_LEFT) & 0x07)
            return KEY_ARROW_LEFT;
        else if (GetAsyncKeyState(VK_SPACE) & 0x07)
            return KEY_SPACEBAR;
        else if (GetAsyncKeyState(0x58) & 0x07)
            return KEY_X;
        else if (GetAsyncKeyState(0x43) & 0x07)
            return KEY_C;
        else if (GetAsyncKeyState(0x56) & 0x07)
            return KEY_V;
        else if (GetAsyncKeyState(VK_DELETE) & 0x07)
            return KEY_DELETE;
        else if (GetAsyncKeyState(VK_RETURN) & 0x07)
            return KEY_ENTER;

        Sleep(SLEEP_TIME_MS);
    }
}

// Return sort type as an int
int getKey_Sort() 
{
    while(1) {
        // Check if the window is focused
        if(GetForegroundWindow() != GetConsoleWindow())
            continue;
        
        if (GetAsyncKeyState(0x31) & 0x07)
            return 1;
        if (GetAsyncKeyState(0x32) & 0x07)
            return 2;
        if (GetAsyncKeyState(0x33) & 0x07)
            return 3;
        if (GetAsyncKeyState(0x34) & 0x07)
            return 4;
        if (GetAsyncKeyState(0x35) & 0x07)
            return 5;
        if (GetAsyncKeyState(0x36) & 0x07)
            return 6;
        if (GetAsyncKeyState(0x30) & 0x07)
            return 0;

        Sleep(SLEEP_TIME_MS);
    }
}

// Print a table with all files and folders in the current directory
void printDirectory(struct _finddata_t* files)
{
    int count = 0; 
    if (dirSize == 0) 
    {
        gotoxy(0, 0);
        printf("Empty directory.\n\n");
    }
    else
    {
        gotoxy(0, 0);
        for (int i = 0; i < dirSize; i++) 
        {
            if (count > MAX_FILES_ON_SCREEN)
                break;
            else
            {
                if (files[i].size == 0)
                {
                    printf(" %-26.26s  %14s\n", files[i].name, "<DIR>");
                }
                else
                {
                    printf(" %-26.26s  %11lu KB\n", files[i].name, files[i].size / 1024);
                }
            }
        }
        gotoxy(0, FILE_COUNT_INFO_LINE);
        printf("Files in directory: %d", dirSize);
    }
}

void printBufferStatus(char bufferFileName[])
{
    // Nothing to pring if buffer is empty
    if (currentBufferState == BUFFER_EMPTY)
        return;
    
    // Clear line from previous prints
    gotoxy(0, BUFFER_STATUS_LINE);
    char clearline[WINDOW_WIDTH] = "                                        ";
    textcolor(BLACK);
    printf("%s", clearline);

    // Print buffer status
    gotoxy(0, BUFFER_STATUS_LINE);
    textcolor(LIGHTGREEN);

    switch(currentBufferState)
    {
        case BUFFER_MOVING:
            printf("Moving: %s", bufferFileName);
            break;
        case BUFFER_COPYING:
            printf("Copying: %s", bufferFileName);
            break;
        case BUFFER_EMPTY:
            break;
    }
    
    textcolor(LIGHTGRAY); // back to default text color
}

void printCustomBufferStatus(char* message, enum TColor textColor)
{
    // Clear line from previous prints
    gotoxy(0, BUFFER_STATUS_LINE);
    char clearline[WINDOW_WIDTH] = "                                        ";
    textcolor(BLACK);
    printf("%s", clearline);

    // Print custom line
    gotoxy(0, BUFFER_STATUS_LINE);
    textcolor(textColor);

    printf("%s", message);
    
    textcolor(LIGHTGRAY); // back to default text color
}

// Change background color and text color of a given line 
void printColoredLine(int line, int color_text, int color_back, struct _finddata_t* file)
{
    gotoxy(0, line);
    textcolor(color_text);
    textbackground(color_back);

    if (file[line].size == 0)
        printf(" %-26.26s  %14s\n", file[line].name, "<DIR>");
    else
        printf(" %-26.26s  %11lu KB\n", file[line].name, file[line].size / 1024);

    // Set colors back to default
    textcolor(LIGHTGRAY);
    textbackground(BLACK);
}

// Append str2/*.* to the end of str1
void appendWildcard(char str1[], char str2[]) 
{
    int i = 0;
    int j = 0;

    while ((str1[i] != '\0') && (str1[i] != '*')) 
    {
        i++;
    }

    while (str2[j] != '\0') 
    {
        str1[i] = str2[j];
        i++;
        j++;
    }

    str1[i] = '\\';
    str1[i+1] = '*';
    str1[i+2] = '.';
    str1[i+3] = '*';
    str1[i+4] = '\0';
}

// Strip a string from the "*.*" 
void removeWildcard(char* str1) 
{
    char* wildPtr = strstr(str1, "*.*");
    if (wildPtr != NULL)
    {
        *wildPtr = '\0'; // end the string at the first char of "*.*"
    }
}

// Change path to previous directory
void prevDir(char path[])
{
    int i = 0;

    while (path[i] != '\0')
        i++;

    i = i - 5;

    while (path[i] != '\\') 
        i--;

    path[i+1] = '*';
    path[i+2] = '.';
    path[i+3] = '*';
    path[i+4] = '\0';
}   

int getFileCount(char path[])
{
    struct _finddata_t c_file;
    intptr_t hFile;

    int count = 0;
    if ((hFile = _findfirst(path, &c_file)) == -1L)
        count = 0;
    else
    {
        do
            count++;
        while (_findnext(hFile, &c_file) == 0);

        if (path[3] != '*')
        count = count - 2;
    }
    return count;
}

// Return contents of a given directory as a struct
struct _finddata_t* getValues(char path[])
{
    struct _finddata_t c_file;
    intptr_t hFile;
    
    // Initaialize an array with the size of the amount of files 
    dirSize = getFileCount(path);
    struct _finddata_t *files_in_dir = (struct _finddata_t *)malloc(sizeof(struct _finddata_t) * dirSize);

    int i = 0;
    if ((hFile = _findfirst(path, &c_file)) == -1L)
        files_in_dir[0].size = -1;
    else
        do
        {
            if (c_file.name[0] != '.' && !(c_file.attrib & _A_SYSTEM)) 
            {
                files_in_dir[i] = c_file;
                i++;
            }
        } 
        while(_findnext(hFile, &c_file) == 0);
    _findclose(hFile);

    dirSize = i;
    return files_in_dir;
}

// Move file from source to destination. Returns 1 if succeeded, 0 if failed
int moveFile(char source[], char destination[])
{
    if (!MoveFile(source, destination))
    {
        if (GetLastError() == 183)
            printCustomBufferStatus("File already exists.", LIGHTRED);
        else
        {
            printCustomBufferStatus("Move failed.", LIGHTRED);
            printf("%ld", GetLastError());
        }
        return 0;
    }
    else
        return 1;
}

// Copy file from source to destination. Returns 1 if succeeded, 0 if failed
int copyFile(char source[], char destination[])
{
    SetFileAttributes(source, FILE_ATTRIBUTE_NORMAL);
    SetFileAttributes(destination, FILE_ATTRIBUTE_NORMAL);

    if (!CopyFileA(source, destination, FALSE))
    {
        if (GetLastError() == 183)
            printCustomBufferStatus("File already exists.", LIGHTRED);
        else
        {
            printCustomBufferStatus("Copy failed.", LIGHTRED);
            printf("%ld", GetLastError());
        }
        return 0;
    }
    else
        return 1;
}

int openFile(char pathToFile[])
{
    char systemPrompt[MAX_PATH_LENGTH + 6] = "start ";
    strcat(systemPrompt, pathToFile);
    system(systemPrompt);
    /*
    HINSTANCE hInstance = ShellExecute(NULL, L"open", pathToFile, NULL, NULL, SW_SHOWNORMAL);

    if ((intptr_t)hInstance <= 32)
    {
        printCustomBufferStatus("Can't open file.", LIGHTRED);
        return 0;
    }
    else
    {
        printCustomBufferStatus("Done.", LIGHTGREEN);
        return 1;
    }
    */
}

int deleteFile(char pathToFile[])
{
    SetFileAttributes(pathToFile, FILE_ATTRIBUTE_NORMAL);
    return DeleteFile(pathToFile);
}

int main(void)
{
    struct _finddata_t* files;
    char currentPath[MAX_PATH_LENGTH] = "C:\\*.*";
    char bufferPath[MAX_PATH_LENGTH];
    char bufferFileName[MAX_PATH_LENGTH];
    char _currentPath[MAX_PATH_LENGTH];

    int posInMenu = 0;   // selected file
    int prevPos = 0;     // previous selected file
    int key = -1;        // key pressed
    int depth = 0;       // how deep is current directory relative to C:/
    int posInPrevDir;    // position in previous directory

    hidecursor();
    setwindow(WINDOW_WIDTH, WINDOW_HEIGHT);

    files = getValues(currentPath);
    sort_bubble(files, dirSize);
    printDirectory(files);
    printBufferStatus(bufferFileName);
    printColoredLine(posInMenu, BLACK, WHITE, files);
    printKeysInfo();

    while (key != 0) 
    {
        key = -1;
        key = getKey_Browse();
        switch(key)
        {
            case KEY_ARROW_UP:                 // Arrow up is pressed
                if (dirSize != 0)
                {
                    if (posInMenu > 0)         // If not on the first line
                    {
                        prevPos = posInMenu;
                        posInMenu--;

                        printColoredLine(prevPos, LIGHTGRAY, BLACK, files);
                        printColoredLine(posInMenu, BLACK, WHITE, files);
                    }
                    else                       // If on the first line
                    {
                        prevPos = 0;
                        if (dirSize > MAX_FILES_ON_SCREEN)
                            posInMenu = MAX_FILES_ON_SCREEN - 1;
                        else
                            posInMenu = dirSize - 1;

                        printColoredLine(prevPos, LIGHTGRAY, BLACK, files);
                        printColoredLine(posInMenu, BLACK, WHITE, files);
                    }
                }
                break;

            case KEY_ARROW_DOWN:    // Arrow down is pressed
                if (dirSize != 0)
                {   // If on the last line (case 1)
                    if ((posInMenu == dirSize - 1) && dirSize < MAX_FILES_ON_SCREEN) 
                    {
                        prevPos = dirSize - 1;
                        posInMenu = 0;

                        printColoredLine(prevPos, LIGHTGRAY, BLACK, files);
                        printColoredLine(posInMenu, BLACK, WHITE, files);
                    }
                    else
                    {   // If on the last line (case 2)
                        if (posInMenu == MAX_FILES_ON_SCREEN - 1) 
                        {
                            prevPos = MAX_FILES_ON_SCREEN - 1;
                            posInMenu = 0;
                            
                            printColoredLine(prevPos, LIGHTGRAY, BLACK, files);
                            printColoredLine(posInMenu, BLACK, WHITE, files);
                        }
                        else                            // If on the last line
                        {
                            prevPos = posInMenu;
                            posInMenu++;

                            printColoredLine(prevPos, LIGHTGRAY, BLACK, files);
                            printColoredLine(posInMenu, BLACK, WHITE, files);
                        }
                    }
                }
                break;

            case KEY_ARROW_RIGHT:    // Arrow right is pressed
                if (files[posInMenu].size == 0) // If a directory is selected
                { 
                    appendWildcard(currentPath, files[posInMenu].name);
                    files = getValues(currentPath);
                    clrscr();

                    posInPrevDir = posInMenu;
                    posInMenu = 0;
                    prevPos = 0;

                    printDirectory(files);
                    printBufferStatus(bufferFileName);
                    printKeysInfo();
                    
                    if (dirSize != 0)
                        printColoredLine(posInMenu, BLACK, WHITE, files);

                    depth++;
                }
                break;

            case KEY_ARROW_LEFT:    // Arrow left is pressed 
                if (depth > 0) 
                {
                    prevDir(currentPath);
                    files = getValues(currentPath);
                    clrscr();

                    posInMenu = posInPrevDir;
                    prevPos = 0;

                    printDirectory(files);
                    printBufferStatus(bufferFileName);
                    printKeysInfo();

                    if (dirSize != 0)
                        printColoredLine(posInMenu, BLACK, WHITE, files);
                    depth--;
                }
                break;

            case KEY_X:
                // Save the filename and path to it
                strcpy(bufferFileName, files[posInMenu].name);
                strcpy(bufferPath, currentPath);
                currentBufferState = BUFFER_MOVING;
                printBufferStatus(bufferFileName);
                break;

            case KEY_C:
                if (files[posInMenu].size == 0)
                {
                    printCustomBufferStatus("Can't copy a directory.", LIGHTRED);
                    break;
                }

                // Save the filename and path to it
                strcpy(bufferFileName, files[posInMenu].name);
                strcpy(bufferPath, currentPath);
                currentBufferState = BUFFER_COPYING;
                printBufferStatus(bufferFileName);
                break;

            case KEY_V:
                // Do nothing if buffer is empty
                if (currentBufferState == BUFFER_EMPTY)
                {
                    printCustomBufferStatus("Can't paste. Buffer empty.", LIGHTRED);
                    break;
                }

                // Make a copy of currentPath, because its used elsewhere
                strcpy(_currentPath, currentPath);
                removeWildcard(bufferPath);
                removeWildcard(_currentPath);

                // Append filename to the paths 
                strcat(bufferPath, bufferFileName);
                strcat(_currentPath, bufferFileName);

                // Determine whether moving or copying a file
                if (currentBufferState == BUFFER_MOVING)
                {
                    moveFile(bufferPath, _currentPath);
                    currentBufferState = BUFFER_EMPTY;
                }
                else if (currentBufferState == BUFFER_COPYING)
                {
                    if (!copyFile(bufferPath, _currentPath))
                    {
                        printCustomBufferStatus("Failed to paste file.", LIGHTRED);
                        break;
                    }
                    else
                    {
                        currentBufferState = BUFFER_EMPTY;
                    }
                }

                // Update current dir
                files = getValues(currentPath);
                clrscr();
                printDirectory(files);
                printKeysInfo();
                printColoredLine(posInMenu, BLACK, WHITE, files);
                printCustomBufferStatus("Done.", LIGHTGREEN);

                break;
            
            case KEY_DELETE:
                // Make a copy of currentPath, because its used elsewhere
                strcpy(_currentPath, currentPath);
                removeWildcard(_currentPath);

                // Append filename to the path
                strcat(_currentPath, files[posInMenu].name);

                // Confirm delete
                char confirmation;
                clrscr();
                printf(" Deleting file '%s'\n Located in: %s\n Are you sure? (y/n) ",
                        files[posInMenu].name, currentPath);
                scanf(" %c", &confirmation);
                clrscr();
                if (confirmation == 'y')
                {
                    if (!deleteFile(_currentPath))
                    {
                        printCustomBufferStatus("Failed to delete.", LIGHTRED);
                        printf(" %ld.", GetLastError());
                    }
                    else
                        printCustomBufferStatus("Done.", LIGHTGREEN);
                }
                else
                {
                    printCustomBufferStatus("Delete cancelled.", LIGHTRED);
                }

                // Update current dir
                files = getValues(currentPath);
                printDirectory(files);
                printKeysInfo();
                printColoredLine(posInMenu, BLACK, WHITE, files);
                printBufferStatus(bufferFileName);

                break;

            case KEY_ENTER:
                if (files[posInMenu].size == 0)
                {
                    printCustomBufferStatus("Can't open DIR", LIGHTRED);   
                }

                // Make a copy of currentPath, because its used elsewhere
                strcpy(_currentPath, currentPath);
                removeWildcard(_currentPath);

                // Append filename to the path
                strcat(_currentPath, files[posInMenu].name);

                openFile(_currentPath);

                break;

            case KEY_SPACEBAR:
                if (dirSize > 0)
                {
                    clrscr();
                    gotoxy(0,0);
                    printf
                        (
                        "Press a key to choose a sort method\n\n"
                        "1 - Bubble sort\n"
                        "2 - Selection sort\n"
                        "3 - Insertion sort\n"
                        "4 - Merge sort\n"
                        "5 - Quick sort\n"
                        "6 - Shell sort\n\n"
                        "0 - Cancel"
                        );
                    int key2 = -1;
                    while (key2 != 0)
                    {
                        key2 = getKey_Sort();
                        clrscr();
                        switch (key2)
                        {
                            case 0:
                                break;
                            case 1:
                                sort_bubble(files, dirSize);
                                break;
                            case 2:
                                sort_selection(files, dirSize);
                                break;
                            case 3:
                                sort_insertion(files, dirSize);
                                break;
                            case 4:
                                sort_merge(files, 0, dirSize - 1);
                                break;
                            case 5:
                                sort_quick(files, dirSize);
                                break;
                            case 6:
                                sort_shell(files, dirSize);
                                break;
                        }
                        printDirectory(files);
                        printKeysInfo();
                        printBufferStatus(bufferFileName);
                        printColoredLine(0, BLACK, WHITE, files);
                        posInMenu = 0;
                        prevPos = 0;
                        key2 = 0;
                    } 
                }
                break;

            case 0:
                system("pause");
                return 1;
        }
    }
    system("pause");
    return 1;
}
