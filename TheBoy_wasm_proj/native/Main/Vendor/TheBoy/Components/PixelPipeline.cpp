#include "PixelPipeline.h"
#include "emulatorController.h"

namespace TheBoy {
	namespace PixelPipe {

		/// <summary>
		/// Fifo Push function declaration
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <param name="value">Push value</param>
		void FifoPush(EmulatorController* ctrl, bit32 value) {
			FIFO_Value* next = new FIFO_Value();
			next->next = NULL;
			next->color = value;

			// Initial entry, set the head and tail to the created next
			if (ctrl->getPpu()->getFifo()->pixelFifo.head == NULL) {
				ctrl->getPpu()->getFifo()->pixelFifo.head = ctrl->getPpu()->getFifo()->pixelFifo.tail = next;
			}
			else {
				// If not defined the current tail next and the created next and define it as the new tail
				ctrl->getPpu()->getFifo()->pixelFifo.tail->next = next;
				ctrl->getPpu()->getFifo()->pixelFifo.tail = next;
			}

			ctrl->getPpu()->getFifo()->pixelFifo.size++;
		}

		/// <summary>
		/// FIFO pop instruction
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <returns>Defined value</returns>
		bit32 FifoPop(EmulatorController* ctrl) {
			if (ctrl->getPpu()->getFifo()->pixelFifo.size <= 0) {
				printf("[PIXEL PIPE] ::: ->  ERROR POPING FIFO VALUE! Wrong size!\n");
				fflush(stdout);
			}

			FIFO_Value* popHead = ctrl->getPpu()->getFifo()->pixelFifo.head;
			ctrl->getPpu()->getFifo()->pixelFifo.head = popHead->next;
			ctrl->getPpu()->getFifo()->pixelFifo.size--;

			bit32 val = popHead->color;
			delete popHead;

			return val;
		}

		/// <summary>
		/// Pipeline internal switcher, fetch the pipeline state
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineFetch(EmulatorController* ctrl) {
			switch (ctrl->getPpu()->getFifo()->currState) {
				/*
				This step determines which background/window tile to fetch pixels from.
				By default the tilemap used is the one at $9800 but certain conditions can change that.
				*/
			case FIFOSTATE::FF_TILE: {
				ctrl->getPpu()->setFetchedEntryCounter(0);

				if (ctrl->getLcd()->getLCDCBgwEnable())
				{
					// Loading 8 pixels per iteration
					ctrl->getPpu()->getFifo()->bg_fetched[0] = ctrl->getCpu()->requestBusRead(
						ctrl->getLcd()->getLCDCBgMapArea() +
						(ctrl->getPpu()->getFifo()->mapX / 8) +
						((ctrl->getPpu()->getFifo()->mapY / 8) * 32)
					);


					if (ctrl->getLcd()->getLCDCBgwDataArea() == 0x8800) {
						ctrl->getPpu()->getFifo()->bg_fetched[0] += 128;
					}

					PipelineLoadWindowTile(ctrl);
				}

				if (ctrl->getLcd()->getLCDCObjEnable() && ctrl->getPpu()->getLineSpritePointer() != NULL) {
					PipelineLoadSpriteTile(ctrl);
				}

				ctrl->getPpu()->getFifo()->currState = FIFOSTATE::FF_DATA_LOW;
				ctrl->getPpu()->getFifo()->fetchedX += 8;

			} break;

				/*
				*	Check LCDC.4 for which tilemap to use.
				*	At this step CGB also needs to check which VRAM bank to use and check if the
				*	tile is flipped vertically. Once the tilemap, VRAM and vertical flip is calculated
				*	the tile data is retrieved from VRAM. However, if the PPU’s access to VRAM is blocked
				*	then the tile data is read as $FF.
				*/
			case FIFOSTATE::FF_DATA_LOW: {
				ctrl->getPpu()->getFifo()->bg_fetched[1] =
					ctrl->getCpu()->requestBusRead(ctrl->getLcd()->getLCDCBgwDataArea() +
						(ctrl->getPpu()->getFifo()->bg_fetched[0] * 16) + ctrl->getPpu()->getFifo()->tileY
					);

				PipelineLoadSpriteData(ctrl, 0);
				ctrl->getPpu()->getFifo()->currState = FIFOSTATE::FF_DATA_HIGH;

			} break;

				/*
				Same as Get Tile Data Low except the tile address is incremented by 1.
				The tile data retrieved in this step will be used in the push steps.
				*/
			case FIFOSTATE::FF_DATA_HIGH: {
				ctrl->getPpu()->getFifo()->bg_fetched[2] =
					ctrl->getCpu()->requestBusRead(ctrl->getLcd()->getLCDCBgwDataArea() +
						(ctrl->getPpu()->getFifo()->bg_fetched[0] * 16) + ctrl->getPpu()->getFifo()->tileY + 1
					);

				PipelineLoadSpriteData(ctrl, 1);
				ctrl->getPpu()->getFifo()->currState = FIFOSTATE::FF_SLEEP;

			}break;

				/*
				Do nothing.
				*/
			case FIFOSTATE::FF_SLEEP:
				ctrl->getPpu()->getFifo()->currState = FIFOSTATE::FF_PUSH;
				break;

				/*
				Pushes a row of background/window pixels to the FIFO. Since tiles are 8 pixels wide,
				a “row” of pixels is 8 pixels from the tile to be rendered based on the X and Y coordinates
				calculated in the previous steps.
				Pixels are only pushed to the background FIFO if it’s empty.
				*/
			case FIFOSTATE::FF_PUSH: {
				if (PipelineFiFoAdd(ctrl)) {
					ctrl->getPpu()->getFifo()->currState = FIFOSTATE::FF_TILE;
				}

			}break;
			}
		}

