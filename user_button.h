#ifndef USER_BUTTON_H
#define USER_BUTTON_H

#include <miosix.h>
#include <pthread.h>
#include <functional>

using namespace miosix;

class UserButton{
    private:
        typedef Gpio<GPIOA_BASE,0> buttonPin;
        bool started = false;
        bool buttonPressed = false;
        bool buttonPressedOld = false;

        // Callback function to be called when the button is pressed
        std::function<void()> callback;

        pthread_t buttonThread;
        pthread_mutex_t buttonMutex = PTHREAD_MUTEX_INITIALIZER;

        // Function to check if the button is pressed
        static void *buttonThreadFunction(void *arg);

        // Private constructor
        UserButton();

        // Singleton instance
        static UserButton* instance;

    public:
        // Deleted copy constructor and assignment operator
        UserButton(const UserButton&) = delete;
        UserButton& operator=(const UserButton&) = delete;

        // Static method to get the instance
        static UserButton* getInstance();

        void start();
        void close();
        bool isButtonPressed();
        void callCallback();
        void setCallback(std::function<void()> callback);
        void removeCallback();
};

#endif