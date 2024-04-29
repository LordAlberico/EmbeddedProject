#include "user_button.h"

UserButton* UserButton::instance = nullptr;

UserButton::UserButton(){
    buttonPin::mode(Mode::INPUT);
    buttonPressed = false;
}

UserButton* UserButton::getInstance() {
    if (instance == nullptr) {
        instance = new UserButton();
    }
    return instance;
}

void UserButton::setCallback(std::function<void()> callback) {
    this->callback = callback;
}

void UserButton::removeCallback() {
    callback = NULL;
}

void UserButton::start(){
    started = true;
    pthread_create(&buttonThread, NULL, buttonThreadFunction, this);
}

void UserButton::close(){
    started = false;
    callback = NULL;
    pthread_join(buttonThread, NULL);
}

bool UserButton::isButtonPressed(){
    return buttonPressed;
}

void UserButton::callCallback(){
    pthread_mutex_lock(&buttonMutex);
    if (buttonPressed) {
        callback();
        buttonPressed = false;
    }
    pthread_mutex_unlock(&buttonMutex);
}

void *UserButton::buttonThreadFunction(void *arg){
    UserButton *self = static_cast<UserButton*>(arg);
    while(self->started) {
        pthread_mutex_lock(&self->buttonMutex);
        if (buttonPin::value() == 1 && !self->buttonPressedOld) {
            self->buttonPressed = true;
            self->buttonPressedOld = true;
        }
        else if (buttonPin::value() == 0) {
            self->buttonPressedOld = false;
        }
        pthread_mutex_unlock(&self->buttonMutex);
        Thread::sleep(10);
    }
    return NULL;
}