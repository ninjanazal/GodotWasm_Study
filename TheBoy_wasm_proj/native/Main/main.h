#ifndef MAIN_H
#define MAIN_H

#include <Button.hpp>
#include <FileDialog.hpp>
#include <Godot.hpp>
#include <Variant.hpp>
#include <Image.hpp>
#include <ImageTexture.hpp>
#include <InputEventAction.hpp>
#include <Label.hpp>
#include <Node.hpp>
#include <RichTextLabel.hpp>
#include <TextureRect.hpp>
#include <Thread.hpp>

#include "emulatorController.h"

namespace TheBoy{
	struct InputStates;
}

namespace Concept1 {
using namespace godot;
using namespace TheBoy;

class Main : public Node {
   public:
    /**
     * @brief Godot API for registing methods
     */
    static void _register_methods();

    /**
     * @brief Construct a new Main object
     */
    Main();
    /**
     * @brief Destroy the Main object
     */
    ~Main();

    void _init();
    void _ready();
    void _process(float delta);
    void _exit_tree();
    void _input(InputEvent* evt);

    InputStates *getInputState();

   private:
    GODOT_CLASS(Main, Node);
    bool _running = false;

    /// @brief Select rom button
    Button *_selectBtn;
    /// @brief Line edit with the rom path
    Label *_romPath;
    FileDialog *_fileDialog;

    /// @brief Reference to the on Scene outTexture
    TextureRect *_outTextureRect;
    /// @brief Reference to the on Scene out vRam texture
    TextureRect *_vRamTextureRect;

    Vector2 tileSizeView = Vector2(24, 16);

    ImageTexture *_outTexture;
    ImageTexture *_outVRamTexture;

    /// @brief Internal output image
    Image *_outImage;
    /// @brief Internal vRam representation
    Image *_outVRamImage;
    /// @brief
    RichTextLabel *_textOut;

    Color gbPallet[4] = {Color(1.0f, 1.0f, 1.0f), Color(0.33f, 0.33f, 0.33f),
                         Color(0.66f, 0.66f, 0.66f), Color()};

    /**
     * @brief Pointer to the TheBoy EmulatorController object
     */
    EmulatorController *_emulCtrl;

    Thread *_cpuThread;

    /// <summary>
    /// Internal input view values
    /// </summary>
    InputStates *viewInput;

    /// @brief On Ready gather nodes
    void OnReadyGets();
    void OnFileSelected(String newText);

    /// @brief Initial texture generation
    void GenerateTextures();
    /// @brief Builds the output string
    void BuildOutputString();
    /// @brief Builds the debug texture image
    void BuildDebugView();
    void addTileToDebug(bit16 addr, int tileId, Ref<Image> img);

    /// @brief Creates the BuildOutput view image
    void BuildOutputView();

    void CpuStep();
};

}  // namespace Concept1
#endif