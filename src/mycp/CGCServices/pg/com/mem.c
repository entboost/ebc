#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "slist.h"

#ifdef WIN32
#include <Windows.h>
#endif

#include "lock_def.h"
//#include <pthread.h>

#include "mem.h"

#define MEM_PROFILE_TABLE_SIZE 4096
#define MEM_AREA_SIZE 4L


struct _MemChunk
{
    char     name[255];         /* name of this MemChunk...used for debugging output */
    unsigned int atom_size;   /* the size of an atom */
    unsigned long atom_no;    /* the size of a memory area */
    SList    *free_atoms;     /* the free atoms list */
    MemChunk *next;           /* pointer to the next chunk */
    MemChunk *prev;           /* pointer to the previous chunk */
    
	/*pthread_mutex_t*/LOCK_T  chunk_lock;

    char mem[MEM_AREA_SIZE];  /* the mem array from which atoms get allocated
			       * the actual size of this array is determined by
			       *  the mem chunk "area_size". ANSI says that it
			       *  must be declared to be the maximum size it
			       *  can possibly be (even though the actual size
			       *  may be less).
			       */
};

//static pthread_mutex_t      mem_chunks_lock = PTHREAD_MUTEX_INITIALIZER;
static LOCK_T               mem_chunks_lock;

static MemChunk            *mem_chunks = NULL;

MemChunk* mem_chunk_new (const char  *name,
    int            atom_size,
    unsigned long  number)
{
	
    MemChunk *mem_chunk;
    int       i;
    void     *atom;
	
	mem_init();
    mem_chunk = (MemChunk*)malloc(sizeof(MemChunk) - 4 + atom_size * number);;

    //mem_chunk->free_atoms = slist_alloc();
     mem_chunk->free_atoms = 0;
 
	//mem_chunk->name = strdup(name);
	strcpy(mem_chunk->name, name);

    mem_chunk->atom_size = atom_size;
    mem_chunk->atom_no = number;


    //pthread_mutex_lock (&mem_chunks_lock);
	LOCK (&mem_chunks_lock);

    mem_chunk->next = mem_chunks;
    mem_chunk->prev = NULL;
    if (mem_chunks)
	mem_chunks->prev = mem_chunk;
    mem_chunks = mem_chunk;
   
	//pthread_mutex_unlock (&mem_chunks_lock);
	UNLOCK(&mem_chunks_lock);

    //pthread_mutex_init(&mem_chunk->chunk_lock, 0);
	LOCK_INIT (&mem_chunk->chunk_lock);

    //pthread_mutex_lock(&mem_chunk->chunk_lock);
	LOCK (&mem_chunk->chunk_lock);

    for(i = 0; i < number; i++) {
	atom = mem_chunk->mem + (i * atom_size);
	mem_chunk->free_atoms = slist_prepend(mem_chunk->free_atoms, atom);
    }

    //pthread_mutex_unlock(&mem_chunk->chunk_lock);
	UNLOCK (&mem_chunk->chunk_lock);

    return mem_chunk;
}

void mem_chunk_destroy (MemChunk *mem_chunk)
{
    if(!mem_chunk)
	return;

    //pthread_mutex_lock (&mem_chunks_lock);
	LOCK (&mem_chunks_lock);

    if (mem_chunk->next)
	mem_chunk->next->prev = mem_chunk->prev;
    if (mem_chunk->prev)
	mem_chunk->prev->next = mem_chunk->next;

    if (mem_chunk == mem_chunks)
	mem_chunks = mem_chunks->next;
    //pthread_mutex_unlock (&mem_chunks_lock);
	UNLOCK (&mem_chunks_lock);


    //pthread_mutex_lock(&mem_chunk->chunk_lock);
	LOCK (&mem_chunk->chunk_lock);

    slist_free(mem_chunk->free_atoms);
    //pthread_mutex_unlock(&mem_chunk->chunk_lock);
	UNLOCK (&mem_chunk->chunk_lock);

   // free (mem_chunk->name);
    free (mem_chunk);
}

void* mem_chunk_alloc (MemChunk *mem_chunk)
{
    void *mem;
    
    if(!mem_chunk)
	return 0;
    mem = 0;

    //pthread_mutex_lock(&mem_chunk->chunk_lock);
    LOCK (&mem_chunk->chunk_lock);

	if (mem_chunk->free_atoms) {
	mem = slist_nth_data(mem_chunk->free_atoms, 0);
	mem_chunk->free_atoms = slist_remove(mem_chunk->free_atoms, mem);
    }

    //pthread_mutex_unlock(&mem_chunk->chunk_lock);
	UNLOCK (&mem_chunk->chunk_lock);

    return mem;
}

void mem_chunk_free (MemChunk *mem_chunk,
		void* mem)
{
    if(!mem_chunk)
	return;
    if(!mem)
	return;

    //pthread_mutex_lock(&mem_chunk->chunk_lock);
	LOCK (&mem_chunk->chunk_lock);

    mem_chunk->free_atoms = slist_prepend(mem_chunk->free_atoms, mem);
    
	//pthread_mutex_unlock(&mem_chunk->chunk_lock);
	UNLOCK (&mem_chunk->chunk_lock);

}

void mem_init (void)
{
    //pthread_mutex_init(&mem_chunks_lock, 0);
	LOCK_INIT (&mem_chunks_lock);
}


