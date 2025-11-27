#include "interfaceRenderer.h"

#include <algorithm>
#include <sstream>
#include <string>

UIRenderer::UIRenderer(Renderer& renderer, Font& font, Font& fontSmall, Font& fontBig,
                       Texture& mapTexture, World& world, uint8_t playerId,
                       Texture& cheatImmortalityImg, Texture& cheatWinImg,
                       Texture& cheatLoseImg, Texture& cheatSpeedImg):
        renderer(renderer),
        font(font),
        fontSmall(fontSmall),
        fontBig(fontBig),
        mapTexture(mapTexture),
        world(world),
        playerId(playerId),
        cheatImmortalityImg(cheatImmortalityImg),
        cheatWinImg(cheatWinImg),
        cheatLoseImg(cheatLoseImg),
        cheatSpeedImg(cheatSpeedImg) {}


void UIRenderer::updateLayout(int windowWidth, int windowHeight) {
    // Update popups
    int popupW = static_cast<int>(windowWidth * 0.7f);
    int popupH = static_cast<int>(windowHeight * 0.8f);
    int popupX = (windowWidth - popupW) / 2;
    int popupY = (windowHeight - popupH) / 2;

    statsPopupRect = Rect(popupX, popupY, popupW, popupH);
    modPopupRect = Rect(popupX, popupY, popupW, popupH);

    // Update buttons (positions relative to the mods popup)
    // Check!!!
    int btnW = static_cast<int>(popupW * 0.4f);
    int btnH = static_cast<int>(popupH * 0.1f);
    int marginX = static_cast<int>(popupW * 0.1f);
    int marginY = static_cast<int>(popupH * 0.1f);

    speedButtonRect = Rect(popupX + marginX, popupY + marginY * 2, btnW, btnH);
    healthButtonRect = Rect(popupX + marginX, popupY + marginY * 4, btnW, btnH);
    accelButtonRect = Rect(popupX + marginX, popupY + marginY * 2, btnW, btnH);
    massButtonRect = Rect(popupX + marginX, popupY + marginY * 4, btnW, btnH);
    saveButtonRect = Rect(popupX + (popupW - btnW) / 2, popupY + marginY * 7, btnW, btnH);

    // Minimap
    // 20% of the window width
    float mapAspectRatio = (float)mapTexture.GetWidth() / (float)mapTexture.GetHeight();
    int minimapW = static_cast<int>(windowWidth * 0.2f);  // 20% of the width
    int minimapH = static_cast<int>(minimapW / mapAspectRatio);
    int margin = static_cast<int>(windowWidth * 0.01f);  // 1% margin

    // Bottom right corner
    minimapRect = Rect(windowWidth - minimapW - margin, windowHeight - minimapH - margin,
                        minimapW, minimapH);
}


void UIRenderer::renderCountdown(uint8_t countdownNumber) {
    // Reset scaling
    renderer.SetScale(1.0f, 1.0f);

    // Dark semi-transparent overlay
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 150);
    renderer.FillRect(Rect(0, 0, renderer.GetOutputWidth(), renderer.GetOutputHeight()));

    // Countdown text and color
    std::string text;
    SDL_Color color;

    if (countdownNumber > 3) {
        text = "READY";
        color = {255, 255, 255, 255};  // White
    } else if (countdownNumber == 3) {
        text = "3";
        color = {255, 0, 0, 255};    // Red
    } else if (countdownNumber == 2) {
        text = "2";
        color = {255, 165, 0, 255};  // Orange
    } else if (countdownNumber == 1) {
        text = "1";
        color = {255, 255, 0, 255};  // Yellow
    } else {                         // 0 = GO!
        text = "GO!";
        color = {0, 255, 0, 255};    // Green
    }

    // Create big font for the countdown
    Font bigFont("client/assets/fonts/VCR_OSD_MONO.ttf", 140);
    Surface s = bigFont.RenderText_Solid(text, color);
    Texture t(renderer, s);

    // Center on screen
    int x = (renderer.GetOutputWidth() - t.GetWidth()) / 2;
    int y = (renderer.GetOutputHeight() - t.GetHeight()) / 2;

    renderer.Copy(t, NullOpt, Rect(x, y, t.GetWidth(), t.GetHeight()));
}


void UIRenderer::renderRaceUI(uint32_t raceTimerMs, int currentRace, int totalRaces,
                                int windowWidth) {
    // Reset scaling to draw the UI that is drawn 1:1 over the window
    renderer.SetScale(1.0f, 1.0f);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 128);
    renderer.FillRect(Rect(0, 0, windowWidth, 40));  // 40px fixed, or could be h * 0.05

    // Manu's note: where's the conversion from ms to seconds? Are we receiving seconds directly? (variable name indicates ms)
    int totalSeconds = raceTimerMs;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    std::string timeText = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
                            (seconds < 10 ? "0" : "") + std::to_string(seconds);

    Surface timeSurface = font.RenderText_Solid(timeText, {255, 255, 255, 255});
    Texture timeTexture(renderer, timeSurface);
    renderer.Copy(timeTexture, NullOpt,
                  Rect(10, 5, timeTexture.GetWidth(), timeTexture.GetHeight()));

    std::string raceText =
            "Race " + std::to_string(currentRace) + "/" + std::to_string(totalRaces);
    Surface raceSurface = font.RenderText_Solid(raceText, {255, 255, 0, 255});
    Texture raceTexture(renderer, raceSurface);
    int xPos = windowWidth - raceTexture.GetWidth() - 10;
    renderer.Copy(raceTexture, NullOpt,
                  Rect(xPos, 5, raceTexture.GetWidth(), raceTexture.GetHeight()));
}


