/******************************************************************************
* @file:cbfifo.h
*
* @brief: This files consists of the function headers used in the cbfifo file
*
* @author: Rishab Shah
* @date:  12-Mar-2021
* @references: Making Embedded Systems - White
*******************************************************************************/
#ifndef _CBFIFO_H_
#define _CBFIFO_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include <stdlib.h>  // for size_t
#include <stdint.h>
#include <stdio.h>
#include "msp432p401r.h"
/*******************************************************************************
Macros
*******************************************************************************/
#define CBFIFO_SIZE                     (256)
/*******************************************************************************
Structures
*******************************************************************************/
typedef struct sCircularBuffer
{
    uint8_t cbBuf[CBFIFO_SIZE];       /* Circular buffer in place */
    uint16_t size;                    /* Size in power of 2 */
    uint16_t read;                    /* Read position 0 to size-1 */
    uint16_t write;                   /* Write position 0 to size-1 */
    int16_t tracker;                  /* To track the position */
}CB_t;

///typedef struct sCircularBuffer CB_t;
/*******************************************************************************
Function declarations
*******************************************************************************/
/*
 * Enqueues data onto the FIFO, up to the limit of the available FIFO
 * capacity.
 *
 * Parameters:
 *   buf      Pointer to the data
 *   nbyte    Max number of bytes to enqueue
 *   cbfifo   Queue under consideration
 * Returns:
 *   The number of bytes actually enqueued, which could be 0. In case
 * of an error, returns -1.
 */
size_t cbfifo_enqueue(CB_t *cbfifo,void *buf, size_t nbyte);


/*
 * Attempts to remove ("dequeue") up to nbyte bytes of data from the
 * FIFO. Removed data will be copied into the buffer pointed to by buf.
 *
 * Parameters:
 *   buf      Destination for the dequeued data
 *   nbyte    Bytes of data requested
 *   cbfifo   Queue under consideration
 *
 * Returns:
 *   The number of bytes actually copied, which will be between 0 and
 * nbyte.
 *
 * To further explain the behavior: If the FIFO's current length is 24
 * bytes, and the caller requests 30 bytes, cbfifo_dequeue should
 * return the 24 bytes it has, and the new FIFO length will be 0. If
 * the FIFO is empty (current length is 0 bytes), a request to dequeue
 * any number of bytes will result in a return of 0 from
 * cbfifo_dequeue.
 */
size_t cbfifo_dequeue(CB_t *cbfifo,void *buf, size_t nbyte);


/*
 * Returns the number of bytes currently on the FIFO.
 *
 * Parameters:
 *   cbfifo   Queue under consideration
 *
 * Returns:
 *   Number of bytes currently available to be dequeued from the FIFO
 */
size_t cbfifo_length(CB_t *cbfifo);


/*
 * Returns the FIFO's capacity
 *
 * Parameters:
 *   cbfifo   Queue under consideration
 *
 * Returns:
 *   The capacity, in bytes, for the FIFO
 */
size_t cbfifo_capacity(CB_t *cbfifo);

/*
 * displays the queue status
 *
 * Parameters:
 *   cbfifo   Queue under consideration
 *
 * Returns:
 *   None
 */
void cbfifo_dump_state(CB_t *cbfifo);

/*
 * describes the queue status
 *
 * Parameters:
 *   cbfifo   Queue under consideration
 *
 * Returns:
 *   None
 */
void DiagnosticMessage(CB_t *cbfifo);

/*
 * Returns if the queue is full or not
 *
 * Parameters:
 *   cbfifo   Queue under consideration
 *
 * Returns:
 *   yes/no
 */
int cbfifo_full(CB_t *cbfifo);


/*
 * INitialises the queue
 *
 * Parameters:
 *   cbfifo   Queue under consideration
 *   size to init with (queue suze)
 * Returns:
 *   yes/no
 */
void cbfifo_init(CB_t *cbfifo , size_t size);

#endif // _CBFIFO_H_
