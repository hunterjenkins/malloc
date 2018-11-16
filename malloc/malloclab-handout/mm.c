#include "mm.h"

#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

#define OVERHEAD sizeof(block_header)
#define HDRP(bp) ((char *)(bp) - sizeof(block_header))

#define GET_SIZE(p)  ((block_header *)(p))->size
#define GET_ALLOC(p) ((block_header *)(p))->allocated

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))

typedef struct {
  size_t size;
  char   allocated;
} block_header;

void *first_bp;

void mm_init(void *heap, size_t heap_size)
{
  void *bp;
  
  bp = heap + sizeof(block_header);
  
  GET_SIZE(HDRP(bp)) = heap_size;
  GET_ALLOC(HDRP(bp)) = 0;

  first_bp = bp;
}

static void set_allocated(void *bp)
{
  GET_ALLOC(HDRP(bp)) = 1;
}

void *mm_malloc(size_t size)
{
  void *bp = first_bp;

  if (GET_ALLOC(HDRP(bp))) {
    /* Our only block is already allocated */
    return NULL;
  } else {
    set_allocated(bp);
    return bp;
  }
}

void mm_free(void *bp)
{
  GET_ALLOC(HDRP(bp)) = 0;
}