void UIRenderer::renderStatsPopup(const RaceResults& currentResults, uint32_t statsTimerMs) {
    // Reset scaling for the UI
    renderer.SetScale(1.0f, 1.0f);

    // Recalculate centered popup according to current window size
    int w = renderer.GetOutputWidth();
    int h = renderer.GetOutputHeight();
    int popupW = static_cast<int>(w * 0.7f);
    int popupH = static_cast<int>(h * 0.8f);
    int popupX = (w - popupW) / 2;
    int popupY = (h - popupH) / 2;
    Rect statsPopupRect(popupX, popupY, popupW, popupH);

    // Choose font based on window size
    Font& activeFont = (w < 1000) ? fontSmall : font;
    
    // Dark overlay in the background
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, w, h));

    renderer.SetDrawColor(40, 40, 40, 255);
    renderer.FillRect(statsPopupRect);

    // Popup's border
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(100, 100, 150, 255);
    renderer.DrawRect(statsPopupRect);

    Surface titleSurface = activeFont.RenderText_Solid("RACE RESULTS", {255, 255, 0, 255});
    Texture titleTexture(renderer, titleSurface);
    int titleX = statsPopupRect.x + (statsPopupRect.w - titleTexture.GetWidth()) / 2;
    renderer.Copy(titleTexture, NullOpt,
                  Rect(titleX, statsPopupRect.y + 30, titleTexture.GetWidth(),
                       titleTexture.GetHeight()));
    
    // TABLE HEADERS (centered in columns)
    int tableStartY = statsPopupRect.y + 100;
    int rowHeight = (w < 1000) ? 40 : 50;  // More compact rows on smaller screens
    
    // Define column widths (proportional to popup)
    int colPosW = static_cast<int>(statsPopupRect.w * 0.12f);
    int colNameW = static_cast<int>(statsPopupRect.w * 0.30f);
    int colRaceW = static_cast<int>(statsPopupRect.w * 0.29f);
    int colTotalW = static_cast<int>(statsPopupRect.w * 0.29f);

    int colPosX = statsPopupRect.x + 10;
    int colNameX = colPosX + colPosW;
    int colRaceX = colNameX + colNameW;
    int colTotalX = colRaceX + colRaceW;
    
    // Render headers
    Surface h1S = activeFont.RenderText_Solid("POS", {200, 200, 200, 255});
    Texture h1T(renderer, h1S);
    int posX = colPosX + (colPosW - h1T.GetWidth()) / 2;
    renderer.Copy(h1T, NullOpt, Rect(posX, tableStartY, h1T.GetWidth(), h1T.GetHeight()));

    Surface h2S = activeFont.RenderText_Solid("PLAYER", {200, 200, 200, 255});
    Texture h2T(renderer, h2S);
    int nameX = colNameX + (colNameW - h2T.GetWidth()) / 2;
    renderer.Copy(h2T, NullOpt, Rect(nameX, tableStartY, h2T.GetWidth(), h2T.GetHeight()));

    Surface h3S = activeFont.RenderText_Solid("RACE TIME", {200, 200, 200, 255});
    Texture h3T(renderer, h3S);
    int raceX = colRaceX + (colRaceW - h3T.GetWidth()) / 2;
    renderer.Copy(h3T, NullOpt, Rect(raceX, tableStartY, h3T.GetWidth(), h3T.GetHeight()));

    Surface h4S = activeFont.RenderText_Solid("TOTAL TIME", {200, 200, 200, 255});
    Texture h4T(renderer, h4S);
    int totalX = colTotalX + (colTotalW - h4T.GetWidth()) / 2;
    renderer.Copy(h4T, NullOpt, Rect(totalX, tableStartY, h4T.GetWidth(), h4T.GetHeight()));
    
    // Separator line under headers
    renderer.SetDrawColor(100, 100, 150, 255);
    int lineY = tableStartY + 35;
    renderer.DrawLine(colPosX, lineY, colTotalX + colTotalW - 20, lineY);
    
    // PLAYER ROWS
    int rowY = lineY + 15;
    int position = 1;

    for (const auto& player: currentResults.players) {
        // Position
        std::string posText = std::to_string(position);
        SDL_Color rowColor = (position == 1) ? SDL_Color{255, 215, 0, 255} : // Gold for 1°
                              (position == 2) ? SDL_Color{192, 192, 192, 255} : // Silver for 2°
                              (position == 3) ? SDL_Color{205, 127, 50, 255} : // Bronze for 3°
                              SDL_Color{255, 255, 255, 255}; // White for the rest

        Surface posS = activeFont.RenderText_Solid(posText, rowColor);
        Texture posT(renderer, posS);
        int posX = colPosX + (colPosW - posT.GetWidth()) / 2;
        renderer.Copy(posT, NullOpt, Rect(posX, rowY, posT.GetWidth(), posT.GetHeight()));

        // Name
        std::string displayName = player.playerName;
        if (displayName.empty()) displayName = "-";

        Surface nameS = activeFont.RenderText_Solid(displayName, rowColor);
        Texture nameT(renderer, nameS);
        int nameX = colNameX + (colNameW - nameT.GetWidth()) / 2;
        renderer.Copy(nameT, NullOpt,
                      Rect(nameX, rowY, nameT.GetWidth(), nameT.GetHeight()));

        // Race time
        int raceMinutes = player.raceTimeMs / 60000;
        int raceSeconds = (player.raceTimeMs % 60000) / 1000;
        int raceMillis = player.raceTimeMs % 1000;
        char raceTimeBuf[32];
        snprintf(raceTimeBuf, sizeof(raceTimeBuf), "%02d:%02d:%03d", raceMinutes, raceSeconds, raceMillis);

        Surface raceS = activeFont.RenderText_Solid(raceTimeBuf, rowColor);
        Texture raceT(renderer, raceS);
        int raceX = colRaceX + (colRaceW - raceT.GetWidth()) / 2;
        renderer.Copy(raceT, NullOpt,
                      Rect(raceX, rowY, raceT.GetWidth(), raceT.GetHeight()));
        
        // Total time
        int totalMinutes = player.totalTimeMs / 60000;
        int totalSeconds = (player.totalTimeMs % 60000) / 1000;
        int totalMillis = player.totalTimeMs % 1000;
        char totalTimeBuf[32];
        snprintf(totalTimeBuf, sizeof(totalTimeBuf), "%02d:%02d.%03d", totalMinutes,
                 totalSeconds, totalMillis);

        Surface totalS = activeFont.RenderText_Solid(totalTimeBuf, rowColor);
        Texture totalT(renderer, totalS);
        int totalX = colTotalX + (colTotalW - totalT.GetWidth()) / 2;
        renderer.Copy(totalT, NullOpt,
                      Rect(totalX, rowY, totalT.GetWidth(), totalT.GetHeight()));

        rowY += rowHeight;
        position++;
    }

    // COUNTDOWN (bottom centered)
    int seconds = statsTimerMs;
    std::cout << "Stats popup timer seconds: " << seconds << std::endl;
    std::string timerText = "Next stage in: " + std::to_string(seconds) + "s";
    Surface timerSurface =
            activeFont.RenderText_Solid(timerText, {150, 255, 150, 255});  // Light green
    Texture timerTexture(renderer, timerSurface);
    int timerX = statsPopupRect.x + (statsPopupRect.w - timerTexture.GetWidth()) / 2;
    int timerY = statsPopupRect.y + statsPopupRect.h - 60;
    renderer.Copy(timerTexture, NullOpt,
                  Rect(timerX, timerY, timerTexture.GetWidth(), timerTexture.GetHeight()));
}


