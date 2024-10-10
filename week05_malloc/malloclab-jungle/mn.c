/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "week06-09",
    /* First member's full name */
    "Taewoo",
    /* First member's email address */
    "terry9508@naver.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

// Basic constants and macros
#define WSIZE       4           // Word and header/footer size(bytes)
#define DSIZE       8           // Double word size (btyes)
#define CHUNKSIZE   (1 << 12)   // Extend heap by this amount (bytes) : 초기 가용 블록과 힙 확장을 위한 기본 크기

#define MAX(x, y)   ((x) > (y) ? (x) : (y))    // x > y가 참이면 x, 거짓이면 y

// PACK매크로 : 크기와 할당 비트를 통합해서 header와 footer에 저장할 수 있는 값 리턴
#define PACK(size, alloc)   ((size) | (alloc))

// Read and wirte a word at address
#define GET(p)  (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

// Read the size and allocated field from address p
#define GET_SIZE(p)    (GET(p) & ~0x7)  // header or footer의 사이즈 반환(8의 배수)
#define GET_ALLOC(p)   (GET(p) & 0x1)   // 

// bp(현재 블록의 포인터)로 현재 블록의 header 위치와 footer 위치 반환
#define HDRP(bp)    ((char *)(bp) - WSIZE)
#define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// 다음과 이전 블록의 포인터 반환
#define NEXT_BLKP(bp)   (((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE)))    // 다음 블록 bp 위치 반환(bp + 현재 블록의 크기)
#define PREV_BLKP(bp)   (((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE)))    // 이전 블록 bp 위치 반환(bp - 이전 블록의 크기)

/* get free prev and free successor */
#define FREE_NEXT(bp)   *(void **)((char *)bp+WSIZE)
#define FREE_PREV(bp)   *(void **)(bp)

// Declaration
static void *heap_listp;
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t a_size);
static void place(void *bp, size_t a_size);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // Create the initial empty heap
    if ((heap_listp = mem_sbrk(8*WSIZE)) == (void *)-1) { 
        return -1;
    }

    PUT(heap_listp, 0);                             /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    /* Prologue footer */ 
    PUT(heap_listp + (3*WSIZE), PACK(2*DSIZE, 0));  /* first free header */
    PUT(heap_listp + (4*WSIZE), 0);                 /* No prev */
    PUT(heap_listp + (5*WSIZE), 0);                 /* No succ */
    PUT(heap_listp + (6*WSIZE), PACK(2*DSIZE, 0));  /* first free footer */
    PUT(heap_listp + (7*WSIZE), PACK(0, 1));        /* Epilogue header */ 
    heap_listp += (4*WSIZE);                        /* start of free_list */

    // Extend the empty heap with a free block of CHUNKSIZE bytes
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) {
        return -1;
    }
    return 0;
}

static void *extend_heap(size_t words) {
    printf("extend_heap\n");
    char *bp;
    size_t size;

    // Allocate an even number of words  to maintain alignment
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1) { // NULL is valid, but void( *)-1 is not.
        return NULL;
    }

    // initialize free block header/footer and the epilogue header
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    // coalesce if the previous block was free
    return coalesce(bp);   
}

// static void *coalesce(void *bp) {
//     printf("coalesce\n");
//     size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
//     size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
//     size_t size = GET_SIZE(HDRP(bp));

//     // case1: prev alloc, next alloc
//     if (prev_alloc && next_alloc) {
//         LIFO(bp);
//         return bp;
//     }

//     // case2: prev alloc, next free
//     else if (prev_alloc && !next_alloc) {
//         /* get freeblock pointers*/
//         *(char **)bp = *(char **)(bp+size);
//         *(char **)(bp+WSIZE) = *(char **)(bp+size+WSIZE);

//         size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
//         PUT(HDRP(bp), PACK(size, 0));
//         PUT(FTRP(bp), PACK(size, 0)); 
//     }

//     // case3: prev free, next alloc
//     else if (!prev_alloc && next_alloc) {
//         size += GET_SIZE(HDRP(PREV_BLKP(bp)));
//         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//         PUT(FTRP(PREV_BLKP(bp)), PACK(size, 0));        
//         bp = PREV_BLKP(bp);
//     }

//     // case4: prev free, next free
//     else {
//         size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
//         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//         PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
//         /* record next's next at prev */
//         *(char **)(PREV_BLKP(bp)+WSIZE) = *(char **)(NEXT_BLKP(bp)+WSIZE);
//         bp = PREV_BLKP(bp);
        
//     }
//     return bp;
// }

