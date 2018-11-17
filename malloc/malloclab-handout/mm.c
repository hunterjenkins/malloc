#include "mm.h"

// This variant of "mm.c" is like the original code, but it allows
// up to 2 allocation blocks insteda of just 1 allocation block;
// it starts with a single block and a terminator, and splits that
// first block if it can

//rounds up to a multiple of the alignment for payloads and for block sizes
#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

//excess space in block not used for payload
#define OVERHEAD sizeof(block_header)

//returns a pointer to the header to the given payload's block
//bp = "Block Payload pointer"
#define HDRP(bp) ((char *)(bp) - sizeof(block_header))

//returns the size of the given block
#define GET_SIZE(p)  ((block_header *)(p))->size

//returns whether the block is allocated or not
#define GET_ALLOC(p) ((block_header *)(p))->allocated

//returns a payload pointer for the next block, given a payload pointer
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))

//representation of header and is a multiple of the alignment
typedef struct {
  size_t size;
  char   allocated;
} block_header;


//pointer to the payload of the first block in our heap
void *first_bp;

//heap      = where the heap starts in memory?
//heap_size = how long the heap will be?
void mm_init(void *heap, size_t heap_size)
{
  void *bp;

  printf("Heap size: %ld\n", heap_size);


  // here's a place where we depend on
  // block_header being a multiple of ALIGNMENT:
  bp = heap + sizeof(block_header);

  // relying on heap_size being a multiple of 16,
  // which is part of the spec for mm_init
  //assuming block header is a multiple of alignment

  //this is actually setting the size
  GET_SIZE(HDRP(bp)) = heap_size - sizeof(block_header);

  //sets the allocation status
  GET_ALLOC(HDRP(bp)) = 0;


  //create terminator block

  //The next bp is the very end, which will be the terminator block
  GET_SIZE(HDRP(NEXT_BLKP(bp))) = 0;
  GET_ALLOC(HDRP(NEXT_BLKP(bp))) = 1;
  first_bp = bp;
}


//Helper method for mm_malloc
static void set_allocated(void *bp, size_t block_size)
{
  size_t current_size = GET_SIZE(HDRP(bp));
  void* second_bp = bp + block_size;
  GET_ALLOC(HDRP(bp)) = 1;
  if((block_size + OVERHEAD) < current_size)
    {
      GET_SIZE(HDRP(bp)) = block_size;
      //second_bp = NEXT_BLKP(bp);
      GET_ALLOC(HDRP(second_bp)) = 0;
      GET_SIZE(HDRP(second_bp)) = current_size - block_size;
    }
}



//Size = how many bytes we want to allocate
void *mm_malloc(size_t size)
{
  void *bp = first_bp; //I don't know if we'll want this to be the first_bp. TODO

  //Don't stop until we've found a place or every place is taken up
  // while (1)
  // {

    //Find the null terminator block
    if (GET_SIZE(HDRP(bp)) == 0)
    {
      printf("Found the null terminator block\n");
      //break;
    }


    //If this bp is allocated or if there is no space at this block
    if (GET_ALLOC(HDRP(bp)) == 1 || ((size + OVERHEAD) > GET_SIZE(HDRP(bp))))
    {
      //Go to the next block
      bp = NEXT_BLKP(bp);

      // if (GET_ALLOC(HDRP(bp)) == 1 || ((size + OVERHEAD) > GET_SIZE(HDRP(bp))))
      // {
      //   set_allocated(bp, ALIGN(size + OVERHEAD));
      // }
      // else
      // {
      //   return NULL;
      // }

    }
    else
    {
      set_allocated(bp, ALIGN(size + OVERHEAD));
      return bp;
    }



  //}


}

void mm_free(void *bp)
{
  // Callers must ensure that bp is actually allocated

  GET_ALLOC(HDRP(bp)) = 0;
}
