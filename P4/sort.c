#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <mqueue.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "sort.h"
#include "utils.h"

/* Private functions */
void worker(Sort *sort, mqd_t mq, sem_t *mutex, pid_t ppid);   /*Workers' code*/
void manejador_sigterm(int sig);
void manejador_sigusr1(int sig);
Status clean_up_multiprocess(Sort *sort, mqd_t mq, sem_t *mutex, Status ret_val); /*Frees resources used in sort_multiprocess*/

/* Interface implementation */
Status bubble_sort(int *vector, int n_elements, int delay) {
    int i, j;
    int temp;

    if ((!(vector)) || (n_elements <= 0)) {
        return ERROR;
    }

    for (i = 0; i < n_elements - 1; i++) {
        for (j = 0; j < n_elements - i - 1; j++) {
            /* Delay. */
            fast_sleep(delay);
            if (vector[j] > vector[j+1]) {
                temp = vector[j];
                vector[j] = vector[j + 1];
                vector[j + 1] = temp;
            }
        }
    }

    return OK;
}

Status merge(int *vector, int middle, int n_elements, int delay) {
    int *aux = NULL;
    int i, j, k, l, m;

    if (!(aux = (int *)malloc(n_elements * sizeof(int)))) {
        return ERROR;
    }

    for (i = 0; i < n_elements; i++) {
        aux[i] = vector[i];
    }

    i = 0; j = middle;
    for (k = 0; k < n_elements; k++) {
        /* Delay. */
        fast_sleep(delay);
        if ((i < middle) && ((j >= n_elements) || (aux[i] < aux[j]))){
            vector[k] = aux[i];
            i++;
        }
        else {
            vector[k] = aux[j];
            j++;
        }

        /* This part is not needed, and it is computationally expensive, but
        it allows to visualize a partial mixture. */
        m = k + 1;
        for (l = i; l < middle; l++) {
            vector[m] = aux[l];
            m++;
        }
        for (l = j; l < n_elements; l++) {
            vector[m] = aux[l];
            m++;
        }
    }

    free((void *)aux);
    return OK;
}

int get_number_parts(int level, int n_levels) {
    /* The number of parts is 2^(n_levels - 1 - level). */
    return 1 << (n_levels - 1 - level);
}

Status init_sort(char *file_name, Sort *sort, int n_levels, int n_processes, int delay) {
    char string[MAX_STRING];
    FILE *file = NULL;
    int i, j, log_data;
    int block_size, modulus;

    if ((!(file_name)) || (!(sort))) {
        fprintf(stderr, "init_sort - Incorrect arguments\n");
        return ERROR;
    }

    /* At most MAX_LEVELS levels. */
    sort->n_levels = MAX(1, MIN(n_levels, MAX_LEVELS));
    /* At most MAX_PARTS processes can work together. */
    sort->n_processes = MAX(1, MIN(n_processes, MAX_PARTS));
    /* The main process PID is stored. */
    sort->ppid = getpid();
    /* Delay for the algorithm in ns (less than 1s). */
    sort->delay = MAX(1, MIN(999999999, delay));

    if (!(file = fopen(file_name, "r"))) {
        perror("init_sort - fopen");
        return ERROR;
    }

    /* The first line contains the size of the data, truncated to MAX_DATA. */
    if (!(fgets(string, MAX_STRING, file))) {
        fprintf(stderr, "init_sort - Error reading file\n");
        fclose(file);
        return ERROR;
    }
    sort->n_elements = atoi(string);
    if (sort->n_elements > MAX_DATA) {
        sort->n_elements = MAX_DATA;
    }

    /* The remaining lines contains one integer number each. */
    for (i = 0; i < sort->n_elements; i++) {
        if (!(fgets(string, MAX_STRING, file))) {
            fprintf(stderr, "init_sort - Error reading file\n");
            fclose(file);
            return ERROR;
        }
        sort->data[i] = atoi(string);
    }
    fclose(file);

    /* Each task should have at least one element. */
    log_data = compute_log(sort->n_elements);
    if (n_levels > log_data) {
        n_levels = log_data;
    }
    sort->n_levels = n_levels;

    /* The data is divided between the tasks, which are also initialized. */
    block_size = sort->n_elements / get_number_parts(0, sort->n_levels);
    modulus = sort->n_elements % get_number_parts(0, sort->n_levels);
    sort->tasks[0][0].completed = INCOMPLETE;
    sort->tasks[0][0].ini = 0;
    sort->tasks[0][0].end = block_size + (modulus > 0);
    sort->tasks[0][0].mid = NO_MID;
    for (j = 1; j < get_number_parts(0, sort->n_levels); j++) {
        sort->tasks[0][j].completed = INCOMPLETE;
        sort->tasks[0][j].ini = sort->tasks[0][j - 1].end;
        sort->tasks[0][j].end = sort->tasks[0][j].ini \
            + block_size + (modulus > j);
        sort->tasks[0][j].mid = NO_MID;
    }
    for (i = 1; i < n_levels; i++) {
        for (j = 0; j < get_number_parts(i, sort->n_levels); j++) {
            sort->tasks[i][j].completed = INCOMPLETE;
            sort->tasks[i][j].ini = sort->tasks[i - 1][2 * j].ini;
            sort->tasks[i][j].mid = sort->tasks[i - 1][2 * j].end;
            sort->tasks[i][j].end = sort->tasks[i - 1][2 * j + 1].end;
        }
    }

    return OK;
}

