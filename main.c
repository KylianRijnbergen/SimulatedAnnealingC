#include <stdio.h> /* Standard IO always included */ 
#include <stdlib.h> /* Included for memory allocation */ 
#include <assert.h> /* Used for assertions */ 
#include <time.h> /* Included for timing our program execution time */ 
#include <math.h> /* Included for getting e powers */
#include <string.h> /* Included for memcopies */

/* Definitions */
#define JOB_COUNT 100 /* We have 100 jobs */
#define MACHINE_COUNT 10 /* We have 10 machines */
/* Our jobs have a length that is obtained from the first 10 decimals of pi for each job, where each new job starts at the next 100th decimal 
Job 0 has the first 10 decimals, job 1 has decimals 100-109, etc. */
const long long JOBS[] = {1415926535, 8214808651, 4428810975, 7245870066, 3305727036, 9833673362, 5681271, 4201995611, 
5024459455, 5982534904, 5820974944, 4811174502, 4564856692, 7892590360, 744623799, 6094370277, 1468440901, 5187072113, 
7101000313, 8903894223, 8979323846, 3282306647, 6659334461, 631558817, 5759591953, 4406566430, 4526356082, 2129021960, 
3469083026, 2875546873, 5923078164, 8410270193, 3460348610, 113305305, 6274956735, 539217176, 2249534301, 4999999837, 
7838752886, 2858849455, 2643383279, 938446095, 2847564823, 4881520920, 921861173, 8602139494, 7785771342, 8640344181, 
4252230825, 1159562863, 628620899, 8521105559, 4543266482, 4882046652, 1885752724, 2931767523, 4654958537, 2978049951, 
5875332083, 9550031194, 5028841971, 5058223172, 3786783165, 9628292540, 8193261179, 6395224737, 7577896091, 5981362977, 
3344685035, 8823537875, 8628034825, 6446229489, 1339360726, 1384146951, 8912279381, 8467481846, 1050792279, 597317328, 
8142061717, 6252505467, 6939937510, 5359408128, 2712019091, 9171536436, 3105118548, 1907021798, 7363717872, 4771309960, 
2619311881, 9375195778, 3421170679, 5493038196, 249141273, 9415116094, 8301194912, 7669405132, 6892589235, 1609631859, 
7669147303, 4157424218};

/* Forward declarations */ 
/* Structs */
typedef struct Machine Machine;
typedef struct Temperature Temperature;

/* Functions */
int randint(int n); 
void print_machine(Machine machine);
void print_machines(Machine *machines, short nr_machines);
void init_machine(Machine *machine, short id);
void init_machines(Machine *machines, int nr_machines);
void assign_job_to_machine(Machine *machine, short job_id);
short get_job_position_on_machine(Machine *machine, short job_id);
void remove_job_from_machine(Machine *machine, short job_id);
void randomly_divide_jobs_over_machines(Machine *machines);
void move(Machine *machines);
void swap(Machine *machines);
void init_temperature(Temperature *temperature, double start_temp, double end_temp, double alpha, long long max_iters);
double get_acceptance_probability(Temperature *temperature, long long delta);
long long get_fitness(Machine *machines);
void simulated_annealing(Machine *working_solution, Temperature *temperature, Machine *best_solution);
int get_occupied_machines(Machine *machine);

/* Declaration of structs */ 
typedef struct Machine
{
    short id;
    short job_count;
    long long makespan; 
    short job_ids[JOB_COUNT];
} Machine;

typedef struct Temperature
{
    double start_temp;
    double end_temp;
    double alpha;
    long long max_iters;
    long long current_iter;
    double current_temp;
    int terminated;
} Temperature;

