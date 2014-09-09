#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void print_vector(char name, int *v, int size) {
    printf("%c: ", name);
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
} vec_tpl;

void *calc(void *arg) {
    // pthread_mutex_lock(&mutex); 
    // //região    críZca  
    // pthread_mutex_unlock(&mutex);   
    vec_tpl *vecs = (vec_tpl*)arg;
    printf("--\n");
    print_vector('X', vecs->vec1, vecs->vec_size);
    print_vector('y', vecs->vec2, vecs->vec_size);
    pthread_exit(NULL);
}   

// int *vetor = (int *) malloc(sizeof(int) * 10)

int sum = 0;

int main(int argc, char const *argv[]) {

	// verifica os parametros do programa
	if(argv[1]==NULL || argv[2]==NULL){
		printf("ERRO: Utilize o formato <nome_do_programa> <vector_size> <num_threads>\n");
		return 1;
	}

    int VECSIZE = atoi(argv[1]);
    int NTHREADS = atoi(argv[2]);

	printf("VECSIZE: %d\tNTHREADS: %d\n", VECSIZE, NTHREADS);

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

        pos += range;
    }


    // cria uma thread para cada subdupla de vetor
    // responsaveis pelo subcalculo do produto escalar
    pthread_t threads[NTHREADS];

    for (i = 0; i < NTHREADS; i++) {
        pthread_create(&threads[i], NULL, calc, (void*)(&subvectors[i]));
        pthread_join(threads[i], NULL); //APENAS PARA TESTES
    }

    pthread_exit(NULL);
}