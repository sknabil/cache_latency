#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <fstream>
//#include <mutex>

using namespace std;

/*static inline uint64_t rdtsc1() {
  uint64_t a, d;
  asm volatile ("cpuid" ::: "rax","rbx","rcx","rdx");
  asm volatile ("rdtscp" : "=a" (a), "=d" (d) : : "rcx");
  a = (d<<32) | a;
  return a;
}
static inline uint64_t rdtsc2() {
  uint64_t a, d;
  asm volatile ("rdtscp" : "=a" (a), "=d" (d) : : "rcx");
  asm volatile ("cpuid" ::: "rax","rbx","rcx","rdx");
  a = (d<<32) | a;
  return a;
}
*/

static inline uint64_t rdtsc1(void)
{
  unsigned  cycles_low, cycles_high;
  asm volatile( "CPUID\n\t" // serialize
                "RDTSC\n\t" // read clock
                "MOV %%edx, %0\n\t"
                "MOV %%eax, %1\n\t"
                "mfence\n\t"
                : "=r" (cycles_high), "=r" (cycles_low)
                :: "%rax", "%rbx", "%rcx", "%rdx" );
  return ((uint64_t) cycles_high << 32) | cycles_low;
}

// bench_end returns a timestamp for use to measure the end of a benchmark run.
static inline uint64_t rdtsc2(void)
{
  unsigned  cycles_low, cycles_high;
  asm volatile( "RDTSCP\n\t" // read clock + serialize
                "MOV %%edx, %0\n\t"
                "MOV %%eax, %1\n\t"
                "CPUID\n\t" // serialze -- but outside clock region!
                : "=r" (cycles_high), "=r" (cycles_low)
                :: "%rax", "%rbx", "%rcx", "%rdx" );
  return ((uint64_t) cycles_high << 32) | cycles_low;
}



static inline void clflush(volatile void *p)
{
    asm volatile ("clflush (%0)" :: "r"(p));
    //__asm__ __volatile__ ("clflush (%0)" :: "r"(p));
}


int i = 0, iteration = 0;

uint64_t a = 0;

// We are using the variable v2 to measure the latency
volatile uint64_t v2;

// Thread synchronization
int flag = -1;

/*
struct StructData {
    //uint64_t v1;
    uint64_t v2;
    //uint64_t v3;
    /*uint64_t v4;
    uint64_t v5;
    uint64_t v6;
    uint64_t v7;
    uint64_t v8;
    uint64_t v9;
    uint64_t v10;
    uint64_t v11;
    uint64_t v12;
    uint64_t v13;
    uint64_t v14;
    uint64_t v15;
};*/


//struct StructData data = { 10};//,11,12};//,13,14,15,16,17,18,19 };

ofstream testfile("output.txt", ios::out);

//mutex m;
pthread_mutex_t lock;

uint64_t calculateFinalLatency(uint64_t t1s, uint64_t t1e, uint64_t t2s, uint64_t t2e, uint64_t t3s, uint64_t t3e){
    uint64_t l1, l2, l3, o_avg, final_latency;

    l1 = t1e - t1s;
    l2 = t2e - t2s;
    l3 = t3e - t3s;

    o_avg = (l1+l3) / 2;

    final_latency = ((l2 < o_avg) ? 0: l2 - o_avg);

    cout << "Latency - avg overhead: "<< l2 <<" - " << o_avg << " = " << ((l2 < o_avg) ? 0: l2 - o_avg) << endl;

    return final_latency;
}


