#include "ppu.h"
#include <cstring>


namespace TheBoy {
	/**
	 * @brief Construct a new Ppu object
	 * @param ctrl Current emulator controller reference
	 */
	Ppu::Ppu(EmulatorController* ctrl) : emulCtrl(ctrl) {
		std::cout << "[PPU] ::: PPU has been created" << std::endl;

		oam_ram = new OamElement[40]{ 0 };
		vRam = new bit8[0x2000]{ };

		buffer = new bit32[yRes * xRes * sizeof(32)]{ 0 };

		cFrame = 0;
		cLineTicks = 0;

		//memset(oam_ram, 0, sizeof(oam_ram));

		lineSpriteCount = 0;
		fetchedEntryCounter = 0;

		fifo = new FIFO_DATA();
		fifo->init();

		lineSpriteCount = 0;
		fetchedEntryCounter = 0;
		windowL = 0;
	}


	/**
	 * @brief Destroy the Ppu object
	 */
	Ppu::~Ppu() {
		delete[] vRam;
		delete[] oam_ram;
		delete[] buffer;
		delete fifo;
	}

	/// <summary>
	/// Gets a pointer to the PPU FIFO data
	/// </summary>
	/// <returns>FIFO pointer</returns>
	FIFO_DATA* Ppu::getFifo() {
		return fifo;
	}

	/**
	 * @brief Ppu interation
	 */
	void Ppu::step() {
		cLineTicks++;

		switch (emulCtrl->getLcd()->getLCDSMode()) {
		case Lcd::LCDMODE::OAM:
			PpuStates::mode_OAM(emulCtrl);
			break;

		case Lcd::LCDMODE::XFER:
			PpuStates::mode_XFER(emulCtrl);
			break;

		case Lcd::LCDMODE::VBLANK:
			PpuStates::mode_VBLANK(emulCtrl);
			break;

		case Lcd::LCDMODE::HBLANK:
			PpuStates::mode_HBLANK(emulCtrl);
			break;

		default:
			std::cout << "[PPU] ::: Failed step call, unknown state" << std::endl;
			break;
		}
	}


	/**
	 * @brief Writes a value to the OAM ram position
	 * @param addr Target address value
	 * @param val Defined value
	 */
	void Ppu::oamWrite(bit16 addr, bit8 val) {
		// Manage offset buffer position
		if (addr >= 0xFE00) {
			addr -= 0xFE00;
		}
		// For memory location address, castring the arry to a bit8 pointer
		// This will produce a access to the target Oam entry value
		((bit8*)oam_ram)[addr] = val;
	}


	/**
	 * @brief Reads from the OAM Ram on the defined addres
	 * @param addr Target addres
	 * @return bit8 Value on the addres
	 */
	bit8 Ppu::oamRead(bit16 addr) {
		// Manage offset buffer position
		if (addr >= 0xFE00) {
			addr -= 0xFE00;
		}
		// For memory location address, castring the arry to a bit8 pointer
		// This will produce a access to the target Oam entry value
		return ((bit8*)oam_ram)[addr];
	}


	/**
	 * @brief Writes to the vRam on the defined address
	 * @param addr Target address
	 * @param val Defined value
	 */
	void Ppu::write(bit16 addr, bit8 val) {
		// Tile data is stored in VRAM in the memory area at $8000-$97FF;
		vRam[addr - 0x8000] = val;
	}


	/**
	 * @brief Reads from the vRam on the defined address
	 * @param addr Target addres
	 * @return bit8 Value on the address
	 */
	bit8 Ppu::read(bit16 addr) {
		// Tile data is stored in VRAM in the memory area at $8000-$97FF;
		return vRam[addr - 0x8000];
	}

	/// <summary>
	/// Get the current PPu Line tick count
	/// </summary>
	/// <returns>Line Tick count</returns>
	bit32 Ppu::getCurrentLineTicks() {
		return cLineTicks;
	}

	/// <summary>
	/// Resets the current line tick value
	/// </summary>
	void Ppu::resetLineTicks() {
		cLineTicks = 0;
	}

	/// <summary>
	/// Gets the current Ppu Frame coutne
	/// </summary>
	/// <returns>Current Frame count</returns>
	bit32 Ppu::getCurrentFrame() {
		return cFrame;
	}

	/// <summary>
	/// Iterates the current frame value
	/// </summary>
	void Ppu::incrementCurrentFrame() {
		cFrame++;
	}


	/// <summary>
	/// Gets the defined target frame Time
	/// </summary>
	/// <returns>Defined Frame time value</returns>
	bit32 Ppu::getTargetFrameTime() {
		return targetFrameTime;
	}


	/// <summary>
	/// Gets the last frame time value
	/// </summary>
	/// <returns>Last frame time value</returns>
	long Ppu::getPreviousFrameTime() {
		return prevFrameTime;
	}

	/// <summary>
	/// Initial frame group time stamp
	/// </summary>
	/// <returns>Initial frame group time</returns>
	long Ppu::getInitialTimer() {
		return  initTimer;
	}

	/// <summary>
	/// Defines the initial frame group time stamp
	/// </summary>
	/// <param name="val">Target frame group stamp</param>
	void Ppu::setInitialTimer(bit32 val) {
		initTimer = val;
	}

