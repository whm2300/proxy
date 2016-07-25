#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "define.h"

#include <new>

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

template<typename T>
class Singleton {
    public:
        static T *GetInstance() {
            if (_instance == NULL) {
                pthread_mutex_lock(&_mutex);
                if (_instance == NULL) {
                    _instance = new(std::nothrow) T;
                    if (_instance == NULL) {
                        abort();
                    }
                }
                pthread_mutex_unlock(&_mutex);
            }
            return _instance;
        }

    private:
        Singleton() {}
        ~Singleton() {}
        DISALLOW_COPY_AND_ASSIGN(Singleton);

    private:
        static T *_instance;
        static pthread_mutex_t _mutex;

        /*
        class garbo {
            public:
                garbo() { printf("garbo()\n"); }
                ~garbo() {
                    if (Singleton<T>::_instance != NULL) {
                        delete Singleton<T>::_instance;
                        Singleton<T>::_instance = NULL;
                    }
                    printf("~garbo()");
                }
        };
        static garbo _garbo;
        */
};

template<typename T>
T *Singleton<T>::_instance = NULL;

template<typename T>
pthread_mutex_t Singleton<T>::_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
template<typename T>
typename Singleton<T>::garbo Singleton<T>::_garbo;
*/

#endif  // SINGLETON_H_
