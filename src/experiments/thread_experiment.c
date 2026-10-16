#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define WORKER_COUNT 4
#define ITERATIONS 100000

static long shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ready_condition = PTHREAD_COND_INITIALIZER;

static int ready_workers = 0;
static int start_workers = 0;

static void *worker(void *argument)
{
    long worker_id = *(long *)argument;

    pthread_mutex_lock(&ready_mutex);
    ready_workers++;
    pthread_cond_broadcast(&ready_condition);

    while (!start_workers)
        pthread_cond_wait(&ready_condition, &ready_mutex);

    pthread_mutex_unlock(&ready_mutex);

    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&counter_mutex);
        shared_counter++;
        pthread_mutex_unlock(&counter_mutex);
    }

    printf("Worker %ld completed %d increments.\n",
           worker_id, ITERATIONS);

    return NULL;
}

int main(void)
{
    pthread_t workers[WORKER_COUNT];
    long worker_ids[WORKER_COUNT];

    printf("========================================\n");
    printf("    THREAD SYNCHRONIZATION EXPERIMENT\n");
    printf("========================================\n");
    printf("Workers: %d\n", WORKER_COUNT);
    printf("Increments per worker: %d\n", ITERATIONS);
    printf("Expected counter: %d\n",
           WORKER_COUNT * ITERATIONS);

    for (long i = 0; i < WORKER_COUNT; i++) {
        worker_ids[i] = i + 1;

        int result = pthread_create(
            &workers[i], NULL, worker, &worker_ids[i]);

        if (result != 0) {
            fprintf(stderr, "pthread_create failed: %d\n", result);
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_lock(&ready_mutex);

    while (ready_workers < WORKER_COUNT)
        pthread_cond_wait(&ready_condition, &ready_mutex);

    printf("\nAll workers are ready. Releasing them together.\n");
    start_workers = 1;
    pthread_cond_broadcast(&ready_condition);

    pthread_mutex_unlock(&ready_mutex);

    for (int i = 0; i < WORKER_COUNT; i++) {
        int result = pthread_join(workers[i], NULL);

        if (result != 0) {
            fprintf(stderr, "pthread_join failed: %d\n", result);
            exit(EXIT_FAILURE);
        }
    }

    long expected = (long)WORKER_COUNT * ITERATIONS;

    printf("\nFinal shared counter: %ld\n", shared_counter);
    printf("Expected counter: %ld\n", expected);

    if (shared_counter == expected)
        printf("PASS: Mutex protected all counter updates.\n");
    else
        printf("FAIL: Counter does not match expected value.\n");

    pthread_cond_destroy(&ready_condition);
    pthread_mutex_destroy(&ready_mutex);
    pthread_mutex_destroy(&counter_mutex);

    return shared_counter == expected
        ? EXIT_SUCCESS : EXIT_FAILURE;
}
