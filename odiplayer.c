#include <stdio.h>
#include <stdlib.h>
#include "Players_data.h"

#define TEAM_COUNT 10
#define ROLE_COUNT 3
#define MAX_PLAYERS 50
#define NAME_LENGTH 51
#define BUFFER_SIZE 101
#define MAX_ID 1000
#define MAX_RUNS 50000
#define MAX_WICKETS 1000
#define MAX_AVERAGE 200
#define MAX_STRIKE_RATE 500
#define MAX_ECONOMY 36
#define LINE "=======================================================================================================\n"

typedef enum Role{
    BATSMAN=1,
    BOWLER=2,
    ALL_ROUNDER=3
}Role;

typedef struct ODIPlayer ODIPlayer;
typedef struct Team Team;

struct ODIPlayer{
    int playerId;
    char name[NAME_LENGTH];
    char teamName[NAME_LENGTH];
    Role role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    ODIPlayer *next;
    ODIPlayer *mergeNext;
};

struct Team{
    int teamId;
    char name[NAME_LENGTH];
    int totalPlayers;
    float averageBattingStrikeRate;
    float strikeRateSum;
    int battingCount;
    ODIPlayer *heads[ROLE_COUNT];
};

int readLine(char buffer[]);
int readInteger(const char *message,int min,int max);
float readFloat(const char *message,float max);
void readName(char name[]);
int isValidName(const char name[]);
void copyString(char destination[],const char source[],int size);
int stringsEqual(const char first[],const char second[]);
int getRole(const char text[]);
const char *getRoleName(Role role);
float calculatePerformanceIndex(ODIPlayer *player);
void initializeTeams(Team teamList[]);
void initializePlayers(Team teamList[]);
int findTeamByName(Team teamList[],const char name[]);
int findTeamById(Team teamList[],int teamId);
int playerIdExists(Team teamList[],int playerId);
void insertPlayer(Team *team,ODIPlayer *player);
void addPlayer(Team teamList[]);
void printTableHeader(void);
void printPlayer(ODIPlayer *player);
void displayTeamPlayers(Team teamList[]);
void displayTeamsByStrikeRate(Team teamList[]);
void displayTopKPlayers(Team teamList[]);
ODIPlayer *mergeTwoLists(ODIPlayer *first,ODIPlayer *second);
ODIPlayer *mergeTeams(Team teamList[],int low,int high,int role);
void displayPlayersByRole(Team teamList[]);
void freeAll(Team teamList[]);
int showMenu(void);

int main(void){
    Team teamList[TEAM_COUNT];
    int choice;

    initializeTeams(teamList);
    initializePlayers(teamList);

    do{
        choice=showMenu();
        switch(choice){
            case 1:
                addPlayer(teamList);
                break;
            case 2:
                displayTeamPlayers(teamList);
                break;
            case 3:
                displayTeamsByStrikeRate(teamList);
                break;
            case 4:
                displayTopKPlayers(teamList);
                break;
            case 5:
                displayPlayersByRole(teamList);
                break;
            case 6:
                printf("Exiting program.\n");
                break;
        }
    }while(choice!=6);

    freeAll(teamList);
    return 0;
}

