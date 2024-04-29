// #ifndef HCSR04_h
// #define HCSR04_h

// #include <miosix.h>
// #include <pthread.h>

// using namespace miosix;

// class HCSR04{
//     private:
//         typedef Gpio<GPIOA_BASE,1> echoPin;
//         typedef Gpio<GPIOA_BASE,2> triggerPin;

//         bool started = false;
//         bool dataReady = false;
//         pthread_t distanceThread;
//         pthread_mutex_t distanceMutex = PTHREAD_MUTEX_INITIALIZER;
//         static void *calculateDistanceThreadFunction(void *arg);

//         float distanceSaved = 0;

//         // Private constructor
//         HCSR04();

//         // Singleton instance
//         static HCSR04* instance;
//     public:
//         void calculateDistance();
//         void begin();
//         void close();
//         static HCSR04* getInstance();

//         bool isDataReady();
//         float getDistance();
// };

// #endif