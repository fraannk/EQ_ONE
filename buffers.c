/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: buffers.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: 
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 17. apr. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include <stdlib.h>
#include "emp_type.h"
#include "buffers.h"

/*****************************    Defines    *******************************/

typedef struct{
  INT16U  head;
  INT16U  tail;
  INT8U   *data;
  INT16U  size;
}buffer_t;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
buffer_t buffers[BUFFERS_MAX];

/*****************************   Functions   *******************************/
INT8S buffer_newid()
{
  static INT8U next_id = 0;
  INT8S new_id;

  if( next_id < BUFFERS_MAX )
    new_id = next_id++;
  else
    new_id = -1;

  return( new_id );
}

INT8U buffer_is_empty(INT8U id)
{
  return( buffers[id].head == buffers[id].tail ? 1 : 0 );
}

INT8U buffer_put(INT8U id, INT8U data)
{
  INT8U ret = 1;

  //TODO : check if buffer is full
  buffers[id].data[buffers[id].head] = data;
  buffers[id].head++;

  if(buffers[id].head == buffers[id].size)
    buffers[id].head = 0;

  return(ret);
}

INT8U buffer_get(INT8U id, INT8U *data)
{
  INT8U ret = 0;

  if( !buffer_is_empty(id) )
  {
    *data = buffers[id].data[buffers[id].tail];
    buffers[id].tail++;
    if(buffers[id].tail == buffers[id].size)
      buffers[id].tail = 0;
    ret=1;
  }

  return(ret);
}

INT8S buffer_create(INT16U size)
{
  INT8U id = buffer_newid();

  if(id >= 0)
  {
    INT8U *data = (INT8U*)malloc(size);
    buffers[id].data = data;
    buffers[id].head = 0;
    buffers[id].tail = 0;
    buffers[id].size = size;
  }

  return( id );
}


/****************************** End Of Module *******************************/
