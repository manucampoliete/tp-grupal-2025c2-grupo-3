#ifndef INTERFACE_RENDERER_H
#define INTERFACE_RENDERER_H

#include <cstdint>
#include <map>

#include <SDL2pp/Font.hh>
#include <SDL2pp/Rect.hh>
#include <SDL2pp/Renderer.hh>
#include <SDL2pp/Texture.hh>

#include "../utils/gameData.h"

#include "../gameHandling/world.h"

using namespace SDL2pp;  // NOLINT


class UIRenderer {
private:
    Renderer& renderer;
    Font& font;
    Font& fontSmall;
    Font& fontBig;
    World& world;
    uint8_t playerId;

    Texture& cheatImmortalityImg;
    Texture& cheatWinImg;
    Texture& cheatLoseImg;
    Texture& cheatSpeedImg;
    Texture& finishImg;

    Rect statsPopupRect;
    Rect modPopupRect;
    Rect speedButtonRect;
    Rect healthButtonRect;
    Rect accelButtonRect;
    Rect massButtonRect;
    Rect saveButtonRect;

    // Minimap: 20% of the window width
    // Bottom right corner
    Rect minimapRect;

public:
    UIRenderer(Renderer& renderer, Font& font, Font& fontSmall, Font& fontBig,
               World& world, uint8_t playerId,
               Texture& cheatImmortalityImg, Texture& cheatWinImg, Texture& cheatLoseImg, Texture& cheatSpeedImg, Texture& finishImg);

    void renderCountdown(uint8_t countdownNumber);

    void renderRaceUI(uint32_t raceTimerMs, int currentRace, int totalRaces,
                        int windowWidth);

    void renderStatsPopup(const RaceResults& currentResults, uint32_t statsTimerMs);

    void renderModificationPopup(bool speedModified, bool healthModified,
                                   bool accelModified, bool massModified, bool saved,
                                   uint32_t modTimerMs, const CarProperties& props);

    void renderCheatNotification(CheatType activeCheatNotification);

    void renderEliminatedPopup();
    void renderFinishedPopup();

    void renderPodium(const FinalResults& results);

    void renderMinimap(Texture& currentMapTexture, int windowWidth, int windowHeight);

    void renderHealthBar(uint8_t health, int windowWidth);
    
    // To recalculate all the UI
    void updateLayout(int windowWidth, int windowHeight);

    const Rect& getSpeedButtonRect() const { return speedButtonRect; }
    const Rect& getHealthButtonRect() const { return healthButtonRect; }
    const Rect& getAccelButtonRect() const { return accelButtonRect; }
    const Rect& getMassButtonRect() const { return massButtonRect; }
    const Rect& getSaveButtonRect() const { return saveButtonRect; }
};

#endif  // INTERFACE_RENDERER_H
