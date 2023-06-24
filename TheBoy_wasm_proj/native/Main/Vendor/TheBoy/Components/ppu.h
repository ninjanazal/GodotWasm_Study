#ifndef PPU_H
#define PPU_H

#include "emulatorController.h"
#include "ppu_states.h"
#include "FIFOData.h"
#include "PixelPipeline.h"

namespace TheBoy {

	/**
	 * @brief Defined struct using the 
	 */
	typedef struct {
		bit8 y;
		bit8 x;
		bit8 tIndex;

		// Defining the flags using bit-field values, 8bit flags
		bit8 paltN_CGB	: 3;
		bit8 vRamBank	: 1;
		bit8 paltN		: 1;
		bit8 xFlip		: 1;
		bit8 yFlip		: 1;
		bit8 bgWind		: 1;
	} OamElement;

	/// <summary>
	/// Defines the OAM line linked list
	/// keep in mind, only 10 sprites can be presented per line
	/// </summary>
	typedef struct OamLineElement {
		OamElement elm;
		OamLineElement* next = NULL;
	}OamLineElement;

	class Ppu {	
		/*
		VRAM Sprite Attribute Table (OAM)
			Sprite attributes reside in the Sprite Attribute Table (OAM - Object Attribute Memory) at $FE00-FE9F.
			Each of the 40 entries consists of four bytes with the following meanings:
			
			Byte 0 - Y Position
				Y = Sprite’s vertical position on the screen + 16
				So for example, Y=0 hides a sprite, Y=2 hides an 8×8 sprite but displays the last two rows of an 8×16 sprite,
				Y=16 displays a sprite at the top of the screen,
				Y=144 displays an 8×16 sprite aligned with the bottom of the screen,
				Y=152 displays an 8×8 sprite aligned with the bottom of the screen,
				Y=154 displays the first six rows of a sprite at the bottom of the screen, Y=160 hides a sprite.

			Byte 1 - X Position
				X = Sprite’s horizontal position on the screen + 8.
				This works similarly to the examples above, except that the width of a sprite is always 8.
				An off-screen value (X=0 or X>=168) hides the sprite, but the sprite still affects the priority ordering,
				thus other sprites with lower priority may be left out due to the ten sprites limit per scan-line.
				A better way to hide a sprite is to set its Y-coordinate off-screen.

			Byte 2 - Tile Index
				In 8x8 mode (LCDC bit 2 = 0), this byte specifies the sprite’s only tile index ($00-$FF).
				This unsigned value selects a tile from the memory area at $8000-$8FFF.
			Byte 3 - Attributes/Flags
				---------------------------------------------------------------------------------
				Bit7   BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
				Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
				Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
				Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
				Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
				Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
		*/

	public:
		/**
		 * @brief Construct a new Ppu object
		 * @param ctrl Current emulator controller reference
		 */
		Ppu(EmulatorController* ctrl);


		/**
		 * @brief Destroy the Ppu object
		 */
		~Ppu();

		/// <summary>
		/// Constant value for the total lines draw per frame
		/// </summary>
		static const int LinePerFrame = 154;


		/// <summary>
		/// Constant value for the total ticks used to draw a line (Docs refer as 456 dots)
		/// </summary>
		static const int TicksPerLine = 456;

		/// <summary>
		/// Vertical lcd resolution
		/// </summary>
		static const int yRes = 144;


		/// <summary>
		/// Horizontal lcd resolution
		/// </summary>
		static const int xRes = 160;


		/// <summary>
		/// Gets a pointer to the PPU FIFO data
		/// </summary>
		/// <returns>FIFO pointer</returns>
		FIFO_DATA* getFifo();

		/**
		 * @brief Ppu interation
		 */
		void step();


		/**
		 * @brief Writes a value to the OAM ram position
		 * @param addr Target address value
		 * @param val Defined value
		 */
		void oamWrite(bit16 addr, bit8 val);


		/**
		 * @brief Reads from the OAM Ram on the defined addres
		 * @param addr Target addres
		 * @return bit8 Value on the addres
		 */
		bit8 oamRead(bit16 addr);


		/**
		 * @brief Writes to the vRam on the defined address
		 * @param addr Target address
		 * @param val Defined value
		 */
		void write(bit16 addr, bit8 val);


		/**
		 * @brief Reads from the vRam on the defined address
		 * @param addr Target addres
		 * @return bit8 Value on the address
		 */
		bit8 read(bit16 addr);


		/// <summary>
		/// Get the current PPu Line tick count
		/// </summary>
		/// <returns>Line Tick count</returns>
		bit32 getCurrentLineTicks();


		/// <summary>
		/// Resets the current line tick value
		/// </summary>
		void resetLineTicks();


		/// <summary>
		/// Gets the current Ppu Frame coutne
		/// </summary>
		/// <returns>Current Frame count</returns>
		bit32 getCurrentFrame();


		/// <summary>
		/// Iterates the current frame value
		/// </summary>
		void incrementCurrentFrame();


		/// <summary>
		/// Gets the defined target frame Time
		/// </summary>
		/// <returns>Defined Frame time value</returns>
		bit32 getTargetFrameTime();


		/// <summary>
		/// Gets the last frame time value
		/// </summary>
		/// <returns>Last frame time value</returns>
		long getPreviousFrameTime();


		/// <summary>
		/// Initial frame group time stamp
		/// </summary>
		/// <returns>Initial frame group time</returns>
		long getInitialTimer();

		/// <summary>
		/// Defines the initial frame group time stamp
		/// </summary>
		/// <param name="val">Target frame group stamp</param>
		void setInitialTimer(bit32 val);


		/// <summary>
		/// Updates the Frame time to the current tick value
		/// </summary>
		void updateFrameTime();

