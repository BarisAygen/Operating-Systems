#include <stdio.h>
#include <string.h>
#include <dirent.h>

// Structes
typedef struct Person{
    char gender;
    char name[100];
    char surname[100];
} Person;

// Functions
void correctTxtFile(const char *filename, Person *data, int dataSize) 
{
    FILE *file = fopen(filename, "r+"); // Open file in "r+" mode
    long int size = 10000;
    char* line = (char*) malloc(size * sizeof(char));
    if (!file) // If file could not open
    {
        printf("Cannot open the file!");
        return;
    }
        fread(line, sizeof(char), size, file);
        int i;
        for (i = 0; i < dataSize; i++)
        {
            Person p = data[i]; // Create new struct and assing it to the current person struct in array
            char *index = strstr(line, p.name);
            if (index)
            {
                int titleOffset = index - line -2;
                if (p.gender == 'm')
                {
                    fseek(file, titleOffset - 2, SEEK_SET);
                    fputs("Mr.", file);
                }
                else if (p.gender == 'f')
                {
                    fseek(file, titleOffset - 2, SEEK_SET);
                    fputs("Ms.", file);
                }
                int surnameOffset = titleOffset + strlen(p.name) + 3;

                if (strncmp(line + surnameOffset, p.surname, strlen(p.surname)) != 0) 
                {
                    fseek(file, surnameOffset, SEEK_SET);
                    fputs(p.surname, file);
                }
            }
        }
    
    fclose(file);
}

void processDirectory(const char *dirname, int dataSize, Person *data) {
    DIR *dir = opendir(dirname); // Open directory
    struct dirent *dir2;

    if (!dir) // Directory steam could not open
    {
        printf("Cannot open the directory!");
        return;
    }

    while ((dir2 = readdir(dir)) != NULL) 
    {
        if (strcmp(dir2->d_name, "..") == 0 || strcmp(dir2->d_name, ".") == 0) 
        {
            continue;
        }

        char path[1000];
        snprintf(path, 1000, "%s/%s", dirname, dir2->d_name);

        if (dir2->d_type == DT_DIR) // Still in directory
        {
            processDirectory(path, dataSize, data); // Countinue recursiveley to process directory
        }
        else if (dir2->d_type == DT_REG) // Regular file
        {
            if (strstr(dir2->d_name, ".txt") != NULL && strstr(path, "database.txt") == NULL) 
            {
                correctTxtFile(path, data, dataSize); // Go to the corrector function
            }
        }
    }
    closedir(dir); // Close directory
}

// Main code
int main(int argc, char *argv[]) {
    int dataSize;
    Person data[100];
    char line[1000];
    FILE *dataFile = fopen("./database.txt", "r"); // Open File

    if (!dataFile) // If file could not open
    {
        printf("File cannot open");
        return -1;
    }

    dataSize = 0;

    while (fgets(line, 1000, dataFile)) // Get data sie and while getting it assing the values
    {
        sscanf(line, "%c %s %s", &data[dataSize].gender, data[dataSize].name, data[dataSize].surname);
        dataSize++;
    }

    fclose(dataFile); // Close file
    processDirectory("./", dataSize, data);
    return 0;
}