static void *coalesce(void *bp) {
    printf("coalesce\n");
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    // case1: prev alloc, next alloc
    if (prev_alloc && next_alloc) {
        LIFO(bp); // 단순히 리스트의 맨 앞에 추가
        return bp;
    }

    // case2: prev alloc, next free
    else if (prev_alloc && !next_alloc) {
        /* 연결 시 포인터 관리 수정 */
        void *next_bp = NEXT_BLKP(bp);
        size += GET_SIZE(HDRP(next_bp));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));

        /* Update free list pointers */
        FREE_PREV(bp) = FREE_PREV(next_bp);
        FREE_NEXT(bp) = FREE_NEXT(next_bp);

        if (FREE_PREV(next_bp) != NULL) {
            FREE_NEXT(FREE_PREV(next_bp)) = bp;
        }
        if (FREE_NEXT(next_bp) != NULL) {
            FREE_PREV(FREE_NEXT(next_bp)) = bp;
        }
    }

    // case3: prev free, next alloc
    else if (!prev_alloc && next_alloc) {
        void *prev_bp = PREV_BLKP(bp);
        size += GET_SIZE(HDRP(prev_bp));
        PUT(HDRP(prev_bp), PACK(size, 0));
        PUT(FTRP(prev_bp), PACK(size, 0));        
        bp = prev_bp;
    }

    // case4: prev free, next free
    else {
        void *prev_bp = PREV_BLKP(bp);
        void *next_bp = NEXT_BLKP(bp);
        size += GET_SIZE(HDRP(prev_bp)) + GET_SIZE(HDRP(next_bp));
        PUT(HDRP(prev_bp), PACK(size, 0));
        PUT(FTRP(next_bp), PACK(size, 0));

        /* Update free list pointers */
        if (FREE_PREV(next_bp) != NULL) {
            FREE_NEXT(FREE_PREV(next_bp)) = FREE_NEXT(next_bp);
        }
        if (FREE_NEXT(next_bp) != NULL) {
            FREE_PREV(FREE_NEXT(next_bp)) = FREE_PREV(next_bp);
        }
        bp = prev_bp;
    }

    return bp;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    printf("malloc\n");
    size_t a_size;       // adjusted block szie
    size_t extend_size;  // Amount to extend heap if no fit
    char *bp;

    // Ignore spurious requests
    if (size == 0) {
        return NULL;
    }

    // Adjust block size to include overhead and alignment reqs
    if (size <= DSIZE) {    
        a_size = 2*DSIZE;
    }
    else {                 
        a_size = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); 
    }

    // Search the free list for a fit
    if ((bp = find_fit(a_size)) != NULL) {   
        place(bp, a_size);                  
        return bp;
    }

    // NO fit found. Get more memory and place the block
    extend_size = MAX(a_size, CHUNKSIZE);
    if ((bp = extend_heap(extend_size/WSIZE)) == NULL) {    
        return NULL;
    }
    place(bp, a_size);
    return bp;
}

static void *find_fit(size_t a_size) {
    printf("find_fit\n");
    void *bp = heap_listp;

    while (bp != NULL) {
        if (a_size <= GET_SIZE(HDRP(bp))) {
            return bp;
        }
        bp = FREE_NEXT(bp);
    }
    return NULL;    // NO fit
}

static void place(void *bp, size_t a_size) {
    printf("place\n");
    size_t c_size = GET_SIZE(HDRP(bp));
    void *prev = FREE_PREV(bp);
    void *next = FREE_NEXT(bp);

    if ((c_size - a_size) >= (2 * (DSIZE))) {       
        PUT(HDRP(bp), PACK(a_size, 1));
        PUT(FTRP(bp), PACK(a_size, 1));
        /* if first free block was bp, update heap_listp */
        if (heap_listp == bp) {
            heap_listp = NEXT_BLKP(bp);
        }

        bp = NEXT_BLKP(bp);        
        PUT(HDRP(bp), PACK(c_size - a_size, 0));
        PUT(FTRP(bp), PACK(c_size - a_size, 0));

        /* maintain prev and next pointer*/
        FREE_PREV(bp) = prev;
        FREE_NEXT(bp) = next;

        /* update free block location in prev & next */
        if (prev != NULL) {
            FREE_NEXT(prev) = bp;
        }

        if (next != NULL) {
            FREE_PREV(next) = bp;
        }
    }
    else {
        PUT(HDRP(bp), PACK(c_size, 1));
        PUT(FTRP(bp), PACK(c_size, 1));
        if (heap_listp == bp) {
            heap_listp = FREE_NEXT(bp);
            return;
        }      
        /* connect prev and next */
        if (prev != NULL) {
            FREE_NEXT(prev) = next; /* prev -> next */
        } 
        if (next != NULL) {
            FREE_PREV(next) = prev; /* next -> prev */
        }        


    }
}

/* append freed block to linked_list head*/
void LIFO(void *bp) {
    printf("lifo\n");
    if (heap_listp != NULL) {
        FREE_PREV(heap_listp) = bp;
    }
    FREE_PREV(bp) = NULL;
    FREE_NEXT(bp) = heap_listp;
    heap_listp = bp;
}


/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{   
    printf("free\n");
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
    // size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    // size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    
    // if (prev_alloc && next_alloc) {
    //     LIFO(bp); /* no linked free block*/
    // } else {
    //     coalesce(bp); /* link free block */
    // }
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *bp, size_t size)
{
    void *old_bp = bp;
    void *new_bp;
    size_t copySize;
    
    new_bp = mm_malloc(size);
    if (new_bp == NULL)
      return NULL;
    copySize = GET_SIZE(HDRP(old_bp));
    if (size < copySize)
      copySize = size;
    memcpy(new_bp, old_bp, copySize);  
    mm_free(old_bp);
    return new_bp;
}

