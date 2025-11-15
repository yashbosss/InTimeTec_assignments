#include <stdio.h>
#include <stdlib.h>
#include "Players_data.h"
#define MAX_TEAMS 10
#define ROLE_BATSMAN 0
#define ROLE_BOWLER 1
#define ROLE_ALLROUNDER 2

int strLen(const char *s)
{
    int i = 0;
    while (s[i] != '\0')
        i++;
    return i;
}

void strCopy(char *dest, const char *src, int maxLen)
{
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int strCmp(const char *a, const char *b)
{
    int lenA = strLen(a);
    int lenB = strLen(b);

    if (lenA != lenB)
    {
        return (lenA > lenB) ? 1 : -1;
    }

    int i = 0;
    while (i < lenA)
    {
        if (a[i] != b[i])
            return (a[i] > b[i]) ? 1 : -1;
        i++;
    }
    return 0;
}

void readLine(char *buf, int max)
{
    char ch;
    int i = 0;
    while (i < max - 1)
    {
        if (scanf("%c", &ch) != 1)
            break;
        if (ch == '\n')
            break;
        buf[i++] = ch;
    }
    buf[i] = '\0';
}

typedef struct
{
    int playerId;
    char name[51];
    char teamName[51];
    int role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    double performanceIndex;
} myPlayer;

typedef struct Node
{
    myPlayer data;
    struct Node *next;
} Node;

typedef struct
{
    int teamId;
    char name[51];
    int totalPlayers;
    float sumBattingStrikeRate;
    int countBattersAllrounders;
    Node *playersHead;
    Node *playersTail;
    Node *roleHeads[3];
} Team;

Team teamsArr[MAX_TEAMS];

double evaluatePerformanceIndex(const myPlayer *p)
{
    if (p->role == ROLE_BATSMAN)
    {
        return (p->battingAverage * p->strikeRate) / 100.0;
    }
    else if (p->role == ROLE_BOWLER)
    {
        return (p->wickets * 2.0) + (100.0 - p->economyRate);
    }
    else
    {
        return ((p->battingAverage * p->strikeRate) / 100.0) + (p->wickets * 2.0);
    }
}

Node *createNode(const myPlayer *mp)
{
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }
    n->data = *mp;
    n->next = NULL;
    return n;
}

void insertPlayerToTeam(Team *t, Node *n)
{
    if (!t->playersHead)
    {
        t->playersHead = t->playersTail = n;
    }
    else
    {
        t->playersTail->next = n;
        t->playersTail = n;
    }
}

void insertIntoRole(Team *t, Node *n, int roleIndex)
{
    Node *head = t->roleHeads[roleIndex];
    if (!head || n->data.performanceIndex > head->data.performanceIndex)
    {
        n->next = head;
        t->roleHeads[roleIndex] = n;
        return;
    }
    Node *cur = head;
    while (cur->next && cur->next->data.performanceIndex >= n->data.performanceIndex)
    {
        cur = cur->next;
    }
    n->next = cur->next;
    cur->next = n;
}

