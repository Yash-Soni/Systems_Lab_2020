#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include <sys/types.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<stdbool.h>

#define clear() printf("\033[H\033[J")
int invalid = 0, ls = 1, ls_a = 2, cat = 3, cat_n = 4, cd = 5, mkdr = 6, mkdr_m = 7, cp = 8, cp_u = 9, grep = 10, grep_n = 11, sort = 12, sort_r = 13;

int isDifferentFrom(FILE*, FILE*);
void sort_file(FILE*,int,int);
bool doesFilesExist(char *[10], int, int, char*);

static int ascendingComparator(const void* a, const void* b)
{
    const char** p = (const char**)a;
    const char** q = (const char**)b;
    return strcasecmp(*p, *q);
}
static int discendingComparator(const void* a, const void* b)
{
    const char** p = (const char**)a;
    const char** q = (const char**)b;
    return -strcasecmp(*p, *q);
}

int main(){
    //char input[100];
    char* input;
    char cwd[2000];

    getcwd(cwd, sizeof(cwd));       //getting the current directory

    char* cwdPtr = strdup(cwd);

    printf("\n");
    strcat(cwd, "~$");
    input = readline(cwd);

    while(strcmp(input, "exit") != 0){
        add_history(input);

        struct dirent* content;
        int n = 0;

        char* inputArray[10];

        int numArguments = 0;
        char* tempInput = strdup(input);
        char* argument;
        while((argument = strsep(&tempInput, " ")) != NULL){
            if(strlen(argument) >= 1 && argument[0] != '\0'){
                inputArray[numArguments++] = argument;
            }
        }

        int command = invalid;

        if(numArguments >= 1){

            if(strcmp(inputArray[0], "ls") == 0){
                command = ls;
                if(numArguments == 2){
                    if(strcmp(inputArray[1], "-a") == 0){
                        command = ls_a;
                    }
                    else {
                        command = invalid;//invalid
                    }
                }
                else if(numArguments > 2){
                    command = invalid;//invalid
                }
            }
            else if(strcmp(inputArray[0], "cat") == 0) {

                command = cat;
                if(numArguments >= 2){
                    int flag =0;
                    for(int i=1;i<numArguments;i++)
                    {
                        if(strcmp(inputArray[i],"-n")==0)
                        {
                            flag =1;
                        }
                    }
                    if(flag == 1){
                        command = cat_n;
                    }
                }
                else {
                    command = invalid;
                }

            }
            else if(strcmp(inputArray[0], "cd") == 0)
            {

                if(numArguments == 2){
                    command = cd;
                }
                else {
                    command = invalid;
                }
            }
            else if(strcmp(inputArray[0], "mkdir") == 0)
            {
                command = mkdr;
                if(numArguments >= 2)
                {
                    if(strcmp(inputArray[1], "-m") == 0){
                        command = mkdr_m;
                    }
                }
                else {
                    command = invalid;
                }
            }
            else if(strcmp(inputArray[0], "cp") == 0)
            {
                command = cp;
                if(numArguments >= 3){
                    if(strcmp(inputArray[1], "-u") == 0){
                        command = cp_u;
                    }
                }
                else {
                    command = invalid;
                }
            }
            else if(strcmp(inputArray[0], "grep") == 0){
                command = grep;
                if(numArguments >= 3){
                    if(strcmp(inputArray[1], "-n") == 0){
                        command = grep_n;
                    }
                }
                else {
                    command = invalid;
                }
            }
            else if(strcmp(inputArray[0], "sort") == 0)
            {
                command = sort;
                if(numArguments >= 2)
                {
                    int flag =0;
                    for(int i=1;i<numArguments;i++)
                    {
                        if(strcmp(inputArray[i],"-r")==0)
                        {
                            flag =1;
                        }
                    }
                    if(flag == 1){
                        command = sort_r;
                    }
                }
                else
                {
                    command = invalid;
                }
            }

            if(command == ls || command == ls_a) {
                //n = scandir(".", content, NULL, alphasort);
                DIR *mydir = opendir(cwdPtr);
                while((content = readdir(mydir)) != NULL){
                    //stat(content->d_name, &mystat);
                    //printf("%d\t", mystat.st_size);
                    if(command == ls_a || (command == ls && (strcmp(content->d_name, ".") && strcmp(content->d_name, ".."))))
                    {
                        printf("%s\t", content->d_name);
                    }
                }
                closedir(mydir);
            }
            else if(command == cat || command == cat_n)
            {
                FILE *file;
                char line[1024];
                int startFileIdx = 1,index;
                int flag =0;
                for(int i=1;i<numArguments;i++)
                {
                    if(strcmp(inputArray[i],"-n")==0)
                    {
                        index = i;
                        flag =1;
                    }
                }

                if(startFileIdx >= numArguments){
                    command = invalid;
                }
                else {
                    /* check files existance */
                    bool filesExist;
                    if(flag == 1)
                    {
                        filesExist = doesFilesExist(inputArray, startFileIdx, index - 1, "cat");
                        filesExist = doesFilesExist(inputArray, index+1, numArguments - 1, "cat");
                    }
                    if(filesExist){

                        int j = 1;
                        //for each input files
                        for(int i = startFileIdx; i < numArguments; i++)
                        {
                            if(strcmp(inputArray[i],"-n")==0)
                            {
                                continue;
                            }
                            else
                            {
                                file = fopen(inputArray[i],"r");
                                while(fgets(line, sizeof(line), file) != NULL) {
                                    if(command == cat_n)
                                    {
                                        printf("%d: ", j++);
                                    }
                                    printf("%s", line);
                                }
                                fclose(file);
                                printf("\n");
                            }
                        }
                    }
                }
            }
            else if(command == cd)
            {
                char *oldDir;
                oldDir = strdup(cwdPtr);
                chdir(inputArray[1]);
                getcwd(cwd,sizeof(cwd));        //getting current directory
                cwdPtr = strdup(cwd);
                strcat(cwd, "~$");

                if(strcmp(oldDir,cwdPtr) == 0)
                {
                    if(strcmp(oldDir,inputArray[1]) != 0)
                    {
                        printf("\nshell: cd: %s: No such file or directory", inputArray[1]);
                    }
                }
            }
            else if(command == mkdr || command == mkdr_m)
            {
                int status = -1;
                if(command == mkdr_m)
                {
                    if(numArguments == 4){

                        __mode_t inputMode = strtol(inputArray[2], 0, 8);

                        if(inputMode != 0) {
                            status = mkdir(inputArray[3], inputMode);
                        }
                    }
                }
                else
                {
                    status = mkdir(inputArray[1], strtol("0777", 0, 8));
                }

                // check if directory is created or not
                if (status != -1)
                    printf("Directory has been created\n");
                else
                    printf("Unable to create directory\n");

            }
            else if(command == cp || command == cp_u){
                FILE* source;
                FILE* destination;
                char *str1,*str2;
                if(command == cp)
                {
                    str1 = inputArray[1];
                    str2 = inputArray[2];
                }
                else if(command == cp_u)
                {
                    str1 = inputArray[2];
                    str2 = inputArray[3];
                }
                char ch;
                source = fopen(str1, "r");
                destination = fopen(str2, "r");

                if(source == NULL){
                    printf("Invalid Input");
                    command = invalid;
                }
                else
                {
                    int isDifferent = 1;
                    if(command == cp_u){
                        if(destination != NULL){
                            isDifferent = isDifferentFrom(source, destination);
                            fseek(source, 0, SEEK_SET);
                        }
                    }

                    if(isDifferent)
                    {
                        fclose(destination);
                        destination = fopen(str2, "w+");
                        while((ch = fgetc(source)) != EOF) {
                            fputc(ch, destination);
                        }
                    }

                    fclose(source);
                    fclose(destination);
                }
            }
            else if(command == grep || command == grep_n) {

                FILE* file;
                char line[1024];
                char* strToSearch = inputArray[1];
                int startFileIdx = 2;
                if(command == grep_n){
                    strToSearch = inputArray[2];
                    startFileIdx = 3;
                }
                if(startFileIdx >= numArguments){
                    command = invalid;
                }
                else
                {
                    bool filesExist = doesFilesExist(inputArray, startFileIdx, numArguments - 1, "grep");
                    if(filesExist) {
                        int lineCount = 0;

                        //for each input files
                        for(int i = startFileIdx; i < numArguments; i++)
                        {
                            file = fopen(inputArray[i],"r");
                            while((fgets(line, sizeof(line), file)) != NULL){
                                lineCount++;
                                if(strstr(line, strToSearch)){
                                    if(command == grep_n){
                                        printf("%d: ", lineCount);
                                    }
                                    printf("%s\n", line);
                                }
                            }
                            fclose(file);
                            printf("\n");
                        }

                    }
                }
            }
            else if(command == sort || command == sort_r)
            {
                char **str;
                char line[10000];

                int startFileIdx = 1,index;
                int flag =0;
                for(int i=1;i<numArguments;i++)
                {
                    if(strcmp(inputArray[i],"-r")==0)
                    {
                        index = i;
                        flag =1;
                    }
                }
                /*if(command == cat_n){
                    startFileIdx = 2;
                }*/
                if(startFileIdx >= numArguments){
                    command = invalid;
                }
                else {
                    /* check files existance */
                    bool filesExist;
                    if(flag == 1)
                    {
                        filesExist = doesFilesExist(inputArray, startFileIdx, index - 1, "cat");
                        filesExist = doesFilesExist(inputArray, index+1, numArguments - 1, "cat");
                    }
                    else
                    {
                        filesExist = doesFilesExist(inputArray, startFileIdx, numArguments - 1, "sort");
                    }

                    if(filesExist)
                    {
                        for(int i = startFileIdx; i < numArguments; i++)
                        {
                            if(strcmp(inputArray[i],"-r")==0)
                            {
                                continue;
                            }
                            else
                            {
                                int lineCount = 0;
                            FILE *file = fopen(inputArray[i],"r");
                            if(file == NULL)
                            {
                                printf("\n Cannot open the file \n");
                                break;
                            }
                            while((fgets(line, sizeof(line), file)) != NULL)
                                lineCount++;
                            fseek(file,0,SEEK_SET);
                            sort_file(file,lineCount,command);
                            fclose(file);
                            }
                        }
                    }
                }
            }

            if(command == 0){
                printf("Invalid command!\n");
            }
        }

        //printf("\n%s", cwd);
        printf("\n");
        input = readline(cwd);
    }

    printf("Logged out... bye!\n");

    return 0;
}

