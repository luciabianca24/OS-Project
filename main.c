#include <stdio.h>
#include <stdint.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#define DIM_MAX 100
typedef struct
{
    float latitude;
    float longitude;
} GPS;
typedef struct
{
    char treasure_id[DIM_MAX];
    char user_name[DIM_MAX];
    GPS gps;
    char clue_text[DIM_MAX];
    int value;
} Treasure;

Treasure getTreasure()
{
    Treasure t;
    fgets(t.treasure_id, DIM_MAX, stdin);
    fgets(t.user_name, DIM_MAX, stdin);
    scanf("%f", &t.gps.latitude);
    scanf("%f", &t.gps.longitude);
    fgets(t.clue_text, DIM_MAX, stdin);
    scanf("%d", &t.value);
    return t;
}

void add(char *hunt_id)
{
    DIR *d;
    char path[DIM_MAX] = "";
    int fd;
    if ((d = opendir(hunt_id)) == NULL)
    {
        mkdir(hunt_id, S_IRWXU | S_IRWXG | S_IRWXO);
    }
    else
    {
        closedir(d);
    }
    sprintf(path, "./%s/%s_treasures.dat", hunt_id, hunt_id);
    fd = open(path, O_APPEND | O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    Treasure t = getTreasure();
}
void list(int)
{
}

void view(char *hunt_id, char *treasure_id);

int main(int argc, char **argv)
{
    add("game1");
}