	/// <summary>
	/// Updates the Frame time to the current tick value
	/// </summary>
	void Ppu::updateFrameTime() {
		prevFrameTime = static_cast<long>(emulCtrl->getCpu()->getTicks());
	}

	/// <summary>
	/// Gets the current frameCount value
	/// </summary>
	/// <returns>Frame count value</returns>
	long Ppu::getFrameCount() {
		return frameCounter;
	}

	/// <summary>
	/// Resets the internal frame counter
	/// </summary>
	void Ppu::resetFrameCount() {
		frameCounter = 0;
	}

	/// <summary>
	/// Add a defined amount of frames to the counter
	/// </summary>
	/// <param name="val">Increment amount</param>
	void Ppu::addFrameCount(bit8 val) {
		frameCounter += val;
	}

	/// <summary>
	/// Defines the video buffer value on a defined position
	/// </summary>
	/// <param name="position">Target position</param>
	/// <param name="val">Defined value</param>
	void Ppu::setBufferValue(bit32 position, bit32 val) {
		buffer[position] = val;
	}

	/// <summary>
	/// Gets the pointer to the current ppu buffer
	/// </summary>
	/// <returns>Ppu buffer pointer</returns>
	bit32* Ppu::getPpuBuffer() {
		return buffer;
	}

	/// <summary>
	/// Defines the lineSprite counter
	/// </summary>
	/// <param name="val">New lineSprite counter value</param>
	void Ppu::setLineSpriteCounter(bit8 val) {
		lineSpriteCount = val;
	}

	/// <summary>
	/// Gets the current line sprite counter value
	/// </summary>
	/// <returns>Current line sprite counter</returns>
	bit8 Ppu::getLineSpriteCounter() {
		return lineSpriteCount;
	}

	/// <summary>
	/// Incrementes the line sprite counter and returns
	/// </summary>
	/// <returns>incremented sprite counter</returns>
	bit8 Ppu::incrementAndGetLineCounter() {
		return lineSpriteCount++;
	}

	/// <summary>
	/// Clears the lineElement link value
	/// </summary>
	void Ppu::clearLineSpritePointer() {
		lSprites = NULL;
	}


	/// <summary>
	/// Gets the current lineSprite pointer
	/// </summary>
	/// <returns>Line Sprite pointer</returns>
	OamLineElement* Ppu::getLineSpritePointer() {
		return lSprites;
	}

	/// <summary>
	/// Defines the Line sprite pointer
	/// </summary>
	/// <param name="pointer">New line sprite pointer</param>
	void Ppu::setLineSpritePointer(OamLineElement* pointer) {
		lSprites = pointer;
	}

	/// <summary>
	/// Gets the oam ram entry by id
	/// </summary>
	/// <param name="id">Lookup id</param>
	/// <returns>Element</returns>
	OamElement Ppu::getOamRamElementId(bit8 id) {
		return oam_ram[id];
	}

	/// <summary>
	/// Gets the LineSprite object data by id
	/// </summary>
	/// <param name="id">Target Id</param>
	/// <returns>Pointer to te LineElement data object</returns>
	OamLineElement* Ppu::getLineSpriteById(bit8 id) {
		return &(lSpriteData[id]);
	}


	/// <summary>
	/// Gets the current fetch entry counter value
	/// </summary>
	/// <returns>Fetched Entry counter value</returns>
	bit8 Ppu::getFetchedEntryCounter() {
		return fetchedEntryCounter;
	}

	/// <summary>
	/// Defines the new fetch entry counter value
	/// </summary>
	/// <param name="val">New value</param>
	void Ppu::setFetchedEntryCounter(bit8 val) {
		fetchedEntryCounter = val;
	}


	/// <summary>
	/// Increment and return the current fetched entry counter
	/// </summary>
	/// <returns>Current entry counter value</returns>
	bit8 Ppu::incrementAndGetFetchedCounter() {
		return fetchedEntryCounter++;
	}


	/// <summary>
	/// Gets a fetched entry object by id
	/// </summary>
	/// <param name="id">Target id</param>
	/// <returns>Return Element</returns>
	OamElement Ppu::getFetchedEntryById(bit8 id) {
		return fetchedEntries[id];
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="id"></param>
	/// <param name="elm"></param>
	void Ppu::setFetchedEntryById(bit8 id, OamElement elm) {
		fetchedEntries[id] = elm;
	}

	/// <summary>
	/// Resets the sprite line data array
	/// </summary>
	void Ppu::resetLineData() {
		memset(lSpriteData, 0, sizeof(lSpriteData));
	}

	/// <summary>
	/// Increments the window line counter value
	/// </summary>
	void Ppu::incrementWindowLine() { windowL++; }

	/// <summary>
	/// Resets the window line counter
	/// </summary>
	void Ppu::resetWindowLine() { windowL = 0; }

	/// <summary>
	/// Gets the current window line counter value
	/// </summary>
	/// <returns>Current window line value</returns>
	bit8 Ppu::getWindowLine() { return windowL;  }
} // namespace TheBoy