Bool check_task_ready(Sort *sort, int level, int part) {
    if (!(sort)) {
        return FALSE;
    }

    if ((level < 0) || (level >= sort->n_levels) \
        || (part < 0) || (part >= get_number_parts(level, sort->n_levels))) {
        return FALSE;
    }

    if (sort->tasks[level][part].completed != INCOMPLETE) {
        return FALSE;
    }

    /* The tasks of the first level are always ready. */
    if (level == 0) {
        return TRUE;
    }

    /* Other tasks depend on the hierarchy. */
    if ((sort->tasks[level - 1][2 * part].completed == COMPLETED) && \
        (sort->tasks[level - 1][2 * part + 1].completed == COMPLETED)) {
        return TRUE;
    }

    return FALSE;
}

Status solve_task(Sort *sort, int level, int part) {
    /* In the first level, bubble-sort. */
    if (sort->tasks[level][part].mid == NO_MID) {
        return bubble_sort(\
            sort->data + sort->tasks[level][part].ini, \
            sort->tasks[level][part].end - sort->tasks[level][part].ini, \
            sort->delay);
    }
    /* In other levels, merge. */
    else {
        return merge(\
            sort->data + sort->tasks[level][part].ini, \
            sort->tasks[level][part].mid - sort->tasks[level][part].ini, \
            sort->tasks[level][part].end - sort->tasks[level][part].ini, \
            sort->delay);
    }
}

Status sort_single_process(char *file_name, int n_levels, int n_processes, int delay) {
    int i, j;
    Sort sort;

    /* The data is loaded and the structure initialized. */
    if (init_sort(file_name, &sort, n_levels, n_processes, delay) == ERROR) {
        fprintf(stderr, "sort_single_process - init_sort\n");
        return ERROR;
    }

    plot_vector(sort.data, sort.n_elements);
    printf("\nStarting algorithm with %d levels and %d processes...\n", sort.n_levels, sort.n_processes);
    /* For each level, and each part, the corresponding task is solved. */
    for (i = 0; i < sort.n_levels; i++) {
        for (j = 0; j < get_number_parts(i, sort.n_levels); j++) {
            solve_task(&sort, i, j);
            plot_vector(sort.data, sort.n_elements);
            printf("\n%10s%10s%10s%10s%10s\n", "PID", "LEVEL", "PART", "INI", \
                "END");
            printf("%10d%10d%10d%10d%10d\n", getpid(), i, j, \
                sort.tasks[i][j].ini, sort.tasks[i][j].end);
        }
    }

    plot_vector(sort.data, sort.n_elements);
    printf("\nAlgorithm completed\n");

    return OK;
}

