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
//     "week06-05",
//     /* First member's full name */
//     "Dapsu",
//     /* First member's email address */
//     "greenrock4@skku.edu",
//     /* Second member's full name (leave blank if none) */
//     "",
//     /* Second member's email address (leave blank if none) */
//     ""
// };

// // Basic constants and macors
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
// #define GET_ALLOC(p)   (GET(p) & 0x1)   // 현재 블록 가용 여부 판단(0이면 alloc, 1이면 free)

// // bp(현재 블록의 포인터)로 현재 블록의 header 위치와 footer 위치 반환
// #define HDRP(bp)    ((char *)(bp) - WSIZE)
// #define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// // 다음과 이전 블록의 포인터 반환
// #define NEXT_BLKP(bp)   (((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE)))    // 다음 블록 bp 위치 반환(bp + 현재 블록의 크기)
// #define PREV_BLKP(bp)   (((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE)))    // 이전 블록 bp 위치 반환(bp - 이전 블록의 크기)

// // Declaration
// static void *heap_listp;
// static void *extend_heap(size_t words);
// static void *coalesce(void *bp);
// static void *find_fit(size_t a_size);
// static void place(void *bp, size_t a_size);

// /* 
//  * mm_init - initialize the malloc package.
//  */
// int mm_init(void)
// {
//     // Create the initial empty heap
//     if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) {  // heap_listp가 힙의 최댓값 이상을 요청한다면 fail
//         return -1;
//     }

//     PUT(heap_listp, 0);                             // Alignment padding
//     PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    // Prologue header
//     PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    // Prologue footer
//     PUT(heap_listp + (3*WSIZE), PACK(0, 1));        // Epilogue header
//     heap_listp += (2*WSIZE);

//     // Extend the empty heap with a free block of CHUNKSIZE bytes
//     if (extend_heap(CHUNKSIZE/WSIZE) == NULL) {
//         return -1;
//     }
//     return 0;
// }

// /*
// extend_heap 사용 경우
//   1) 힙이 초기화될 때
//   2) mm_malloc이 적당한 fit을 찾지 못했을 때
// */
// static void *extend_heap(size_t words) {
//     char *bp;
//     size_t size;

//     // Allocate an even number of words  to maintain alignment
//     size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; // words가 홀수면 +1을 해서 공간 할당
//     if ((long)(bp = mem_sbrk(size)) == -1) { // NULL은 유효한 주소이지만, void( *)-1은 유효한 주소가 아님.
//         return NULL;
//     }

//     // initialize free block header/footer and the epilogue header
//     PUT(HDRP(bp), PACK(size, 0));
//     PUT(FTRP(bp), PACK(size, 0));
//     PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
//     /*
//     extend_heap 블록 너머에 오지 않도록 배치한 블록 다음 공간을 블록이라 가정하고 epilogue header 배치
//     (실제로는 존재하지 않는 블록)
//     */

//     // coalesce if the previous block was free
//     return coalesce(bp);   
// }

// static void *coalesce(void *bp) {
//     size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
//     size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
//     size_t size = GET_SIZE(HDRP(bp));

//     // case1: 앞, 뒤 블록 모두 할당되어 있을 때
//     if (prev_alloc && next_alloc) {
//         return bp;
//     }

//     // case2: 앞 블록 할당, 뒷 블록 가용
//     else if (prev_alloc && !next_alloc) {
//         size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
//         PUT(HDRP(bp), PACK(size, 0));
//         PUT(FTRP(bp), PACK(size, 0));
//     }

//     // case3: 앞 블록 가용, 뒷 블록 할당
//     else if (!prev_alloc && next_alloc) {
//         size += GET_SIZE(HDRP(PREV_BLKP(bp)));
//         PUT(FTRP(bp), PACK(size, 0));
//         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//         bp = PREV_BLKP(bp);
//     }

//     // case4: 앞, 뒤 블록 모두 가용
//     else {
//         size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
//         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//         PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
//         bp = PREV_BLKP(bp);
//     }
//     return bp;
// } 


// /* 
//  * mm_malloc - Allocate a block by incrementing the brk pointer.
//  *     Always allocate a block whose size is a multiple of the alignment.
//  */
// void *mm_malloc(size_t size) {
//     size_t a_size;       // adjusted block szie
//     size_t extend_size;  // Amount to extend heap if no fit
//     char *bp;

