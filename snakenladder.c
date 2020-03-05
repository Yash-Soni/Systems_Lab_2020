#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include<signal.h>
#include<time.h>
#include<stdbool.h>

void init_board_manager(pid_t*, int, int, int*, int*, int[][2], int[][2]);
void init_player(int pos, int[][2], int[][2]);
bool read_inputs_and_validate(char*, int*, int*, int, int);
void display_positions(int*, int);

int main(int argc, char *argv[])
{

    if(argc < 4 || argc > 4){
        printf("\nInput is not valid!\n");
        return 1;
    }
    int boardSize = atoi(argv[1]);
    int numPlayer = atoi(argv[2]);

    if(boardSize == 0 || numPlayer == 0){
        printf("\nPlease enter valid board and players\n");
        return 1;
    }

    int board[boardSize];
    int positions[numPlayer];

    bool canProceed = read_inputs_and_validate(argv[3], board, positions, boardSize, numPlayer);
    if(!canProceed){
        return 1;
    }

	pid_t p, c = 1;

    int num = 0;
    pid_t childPids[numPlayer];//stores child pids
    int pipe1[numPlayer][2];
    int pipe2[numPlayer][2];

    //opening all pipes
    for (int i = 0; i < numPlayer; i++)
    {
        if(pipe(pipe1[i]) == -1){
            printf("Pipe creation failed\n");
            return 1;
        }
        if(pipe(pipe2[i]) == -1){
            printf("Pipe creation failed\n");
            return 1;
        }
    }

    //create all the child players
    while(c > 0 && num < numPlayer){
        c = fork();
        childPids[num] = c;
        if(c < 0){
            perror("child fork failed");
            break;
        }
        if(c == 0){
            break;
        }
        num++;
    }

    //parent
    if(c > 0) {
        //srand(time(0));//random seed
        init_board_manager(childPids, numPlayer, boardSize, board, positions, pipe1, pipe2);
    }
    //child
    else if(c == 0){
        init_player(num, pipe1, pipe2);
    }
}

bool read_inputs_and_validate(char* fileName, int* board, int* positions, int boardSize, int numPlayer){
    char positionType;
    int startPos = -1, endPos = -1;
    bool fileHasValidInput = false;

    FILE* inputFile = fopen(fileName, "r");
    if(inputFile == NULL){
        printf("\nInput File does not exist\n");
        return false;
    }

    //initialize board with 0 values
    for(int i = 1; i <= boardSize; i++){
        board[i] = 0;
    }

    //initalize player with 0 position
    for(int i = 0; i < numPlayer; i++){
        positions[i] = 0;
    }

    while(fscanf(inputFile, "%c %d %d\n", &positionType, &startPos, &endPos) == 3){
        if((positionType != 'L' && positionType != 'S') || startPos <= 1 || endPos <= 1 || startPos > boardSize || endPos > boardSize){
            fileHasValidInput = false;
            break;
        }
        if(!fileHasValidInput){
            fileHasValidInput = true;//making true to know the file has valid input
        }
        // changes done here
        board[startPos ] = endPos ; // -1 to make it compatible with array index
        printf("%c %d %d\n", positionType, startPos, endPos);
    }
    if(!fileHasValidInput || !feof(inputFile)){
        printf("\nFile does not contain valid input\n");
        fclose(inputFile);
        return false;
    }
    fclose(inputFile);
    return true;
}

void init_board_manager(pid_t* childPids, int numPlayer, int boardSize, int* board, int* positions, int pipe1[][2], int pipe2[][2]){
    int status = 1;

    //Game start here

    bool gameFinished = false;
    int count = 0;
    int returnValue = 0;
    //int player = (rand() % (numPlayer));
    srand(time(0));
    int player = (rand() % (numPlayer));
    printf("First player selected is : %d\n", player);
    while(!gameFinished){
        player = player % numPlayer;
        //positions[player] += returnValue;
        //printf("recieved %d from %d\n", returnValue, player);

        do {
            printf("\nTurn passed to player %d \n", player+1);
            sleep(1);
            write(pipe1[player][1],&childPids[player],sizeof(pid_t));

            while(read(pipe2[player][0],&returnValue,sizeof(int)) == -1);

            positions[player] += returnValue;
            printf("Player %d generated value %d\n", player+1, returnValue);
            if(positions[player] < boardSize)
            do
            {
                if((board[positions[player]] != 0)&&(board[positions[player]] < positions[player])){
                    positions[player] = board[positions[player]];
                    printf("OOPS!!! Snake has bitten player %d, the player moved to the position %d\n",player+1,positions[player]);
                }
                else if(board[positions[player]] > positions[player]){
                    positions[player] = board[positions[player]];
                    printf("That's AMAZING!!! player %d climbed up the ladder to the position %d\n",player+1,positions[player]);
                }

            }while((board[positions[player]] != 0)&&(positions[player] < boardSize));
            sleep(1);
            display_positions(positions, numPlayer);
        } while(returnValue == 6 && positions[player] < boardSize);

        if(positions[player] >= boardSize) gameFinished = true;
        if(gameFinished == true)
        {
            printf("Player %d is the winner!!!\n",player+1);
        }
        player++;
    }

    for(int i = 0; i < numPlayer; i++){
        pid_t exitCode = 0;
        write(pipe1[i][1],&exitCode,sizeof(pid_t));
        waitpid(childPids[i],&status, 0);
    }
    //closing all pipes
    for (int i = 0; i < numPlayer; i++)
    {
        close(pipe1[i][0]);
        close(pipe1[i][1]);
        close(pipe2[i][0]);
        close(pipe2[i][1]);
    }

    //printf("parent ended with child %d, parent pid : %d\n", numPlayer, getpid());
    printf("\n GAME OVER!!!\n");
}

void init_player(int playerNum, int pipe1[][2], int pipe2[][2]){
    //printf("%d. child running with pid %d\n", playerNum, getpid());

    pid_t readPid = 1, exitCode = 0;
    //run until parent sends exit code
    while(readPid != exitCode) {
        close(pipe1[playerNum][1]);
        while(read(pipe1[playerNum][0], &readPid, sizeof(pid_t)) == -1);

        if(readPid == getpid()){
            //my turn
            //int randValue = (rand() % 6) + 1;
            srand(time(0) + rand() % 15);
            int returnValue = (rand() % 6) + 1;
            //printf("value generated : %d\n",returnValue);
            write(pipe2[playerNum][1], &returnValue,sizeof(int));
        }
    }
    //printf("   child exiting with pid %d\n\n", getpid());
}

void display_positions(int* positions, int numPlayer){
    printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    for(int i = 0; i < numPlayer; i++){
        printf("Player %d is at board position %d\n", (i + 1), positions[i]);
    }
    printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}