void UIRenderer::renderModificationPopup(bool speedModified, bool healthModified, 
                                           bool accelModified, bool massModified, bool saved,
                                           uint32_t modTimerMs, const CarProperties& props) {
    // Reset scaling for the UI
    renderer.SetScale(1.0f, 1.0f);
    
    // Recalculate centered popup according to current window size
    int w = renderer.GetOutputWidth();
    int h = renderer.GetOutputHeight();
    int popupW = static_cast<int>(w * 0.7f);
    int popupH = static_cast<int>(h * 0.8f);
    int popupX = (w - popupW) / 2;
    int popupY = (h - popupH) / 2;
    Rect modPopupRect(popupX, popupY, popupW, popupH);
    
    // Choose font based on window size
    Font& activeFont = (w < 1000) ? fontSmall : font;

    // Buttons in two columns
    int btnW = static_cast<int>(popupW * 0.40);
    int btnH = static_cast<int>(popupH * 0.16f);
    int gapX = static_cast<int>(popupW * 0.06f); // Space between cols
    int gapY = static_cast<int>(popupH * 0.05f); // Space between rows
    int startY = modPopupRect.y + 130;
    
    int leftX = popupX + static_cast<int>(popupW * 0.08f);
    int rightX = leftX + btnW + gapX;

    Rect speedBtn(leftX, startY, btnW, btnH);
    Rect healthBtn(rightX, startY, btnW, btnH);
    Rect accelBtn(leftX, startY + btnH + gapY, btnW, btnH);
    Rect massBtn(rightX, startY + btnH + gapY, btnW, btnH);
    
    // Save button (centered)
    int saveBtnW = static_cast<int>(popupW * 0.5f);
    int saveBtnH = static_cast<int>(popupH * 0.1f);
    Rect saveBtn(popupX + (popupW - saveBtnW) / 2, modPopupRect.y + popupH - saveBtnH - 70, saveBtnW, saveBtnH);

    // Dark overlay in the background
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, w, h));

    renderer.SetDrawColor(40, 40, 40, 255);
    renderer.FillRect(modPopupRect);

    // Popup's border
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(100, 100, 150, 255);
    renderer.DrawRect(modPopupRect);

    Surface titleSurface = activeFont.RenderText_Solid("CAR MODIFICATIONS", {255, 255, 0, 255});
    Texture titleTexture(renderer, titleSurface);
    int titleX = modPopupRect.x + (modPopupRect.w - titleTexture.GetWidth()) / 2;
    renderer.Copy(titleTexture, NullOpt,
                  Rect(titleX, modPopupRect.y + 30, titleTexture.GetWidth(),
                       titleTexture.GetHeight()));

    // SUBTITLE
    Surface subSurface = activeFont.RenderText_Solid("Choose the updates you want for your car",
                                                       {180, 180, 180, 255});
    Texture subtexture(renderer, subSurface);
    int subX = modPopupRect.x + (modPopupRect.w - subtexture.GetWidth()) / 2;
    renderer.Copy(
            subtexture, NullOpt,
            Rect(subX, modPopupRect.y + 70, subtexture.GetWidth(), subtexture.GetHeight()));


    // SPEED BUTTON
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(speedModified ? 50 : 80, speedModified ? 200 : 80,
                          speedModified ? 50 : 100, 255);
    renderer.FillRect(speedBtn);
    
    // Button's border
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(speedModified ? 100 : 60, speedModified ? 255 : 100,
                          speedModified ? 100 : 120, 255);
    renderer.DrawRect(speedBtn);
    
    // Button's text (vertically centered)
    std::string speedText =
            "Speed: " + std::to_string(props.speed) + " -> " + std::to_string(props.speed + 5);
    Surface speedSurface = activeFont.RenderText_Solid(speedText, {255, 255, 255, 255});
    Texture speedTexture(renderer, speedSurface);
    int speedTextX = speedBtn.x + (speedBtn.w - speedTexture.GetWidth()) / 2;  // Centered
    int speedTextY = speedBtn.y + 12;
    renderer.Copy(speedTexture, NullOpt,
            Rect(speedTextX, speedTextY, speedTexture.GetWidth(), speedTexture.GetHeight()));
    
    // Penalty (to the right of the button)
    Surface speedPen = activeFont.RenderText_Solid("Cost: +10s", {255, 200, 100, 255});
    Texture speedPenT(renderer, speedPen);
    int speedPenX = speedBtn.x + (speedBtn.w - speedPenT.GetWidth()) / 2;  // Centered
    int speedPenY = speedTextY + speedTexture.GetHeight() + 5;
    renderer.Copy(speedPenT, NullOpt,
                  Rect(speedPenX, speedPenY, speedPenT.GetWidth(), speedPenT.GetHeight()));


    // HEALTH'S BUTTON
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(healthModified ? 50 : 80, healthModified ? 200 : 80,
                          healthModified ? 50 : 100, 255);
    renderer.FillRect(healthBtn);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(healthModified ? 100 : 60, healthModified ? 255 : 100,
                          healthModified ? 100 : 120, 255);
    renderer.DrawRect(healthBtn);

    std::string healthText =
            "Health: " + std::to_string(props.health) + " -> " + std::to_string(props.health + 5);
    Surface healthSurface = activeFont.RenderText_Solid(healthText, {255, 255, 255, 255});
    Texture healthTexture(renderer, healthSurface);
    int healthTextX = healthBtn.x + (healthBtn.w - healthTexture.GetWidth()) / 2;  // Centered
    int healthTextY = healthBtn.y + 10;
    renderer.Copy(healthTexture, NullOpt,
                  Rect(healthTextX, healthTextY, healthTexture.GetWidth(), healthTexture.GetHeight()));

    Surface healthPen = activeFont.RenderText_Solid("Cost: +8s", {255, 200, 100, 255});
    Texture healthPenT(renderer, healthPen);
    int healthPenX = healthBtn.x + (healthBtn.w - healthPenT.GetWidth()) / 2;  // Centered
    int healthPenY = healthTextY + healthTexture.GetHeight() + 5;
    renderer.Copy(healthPenT, NullOpt,
            Rect(healthPenX, healthPenY, healthPenT.GetWidth(), healthPenT.GetHeight()));
    

    // ACCEL BUTTON
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(accelModified ? 50 : 80, accelModified ? 200 : 80,
                          accelModified ? 50 : 100, 255);
    renderer.FillRect(accelBtn);
    
    // Button's border
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(accelModified ? 100 : 60, accelModified ? 255 : 100,
                          accelModified ? 100 : 120, 255);
    renderer.DrawRect(accelBtn);
    
    // Button's text (vertically centered)
    std::string accelText =
            "Accel: " + std::to_string(props.accel) + " -> " + std::to_string(props.accel + 5);
    Surface accelSurface = activeFont.RenderText_Solid(accelText, {255, 255, 255, 255});
    Texture accelTexture(renderer, accelSurface);
    int accelTextX = accelBtn.x + (accelBtn.w - accelTexture.GetWidth()) / 2;  // Centered
    int accelTextY = accelBtn.y + 10;
    renderer.Copy(accelTexture, NullOpt,
            Rect(accelTextX, accelTextY, accelTexture.GetWidth(), accelTexture.GetHeight()));
    
    // Penalty (to the right of the button)
    Surface accelPen = activeFont.RenderText_Solid("Cost: +10s", {255, 200, 100, 255});
    Texture accelPenT(renderer, accelPen);
    int accelPenX = accelBtn.x + (accelBtn.w - accelPenT.GetWidth()) / 2;  // Centered
    int accelPenY = accelTextY + accelTexture.GetHeight() + 5;
    renderer.Copy(accelPenT, NullOpt,
                  Rect(accelPenX, accelPenY, accelPenT.GetWidth(), accelPenT.GetHeight()));
    

    // MASS BUTTON
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(massModified ? 50 : 80, massModified ? 200 : 80,
                          massModified ? 50 : 100, 255);
    renderer.FillRect(massBtn);
    
    // Button's border
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(massModified ? 100 : 60, massModified ? 255 : 100,
                          massModified ? 100 : 120, 255);
    renderer.DrawRect(massBtn);
    
    // Button's text (vertically centered)
    std::string massText =
            "Mass: " + std::to_string(props.mass) + " -> " + std::to_string(props.mass + 5);
    Surface massSurface = activeFont.RenderText_Solid(massText, {255, 255, 255, 255});
    Texture massTexture(renderer, massSurface);
    int massTextX = massBtn.x + (massBtn.w - massTexture.GetWidth()) / 2;  // Centered
    int massTextY = massBtn.y + 10;
    renderer.Copy(massTexture, NullOpt,
            Rect(massTextX, massTextY, massTexture.GetWidth(), massTexture.GetHeight()));
    
    // Penalty (to the right of the button)
    Surface massPen = activeFont.RenderText_Solid("Cost: +10s", {255, 200, 100, 255});
    Texture massPenT(renderer, massPen);
    int massPenX = massBtn.x + (massBtn.w - massPenT.GetWidth()) / 2;  // Centered
    int massPenY = massTextY + massTexture.GetHeight() + 5;
    renderer.Copy(massPenT, NullOpt,
                  Rect(massPenX, massPenY, massPenT.GetWidth(), massPenT.GetHeight()));

    
    // SAVE BUTTON
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);

    if (saved)
        // Dark green when saved (can't change)
        renderer.SetDrawColor(30, 120, 30, 255);
    else
        // Blue when not saved
        renderer.SetDrawColor(50, 150, 255, 255);
    renderer.FillRect(saveBtn);

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

    if (saved)
        renderer.SetDrawColor(60, 200, 60, 255);  // Green border
    else
        renderer.SetDrawColor(100, 200, 255, 255);  // Blue border
    renderer.DrawRect(saveBtn);

    std::string saveText = saved ? "SAVED!" : "SAVE AND CONTINUE";
    Surface saveSurface = activeFont.RenderText_Solid(saveText, {255, 255, 255, 255});
    Texture saveTexture(renderer, saveSurface);
    int saveTextX = saveBtn.x + (saveBtn.w - saveTexture.GetWidth()) / 2;
    int saveTextY = saveBtn.y + (saveBtn.h - saveTexture.GetHeight()) / 2;
    renderer.Copy(saveTexture, NullOpt,
            Rect(saveTextX, saveTextY, saveTexture.GetWidth(), saveTexture.GetHeight()));

    
    // COUNTDOWN (bottom centered)
    int seconds = modTimerMs / 1000;
    std::string countdown = "Next race in: " + std::to_string(seconds) + "s";
    Surface countdownSurface =
            activeFont.RenderText_Solid(countdown, {255, 150, 150, 255});  // Light red
    Texture countdownTexture(renderer, countdownSurface);
    int timerX = modPopupRect.x + (modPopupRect.w - countdownTexture.GetWidth()) / 2;
    int timerY = modPopupRect.y + modPopupRect.h - 40;
    renderer.Copy(countdownTexture, NullOpt,
            Rect(timerX, timerY, countdownTexture.GetWidth(), countdownTexture.GetHeight()));

    speedButtonRect = speedBtn;
    healthButtonRect = healthBtn;
    accelButtonRect = accelBtn;
    massButtonRect = massBtn;
    saveButtonRect = saveBtn;
}


