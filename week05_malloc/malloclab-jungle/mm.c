// // #include <stdio.h>
// // #include <stdlib.h>
// // #include <assert.h>
// // #include <unistd.h>
// // #include <string.h>
// // #include "mm.h"
// // #include "memlib.h"
// // /*********************************************************
// //  * NOTE TO STUDENTS: Before you do anything else, please
// //  * provide your team information in the following struct.
// //  ********************************************************/
// // team_t team = {
// //     /* Team name */
// //     "team 9",
// //     /* First member's full name */
// //     "Taewoo",
// //     /* First member's email address */
// //     "terry9508@naver.com",
// //     /* Second member's full name (leave blank if none) */
// //     "Byeongchan",
// //     /* Second member's email address (leave blank if none) */
// //     "Byeongchan@naver.com"
// // };
// // void *heap_listp = NULL;
// // void *linked_listp = NULL;      /*first free block*/
// // void *extend_heap(size_t words);
// // void *find_fit(size_t asize);
// // void place(void *bp, size_t asize);
// // void insert_linked(void *bp);
// // void delete_linked(void *bp);
// // void *coalesce(void *bp);

// // #define WSIZE 4
// // #define DSIZE 8
// // #define CHUNKSIZE       (1<<12)
// // #define MAX(x, y) ((x) > (y) ? (x) : (y))
// // #define PACK(size, alloc)   ((size) | (alloc))
// // #define GET(p)          (*(unsigned int *)(p))
// // #define PUT(p, val)     (*(unsigned int *)(p) = (val))
// // #define GET_SIZE(p)     (GET(p) & ~0x7)
// // #define GET_ALLOC(p)    (GET(p) & 0x1)
// // #define HDRP(bp)        ((char *)(bp) - WSIZE)
// // #define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
// // #define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
// // #define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

// // #define PREV_LINK(bp)   (*(void **)(bp))                    //prev free block
// // #define NEXT_LINK(bp)   (*(void **)(char *)(bp + WSIZE))    //next free block

// // int mm_init(void)
// // {
// //     if ((heap_listp = mem_sbrk(6*WSIZE)) == (void *)-1){
// //         return -1;
// //     }
// //     PUT(heap_listp, 0);
// //     PUT(heap_listp + 1*WSIZE, PACK(2*DSIZE, 1));    
// //     PREV_LINK(heap_listp + 2*WSIZE) = NULL;
// //     NEXT_LINK(heap_listp + 3*WSIZE) = NULL;
// //     PUT(heap_listp + 4*WSIZE, PACK(2*DSIZE, 1));
// //     PUT(heap_listp + 5*WSIZE, PACK(0, 1));
// //     linked_listp = heap_listp + 2*WSIZE;
// //     if (extend_heap(CHUNKSIZE/WSIZE) == NULL){
// //         return -1;
// //     }
// //     return 0;
// // }

// // void *extend_heap(size_t words){
// //     char *bp;
// //     size_t size;
// //     size = (words%2) ? (words+1)*WSIZE : words*WSIZE;
// //     if ((bp = mem_sbrk(size)) == (void *)-1){
// //         return NULL;
// //     }
// //     PUT(HDRP(bp), PACK(size, 0));
// //     PUT(FTRP(bp), PACK(size, 0));
// //     PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
// //     return coalesce(bp);
// // }

// // void *find_fit(size_t asize){
// //     void *bp;
// //     for (bp = linked_listp; GET_ALLOC(HDRP(bp)) != 1; bp = NEXT_LINK(bp)){
// //         if(asize <= GET_SIZE(HDRP(bp))){
// //             return bp;
// //         }
// //     }
// //     return NULL;
// // }

// // void place(void *bp, size_t asize) {
// //     size_t csize = GET_SIZE(HDRP(bp));
// //     delete_linked(bp);
// //     if ((csize - asize) >= (2 * DSIZE)) {   
// //         PUT(HDRP(bp), PACK(asize, 1));      
// //         PUT(FTRP(bp), PACK(asize, 1));
// //         bp = NEXT_BLKP(bp);
// //         PUT(HDRP(bp), PACK(csize-asize, 0));
// //         PUT(FTRP(bp), PACK(csize-asize, 0));
// //         insert_linked(bp);
// //     } else {
// //         PUT(HDRP(bp), PACK(csize, 1));
// //         PUT(FTRP(bp), PACK(csize, 1));
// //     }
// // }

