#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


pthread_mutex_t mutex;

int sum = 0;

void print_vector(char name, int *v, int size) {
    printf("%c = ", name);
    int i;
    for (i = 0; i < size; i++) {
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
    int i;  
    for (i = 0; i < size; i++) {
        slice[i] = v[offset + i];
    }
    return slice;
}

typedef struct vec_tuple {
    int* vec1;
    int* vec2;
    int vec_size;
    int thread_number;
    int offset;
} vec_tpl;

void *calc(void *arg) {
    vec_tpl *vecs = (vec_tpl*)arg;

    int i;
    int partial = 0;
    for (i = 0; i < vecs->vec_size; i++) {
        partial += vecs->vec1[i] * vecs->vec2[i];
    }
    pthread_mutex_lock(&mutex); 
    sum += partial;
    pthread_mutex_unlock(&mutex);


    printf("Thread %i calculou de %i a %i: produto escalar parcial = %i\n", 
           vecs->thread_number, 
           vecs->offset, 
           vecs->offset + vecs->vec_size-1, 
           partial);

    pthread_exit(NULL);
}   


int main(int argc, char const *argv[]) {

    // verifica os parametros do programa
    if(argv[1]==NULL || argv[2]==NULL){
        printf("ERRO: Utilize o formato <nome_do_programa> <vector_size> <num_threads>\n");
        return 1;
    }

    pthread_mutex_init(&mutex, NULL);

    int VECSIZE = atoi(argv[1]);
    int NTHREADS = atoi(argv[2]);

    int vector1[VECSIZE];
    int vector2[VECSIZE];

    srand(time(NULL));

    // inicializa os vetores com valores aleatorios
    int i;
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
    int rest = VECSIZE % NTHREADS;

    // cria subduplas de vetores dividindo igualmente, se possivel
    vec_tpl subvectors[NTHREADS];

    int pos = 0;
    for (i = 0; i < NTHREADS; i++) {
        int range = chunk_size;

        // se houver resto na divisao,
        // os primeiros subvetores recebem mais um valor
        if(rest>0 && i < rest){
            range++;
        }
        subvectors[i].vec1 = slice(pos, range, vector1);
        subvectors[i].vec2 = slice(pos, range, vector2);
        subvectors[i].vec_size = range;
        subvectors[i].thread_number = i+1; 
        subvectors[i].offset = pos; 
        pos += range;
    }


    // cria uma thread para cada subdupla de vetor
    // responsaveis pelo subcalculo do produto escalar
    pthread_t threads[NTHREADS];

    for (i = 0; i < NTHREADS; i++) {
        pthread_create(&threads[i], NULL, calc, (void*)(&subvectors[i]));
    }
    
    // sincroniza apos o termino de todas as threads
    for (i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Produto escalar = %i\n", sum);

    pthread_mutex_destroy(&mutex);
    return 0;
}