//     // Ignore spurious requests
//     if (size == 0) {
//         return NULL;
//     }

//     // Adjust block size to include overhead and alignment reqs
//     if (size <= DSIZE) {    // 2words 이하의 사이즈는 4워드로 할당 요청 (header 1word, footer 1word)
//         a_size = 2*DSIZE;
//     }
//     else {                  // 할당 요청의 용량이 2words 초과 시, 충분한 8byte의 배수의 용량 할당
//         a_size = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); //size가 8의 배수일 때, padding이 없게끔
//     }

//     // Search the free list for a fit
//     if ((bp = find_fit(a_size)) != NULL) {   // 적당한 크기의 가용 블록 검색
//         place(bp, a_size);                   // 초과 부분을 분할하고 새롭게 할당한 블록의 포인터 반환
//         return bp;
//     }

//     // NO fit found. Get more memory and place the block
//     extend_size = MAX(a_size, CHUNKSIZE);
//     if ((bp = extend_heap(extend_size/WSIZE)) == NULL) {    // 칸의 개수
//         return NULL;
//     }
//     place(bp, a_size);
//     return bp;
// }

// static void *find_fit(size_t a_size) {
//     void *bp;

//     for (bp = (char *)heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
//         if (!GET_ALLOC(HDRP(bp)) && (a_size <= GET_SIZE(HDRP(bp)))) {
//             return bp;
//         }
//     }
//     return NULL;    // NO fit
// }

// // static void *find_fit(size_t a_size) {
// //     char *bp = heap_listp;
// //     bp = NEXT_BLKP(bp);
// //     while (GET_SIZE(HDRP(bp)) < a_size || GET_ALLOC(HDRP(bp)) == 1) {   // bp가 적용될 블록의 크기보다 작고, free일 때
// //         bp = NEXT_BLKP(bp);
// //         if (GET_SIZE(HDRP(bp)) == 0) {      // Epilogue를 만났을 때
// //             return NULL;
// //         }
// //     }
// //     return bp;
// // }

// static void place(void *bp, size_t a_size) {
//     size_t c_size = GET_SIZE(HDRP(bp));

//     if ((c_size - a_size) >= (2 * (DSIZE))) {
//         // 요청 용량 만큼 블록 배치
//         PUT(HDRP(bp), PACK(a_size, 1));
//         PUT(FTRP(bp), PACK(a_size, 1));
        
//         bp = NEXT_BLKP(bp);
//         // 남은 블록에 header, footer 배치
//         PUT(HDRP(bp), PACK(c_size - a_size, 0));
//         PUT(FTRP(bp), PACK(c_size - a_size, 0));
//     }
//     else {      // csize와 aszie 차이가 네 칸(16byte)보다 작다면 해당 블록 통째로 사용
//         PUT(HDRP(bp), PACK(c_size, 1));
//         PUT(FTRP(bp), PACK(c_size, 1));
//     }
// }

// /*
//  * mm_free - Freeing a block does nothing.
//  */
// void mm_free(void *bp)
// {
//     size_t size = GET_SIZE(HDRP(bp));

//     PUT(HDRP(bp), PACK(size, 0));
//     PUT(FTRP(bp), PACK(size, 0));
//     coalesce(bp);
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
//     memcpy(new_bp, old_bp, copySize);  // 메모리의 특정한 부분으로부터 얼마까지의 부분을 다른 메모리 영역으로 복사해주는 함수(old_bp로부터 copySize만큼의 문자를 new_bp로 복사해라)
//     mm_free(old_bp);
//     return new_bp;
// }


//explicit 구현
//1. implicit의 기본 골격을 유지
//2. find_fit을 할 때 가용블럭만 순회
//3. 가용블럭에다가 이전 가용블럭의 주소, 다음 가용블럭의 주소를 저장

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
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "week06-05",
    /* First member's full name */
    "Taewoo",
    /* First member's email address */
    "terry9508@naver.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

// Basic constants and macors
#define WSIZE       4           // Word and header/footer size(bytes)
#define DSIZE       8           // Double word size (btyes)
#define CHUNKSIZE   (1 << 12)   // Extend heap by this amount (bytes) : 초기 가용 블록과 힙 확장을 위한 기본 크기

#define MAX(x, y)   ((x) > (y) ? (x) : (y))    // x > y가 참이면 x, 거짓이면 y
#define MIN(x, y)   ((uintptr_t)(x) > (uintptr_t)(y) ? (y) : (x))

