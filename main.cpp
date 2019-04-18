
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


struct header_t{
    size_t size;
    unsigned is_free;
    struct header_t *next;
};
//help keep track of the list
struct header_t *head = NULL, *tail = NULL;
//global lock
pthread_mutex_t global_malloc_lock;

struct header_t *get_free_block(size_t size){
    struct header_t *curr = head;
    while(curr){
        if(curr->is_free && curr->size >= size){
            return curr;
        }
        curr = curr-> next;
    }
    return NULL;
}



//new threaded malloc??
void *malloc(size_t size){
    size_t total_size;
    void *block;
    struct header_t *header;

    //this is for if the parameter size is 0
    if(!size){
        return NULL;
    }
    //aquire the lock, if it has already been aquired then itll block other threads
    pthread_mutex_lock(&global_malloc_lock);
    //checks method to see if there is a block that is already large enough
    header = get_free_block(size);//header data
    //if header has a value
    if(header){
        header->is_free = 0; //marks block as not free
        pthread_mutex_unlock(&global_malloc_lock); //releases the lock
        return(void*)(header + 1); //returns a pointer to that block as void type
    }
    //if i dont find that block then i have to increase the heap
    total_size = sizeof(struct header_t) + size;//must be the total size so the heap canfit it all
    block = sbrk(total_size);
    if(block == (void*) - 1){
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }
    header = (header_t*)block;
    header->size = size; //recommended size not total size
    header->is_free = 0; //marked as not free
    header-> next = NULL; // end of the allocated list

    if(!head){
        head = header;
    }
    if(tail){
        tail->next = header;
    }
    tail = header;
    pthread_mutex_unlock(&global_malloc_lock);
    return(void*)(header+1);
}

void free(void *block){
    struct header_t *header, *temp;
    void *programbreak;
    //if the block has no value
    if(!block){
        return;
    }
    pthread_mutex_lock(&global_malloc_lock);//acquire the lock
    header = (struct header_t*)block - 1;
    //first data segment after the uninitialized segment
    programbreak = sbrk(0);
    //end of the current block
    if((char*)block + header->size == programbreak){
        //if it is at the end of the heap
        if(head == tail){
            head = tail = NULL;//the last block has been freed
        }else{
            temp = head;
            //loop until you get to the end of the heap
            while(temp){
                if(temp->next == tail){
                    temp->next = NULL;
                    tail = temp;
                }
                temp = temp->next;
            }
        }
        //how much is going to be released
        sbrk(0 - sizeof(struct header_t) - header->size);
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    header-> is_free = 1;
    pthread_mutex_unlock(&global_malloc_lock);

}

//number of elements and how much storage they need
void *calloc(size_t num, size_t nsize){
    size_t size;
    void *block;
    if(!num || !nsize){
        return NULL;
    }
    size = num*nsize;

    if(nsize != size / num){
        return NULL;
    }
    block = malloc(size);
    if(!block){
        return NULL;
    }
    //fills the first n bytes of block
    memset(block, 0, size);
    return block;
}

void *realloc(void *block, size_t size){
    struct header_t *header;
    void *ret;
    if(!block || !size){
        return malloc(size);
    }
    //get header if it fits then its all good
    header = (struct header_t*)block - 1;
    if(header->size >= size){
        return block;
    }
    //get block of size of request
    ret = malloc(size);
    //reallocate with memcpy and then free the block
    if(ret){
        memcpy(ret, block, header->size);
        free(block);
    }
    return ret;

}



int main(){
    int *var_i, *cal;
    char *var_c;
    float *var_f;
    int n, i;

    //malloc
    var_i=(int*)malloc(1*sizeof(int));
    var_c=(char*)malloc(1*sizeof(char));
    var_f=(float*)malloc(1*sizeof(float));
    if(var_i == NULL || var_c == NULL || var_f == NULL){
        printf("call to malloc() was unsucessful :(\n");
    }

    printf("Enter integer value: ");
    scanf("%d",var_i);

    printf("Enter character value: ");
    scanf(" %c",var_c);

    printf("Enter float value: ");
    scanf("%f",var_f);

    printf("Inputted value are: %d, %c, %.2f\n",*var_i,*var_c,*var_f);

    free(var_i);
    free(var_c);
    free(var_f);
    //calloc
    n = 4;
    printf("How much memory do you want to allocate? %d\n", n);
    cal = (int*)calloc(n, sizeof(int));
    if (cal == NULL) {
        printf("call to calloc() was unsucessful :(\n");
        exit(0);
    }
    for( i=0; i < n; ++i){
        cal[i] = i + 1;
    }
    printf("Here are the array elements: ");
    for (i = 0; i < n; ++i) {
        printf("%d, ", cal[i]);
    }
    printf("\n");
    // free(cal);
    n = 6;
    printf("The new size of the array is %d\n", n);
    cal = (int*)realloc(cal, n * sizeof(int));
    if(cal == NULL){
        printf("call to realloc() was unsucessful :(\n");
        exit(0);
    }
    for (i = 4; i < n; ++i) {
        cal[i] = i + 1;
    }

    printf("The elements of the array are: ");
    for (i = 0; i < n; ++i) {
        printf("%d, ", cal[i]);
    }

    free(cal);

}