// // void insert_linked(void *bp) {
// //     PREV_LINK(bp) = NULL; /* LIFO */
// //     NEXT_LINK(bp) = linked_listp;
// //     PREV_LINK(linked_listp) = bp;
// //     linked_listp = bp;
// // }

// // void delete_linked(void *bp) {
// //     if (bp == linked_listp) {    /* disconnect link */
// //         PREV_LINK(NEXT_LINK(bp)) = NULL;
// //         linked_listp = NEXT_LINK(bp);
// //     } else {
// //         NEXT_LINK(PREV_LINK(bp)) = NEXT_LINK(bp);
// //         PREV_LINK(NEXT_LINK(bp)) = PREV_LINK(bp);
// //     }
// // }

// // void *mm_malloc(size_t size) {
// //     char *bp;
// //     size_t asize;
// //     size_t extendsize;
// //     if (size == 0) {
// //         return NULL;
// //     }

// //     if (size <= DSIZE) {
// //         asize = 2*DSIZE; 
// //     } else {
// //         asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
// //     }

// //     if ((bp = find_fit(asize)) != NULL) {
// //         place(bp, asize);
// //         return bp;
// //     }

// //     extendsize = MAX(asize, CHUNKSIZE);
// //     if ((bp = extend_heap(extendsize/WSIZE)) == NULL) {
// //         return NULL;
// //     }
// //     place(bp, asize);
// //     return bp;
// // }

// // void mm_free(void *bp) {
// //     size_t size = GET_SIZE(HDRP(bp));
// //     PUT(HDRP(bp), PACK(size, 0));
// //     PUT(FTRP(bp), PACK(size, 0));
// //     coalesce(bp);
// // }

// // void *coalesce(void *bp) {
// //     size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
// //     size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
// //     size_t size = GET_SIZE(HDRP(bp));

// //     if (prev_alloc && next_alloc) {        
// //     } else if (prev_alloc && !next_alloc) {
// //         delete_linked(NEXT_BLKP(bp));   
// //         size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
// //         PUT(HDRP(bp), PACK(size, 0));   
// //         PUT(FTRP(bp), PACK(size, 0));
        
// //     } else if (!prev_alloc && next_alloc) {
// //         delete_linked(PREV_BLKP(bp));   
// //         size += GET_SIZE(HDRP(PREV_BLKP(bp)));
// //         bp = PREV_BLKP(bp);
// //         PUT(HDRP(bp), PACK(size, 0));
// //         PUT(FTRP(bp), PACK(size, 0));
        
// //     } else {
// //         delete_linked(PREV_BLKP(bp));   
// //         delete_linked(NEXT_BLKP(bp));   
// //         size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
// //         bp = PREV_BLKP(bp);
// //         PUT(HDRP(bp), PACK(size, 0));
// //         PUT(FTRP(bp), PACK(size, 0));
        
// //     }
// //     insert_linked(bp);
// //     return bp;
// // }

// // void *mm_realloc(void *ptr, size_t size) {
// //     void *oldptr = ptr;
// //     void *newptr;
// //     size_t copySize;
// //     newptr = mm_malloc(size);
// //     if (newptr == NULL) {
// //         return NULL;
// //     }
// //     copySize = GET_SIZE(HDRP(oldptr)) - DSIZE;
// //     if (size < copySize) {
// //         copySize = size;
// //     }
// //     memcpy(newptr, oldptr, copySize);
// //     mm_free(oldptr);
// //     return newptr;
// // }


// /*
//  * mm-naive.c - The fastest, least memory-efficient malloc package.
//  * 
//  * In this naive approach, a block is allocated by simply incrementing
//  * the brk pointer.  A block is pure payload. There are no headers or
//  * footers.  Blocks are never coalesced or reused. Realloc is
//  * implemented directly using mm_malloc and mm_free.
//  *
//  * NOTE TO STUDENTS: Replace this header comment with your own header
//  * comment that gives a high level description of your solution.
//  */
// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>
// #include <unistd.h>
// #include <string.h>

// #include "mm.h"
// #include "memlib.h"