// Thread T1
void *threadT1(void *vargp){

    if(flag == -1){
        pthread_mutex_lock(&lock);
        cout << "T1 first call, LOCKS"<<endl;
        flag = 0;
    }

    while(true){

        if(flag == 0) {
            cout << "T1 flag =0, Iteration:" << iteration << endl;
            break;
        }

    }

    if(flag == 0){
        pthread_mutex_unlock(&lock);

        pthread_mutex_lock(&lock);

        uint64_t tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;


        //sleep(1);

        //pthread_mutex_lock(&lock);

        a = 10;
        v2 = 100;
        a = v2;
        cout << "Value2 : " << v2 <<" a :" << a<< endl;


        //clflush(&value2);
        //unique_lock<mutex> lk(m);
        //std::this_thread::sleep_for(std::chrono::milliseconds(300));


        cout << "Test func1: " << getpid()<< endl;
        cout << "Thread 1 Core: " << sched_getcpu() << endl;

        clflush(&v2);
        //value = 10;
        //cout << "Thread T1: " << value <<endl;
        //lk.unlock();
        //clflush(&value2);

        for(int counter = 0; counter <14; counter++)
        {

            tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;

            //rdtsc2();
            //rdtsc1();
            tick1_start = rdtsc1();
            // Nothing. Just the overhead

            tick1_end = rdtsc2();
            //rdtsc2();

            //rdtsc1();
            tick2_start = rdtsc1();

            // TEST LATENCY HERE
            //c = b;
            if( counter == 3 || counter ==  7|| counter == 11){
                clflush(&v2);
                //asm volatile ("cpuid" ::: "rax","rbx","rcx","rdx");
            }


            a = v2;

            tick2_end = rdtsc2();
            //rdtsc2();

            //rdtsc1();
            tick3_start = rdtsc1();
            // Nothing. Just the overhead

            tick3_end = rdtsc2();
            //rdtsc2();


            /*rdtsc1();
            tick1_start = rdtsc1();
            // Nothing. Just the overhead

            tick1_end = rdtsc2();
            rdtsc2();

            rdtsc1();
            tick2_start = rdtsc1();

            // TEST LATENCY HERE
            //c = b;
            if( counter == 5 || counter == 10 || counter == 15|| counter == 30)
                clflush(&v2);

            a = v2;

            tick2_end = rdtsc2();
            rdtsc2();

            rdtsc1();
            tick3_start = rdtsc1();
            // Nothing. Just the overhead

            tick3_end = rdtsc2();
            rdtsc2();*/

            cout << "Tick1 (overhead): " << tick1_end - tick1_start << endl;
            cout << "Tick2 (latency) : "<<counter<<"$ " << tick2_end - tick2_start << endl;
            cout << "Tick1 (overhead): " << tick3_end - tick3_start << endl;

            cout << "Timing ends: ###"<< endl;

            uint64_t lat = calculateFinalLatency(tick1_start, tick1_end, tick2_start, tick2_end, tick3_start, tick3_end);
            //writeToFile("TT,");


            if(testfile.is_open()){
                testfile << lat << "\t";
                //testfile.close();
            }
            else {
                cout << "Error file opening.";
            }
            //sleep(0.005);
        }

        if(testfile.is_open()){
                testfile << "\t";
                //testfile.close();
            }
            else {
                cout << "Error file opening.";
        }

        flag = 1;
        cout << "Thread 1 ENDS::" <<endl << endl << endl;

    }
    //pthread_mutex_unlock(&lock);

}

// Thread T2
void *threadT2(void *vargp){


    // Set cpu affinity
    const int core_id1 = 1;
	const pid_t pid1 = getpid();

	// cpu_set_t: This data set is a bitset where each bit represents a CPU.
	cpu_set_t  cpuset1;
	// CPU_ZERO: This macro initializes the CPU set set to be the empty set.
	CPU_ZERO(&cpuset1);
	// CPU_SET: This macro adds cpu to the CPU set set.
	CPU_SET(core_id1, &cpuset1);

	int rc1 = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset1);

    if(rc1 != 0) {
        std::cerr << "Error calling pthread: " << rc1 << "\n";
    }


    while(true){

        if(flag == 1){

            cout << "T2 flag =0, Iteration:" << iteration << endl;
            break;
        }
    }
    if(flag == 1){
        pthread_mutex_unlock(&lock);

        pthread_mutex_lock(&lock);

        uint64_t tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;

        //sleep(4);

        //pthread_mutex_lock(&lock);

            //unique_lock<mutex> lk(m);
            //std::this_thread::sleep_for(std::chrono::milliseconds(300));

        cout << "Test func2: " << getpid()<< endl;
        cout << "Thread 2 Core: " << sched_getcpu() << endl;

        //cout << "Thread T2: " << value<<endl;
        //lk.unlock();

        tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;


        //rdtsc2();
        //rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE
        //c = b;
        a = v2;

        tick2_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        //rdtsc2();
        /*rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        rdtsc2();


        rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE
        //c = b;
        a = v2;

        tick2_end = rdtsc2();
        rdtsc2();

        rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        rdtsc2();*/

        cout << "Tick1 (overhead): " << tick1_end - tick1_start << endl;
        cout << "Tick2 (latency) : " << tick2_end - tick2_start << endl;
        cout << "Tick1 (overhead): " << tick3_end - tick3_start << endl;

        cout << "Timing ends: "<< endl;
        uint64_t lat = calculateFinalLatency(tick1_start, tick1_end, tick2_start, tick2_end, tick3_start, tick3_end);
        //writeToFile(",");

        if(testfile.is_open()){
            testfile << lat << "\t";
            //testfile.close();
        }
        else {
            cout << "Error file opening.";
        }

        flag = 2;
        cout << "Thread 2 ENDS::" << lat << endl << endl;


        //pthread_mutex_unlock(&lock);
    }
}