/* Main function */
int main(void)
{
    /* Start of timer */ 
    clock_t start_time = clock();

    /* Set random number seed */
    int seed = 1843397;
    srand(seed);

    /* Create pointer for our machines */
    Machine *machinesPtr;
    /* Allocate memory for MACHINE_COUNT machines */ 
    machinesPtr = malloc(sizeof(Machine) * MACHINE_COUNT);

    /* Create another pointer for our machines. This object is used to store the best solution. */
    Machine *best_solPtr;
    best_solPtr = malloc(sizeof(Machine) * MACHINE_COUNT);

    /* Create pointer for our temperature controller */
    Temperature *temp_controlPtr;
    /* Allocate memory for our temperature controller */
    temp_controlPtr = malloc(sizeof(Temperature));

    /* Initialize machines */ 
    init_machines(machinesPtr, MACHINE_COUNT);

    /* TEMPERATURE CONTROLS */
    /* Temperature controller settings */ 
    double start_temp = 100000000;
    double end_temp = 1;
    double alpha = 0.9999999;
    long long max_iters = 999999999999999999;
    /* Initialize temperature */
    init_temperature(temp_controlPtr, start_temp, end_temp, alpha, max_iters);

    /* Randomly divide jobs over machines */
    randomly_divide_jobs_over_machines(machinesPtr);

    /* Print machines */ 
    printf("Initial solution is : \n");
    print_machines(machinesPtr, MACHINE_COUNT);
    printf("\n\n");

    printf("Starting simulated annealing \n");
    /* Start simulated annealing */
    simulated_annealing(machinesPtr, temp_controlPtr, best_solPtr);

    /* Print final solution */
    print_machines(best_solPtr, MACHINE_COUNT);
    printf("\n\n");
    printf("Best solution has a fitness of %lld\n", get_fitness(best_solPtr));
    
    /* End timer and print time elapsed */ 
    clock_t end_time = clock();
    double time_taken = ((double)(end_time-start_time)/CLOCKS_PER_SEC);
    printf("Time taken was %lf seconds \n", time_taken);

    /* End program */
    return 0;
}

/* Other functions */
/* Function that returns a random integer with upper bound n. */ 
int randint(int n)
{
    if ((n - 1) == RAND_MAX) 
    {
        return rand();
    }
    else
    {
        assert (n <= RAND_MAX);

        int end = RAND_MAX / n;
        assert (end > 0);
        end *= n;

        int r;
        while ((r = rand()) >= end);
            return r % n; 
    }
}

/* Function that prints info about a machine and its jobs */
void print_machine(Machine machine)
{
    printf("Visualizing machine. \n");
    printf("The id of the machine is %d \n", machine.id);
    printf("The job count of the machine is %d \n", machine.job_count);
    printf("Jobs on the machine are :");
    for (int i = 0; i < machine.job_count; i++)
    {
        printf(" %d", machine.job_ids[i]);
    }
    printf("\n");
    printf("Machine makespan is: %lld \n\n", machine.makespan);
}

/* Function that prints our machines */
void print_machines(Machine *machines, short nr_machines)
{
    for (short i = 0; i < nr_machines; i++)
    {
        print_machine(*(machines+i));
    }
}

/* Function that initializes a single machine */ 
void init_machine(Machine *machine, short id)
{
    machine->id = id;
    machine->job_count = 0;
    machine->makespan = 0;
}

/* Function that initializes all machines */
void init_machines(Machine *machines, int nr_machines)
{
    for (short i = 0; i < nr_machines; i++)
    {
        init_machine(machines+i, i);
    }
}

/* Function that assigns a job id to a provided machine */ 
void assign_job_to_machine(Machine *machine, short job_id)
{
    machine->job_ids[machine->job_count] = job_id;
    machine->job_count++;
    machine->makespan += JOBS[job_id];
}

/* Function that can check whether a job is attached to a machine. 
If the machine contains the job, its position is returned.
If the machine does not contain the job, we return -1 */ 
short get_job_position_on_machine(Machine *machine, short job_id)
{
    for (short i = 0; i < machine->job_count; i++)
    {
        if (machine->job_ids[i] == job_id)
            return i;
    }
    return -1;
} 

/* Function that removes a job id from a provided machine */ 
void remove_job_from_machine(Machine *machine, short job_id)
{
    /* If the job is not on the machine, or the job count is 0,
    something went wrong and the program is terminated. */ 
    short jobpos = get_job_position_on_machine(machine, job_id);
    if (jobpos == -1 || machine->job_count == 0) /* This last check should be redundant but is kept in for development */ 
    {
        printf("Job is not present on machine. Terminating program.\n");
        exit(1);
    }
    /* if the job is on the machine, we remove it.
    We also move the last job on the machine such that we can update makespan efficiently. */ 
    else
    {
        if(machine->job_count == 1)
        {
            machine->job_count = 0;
            machine->makespan -= JOBS[job_id];
        }
        else
        {
            short job_id_to_move = machine->job_ids[machine->job_count-1];
            machine->job_ids[jobpos] = job_id_to_move;
            machine->makespan -= JOBS[job_id];
            machine->job_count--;
        }
    }
}

