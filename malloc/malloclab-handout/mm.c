#include "mm.h"

// This variant of "mm.c" is like the original code, but it allows
// up to 2 allocation blocks insteda of just 1 allocation block;
// it starts with a single block and a terminator, and splits that
// first block if it can

//rounds up to a multiple of the alignment for payloads and for block sizes
#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

//excess space in block not used for payload
#define OVERHEAD (sizeof(block_header) + sizeof(block_footer))

//returns a pointer to the header to the given payload's block
//bp = "Block Payload pointer"
#define HDRP(bp) ((char *)(bp) - (sizeof(block_header) ) )

//returns a pointer to the footer to the given payload's block
#define FTRP(bp) ((char *)(bp)+GET_SIZE(HDRP(bp))-OVERHEAD)


//returns a payload pointer for the next block, given a payload pointer
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE((char *)(bp)-OVERHEAD))

#define GET(p) (*(size_t *)(p))


#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_SIZE(p) (GET(p) & ~0xF)


//set the size
#define PUT(p, val) (*(size_t *)(p) = (val))
#define PACK(size, alloc) ((size) | (alloc))

typedef size_t block_header;
typedef size_t block_footer;


//pointer to the payload of the first block in our heap
void *first_bp;

//heap      = where the heap starts in memory?
//heap_size = how long the heap will be?
void mm_init(void *heap, size_t heap_size)
{
  void *bp;
  bp = heap + sizeof(block_header) + 8;

  // relying on heap_size being a multiple of 16,
  // which is part of the spec for mm_init
  //assuming block header is a multiple of alignment

  //this is actually setting the size
  PUT(HDRP(bp), PACK((heap_size - ( sizeof(block_header) + sizeof(block_footer))), 0 ));

  first_bp = bp;

  //create terminator block
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
  mm_malloc(0);
  return 0;
}


//Helper method for mm_malloc
static void set_allocated(void *bp, size_t size)
{
  printf("set_allocated\n");


  size_t extra_size = GET_SIZE(HDRP(bp)) - size;
  if (extra_size > ALIGN(1 + OVERHEAD)){

    PUT(HDRP(bp), size);
    PUT(FTRP(bp), size);

    PUT(HDRP(NEXT_BLKP(bp)), PACK(extra_size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), extra_size);

  }

  PUT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));

}



//Size = how many bytes we want to allocate
void *mm_malloc(size_t size)
{
  void *bp = first_bp; //I don't know if we'll want this to be the first_bp. TODO

  //Don't stop until we've found a place or every place is taken up
  while (1)
  {

    //Find the null terminator block
    if (GET_SIZE(HDRP(bp)) == 0)
    {
      return NULL;
      break;
    }

    //If this bp is allocated or if there is no space at this block
    if (GET_ALLOC(HDRP(bp)) == 1 || ((size + OVERHEAD) > GET_SIZE(HDRP(bp))))
    {
      //Go to the next block
      bp = NEXT_BLKP(bp);
    }
    else
    {
      set_allocated(bp, ALIGN(size + OVERHEAD));
      return bp;
    }
  }
}


void *coalesce(void *bp);

void mm_free(void *bp)
{
  // Callers must ensure that bp is actually allocated
  size_t sz = GET_SIZE(HDRP(bp));
  PUT(HDRP(bp), PACK(sz, 0));

  coalesce(bp);
}


void *coalesce(void *bp) {

  size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size       = GET_SIZE(HDRP(bp));


  if (prev_alloc && next_alloc) { /* Case 1 */
  }

  else if (prev_alloc && !next_alloc) { /* Case 2 */
     size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
     PUT(HDRP(bp), PACK(size, 0));
     PUT(FTRP(bp), PACK(size, 0));
  }

  else if (!prev_alloc && next_alloc) { /* Case 3 */
     size += GET_SIZE(HDRP(PREV_BLKP(bp)));
     PUT(FTRP(bp), PACK(size, 0));
     PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
     bp = PREV_BLKP(bp);
   }

   else { /* Case 4 */
     size += (GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp))));

     PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
     PUT(FTRP(PREV_BLKP(bp)), PACK(size, 0));
     bp = PREV_BLKP(bp);
   }

}
