#include "lcd.h"

namespace TheBoy {

	/// <summary>
	/// LCD Class Constructor 
	/// </summary>
	Lcd::Lcd(EmulatorController* ctrl) : emulCtrl(ctrl) {
		regs = LcdRegs();

		regs.lcdc = 0x91;
		regs.scrollX = 0;
		regs.scrollY = 0;
		regs.ly = 0;
		regs.lyComp = 0;
		regs.backPallet = 0xFC;
		regs.objPallets[0] = 0xFF;
		regs.objPallets[1] = 0xFF;
		regs.WX = 0;
		regs.WY = 0;

		for (int i = 0; i < 4; i++) {
			bgColorPallets[i] = defaultColors[i];
			spriteColors1[i] = defaultColors[i];
			spriteColors2[i] = defaultColors[i];
		}
		std::cout << "[LCD] ::: LCD has been created" << std::endl;
	}

	/// <summary>
	/// Lcd Clas Destructor
	/// </summary>
	Lcd::~Lcd() {
		//delete regs;
	}

	/// <summary>
	/// Gets the defined value for a addres on the Lcd
	/// </summary>
	/// <param name="addres">Target address</param>
	/// <returns>Value</returns>
	bit8 Lcd::read(bit16 address) {
		// Since the order on the struct is the same as the defined, using offset
		bit8 offSet = (address - 0xFF40);
		return ((bit8*)&regs)[offSet];
	}

	/// <summary>
	/// Writes to the defined addres value
	/// </summary>
	/// <param name="addres">Target address value</param>
	/// <param name="value">Value</param>
	void Lcd::write(bit16 addres, bit8 value) {
		bit8 offSet = (addres - 0xFF40);
		bit8* p = (bit8*)&regs;
		p[offSet] = value;
		//((bit8*)&regs)[offSet] = value;

		// On DMA Address
		if (offSet == 6) {
			emulCtrl->getDma()->start(value);
		}

		// FF47 - BGP (BG Palette Data) (R/W) - Non CGB Mode Only
		if (addres == 0xFF47) {
			updatePallet(value, 0);
		}
		// FF48 - OBP0 (OBJ Palette 0 Data) (R/W)
		else if (addres == 0xFF48) {
			// Since the last 2 bits should be ignored, transparent
			updatePallet((value & 0b11111100), 1);
		}
		else if (addres == 0xFF49) {
			updatePallet((value & 0b11111100), 2);

		}
	}

	/// <summary>
	/// Gets a pointer to the LCD registors
	/// </summary>
	/// <returns>Pointer to the current registors</returns>
	LcdRegs* Lcd::getLcdRegistors() {
		return &regs;
	}

	/// <summary>
	/// Gets the bit value for the BG and Window enable/priority
	/// </summary>
	/// <returns>BG and Window enable/priority value</returns>
	bit8 Lcd::getLCDCBgwEnable() {
		return static_cast<bit8>(GETBIT(regs.lcdc, 0));
	}

	/// <summary>
	/// Gets the bit value for the OBJ enable
	/// </summary>
	/// <returns>OBJ enable value</returns>
	bit8 Lcd::getLCDCObjEnable() {
		return static_cast<bit8>(GETBIT(regs.lcdc, 1));
	}

	/// <summary>
	/// Gets the obj size value 
	/// </summary>
	/// <returns>OBJ size value</returns>
	bit8 Lcd::getLCDCObjHeight() {
		return GETBIT(regs.lcdc, 2) ? 0x10 : 0x8;
	}

	/// <summary>
	/// Gets the Background map area
	/// </summary>
	/// <returns>BG tile map area value</returns>
	bit16 Lcd::getLCDCBgMapArea() {
		return GETBIT(regs.lcdc, 3) ? 0x9C00 : 0x9800;
	}

	/// <summary>
	/// Gets the background and window tile data area
	/// </summary>
	/// <returns>BG and Window tile data area value</returns>
	bit16 Lcd::getLCDCBgwDataArea() {
		return GETBIT(regs.lcdc, 4) ? 0x8000 : 0x8800;
	}

	/// <summary>
	/// Gets the window enable bit value
	/// </summary>
	/// <returns>Window enable</returns>
	bit8 Lcd::getLCDCWindEnable() {
		return static_cast<bit8>(GETBIT(regs.lcdc, 5));
	}

	/// <summary>
	/// Gets the windows tile map area value
	/// </summary>
	/// <returns>Window tile map area value</returns>
	bit16 Lcd::getLCDCWindMapArea() {
		return GETBIT(regs.lcdc, 6) ? 0x9C00 : 0x9800;
	}