// PACK매크로 : 크기와 할당 비트를 통합해서 header와 footer에 저장할 수 있는 값 리턴
#define PACK(size, alloc)   ((size) | (alloc))

// Read and wirte a word at address
#define GET(p)  (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

// Read the size and allocated field from address p
#define GET_SIZE(p)    (GET(p) & ~0x7)  // header or footer의 사이즈 반환(8의 배수)
#define GET_ALLOC(p)   (GET(p) & 0x1)   // 현재 블록 가용 여부 판단(0이면 free, 1이면 alloc)

// bp(현재 블록의 포인터)로 현재 블록의 header 위치와 footer 위치 반환
#define HDRP(bp)    ((char *)(bp) - WSIZE)
#define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// 다음과 이전 블록의 포인터 반환
#define NEXT_BLKP(bp)   (((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE)))    // 다음 블록 bp 위치 반환(bp + 현재 블록의 크기)
#define PREV_BLKP(bp)   (((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE)))    // 이전 블록 bp 위치 반환(bp - 이전 블록의 크기)

// #define PREV(bp)    (*(char**)(bp))
// #define NEXT(bp)    (*(char**)(bp)+WSIZE)

// Declaration
static void *heap_listp;
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t a_size);
static void place(void *bp, size_t a_size);
static char *free_start;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // Create the initial empty heap
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) {  // heap_listp가 힙의 최댓값 이상을 요청한다면 fail
        return -1;
    }

    PUT(heap_listp, 0);                             // Alignment padding
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    // Prologue header
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    // Prologue footer
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));        // Epilogue header
    heap_listp += (2*WSIZE);
    free_start = heap_listp + (2*WSIZE); //초기화시 첫 가용블록의 데이터영역 시작부분

    // Extend the empty heap with a free block of CHUNKSIZE bytes
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) {
        return -1;
    }

    return 0;
}

char *find_prev(char *bp) {
    bp = PREV_BLKP(bp);
    while (GET_SIZE(HDRP(bp)) > DSIZE) {
        if (!GET_ALLOC(bp)) {
            return bp;
        }
        bp = PREV_BLKP(bp);
    }
    return bp; /* 프롤로그 블록 */
}

char *find_succ(void *bp) {
    bp = NEXT_BLKP(bp);
    while (GET_SIZE(HDRP(bp)) > 0) {
        if (!GET_ALLOC(bp)) {
            return bp;
        }
        bp = NEXT_BLKP(bp);
    }
    return bp; /* 에필로그 블록 */
}