// /*********************************************************
//  * NOTE TO STUDENTS: Before you do anything else, please
//  * provide your team information in the following struct.
//  ********************************************************/
// team_t team = {
//     /* Team name */
//     "week06-09",
//     /* First member's full name */
//     "Taewoo",
//     /* First member's email address */
//     "terry9508@naver.com",
//     /* Second member's full name (leave blank if none) */
//     "",
//     /* Second member's email address (leave blank if none) */
//     ""
// };

// // Basic constants and macros
// #define WSIZE       4           // Word and header/footer size(bytes)
// #define DSIZE       8           // Double word size (btyes)
// #define CHUNKSIZE   (1 << 12)   // Extend heap by this amount (bytes) : 초기 가용 블록과 힙 확장을 위한 기본 크기

// #define MAX(x, y)   ((x) > (y) ? (x) : (y))    // x > y가 참이면 x, 거짓이면 y

// // PACK매크로 : 크기와 할당 비트를 통합해서 header와 footer에 저장할 수 있는 값 리턴
// #define PACK(size, alloc)   ((size) | (alloc))

// // Read and wirte a word at address
// #define GET(p)  (*(unsigned int *)(p))
// #define PUT(p, val)  (*(unsigned int *)(p) = (val))

// // Read the size and allocated field from address p
// #define GET_SIZE(p)    (GET(p) & ~0x7)  // header or footer의 사이즈 반환(8의 배수)
// #define GET_ALLOC(p)   (GET(p) & 0x1)   // 

// // bp(현재 블록의 포인터)로 현재 블록의 header 위치와 footer 위치 반환
// #define HDRP(bp)    ((char *)(bp) - WSIZE)
// #define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// // 다음과 이전 블록의 포인터 반환
// #define NEXT_BLKP(bp)   (((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE)))    // 다음 블록 bp 위치 반환(bp + 현재 블록의 크기)
// #define PREV_BLKP(bp)   (((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE)))    // 이전 블록 bp 위치 반환(bp - 이전 블록의 크기)

// /* get free prev and free successor */
// #define FREE_NEXT(bp)   *(void **)((char *)bp+WSIZE)
// #define FREE_PREV(bp)   *(void **)(bp)

// // Declaration
// static void *heap_listp;
// static void *extend_heap(size_t words);
// static void *coalesce(void *bp);
// static void *find_fit(size_t a_size);
// static void place(void *bp, size_t a_size);
// void LIFO(void *bp);
// /* 
//  * mm_init - initialize the malloc package.
//  */
// int mm_init(void)
// {
//     // Create the initial empty heap
//     if ((heap_listp = mem_sbrk(8*WSIZE)) == (void *)-1) { 
//         return -1;
//     }

//     PUT(heap_listp, 0);                             /* Alignment padding */
//     PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    /* Prologue header */
//     PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    /* Prologue footer */ 
//     PUT(heap_listp + (3*WSIZE), PACK(2*DSIZE, 0));  /* first free header */
//     PUT(heap_listp + (4*WSIZE), 0);                 /* No prev */
//     PUT(heap_listp + (5*WSIZE), 0);                 /* No succ */
//     PUT(heap_listp + (6*WSIZE), PACK(2*DSIZE, 0));  /* first free footer */
//     PUT(heap_listp + (7*WSIZE), PACK(0, 1));        /* Epilogue header */ 
//     heap_listp += (4*WSIZE);                        /* start of free_list */

//     // Extend the empty heap with a free block of CHUNKSIZE bytes
//     if (extend_heap(CHUNKSIZE/WSIZE) == NULL) {
//         return -1;
//     }
//     return 0;
// }

// static void *extend_heap(size_t words) {
//     printf("extend_heap\n");
//     char *bp;
//     size_t size;

//     // Allocate an even number of words  to maintain alignment
//     size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
//     if ((long)(bp = mem_sbrk(size)) == -1) { // NULL is valid, but void( *)-1 is not.
//         return NULL;
//     }

//     // initialize free block header/footer and the epilogue header
//     PUT(HDRP(bp), PACK(size, 0));
//     PUT(FTRP(bp), PACK(size, 0));
//     PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

//     // coalesce if the previous block was free
//     return coalesce(bp);   
// }

