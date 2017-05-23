/*****************************************************************************
 * University of Southern Denmark
 *
 * MODULENAME.: buffers.c
 *
 * PROJECT....: EQ_ONE
 *
 * DESCRIPTION: Module to handle dynamic FIFO buffers
 *
 * Change Log:
 *****************************************************************************
 * Date    Id    Change
 * --------------------
 * 17. apr. 2017  jorn    Module created.
 *
 *****************************************************************************/

/***************************** Include files *******************************/
#include "buffers.h"
#include <stdint.h>
#include <stdlib.h>
#include "emp_type.h"

/*****************************    Defines    *******************************/
typedef struct{
  INT16U  head;
  INT16U  tail;
  INT8U   *data;
  INT16U  size;
}buffer_t;                           // Buffer control block

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
buffer_t buffers[BUFFERS_MAX];      // Buffer array

/*****************************   Functions   *******************************/

INT8S buffer_newid()
/*****************************************************************************
 *   Input    : -
 *   Output   : -
 *   Function : Returns the next available buffer id. Returns -1 if none avail.
 ******************************************************************************/
{
  static INT8U next_id = 0;         // Holds the next available id
  INT8S new_id;

  if( next_id < BUFFERS_MAX )       // Check if next id is within max allowed
    new_id = next_id++;
  else
    new_id = -1;                    //  Set -1 if none available

  return( new_id );
}

INT8U buffer_is_empty(INT8U id)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  return( buffers[id].head == buffers[id].tail ? 1 : 0 );
}

INT8U buffer_put(INT8U id, INT8U data)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  INT8U ret = 1;

  buffers[id].data[buffers[id].head] = data;    // puts data at head
  buffers[id].head++;

  if(buffers[id].head == buffers[id].size)      // check for overrun in size
    buffers[id].head = 0;

  return(ret);
}

INT8U buffer_get(INT8U id, INT8U *data)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  INT8U ret = 0;

  if( !buffer_is_empty(id) )
  {
    *data = buffers[id].data[buffers[id].tail];   // get data at tail
    buffers[id].tail++;
    if(buffers[id].tail == buffers[id].size)      // checl for overrun in size
      buffers[id].tail = 0;
    ret=1;
  }

  return(ret);
}

INT8S buffer_create(INT16U size)
/*****************************************************************************
 *   Header description
 ******************************************************************************/
{
  INT8U id = buffer_newid();

  if(id >= 0)
  {
    INT8U *data = (INT8U*)malloc(size);     // allocate mem for buffer
    buffers[id].data = data;                // init bcb
    buffers[id].head = 0;
    buffers[id].tail = 0;
    buffers[id].size = size;
  }

  return( id );                             // return handle
}


/****************************** End Of Module *******************************/