void UIRenderer::renderMinimap() {
    // Reset scaling just in case
    renderer.SetScale(1.0f, 1.0f);
    
    // Semi-transparent background
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 128);  // Black, 50% opacity
    renderer.FillRect(minimapRect);
    
    // Complete map, scaled down (NullOpt means "copy the entire texture")
    renderer.Copy(mapTexture, NullOpt, minimapRect);

    // Cars as points for the minimap
    const auto& cars = world.getCars();
    float mapW = (float)mapTexture.GetWidth();
    float mapH = (float)mapTexture.GetHeight();

    for (const auto& [id, carState]: cars) {
        float ratioX = carState.x / mapW;
        float ratioY = carState.y / mapH;

        int minimapCarX = minimapRect.x + static_cast<int>(ratioX * minimapRect.w);
        int minimapCarY = minimapRect.y + static_cast<int>(ratioY * minimapRect.h);
        
        uint8_t carType = carState.type;
        switch (carType) {
            case 0:  // Green (CARS[0])
                renderer.SetDrawColor(0, 255, 0, 255);
                break;
            case 1:  // Red (CARS[1])
            case 2:  // Red (CARS[2])
            case 5:  // Red/Black (CARS[5])
                renderer.SetDrawColor(255, 0, 0, 255);
                break;
            case 3:  // Blue (CARS[3])
            case 4:  // Blue (CARS[4])
                renderer.SetDrawColor(0, 150, 255, 255);
                break;
            case 6:  // Violet (CARS[6])
                renderer.SetDrawColor(128, 0, 128, 255);
                break;
            default:
                renderer.SetDrawColor(255, 255, 255, 255);
                break;
        }

        // Draw player point a bit bigger
        if (id == playerId) {
            Rect carDotRect(minimapCarX - 2, minimapCarY - 2, 8, 8);  // 5x5
            renderer.FillRect(carDotRect);
        } else {
            Rect carDotRect(minimapCarX - 1, minimapCarY - 1, 5, 5);  // 3x3
            renderer.FillRect(carDotRect);
        }
    }

    // White border
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(255, 255, 255, 255);
    renderer.DrawRect(minimapRect);
}


