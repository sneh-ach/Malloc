#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN4(s) (((((s)-1) >> 2) << 2) + 4)
#define BLOCK_DATA(b) ((b) + 1)
#define BLOCK_HEADER(ptr) ((struct _block *)(ptr)-1)

static int atexit_registered = 0;
static int num_mallocs = 0;
static int num_frees = 0;
static int num_reuses = 0;
static int num_grows = 0;
static int num_splits = 0;
static int num_coalesces = 0;
static int num_blocks = 0;
static int num_requested = 0;
static int max_heap = 0;

/*
 *  \brief printStatistics
 *
 *  \param none
 *
 *  Prints the heap statistics upon process exit.  Registered
 *  via atexit()
 *
 *  \return none
 */
void printStatistics(void)
{
   printf("\nheap management statistics\n");
   printf("mallocs:\t%d\n", num_mallocs);
   printf("frees:\t\t%d\n", num_frees);
   printf("reuses:\t\t%d\n", num_reuses);
   printf("grows:\t\t%d\n", num_grows);
   printf("splits:\t\t%d\n", num_splits);
   printf("coalesces:\t%d\n", num_coalesces);
   printf("blocks:\t\t%d\n", num_blocks);
   printf("requested:\t%d\n", num_requested);
   printf("max heap:\t%d\n", max_heap);
}

struct _block
{
   size_t size;         /* Size of the allocated _block of memory in bytes */
   struct _block *next; /* Pointer to the next _block of allcated memory   */
   bool free;           /* Is this _block free?                            */
   char padding[3];     /* Padding: IENTRTMzMjAgU3ByaW5nIDIwMjM            */
};

struct _block *heapList = NULL; /* Free list to track the _blocks available */

/*
 * \brief findFreeBlock
 *
 * \param last pointer to the linked list of free _blocks
 * \param size size of the _block needed in bytes
 *
 * \return a _block that fits the request or NULL if no free _block matches
 *
 * \TODO Implement Next Fit
 * \TODO Implement Best Fit
 * \TODO Implement Worst Fit
 */

struct _block *next_fit_ptr = NULL;

struct _block *findFreeBlock(struct _block **last, size_t size)
{
   *last = NULL;
   struct _block *curr = heapList;

#if defined FIT && FIT == 0
   // First Fit
   while (curr && !(curr->free && curr->size >= size))
   {
      *last = curr;
      curr = curr->next;
   }
#endif

#if defined BEST && BEST == 0
   // Best Fit
   struct _block *best = NULL;
   while (curr)
   {
      if (curr->free && curr->size >= size)
      {
         if (!best || curr->size < best->size)
         {
            best = curr;
         }
      }
      *last = curr;
      curr = curr->next;
   }
   curr = best;
#endif

#if defined WORST && WORST == 0
   // Worst Fit
   struct _block *worst = NULL;
   while (curr)
   {
      if (curr->free && curr->size >= size)
      {
         if (!worst || curr->size > worst->size)
         {
            worst = curr;
         }
      }
      *last = curr;
      curr = curr->next;
   }
   curr = worst;
#endif

#if defined NEXT && NEXT == 0
   // Next Fit
   if (next_fit_ptr == NULL)
      curr = heapList;
   else
      curr = next_fit_ptr->next;

   while (curr && !(curr->free && curr->size >= size) && curr != next_fit_ptr)
   {
      *last = curr;
      curr = curr->next;
   }

   if (curr == NULL)
   {
      curr = heapList;

      while (curr && curr != next_fit_ptr && !(curr->free && curr->size >= size))
      {
         *last = curr;
         curr = curr->next;
      }
   }
#endif
   next_fit_ptr = curr;
   return curr;
}

/*
 * \brief growheap
 *
 * Given a requested size of memory, use sbrk() to dynamically
 * increase the data segment of the calling process.  Updates
 * the free list with the newly allocated memory.
 *
 * \param last tail of the free _block list
 * \param size size in bytes to request from the OS
 *
 * \return returns the newly allocated _block of NULL if failed
 */
struct _block *growHeap(struct _block *last, size_t size)
{
   /* Request more space from OS */
   struct _block *curr = (struct _block *)sbrk(0);
   struct _block *prev = (struct _block *)sbrk(sizeof(struct _block) + size);

   assert(curr == prev);

   /* OS allocation failed */
   if (curr == (struct _block *)-1)
   {
      return NULL;
   }

   /* Update heapList if not set */
   if (heapList == NULL)
   {
      heapList = curr;
   }

   /* Attach new _block to previous _block */
   if (last)
   {
      last->next = curr;
   }

   /* Update _block metadata:
      Set the size of the new block and initialize the new block to "free".
      Set its next pointer to NULL since it's now the tail of the linked list.
   */
   curr->size = size;
   curr->next = NULL;
   curr->free = false;

   // Increment the grow counter only if the sbrk() call was successful
   num_grows++;
   // Increment the number of blocks counter
   num_blocks++;
   // Update the maximum heap size
   max_heap += size + sizeof(struct _block);

   return curr;
}

/*
 * \brief malloc
 *
 * finds a free _block of heap memory for the calling process.
 * if there is no free _block that satisfies the request then grows the
 * heap and returns a new _block
 *
 * \param size size of the requested memory in bytes
 *
 * \return returns the requested memory allocation to the calling process
 * or NULL if failed
 */

