#include "cstack.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define UNUSED(VAR) (void)(VAR)


struct node { //структура элемент стека 
    int bytes;
    struct node* prev;
    char * data;
};

typedef struct node* stack_piece_ptr; // указатель на элемент стека

struct stack_entry{
    int reserved; //количество элементов в стеке
    stack_piece_ptr stack_ptr;
};

typedef struct stack_entry stack_entry_t; //структура верхушки стека

struct stack_entries_table // список созданных стеков
{
    unsigned int size;
    stack_entry_t* entries;//указатель на массив созданных стеков
};


struct stack_entries_table g_table = {0u, NULL};

hstack_t stack_new(void)
{
    if (g_table.size){
        g_table.entries = realloc(g_table.entries, sizeof(stack_entry_t)*(g_table.size+1)); // увеличиваем размер таблицы на один (ДОПИСАТЬ ПРОВЕРКУ)
        g_table.entries[g_table.size].reserved=0; //инициализируем новый стек
        g_table.entries[g_table.size].stack_ptr=NULL;
        return g_table.size++;// увеличиваем счетчик размера таблицы на 1
    }
    else{ if(!g_table.size){
        g_table.entries=malloc(sizeof(stack_entry_t));
        g_table.entries->reserved=0;
        g_table.entries->stack_ptr=NULL;
        
        return g_table.size++;
    }

    }
    return -1;
}

void stack_free(const hstack_t hstack)
{
    if ((g_table.size>=(unsigned)hstack&&hstack>=0)){ //если наш стек инициализирован, то освобождаем его
        int size = (g_table.entries+hstack)->reserved;  
        for (int i=0;i<size&&g_table.entries[hstack].stack_ptr!=NULL; i++){
            stack_piece_ptr temp; //создаем временную переменную для хранения указателя на элемент стека
            temp=(g_table.entries+hstack)->stack_ptr->prev; //записываем в нее указатель прошлого элемента стека
            free(g_table.entries[hstack].stack_ptr->data);//освобождаем память, занятую данными
            free(g_table.entries[hstack].stack_ptr); //освобождаем память, занятую текущим элементом стека (верхним)
            (g_table.entries+hstack)->stack_ptr = temp; // записываем в верхушку стека указатель на прошлый элемент стека (верхний теперь невалиден)
            g_table.entries[hstack].reserved--;
        }
        free(g_table.entries[hstack].stack_ptr); //освобождаем память последнего элемента стека
        g_table.entries[hstack].reserved=-1;
    }
    

}

int stack_valid_handler(const hstack_t hstack)
{
    if (g_table.size>=(unsigned)hstack&&hstack>-1){ //если существует стек под таким номером
        return (g_table.entries[hstack].reserved==-1)?1:0; // возвращаем 0, если существует указатель на такой стек, 1, если нет
    }
    else {return 1;}
}

unsigned int stack_size(const hstack_t hstack) //получить количество элементов в стеке
{
    if (g_table.size>=(unsigned)hstack&&hstack>-1){//если существует стек под таким номером
        return g_table.entries[hstack].reserved; // возвращаем 1, если существует указатель на такой стек, 0, если нет
    }
    else {return 0;}
}

void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    if (g_table.size>=(unsigned)hstack&&hstack>-1){//если существует стек под таким номером
        if (data_in!=NULL&&size>0){//верные ли данные нам передали?
            stack_piece_ptr temp = malloc(sizeof(struct node));//СДЕЛАТЬ ПРОВЕРКУ НА ВЫДЕЛЕНИЕ ПАМЯТИ
            char * data  = malloc(size);//выделяем место под данные СДЕЛАТЬ ПРОВЕРКУ
            memcpy(data, data_in,size); //Проверить, могу ли я вообще так копировать данные PS да, могу
            temp->data=data; //записываем указатель на данные в элемент стека
            temp->bytes=size; //записываем количество байт в соответствующую полочку

            temp->prev=g_table.entries[hstack].stack_ptr; //покладаем в новый элемент стека указание на предыдущий
            g_table.entries[hstack].stack_ptr=temp; // меняем адрес последнего элемента стека на новый
            g_table.entries[hstack].reserved++;// увеличиваем счетчик записанных элементов на один
            
        }
    }   
    
}

unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    size_t ret=0;   
    if (g_table.size>=(unsigned)hstack&&hstack>-1){//если существует стек под таким номером
        if (data_out!=NULL&& size>0&&g_table.entries[hstack].stack_ptr){  //если указатель куда передавать данные существует и указатель на стек тоже
            if ((g_table.entries+hstack)->stack_ptr->data&& size==(unsigned)g_table.entries[hstack].stack_ptr->bytes){ //если стек вообще инициализирован и мы передаем элемент стека целиком
                memcpy(data_out, g_table.entries[hstack].stack_ptr->data, size); // копируем size данных в data_out
                ret = size; //присваеваем возвращаемому значению длину скопированных символов, если проверка прошла
                if (g_table.entries[hstack].stack_ptr->prev!=NULL){ //еСли текущий элемент стека не последний
                    stack_piece_ptr temp;
                    temp=g_table.entries[hstack].stack_ptr->prev;//сохраняем адрес предыдущего элемента стека во временную переменную
                    free(g_table.entries[hstack].stack_ptr->data); //освобождаем память, отведенную под данные
                    free(g_table.entries[hstack].stack_ptr); // освобождаем верхний элемент стека, если он не последний
                    g_table.entries[hstack].stack_ptr = temp; //передаем в верхушку стека указатель на прошлый элемент
                    g_table.entries[hstack].reserved--; //уменьшаем счетчик элементов в стеке
                }
                else{ if (g_table.entries[hstack].stack_ptr->prev==NULL){
                        g_table.entries[hstack].reserved--;
                        free(g_table.entries[hstack].stack_ptr->data); //освобождаем область памяти с данными
                        free(g_table.entries[hstack].stack_ptr); // освобождаем верхний элемент стека
                        g_table.entries[hstack].stack_ptr=NULL; //записываем нулевой указатель как указатель на первый элемент стека
                    }

                }
            }    
    }
    }
    return ret;
}