	/// <summary>
	/// Gets if the LCD and PPU are enable
	/// </summary>
	/// <returns>LCD and PPU enable value</returns>
	bit8 Lcd::getLCDCLcdEnable() {
		return static_cast<bit8>(GETBIT(regs.lcdc, 7));
	}

	/// <summary>
	/// Gets the current Lcd Mode
	/// </summary>
	/// <returns>LCDMODE value</returns>
	Lcd::LCDMODE Lcd::getLCDSMode() {
		return static_cast<Lcd::LCDMODE>(regs.lcds & 0b11);
	}

	/// <summary>
	/// Defines the current lcd mode
	/// </summary>
	/// <param name="mode">LCD mode value</param>
	void Lcd::setLCDSMode(Lcd::LCDMODE mode) {
		regs.lcds &= ~0b11;
		regs.lcds |= static_cast<bit8>(mode);
	}

	/// <summary>
	/// Gets the LYC Flag value
	/// </summary>
	/// <returns>LYC Value</returns>
	bit8 Lcd::getLCDSLycFlag() {
		return static_cast<bit8>(GETBIT(regs.lcds, 2));
	}

	/// <summary>
	/// Defines the LYC flag value
	/// </summary>
	/// <param name="value">LYC define value</param>
	void Lcd::setLCDSLycFlag(bit8 value) {
		SETBIT(regs.lcds, 2, value);
	}

	/// <summary>
	/// Get the target stat vlaue
	/// </summary>
	/// <param name="stat">Target Stat value</param>
	/// <returns>Current stat flag value</returns>
	bit8 Lcd::getLCDSStat(Lcd::LCDSSTATS stat) {
		return static_cast<bit8>(regs.lcds & stat);
	}

	/// <summary>
	/// Gets the Lcd vertical Line current value
	/// </summary>
	/// <returns>Vertical line current value</returns>
	bit8 Lcd::getLyValue() {
		return regs.ly;
	}

	/// <summary>
	/// Resets the vertical line counter
	/// </summary>
	void Lcd::resetLyValue() {
		regs.ly = 0;
	}

	/// <summary>
	/// Increment the Vertical line counter
	/// </summary>
	void Lcd::incrementLy() {
		regs.ly++;
	}

	/// <summary>
	/// Gets the current LyCompare value
	/// </summary>
	/// <returns>LyCompare value</returns>
	bit8 Lcd::getLyCompValue() {
		return regs.lyComp;
	}

	/// <summary>
	/// Gets the Lcd Color by index
	/// </summary>
	/// <param name="index">Target index</param>
	/// <returns>Defined index Color</returns>
	bit32 Lcd::getColorByIndex(bit8 index) {
		return defaultColors[index];
	}

	/// <summary>
	/// Updates the target pallet colors based on defined values
	/// </summary>
	/// <param name="palletAddr">Pallet addres element</param>
	/// <param name="val">Target pallet group</param>
	void Lcd::updatePallet(bit8 palletAddr, bit8 val) {
		bit32* pColor = bgColorPallets;
		
		switch (val)
		{
		case 1:
			pColor = spriteColors1;
			break;
		case 2:
			pColor = spriteColors2;
			break;
		}
		// Defined color by the value setted
		pColor[0] = defaultColors[static_cast<int>(palletAddr & 0b11)];
		pColor[1] = defaultColors[static_cast<int>((palletAddr >> 2) & 0b11)];
		pColor[2] = defaultColors[static_cast<int>((palletAddr >> 4) & 0b11)];
		pColor[3] = defaultColors[static_cast<int>((palletAddr >> 6) & 0b11)];
	}

	/// <summary>
	/// Gets a color from the background group by index
	/// </summary>
	/// <param name="index">Target index</param>
	/// <returns>Indexed color</returns>
	bit32 Lcd::getBgColorByIndex(bit8 index) {
		return bgColorPallets[index];
	}

	/// <summary>
	/// Gets the sprite color by id from the first set
	/// </summary>
	/// <param name="id">Target color id</param>
	/// <returns>Defined color</returns>
	bit32 Lcd::getSpriteColorOneById(bit8 id) {
		return spriteColors1[id];
	}

	/// <summary>
	/// Gets the sprite color by id from the second set
	/// </summary>
	/// <param name="id">Target color id</param>
	/// <returns>Defined color</returns>
	bit32 Lcd::getSpriteColorTwoById(bit8 id) {
		return spriteColors2[id];
	}
}