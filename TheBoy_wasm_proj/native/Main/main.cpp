#include "main.h"
// #include <stdio.h>
#include <OS.hpp>
#include <SceneTree.hpp>

#include "ppu.h"

namespace Concept1 {

Main::Main() {
    _emulCtrl = new EmulatorController();
    viewInput = new InputStates();
}
Main::~Main() {
    delete _emulCtrl;
    delete _outTextureRect;
    delete _vRamTextureRect;
    delete _outTexture;
    delete _outVRamTexture;
    delete _outImage;
    delete viewInput;
}
void Main::_init() {}

void Main::_register_methods() {
    register_method("_ready", &Main::_ready);
    register_method("_process", &Main::_process);
    register_method("_input", &Main::_input);
    register_method("_exit_tree", &Main::_exit_tree);
    register_method("OnFileSelected", &Main::OnFileSelected);
    register_method("CpuStep", &Main::CpuStep);
}
void Main::_ready() {
    _cpuThread = Thread::_new();
    OnReadyGets();
    _selectBtn->connect("pressed", _fileDialog, "popup_centered");
    _fileDialog->connect("file_selected", this, "OnFileSelected");
    GenerateTextures();
    _textOut->set_text(" - - - - - - - - - -");
    std::cout << _outVRamImage->get_size().width << std::endl;
}
void Main::_process(float delta) {
    _textOut->set_text(
        "|TheBoy - Game boy Emulator\n - - - - - - - - - - -\n|:: Cartrige "
        "Information");
    if (_running) {
        if (!OS::get_singleton()->can_use_threads()) {
            _emulCtrl->cpuStep(this);
        }
        _emulCtrl->iteration(this);
        BuildOutputString();
        BuildDebugView();
        BuildOutputView();
    }
}
void Main::_exit_tree() {
    _running = false;
    if (OS::get_singleton()->can_use_threads() && _cpuThread->is_alive()) {
        _cpuThread->wait_to_finish();
    }
}

void Main::_input(InputEvent *event) {
    if (event->is_action("A_press")) {
        viewInput->a = event->is_pressed();
        get_tree()->set_input_as_handled();
    } else if (event->is_action("B_press")) {
        viewInput->b = event->is_pressed();
    } else if (event->is_action("Start_press")) {
        viewInput->start = event->is_pressed();
    } else if (event->is_action("Select_press")) {
        viewInput->select = event->is_pressed();
    } else if (event->is_action("Up_press")) {
        viewInput->up = event->is_pressed();
    } else if (event->is_action("Down_press")) {
        viewInput->down = event->is_pressed();
    } else if (event->is_action("Left_press")) {
        viewInput->left = event->is_pressed();
    } else if (event->is_action("Right_press")) {
        viewInput->right = event->is_pressed();
    }
}

void Main::OnReadyGets() {
    _selectBtn = get_node<Button>(
        "CanvasLayer/topBar/HBoxContainer/pathSelection/selectButton");
    _romPath = get_node<Label>(
        "CanvasLayer/topBar/HBoxContainer/pathSelection/currentPath");
    _fileDialog = get_node<FileDialog>("CanvasLayer/FileDialog");

    _textOut = get_node<RichTextLabel>("body/RichTextLabel");
    _outTextureRect = get_node<TextureRect>("body/outputView");
    _vRamTextureRect = get_node<TextureRect>("body/vramView");
}
void Main::OnFileSelected(String newText) {
    _selectBtn->set_disabled(true);
    _running = false;

    if (OS::get_singleton()->can_use_threads() && _cpuThread->is_alive()) {
        _cpuThread->wait_to_finish();
    }
    _romPath->set_text(newText);
    _emulCtrl->Start(newText.alloc_c_string());
    _running = true;
    if (OS::get_singleton()->can_use_threads()) {
        _cpuThread->start(this, "CpuStep");
    }
}

void Main::GenerateTextures() {
    _outTexture = ImageTexture::_new();
    _outVRamTexture = ImageTexture::_new();

    _outImage = Image::_new();
    _outVRamImage = Image::_new();

    _outImage->create(TheBoy::Ppu::xRes, TheBoy::Ppu::yRes, false,
                      Image::FORMAT_RGB8);
    _outVRamImage->create(static_cast<int64_t>(tileSizeView.x * 8),
                          static_cast<int64_t>(tileSizeView.y * 8), false,
                          Image::FORMAT_RGB8);

    _outTexture->create_from_image(_outImage, 0x0);
    _outVRamTexture->create_from_image(_outVRamImage, 0x0);

    _outTextureRect->set_texture(_outTexture);
    _vRamTextureRect->set_texture(_outVRamTexture);
}

void Main::BuildOutputString() {
    char *regBuffer(new char[256]{});
    char *opBuffer(new char[64]{});
    char *msgBuffer(new char[64]{});
    snprintf(msgBuffer, 64, "-> Ppu Frames: %d",
              _emulCtrl->ppuFrame);

    _emulCtrl->getCpu()->getCpuSummary(regBuffer, opBuffer);
    _textOut->add_text(regBuffer);
    _textOut->add_text("\n");
    _textOut->add_text(opBuffer);
    _textOut->add_text("\n");
    _textOut->add_text(msgBuffer);

    delete[] regBuffer;
    delete[] opBuffer;
    delete[] msgBuffer;
}

void Main::BuildDebugView() {
    /*
    Tile data is stored in VRAM in the memory area at $8000-$97FF;
    with each tile taking 16 bytes, this area defines data for 384 tiles
    Each tile has 8x8 pixels and has a color depth of 4 colors/gray shades
    384 = 16 * 24 disposal
*/
    bit16 addr = 0x8000;
    // Making the display table
    // iGRam->create(tileSizeView.x * 8, tileSizeView.y * 8, sf::Color::White);
    Ref<Image> outRamText = _outVRamTexture->get_data();
    outRamText->lock();
    for (unsigned int y = 0; y < tileSizeView.y; y++) {
        for (unsigned int x = 0; x < tileSizeView.x; x++) {
            addTileToDebug(addr, ((y * 24) + x), outRamText);
        }
    }
    outRamText->unlock();
    _outVRamTexture->create_from_image(outRamText, 0x0);
    _vRamTextureRect->set_texture(_outVRamTexture);
}
void Main::addTileToDebug(bit16 addr, int tileId, Ref<Image> img) {
    /*
                    with each tile taking 16 bytes
                    Each tile occupies 16 bytes, where each line is represented
       by 2 bytes For each line, the first byte specifies the least significant
       bit of the color ID of each pixel, and the second byte specifies the most
       significant bit. In both bytes, bit 7 represents the leftmost pixel, and
       bit 0 the rightmost.
            */
    int yTileOff = static_cast<int>(floor(tileId / tileSizeView.x));
    int xTileOff = tileId - (yTileOff * tileSizeView.x);

    for (int t = 0; t < 16; t += 2) {
        // For each tile line, needs to be readed 2 bytes
        // High and Low tile line value
        bit8 byte1 = _emulCtrl->getBus()->abRead(addr + (tileId * 16) + t);
        bit8 byte2 = _emulCtrl->getBus()->abRead(addr + (tileId * 16) + t + 1);

        // for each bit on the gathered line, the low nib value is the most
        // significat one and vice versa Place the target bit value from the 1st
        // byte on the left of the target bit from the 2st byte
        for (int b = 0; b < 8; b++) {
            bit8 pixelID = (GETBIT(byte1, b) << 1) | GETBIT(byte2, b);
            // Pixel ID makes the target pixel color value, for the gb is 1 of 4
            // colors Since each pixel needs a 4 value entrance 64 pixels per
            // tile
            img->set_pixel((8 * xTileOff) + (7 - b),
                           (8 * yTileOff) + static_cast<int>(floor(t / 2)),
                           gbPallet[pixelID]);
        }
    }
}

void Main::BuildOutputView() {
    Ref<Image> img = _outTexture->get_data();
    img->lock();
    for (int row = 0; row < Ppu::yRes; row++) {
        for (int line = 0; line < Ppu::xRes; line++) {
            img->set_pixel(
                line, row,
                Color().hex(_emulCtrl->getPpu()
                                ->getPpuBuffer()[line + (Ppu::xRes * row)]));
        }
    }
    img->unlock();
    _outTexture->create_from_image(img, 0x0);
    _outTextureRect->set_texture(_outTexture);
}

void Main::CpuStep() {
    while (_running) _emulCtrl->cpuStep(this);
}
/// <summary>
/// Get the registed input
/// </summary>
/// <returns>Registed input values</returns>
InputStates *Main::getInputState() { return viewInput; }
}  // namespace Concept1