int teamSearchById(int id)
{
    int low = 0, high = MAX_TEAMS - 1;
    while (low <= high)
    {
        int mid = (low + high) / 2;
        if (teamsArr[mid].teamId == id)
            return mid;
        if (teamsArr[mid].teamId < id)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1;
}

int findTeamIndexByName(const char *name)
{
    for (int i = 0; i < MAX_TEAMS; ++i)
    {
        if (strCmp(teamsArr[i].name, name) == 0)
            return i;
    }
    return -1;
}

void addPlayer(Team *t, const myPlayer *mp)
{
    Node *n_all = createNode(mp);
    Node *n_role = createNode(mp);

    insertPlayerToTeam(t, n_all);

    insertIntoRole(t, n_role, (int)mp->role);

    t->totalPlayers += 1;
    if (mp->role == ROLE_BATSMAN || mp->role == ROLE_ALLROUNDER)
    {
        t->sumBattingStrikeRate += mp->strikeRate;
        t->countBattersAllrounders += 1;
    }
}

void initializeTeams()
{
    for (int i = 0; i < teamCount && i < MAX_TEAMS; ++i)
    {
        teamsArr[i].teamId = i + 1;
        strCopy(teamsArr[i].name, teams[i], 51);
        teamsArr[i].totalPlayers = 0;
        teamsArr[i].sumBattingStrikeRate = 0.0f;
        teamsArr[i].countBattersAllrounders = 0;
        teamsArr[i].playersHead = teamsArr[i].playersTail = NULL;
        teamsArr[i].roleHeads[0] = teamsArr[i].roleHeads[1] = teamsArr[i].roleHeads[2] = NULL;
    }
}

int checkRole(const char *roleStr)
{
    if (!roleStr)
        return -1;
    if (roleStr[0] == 'B' || roleStr[0] == 'b')
        return ROLE_BATSMAN;
    if (roleStr[0] == 'B' && roleStr[1] == 'o')
        return ROLE_BOWLER;
    if (roleStr[0] == 'A' || roleStr[0] == 'a')
        return ROLE_ALLROUNDER;
    if (roleStr[0] == 'B' && roleStr[1] == 'o')
        return ROLE_BOWLER;
    return -1;
}

const char *roleType(int r)
{
    if (r == ROLE_BATSMAN)
        return "Batsman";
    if (r == ROLE_BOWLER)
        return "Bowler";
    return "All-Rounder";
}

void initializePlayersFromPlayersDataSet()
{
    for (int i = 0; i < playerCount; ++i)
    {
        myPlayer mp;
        mp.playerId = players[i].id;
        strCopy(mp.name, players[i].name, 51);
        strCopy(mp.teamName, players[i].team, 51);
        int r = checkRole(players[i].role);
        if (r == -1)
        {
            if ((players[i].role[0] == 'B' || players[i].role[0] == 'b') && (players[i].role[1] == 'O' || players[i].role[1] == 'o'))
                r = ROLE_BOWLER;
            else if ((players[i].role[0] == 'B' || players[i].role[0] == 'b') && (players[i].role[1] == 'A' || players[i].role[1] == 'A'))
                r = ROLE_BATSMAN;
            else
                r = ROLE_ALLROUNDER;
        }
        mp.role = r;
        mp.totalRuns = players[i].totalRuns;
        mp.battingAverage = players[i].battingAverage;
        mp.strikeRate = players[i].strikeRate;
        mp.wickets = players[i].wickets;
        mp.economyRate = players[i].economyRate;
        mp.performanceIndex = evaluatePerformanceIndex(&mp);

        int tidx = findTeamIndexByName(players[i].team);
        if (tidx == -1)
        {
            continue;
        }
        addPlayer(&teamsArr[tidx], &mp);
    }
}

void addPlayerToTeam()
{
    printf("Enter Team ID to add player: ");
    int teamId;
    if (scanf("%d", &teamId) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input\n");
        return;
    }
    while (getchar() != '\n')
        ;

    int tempId = teamSearchById(teamId);
    if (tempId == -1)
    {
        printf("Team ID %d not found\n", teamId);
        return;
    }
    Team *t = &teamsArr[tempId];

    myPlayer mp;
    char buf[128];

    printf("Enter Player ID: ");
    if (scanf("%d", &mp.playerId) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid id\n");
        return;
    }
    while (getchar() != '\n')
        ;

    printf("Name: ");
    readLine(buf, sizeof(buf));
    strCopy(mp.name, buf, 51);

    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int r;
    if (scanf("%d", &r) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid role\n");
        return;
    }
    while (getchar() != '\n')
        ;
    mp.role = (r == 1 ? ROLE_BATSMAN : (r == 2 ? ROLE_BOWLER : ROLE_ALLROUNDER));

    printf("Total Runs: ");
    if (scanf("%d", &mp.totalRuns) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid runs\n");
        return;
    }
    while (getchar() != '\n')
        ;

    printf("Batting Average: ");
    if (scanf("%f", &mp.battingAverage) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid avg\n");
        return;
    }
    while (getchar() != '\n')
        ;

    printf("Strike Rate: ");
    if (scanf("%f", &mp.strikeRate) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid SR\n");
        return;
    }
    while (getchar() != '\n')
        ;

    printf("Wickets: ");
    if (scanf("%d", &mp.wickets) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid wkts\n");
        return;
    }
    while (getchar() != '\n')
        ;

    printf("Economy Rate: ");
    if (scanf("%f", &mp.economyRate) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid ER\n");
        return;
    }
    while (getchar() != '\n')
        ;

    strCopy(mp.teamName, t->name, 51);

    mp.performanceIndex = evaluatePerformanceIndex(&mp);

    addPlayer(t, &mp);

    printf("Player added successfully to Team %s!\n", t->name);
}

void displayTeamPlayers()
{
    printf("Enter Team ID: ");
    int teamId;
    if (scanf("%d", &teamId) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input\n");
        return;
    }
    while (getchar() != '\n')
        ;
    int tempId = teamSearchById(teamId);
    if (tempId == -1)
    {
        printf("Team ID %d not found\n", teamId);
        return;
    }
    Team *t = &teamsArr[tempId];

    printf("Players of Team %s:\n", t->name);
    printf("====================================================================================\n");
    printf("ID   Name                          Role         Runs   Avg     SR     Wkts   ER     Perf.Index\n");
    printf("====================================================================================\n");
    Node *cur = t->playersHead;
    while (cur)
    {
        myPlayer *p = &cur->data;
        printf("%-4d %-29s %-11s %-6d %6.1f %7.1f %6d %6.1f %10.2f\n",
               p->playerId, p->name, roleType(p->role), p->totalRuns,
               p->battingAverage, p->strikeRate, p->wickets, p->economyRate,
               p->performanceIndex);
        cur = cur->next;
    }
    printf("====================================================================================\n");
    printf("Total Players: %d\n", t->totalPlayers);
    double avgSR = 0.0;
    if (t->countBattersAllrounders > 0)
        avgSR = t->sumBattingStrikeRate / (double)t->countBattersAllrounders;
    printf("Average Batting Strike Rate: %.2f\n", avgSR);
}

void sortTeamsByAvgStrikeRate(int idxs[], int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        int maxj = i;
        double maxi = 0.0;
        for (int k = i; k < n; ++k)
        {
            int id = idxs[k];
            Team *t = &teamsArr[id];
            double avgSR = (t->countBattersAllrounders > 0) ? (t->sumBattingStrikeRate / t->countBattersAllrounders) : 0.0;
            double curMax = (k == i) ? avgSR : -1e9;
            if (k == i)
                maxi = avgSR;
        }
        int best = i;
        double bestVal = -1e9;
        for (int k = i; k < n; ++k)
        {
            int id = idxs[k];
            Team *t = &teamsArr[id];
            double avgSR = (t->countBattersAllrounders > 0) ? (t->sumBattingStrikeRate / t->countBattersAllrounders) : 0.0;
            if (avgSR > bestVal)
            {
                bestVal = avgSR;
                best = k;
            }
        }

        if (best != i)
        {
            int tmp = idxs[i];
            idxs[i] = idxs[best];
            idxs[best] = tmp;
        }
    }
}