/* Function that randomly divides the jobs over all machines */
void randomly_divide_jobs_over_machines(Machine *machines)
{
    for (short i = 0; i < JOB_COUNT; i++)
    {
        /* Draw a random machine */
        int randmach = randint(MACHINE_COUNT);
        assign_job_to_machine(machines+randmach, i);
    }
}

/* Function that randomly selects a machine that has at least one job, 
and moves that job to a different machine */
void move(Machine *machines)
{
    int mach1 = randint(MACHINE_COUNT); /* Get random machine */
    while((machines+mach1)->job_count == 0) /* If the machine has no jobs, we try a different machine */
    {
        mach1 = randint(MACHINE_COUNT);
    }
    int jobs_on_mach1 = (machines+mach1)->job_count; /* We check the number of jobs on the machine */
    int job_to_move =  randint(jobs_on_mach1); /* We get any random job as placed on the machine */
    short job_id = (machines+mach1)->job_ids[job_to_move]; /* We get the id of the random job */
    
    int mach2 = randint(MACHINE_COUNT); /* We get a machine to which the job should be moved */
    while(mach1 == mach2) /* If origin and destination are the same, draw a new destination */ 
    {
        mach2 = randint(MACHINE_COUNT);
    }
    /* Remove job from machine 1 */
    remove_job_from_machine(machines+mach1, job_id);
    /* Add job to machine 2 */
    assign_job_to_machine(machines+mach2, job_id);
} 

/* Function that can swap 2 jobs between machines */
void swap(Machine *machines)
{   
    /* Get a random first machine that has at least one job */
    int mach1 = randint(MACHINE_COUNT); /* Get random machine */
    while((machines+mach1)->job_count == 0) /* If the machine has no jobs, we try a different machine */
    {
        mach1 = randint(MACHINE_COUNT);
    }

    /* Get a random second machine that has at least one job and is NOT equal to the first machine */
    int mach2 = randint(MACHINE_COUNT); /* We get a machine to which the job should be moved */
    while((mach1 == mach2) || ((machines+mach2)->job_count == 0)) /* If origin and destination are the same, draw a new destination */ 
    {
        mach2 = randint(MACHINE_COUNT);
    }

    int mach1_job_count, mach2_job_count;
    mach1_job_count = (machines+mach1)->job_count;
    mach2_job_count = (machines+mach2)->job_count;
    int job1_placement, job2_placement;
    job1_placement = randint(mach1_job_count);
    job2_placement = randint(mach2_job_count);
    short job1_id, job2_id;
    job1_id = (machines+mach1)->job_ids[job1_placement];
    job2_id = (machines+mach2)->job_ids[job2_placement];

    /* Remove jobs from machines */
    remove_job_from_machine(machines+mach1, job1_id);
    remove_job_from_machine(machines+mach2, job2_id);
    
    /* Add jobs to machines */
    assign_job_to_machine(machines+mach1, job2_id);
    assign_job_to_machine(machines+mach2, job1_id);
}

/* Function that initializes our temperature */ 
void init_temperature(Temperature *temperature, double start_temp, double end_temp, double alpha, long long max_iters)
{
    temperature->start_temp = start_temp;
    temperature->end_temp = end_temp;
    temperature->alpha = alpha;
    temperature->max_iters = max_iters;
    temperature->current_iter = 0;
    temperature->current_temp = start_temp;
    temperature->terminated = 0;
}

/* Function that updates our temperature for a single iteration */
void update_temperature(Temperature *temperature)
{
    /* Increase current iterations by one */ 
    temperature->current_iter += 1;
    if(temperature->current_iter > temperature->max_iters)
    {
        temperature->terminated = 1;
        return;
    }
    /* Update current temperature by multiplying by alpha */
    temperature->current_temp *= temperature->alpha;
    if(temperature->current_temp < temperature->end_temp)
    {
        temperature->terminated = 1;
        return;
    }
}

/* Function that returns the acceptance probability */
double get_acceptance_probability(Temperature *temperature, long long delta)
{
    return exp((-1*delta)/(temperature->current_temp));
}

/* Function that gets the overall fitness of the solution */
long long get_fitness(Machine *machines)
{
    long long worst_fitness = INT_MIN; /* We initialize the worst fitness to the lowest integer we have */
    for (int i = 0; i < MACHINE_COUNT; i++)
    {
        if (((machines+i)->makespan) >= worst_fitness)
        {
            worst_fitness = ((machines+i)->makespan);
        } 
    }
    return worst_fitness;
}