// // static void *coalesce(void *bp) {
// //     printf("coalesce\n");
// //     size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
// //     size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
// //     size_t size = GET_SIZE(HDRP(bp));

// //     // case1: prev alloc, next alloc
// //     if (prev_alloc && next_alloc) {
// //         LIFO(bp);
// //         return bp;
// //     }

// //     // case2: prev alloc, next free
// //     else if (prev_alloc && !next_alloc) {
// //         /* get freeblock pointers*/
// //         *(char **)bp = *(char **)(bp+size);
// //         *(char **)(bp+WSIZE) = *(char **)(bp+size+WSIZE);

// //         size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
// //         PUT(HDRP(bp), PACK(size, 0));
// //         PUT(FTRP(bp), PACK(size, 0)); 
// //     }

// //     // case3: prev free, next alloc
// //     else if (!prev_alloc && next_alloc) {
// //         size += GET_SIZE(HDRP(PREV_BLKP(bp)));
// //         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
// //         PUT(FTRP(PREV_BLKP(bp)), PACK(size, 0));        
// //         bp = PREV_BLKP(bp);
// //     }

// //     // case4: prev free, next free
// //     else {
// //         size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
// //         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
// //         PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
// //         /* record next's next at prev */
// //         *(char **)(PREV_BLKP(bp)+WSIZE) = *(char **)(NEXT_BLKP(bp)+WSIZE);
// //         bp = PREV_BLKP(bp);
        
// //     }
// //     return bp;
// // }

// static void *coalesce(void *bp) {
//     printf("coalesce\n");
//     size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
//     size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
//     size_t size = GET_SIZE(HDRP(bp));

//     // case1: prev alloc, next alloc
//     if (prev_alloc && next_alloc) {
//         LIFO(bp); 
//         return bp;
//     }

//     // case2: prev alloc, next free
//     else if (prev_alloc && !next_alloc) {
//         /* 연결 시 포인터 관리 수정 */
//         void *next_bp = NEXT_BLKP(bp);
//         size += GET_SIZE(HDRP(next_bp));
//         PUT(HDRP(bp), PACK(size, 0));
//         PUT(FTRP(bp), PACK(size, 0));

//         /* Update free list pointers */
//         FREE_PREV(next_bp) = FREE_PREV(bp);
//         FREE_NEXT(next_bp) = FREE_NEXT(bp);

//         if (FREE_PREV(next_bp) != NULL) {
//             FREE_NEXT(FREE_PREV(next_bp)) = next_bp;
//         }
//         if (FREE_NEXT(next_bp) != NULL) {
//             FREE_PREV(FREE_NEXT(next_bp)) = next_bp;
//         }
//     }

//     // case3: prev free, next alloc
//     else if (!prev_alloc && next_alloc) {
//         void *prev_bp = PREV_BLKP(bp);
//         size += GET_SIZE(HDRP(prev_bp));
//         PUT(HDRP(prev_bp), PACK(size, 0));
//         PUT(FTRP(prev_bp), PACK(size, 0));        
//         bp = prev_bp;
//     }

//     // case4: prev free, next free
//     else {
//         void *prev_bp = PREV_BLKP(bp);
//         void *next_bp = NEXT_BLKP(bp);
//         size += GET_SIZE(HDRP(prev_bp)) + GET_SIZE(HDRP(next_bp));
//         PUT(HDRP(prev_bp), PACK(size, 0));
//         PUT(FTRP(next_bp), PACK(size, 0));
//         FREE_NEXT(prev_bp) = FREE_NEXT(next_bp);
//         if (FREE_NEXT(prev_bp) != NULL) {
//             FREE_PREV(FREE_NEXT(prev_bp)) = prev_bp;
//         }
//         // /* Update free list pointers */
//         // if (FREE_PREV(next_bp) != NULL) {
//         //     FREE_NEXT(prev_bp) = FREE_NEXT(next_bp);
//         // }
//         // if (FREE_NEXT(next_bp) != NULL) {
//         //     FREE_PREV(FREE_NEXT(next_bp)) = FREE_PREV(next_bp);
//         // }
//         bp = prev_bp;
//     }

//     return bp;
// }

