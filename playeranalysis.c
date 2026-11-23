#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<errno.h>

#include "Players_data.h"

#define MAX_TEAMS 10
#define MAX_NAME 51
#define MAX_PLAYERS_PER_TEAM 50


typedef enum {
    INVALID_ROLE =0,
    BATSMAN = 1,
    BOWLER = 2,
    ALL_ROUNDER = 3
} Role;

typedef struct ODIPlayer
{
    int playerid;
    char name[MAX_NAME];
    char teamName[MAX_NAME];
    Role role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct ODIPlayer *next;
} ODIPlayer;

typedef struct {
    int teamId;
    char name[MAX_NAME];
    int totalPlayers;
    float averageBattingStrikerate;
    ODIPlayer *batsmen;
    ODIPlayer *bowlers;
    ODIPlayer *allRounders;
}Team;

int readInt(const char *message, int *value){

    printf("%s", message);
    if(scanf("%d", value)!=1){
        printf("Invalid input!\n");
        int ch;
        while((ch = getchar()) !='\n' && ch != EOF);
        return 0;
    }
    int ch;
    while((ch = getchar()) !='\n' && ch != EOF);
    return 1;

}

int readFloat(const char *message, float *value){
    printf("%s", message);
    if(scanf("%f", value)!=1){
        printf("Invalid input!\n");
        int ch;
        while((ch = getchar()) !='\n' && ch != EOF);
        return 0;
    }
    int ch;
    while((ch = getchar()) !='\n' && ch != EOF);
    return 1;
}

int readString(const char *message, char *value, int size){
    printf("%s", message);
    if(fgets(value, size, stdin)==NULL) return 0;
    value[strcspn(value,"\n")]='\0';
    if (strlen(value) == 0) {
        printf("Name cannot be empty!\n");
        return 0;
    }
    return 1;
}
Role getRole(const char *roleName){
    if(strcmp(roleName,"Batsman")==0) return BATSMAN;
    else if(strcmp(roleName,"Bowler")==0) return BOWLER;
    else if(strcmp(roleName,"All-rounder")==0) return ALL_ROUNDER;
    return INVALID_ROLE;
}
const char* getRoleName(Role role){
    if(role == BATSMAN) return "Batsman";
    else if(role == BOWLER) return "Bowler";
    else if(role == ALL_ROUNDER) return "All-rounder";

    return "Unknown";
}
Role getRoleFromChoice(int choice){
    if(choice == 1) return BATSMAN;
    if(choice ==2 )return BOWLER;
    if(choice == 3) return ALL_ROUNDER;

    return INVALID_ROLE;
}
float calculatePerformanceIndex(ODIPlayer *p){
    if(p->role == BATSMAN) return (p->battingAverage * p->strikeRate)/100.0;
    else if(p->role == BOWLER) return (p->wickets * 2)+(100.0-p->economyRate);
    else if(p->role == ALL_ROUNDER) return ((p->battingAverage * p->strikeRate)/100.0)+(p->wickets*2);
    return 0.0;
}
void initializeTeams(Team teamList[]){
    for(int i=0; i < teamCount ;i++){
        teamList[i].teamId = i+1;
        strcpy(teamList[i].name, teams[i]);

        teamList[i].totalPlayers =0;
        teamList[i].averageBattingStrikerate =0.0;
        teamList[i].batsmen =NULL;
        teamList[i].bowlers = NULL;
        teamList[i].allRounders = NULL;
    }
}

ODIPlayer* createPlayerNode(const Player *source){
    ODIPlayer *newNode = malloc(sizeof(ODIPlayer));
    if(newNode ==NULL) return NULL;

    newNode->playerid = source->id;
    strcpy(newNode->name, source->name);
    strcpy(newNode->teamName, source->team);
    newNode->role = getRole(source->role);

    newNode->totalRuns = source->totalRuns;
    newNode->battingAverage = source->battingAverage;
    newNode->strikeRate = source->strikeRate;
    newNode->wickets = source->wickets;
    newNode->economyRate = source->economyRate;
    newNode->performanceIndex = calculatePerformanceIndex(newNode);
    newNode->next = NULL;
    return newNode;
    
}

void insertSorted(ODIPlayer **head, ODIPlayer *newNode){
    if(*head == NULL || (*head)->performanceIndex < newNode->performanceIndex){
        newNode->next = *head;
        *head = newNode;
        return;
    }
    ODIPlayer *temp = *head;
    while(temp->next != NULL && temp->next->performanceIndex >= newNode->performanceIndex){
        temp = temp->next;
    }
    newNode->next = temp->next;
    temp->next = newNode;
}

