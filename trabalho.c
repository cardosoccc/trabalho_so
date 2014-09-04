#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void print_vector(char name, int *v, int size) {
    printf("%c: ", name);
    for (int i = 0; i < size; i++) {
        printf("%i", v[i]);
        if (i == size - 1) {
            printf(" \n");
        } else {
            printf(", ");
        }
    }
}

int* slice(int offset, int size, int *v) {
    int *slice = (int*)malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) {
        slice[i] = v[offset + i];
    }
    return slice;
}

typedef struct vec_tuple {
    int* vec1;
    int* vec2;
    int vec_size;
} vec_tpl;

void *calc(void *arg) {
    // pthread_mutex_lock(&mutex); 
    // //região    críZca  
    // pthread_mutex_unlock(&mutex);   
    vec_tpl *vecs = (vec_tpl*)arg;
    print_vector('X', vecs->vec1, vecs->vec_size);
    print_vector('y', vecs->vec2, vecs->vec_size);
    pthread_exit(NULL);
}   

// int *vetor = (int *) malloc(sizeof(int) * 10)

int sum = 0;

int main(int argc, char const *argv[]) {
    
    int VECSIZE = 19;
    int NTHREADS = 7;

    int vector1[VECSIZE];
    int vector2[VECSIZE];

    int i = 0;

    srand(time(NULL));

    for (i = 0; i < VECSIZE; i++) {
        vector1[i] = rand() % 10;
        vector2[i] = rand() % 10;
    }

    print_vector('A', vector1, VECSIZE);
    print_vector('B', vector2, VECSIZE);

    if (NTHREADS > VECSIZE) {
        NTHREADS = VECSIZE;
    }

    int chunk_size = VECSIZE / NTHREADS;
    

    pthread_t threads[NTHREADS];

    for (i = 0; i < NTHREADS; i++) {
        vec_tpl subvectors;
        subvectors.vec1 = slice(i, chunk_size, vector1);
        subvectors.vec2 = slice(i, chunk_size, vector2);
        subvectors.vec_size = chunk_size;
        pthread_create(&threads[i], NULL, calc, (void*)&subvectors);
    }

    return 0;
}