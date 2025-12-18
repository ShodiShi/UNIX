#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;   // Условная переменная
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;  // Мьютекс для синхронизации
int ready = 0;                                     // Флаг готовности данных

// Функция поставщика
void* supplier(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);    // Захват мьютекса
        if (ready == 1) {
            pthread_mutex_unlock(&lock);  // Если данные уже готовы, отпускаем мьютекс и продолжаем
            continue;
        }

        ready = 1;   // Устанавливаем флаг готовности
        printf("Provided\n");

        pthread_cond_signal(&cond1);  
        pthread_mutex_unlock(&lock);  

        sleep(1);  
    }
    return NULL;
}

// Функция потребителя
void* consumer(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);    // Захват мьютекса
        while (ready == 0) {          // Ожидание, пока данные не будут готовы
            pthread_cond_wait(&cond1, &lock);  // Освобождаем мьютекс и ждем сигнала
        }

        ready = 0;   // Сбрасываем флаг после потребления данных
        printf("Consumed\n");

        pthread_mutex_unlock(&lock);  

        sleep(2);  // Имитация времени обработки данных
    }
    return NULL;
}

int main() {
    pthread_t supplier_thread, consumer_thread;

    // Создание потоков
    pthread_create(&supplier_thread, NULL, supplier, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    
    pthread_join(supplier_thread, NULL);
    pthread_join(consumer_thread, NULL);

    return 0;
}