		/// <summary>
		/// Push the pixel value to the ppu buffer
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelinePushPixel(EmulatorController* ctrl) {
			if (ctrl->getPpu()->getFifo()->pixelFifo.size > 8) {
				bit32 pData = FifoPop(ctrl);

				if (ctrl->getPpu()->getFifo()->lineX >= ctrl->getLcd()->getLcdRegistors()->scrollX % 8) {
					ctrl->getPpu()->setBufferValue(
						ctrl->getPpu()->getFifo()->pushedX + (ctrl->getLcd()->getLyValue() * Ppu::xRes),
						pData
					);

					ctrl->getPpu()->getFifo()->pushedX++;
				}
				ctrl->getPpu()->getFifo()->lineX++;
			}
		}

		/// <summary>
		/// Pipeline iteration function
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineStep(EmulatorController* ctrl) {
			ctrl->getPpu()->getFifo()->mapY =
				(ctrl->getLcd()->getLyValue() + ctrl->getLcd()->getLcdRegistors()->scrollY);
			ctrl->getPpu()->getFifo()->mapX =
				(ctrl->getPpu()->getFifo()->fetchedX + ctrl->getLcd()->getLcdRegistors()->scrollX);

			ctrl->getPpu()->getFifo()->tileY =
				((ctrl->getLcd()->getLyValue() + ctrl->getLcd()->getLcdRegistors()->scrollY) % 8) * 2;

			// During a even line step
			if (!(ctrl->getPpu()->getCurrentLineTicks() & 1)) {
				PipelineFetch(ctrl);
			}
			PipelinePushPixel(ctrl);
		}

		/// <summary>
		/// Adds a color to the fifo
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <returns>Was able to push to fifo</returns>
		bool PipelineFiFoAdd(EmulatorController* ctrl) {
			if (ctrl->getPpu()->getFifo()->pixelFifo.size > 8) {
				// Fifo is completed
				return false;
			}

			int x = ctrl->getPpu()->getFifo()->fetchedX -
				(8 - (ctrl->getLcd()->getLcdRegistors()->scrollX % 8));

			for (int i = 0; i < 8; i++)
			{
				int bit = 7 - i;
				bit8 lo = !!(ctrl->getPpu()->getFifo()->bg_fetched[1] & (1 << bit));
				bit8 hi = !!(ctrl->getPpu()->getFifo()->bg_fetched[2] & (1 << bit)) << 1;

				bit32 col = ctrl->getLcd()->getColorByIndex(static_cast<bit8>(lo | hi));

				// Background not enabled
				if (!ctrl->getLcd()->getLCDCBgwEnable()) {
					col = ctrl->getLcd()->getBgColorByIndex(0);
				}

				// Sprites enabled
				if (ctrl->getLcd()->getLCDCObjEnable()) {
					// fetch the sprite prixels
					col = PipelineFetchSprite(ctrl, bit, col, lo | hi);
				}

				if (x >= 0) {
					FifoPush(ctrl, col);
					ctrl->getPpu()->getFifo()->fifoX++;
				}
			}
			return true;
		}

		/// <summary>
		/// Resets the fifo state
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineFiFoReset(EmulatorController* ctrl) {
			while (ctrl->getPpu()->getFifo()->pixelFifo.size)
			{
				FifoPop(ctrl);
			}
			ctrl->getPpu()->getFifo()->pixelFifo.head = NULL;
		}

		/// <summary>
		/// Fetch the sprite pixel colors
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <param name="bit">Current Tile bit</param>
		/// <param name="col">Background calculated color</param>
		/// <param name="bgCol">Bg palette calculated id</param>
		/// <returns></returns>
		bit32 PipelineFetchSprite(EmulatorController* ctrl, int bit, bit32 col, bit8 bgCol) {
			for (int i = 0; i < ctrl->getPpu()->getFetchedEntryCounter(); i++) {
				int spX = (ctrl->getPpu()->getFetchedEntryById(i).x - 8) +
					(ctrl->getLcd()->getLcdRegistors()->scrollX % 8);

				if (spX + 8 < ctrl->getPpu()->getFifo()->fifoX) {
					// Pixel point passed
					continue;
				}

				int offset = ctrl->getPpu()->getFifo()->fifoX - spX;
				//Out of bounds
				if (offset < 0 || offset > 7) {
					continue;
				}
				// if sprite is flipped
				bit = ctrl->getPpu()->getFetchedEntryById(i).xFlip ? offset : (7 - offset);

				bit8 lo = static_cast<bool>(ctrl->getPpu()->getFifo()->fetch_data[i * 2] & (1 << bit));
				bit8 hi = static_cast<bool>((ctrl->getPpu()->getFifo()->fetch_data[(i * 2) + 1] & (1 << bit))) << 1;

				bool bgPriority = ctrl->getPpu()->getFetchedEntryById(i).bgWind;

				// If the pixel is transparent
				if (!(lo | hi)) {
					continue;
				}

				if (!bgPriority || bgCol == 0) {
					col = ctrl->getPpu()->getFetchedEntryById(i).paltN ?
						ctrl->getLcd()->getSpriteColorTwoById(static_cast<bit8>(lo | hi)) :
						ctrl->getLcd()->getSpriteColorOneById(static_cast<bit8>(lo | hi));

					if (lo | hi) {
						break;
					}
				}
			}
			return col;
		}

