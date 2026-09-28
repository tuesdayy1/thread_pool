#include <stdio.h>
#include <windows.h>
#include <time.h>

#define BUFFER_SIZE 5
#define WORKERS_COUNT 3

typedef struct {
    int task_id;
    int operation; // 1 - Квадрат, 2 - Инкремент
    int data;
} Task;

Task buf[BUFFER_SIZE];
int write_index = 0;
int read_index = 0;

HANDLE hMutex;
HANDLE hSemEmpty;
HANDLE hSemFull;
BOOL bWork = TRUE;


DWORD WINAPI WorkerThread(LPVOID lpParam) {
    int worker_id = (int)(INT_PTR)lpParam;
    Task current_task;

    while (1) {
        
        DWORD wait_result = WaitForSingleObject(hSemFull, 2000);

        if (wait_result == WAIT_TIMEOUT) {
            if (!bWork) {
                break;
            }
            continue;
        }

        WaitForSingleObject(hMutex, INFINITE);
        current_task = buf[read_index];
        read_index = (read_index + 1) % BUFFER_SIZE;
        ReleaseMutex(hMutex);
        ReleaseSemaphore(hSemEmpty, 1, NULL);

        printf("[WORKER %d] Started task %d (Op: %d, Data: %d).\n", worker_id, current_task.task_id, current_task.operation, current_task.data);
        Sleep(2000);
        int result = 0;
        if (current_task.operation == 1) {
            result = current_task.data * current_task.data;
        }
        else if (current_task.operation == 2) {
            result = current_task.data + 1;
        }

        printf("[WORKER %d] Finished task %d. Result: %d.\n", worker_id, current_task.task_id, result);
    }
    printf("[WORKER %d] No more tasks. Thread closed.\n", worker_id);
    return 0;
}


int main() {
    
    printf("[MAIN] Starting.\n");
    srand((unsigned int)time(NULL));

    hMutex = CreateMutex(NULL, FALSE, NULL);
    hSemEmpty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
    hSemFull = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);

    HANDLE hWorkers[WORKERS_COUNT];
    DWORD threadIds[WORKERS_COUNT];
    for (int i = 0; i < WORKERS_COUNT; i++) {
        hWorkers[i] = CreateThread(NULL, 0, WorkerThread, (LPVOID)(INT_PTR)(i + 1), 0, &threadIds[i]);
    }

    for (int id = 1; id <= 15; id++) {
        Task t;
        t.task_id = id;
        t.operation = (id % 2 == 0) ? 1 : 2;
        t.data = rand() % 10 + 1;

        WaitForSingleObject(hSemEmpty, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        buf[write_index] = t;
        write_index = (write_index + 1) % BUFFER_SIZE;

        ReleaseMutex(hMutex);
        ReleaseSemaphore(hSemFull, 1, NULL);

        if (id == 3) {
            Sleep(2000);
        }
    }
    bWork = FALSE;

    WaitForMultipleObjects(WORKERS_COUNT, hWorkers, TRUE, INFINITE);;

    printf("[MAIN] All workers shut down.\n");
    for (int i = 0; i < WORKERS_COUNT; i++) {
        CloseHandle(hWorkers[i]);
    }
    CloseHandle(hMutex);
    CloseHandle(hSemEmpty);
    CloseHandle(hSemFull);

    return 0;
}