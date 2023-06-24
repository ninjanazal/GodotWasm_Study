#pragma once
#ifndef PIXELPIPELINE_H
#define PIXELPIPELINE_H
#include <common.h>

namespace TheBoy {
	class EmulatorController;
	namespace PixelPipe {

		/// <summary>
		/// Fifo Push function
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <param name="value">Push value</param>
		void FifoPush(EmulatorController* ctrl, bit32 value);


		/// <summary>
		/// FIFO pop instruction
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <returns>Defined value</returns>
		bit32 FifoPop(EmulatorController* ctrl);


		/// <summary>
		/// Pipeline internal switcher, fetch the pipeline state
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineFetch(EmulatorController* ctrl);


		/// <summary>
		/// Pipeline iteration function
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineStep(EmulatorController* ctrl);


		/// <summary>
		/// Push the pixel value to the ppu buffer
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelinePushPixel(EmulatorController* ctrl);


		/// <summary>
		/// Adds a color to the fifo
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <returns>Was able to push to fifo</returns>
		bool PipelineFiFoAdd(EmulatorController* ctrl);


		/// <summary>
		/// Resets the fifo state
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineFiFoReset(EmulatorController* ctrl);


		/// <summary>
		/// Fetch the sprite pixel colors
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <param name="bit">Current Tile bit</param>
		/// <param name="col">Background calculated color</param>
		/// <param name="bgCol">Bg palette calculated id</param>
		/// <returns></returns>
		bit32 PipelineFetchSprite(EmulatorController* ctrl, int bit, bit32 col, bit8 bgCol);


		/// <summary>
		/// Loads the defined sprite tile to memory
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineLoadSpriteTile(EmulatorController* ctrl);


		/// <summary>
		/// Loads the sprite from the data, using offset
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		/// <param name="offset">Offest data value</param>
		void PipelineLoadSpriteData(EmulatorController* ctrl, bit8 offset);


		/// <summary>
		/// Evaluates if the game window is visible on the current draw
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		bool PipelineWindowVisible(EmulatorController* ctrl);


		/// <summary>
		/// Loads the current window tile
		/// </summary>
		/// <param name="ctrl">Target Emulator controller</param>
		void PipelineLoadWindowTile(EmulatorController* ctrl);
	}
}
#endif // !PIXELPIPELINE_H