Status sort_multiprocess(char *file_name, int n_levels, int n_processes, int delay) {
    int i, j, n_parts;
    int fd;
    Sort *sort;

    mqd_t mq;
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = sizeof(2*sizeof(int)) /*level and part determines a task*/
    };
    Message msg;

    pid_t children_id[MAX_PARTS] = {0}, ppid = getpid();
    int child_exit_status;
    Bool worker_failed = FALSE, level_completed;

    struct sigaction s_term, s_u1;
    sem_t *mutex;
    sigset_t wait_su1, block_su1;

    /*Initializing handlers and masks*/
    s_term.sa_handler = manejador_sigterm;
    s_term.sa_flags = 0;
    sigemptyset(&(s_term.sa_mask));

    s_u1.sa_handler = manejador_sigusr1;
    s_u1.sa_flags = 0;
    sigemptyset(&(s_u1.sa_mask));

    sigemptyset(&block_su1);
    sigaddset(&block_su1, SIGUSR1);
    sigfillset(&wait_su1);
    sigdelset(&wait_su1, SIGUSR1);
    sigdelset(&wait_su1, SIGINT);

    /* POSIX shared memory object is created and mapped */
    fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        perror("shm_open");
        return ERROR;
    }
    shm_unlink(SHM_NAME);   /*SHM_NAME will no longer be used*/

    if (ftruncate(fd, sizeof(Sort)) < 0) {
        perror("ftruncate");
        return ERROR;
    }

    sort = mmap(NULL, sizeof(Sort), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (sort == MAP_FAILED) {
        perror("mmap");
        return ERROR;
    }

    /*Mutual exclusion semaphore is created*/
    if ((mutex = sem_open("/mutex", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        perror("sem_open");
        return clean_up_multiprocess(sort, (mqd_t)-1, NULL, ERROR);
    }
    sem_unlink("/mutex");

    /* POSIX message queue is created */
    mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attributes);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        clean_up_multiprocess(sort, mq, mutex, ERROR);
    }
    mq_unlink(MQ_NAME); /*MQ_NAME will no longer be used*/

    /* The data is loaded and the structure initialized. */
    if (init_sort(file_name, sort, n_levels, n_processes, delay) == ERROR) {
        fprintf(stderr, "sort_single_process - init_sort\n");
        return clean_up_multiprocess(sort, mq, mutex, ERROR);
    }

    plot_vector(sort->data, sort->n_elements);
    printf("\nStarting algorithm with %d levels and %d processes...\n", sort->n_levels, sort->n_processes);

    /*Applying SIGUSR1 mask and handler*/
    if (sigprocmask(SIG_BLOCK, &block_su1, NULL) == -1) {
        perror("sigprocmask");
        return clean_up_multiprocess(sort, mq, mutex, ERROR);
    }

    if (sigaction(SIGUSR1, &s_u1, NULL) == -1) {
        perror("sigaction");
        return clean_up_multiprocess(sort, mq, mutex, ERROR);
    }
    
    /*Initializing the workers*/
    for (j = 0; j < sort->n_processes; j++) {
        children_id[j] = fork();
        if (children_id[j] < 0) {
            perror("fork");
            return clean_up_multiprocess(sort, mq, mutex, ERROR);
        }
        if (!children_id[j]) {
            if (sigaction(SIGTERM, &s_term, NULL) == -1) {
                perror("sigaction");
                return clean_up_multiprocess(sort, mq, mutex, ERROR);
            }
            worker(sort, mq, mutex, ppid);
        }
    }

    /* For each level, and each part, the corresponding task is solved. */
    for (i = 0; i < sort->n_levels; i++) {
        level_completed = FALSE;
        n_parts = get_number_parts(i, sort->n_levels);
        msg.level = i;
        for (j = 0; j < n_parts; j++) {
            msg.part = j;
            if (mq_send(mq, (char *)&msg, sizeof(msg), 1) == -1) {
                perror("mq_send");
                return clean_up_multiprocess(sort, mq, mutex, ERROR);  /*Fatal error*/
            }
        }

        while (!level_completed) {
            sigsuspend(&wait_su1);
            sem_wait(mutex);
            for (j = 0; j < n_parts; j++) {
                level_completed = TRUE;
                if (sort->tasks[i][j].completed != COMPLETED) {
                    level_completed = FALSE;
                    break;
                }
            }
            sem_post(mutex);
        }
        
        plot_vector(sort->data, sort->n_elements);
        printf("\n%10s%10s%10s%10s%10s\n", "PID", "LEVEL", "PART", "INI", "END");
        printf("%10d%10d%10d%10d%10d\n", getpid(), i, -1, sort->tasks[i][j].ini, sort->tasks[i][j].end);
    }

    /*Sending SIGTERM to child processes*/
    for (j = 0; j < sort->n_processes; j++) {
        if (kill(children_id[j], SIGTERM) == -1) {
            perror("kill");
            return clean_up_multiprocess(sort, mq, mutex, ERROR);
        }
    }
    for (j = 0; j < sort->n_processes; j++) {
        waitpid(children_id[j], &child_exit_status, 0);
        if (WIFEXITED(child_exit_status) && WEXITSTATUS(child_exit_status) == EXIT_FAILURE) {
            fprintf(stderr, "Worker failed\n");
            worker_failed = TRUE;
        }
    }
    if (worker_failed) {
        return clean_up_multiprocess(sort, mq, mutex, ERROR);
    }

    plot_vector(sort->data, sort->n_elements);
    printf("\nAlgorithm completed\n");

    return clean_up_multiprocess(sort, mq, mutex, OK);
}

/* Private functions implementation */
static Bool term = FALSE;

void worker(Sort *sort, mqd_t mq, sem_t *mutex, pid_t ppid) {
    Message msg;
    int status = EXIT_SUCCESS;

    while(!term) {
        if (mq_receive(mq, (char *)&msg, sizeof(msg), NULL) == -1) {
            if (errno != 4) {
                perror("mq_receive");
            }
        } else if (solve_task(sort, msg.level, msg.part) == OK) {
            status = EXIT_SUCCESS;
        }

        if (!term) {
            sem_wait(mutex);
            sort->tasks[msg.level][msg.part].completed = COMPLETED;
            sem_post(mutex);

            if (kill(ppid, SIGUSR1) == -1) {
                perror("kill");
                term = TRUE;
                status = EXIT_FAILURE;
            }
        }
    }

    clean_up_multiprocess(sort, mq, mutex, OK);
    exit(status);
}

void manejador_sigterm(int sig) {
    term = TRUE;
}

void manejador_sigusr1(int sig) {}

Status clean_up_multiprocess(Sort *sort, mqd_t mq, sem_t *mutex, Status ret_val) {
    if (sort != NULL) {
        munmap(sort, sizeof(*sort));
    }
    if (mq != (mqd_t)-1) {
        mq_close(mq);
    }
    if (mutex != NULL) {
        sem_close(mutex);
    }
    return ret_val;
}