		/// <summary>
		/// Gets the current frameCount value
		/// </summary>
		/// <returns>Frame count value</returns>
		long getFrameCount();
		
		/// <summary>
		/// Resets the internal frame counter
		/// </summary>
		void resetFrameCount();

		/// <summary>
		/// Add a defined amount of frames to the counter
		/// </summary>
		/// <param name="val">Increment amount</param>
		void addFrameCount(bit8 val);


		/// <summary>
		/// Defines the video buffer value on a defined position
		/// </summary>
		/// <param name="position">Target position</param>
		/// <param name="val">Defined value</param>
		void setBufferValue(bit32 position, bit32 val);


		/// <summary>
		/// Gets the pointer to the current ppu buffer
		/// </summary>
		/// <returns>Ppu buffer pointer</returns>
		bit32* getPpuBuffer();


		/// <summary>
		/// Defines the lineSprite counter
		/// </summary>
		/// <param name="val">New lineSprite counter value</param>
		void setLineSpriteCounter(bit8 val);


		/// <summary>
		/// Gets the current line sprite counter value
		/// </summary>
		/// <returns>Current line sprite counter</returns>
		bit8 getLineSpriteCounter();

		/// <summary>
		/// Incrementes the line sprite counter and returns
		/// </summary>
		/// <returns>incremented sprite counter</returns>
		bit8 incrementAndGetLineCounter();


		/// <summary>
		/// Clears the lineElement link value
		/// </summary>
		void clearLineSpritePointer();


		/// <summary>
		/// Gets the current lineSprite pointer
		/// </summary>
		/// <returns>Line Sprite pointer</returns>
		OamLineElement* getLineSpritePointer();


		/// <summary>
		/// Defines the Line sprite pointer
		/// </summary>
		/// <param name="pointer">New line sprite pointer</param>
		void setLineSpritePointer(OamLineElement* pointer);


		/// <summary>
		/// Gets the oam ram entry by id
		/// </summary>
		/// <param name="id">Lookup id</param>
		/// <returns>Element</returns>
		OamElement getOamRamElementId(bit8 id);


		/// <summary>
		/// Gets the LineSprite object data by id
		/// </summary>
		/// <param name="id">Target Id</param>
		/// <returns>Pointer to te LineElement data object</returns>
		OamLineElement* getLineSpriteById(bit8 id);


		/// <summary>
		/// Gets the current fetch entry counter value
		/// </summary>
		/// <returns>Fetched Entry counter value</returns>
		bit8 getFetchedEntryCounter();


		/// <summary>
		/// Defines the new fetch entry counter value
		/// </summary>
		/// <param name="val">New value</param>
		void setFetchedEntryCounter(bit8 val);

		/// <summary>
		/// Increment and return the current fetched entry counter
		/// </summary>
		/// <returns>Current entry counter value</returns>
		bit8 incrementAndGetFetchedCounter();


		/// <summary>
		/// Gets a fetched entry object by id
		/// </summary>
		/// <param name="id">Target id</param>
		/// <returns>Return Element</returns>
		OamElement getFetchedEntryById(bit8 id);


		/// <summary>
		/// Defines the fetched Entry id slot to the element
		/// </summary>
		/// <param name="id">Target id slot</param>
		/// <param name="elm">Target element</param>
		void setFetchedEntryById(bit8 id, OamElement elm);


		/// <summary>
		/// Resets the sprite line data array
		/// </summary>
		void resetLineData();


		/// <summary>
		/// Increments the window line counter value
		/// </summary>
		void incrementWindowLine();


		/// <summary>
		/// Resets the window line counter
		/// </summary>
		void resetWindowLine();


		/// <summary>
		/// Gets the current window line counter value
		/// </summary>
		/// <returns>Current window line value</returns>
		bit8 getWindowLine();

	private:
		/**
		 * @brief Pointer to the target emulator controller
		 */
		EmulatorController* emulCtrl;


		/**
		 * @brief Pointer to the available 40 OAM ram entries
		 */
		OamElement* oam_ram;


		/// <summary>
		/// Holds the sprite line counter
		/// </summary>
		bit8 lineSpriteCount;


		/// <summary>
		/// Pointer to the Entry OamLine Element
		/// </summary>
		OamLineElement* lSprites = NULL;


		/// <summary>
		/// 
		/// </summary>
		OamLineElement lSpriteData[10];


		/// <summary>
		/// Fetch entry counter
		/// </summary>
		bit8 fetchedEntryCounter;


		/// <summary>
		/// Fetched entries holder
		/// </summary>
		OamElement fetchedEntries[3];


		/// <summary>
		/// Ppu Fifo data
		/// </summary>
		FIFO_DATA* fifo;


		/**
		 * @brief Memory allocation for the video Ram
		 */
		bit8* vRam;


		/// <summary>
		/// Holds current frame information
		/// </summary>
		bit32 cFrame;


		/// <summary>
		/// Current line tick counter
		/// </summary>
		bit32 cLineTicks;


		/// <summary>
		/// Output prixel buffer
		/// </summary>
		bit32* buffer;


		/// <summary>
		/// Target Frame time, default is the 60 frames per second in ms
		/// Looking for a 1000 ms total divided by the frame target
		/// </summary>
		bit32 targetFrameTime = 1000 / 60;

		/// <summary>
		/// Current window line draw
		/// </summary>
		bit8 windowL;


		/// <summary>
		/// Holds the previous frame time stamp
		/// </summary>
		long prevFrameTime = 0;


		/// <summary>
		/// Holds the initial frame time stamp
		/// </summary>
		long initTimer = 0;


		/// <summary>
		/// Holds the total frames on the current second
		/// </summary>
		long frameCounter = 0;
	};
} // namespace TheBoy
#endif