Team* findTeamByName(Team teamList[], const char *teamName){
    for(int i=0; i < teamCount ;i++){
        if(strcmp(teamList[i].name, teamName)==0) return &teamList[i];
    }
    return NULL;
}

void addPlayerToTeam(Team *team, ODIPlayer *newNode){
    if(newNode->role == BATSMAN){
        insertSorted(&team->batsmen, newNode);
    } else if(newNode->role == BOWLER){
        insertSorted(&team->bowlers, newNode);
    }
    else if(newNode->role == ALL_ROUNDER){
        insertSorted(&team->allRounders,newNode);
    }
    team->totalPlayers++;
}

float calculateAverageStrikeRate(Team *team){
    float sum = 0.0f;
    int count = 0;

    ODIPlayer *temp = team->batsmen;
    while(temp!= NULL){
        sum+= temp->strikeRate;
        count++;
        temp = temp->next;
    }

    temp = team->allRounders;
    while(temp!= NULL){
        sum+= temp->strikeRate;
        count++;
        temp = temp->next;
    }

    if(count==0) return 0.0;
    return sum/count;
}

int initializePlayers(Team teamList[]){
    for(int i =0; i <playerCount; i++){
        Team *team = findTeamByName(teamList, players[i].team);
        if(team == NULL) return 0;

        ODIPlayer *newNode = createPlayerNode(&players[i]);
        if(newNode == NULL) return 0;

        addPlayerToTeam(team, newNode);
    }

    for(int i=0; i < teamCount;i++){
        teamList[i].averageBattingStrikerate = calculateAverageStrikeRate(&teamList[i]);
    }
    return 1;
}

Team* findTeamById(Team teamList[], int id){
    int low =0;
    int high = teamCount-1;
    while(low<=high){
        int mid = low +(high-low)/2;
        if(teamList[mid].teamId==id) return &teamList[mid];
        else if(teamList[mid].teamId<id) low = mid+1;
        else high = mid-1;
    }
    return NULL;
}

int playerIdExistsInList(ODIPlayer *head, int id){
    while(head != NULL){
        if(head->playerid == id) return 1;
        head = head ->next;
    }
    return 0;
}

int playerIdExists(Team teamList[], int id) {
    for (int i = 0; i < teamCount; i++) {
        if (playerIdExistsInList(teamList[i].batsmen, id) ||
            playerIdExistsInList(teamList[i].bowlers, id) ||
            playerIdExistsInList(teamList[i].allRounders, id)) {

            return 1;
        }
    }
    return 0;
}

void printplayerHeader(){
    printf("\n================================================================================================================\n");

    printf("%-6s %-22s %-14s %-10s %-10s %-10s %-8s %-8s %-12s\n","ID","Name","Role","Runs","Average","SR","Wickets","Economy","Perf.Index");
    printf("================================================================================================================\n");

}
void displayPlayerList(ODIPlayer *head){
    ODIPlayer *temp = head;
    while(temp!= NULL){
        printf("%-6d %-22s %-14s %-10d %-10.2f %-10.2f %-8d %-8.2f %-12.2f\n", temp->playerid, temp->name, getRoleName(temp->role), temp->totalRuns, temp->battingAverage, temp->strikeRate, temp->wickets, temp->economyRate, temp->performanceIndex);
        temp = temp->next;

    }
}

void displayTeamPlayers(Team *team){
    printf("\nPlayers of Team %s\n", team->name);
    printplayerHeader();
    displayPlayerList(team->batsmen);
    displayPlayerList(team->bowlers);
    displayPlayerList(team->allRounders);

     printf("================================================================================================================\n");

    printf("Total Players: %d\n", team->totalPlayers);

    printf("Average Batting Strike Rate: %.2f\n",

           team->averageBattingStrikerate);
}
void freePlayerList(ODIPlayer *head){
    ODIPlayer *temp;
    while(head!=NULL){
        temp = head;
        head = head->next;
        free(temp);
    }
}

