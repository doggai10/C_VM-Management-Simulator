#include "main.h"

FILE *file;
int N, M, W, K;
int pageRef[KMAX + 5];
int memStatus[MMAX + 5];
int forwardDist[MMAX + 5];
int memLoadTime[MMAX + 5];
int memRefTime[MMAX + 5];
int memRefCnt[MMAX + 5];
bool workSet[NMAX + 5];
bool outPage[NMAX + 5];
bool terminate = false, faultCheck = false, outPageCheck = false;
int simulType = 1, resPos = -1, pageNum = -1, nowTime = 1, pageFault = 0;

bool inputCheck();
void simulation();
void printSimulName();
bool fixSimul();
bool varSimul();
void wsMemInitialize();
void wsMemSetting();
void wsMemFaultCheck();
void wsOutPageCheck();
void memFaultCheck();
bool memEmptyCheck();
void memSetting();
bool replaceStrategy();
int minReplace();
void forwardDistCalc();
int fifoReplace();
int lruReplace();
int lfuReplace();
void printResultHeader();
void printSimulResult();
void fixSetting();
void varSetting();

int main(int argc, char *argv[])
{
    file = fopen("./input.txt", "r");
    if (file == NULL)
        printf("Couldn't find input file\n");

    else if (fscanf(file, "%d %d %d %d\n", &N, &M, &W, &K) == 4)
    {
        terminate = inputCheck();
        fclose(file);
        if (terminate)
            printf("input file doesn't follow instructions\n");
        else
            simulation();
    }
    else
        printf("input file doesn't follow instructions\n");
    return 0;
}

bool inputCheck()
{
    bool check = false;
    if (N > NMAX || M > MMAX || W > WMAX || K > KMAX)
        check = true;
    else
    {
        for (int i = 0; i < K; i++)
        {
            int temp;
            if (fscanf(file, "%d", &temp) > 0)
            {
                if (temp < 0)
                {
                    check = true;
                    break;
                }
                else
                    pageRef[i] = temp;
            }
            else
            {
                check = true;
                break;
            }
        }
    }
    return check;
}

void simulation()
{
    bool simulCheck = true;
    while (simulType < SMAX)
    {
        memset(memStatus, -1, sizeof(memStatus));
        memset(outPage, false, sizeof(outPage));
        printSimulName();
        if (simulType < SMAX - 1)
            simulCheck = fixSimul();
        else
            simulCheck = varSimul();
        if (!simulCheck)
            return;
        printf("total Page Fault: %d\n\n", pageFault);
        fixSetting();
    }
}

void printSimulName()
{
    switch (simulType)
    {
    case 1:
        printf("MIN\n");
        break;
    case 2:
        printf("FIFO\n");
        break;
    case 3:
        printf("LRU\n");
        break;
    case 4:
        printf("LFU\n");
        break;
    case 5:
        printf("WS\n");
        break;
    default:
        break;
    }
    printResultHeader();
}

bool fixSimul()
{
    for (int i = 0; i < K; i++)
    {
        pageNum = pageRef[i];
        if (simulType == 4)
            memRefCnt[pageNum]++;
        memFaultCheck();
        if (faultCheck)
        {
            pageFault++;
            bool emptyCheck = memEmptyCheck();
            if (!emptyCheck)
            {
                terminate = replaceStrategy();
                if (!terminate)
                {
                    printf("Replacement Startegy has a problem\n");
                    return false;
                }
            }
            memSetting();
        }
        printSimulResult(faultCheck);
        nowTime++;
    }
    return true;
}

void memFaultCheck()
{
    faultCheck = true;
    for (int j = 0; j < M; j++)
    {
        if (memStatus[j] == pageNum)
        {
            memRefTime[j] = nowTime;
            faultCheck = false;
            break;
        }
    }
}

bool memEmptyCheck()
{
    bool empty = false;
    for (int j = 0; j < M; j++)
    {
        if (memStatus[j] == -1)
        {
            resPos = j;
            empty = true;
            break;
        }
    }
    return empty;
}

bool replaceStrategy()
{
    bool replace = true;
    switch (simulType)
    {
    case 1:
        resPos = minReplace();
        break;
    case 2:
        resPos = fifoReplace();
        break;
    case 3:
        resPos = lruReplace();
        break;
    case 4:
        resPos = lfuReplace();
        break;
    default:
        replace = false;
        break;
    }
    return replace;
}

int minReplace()
{
    int maxValue = -1;
    resPos = -1;
    memset(forwardDist, -1, sizeof(forwardDist));
    forwardDistCalc();
    for (int i = 0; i < M; i++)
    {
        if (forwardDist[i] == -1)
        {
            if (maxValue == -1 && resPos == -1)
                resPos = i;
            else if (maxValue == -1 && resPos != -1)
                resPos = resPos > i ? i : resPos;
            else
                resPos = i;
            break;
        }
        else if (maxValue < forwardDist[i])
        {
            maxValue = forwardDist[i];
            resPos = i;
        }
        else if (maxValue == forwardDist[i])
            resPos = resPos > i ? i : resPos;
        else
            continue;
    }
    return resPos;
}