/* Function that performs the simulated annealing for us */ 
void simulated_annealing(Machine *working_solution, Temperature *temperature, Machine *best_solution)
{
    /* Initialize current fitness, and set best fitness to current fitness as a starting value. We also set the working fitness. */
    long long working_fitness = get_fitness(working_solution);
    long long best_fitness = working_fitness;
    long long current_fitness = working_fitness;

    /* Allocate some memory that can store a solution. 
    This is the memory we will copy current soluitions to, 
    such that we can restore it in case the candidate solution is not accepted. */
    Machine *current_solution;
    current_solution = malloc(sizeof(Machine) * MACHINE_COUNT);

    /* Copy the initial solution to both the current solution and the best solution */
    memcpy(current_solution, working_solution, sizeof(Machine) * MACHINE_COUNT); /* Copy to current */
    memcpy(best_solution, working_solution, sizeof(Machine) * MACHINE_COUNT); /* Copy to best */
    
    /* We now change the working solution by performing either a move or a swap.
    We perform moves and swaps at the same rate for now. */
    int moves = 5;
    int swaps = 5;

    /* Declare delta as it is used for determining the acceptance probability later on */ 
    long long delta;

    /* We start an infinite loop to start our simulated annealing. This loop is exited once the termination criterion is met */
    while(1)
    {
        /* We get a neighbour solution
        We randomly choose between a move and a swap. 
        If it is the case that a single machine contains ALL jobs, we always move */
        if (randint(moves+swaps) <= moves || get_occupied_machines(working_solution) == 1)
        {   
            move(working_solution);
        }
        else
        {
            swap(working_solution);
        }

        /* Get fitness of new solution */
        working_fitness = get_fitness(working_solution);
        
        /* Get delta */
        delta = working_fitness - current_fitness;
        
        /* If the new solution is better than the current solution, we accept it as our current solution */ 
        if (working_fitness < current_fitness)
        {
            /* Store the working solution as our current solution */
            memcpy(current_solution, working_solution, sizeof(Machine) * MACHINE_COUNT);
            /* Set current fitness as working fitness */
            current_fitness = working_fitness;

            /* Check if the newly found solution is also better than the best solution so far.
            If it is, we also store it as our best solution. */
            if (current_fitness < best_fitness)
            {
                /* Store the working solution as our best solution */
                memcpy(best_solution, current_solution, sizeof(Machine) * MACHINE_COUNT);
                /* Set best fitness as working fitness */
                best_fitness = current_fitness;
            }
        }
        /* If the new solution is NOT better than the current solution, we accept it as our current solution only with a certain probability. 
        As it is not better than the current solution, it is also never better than the best solution so we do not have to touch the best solution */
        else if ( ( (float)rand() ) / ( (float)(RAND_MAX) )  < get_acceptance_probability(temperature, delta) )
        {
            /* Store the working solution as our current solution */
            memcpy(current_solution, working_solution, sizeof(Machine) * MACHINE_COUNT);
            /* Set current fitness as working fitness */
            current_fitness = working_fitness;
        }
        /* If the working solution is not accepted as our current solution, we have to restore the current solution to our working solution */
        else
        {
            memcpy(working_solution, current_solution, sizeof(Machine) * MACHINE_COUNT);
        }

        /* Once we have explored a new solution, we have to update the temperature. */
        update_temperature(temperature);
        
        /* Print temperature and iterations every x iterations */ 
        if (temperature->current_iter % 100000000 == 0)
        {
            printf("Current iteration is %lld, current temperature is %f \n", temperature->current_iter, temperature->current_temp);
            printf("Best fitness is %lld\n", best_fitness);
        }       

        /* Then, if the termination criterion is met, we end the simulated annealing by exiting this function. */
        if (temperature->terminated != 0)
        {
            printf("Finished simulated annealing. Exiting function. \n");
            return;
        }
    }
}

/* Function that retrieves the number of occupied machines. This is needed because swaps are impossible when only 1 machine is occupied. */
int get_occupied_machines(Machine *machines)
{
    int occ_machines = 0;

    for (int i = 0; i < MACHINE_COUNT; i++)
    {
        if (((machines+i)->job_count) >= 1)
        {
            occ_machines++;
        } 
    }
    return occ_machines;
}