void displayTeamsByAvgStrikeRate()
{
    int idxs[MAX_TEAMS];
    for (int i = 0; i < MAX_TEAMS; ++i)
        idxs[i] = i;
    sortTeamsByAvgStrikeRate(idxs, MAX_TEAMS);

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("ID  Team Name                 Avg Bat SR   Total Players\n");
    printf("=========================================================\n");
    for (int i = 0; i < MAX_TEAMS; ++i)
    {
        Team *t = &teamsArr[idxs[i]];
        double avgSR = (t->countBattersAllrounders > 0) ? (t->sumBattingStrikeRate / t->countBattersAllrounders) : 0.0;
        printf("%-3d %-25s %9.2f   %5d\n", t->teamId, t->name, avgSR, t->totalPlayers);
    }
    printf("=========================================================\n");
}

void topKPlayersByRole()
{
    printf("Enter Team ID: ");
    int teamId;
    if (scanf("%d", &teamId) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid\n");
        return;
    }
    while (getchar() != '\n')
        ;
    int tidx = teamSearchById(teamId);
    if (tidx == -1)
    {
        printf("Team ID not found\n");
        return;
    }
    Team *t = &teamsArr[tidx];

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int r;
    if (scanf("%d", &r) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid\n");
        return;
    }
    while (getchar() != '\n')
        ;
    int roleIndex = (r == 1 ? 0 : (r == 2 ? 1 : 2));

    printf("Enter number of players (K): ");
    int K;
    if (scanf("%d", &K) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid\n");
        return;
    }
    while (getchar() != '\n')
        ;

    Node *cur = t->roleHeads[roleIndex];
    printf("Top %d %s of Team %s:\n", K, (roleIndex == 0 ? "Batsmen" : (roleIndex == 1 ? "Bowlers" : "All-rounders")), t->name);
    printf("====================================================================================\n");
    printf("ID   Name                          Role         Runs   Avg     SR     Wkts   ER     Perf.Index\n");
    printf("====================================================================================\n");
    int cnt = 0;
    while (cur && cnt < K)
    {
        myPlayer *p = &cur->data;
        printf("%-4d %-29s %-11s %-6d %6.1f %7.1f %6d %6.1f %10.2f\n",
               p->playerId, p->name, roleType(p->role), p->totalRuns,
               p->battingAverage, p->strikeRate, p->wickets, p->economyRate,
               p->performanceIndex);
        cur = cur->next;
        cnt++;
    }
    if (cnt == 0)
        printf("No players of that role in this team.\n");
}