// /* 
//  * mm_malloc - Allocate a block by incrementing the brk pointer.
//  *     Always allocate a block whose size is a multiple of the alignment.
//  */
// void *mm_malloc(size_t size) {
//     printf("malloc\n");
//     size_t a_size;       // adjusted block szie
//     size_t extend_size;  // Amount to extend heap if no fit
//     char *bp;

//     // Ignore spurious requests
//     if (size == 0) {
//         return NULL;
//     }

//     // Adjust block size to include overhead and alignment reqs
//     if (size <= DSIZE) {    
//         a_size = 2*DSIZE;
//     }
//     else {                 
//         a_size = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); 
//     }

//     // Search the free list for a fit
//     if ((bp = find_fit(a_size)) != NULL) {   
//         place(bp, a_size);                  
//         return bp;
//     }

//     // NO fit found. Get more memory and place the block
//     extend_size = MAX(a_size, CHUNKSIZE);
//     if ((bp = extend_heap(extend_size/WSIZE)) == NULL) {    
//         return NULL;
//     }
//     place(bp, a_size);
//     return bp;
// }

// static void *find_fit(size_t a_size) {
//     printf("find_fit\n");
//     void *bp = heap_listp;

//     while (bp != NULL) {
//         if (a_size <= GET_SIZE(HDRP(bp))) {
//             return bp;
//         }
//         bp = FREE_NEXT(bp);
//     }
//     return NULL;    // NO fit
// }

// static void place(void *bp, size_t a_size) {
//     printf("place\n");
//     size_t c_size = GET_SIZE(HDRP(bp));
//     void *prev = FREE_PREV(bp);
//     void *next = FREE_NEXT(bp);

//     if ((c_size - a_size) >= (2 * (DSIZE))) {       
//         PUT(HDRP(bp), PACK(a_size, 1));
//         PUT(FTRP(bp), PACK(a_size, 1));
//         /* if first free block was bp, update heap_listp */
//         if (heap_listp == bp) {
//             heap_listp = NEXT_BLKP(bp);
//         }

//         bp = NEXT_BLKP(bp);        
//         PUT(HDRP(bp), PACK(c_size - a_size, 0));
//         PUT(FTRP(bp), PACK(c_size - a_size, 0));

//         /* maintain prev and next pointer*/
//         FREE_PREV(bp) = prev;
//         FREE_NEXT(bp) = next;

//         /* update free block location in prev & next */
//         if (prev != NULL) {
//             FREE_NEXT(prev) = bp;
//         }

//         if (next != NULL) {
//             FREE_PREV(next) = bp;
//         }
//     }
//     else {
//         PUT(HDRP(bp), PACK(c_size, 1));
//         PUT(FTRP(bp), PACK(c_size, 1));
//         if (heap_listp == bp) {
//             heap_listp = FREE_NEXT(bp);
//             return;
//         }      
//         /* connect prev and next */
//         if (prev != NULL) {
//             FREE_NEXT(prev) = next; /* prev -> next */
//         } 
//         if (next != NULL) {
//             FREE_PREV(next) = prev; /* next -> prev */
//         }        


//     }
// }

// /* append freed block to linked_list head*/
// void LIFO(void *bp) {
//     printf("lifo\n");
//     if (heap_listp != NULL) {
//         FREE_PREV(heap_listp) = bp;
//     }
//     FREE_PREV(bp) = NULL;
//     FREE_NEXT(bp) = heap_listp;
//     heap_listp = bp;
// }


// /*
//  * mm_free - Freeing a block does nothing.
//  */
// void mm_free(void *bp)
// {   
//     printf("free\n");
//     size_t size = GET_SIZE(HDRP(bp));
//     PUT(HDRP(bp), PACK(size, 0));
//     PUT(FTRP(bp), PACK(size, 0));
//     coalesce(bp);
//     // size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
//     // size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    
//     // if (prev_alloc && next_alloc) {
//     //     LIFO(bp); /* no linked free block*/
//     // } else {
//     //     coalesce(bp); /* link free block */
//     // }
// }

// /*
//  * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
//  */
// void *mm_realloc(void *bp, size_t size)
// {
//     void *old_bp = bp;
//     void *new_bp;
//     size_t copySize;
    