int showMenu(void){
    printf("\n==============================================================\n");
    printf("            ICC ODI Player Performance Analyzer\n");
    printf("==============================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printf("==============================================================\n");
    return readInteger("Enter your choice: ",1,6);
}

int readLine(char buffer[]){
    char next='\n';
    int result;

    buffer[0]='\0';
    result=scanf("%100[^\n]%c",buffer,&next);
    if(result==EOF){
        printf("\nInput ended. Exiting program.\n");
        exit(0);
    }
    if(result==0){
        result=scanf("%c",&next);
        return 1;
    }
    if(result==2&&next!='\n'){
        result=scanf("%*[^\n]%*c");
        return 0;
    }
    return 1;
}

int readInteger(const char *message,int min,int max){
    char buffer[BUFFER_SIZE];
    int number,i,valid;

    while(1){
        printf("%s",message);
        number=0;
        valid=readLine(buffer);
        if(buffer[0]=='\0'){
            valid=0;
        }
        for(i=0;valid&&buffer[i]!='\0';i++){
            if(buffer[i]>='0'&&buffer[i]<='9'){
                number=number*10+(buffer[i]-'0');
                if(number>max){
                    valid=0;
                }
            }else{
                valid=0;
            }
        }
        if(valid&&number>=min&&number<=max){
            return number;
        }
        printf("Invalid input. Enter a whole number between %d and %d.\n",min,max);
    }
}

float readFloat(const char *message,float max){
    char buffer[BUFFER_SIZE];
    double number,fraction;
    int i,digits,pointSeen,valid;

    while(1){
        printf("%s",message);
        number=0;
        fraction=0.1;
        digits=0;
        pointSeen=0;
        valid=readLine(buffer);
        for(i=0;valid&&buffer[i]!='\0';i++){
            if(buffer[i]>='0'&&buffer[i]<='9'){
                digits++;
                if(pointSeen==0){
                    number=number*10+(buffer[i]-'0');
                }else{
                    number=number+(buffer[i]-'0')*fraction;
                    fraction=fraction/10;
                }
                if(number>max){
                    valid=0;
                }
            }else if(buffer[i]=='.'&&pointSeen==0){
                pointSeen=1;
            }else{
                valid=0;
            }
        }
        if(valid&&digits>0){
            return (float)number;
        }
        printf("Invalid input. Enter a number between 0 and %.0f (example 45.5).\n",max);
    }
}

void readName(char name[]){
    char buffer[BUFFER_SIZE];

    while(1){
        printf("Name: ");
        if(readLine(buffer)&&isValidName(buffer)){
            copyString(name,buffer,NAME_LENGTH);
            return;
        }
        printf("Invalid name. Use 1 to 50 characters: letters, spaces, '.' and '-' only.\n");
    }
}

int isValidName(const char name[]){
    int i;

    for(i=0;name[i]!='\0';i++){
        if((name[i]>='a'&&name[i]<='z')||(name[i]>='A'&&name[i]<='Z')){
            continue;
        }
        if(i>0&&(name[i]==' '||name[i]=='.'||name[i]=='-')){
            continue;
        }
        return 0;
    }
    return i>0&&i<NAME_LENGTH;
}

void copyString(char destination[],const char source[],int size){
    int i=0;

    while(source[i]!='\0'&&i<size-1){
        destination[i]=source[i];
        i++;
    }
    destination[i]='\0';
}

int stringsEqual(const char first[],const char second[]){
    int i=0;

    while(first[i]!='\0'&&first[i]==second[i]){
        i++;
    }
    return first[i]==second[i];
}

int getRole(const char text[]){
    if(stringsEqual(text,"Batsman")){
        return BATSMAN;
    }
    if(stringsEqual(text,"Bowler")){
        return BOWLER;
    }
    if(stringsEqual(text,"All-rounder")){
        return ALL_ROUNDER;
    }
    return 0;
}

const char *getRoleName(Role role){
    if(role==BATSMAN){
        return "Batsman";
    }
    if(role==BOWLER){
        return "Bowler";
    }
    return "All-Rounder";
}

float calculatePerformanceIndex(ODIPlayer *player){
    if(player->role==BATSMAN){
        return (player->battingAverage*player->strikeRate)/100;
    }
    if(player->role==BOWLER){
        return (player->wickets*2)+(100-player->economyRate);
    }
    return ((player->battingAverage*player->strikeRate)/100)+(player->wickets*2);
}

void initializeTeams(Team teamList[]){
    const char *teamOrder[TEAM_COUNT]={"India","Australia","England","Pakistan","New Zealand","South Africa","Bangladesh","Sri Lanka","Afghanistan","West Indies"};
    int i,j;

    for(i=0;i<TEAM_COUNT;i++){
        teamList[i].teamId=i+1;
        copyString(teamList[i].name,teamOrder[i],NAME_LENGTH);
        teamList[i].totalPlayers=0;
        teamList[i].averageBattingStrikeRate=0;
        teamList[i].strikeRateSum=0;
        teamList[i].battingCount=0;
        for(j=0;j<ROLE_COUNT;j++){
            teamList[i].heads[j]=NULL;
        }
    }
}

void initializePlayers(Team teamList[]){
    int i,teamIndex,role;
    ODIPlayer *newPlayer;

    for(i=0;i<playerCount;i++){
        teamIndex=findTeamByName(teamList,players[i].team);
        role=getRole(players[i].role);
        if(teamIndex==-1||role==0||teamList[teamIndex].totalPlayers>=MAX_PLAYERS||playerIdExists(teamList,players[i].id)){
            continue;
        }
        newPlayer=(ODIPlayer *)malloc(sizeof(ODIPlayer));
        if(newPlayer==NULL){
            printf("Memory allocation failed while loading players.\n");
            return;
        }
        newPlayer->playerId=players[i].id;
        copyString(newPlayer->name,players[i].name,NAME_LENGTH);
        copyString(newPlayer->teamName,players[i].team,NAME_LENGTH);
        newPlayer->role=(Role)role;
        newPlayer->totalRuns=players[i].totalRuns;
        newPlayer->battingAverage=players[i].battingAverage;
        newPlayer->strikeRate=players[i].strikeRate;
        newPlayer->wickets=players[i].wickets;
        newPlayer->economyRate=players[i].economyRate;
        newPlayer->performanceIndex=calculatePerformanceIndex(newPlayer);
        insertPlayer(&teamList[teamIndex],newPlayer);
    }
}

int findTeamByName(Team teamList[],const char name[]){
    int i;

    for(i=0;i<TEAM_COUNT;i++){
        if(stringsEqual(teamList[i].name,name)){
            return i;
        }
    }
    return -1;
}

int findTeamById(Team teamList[],int teamId){
    int low=0,high=TEAM_COUNT-1,mid;

    while(low<=high){
        mid=(low+high)/2;
        if(teamList[mid].teamId==teamId){
            return mid;
        }
        if(teamList[mid].teamId<teamId){
            low=mid+1;
        }else{
            high=mid-1;
        }
    }
    return -1;
}

int playerIdExists(Team teamList[],int playerId){
    int i,j;
    ODIPlayer *temp;

    for(i=0;i<TEAM_COUNT;i++){
        for(j=0;j<ROLE_COUNT;j++){
            temp=teamList[i].heads[j];
            while(temp!=NULL){
                if(temp->playerId==playerId){
                    return 1;
                }
                temp=temp->next;
            }
        }
    }
    return 0;
}

void insertPlayer(Team *team,ODIPlayer *player){
    int roleIndex=player->role-1;
    ODIPlayer *temp=team->heads[roleIndex];

    if(temp==NULL||player->performanceIndex>temp->performanceIndex){
        player->next=temp;
        team->heads[roleIndex]=player;
    }else{
        while(temp->next!=NULL&&temp->next->performanceIndex>=player->performanceIndex){
            temp=temp->next;
        }
        player->next=temp->next;
        temp->next=player;
    }

    team->totalPlayers++;
    if(player->role==BATSMAN||player->role==ALL_ROUNDER){
        team->strikeRateSum+=player->strikeRate;
        team->battingCount++;
        team->averageBattingStrikeRate=team->strikeRateSum/team->battingCount;
    }
}

void addPlayer(Team teamList[]){
    int teamId,index;
    ODIPlayer *newPlayer;

    teamId=readInteger("Enter Team ID to add player: ",1,MAX_ID);
    index=findTeamById(teamList,teamId);
    if(index==-1){
        printf("Team not found.\n");
        return;
    }
    if(teamList[index].totalPlayers>=MAX_PLAYERS){
        printf("Team %s already has %d players. Cannot add more.\n",teamList[index].name,MAX_PLAYERS);
        return;
    }

    newPlayer=(ODIPlayer *)malloc(sizeof(ODIPlayer));
    if(newPlayer==NULL){
        printf("Memory allocation failed.\n");
        return;
    }

    printf("Enter Player Details:\n");
    newPlayer->playerId=readInteger("Player ID: ",1,MAX_ID);
    while(playerIdExists(teamList,newPlayer->playerId)){
        printf("Player ID already exists. Enter a different ID.\n");
        newPlayer->playerId=readInteger("Player ID: ",1,MAX_ID);
    }
    readName(newPlayer->name);
    newPlayer->role=(Role)readInteger("Role (1-Batsman, 2-Bowler, 3-All-rounder): ",BATSMAN,ALL_ROUNDER);
    newPlayer->totalRuns=readInteger("Total Runs: ",0,MAX_RUNS);
    newPlayer->battingAverage=readFloat("Batting Average: ",MAX_AVERAGE);
    newPlayer->strikeRate=readFloat("Strike Rate: ",MAX_STRIKE_RATE);
    newPlayer->wickets=readInteger("Wickets: ",0,MAX_WICKETS);
    newPlayer->economyRate=readFloat("Economy Rate: ",MAX_ECONOMY);
    copyString(newPlayer->teamName,teamList[index].name,NAME_LENGTH);
    newPlayer->performanceIndex=calculatePerformanceIndex(newPlayer);
    insertPlayer(&teamList[index],newPlayer);

    printf("Player added successfully to Team %s!\n",teamList[index].name);
}

void printTableHeader(void){
    printf(LINE);
    printf("%-6s %-22s %-13s %-12s %-7s %-7s %-7s %-6s %-6s %-8s\n","ID","Name","Team","Role","Runs","Avg","SR","Wkts","ER","Perf.Idx");
    printf(LINE);
}

void printPlayer(ODIPlayer *player){
    printf("%-6d %-22s %-13s %-12s %-7d %-7.1f %-7.1f %-6d %-6.1f %-8.2f\n",player->playerId,player->name,
           player->teamName,getRoleName(player->role),player->totalRuns,player->battingAverage,
           player->strikeRate,player->wickets,player->economyRate,player->performanceIndex);
}

void displayTeamPlayers(Team teamList[]){
    int teamId,index,i;
    ODIPlayer *temp;

    teamId=readInteger("Enter Team ID: ",1,MAX_ID);
    index=findTeamById(teamList,teamId);
    if(index==-1){
        printf("Team not found.\n");
        return;
    }

    printf("Players of Team %s:\n",teamList[index].name);
    printTableHeader();
    for(i=0;i<ROLE_COUNT;i++){
        temp=teamList[index].heads[i];
        while(temp!=NULL){
            printPlayer(temp);
            temp=temp->next;
        }
    }
    printf(LINE);
    printf("Total Players: %d\n",teamList[index].totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n",teamList[index].averageBattingStrikeRate);
}

void displayTeamsByStrikeRate(Team teamList[]){
    Team *sorted[TEAM_COUNT],*swap;
    int i,j;

    for(i=0;i<TEAM_COUNT;i++){
        sorted[i]=&teamList[i];
    }
    for(i=0;i<TEAM_COUNT-1;i++){
        for(j=0;j<TEAM_COUNT-i-1;j++){
            if(sorted[j]->averageBattingStrikeRate<sorted[j+1]->averageBattingStrikeRate){
                swap=sorted[j];
                sorted[j]=sorted[j+1];
                sorted[j+1]=swap;
            }
        }
    }

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("==========================================================\n");
    printf("%-5s %-20s %-12s %-12s\n","ID","Team Name","Avg Bat SR","Total Players");
    printf("==========================================================\n");
    for(i=0;i<TEAM_COUNT;i++){
        printf("%-5d %-20s %-12.2f %-12d\n",sorted[i]->teamId,sorted[i]->name,
               sorted[i]->averageBattingStrikeRate,sorted[i]->totalPlayers);
    }
    printf("==========================================================\n");
}

void displayTopKPlayers(Team teamList[]){
    int teamId,index,k,shown=0;
    Role role;
    ODIPlayer *temp;

    teamId=readInteger("Enter Team ID: ",1,MAX_ID);
    index=findTeamById(teamList,teamId);
    if(index==-1){
        printf("Team not found.\n");
        return;
    }
    role=(Role)readInteger("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ",BATSMAN,ALL_ROUNDER);
    k=readInteger("Enter number of players: ",1,MAX_PLAYERS);

    temp=teamList[index].heads[role-1];
    if(temp==NULL){
        printf("No %s found in Team %s.\n",getRoleName(role),teamList[index].name);
        return;
    }

    if(role==BATSMAN){
        printf("Top %d Batsmen of Team %s:\n",k,teamList[index].name);
    }else if(role==BOWLER){
        printf("Top %d Bowlers of Team %s:\n",k,teamList[index].name);
    }else{
        printf("Top %d All-rounders of Team %s:\n",k,teamList[index].name);
    }
    printTableHeader();
    while(temp!=NULL&&shown<k){
        printPlayer(temp);
        temp=temp->next;
        shown++;
    }
    printf(LINE);
    if(shown<k){
        printf("Only %d %s(s) available in this team.\n",shown,getRoleName(role));
    }
}

ODIPlayer *mergeTwoLists(ODIPlayer *first,ODIPlayer *second){
    if(first==NULL){
        return second;
    }
    if(second==NULL){
        return first;
    }
    if(first->performanceIndex>=second->performanceIndex){
        first->mergeNext=mergeTwoLists(first->mergeNext,second);
        return first;
    }
    second->mergeNext=mergeTwoLists(first,second->mergeNext);
    return second;
}

ODIPlayer *mergeTeams(Team teamList[],int low,int high,int role){
    int mid;

    if(low==high){
        return teamList[low].heads[role-1];
    }
    mid=(low+high)/2;
    return mergeTwoLists(mergeTeams(teamList,low,mid,role),mergeTeams(teamList,mid+1,high,role));
}

void displayPlayersByRole(Team teamList[]){
    Role role;
    int i;
    ODIPlayer *temp;

    role=(Role)readInteger("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ",BATSMAN,ALL_ROUNDER);

    for(i=0;i<TEAM_COUNT;i++){
        temp=teamList[i].heads[role-1];
        while(temp!=NULL){
            temp->mergeNext=temp->next;
            temp=temp->next;
        }
    }

    temp=mergeTeams(teamList,0,TEAM_COUNT-1,role);
    if(temp==NULL){
        printf("No %s found in any team.\n",getRoleName(role));
        return;
    }

    if(role==BATSMAN){
        printf("Batsmen of all teams:\n");
    }else if(role==BOWLER){
        printf("Bowlers of all teams:\n");
    }else{
        printf("All-rounders of all teams:\n");
    }
    printTableHeader();
    while(temp!=NULL){
        printPlayer(temp);
        temp=temp->mergeNext;
    }
    printf(LINE);
}

void freeAll(Team teamList[]){
    int i,j;
    ODIPlayer *temp,*nextPlayer;

    for(i=0;i<TEAM_COUNT;i++){
        for(j=0;j<ROLE_COUNT;j++){
            temp=teamList[i].heads[j];
            while(temp!=NULL){
                nextPlayer=temp->next;
                free(temp);
                temp=nextPlayer;
            }
        }
    }
}