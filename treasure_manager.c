#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#define DIM_MAX 100 // Maximum dimension for strings
mode_t mode = S_IRWXO | S_IRWXG | S_IRWXU;
// Struct to store GPS coordinates
typedef struct
{
    float latitude;
    float longitude;
} GPS;
// Struct to store treasure details
typedef struct
{
    char treasure_id[DIM_MAX];
    char user_name[DIM_MAX];
    GPS gps;
    char clue_text[DIM_MAX];
    int value;
} Treasure;
// Function to get treasure details from the user

int number_of_treasures(char *huntId)
{

    char path[DIM_MAX] = "";
    int fd, res = 0, count = 0;
    Treasure t;
    DIR *d;

    if (sprintf(path, "./hunts/%s", huntId) < 0)
    {
        perror("Error creating path: add function\n");
        exit(-1);
    }

    if (sprintf(path, "./hunts/%s/%s_treasures.dat", huntId, huntId) < 0)
    {
        perror("Error making treasure file path:number_of_treasures");
        exit(-1);
    }

    if ((fd = open(path, O_RDONLY, mode)) < 0)
    {
        perror("Error opening treasures file:number_of_treasures");
        exit(-1);
    }

    while ((res = read(fd, &t, sizeof(Treasure))) > 0)
    {
        count++;
    }

    if (res < 0)
    {
        perror("Error reading from treasures file:number_of_treasures");
        exit(-1);
    }

    if (close(fd) == -1)
    {
        perror("Error closing treasures file:number_of_treasures");
        exit(-1);
    }
    return count;
}