void addNewPlayer(Team teamList[]){
    int teamId;
    int playerId;
    int roleChoice;

    Team *team;
     printf("\n========== ADD NEW PLAYER ==========\n");

    if (!readInt("Enter Team ID (1-10): ", &teamId)) {
        return;
    }
    team = findTeamById(teamList, teamId);
    if (team == NULL) {
        printf("Invalid Team ID!\n");
        return;
    }

    if (team->totalPlayers >= MAX_PLAYERS_PER_TEAM) {
        printf("Team already has 50 players!\n");
        return;
    }

    if (!readInt("Enter Player ID (1-1000): ", &playerId)) {
        return;
    }

    if (playerId < 1 || playerId > 1000) {
        printf("Player ID must be between 1 and 1000!\n");
        return;
    }

    if (playerIdExists(teamList, playerId)) {
        printf("Player ID already exists!\n");
        return;
    }

    ODIPlayer *newNode = malloc(sizeof(ODIPlayer));

    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    newNode->playerid = playerId;

    if (!readString("Enter Player Name: ",
                    newNode->name, MAX_NAME)) {

        free(newNode);
        return;
    }

    strcpy(newNode->teamName, team->name);

    printf("\nSelect Role:\n");

    printf("1. Batsman\n");
    printf("2. Bowler\n");
    printf("3. All-rounder\n");

    if (!readInt("Enter role choice: ", &roleChoice)) {
        free(newNode);
        return;
    }

    newNode->role = getRoleFromChoice(roleChoice);

    if (newNode->role == INVALID_ROLE) {
        printf("Invalid role!\n");
        free(newNode);
        return;
    }

    if (!readInt("Enter Total Runs: ", &newNode->totalRuns) ||
        !readFloat("Enter Batting Average: ", &newNode->battingAverage) ||
        !readFloat("Enter Strike Rate: ", &newNode->strikeRate) ||
        !readInt("Enter Wickets: ", &newNode->wickets) ||
        !readFloat("Enter Economy Rate: ", &newNode->economyRate)) {

        free(newNode);
        return;
    }

    if (newNode->totalRuns < 0 ||
        newNode->battingAverage < 0 ||
        newNode->strikeRate < 0 ||
        newNode->wickets < 0 ||
        newNode->economyRate < 0) {

        printf("Statistics cannot be negative!\n");

        free(newNode);

        return;
    }

    newNode->performanceIndex =
        calculatePerformanceIndex(newNode);

    newNode->next = NULL;

    addPlayerToTeam(team, newNode);

    team->averageBattingStrikerate =
        calculateAverageStrikeRate(team);

    printf("\nPlayer added successfully!\n");

    printf("Player Name: %s\n", newNode->name);

    printf("Team: %s\n", team->name);

    printf("Performance Index: %.2f\n",
           newNode->performanceIndex);
}

void optionDisplayTeam(Team teamList[]){
    int id;
    if(!readInt("\n Enter Team ID(1-10): ", &id)) return ;

    Team *team = findTeamById(teamList, id);
    if(team == NULL){
        printf("Invalid team ID !\n");
        return;
    }
    displayTeamPlayers(team);
}

void displayTeamsByAverageSR(Team teamList[]) {

    Team sortedTeams[MAX_TEAMS];

    for (int i = 0; i < teamCount; i++) {
        sortedTeams[i] = teamList[i];
    }

    for (int i = 0; i < teamCount - 1; i++) {

        int maxIndex = i;

        for (int j = i + 1; j < teamCount; j++) {

            if (sortedTeams[j].averageBattingStrikerate >
                sortedTeams[maxIndex].averageBattingStrikerate) {

                maxIndex = j;
            }
        }

        if (maxIndex != i) {

            Team temp = sortedTeams[i];

            sortedTeams[i] = sortedTeams[maxIndex];

            sortedTeams[maxIndex] = temp;
        }
    }

    printf("\n======= TEAMS BY AVERAGE BATTING STRIKE RATE =======\n");

    printf("\n%-8s %-22s %-15s %-20s\n",
           "Team ID", "Team Name",
           "Total Players", "Average SR");

    printf("-----------------------------------------------------------------\n");

    for (int i = 0; i < teamCount; i++) {

        printf("%-8d %-22s %-15d %-20.2f\n",
               sortedTeams[i].teamId,
               sortedTeams[i].name,
               sortedTeams[i].totalPlayers,
               sortedTeams[i].averageBattingStrikerate);
    }
}
void displayTopKPlayers(Team *team){
    int roleChoice, k, shown = 0;
    Role role;
    ODIPlayer *current;

    if (!readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", &roleChoice))
        return;

    role = getRoleFromChoice(roleChoice);
    if (role == INVALID_ROLE) {
        printf("Invalid role!\n");
        return;
    }

    if (!readInt("Enter number of players (K): ", &k))
        return;
    if (k <= 0) {
        printf("K must be greater than 0.\n");
        return;
    }

    if (role == BATSMAN) current = team->batsmen;
    else if (role == BOWLER) current = team->bowlers;
    else current = team->allRounders;

    printf("\nTop %d %s players of Team %s:\n", k, getRoleName(role), team->name);
    printplayerHeader();

    while (current != NULL && shown < k) {
        printf("%-6d %-22s %-14s %-10d %-10.2f %-10.2f %-8d %-8.2f %-12.2f\n",
               current->playerid, current->name, getRoleName(current->role),
               current->totalRuns, current->battingAverage, current->strikeRate,
               current->wickets, current->economyRate, current->performanceIndex);
        current = current->next;
        shown++;
    }

    printf("================================================================================================================\n");
    if (shown == 0) printf("No players found for this role.\n");
    else printf("Displayed %d player(s).\n", shown);
}

void heapPush(ODIPlayer *heap[], int *size, ODIPlayer *node){
    int i = (*size)++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap[parent]->performanceIndex >= node->performanceIndex) break;
        heap[i] = heap[parent];
        i = parent;
    }
    heap[i] = node;
}
ODIPlayer* heapPop(ODIPlayer *heap[], int *size) {
    ODIPlayer *result = heap[0];
    ODIPlayer *last = heap[--(*size)];
    int i = 0;

    if (*size == 0) return result;

    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest;

        if (left >= *size) break;
        largest = left;
        if (right < *size &&
            heap[right]->performanceIndex > heap[left]->performanceIndex)
            largest = right;

        if (last->performanceIndex >= heap[largest]->performanceIndex) break;
        heap[i] = heap[largest];
        i = largest;
    }
    heap[i] = last;
    return result;
}