void forwardDistCalc()
{
    for (int i = 0; i < M; i++)
    {
        int numCheck = memStatus[i];
        if (numCheck == -1)
            continue;
        for (int j = nowTime; j < K; j++)
        {
            if (numCheck == pageRef[j])
            {
                forwardDist[i] = j - nowTime + 1;
                break;
            }
        }
    }
}

int fifoReplace()
{
    int minValue = INF;
    resPos = -1;
    for (int i = 0; i < M; i++)
    {
        if (minValue > memLoadTime[i])
        {
            minValue = memLoadTime[i];
            resPos = i;
        }
        else if (minValue == memLoadTime[i])
        {
            resPos = resPos > i ? i : resPos;
        }
        else
            continue;
    }
    return resPos;
}

int lruReplace()
{
    int minValue = INF;
    resPos = -1;
    for (int i = 0; i < M; i++)
    {
        if (minValue > memRefTime[i])
        {
            minValue = memRefTime[i];
            resPos = i;
        }
        else if (minValue == memRefTime[i])
            resPos = resPos > i ? i : resPos;
        else
            continue;
    }
    return resPos;
}

int lfuReplace()
{
    int minValue = INF;
    resPos = -1;
    for (int i = 0; i < M; i++)
    {
        int numCheck = memStatus[i];
        if (minValue > memRefCnt[numCheck])
        {
            minValue = memRefCnt[numCheck];
            resPos = i;
        }
        else if (minValue == memRefCnt[numCheck])
        {
            if (memRefTime[resPos] > memRefTime[i])
                resPos = i;
            else if (memRefTime[resPos] == memRefTime[i])
                resPos = resPos > i ? i : resPos;
            else
                continue;
        }
        else
            continue;
    }
    return resPos;
}

void memSetting()
{
    memStatus[resPos] = pageNum;
    if (simulType == 3 || simulType == 4)
        memRefTime[resPos] = nowTime;
    if (simulType == 2)
        memLoadTime[resPos] = nowTime;
}

bool varSimul()
{
    for (int i = 0; i < K; i++)
    {
        pageNum = pageRef[i];
        resPos = i;
        wsMemInitialize();
        wsMemSetting();
        wsMemFaultCheck();
        wsOutPageCheck();
        printSimulResult();
        nowTime++;
    }
    return true;
}

void wsMemInitialize()
{
    for (int j = 0; j < N; j++)
    {
        if (workSet[j])
        {
            workSet[j] = false;
            outPage[j] = true;
        }
    }
}

void wsMemSetting()
{
    for (int j = 1; j <= W; j++)
    {
        if (resPos - j < 0)
            break;
        workSet[pageRef[resPos - j]] = true;
        outPage[pageRef[resPos - j]] = false;
    }
}

void wsMemFaultCheck()
{
    if (!workSet[pageNum])
    {
        if (outPage[pageNum])
        {
            workSet[pageNum] = true;
            outPage[pageNum] = false;
        }
        else
        {
            pageFault++;
            faultCheck = true;
            workSet[pageNum] = true;
        }
    }
}

void wsOutPageCheck()
{
    for (int j = 0; j < N; j++)
    {
        if (outPage[j] && !workSet[j])
        {
            outPageCheck = true;
        }
        continue;
    }
}

void printResultHeader()
{
    printf("| Time  | Page String | page Fault | Memory |\n");
    printf("=============================================\n");
}

void printSimulResult()
{
    printf("| %3d   | %6d      | %s | ", nowTime, pageNum, faultCheck ? "  Fault   " : "          ");

    if (simulType < 5)
    {
        for (int i = 0; i < M; i++)
        {
            if (memStatus[i] > -1)
            {
                printf("%d ", memStatus[i]);
            }
        }
    }
    else
    {
        for (int i = 0; i < N; i++)
        {
            if (workSet[i])
                printf("%d ", i);
        }
        printf("\n Page Out List : ");
        if (outPageCheck)
        {
            for (int i = 0; i < N; i++)
            {
                if (outPage[i])
                    printf("%d ", i);
            }
        }
        else
            printf("page out didn't happen");
        printf("\n");
        varSetting();
    }
    printf("\n\n");
}

void fixSetting()
{
    if (simulType == 3)
        memset(memRefTime, 0, sizeof(memRefTime));
    simulType++;
    nowTime = 1;
    pageNum = -1;
    resPos = -1;
    pageFault = 0;
}

void varSetting()
{
    memset(outPage, false, sizeof(outPage));
    faultCheck = false;
    outPageCheck = false;
}