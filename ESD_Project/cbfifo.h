/******************************************************************************
* @file: cbfifo.h
*
* @brief: This files consists of the function prototypes and structure
* used in the cbfifo file
*
* @author: Rishab Shah
* @date:  12-Apr-2021
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

/*******************************************************************************
Function declarations
*******************************************************************************/
size_t cbfifo_enqueue(CB_t *cbfifo,void *buf, size_t nbyte);
size_t cbfifo_dequeue(CB_t *cbfifo,void *buf, size_t nbyte);
size_t cbfifo_length(CB_t *cbfifo);
size_t cbfifo_capacity(CB_t *cbfifo);
void cbfifo_dump_state(CB_t *cbfifo);
void DiagnosticMessage(CB_t *cbfifo);
int cbfifo_full(CB_t *cbfifo);
void cbfifo_init(CB_t *cbfifo , size_t size);

#endif // _CBFIFO_H_