/*
extend_heap 사용 경우
  1) 힙이 초기화될 때
  2) mm_malloc이 적당한 fit을 찾지 못했을 때
*/
static void *extend_heap(size_t words) {
    char *bp;
    size_t size;

    // Allocate an even number of words  to maintain alignment
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; // words가 홀수면 +1을 해서 공간 할당
    if ((long)(bp = mem_sbrk(size)) == -1) { // NULL은 유효한 주소이지만, void( *)-1은 유효한 주소가 아님.
        return NULL;
    }

    // initialize free block header/footer and the epilogue header
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    /*
    extend_heap 블록 너머에 오지 않도록 배치한 블록 다음 공간을 블록이라 가정하고 epilogue header 배치
    (실제로는 존재하지 않는 블록)
    */

    // coalesce if the previous block was free
    return coalesce(bp);   
}
static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    void *prev_block = PREV_BLKP(bp);
    void *next_block = NEXT_BLKP(bp);

    // case1: 앞, 뒤 블록 모두 할당되어 있을 때
    if (prev_alloc && next_alloc) {
        // 가용 리스트에서 이전 블록과 다음 블록 찾기
        void *bp_prev = find_prev(bp);
        void *bp_next = find_succ(bp);

        // 가용 리스트 연결
        *(char **)bp = bp_prev;        // bp의 첫 번째 부분에 이전 블록 주소 저장
        *(char **)(bp + WSIZE) = bp_next;  // bp의 두 번째 부분에 다음 블록 주소 저장
        
        // 이전 블록이 프롤로그 블록이 아닐 때만, 그 블록이 bp를 가리키도록 설정
        if (!GET_ALLOC(HDRP(bp_prev))) {
            *(char **)(bp_prev + WSIZE) = bp;
        }

        // 다음 블록이 에필로그 블록이 아닐 때만, 그 블록이 bp를 가리키도록 설정
        if (!GET_ALLOC(HDRP(bp_next))) {
            *(char **)bp_next = bp;
        }
        
        return bp;
    }

    // case2: 앞 블록 할당, 뒷 블록 가용
    else if (prev_alloc && !next_alloc) {
        size += GET_SIZE(HDRP(next_block));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));

        // 뒷 블록 정보 현재 위치로 가져오기
        *(char **)bp = *(char **)next_block;
        *(char **)(bp + WSIZE) = *(char **)(next_block + WSIZE);

        // 뒷 블록 초기화
        *(char **)next_block = NULL;
        *(char **)(next_block + WSIZE) = NULL;
    }

    // case3: 앞 블록 가용, 뒷 블록 할당
    else if (!prev_alloc && next_alloc) {
        size += GET_SIZE(HDRP(prev_block));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(prev_block), PACK(size, 0));
        bp = prev_block;
    }

    // case4: 앞, 뒤 블록 모두 가용
    else {
        size += GET_SIZE(HDRP(prev_block)) + GET_SIZE(FTRP(next_block));
        PUT(HDRP(prev_block), PACK(size, 0));
        PUT(FTRP(next_block), PACK(size, 0));

        // 뒷 블록의 뒤 정보만 가져오기
        *(char **)(prev_block + WSIZE) = *(char **)(next_block + WSIZE);

        // 뒷 블록 정보 초기화
        *(char **)next_block = NULL;
        *(char **)(next_block + WSIZE) = NULL;

        bp = prev_block;
    }

    return bp;
}
// static void *coalesce(void *bp) {
//     size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
//     size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
//     size_t size = GET_SIZE(HDRP(bp));
//     char **free_mark = (char **)((char *)bp);
//     // case1: 앞, 뒤 블록 모두 할당되어 있을 때
//     if (prev_alloc && next_alloc) {
//         void *bp_prev = find_prev(bp);
//         void *bp_next = find_succ(bp);

//         *(free_mark) = bp_prev;
//         *(free_mark + WSIZE) = bp_next;
        
//         //prev가 프롤로그블록이 아닐 때
//         if (!GET_ALLOC(HDRP(bp_prev))) {
//             *(void **)((char *)bp_prev + WSIZE) = free_mark;
//         }

//         //next가 에필로그블록이 아닐 때
//         if (!GET_ALLOC(HDRP(bp_next))) {
//             *(void **)(bp_next) = free_mark;
//         }        
//         return bp;
//     }

//     // case2: 앞 블록 할당, 뒷 블록 가용
//     else if (prev_alloc && !next_alloc) {
//         size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
//         PUT(HDRP(bp), PACK(size, 0));
//         PUT(FTRP(bp), PACK(size, 0));
//         //뒷블록의 정보 현재 위치에 가져오기
//         *(free_mark) = *((char **)NEXT_BLKP(bp));
//         *(free_mark+WSIZE) = *((char **)(NEXT_BLKP(bp)+WSIZE));
//         //뒷블록 초기화
//         *(NEXT_BLKP(bp)) = 0;
//         *(NEXT_BLKP(bp)+WSIZE) = 0;    
//     }

//     // case3: 앞 블록 가용, 뒷 블록 할당
//     else if (!prev_alloc && next_alloc) {
//         size += GET_SIZE(HDRP(PREV_BLKP(bp)));
//         PUT(FTRP(bp), PACK(size, 0));
//         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//         bp = PREV_BLKP(bp);
//     }

//     // case4: 앞, 뒤 블록 모두 가용
//     else {
//         size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
//         PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//         PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
//         //뒷블록의 뒷정보만 가져오기
//         *(PREV_BLKP(bp)+WSIZE) = *(NEXT_BLKP(bp)+WSIZE);
//         //뒷블록 정보 초기화
//         *(NEXT_BLKP(bp)) = 0;
//         *(NEXT_BLKP(bp)+WSIZE) = 0;
//         bp = PREV_BLKP(bp);
//     }
//     return bp;
// } 