bool doesFilesExist(char* inputArray[10], int startIdx, int endIdx, char* commandStr){
    /* check files existance */
    for (int i = startIdx; i <= endIdx; i++) {
        FILE* file;
        if((file = fopen(inputArray[i], "r")) == NULL){
            printf("%s: %s: No such file or directory\n", commandStr, inputArray[i]);
            return false;
        }
        fclose(file);
    }
    return true;
}

int isDifferentFrom(FILE* file1, FILE* file2)
{
    char ch1;
    char ch2;

    while((ch1 = fgetc(file1)) != EOF && (ch2 = fgetc(file2)) != EOF){
        if(ch1 != ch2){
            return 1;
        }
    }
    if(ch1 == ch2){
        return 0;
    }
    return 1;
}

void sort_file(FILE* file,int lines,int c)
{
    if (feof (file))
        return;

    char *str[lines];
    int i = 0;

    char line[lines][500];
    while(fgets(line[i],500,file) != NULL)
    {
        str[i] = line[i];
        i++;
    }

    if(c == sort)
        qsort(str, lines, sizeof(const char*), ascendingComparator);
    else if(c == sort_r)
        qsort(str, lines, sizeof(const char*), discendingComparator);

    for(i=0;i<lines;i++)
    {
        printf("%s\n",str[i]);
    }
}