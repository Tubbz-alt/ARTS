/******************************************************************************
** This material was prepared as an account of work sponsored by an agency   **
** of the United States Government.  Neither the United States Government    **
** nor the United States Department of Energy, nor Battelle, nor any of      **
** their employees, nor any jurisdiction or organization that has cooperated **
** in the development of these materials, makes any warranty, express or     **
** implied, or assumes any legal liability or responsibility for the accuracy,* 
** completeness, or usefulness or any information, apparatus, product,       **
** software, or process disclosed, or represents that its use would not      **
** infringe privately owned rights.                                          **
**                                                                           **
** Reference herein to any specific commercial product, process, or service  **
** by trade name, trademark, manufacturer, or otherwise does not necessarily **
** constitute or imply its endorsement, recommendation, or favoring by the   **
** United States Government or any agency thereof, or Battelle Memorial      **
** Institute. The views and opinions of authors expressed herein do not      **
** necessarily state or reflect those of the United States Government or     **
** any agency thereof.                                                       **
**                                                                           **
**                      PACIFIC NORTHWEST NATIONAL LABORATORY                **
**                                  operated by                              **
**                                    BATTELLE                               **
**                                     for the                               **
**                      UNITED STATES DEPARTMENT OF ENERGY                   **
**                         under Contract DE-AC05-76RL01830                  **
**                                                                           **
** Copyright 2019 Battelle Memorial Institute                                **
** Licensed under the Apache License, Version 2.0 (the "License");           **
** you may not use this file except in compliance with the License.          **
** You may obtain a copy of the License at                                   **
**                                                                           **
**    https://www.apache.org/licenses/LICENSE-2.0                            **
**                                                                           **
** Unless required by applicable law or agreed to in writing, software       **
** distributed under the License is distributed on an "AS IS" BASIS, WITHOUT **
** WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the  **
** License for the specific language governing permissions and limitations   **
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "arts.h"
#include "shadAdapter.h"
#define EDTCOUNT 100
uint64_t lock = 0;
unsigned int count = 0;

void tester(uint32_t paramc, uint64_t * paramv, uint32_t depc, artsEdtDep_t depv[])
{
    unsigned int local;
    while(!artsShadAliasTryLock(&lock)) artsYield();
    PRINTF("Yield %u Local: %u Lock: %lu\n", artsGetCurrentWorker(), local, lock);
    artsYield();
    local = ++count;
    PRINTF("Done  %u Local: %u Lock: %lu\n", artsGetCurrentWorker(), local, lock);
    artsShadAliasUnlock(&lock);
    
    if(local == EDTCOUNT)
    {
        PRINTF("SHUTTING DOWN %u\n", count);
        artsShutdown();
    }
}

void initPerNode(unsigned int nodeId, unsigned int workerId, int argc, char** argv)
{
    PRINTF("%u -- %u\n", artsGetTotalWorkers(), artsGetCurrentWorker());
}

void initPerWorker(unsigned int nodeId, unsigned int workerId, int argc, char** argv)
{   
    for(unsigned int i=0; i<EDTCOUNT; i++)
    {
        if(i % artsGetTotalWorkers() == workerId)
            artsEdtCreate(tester, 0, 0, NULL, 0);
    }
}

int main(int argc, char** argv)
{
    artsRT(argc, argv);
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include "arts.h"
// #include "shadAdapter.h"
// #define EDTCOUNT 100
// artsShadLock_t * lock;
// unsigned int count = 0;

// void tester(uint32_t paramc, uint64_t * paramv, uint32_t depc, artsEdtDep_t depv[])
// {
//     unsigned int local;
//     artsShadLock(lock);
//     PRINTF("%u A\n", artsGetCurrentWorker());
//     local = ++count;
//     PRINTF("%u B Local: %u\n", artsGetCurrentWorker(), local);
//     artsShadUnlock(lock);
//     if(local == EDTCOUNT)
//     {
//         PRINTF("SHUTTING DOWN\n");
//         artsShutdown();
//     }
// }

// void initPerNode(unsigned int nodeId, int argc, char** argv)
// {
//     lock = artsShadCreateLock();
// }

// void initPerWorker(unsigned int nodeId, unsigned int workerId, int argc, char** argv)
// {   
        
//         for(unsigned int i=0; i<EDTCOUNT; i++)
//         {
//             if(i % artsGetTotalWorkers() == workerId)
//                 artsEdtCreate(tester, 0, 0, NULL, 0);
//         }
// }

// int main(int argc, char** argv)
// {
//     artsRT(argc, argv);
//     return 0;
// }