// Thread T3
void *threadT3(void *vargp){

    // Set cpu affinity
     // Set cpu affinity
    const int core_id1 = 2;
	const pid_t pid1 = getpid();

	// cpu_set_t: This data set is a bitset where each bit represents a CPU.
	cpu_set_t  cpuset1;
	// CPU_ZERO: This macro initializes the CPU set set to be the empty set.
	CPU_ZERO(&cpuset1);
	// CPU_SET: This macro adds cpu to the CPU set set.
	CPU_SET(core_id1, &cpuset1);

	int rc1 = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset1);

    if(rc1 != 0) {
        std::cerr << "Error calling pthread: " << rc1 << "\n";
    }

    while(true){

        if(flag == 2){

            cout << "T3 flag =0, Iteration:" << iteration << endl;
            break;
        }
    }
    if(flag == 2){
        pthread_mutex_unlock(&lock);

        pthread_mutex_lock(&lock);
        uint64_t tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;

        //sleep(7);

        //pthread_mutex_lock(&lock);

        //unique_lock<mutex> lk(m);
        //std::this_thread::sleep_for(std::chrono::milliseconds(300));

        cout << "Test func3: " << getpid()<< endl;
        cout << "Thread 3 Core: " << sched_getcpu() << endl;

        //value = 30;
        //cout << "Thread T3: " << value <<endl;
        //lk.unlock();


        tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;


        //rdtsc2();
        //rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE
        //c = b;
        a = v2;

        tick2_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        //rdtsc2();
        /*rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        rdtsc2();


        rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE
        //c = b;
        a = v2;

        tick2_end = rdtsc2();
        rdtsc2();

        rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        rdtsc2();*/

        cout << "Tick1 (overhead): " << tick1_end - tick1_start << endl;
        cout << "Tick2 (latency) : " << tick2_end - tick2_start << endl;
        cout << "Tick1 (overhead): " << tick3_end - tick3_start << endl;

        cout << "Timing ends: "<< endl;
        uint64_t lat = calculateFinalLatency(tick1_start, tick1_end, tick2_start, tick2_end, tick3_start, tick3_end);
        //writeToFile(",");

        if(testfile.is_open()){
            testfile << lat << "\t";
            //testfile.close();
        }
        else {
            cout << "Error file opening.";
        }

        flag = 3;
        cout << "Thread 3 ENDS::" <<lat<< endl << endl;


        //pthread_mutex_unlock(&lock);
    }

}

// Thread 4
void *threadT4(void *vargp){

    // Set cpu affinity
     // Set cpu affinity
    const int core_id1 = 1;
	const pid_t pid1 = getpid();

	// cpu_set_t: This data set is a bitset where each bit represents a CPU.
	cpu_set_t  cpuset1;
	// CPU_ZERO: This macro initializes the CPU set set to be the empty set.
	CPU_ZERO(&cpuset1);
	// CPU_SET: This macro adds cpu to the CPU set set.
	CPU_SET(core_id1, &cpuset1);

	int rc1 = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset1);

    if(rc1 != 0) {
        std::cerr << "Error calling pthread: " << rc1 << "\n";
    }

    while(true){

        if(flag == 3){

            cout << "T4 flag =0, Iteration:" << iteration << endl;
            break;
        }
    }
    if(flag == 3){
        pthread_mutex_unlock(&lock);

        pthread_mutex_lock(&lock);

        uint64_t tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;

        //sleep(10);

        //pthread_mutex_lock(&lock);

        //unique_lock<mutex> lk(m);

        //std::this_thread::sleep_for(std::chrono::milliseconds(300));

        cout << "Test func4: " << getpid()<< endl;
        cout << "Thread 4 Core: " << sched_getcpu() << endl;

        //value = 30;
        //cout << "Thread T4: " << value <<endl;
        //lk.unlock();


        tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;


        //rdtsc2();
        //rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE
        //c = b;
        v2 = 20L;

        tick2_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        //rdtsc2();
        /*rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        rdtsc2();

        rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE

        //a = value2;
        v2 = 20L;

        tick2_end = rdtsc2();
        rdtsc2();

        rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        rdtsc2();*/

        cout << "Tick1 (overhead): " << tick1_end - tick1_start << endl;
        cout << "Tick2 (latency) : " << tick2_end - tick2_start << endl;
        cout << "Tick1 (overhead): " << tick3_end - tick3_start << endl;

        cout << "Timing ends: "<< endl;

        uint64_t lat = calculateFinalLatency(tick1_start, tick1_end, tick2_start, tick2_end, tick3_start, tick3_end);
        //writeToFile(",");

        if(testfile.is_open()){
            testfile << lat << "\t";
            //testfile.close();
        }
        else {
            cout << "Error file opening.";
        }

        flag = 4;
        cout << "Thread 4 ENDS::" << lat<< endl << endl;


        //pthread_mutex_unlock(&lock);
    }

}