void UIRenderer::renderCheatNotification(CheatType activeCheatNotification) {
    renderer.SetScale(1.0f, 1.0f);

    int w = renderer.GetOutputWidth();
    int h = renderer.GetOutputHeight();

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, w, h));

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

    std::string title;
    SDL_Color titleColor;
    Texture* cheatImg = nullptr;

    switch (activeCheatNotification) {
        case CheatType::INMORTALITY:
            title = "IMMORTALITY ACTIVATED";
            titleColor = {255, 215, 0, 255};  // Gold
            cheatImg = &cheatImmortalityImg;
            break;

        case CheatType::INSTA_WIN:
            title = "INSTANT WIN!";
            titleColor = {0, 255, 0, 255};  // Green
            cheatImg = &cheatWinImg;
            break;

        case CheatType::INSTA_LOSE:
            title = "INSTANT LOSE";
            titleColor = {255, 0, 0, 255};  // Red
            cheatImg = &cheatLoseImg;
            break;
        
        case CheatType::SUPER_SPEED:
            title = "SUPER SPEED";
            titleColor = {255, 255, 0, 255};  // Yellow
            cheatImg = &cheatSpeedImg;
            break;

        default:
            return;
    }

    // Render PNG with transparency
    if (cheatImg) {
        int imgW = std::min(cheatImg->GetWidth(), w / 2);
        int imgH = (imgW * cheatImg->GetHeight()) / cheatImg->GetWidth();

        int imgX = (w - imgW) / 2;
        int imgY = (h - imgH) / 2 + 20;

        renderer.Copy(*cheatImg, NullOpt, Rect(imgX, imgY, imgW, imgH));
    }

    // Title below the image
    Surface titleSurface = fontBig.RenderText_Solid(title, titleColor);
    Texture titleTexture(renderer, titleSurface);

    int titleX = (w - titleTexture.GetWidth()) / 2;
    int titleY = 100;

    if (!cheatImg)
        titleY = (h - titleTexture.GetHeight()) / 2;

    renderer.Copy(titleTexture, NullOpt,
                  Rect(titleX, titleY, titleTexture.GetWidth(), titleTexture.GetHeight()));
}