/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
    size_t a_size;       // adjusted block szie
    size_t extend_size;  // Amount to extend heap if no fit
    char *bp;

    // Ignore spurious requests
    if (size == 0) {
        return NULL;
    }

    // Adjust block size to include overhead and alignment reqs
    if (size <= DSIZE) {    // 2words 이하의 사이즈는 4워드로 할당 요청 (header 1word, footer 1word)
        a_size = 2*DSIZE;
    }
    else {                  // 할당 요청의 용량이 2words 초과 시, 충분한 8byte의 배수의 용량 할당
        a_size = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); //size가 8의 배수일 때, padding이 없게끔
    }

    // Search the free list for a fit
    if ((bp = find_fit(a_size)) != NULL) {   // 적당한 크기의 가용 블록 검색
        place(bp, a_size);                   // 초과 부분을 분할하고 새롭게 할당한 블록의 포인터 반환
        return bp;
    }

    // NO fit found. Get more memory and place the block
    extend_size = MAX(a_size, CHUNKSIZE);
    if ((bp = extend_heap(extend_size/WSIZE)) == NULL) {    // 칸의 개수
        return NULL;
    }
    place(bp, a_size);
    return bp;
}

static void *find_fit(size_t a_size) {
    char *bp = free_start;
    // bp = free_start;
    while (GET_SIZE(HDRP(bp)) > 0) {
        if (a_size <= GET_SIZE(HDRP(bp))) {
            return bp;
        }
        bp = *(void **)(bp + WSIZE); // 다음 가용블록 주소
    }
    // for (bp = (char *)heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
    //     if (!GET_ALLOC(HDRP(bp)) && (a_size <= GET_SIZE(HDRP(bp)))) {
    //         return bp;
    //     }
    // }
    return NULL;    // NO fit
};

// static void *find_fit(size_t a_size) {
//     char *bp = heap_listp;
//     bp = NEXT_BLKP(bp);
//     while (GET_SIZE(HDRP(bp)) < a_size || GET_ALLOC(HDRP(bp)) == 1) {   // bp가 적용될 블록의 크기보다 작고, free일 때
//         bp = NEXT_BLKP(bp);
//         if (GET_SIZE(HDRP(bp)) == 0) {      // Epilogue를 만났을 때
//             return NULL;
//         }
//     }
//     return bp;
// }

static void place(void *bp, size_t a_size) {
    size_t c_size = GET_SIZE(HDRP(bp));
    
    char *next_block = (char *)(bp+a_size); // 할당된 부분을 제외한 가용블록
    if ((c_size - a_size) >= (2 * (DSIZE))) {
        // 요청 용량 만큼 블록 배치
        PUT(HDRP(bp), PACK(a_size, 1));
        PUT(FTRP(bp), PACK(a_size, 1));
        //만약 bp가 free_start라면 free_start를 a_size만큼 늘려줌
        if (bp == free_start) {
            free_start = free_start + a_size;
        }
        //주소 옮기기
        *(char **)next_block = *(char **)bp; //이전 블록
        *(char **)(next_block + WSIZE) = *(char **)(bp + WSIZE); //다음 블록
        *(char **)next_block = (char *)next_block;
        *(char **)(next_block + WSIZE) = (char *)next_block;
        //주소값 초기화
        *(char **)(bp) = 0;
        *(char **)(bp + WSIZE) = 0;   
        bp = NEXT_BLKP(bp);
        // 남은 블록에 header, footer 배치
        PUT(HDRP(bp), PACK(c_size - a_size, 0));
        PUT(FTRP(bp), PACK(c_size - a_size, 0));
    }
    else {      // csize와 aszie 차이가 네 칸(16byte)보다 작다면 해당 블록 통째로 사용
        PUT(HDRP(bp), PACK(c_size, 1));
        PUT(FTRP(bp), PACK(c_size, 1));
        //bp가 free_start라면 다음 블록이 첫 가용 주소
        if (bp == free_start) {
            free_start = *(char **)(bp + WSIZE);
        }

        void *bp_prev = *(char **)(bp); //이전 블록
        void *bp_next = *(char **)(bp + WSIZE); //다음 블록
        if (!GET_ALLOC(HDRP(bp_prev))) {
            *(void **)(bp_prev + WSIZE) = bp_next;//이전 블록의 다음은 다음블록으로
        } 
        if (!GET_ALLOC(HDRP(bp_next))) {
            *(void **)(bp_next) = bp_prev;//다음블록의 이전은 이전 블록
        } 
        // *(char **)(bp_prev + WSIZE) = 
        // *(char **)(bp_next) =          
    }
}
// static void place(void *bp, size_t a_size) {
//     size_t c_size = GET_SIZE(HDRP(bp));
    