// Thread 5
void *threadT5(void *vargp){

    // Set cpu affinity
    const int core_id1 = 2;
	const pid_t pid1 = getpid();

	// cpu_set_t: This data set is a bitset where each bit represents a CPU.
	cpu_set_t  cpuset1;
	// CPU_ZERO: This macro initializes the CPU set set to be the empty set.
	CPU_ZERO(&cpuset1);
	// CPU_SET: This macro adds cpu to the CPU set set.
	CPU_SET(core_id1, &cpuset1);

	int rc1 = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset1);

    if(rc1 != 0) {
        std::cerr << "Error calling pthread: " << rc1 << "\n";
    }

     while(true){

        if(flag == 4){

            cout << "T5 flag =0, Iteration:" << iteration << endl;
            break;
        }
    }
    if(flag == 4){
        pthread_mutex_unlock(&lock);

        pthread_mutex_lock(&lock);

        uint64_t tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;

        //sleep(13);

        //pthread_mutex_lock(&lock);

        //unique_lock<mutex> lk(m);

        //std::this_thread::sleep_for(std::chrono::milliseconds(300));

        cout << "Test func5: " << getpid()<< endl;
        cout << "Thread 5 Core: " << sched_getcpu() << endl;

        //value = 30;
        //cout << "Thread T5: " << value <<endl;
        //lk.unlock();


        tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;


        //rdtsc2();
        //rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE
        //c = b;
        a = v2;

        tick2_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        //rdtsc2();
        /*rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        rdtsc2();


        rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE
        //c = b;
        a = v2;

        tick2_end = rdtsc2();
        rdtsc2();

        rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        rdtsc2();*/

        cout << "Tick1 (overhead): " << tick1_end - tick1_start << endl;
        cout << "Tick2 (latency) : " << tick2_end - tick2_start << endl;
        cout << "Tick1 (overhead): " << tick3_end - tick3_start << endl;

        cout << "Timing ends: "<< endl;
        uint64_t lat = calculateFinalLatency(tick1_start, tick1_end, tick2_start, tick2_end, tick3_start, tick3_end);
        //writeToFile(",");

        if(testfile.is_open()){
            testfile << lat << "\t";
            //testfile.close();
        }
        else {
            cout << "Error file opening.";
        }
        flag = 5;

        cout << "Thread 5 ENDS::" << lat<< endl << endl;


        //pthread_mutex_unlock(&lock);
    }

}