void UIRenderer::renderEliminatedPopup() {
    renderer.SetScale(1.0f, 1.0f);

    int w = renderer.GetOutputWidth();
    int h = renderer.GetOutputHeight();

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(0, 0, w, h));

    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);

    std::string title = "YOU DIED!";
    SDL_Color titleColor = {255, 0, 0, 255};
    Texture* img = &cheatLoseImg;

    // Render PNG with transparency
    if (img) {
        int imgW = std::min(img->GetWidth(), w / 2);
        int imgH = (imgW * img->GetHeight()) / img->GetWidth();

        int imgX = (w - imgW) / 2;
        int imgY = (h - imgH) / 2 + 20;

        renderer.Copy(*img, NullOpt, Rect(imgX, imgY, imgW, imgH));
    }

    // Title below the image
    Surface titleSurface = fontBig.RenderText_Solid(title, titleColor);
    Texture titleTexture(renderer, titleSurface);

    int titleX = (w - titleTexture.GetWidth()) / 2;
    int titleY = 100;

    if (!img)
        titleY = (h - titleTexture.GetHeight()) / 2;

    renderer.Copy(titleTexture, NullOpt,
                  Rect(titleX, titleY, titleTexture.GetWidth(), titleTexture.GetHeight()));
}


void UIRenderer::renderPodium(const FinalResults& results) {
    renderer.SetScale(1.0f, 1.0f);

    int w = renderer.GetOutputWidth();
    int h = renderer.GetOutputHeight();

    // Dark background
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 200);
    renderer.FillRect(Rect(0, 0, w, h));

    // Popup's background
    int popupW = static_cast<int>(w * 0.85f);
    int popupH = static_cast<int>(h * 0.9f);
    int popupX = (w - popupW) / 2;
    int popupY = (h - popupH) / 2;

    renderer.SetDrawColor(30, 30, 40, 255);
    renderer.FillRect(Rect(popupX, popupY, popupW, popupH));

    // Border
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(200, 200, 200, 255);
    renderer.DrawRect(Rect(popupX, popupY, popupW, popupH));

    // Title
    Surface titleS = fontBig.RenderText_Solid("FINAL RESULTS", {255, 215, 0, 255});
    Texture titleT(renderer, titleS);
    int titleX = popupX + (popupW - titleT.GetWidth()) / 2;
    int titleY = popupY + 30;
    renderer.Copy(titleT, NullOpt,
                  Rect(titleX, titleY, titleT.GetWidth(), titleT.GetHeight()));


    // PODIUM
    int podiumY = titleY + titleT.GetHeight() + 50;
    int podiumBaseY = podiumY + 200;
    int boxWidth = 120;

    // (2º - 1º - 3º)
    int pos1X = popupX + popupW / 2 - boxWidth / 2;
    int pos2X = pos1X - boxWidth - 40;
    int pos3X = pos1X + boxWidth + 40;
    int height1 = 150;
    int height2 = 120;
    int height3 = 90;

    if (results.standings.size() >= 1) {  // 1º
        const auto& first = results.standings[0];
        int boxY = podiumBaseY - height1;

        renderer.SetDrawColor(255, 215, 0, 255);  // Gold
        renderer.FillRect(Rect(pos1X, boxY, boxWidth, height1));
        renderer.SetDrawColor(200, 170, 0, 255);
        renderer.DrawRect(Rect(pos1X, boxY, boxWidth, height1));

        Surface numS = fontBig.RenderText_Solid("1", {255, 255, 255, 255});
        Texture numT(renderer, numS);
        renderer.Copy(numT, NullOpt,
                      Rect(pos1X + (boxWidth - numT.GetWidth()) / 2, boxY + 10,
                           numT.GetWidth(), numT.GetHeight()));

        // Name
        Surface nameS = fontSmall.RenderText_Solid(first.playerName, {255, 255, 255, 255});
        Texture nameT(renderer, nameS);
        renderer.Copy(nameT, NullOpt,
                      Rect(pos1X + (boxWidth - nameT.GetWidth()) / 2, boxY + 70,
                           nameT.GetWidth(), nameT.GetHeight()));

        // Final time
        int minutes = first.totalTimeMs / 60000;
        int seconds = (first.totalTimeMs % 60000) / 1000;
        int millis = first.totalTimeMs % 1000;
        char timeBuf[32];
        snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d.%03d", minutes, seconds, millis);

        Surface timeS = fontSmall.RenderText_Solid(timeBuf, {255, 255, 255, 255});
        Texture timeT(renderer, timeS);
        renderer.Copy(timeT, NullOpt,
                      Rect(pos1X + (boxWidth - timeT.GetWidth()) / 2, boxY + 100,
                           timeT.GetWidth(), timeT.GetHeight()));
    }

    if (results.standings.size() >= 2) {  // 2º
        const auto& second = results.standings[1];
        int boxY = podiumBaseY - height2;

        renderer.SetDrawColor(192, 192, 192, 255);  // Silver
        renderer.FillRect(Rect(pos2X, boxY, boxWidth, height2));
        renderer.SetDrawColor(140, 140, 140, 255);
        renderer.DrawRect(Rect(pos2X, boxY, boxWidth, height2));

        Surface numS = font.RenderText_Solid("2", {255, 255, 255, 255});
        Texture numT(renderer, numS);
        renderer.Copy(numT, NullOpt,
                      Rect(pos2X + (boxWidth - numT.GetWidth()) / 2, boxY + 10,
                           numT.GetWidth(), numT.GetHeight()));

        // Name
        Surface nameS = fontSmall.RenderText_Solid(second.playerName, {255, 255, 255, 255});
        Texture nameT(renderer, nameS);
        renderer.Copy(nameT, NullOpt,
                      Rect(pos2X + (boxWidth - nameT.GetWidth()) / 2, boxY + 50,
                           nameT.GetWidth(), nameT.GetHeight()));

        // Final time
        int minutes = second.totalTimeMs / 60000;
        int seconds = (second.totalTimeMs % 60000) / 1000;
        int millis = second.totalTimeMs % 1000;
        char timeBuf[32];
        snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d.%03d", minutes, seconds, millis);

        Surface timeS = fontSmall.RenderText_Solid(timeBuf, {255, 255, 255, 255});
        Texture timeT(renderer, timeS);
        renderer.Copy(timeT, NullOpt,
                      Rect(pos2X + (boxWidth - timeT.GetWidth()) / 2, boxY + 75,
                           timeT.GetWidth(), timeT.GetHeight()));
    }

    if (results.standings.size() >= 3) {  // 3º
        const auto& third = results.standings[2];
        int boxY = podiumBaseY - height3;

        renderer.SetDrawColor(205, 127, 50, 255);  // Bronze
        renderer.FillRect(Rect(pos3X, boxY, boxWidth, height3));
        renderer.SetDrawColor(160, 100, 40, 255);
        renderer.DrawRect(Rect(pos3X, boxY, boxWidth, height3));

        Surface numS = font.RenderText_Solid("3", {255, 255, 255, 255});
        Texture numT(renderer, numS);
        renderer.Copy(numT, NullOpt,
                      Rect(pos3X + (boxWidth - numT.GetWidth()) / 2, boxY + 10,
                           numT.GetWidth(), numT.GetHeight()));

        // Name
        Surface nameS = fontSmall.RenderText_Solid(third.playerName, {255, 255, 255, 255});
        Texture nameT(renderer, nameS);
        renderer.Copy(nameT, NullOpt,
                      Rect(pos3X + (boxWidth - nameT.GetWidth()) / 2, boxY + 40,
                           nameT.GetWidth(), nameT.GetHeight()));

        // Final time
        int minutes = third.totalTimeMs / 60000;
        int seconds = (third.totalTimeMs % 60000) / 1000;
        int millis = third.totalTimeMs % 1000;
        char timeBuf[32];
        snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d.%03d", minutes, seconds, millis);

        Surface timeS = fontSmall.RenderText_Solid(timeBuf, {255, 255, 255, 255});
        Texture timeT(renderer, timeS);
        renderer.Copy(timeT, NullOpt,
                      Rect(pos3X + (boxWidth - timeT.GetWidth()) / 2, boxY + 60,
                           timeT.GetWidth(), timeT.GetHeight()));
    }

    // TABLA DEL RESTO (4° en adelante)
    if (results.standings.size() > 3) {
        int tableY = podiumBaseY + 50;
        int rowHeight = 35;

        Surface headerS = fontSmall.RenderText_Solid("Other Players:", {200, 200, 200, 255});
        Texture headerT(renderer, headerS);
        renderer.Copy(headerT, NullOpt,
                      Rect(popupX + 40, tableY, headerT.GetWidth(), headerT.GetHeight()));

        tableY += headerT.GetHeight() + 10;
        renderer.SetDrawColor(100, 100, 100, 255);
        renderer.DrawLine(popupX + 40, tableY, popupX + popupW - 40, tableY);

        tableY += 10;
        
        // Players from 4th place onwards
        for (size_t i = 3; i < results.standings.size() && i < 8; i++) {
            const auto& player = results.standings[i];

            // Position
            std::string posText = std::to_string(player.position) + "°";
            Surface posS = fontSmall.RenderText_Solid(posText, {180, 180, 180, 255});
            Texture posT(renderer, posS);
            renderer.Copy(posT, NullOpt,
                          Rect(popupX + 60, tableY, posT.GetWidth(), posT.GetHeight()));

            // Name
            Surface nameS = fontSmall.RenderText_Solid(player.playerName, {200, 200, 200, 255});
            Texture nameT(renderer, nameS);
            renderer.Copy(nameT, NullOpt,
                          Rect(popupX + 140, tableY, nameT.GetWidth(), nameT.GetHeight()));

            // Final time
            int minutes = player.totalTimeMs / 60000;
            int seconds = (player.totalTimeMs % 60000) / 1000;
            int millis = player.totalTimeMs % 1000;
            char timeBuf[32];
            snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d.%03d", minutes, seconds, millis);

            Surface timeS = fontSmall.RenderText_Solid(timeBuf, {180, 180, 180, 255});
            Texture timeT(renderer, timeS);
            renderer.Copy(
                    timeT, NullOpt,
                    Rect(popupX + popupW - 200, tableY, timeT.GetWidth(), timeT.GetHeight()));

            tableY += rowHeight;
        }
    }

    Surface closeS = fontSmall.RenderText_Solid("Press ESC to exit", {150, 150, 150, 255});
    Texture closeT(renderer, closeS);
    renderer.Copy(closeT, NullOpt,
                  Rect(popupX + (popupW - closeT.GetWidth()) / 2, popupY + popupH - 40,
                       closeT.GetWidth(), closeT.GetHeight()));
}


