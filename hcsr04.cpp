// #include "hcsr04.h"

// using namespace miosix;

// HCSR04* HCSR04::instance = nullptr;

// HCSR04::HCSR04() {
//     triggerPin::mode(Mode::OUTPUT);
//     echoPin::mode(Mode::INPUT);
//     triggerPin::low();
// }

// void HCSR04::begin() {
//     started = true;
//     pthread_create(&distanceThread, NULL, calculateDistanceThreadFunction, this);
// }

// void HCSR04::close() {
//     started = false;
//     pthread_join(distanceThread, NULL);
// }

// HCSR04* HCSR04::getInstance() {
//     if (instance == nullptr) {
//         instance = new HCSR04();
//     }
//     return instance;
// }

// void HCSR04::calculateDistance() {
//     // Trigger the sensor
//     triggerPin::high();
//     Thread::nanoSleep(10000); // 10us
//     triggerPin::low();

//     // Wait for the echo
//     long long start, end;
//     while(echoPin::value() == 0); // Wait for the echo to be high
//     start = getTime();
//     while(echoPin::value() == 1); // Wait for the echo to be low
//     end = getTime();

//     // Calculate the time in us
//     long long time = (end - start) / 1000;

//     // If time grate than 38ms, the distance is too big
//     if(time > 38000) {
//         pthread_mutex_lock(&distanceMutex);
//         distanceSaved = -1;
//         dataReady = true;
//         pthread_mutex_unlock(&distanceMutex);
//         return;
//     }

//     // Calculate the distance
//     pthread_mutex_lock(&distanceMutex);
//     distanceSaved = time / 58.0;
//     dataReady = true;
//     pthread_mutex_unlock(&distanceMutex);
// }

// void *HCSR04::calculateDistanceThreadFunction(void *arg) {
//     HCSR04 *self = static_cast<HCSR04*>(arg);
//     while(self->started) {
//         self->calculateDistance();
//         Thread::sleep(60); // Suggested time to wait between measurements
//     }
//     return NULL;
// }

// float HCSR04::getDistance() {
//     float distance;
//     pthread_mutex_lock(&distanceMutex);
//     distance = distanceSaved;
//     pthread_mutex_unlock(&distanceMutex);
//     return distance;
// }

// bool HCSR04::isDataReady() {
//     bool ready;
//     pthread_mutex_lock(&distanceMutex);
//     ready = dataReady;
//     dataReady = false;
//     pthread_mutex_unlock(&distanceMutex);
//     return ready;
// }