// Thread 6
void *threadT6(void *vargp){

    // Set cpu affinity
     // Set cpu affinity
    const int core_id1 = 3;
	const pid_t pid1 = getpid();

	// cpu_set_t: This data set is a bitset where each bit represents a CPU.
	cpu_set_t  cpuset1;
	// CPU_ZERO: This macro initializes the CPU set set to be the empty set.
	CPU_ZERO(&cpuset1);
	// CPU_SET: This macro adds cpu to the CPU set set.
	CPU_SET(core_id1, &cpuset1);

	int rc1 = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset1);

    if(rc1 != 0) {
        std::cerr << "Error calling pthread: " << rc1 << "\n";
    }

    while(true){

        if(flag == 5){
            cout << "T6 flag =0, Iteration:" << iteration << endl;
            break;
        }
    }

     if(flag == 5){
        pthread_mutex_unlock(&lock);
        pthread_mutex_lock(&lock);

        uint64_t tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;

        //sleep(16);

        //pthread_mutex_lock(&lock);

        //unique_lock<mutex> lk(m);
        //std::this_thread::sleep_for(std::chrono::milliseconds(300));

        cout << "Test func6: " << getpid()<< endl;
        cout << "Thread 6 Core: " << sched_getcpu() << endl;

        //value = 30;
        //cout << "Thread T6: " << value <<endl;
        //lk.unlock();


        tick1_start = 0, tick1_end = 0, tick2_start = 0, tick2_end = 0, tick3_start = 0, tick3_end = 0;


        //rdtsc2();
        //rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE
        //c = b;
        a = v2;

        tick2_end = rdtsc2();
        //rdtsc2();

        //rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        //rdtsc2();
        /*rdtsc1();
        tick1_start = rdtsc1();
        // Nothing. Just the overhead

        tick1_end = rdtsc2();
        rdtsc2();


        rdtsc1();
        tick2_start = rdtsc1();

        // TEST LATENCY HERE
        //c = b;
        a = v2;

        tick2_end = rdtsc2();
        rdtsc2();

        rdtsc1();
        tick3_start = rdtsc1();
        // Nothing. Just the overhead

        tick3_end = rdtsc2();
        rdtsc2();*/

        cout << "Tick1 (overhead): " << tick1_end - tick1_start << endl;
        cout << "Tick2 (latency) : " << tick2_end - tick2_start << endl;
        cout << "Tick1 (overhead): " << tick3_end - tick3_start << endl;

        cout << "Timing ends: "<< endl;
        uint64_t lat = calculateFinalLatency(tick1_start, tick1_end, tick2_start, tick2_end, tick3_start, tick3_end);
        //writeToFile("\n");

        if(testfile.is_open()){
            testfile << lat << "\n";
            //testfile.close();
        }
        else {
            cout << "Error file opening.";
        }
        flag = 0;

        cout << "Thread 6 ENDS::" << lat << endl << endl;


        //pthread_mutex_unlock(&lock);
     }

}

void runThread()
{
    //cout << "PID:: " << getpid() << endl;
    //std::thread t1(func1);
    pthread_t t1, t2, t3, t4, t5, t6;
    pthread_create(&t1, NULL, threadT1, NULL);
    pthread_create(&t2, NULL, threadT2, NULL);
    pthread_create(&t3, NULL, threadT3, NULL);
    pthread_create(&t4, NULL, threadT4, NULL);
    pthread_create(&t5, NULL, threadT5, NULL);
    pthread_create(&t6, NULL, threadT6, NULL);


    //t1.join();
    //t2.join();
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);
    pthread_join(t6, NULL);
}

int main(){

    cout << "PID:: " << getpid() << endl;

	const int core_id = 1;
	const pid_t pid = getpid();

	// cpu_set_t: This data set is a bitset where each bit represents a CPU.
	cpu_set_t  cpuset;
	// CPU_ZERO: This macro initializes the CPU set set to be the empty set.
	CPU_ZERO(&cpuset);
	// CPU_SET: This macro adds cpu to the CPU set set.
	CPU_SET(core_id, &cpuset);

	/*
	sched_setaffinity: This function installs the cpusetsize bytes long affinity mask pointed
	to by cpuset for the process or thread with the ID pid. If successful the function returns
	zero and the scheduler will in future take the affinity information into account.
	*/

	const int set_result = sched_setaffinity(pid, sizeof(cpu_set_t), &cpuset);
	if (set_result != 0) {
        //printf("sched_setaffinity ERROR: %d \n", set_result);exit(1);
        cout << "sched_setaffinity ERROR: " << set_result << endl;
        exit(1);
	}

    //clflush(&value2);

    for(int i = 0; i < 30; i++)
    {
        cout << "Iteration # " << i << " Starts" << endl;
        runThread();
        sleep(1);
        iteration++;
        cout << "Iteration # "<< i << " Ends" << endl;
    }


    if(testfile.is_open()){
        //testfile << lat << "\n";
        testfile.close();
    }
    else {
        cout << "Error file opening.";
    }
	//std::this_thread::sleep_for(std::chrono::milliseconds(20000));


    return 0;
}