void list_hunts()
{

    struct dirent *dp;
    DIR *d;

    if ((d = opendir("./hunts")) == NULL)
    {
        perror("Error opening hunts directory:list_hunts");
        exit(-1);
    }

    while ((dp = readdir(d)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;

        printf("HuntID:%s\nNumber of treasures:%d\n", dp->d_name, number_of_treasures(dp->d_name));
    }
    if (closedir(d) == -1)
    {
        perror("Error closing dir:list_hunts");
        exit(-1);
    }
}

Treasure getTreasure()
{
    Treasure t;
    char buffer[DIM_MAX];
    char *endptr = NULL;
    printf("Introduce treasure_id: ");
    if (fgets(t.treasure_id, DIM_MAX, stdin) == NULL)
    {

        perror("Error reading treasure_id: getTreasure function");
        exit(-1);
    }
    t.treasure_id[strcspn(t.treasure_id, "\n")] = '\0';
    printf("Introduce user_name: ");
    if (fgets(t.user_name, DIM_MAX, stdin) == NULL)
    {
        perror("Error reading user_name: getTreasure function   ");
        exit(-1);
    }
    t.user_name[strcspn(t.user_name, "\n")] = '\0';
    printf("Introduce gps_latitude: ");
    if (fgets(buffer, DIM_MAX, stdin) == NULL)
    {
        perror("Error reading gps.latitude: getTreasure function");
        exit(-1);
    }
    t.gps.latitude = strtof(buffer, &endptr);
    if (endptr == buffer || *endptr != '\n')
    {
        perror("Invalid input for gps.latitude: getTreasure function");
        exit(-1);
    }
    printf("Introduce gps_longitude: ");
    if (fgets(buffer, DIM_MAX, stdin) == NULL)
    {
        perror("Error reading gps.longitude: getTreasure function");
        exit(-1);
    }
    t.gps.longitude = strtof(buffer, &endptr);
    if (endptr == buffer || *endptr != '\n')
    {
        perror("Invalid input for gps.longitude: getTreasure function");
        exit(-1);
    }
    printf("Introduce clue_text: ");
    if (fgets(t.clue_text, DIM_MAX, stdin) == NULL)
    {
        perror("Error reading clue_text: getTreasure function");
        exit(-1);
    }
    t.clue_text[strcspn(t.clue_text, "\n")] = '\0';
    printf("Introduce value: ");
    if (fgets(buffer, DIM_MAX, stdin) == NULL)
    {
        perror("Error reading value: getTreasure function");
        exit(-1);
    }
    t.value = strtol(buffer, &endptr, 10);
    if (endptr == buffer || *endptr != '\n')
    {
        perror("Invalid input for value: getTreasure function");
        exit(-1);
    }
    return t;
}
// Function to log messages to a file
void logger(char *hunt_id, char *message)
{
    int fd;
    char path[DIM_MAX] = "";
    // Create the path for the log file
    if (sprintf(path, "./hunts/%s/loggedhunt", hunt_id) < 0)
    {
        perror("Error creating path: logger function\n");
        exit(-1);
    }
    // Open the log file for appending
    fd = open(path, O_APPEND | O_WRONLY);
    if (fd == -1)
    {
        perror("Error opening log file: logger function\n");
        exit(-1);
    }
    // Write the message to the log file
    if (write(fd, message, strlen(message)) == -1)
    {
        perror("Error writing to log file: logger function\n");
        close(fd);
        exit(-1);
    }
    if (close(fd) == -1)
    {
        perror("Error closing log file: logger function\n");
        exit(-1);
    }
}
// Function to add a treasure to a hunt
void add(char *hunt_id)
{
    DIR *d;
    char path[DIM_MAX] = "";
    char path2[DIM_MAX] = "";
    char message[DIM_MAX] = "";
    int fd; // File descriptor for reading the treasures file

    Treasure t = getTreasure();
    // Check if the hunt directory exists, create it if not
    if (sprintf(path, "./hunts/%s", hunt_id) < 0)
    { 
        perror("Error creating path: add function\n");
        exit(-1);
    }
    if ((d = opendir(path)) == NULL)
    {
        if (mkdir(path, mode) == -1)
        {
            perror("Error opening directory: add function\n");
            exit(-1);
        }
        if (sprintf(path, "./hunts/%s/loggedhunt", hunt_id) < 0)
        {
            perror("Error creating path: add function\n");
            exit(-1);
        }

        if (creat(path, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
        {
            perror("Error creating log file: add function\n");
            exit(-1);
        }
        if (sprintf(path2, "./loggedhunt-%s", hunt_id) < 0)
        {
            perror("Error creating path: add function\n");
            exit(-1);
        }
        if (symlink(path, path2) == -1)
        {
            perror("Error creating symlink: add function\n");
            exit(-1);
        }
    }
    else
    {
        if (closedir(d) == -1)
        {
            perror("Error closing directory: add function\n");
            exit(-1);
        }
    }
    // Create the path for the treasures file
    if (sprintf(path, "./hunts/%s/%s_treasures.dat", hunt_id, hunt_id) < 0)
    {
        perror("Error creating path: add function\n");
        exit(-1);
    }
    fd = open(path, O_APPEND | O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd == -1)
    {
        perror("Error opening file: add function\n");
        exit(-1);
    }

    if (write(fd, &t, sizeof(Treasure)) == -1)
    {
        perror("Error writing to file: add function\n");
        exit(-1);
    }
    if (close(fd) == -1)
    {
        perror("Error closing file: add function\n");
        exit(-1);
    }
    if (sprintf(message, "Added treasure %s to hunt %s\n", t.treasure_id, hunt_id) < 0)
    {
        perror("Error creating message: add function\n");
        exit(-1);
    }
    logger(hunt_id, message); // Log the addition of the treasure
}
// Function to print the details of a treasure
void print_treasure(Treasure t)
{
    printf("Treasure ID: %s\n", t.treasure_id);
    printf("User Name: %s\n", t.user_name);
    printf("GPS Latitude: %f\n", t.gps.latitude);
    printf("GPS Longitude: %f\n", t.gps.longitude);
    printf("Clue Text: %s\n", t.clue_text);
    printf("Value: %d\n", t.value);
}
// Function to list all treasures in a hunt
void list(char *hunt_id)
{
    DIR *d;
    int fd;                // File descriptor for reading the treasures file
    struct stat file_stat; // Structure to store file information
    char message[DIM_MAX] = "";
    char path[DIM_MAX] = "";
    // Open the hunt directory
    if (sprintf(path, "./hunts/%s", hunt_id) < 0)
    {
        perror("Error creating path: add function\n");
        exit(-1);
    }
    if ((d = opendir(path)) == NULL)
    {
        printf("Hunt doesn't exist: list function\n");
        exit(-1);
    }

    printf("The hunt name: %s\n", hunt_id);
    // Create the path for the treasures file
    if (sprintf(path, "./hunts/%s/%s_treasures.dat", hunt_id, hunt_id) < 0)
    {
        perror("Error creating path: list function\n");
        exit(-1);
    }
    // Get file information
    if (stat(path, &file_stat) == -1)
    {
        perror("Error getting file information: list function\n");
        exit(-1);
    }
    printf("Total file size: %lld\n", file_stat.st_size);
    printf("Last modfication: %s\n", ctime(&file_stat.st_mtime));
    // Open the treasures file for reading
    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        perror("Error file doesn't exist: list function\n");
        exit(-1);
    }
    Treasure t;
    int res_read = 0;
    // Read and print each treasure
    while ((res_read = (read(fd, &t, sizeof(Treasure)))) > 0)
    {
        print_treasure(t);
        printf("\n");
    }
    if (res_read == -1)
    {
        perror("Error reading file : list function\n");
        exit(-1);
    }
    if (sprintf(message, "Listed all treasures from hunt %s\n", hunt_id) < 0)
    {
        perror("Error creating message: list function\n");
        exit(-1);
    }
    logger(hunt_id, message); // Log the listing of treasures
    // Close the file and directory
    if (close(fd) == -1)
    {
        perror("Error closing file: list function\n");
        exit(-1);
    }
    if (closedir(d) == -1)
    {
        perror("Error closing directory: list function\n");
        exit(-1);
    }
}
// Function to view a specific treasure in a hunt
void view(char *hunt_id, char *t_id)
{
    DIR *d;
    int fd; // File descriptor for reading the treasures file
    char path[DIM_MAX];
    char message[DIM_MAX] = "";
    // Open the hunt directory
    if (sprintf(path, "./hunts/%s", hunt_id) < 0)
    {
        perror("Error creating path: add function\n");
        exit(-1);
    }
    if ((d = opendir(path)) == NULL)
    {
        printf("Hunt doesn't exist: view function\n");
        exit(-1);
    }
    // Create the path for the treasures file
    if (sprintf(path, "./hunts/%s/%s_treasures.dat", hunt_id, hunt_id) < 0)
    {
        perror("Error creating path: view function\n");
        exit(-1);
    }
    // Open the treasures file for reading
    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        perror("Error file doesn't exist: view function\n");
        exit(-1);
    }
    Treasure t;

    int found = 0; // Flag to indicate if the treasure was found
    int res_read = 0;
    while ((res_read = (read(fd, &t, sizeof(Treasure)))) > 0)
    {
        if (strcmp(t.treasure_id, t_id) == 0) // Search for the treasure by ID
        {
            found = 1;         // Treasure found
            print_treasure(t); // Print the treasure details
            printf("\n");
            break;
        }
    }
    if (found == 0)
    {
        printf("Treasure not found: view function\n");
        exit(-1);
    }
    else
    {
        if (sprintf(message, "Viewed treasure %s from hunt %s\n", t.treasure_id, hunt_id) < 0)
        {
            perror("Error creating message: view function\n");
            exit(-1);
        }
        logger(hunt_id, message); // Log the viewing of the treasure
    }
    if (res_read == -1)
    {
        perror("Error reading file: view function\n");
        exit(-1);
    }
    // Close the file and directory
    if (close(fd) == -1)
    {
        perror("Error closing file: view function\n");
        exit(-1);
    }
    if (closedir(d) == -1)
    {
        perror("Error closing directory: view function\n");
        exit(-1);
    }
}
// Function to remove a treasure from a hunt
void remove_treasure(char *hunt_id, char *t_id)
{
    DIR *d;
    int fd;  // File descriptor for reading the treasures file
    int fd2; // File descriptor for reading the treasures file
    char path[DIM_MAX];
    char message[DIM_MAX] = "";
    // Open the hunt directory
    if (sprintf(path, "./hunts/%s", hunt_id) < 0)
    {
        perror("Error creating path: add function\n");
        exit(-1);
    }
    if ((d = opendir(path)) == NULL)
    {
        printf("Hunt doesn't exist: remove_treasure function\n");
        exit(-1);
    }
    // Create the path for the treasures file
    if (sprintf(path, "./hunts/%s/%s_treasures.dat", hunt_id, hunt_id) < 0)
    {
        perror("Error creating path: remove_treasure function\n");
        exit(-1);
    }
    // Open the treasures file for reading
    if ((fd = open(path, O_RDONLY)) == -1)
    {
        perror("Error opening file: remove_treasure function\n");
        exit(-1);
    }
    Treasure t;
    int treasure_position = 0; // Position of the treasure in the file
    int found = 0;             // Flag to indicate if the treasure was found
    int res_read = 0;
    // Search for the treasure by ID
    while ((res_read = (read(fd, &t, sizeof(Treasure)))) > 0)
    {
        if (strcmp(t.treasure_id, t_id) == 0)
        {
            found = 1; // Treasure found
            break;
        }
        treasure_position++; // Increment position for each treasure read
    }
    if (found == 0)
    {
        printf("Treasure not found: remove_treasure function\n");
        exit(-1);
    }
    if (res_read == -1)
    {
        perror("Error reading file: remove_treasure function\n");
        exit(-1);
    }
    if (found == 1)
    {
        // Open the treasures file for writing
        if ((fd2 = open(path, O_WRONLY)) == -1)
        {
            perror("Error opening file: remove_treasure function\n");
            exit(-1);
        }
        // Move the file pointer to the position of the treasure to be removed
        if (lseek(fd2, treasure_position * sizeof(Treasure), SEEK_SET) == -1)
        {
            perror("Error seeking in file: remove_treasure function\n");
            exit(-1);
        }

        // Shift all treasures after the removed one to fill the gap
        while ((res_read = (read(fd, &t, sizeof(Treasure)))) > 0)
        {
            if (write(fd2, &t, sizeof(Treasure)) == -1)
            {
                perror("Error writing to file: remove_treasure function\n");
                exit(-1);
            }
            treasure_position++; // Increment position for each treasure written
        }
        if (res_read == -1)
        {
            perror("Error reading file: remove_treasure function\n");
            exit(-1);
        }
        // Truncate the file to remove the extra space at the end
        if (ftruncate(fd2, treasure_position * sizeof(Treasure)) == -1)
        {
            perror("Error truncating file: remove_treasure function\n");
            exit(-1);
        }
        // Close the second file descriptor
        if (close(fd2) == -1)
        {
            perror("Error closing file: remove_treasure function\n");
            exit(-1);
        }
    }
    if (sprintf(message, "Removed treasure %s from hunt %s\n", t_id, hunt_id) < 0)
    {
        perror("Error creating message: remove_treasure function\n");
        exit(-1);
    }
    logger(hunt_id, message); // Log the removal of the treasure
    // Close the first file descriptor
    if (close(fd) == -1)
    {
        perror("Error closing file: remove_treasure function\n");
        exit(-1);
    }
    // Close the hunt directory
    if (closedir(d) == -1)
    {
        perror("Error closing directory: remove_treasure function\n");
        exit(-1);
    }
}
// Function to remove a hunt and its associated files
void remove_hunt(char *hunt_id)
{
    DIR *d;
    char path[DIM_MAX];
    struct dirent *dp;
    if (sprintf(path, "./hunts/%s", hunt_id) < 0)
    {
        perror("Error creating path\n");
        exit(-1);
    }
    if ((d = opendir(path)) == NULL) // Open the hunt directory
    {
        perror("Hunt doesn't exist: remove_hunt function\n");
        exit(-1);
    }
    // Create the path for the treasures file
    while ((dp = readdir(d)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue; // Skip the current and parent directory entries
        if (sprintf(path, "./hunts/%s/%s", hunt_id, dp->d_name) < 0)
        {
            perror("Error creating path: remove_hunt function\n");
            exit(-1);
        }
        // Remove the file
        if (unlink(path) == -1)
        {
            perror("Error removing file: remove_hunt function\n");
            exit(-1);
        }
    }
    if (sprintf(path, "./hunts/%s", hunt_id) < 0)
    {
        perror("Error creating path: remove_hunt function\n");
        exit(-1);
    }
    if (rmdir(path) == -1)
    {
        perror("Error removing directory: remove_hunt function\n");
        exit(-1);
    }
    // Create the path for the symlink
    if (sprintf(path, "./loggedhunt-%s", hunt_id) < 0)
    {
        perror("Error creating path: remove_hunt function\n");
        exit(-1);
    }
    if (unlink(path) == -1)
    {
        perror("Error removing symlink: remove_hunt function\n");
        exit(-1);
    }
    // Close the hunt directory
    if (closedir(d) == -1)
    {
        perror("Error closing directory: remove_hunt function\n");
        exit(-1);
    }
}
int main(int argc, char **argv)
{
    int ok = 0;

    if (argc < 3)
    {
        if (argc == 2 && (strcmp(argv[1], "--list_hunts") == 0))
        {
            list_hunts();
            ok = 1;
        }
        else
        {
            ok = 1;
            printf("Not enough arguments\n");
            exit(-1);
        }
    }
    mkdir("./hunts", mode);
    if (strcmp(argv[1], "--add") == 0)
    {
        ok = 1;
        if (argc != 3)
        {
            printf("The number of arguments is incorrect\n");
            exit(-1);
        }
        else
        {
            add(argv[2]);
        }
    }
    if (strcmp(argv[1], "--list") == 0)
    {
        ok = 1;
        if (argc != 3)
        {
            printf("The number of arguments is incorrect\n");
            exit(-1);
        }
        else
        {
            list(argv[2]);
        }
    }
    if (strcmp(argv[1], "--view") == 0)
    {
        ok = 1;
        if (argc != 4)
        {
            printf("The number of arguments is incorrect\n");
            exit(-1);
        }
        else
        {
            view(argv[2], argv[3]);
        }
    }
    if (strcmp(argv[1], "--remove") == 0)
    {
        ok = 1;
        if (argc != 4)
        {
            printf("The number of arguments is incorrect\n");
            exit(-1);
        }
        else
        {
            remove_treasure(argv[2], argv[3]);
        }
    }
    if (strcmp(argv[1], "--remove-hunt") == 0)
    {
        ok = 1;
        if (argc != 3)
        {
            printf("The number of arguments is incorrect\n");
            exit(-1);
        }
        else
        {
            remove_hunt(argv[2]);
        }
    }
    if (ok == 0)
    {
        printf("Invalid command\n");
        exit(-1);
    }
}