		/// <summary>
		/// Loads the defined sprite tile to memory
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineLoadSpriteTile(EmulatorController* ctrl) {
			OamLineElement* lineS = ctrl->getPpu()->getLineSpritePointer();

			while (lineS != NULL)
			{
				int spX = (lineS->elm.x - 8) + (ctrl->getLcd()->getLcdRegistors()->scrollX % 8);

				if ((spX >= ctrl->getPpu()->getFifo()->fetchedX && spX < ctrl->getPpu()->getFifo()->fetchedX + 8) ||
					((spX + 8) >= ctrl->getPpu()->getFifo()->fetchedX && (spX + 8) < ctrl->getPpu()->getFifo()->fetchedX + 8))
				{
					ctrl->getPpu()->setFetchedEntryById(ctrl->getPpu()->incrementAndGetFetchedCounter(), lineS->elm);
				}

				lineS = lineS->next;

				// Ended linked list or sprite limit reached
				if (lineS == NULL || ctrl->getPpu()->getFetchedEntryCounter() >= 3) {
					break;
				}
			}
		}


		/// <summary>
		/// Loads the sprite from the data, using offset
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <param name="offset">Offest data value</param>
		void PipelineLoadSpriteData(EmulatorController* ctrl, bit8 offset) {
			int currLy = ctrl->getLcd()->getLyValue();
			bit8 spriteH = ctrl->getLcd()->getLCDCObjHeight();

			for (int i = 0; i < ctrl->getPpu()->getFetchedEntryCounter(); i++) {
				bit8 tileY = ((currLy + 16) - ctrl->getPpu()->getFetchedEntryById(i).y) * 2;

				if (ctrl->getPpu()->getFetchedEntryById(i).yFlip) {
					// flipped vertical
					tileY = ((spriteH * 2) - 2) - tileY;
				}

				bit8 tileId = ctrl->getPpu()->getFetchedEntryById(i).tIndex;

				// Removing last bit if size is 16 ???
				if (spriteH == 16) {
					tileId &= ~(1);
				}

				ctrl->getPpu()->getFifo()->fetch_data[(i * 2) + offset] =
					ctrl->getCpu()->requestBusRead(0x8000 + (tileId * 16) + tileY + offset);
			}
		}

		/// <summary>
		/// Evaluates if the game window is visible on the current draw
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		bool PipelineWindowVisible(EmulatorController* ctrl) {
			// Check if the window is on the visible screen
			return (ctrl->getLcd()->getLCDCWindEnable() &&
				ctrl->getLcd()->getLcdRegistors()->WX >= 0 &&
				ctrl->getLcd()->getLcdRegistors()->WX <= 166 &&
				ctrl->getLcd()->getLcdRegistors()->WY >= 0 &&
				ctrl->getLcd()->getLcdRegistors()->WY < Ppu::yRes);
		}


		/// <summary>
		/// Loads the current window tile
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineLoadWindowTile(EmulatorController* ctrl) {
			if (!PipelineWindowVisible(ctrl)) { return; }

			bit8 wY = ctrl->getLcd()->getLcdRegistors()->WY;

			if (ctrl->getPpu()->getFifo()->fetchedX + 7 >= ctrl->getLcd()->getLcdRegistors()->WX &&
				ctrl->getPpu()->getFifo()->fetchedX + 7 < ctrl->getLcd()->getLcdRegistors()->WX + Ppu::yRes + 14)
			{
				if (ctrl->getLcd()->getLyValue() >= wY && ctrl->getLcd()->getLyValue() < (wY + Ppu::xRes))
				{
					bit8 wTileY = ctrl->getPpu()->getWindowLine() / 8;
					ctrl->getPpu()->getFifo()->bg_fetched[0] = ctrl->getCpu()->requestBusRead(
						ctrl->getLcd()->getLCDCWindMapArea() +
						((ctrl->getPpu()->getFifo()->fetchedX + 7 - ctrl->getLcd()->getLcdRegistors()->WX) / 8) +
						(wTileY * 32)
					);


					if (ctrl->getLcd()->getLCDCBgwDataArea() == 0x8800)
					{ 
						ctrl->getPpu()->getFifo()->bg_fetched[0] += 128;
					}
				}
			}
		}
	}
}