//     new_bp = mm_malloc(size);
//     if (new_bp == NULL)
//       return NULL;
//     copySize = GET_SIZE(HDRP(old_bp));
//     if (size < copySize)
//       copySize = size;
//     memcpy(new_bp, old_bp, copySize);  
//     mm_free(old_bp);
//     return new_bp;
// }

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
    "team 7",
    /* First member's full name */
    "Choi Subin",
    /* First member's email address */
    "lightsaber2929@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/************************************************************************/
/* Basic constants and macros */
#define WSIZE 4 /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */
#define CHUNKSIZE (1<<12) /* Extend heap by this amount (bytes) */
// 1 << 12: 비트 시프트 연산
// 이진수로 1을 왼쪽으로 12비트만큼 이동시킴
// 1000000000000 = 4096byte

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
/************************************************************************/

/************************************************************************/
// macro for explicit
// pred: 이전 블럭 찾기
#define PRED(bp) (*(void **)(bp))
// succ: 다음 블럭 찾기
#define SUCC(bp) (*(void **)((char *)(bp) + WSIZE))
// pred 에 주솟값 넣기
#define PUT_PRED(bp, pred) (PRED(bp) = (pred))
// succ 에 주솟값 넣기
#define PUT_SUCC(bp, succ) (SUCC(bp) = (succ))
/************************************************************************/

/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */
static char *free_listp = NULL;  /* 가용 블럭의 첫번째 블럭 */

/* declair prototype */
static void *extend_heap(size_t);
static void *coalesce(void *bp);
static void *find_fit(size_t);
static void place(void *bp, size_t);
void insert_free_block(void *bp);
void remove_block(void *bp);

/**
 * mm_init
 * : 할당기 초기화
 */
int mm_init(void) {
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(6*WSIZE)) == (void *)-1) {
        return -1;
    }

    PUT(heap_listp, 0);                             /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(2*DSIZE, 1));  /* Prologue header */
    PUT(heap_listp + (2*WSIZE), 0);                 /* prec 초기화 */
    PUT(heap_listp + (3*WSIZE), 0);                 /* succ 초기화 */
    PUT(heap_listp + (4*WSIZE), PACK(2*DSIZE, 1));  /* Prologue footer */
    PUT(heap_listp + (5*WSIZE), PACK(0, 1));        /* Epilogue header */

    // 탐색 시작점 설정
    free_listp = heap_listp + (6*WSIZE);
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    // CHUNKSIZE는 힙을 확장할 때 사용할 기본 크기
    // 이를 word size로 나누면 힙 내에 가능한 워드 개수가 나옴
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}


/**
 * extend_heap
 * : 주어진 word 크기만큼 heap 확장
 */
static void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    // 더블 워드 정렬을 맞춰주려고 size를 짝수로 맞춰줌
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1) {
        return NULL;
    }

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0)); /* Free block header */
    PUT(FTRP(bp), PACK(size, 0)); /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/**
 * coalesce
 * : 가용 블럭 간 통합(열 병합)
 * @return 연결된 가용블럭의 주소값
 */
static void *coalesce(void *bp) {
    // 앞, 뒤 블럭 할당여부 확인
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    /* Case 1: 앞, 뒤 모두 할당 */

    /* Case 2: 앞 할당 뒤 가용 */
    if (prev_alloc && !next_alloc) {
    // } else if (prev_alloc && !next_alloc) {
        remove_block(NEXT_BLKP(bp)); // 열 병합을 위한 뒤 가용 블럭 제거
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0)); 
    /* Case 3: 앞 가용 뒤 할당 */
    } else if (!prev_alloc && next_alloc) {
        remove_block(PREV_BLKP(bp)); // 열 병합을 위한 앞 가용 블럭을 제거
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        // bp = PREV_BLKP(bp);
    /* Case 4: 앞, 뒤 모두 가용 */
    } else if (!prev_alloc && !next_alloc) {
        // 열 병합을 위한 앞 뒤 가용 블럭 제거
        remove_block(PREV_BLKP(bp));
        remove_block(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        bp = PREV_BLKP(bp);
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    }

    // 가용 블럭 리스트에 추가
    insert_free_block(bp);

    return bp;
}


/**
 * mm_free
 * : 할당한 블럭의 가용 처리
 */
void mm_free(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    // header, footer 사이즈, 할당여부 값 갱신
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    // 열 병합 가능 여부 확인
    coalesce(bp);
}