void displayAllPlayersOfRole(Team teamList[]) {
    int roleChoice;
    int heapSize = 0;
    ODIPlayer *heap[MAX_TEAMS];
    ODIPlayer *current;
    Role role;

    if (!readInt("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", &roleChoice))
        return;

    role = getRoleFromChoice(roleChoice);
    if (role == INVALID_ROLE) {
        printf("Invalid role!\n");
        return;
    }

    for (int i = 0; i < teamCount; i++) {
        if (role == BATSMAN) current = teamList[i].batsmen;
        else if (role == BOWLER) current = teamList[i].bowlers;
        else current = teamList[i].allRounders;

        if (current != NULL)
            heapPush(heap, &heapSize, current);
    }

    printf("\nAll %s players across all teams:\n", getRoleName(role));
    printf("\n===============================================================================================================================\n");
    printf("%-6s %-22s %-18s %-14s %-10s %-10s %-10s %-8s %-8s %-12s\n",
           "ID", "Name", "Team", "Role", "Runs", "Average", "SR",
           "Wickets", "Economy", "Perf.Index");
    printf("===============================================================================================================================\n");

    while (heapSize > 0) {
        current = heapPop(heap, &heapSize);

        printf("%-6d %-22s %-18s %-14s %-10d %-10.2f %-10.2f %-8d %-8.2f %-12.2f\n",
               current->playerid, current->name, current->teamName,
               getRoleName(current->role), current->totalRuns,
               current->battingAverage, current->strikeRate,
               current->wickets, current->economyRate,
               current->performanceIndex);

        if (current->next != NULL)
            heapPush(heap, &heapSize, current->next);
    }

    printf("===============================================================================================================================\n");
}

void freeAllTeams(Team teamList[]){
    for(int i=0; i < teamCount;i++){
        freePlayerList(teamList[i].batsmen);
        freePlayerList(teamList[i].bowlers);
        freePlayerList(teamList[i].allRounders);
    }
}

int main(){
    Team teamList[10];
    int choice;
    printf("\n============================================\n");

    printf("ICC ODI Player Performace Analyzer\n");
    printf("============================================\n");


    initializeTeams(teamList);

    if(!initializePlayers(teamList)){
        printf("\n Initialization Failed!\n");
        freeAllTeams(teamList);
        return 1;
    }
    printf("\nInitialization Successful!\n");
    printf("\nTotal Teams: %d\n",teamCount);
    printf("Total Players: %d\n",playerCount);

      do {
        printf("\n============================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display All Players of a Specific Role Across All Teams\n");
        printf("6. Exit\n");
        printf("============================================\n");

        if (!readInt("Enter your choice: ", &choice)) {
            choice = 0;
            continue;
        }

        switch (choice) {
            case 1:
                addNewPlayer(teamList);
                break;

            case 2:
                optionDisplayTeam(teamList);
                break;

            case 3:
                displayTeamsByAverageSR(teamList);
                break;

            case 4: {
                int id;
                Team *team;

                if (!readInt("Enter Team ID (1-10): ", &id))
                    break;

                team = findTeamById(teamList, id);
                if (team == NULL)
                    printf("Invalid Team ID!\n");
                else
                    displayTopKPlayers(team);
                break;
            }
            case 5:
                displayAllPlayersOfRole(teamList);
                break;

            case 6:
                printf("\nExiting program...\n");
                break;

            default:
                printf("Invalid choice! Please select 1-6.\n");
        }
    } while (choice != 6);

    freeAllTeams(teamList);
    return 0;

}