void UIRenderer::renderHealthBar(uint8_t health, int windowWidth) {
    renderer.SetScale(1.0f, 1.0f);
    
    int barWidth = 150;
    int barHeight = 20;
    int barX = (windowWidth - barWidth) / 2; 
    int barY = 10;  // same Y as timer
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(50, 50, 50, 200);
    renderer.FillRect(Rect(barX, barY, barWidth, barHeight));
    
    // width of the filled part of the bar
    int filledWidth = (health * barWidth) / 100;
    
    SDL_Color healthColor;
    if (health > 60)
        healthColor = {0, 200, 0, 255}; // Green
    else if (health > 30)
        healthColor = {255, 165, 0, 255}; // Orange
    else
        healthColor = {255, 0, 0, 255}; // Red
    
    // filled part of the bar
    if (filledWidth > 0) {
        renderer.SetDrawColor(healthColor.r, healthColor.g, healthColor.b, healthColor.a);
        renderer.FillRect(Rect(barX, barY, filledWidth, barHeight));
    }
    
    renderer.SetDrawBlendMode(SDL_BLENDMODE_NONE);
    renderer.SetDrawColor(255, 255, 255, 255);
    renderer.DrawRect(Rect(barX, barY, barWidth, barHeight));
    
    std::string healthText = std::to_string(health) + "%";
    Surface healthSurface = fontSmall.RenderText_Solid(healthText, {255, 255, 255, 255});
    Texture healthTexture(renderer, healthSurface);
    
    int textX = barX + (barWidth - healthTexture.GetWidth()) / 2;
    int textY = barY + (barHeight - healthTexture.GetHeight()) / 2;
    renderer.Copy(healthTexture, NullOpt, Rect(textX, textY, healthTexture.GetWidth(), healthTexture.GetHeight()));
}