struct _block *splitBlock(struct _block *block, size_t size)
{
   struct _block *newBlock = (struct _block *)((char *)BLOCK_DATA(block) + size);
   newBlock->size = block->size - size - sizeof(struct _block);
   newBlock->next = block->next;
   newBlock->free = true;

   block->size = size;
   block->next = newBlock;

   // Increment the split counter
   num_splits++;
   // Increment the number of blocks counter
   num_blocks++;
   // Update the maximum heap size
   max_heap += sizeof(struct _block);

   return block;
}

struct _block *coalesce(struct _block *block)
{
   // Get the next block in the list
   struct _block *nextBlock = block->next;

   // Check if the next block is a valid block, not the head of the heap list, and is free
   if (nextBlock && nextBlock != heapList && nextBlock->free)
   {
      // Update the current block's size to include the next block's size and the _block metadata size
      block->size += sizeof(struct _block) + nextBlock->size;

      // Update the current block's next pointer to skip the next (coalesced) block
      block->next = nextBlock->next;

      // Increment the coalesces counter
      num_coalesces++;

      // Decrement the number of blocks counter
      num_blocks--;

      // Update the maximum heap size
      max_heap -= sizeof(struct _block);
   }

   // Return the updated block
   return block;
}

void *malloc(size_t size)
{

   if (atexit_registered == 0)
   {
      atexit_registered = 1;
      atexit(printStatistics);
   }

   /* Align to multiple of 4 */
   size = ALIGN4(size);

   /* Handle 0 size */
   if (size == 0)
   {
      return NULL;
   }

   /* Look for free _block.  If a free block isn't found then we need to grow our heap. */

   struct _block *last = heapList;
   struct _block *next = findFreeBlock(&last, size);

   num_requested += size;

   /* TODO: If the block found by findFreeBlock is larger than we need then:
            If the leftover space in the new block is greater than the sizeof(_block)+4 then
            split the block.
            If the leftover space in the new block is less than the sizeof(_block)+4 then
            don't split the block.
   */

   // Check if the found block is larger than needed and can be split
   // Check if the found block is larger than needed and can be split
   if (next != NULL && next->size >= size + sizeof(struct _block) + 4)
   {
      splitBlock(next, size);
   }
   // If the found block is suitable for the requested size, increment the reuse counter
   if (next != NULL)
   {
      num_reuses++;
   }

   /* Could not find free _block, so grow heap */
   if (next == NULL)
   {
      next = growHeap(last, size);
   }

   /* Could not find free _block or grow heap, so just return NULL */
   if (next == NULL)
   {
      return NULL;
   }

   // Mark _block as in use
   next->free = false;

   // Increment the malloc counter
   num_mallocs++;

   // Return data address associated with _block to the user
   return BLOCK_DATA(next);
}

/*
 * \brief free
 *
 * frees the memory _block pointed to by pointer. if the _block is adjacent
 * to another _block then coalesces (combines) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */
void free(void *ptr)
{
   if (!ptr)
   {
      return;
   }

   // Get the block pointer from the user pointer
   struct _block *curr = (struct _block *)(((char *)ptr) - sizeof(struct _block));

   // Set the block as free
   curr->free = 1;
   num_frees++;

   // Coalesce with the previous block if it's free
   struct _block *prev = heapList;
   struct _block *last = NULL;
   while (prev && prev->next != curr)
   {
      last = prev;
      prev = prev->next;
   }

   if (last && last->free)
   {
      last->size += sizeof(struct _block) + curr->size;
      last->next = curr->next;

      // Increment the coalesces counter
      num_coalesces++;
      // Decrement the number of blocks counter
      num_blocks--;
      // Update the next_fit_ptr if it's pointing to a coalesced block
      if (next_fit_ptr && next_fit_ptr->free)
      {
         next_fit_ptr = last;
      }
      // Update the current block pointer to the coalesced block
      curr = last;
   }

   // Coalesce with the next block if it's free
   if (curr->next && curr->next->free)
   {
      curr->size += sizeof(struct _block) + curr->next->size;
      curr->next = curr->next->next;

      // Increment the coalesces counter
      num_coalesces++;
      // Decrement the number of blocks counter
      num_blocks--;
      // Update the next_fit_ptr if it's pointing to a coalesced block
      if (next_fit_ptr && next_fit_ptr->free)
      {
         next_fit_ptr = curr;
      }
   }
}

void *realloc(void *ptr, size_t size)
{
   if (ptr == NULL)
   {
      return malloc(size);
   }

   if (size == 0)
   {
      free(ptr);
      return NULL;
   }

   // Align to multiple of 4
   size = ALIGN4(size);
   num_requested += size;

   struct _block *curr = BLOCK_HEADER(ptr);
   if (curr->size >= size)
   {
      if (curr->size > size + sizeof(struct _block) + 4)
      {
         curr = splitBlock(curr, size);
      }
      return BLOCK_DATA(curr);
   }

   void *new_ptr = malloc(size);
   if (new_ptr)
   {
      memcpy(new_ptr, ptr, curr->size);
      free(ptr);
   }

   return new_ptr;
}
void *calloc(size_t num, size_t size)
{
   // Calculate the total size of memory required
   size_t total_size = num * size;

   // Allocate memory using malloc
   void *ptr = malloc(total_size);

   // If the memory allocation was successful, initialize the memory to zero
   if (ptr)
   {
      memset(ptr, 0, total_size);
   }

   return ptr;
}