typedef struct
{
    Node *node;
    int teamIndex;
} HeapItem;

void heap_swap(HeapItem *a, HeapItem *b)
{
    HeapItem tmp = *a;
    *a = *b;
    *b = tmp;
}

void heap_push(HeapItem heap[], int *hsize, HeapItem item)
{
    int i = (*hsize)++;
    heap[i] = item;
    while (i > 0)
    {
        int parent = (i - 1) / 2;
        if (heap[parent].node->data.performanceIndex >= heap[i].node->data.performanceIndex)
            break;
        heap_swap(&heap[parent], &heap[i]);
        i = parent;
    }
}

HeapItem heap_pop(HeapItem heap[], int *hsize)
{
    HeapItem ret = heap[0];
    heap[0] = heap[--(*hsize)];
    int i = 0;
    while (1)
    {
        int l = 2 * i + 1, r = 2 * i + 2, largest = i;
        if (l < *hsize && heap[l].node->data.performanceIndex > heap[largest].node->data.performanceIndex)
            largest = l;
        if (r < *hsize && heap[r].node->data.performanceIndex > heap[largest].node->data.performanceIndex)
            largest = r;
        if (largest == i)
            break;
        heap_swap(&heap[i], &heap[largest]);
        i = largest;
    }
    return ret;
}

void displayByRole()
{
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    int r;
    if (scanf("%d", &r) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid\n");
        return;
    }
    while (getchar() != '\n')
        ;
    int roleIndex = (r == 1 ? 0 : (r == 2 ? 1 : 2));

    HeapItem heap[MAX_TEAMS];
    int hsize = 0;
    for (int i = 0; i < MAX_TEAMS; ++i)
    {
        if (teamsArr[i].roleHeads[roleIndex])
        {
            HeapItem it;
            it.node = teamsArr[i].roleHeads[roleIndex];
            it.teamIndex = i;
            heap_push(heap, &hsize, it);
        }
    }
    printf("All %s of all teams:\n", (roleIndex == 0 ? "Batsmen" : (roleIndex == 1 ? "Bowlers" : "All-rounders")));
    printf("======================================================================================\n");
    printf("ID   Name                          Team                 Role         Runs   Avg    SR   Wkts ER  Perf.Index\n");
    printf("======================================================================================\n");
    while (hsize > 0)
    {
        HeapItem top = heap_pop(heap, &hsize);
        Node *cur = top.node;
        myPlayer *p = &cur->data;
        printf("%-4d %-29s %-20s %-11s %-6d %6.1f %6.1f %5d %4.1f %10.2f\n",
               p->playerId, p->name, p->teamName, roleType(p->role),
               p->totalRuns, p->battingAverage, p->strikeRate, p->wickets, p->economyRate, p->performanceIndex);
        if (cur->next)
        {
            HeapItem ni;
            ni.node = cur->next;
            ni.teamIndex = top.teamIndex;
            heap_push(heap, &hsize, ni);
        }
    }
    printf("======================================================================================\n");
}

void printMenu()
{
    printf("==============================================================================\n");
    printf("ICC ODI Player Performance Analyzer\n");
    printf("==============================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printf("==============================================================================\n");
    printf("Enter your choice: ");
}

void freeAllMemory5()
{
    for (int i = 0; i < MAX_TEAMS; ++i)
    {
        Node *cur = teamsArr[i].playersHead;
        while (cur)
        {
            Node *tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
}

int main()
{
    initializeTeams();
    initializePlayersFromPlayersDataSet();

    while (1)
    {
        printMenu();
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            printf("Invalid input\n");
            continue;
        }
        while (getchar() != '\n')
            ;
        switch (choice)
        {
        case 1:
            addPlayerToTeam();
            break;
        case 2:
            displayTeamPlayers();
            break;
        case 3:
            displayTeamsByAvgStrikeRate();
            break;
        case 4:
            topKPlayersByRole();
            break;
        case 5:
            displayByRole();
            break;
        case 6:
            freeAllMemory5();
            printf("Exiting.\n");
            return 0;
        default:
            printf("Invalid choice\n");
        }
        printf("\n");
    }
    return 0;
}