//     // 블록을 분할할 수 있을 정도로 크다면
//     if ((c_size - a_size) >= (2 * DSIZE)) {
//         // 요청 용량만큼 블록 배치
//         PUT(HDRP(bp), PACK(a_size, 1));
//         PUT(FTRP(bp), PACK(a_size, 1));
        
//         // 남은 공간에 대해 가용 블록으로 설정
//         bp = NEXT_BLKP(bp);
//         PUT(HDRP(bp), PACK(c_size - a_size, 0));
//         PUT(FTRP(bp), PACK(c_size - a_size, 0));

//         // 가용 리스트 연결 업데이트
//         *(char **)bp = NULL;
//         *(char **)(bp + WSIZE) = NULL;
//     }
//     else { // c_size와 a_size 차이가 작으면 해당 블록 전체 사용
//         PUT(HDRP(bp), PACK(c_size, 1));
//         PUT(FTRP(bp), PACK(c_size, 1));
//         if (bp == free_start) {
//             free_start = *(char **)(bp + WSIZE);
//         }
//         // 가용 리스트에서 블록 제거
//         char *bp_prev = *(char **)(bp); // 이전 블록
//         char *bp_next = *(char **)(bp + WSIZE); // 다음 블록
//         if (bp_prev && !GET_ALLOC(HDRP(bp_prev))) {
//             *(char **)(bp_prev + WSIZE) = bp_next;
//         }
//         if (bp_next && !GET_ALLOC(HDRP(bp_next))) {
//             *(char **)bp_next = bp_prev;
//         }
//     }
// }
// static void place(void *bp, size_t a_size) {
//     size_t c_size = GET_SIZE(HDRP(bp)); // 현재 블록의 크기 가져오기
//     char *next_block = (char *)(bp + a_size); // 할당된 부분 이후의 가용 블록 시작 주소

//     if ((c_size - a_size) >= (2 * DSIZE)) { // 남은 블록이 최소 블록 크기 이상일 때
//         // 요청 용량만큼 블록 배치 (header와 footer 설정)
//         PUT(HDRP(bp), PACK(a_size, 1)); // 헤더에 할당 표시
//         PUT(FTRP(bp), PACK(a_size, 1)); // 푸터에 할당 표시

//         // 이전, 다음 블록 연결 갱신 (free list)
//         *(char **)next_block = *(char **)bp; // 이전 블록 연결
//         *(char **)(next_block + WSIZE) = *(char **)(bp + WSIZE); // 다음 블록 연결
//         *(char **)(*(char **)next_block + WSIZE) = next_block; // 이전 블록의 다음 포인터 업데이트
//         *(char **)(*(char **)(next_block + WSIZE)) = next_block; // 다음 블록의 이전 포인터 업데이트

//         // 현재 블록의 이전 및 다음 포인터 초기화
//         *(char **)bp = NULL;
//         *(char **)(bp + WSIZE) = NULL;

//         // 남은 블록을 가용 블록으로 설정
//         bp = NEXT_BLKP(bp); // 다음 블록으로 이동
//         PUT(HDRP(bp), PACK(c_size - a_size, 0)); // 헤더에 남은 크기 및 가용 표시
//         PUT(FTRP(bp), PACK(c_size - a_size, 0)); // 푸터에 남은 크기 및 가용 표시
//     } 
//     else { // 남은 공간이 충분하지 않으면 블록 전체를 사용
//         PUT(HDRP(bp), PACK(c_size, 1)); // 전체 블록을 할당 표시
//         PUT(FTRP(bp), PACK(c_size, 1));

//         // 이전, 다음 블록 연결 갱신 (free list)
//         char *bp_prev = *(char **)bp; // 이전 블록
//         char *bp_next = *(char **)(bp + WSIZE); // 다음 블록

//         *(char **)(bp_prev + WSIZE) = bp_next; // 이전 블록의 다음 포인터를 다음 블록으로 설정
//         *(char **)bp_next = bp_prev; // 다음 블록의 이전 포인터를 이전 블록으로 설정
//     }
// }
/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    bp = coalesce(bp);
    free_start = MIN(bp, free_start);
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
    memcpy(new_bp, old_bp, copySize);  // 메모리의 특정한 부분으로부터 얼마까지의 부분을 다른 메모리 영역으로 복사해주는 함수(old_bp로부터 copySize만큼의 문자를 new_bp로 복사해라)
    mm_free(old_bp);
    return new_bp;
}