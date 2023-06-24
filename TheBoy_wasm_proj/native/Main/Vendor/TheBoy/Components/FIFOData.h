#pragma once
#ifndef FIFO_DATA_H
#define FIFO_DATA_H
#include "common.h"

namespace TheBoy {
	/*
FIFO Pixel Fetcher
The fetcher fetches a row of 8 background or window pixels and queues them up to be mixed with sprite pixels.
The pixel fetcher has 5 steps. The first four steps take 2 dots each and the fifth step is attempted every dot until it succeeds.
The order of the steps are as follows:

	Get tile
	Get tile data low
	Get tile data high
	Sleep
	Push
*/

/// <summary>
/// Enumerates the FIFO pixel fetcher state
/// </summary>
	typedef enum {
		FF_TILE,
		FF_DATA_LOW,
		FF_DATA_HIGH,
		FF_SLEEP,
		FF_PUSH
	} FIFOSTATE;


	/// <summary>
	/// Linked element representing the fetch information
	/// </summary>
	typedef struct FIFO_Value {
		FIFO_Value* next = NULL;
		bit32 color;
	} FIFO_Value;


	/// <summary>
	/// FIFO Structure
	/// </summary>
	typedef struct PIXELFIFO{
		FIFO_Value* head;
		FIFO_Value* tail;
		bit32 size;
	} PIXELFIFO;


	/// <summary>
	/// Pixel Fifo data struct
	/// </summary>
	typedef struct FIFO_DATA {
		FIFOSTATE currState;
		PIXELFIFO pixelFifo;
		bit8 lineX;
		bit8 pushedX;
		bit8 fetchedX;
		bit8 bg_fetched[3];
		bit8 fetch_data[6];
		bit8 mapY;
		bit8 mapX;
		bit8 tileY;
		bit8 fifoX;

		/// <summary>
		/// Defines the initial state
		/// </summary>
		void init() {
			lineX = 0;
			pushedX = 0;
			fetchedX = 0;
			pixelFifo.size = 0;
			pixelFifo.head = pixelFifo.tail = NULL;
			currState = FIFOSTATE::FF_TILE;
		}
	} FIFO_DATA;
}
#endif