/**
 * remove_block
 * : 할당 시 가용 블럭 리스트에서 삭제
 */
void remove_block(void *bp) {
    // 가용블럭의 첫번째 블록 삭제
    if (bp == free_listp) {
        // 새로운 헤드의 PRED를 NULL로 설정
        PRED(SUCC(bp)) = NULL;
        // 헤드를 갱신
        free_listp = SUCC(bp);
    } else {
        if (PRED(bp) != NULL) {
            SUCC(PRED(bp)) = SUCC(bp);
        }
        if (SUCC(bp) != NULL) {
            PRED(SUCC(bp)) = PRED(bp);
        }
    }
}


void *mm_malloc(size_t size) {
    size_t asize; /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if (size == 0) {
        return NULL;
    }

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE) {
        asize = 2*DSIZE;
    } else {
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);
    }

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL) {
        return NULL;
    }

    place(bp, asize);
    return bp;
}

/**
 * find_fit
 * : 묵시적 가용 리스트에서 first fit 검색을 수행하는 함수
 */
static void *find_fit(size_t size) {
    void *bp;
    /*
        first fit
        가용 리스트를 처음부터 검색해서 크기가 맞는 첫 번째 가용 블록을 선택한다
    */
    for (bp = free_listp; bp != NULL; bp = SUCC(bp)) {
        // 크기가 맞고, 할당 안 되어있으면 담기
        if (size <= GET_SIZE(HDRP(bp))) {
            return bp;
        }
    }

    return NULL;
}

/**
 * place
 * : 요청한 블록을 가용 블록의 시작 부분에 배치해야하며
 *   나머지 부분의 크기가 최소 블록 크기와 같거나 큰 경우에만 분할함
 * @params: 위치, 할당할 크기
 */
static void place(void *bp, size_t asize) {
    // 일단 현재 블럭 사이즈부터 가져오고
    size_t current_size = GET_SIZE(HDRP(bp));

    // 할당 시 가용블럭 리스트에서 삭제
    remove_block(bp);

    // 현재 블록 사이즈랑 요청한 블록 사이즈랑 비교해서 현재 블록 사이즈가 더 크면 분할
    // if (current_size > asize) {
    if ((current_size - asize) >= 2*DSIZE) {
        PUT(HDRP(bp), PACK(asize, 1)); // 헤더에 asize만큼 넣고 할당처리
        PUT(FTRP(bp), PACK(asize, 1)); // 푸터에 asize만큼 넣고 할당처리

        // 분할한 다음 위치로 이동
        bp = NEXT_BLKP(bp);

        // 쪼갠 나머지는 할당 가능하다고 처리
        PUT(HDRP(bp), PACK(current_size-asize, 0));
        PUT(FTRP(bp), PACK(current_size-asize, 0));

        insert_free_block(bp);
    // 요청한 사이즈와 가용 사이즈가 동일할 때
    } else {
        PUT(HDRP(bp), PACK(current_size, 1)); // 헤더에 current_size 넣고 할당처리
        PUT(FTRP(bp), PACK(current_size, 1)); // 푸터에 current_size 넣고 할당처리
    }
}

/**
 * insert_free_block
 * : 가용 블럭이 생기면 블럭을 리스트 내에 insert 하는 함수
 */
void insert_free_block(void *bp) {
    if (free_listp != NULL) {
        PRED(free_listp) = bp;
    }
    SUCC(bp) = free_listp;
    PRED(bp) = NULL;
    free_listp = bp;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
// void *mm_malloc(size_t size)
// {
//     int newsize = ALIGN(size + SIZE_T_SIZE);
//     void *p = mem_sbrk(newsize);
//     if (p == (void *)-1)
// 	return NULL;
//     else {
//         *(size_t *)p = size;
//         return (void *)((char *)p + SIZE_T_SIZE);
//     }
// }

/*
 * mm_free - Freeing a block does nothing.
 */
// void mm_free(void *ptr)
// {
// }

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    if (size <= 0) { 
        mm_free(ptr);
        return 0;
    }
    if (ptr == NULL) {
        return mm_malloc(size); 
    }

    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);

    if (newptr == NULL) {
        return NULL;
    }

    // copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    copySize = GET_SIZE(HDRP(ptr));

    if (size < copySize) {
        copySize = size;
    }

    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

