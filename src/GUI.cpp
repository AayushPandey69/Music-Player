#include "GUI.hpp"
#include "Utilities.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>


GUI::GUI(sf::RenderWindow& window, MusicPlayer& player)
    : window(window), player(player), currentPage(Page::Home), isSearchBarActive(false), clickedSongIndex(-1) {
    initializeGUI();
}

void GUI::initializeGUI() {
    // Load textures and set up sprites
    if (!playTexture.loadFromFile("../Icons/play.png") ||
        !pauseTexture.loadFromFile("../Icons/pause.png") ||
        !nextTexture.loadFromFile("../Icons/skip.png") ||
        !prevTexture.loadFromFile("../Icons/back.png") ||
        !shuffleTexture.loadFromFile("../Icons/shuffle.png") ||
        !loopTexture.loadFromFile("../Icons/loop.png") ||
        !volumeTexture.loadFromFile("../Icons/volume.png")) {
        std::cerr << "Error loading images" << std::endl;
    }

    // Set up sprites
    playPauseButton.setTexture(playTexture);
    nextButton.setTexture(nextTexture);
    prevButton.setTexture(prevTexture);
    shuffleButton.setTexture(shuffleTexture);
    loopButton.setTexture(loopTexture);
    volumeButton.setTexture(volumeTexture);

    // Set button sizes and positions
    float buttonWidth = 80.0f;
    float buttonHeight = 80.0f;
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;
    float yPosition = windowHeight - buttonHeight - 10.0f;

    playPauseButton.setScale(buttonWidth / playPauseButton.getLocalBounds().width, buttonHeight / playPauseButton.getLocalBounds().height);
    nextButton.setScale(buttonWidth / nextButton.getLocalBounds().width, buttonHeight / nextButton.getLocalBounds().height);
    prevButton.setScale(buttonWidth / prevButton.getLocalBounds().width, buttonHeight / prevButton.getLocalBounds().height);
    shuffleButton.setScale(buttonWidth / shuffleButton.getLocalBounds().width, buttonHeight / shuffleButton.getLocalBounds().height);
    loopButton.setScale(buttonWidth / loopButton.getLocalBounds().width, buttonHeight / loopButton.getLocalBounds().height);
    volumeButton.setScale(buttonWidth / volumeButton.getLocalBounds().width, buttonHeight / volumeButton.getLocalBounds().height);

    playPauseButton.setPosition((windowWidth - buttonWidth) / 2, yPosition);
    prevButton.setPosition(playPauseButton.getPosition().x - 2 * buttonWidth, yPosition);
    nextButton.setPosition(playPauseButton.getPosition().x + 2 * buttonWidth, yPosition);
    shuffleButton.setPosition(playPauseButton.getPosition().x - 4 * buttonWidth, yPosition);
    loopButton.setPosition(playPauseButton.getPosition().x + 4 * buttonWidth, yPosition);
    volumeButton.setPosition(10, yPosition);

    // Set up shapes
    sidebar.setSize(sf::Vector2f(200.0f, windowHeight));
    sidebar.setFillColor(sf::Color(30, 30, 30));

    contentArea.setSize(sf::Vector2f(windowWidth - 200.0f, windowHeight));
    contentArea.setPosition(200.0f, 0.0f);
    contentArea.setFillColor(sf::Color(40, 40, 40));

    controlBar.setSize(sf::Vector2f(windowWidth, 100.0f));
    controlBar.setPosition(0.0f, windowHeight - 100.0f);
    controlBar.setFillColor(sf::Color(20, 20, 20));

    searchBar.setSize(sf::Vector2f(180.0f, 30.0f));
    searchBar.setPosition(10.0f, 10.0f);
    searchBar.setFillColor(sf::Color(50, 50, 50));

    progressBar.setSize(sf::Vector2f(window.getSize().x - 240.0f, 15.0f));
    progressBar.setPosition(220.0f, windowHeight - 135.0f);
    progressBar.setFillColor(sf::Color(60, 60, 60));

    progressFill.setSize(sf::Vector2f(0, 15.0f));
    progressFill.setPosition(220.0f, windowHeight - 135.0f);
    progressFill.setFillColor(sf::Color(29, 185, 84));

    volumeSliderBackground.setSize(sf::Vector2f(120.0f, 10.0f));
    volumeSliderBackground.setPosition(120.0f, windowHeight - 53.0f);
    volumeSliderBackground.setFillColor(sf::Color(60, 60, 60));

    volumeFill.setSize(sf::Vector2f(80.0f, 10.0f));
    volumeFill.setPosition(120.0f, windowHeight - 53.0f);
    volumeFill.setFillColor(sf::Color(29, 185, 84));

    // Load font
    if (!font.loadFromFile("../Fonts/ARIAL.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }

    // Set up text
    search.setFont(font);
    search.setString("Search");
    search.setCharacterSize(20);
    search.setFillColor(sf::Color(200, 200, 200));
    search.setPosition(15.0f, 15.0f);

    searchText.setFont(font);
    searchText.setCharacterSize(20);
    searchText.setFillColor(sf::Color::White);
    searchText.setPosition(15.0f, 15.0f);

    // Set up sidebar texts
    std::vector<std::string> sidebarOptions = { "Home", "Playlists", "Now\nPlaying" };
    for (size_t i = 0; i < sidebarOptions.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(sidebarOptions[i]);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::White);
        text.setPosition(10.0f, 60.0f + i * 120.0f);
        sidebarTexts.push_back(text);
    }

    // Initialize animation bars
    for (int i = 0; i < 3; ++i) {
        sf::RectangleShape bar;
        bar.setSize(sf::Vector2f(5.0f, 0.0f));
        bar.setFillColor(sf::Color(29, 185, 84));
        animationBars.push_back(bar);
    }

    currentSong = ""; // Initialize with empty string
    playPauseButton.setTexture(playTexture);
}

void GUI::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            handleMouseClick(event.mouseButton);
        }
        else if (event.type == sf::Event::MouseMoved) {
            handleMouseMove(event.mouseMove);
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            handleMouseRelease(event.mouseButton);
        }
        else if (event.type == sf::Event::TextEntered && isSearchBarActive) {
            handleTextEntered(event.text);
        }
    }
}

void GUI::handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton) {
    if (playPauseButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        togglePlayPause();
    }
    else if (nextButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        player.next();
        playPauseButton.setTexture(pauseTexture);
        currentSong = getBaseName(player.getCurrentSong());
        clickedSongIndex = player.getCurrentIndex();
    }
    else if (prevButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        player.previous();
        playPauseButton.setTexture(pauseTexture);
        currentSong = getBaseName(player.getCurrentSong());
        clickedSongIndex = player.getCurrentIndex();
    }
    else if (shuffleButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        toggleShuffle();
    }
    else if (loopButton.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        toggleLoop();
    }
    else if (sidebarTexts[0].getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        currentPage = Page::Home;
    }
    else if (sidebarTexts[1].getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        currentPage = Page::Playlists;
    }
    else if (sidebarTexts[2].getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        currentPage = Page::NowPlaying;
    }
    else if (progressBar.getGlobalBounds().contains(mouseButton.x, mouseButton.y) && player.getStatus() == sf::SoundSource::Playing) {
        setProgressFromMouseClick(mouseButton.x);
    }
    else if (volumeSliderBackground.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        setVolumeFromMouseClick(mouseButton.x);
    }
    else if (searchBar.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
        activateSearchBar();
    }
    else {
        deactivateSearchBar();
    }

    if (currentPage == Page::Home) {
        handleHomePageClick(mouseButton);
    }
}

void GUI::handleMouseMove(const sf::Event::MouseMoveEvent& mouseMove) {
    if (progressBar.getGlobalBounds().contains(mouseMove.x, mouseMove.y) && player.getStatus() == sf::SoundSource::Playing) {
        updateProgressBarPreview(mouseMove.x);
    }
    if (volumeSliderBackground.getGlobalBounds().contains(mouseMove.x, mouseMove.y)) {
        updateVolumeSliderPreview(mouseMove.x);
    }
}

void GUI::handleMouseRelease(const sf::Event::MouseButtonEvent& mouseButton) {
    if (progressBar.getGlobalBounds().contains(mouseButton.x, mouseButton.y) && player.getStatus() == sf::SoundSource::Playing) {
        setProgressFromMouseClick(mouseButton.x);
    }
}

void GUI::handleTextEntered(const sf::Event::TextEvent& text) {
    if (text.unicode == 8 && !searchQuery.empty()) { // Backspace
        searchQuery.pop_back();
    }
    else if (text.unicode < 128) {
        searchQuery += static_cast<char>(text.unicode);
    }
    searchText.setString(searchQuery);
    filterMusicFiles(player.getMusicFiles(), searchQuery, filteredMusicFiles);
}

void GUI::update() {
    updateProgressBar();
}

void GUI::draw() {
    window.clear();

    window.draw(sidebar);
    for (const auto& text : sidebarTexts) {
        window.draw(text);
    }

    window.draw(contentArea);
    window.draw(controlBar);

    window.draw(playPauseButton);
    window.draw(nextButton);
    window.draw(prevButton);
    window.draw(shuffleButton);
    window.draw(loopButton);
    window.draw(volumeButton);

    window.draw(searchBar);
    window.draw(search);
    window.draw(searchText);

    if (isSearchBarActive) {
        drawSearchCursor();
    }

    window.draw(progressBar);
    window.draw(progressFill);

    window.draw(volumeSliderBackground);
    window.draw(volumeFill);

    switch (currentPage) {
    case Page::Home:
        drawHomePage();
        break;
    case Page::Playlists:
        drawPlaylistsPage();
        break;
    case Page::NowPlaying:
        drawNowPlayingPage();
        break;
    }

    window.display();
}

void GUI::drawHomePage() {
    const auto& displayFiles = searchQuery.empty() ? player.getMusicFiles() : filteredMusicFiles;
    for (size_t i = 0; i < displayFiles.size(); ++i) {
        sf::RectangleShape songButton(sf::Vector2f(window.getSize().x - 220.0f, 50.0f));
        songButton.setPosition(210.0f, 60.0f + i * 60.0f);
        songButton.setFillColor(sf::Color(70, 70, 70));
        
        if (i == clickedSongIndex) {
            songButton.setFillColor(sf::Color::Red);
        }

        window.draw(songButton);

        sf::Text songText;
        songText.setFont(font);
        songText.setString(getBaseName(displayFiles[i]));
        songText.setCharacterSize(30);
        songText.setFillColor(sf::Color::White);
        songText.setPosition(220.0f, 60.0f + i * 60.0f + 10.0f);
        window.draw(songText);

        if (i == clickedSongIndex && player.getStatus() == sf::SoundSource::Playing) {
            drawAnimationBars(songButton.getPosition());
        }
    }
}

void GUI::drawPlaylistsPage() {
    std::vector<std::string> playlists = {
        "Favorites",
        "Chill Vibes",
        "Workout Mix"
    };

    for (size_t i = 0; i < playlists.size(); ++i) {
        sf::RectangleShape playlistButton(sf::Vector2f(window.getSize().x - 220.0f, 50.0f));
        playlistButton.setPosition(210.0f, 60.0f + i * 60.0f);
        playlistButton.setFillColor(sf::Color(70, 70, 70));
        playlistButton.setOutlineColor(sf::Color::Black);
        playlistButton.setOutlineThickness(2.0f);

        window.draw(playlistButton);

        sf::Text playlistText;
        playlistText.setFont(font);
        playlistText.setString(playlists[i]);
        playlistText.setCharacterSize(30);
        playlistText.setFillColor(sf::Color::White);
        playlistText.setPosition(220.0f, 60.0f + i * 60.0f + 10.0f);
        window.draw(playlistText);
        }
}

void GUI::drawNowPlayingPage() {
    sf::Text songNameText;
    songNameText.setFont(font);
    std::string wrappedSongName = wrapText(currentSong, 30);
    songNameText.setString(wrappedSongName);
    songNameText.setCharacterSize(50);
    songNameText.setFillColor(sf::Color::White);
    songNameText.setPosition((window.getSize().x - songNameText.getLocalBounds().width) / 2, 100.0f);
    window.draw(songNameText);

    float largeBarMaxHeight = 100.0f;
    for (int i = 0; i < 20; ++i) {
        float height = (std::sin(clock.getElapsedTime().asSeconds() * 5 + i) + 1) * largeBarMaxHeight / 2;
        sf::RectangleShape bar(sf::Vector2f(15.0f, height));
        bar.setFillColor(sf::Color(29, 185, 84));
        bar.setPosition(window.getSize().x / 2 - 200.0f + i * 20.0f, window.getSize().y / 2 - height / 2);
        window.draw(bar);
    }
}

void GUI::drawAnimationBars(const sf::Vector2f& position) {
    for (int i = 0; i < 3; ++i) {
        float height = (std::sin(clock.getElapsedTime().asSeconds() * 5 + i) + 1) * barMaxHeight / 2;
        animationBars[i].setSize(sf::Vector2f(5.0f, height));
        animationBars[i].setPosition(window.getSize().x - 50.0f + i * 10.0f, position.y + 25.0f - height / 2);
        window.draw(animationBars[i]);
    }
}

void GUI::drawSearchCursor() {
    if (cursorBlinkClock.getElapsedTime().asSeconds() < 0.5f) {
        sf::RectangleShape cursor(sf::Vector2f(2, 20));
        cursor.setFillColor(sf::Color::White);
        cursor.setPosition(15.0f + searchText.getLocalBounds().width, 15.0f);
        window.draw(cursor);
    }
    if (cursorBlinkClock.getElapsedTime().asSeconds() >= 1.0f) {
        cursorBlinkClock.restart();
    }
}

void GUI::togglePlayPause() {
    if (player.getStatus() == sf::SoundSource::Playing) {
        player.pause();
        playPauseButton.setTexture(playTexture);
    }
    else {
        player.play();
        playPauseButton.setTexture(pauseTexture);
    }
}

void GUI::toggleShuffle() {
    bool shuffleState = !player.getIsShuffled();
    player.shuffle(shuffleState);
    shuffleButton.setColor(shuffleState ? sf::Color::Green : sf::Color::White);
}

void GUI::toggleLoop() {
    bool loopState = !player.getIsLooping();
    player.loop(loopState);
    loopButton.setColor(loopState ? sf::Color::Green : sf::Color::White);
}

void GUI::setProgressFromMouseClick(float mouseX) {
    float position = (mouseX - progressBar.getPosition().x) / progressBar.getSize().x;
    position = std::clamp(position, 0.0f, 1.0f);
    player.setPlaybackPosition(position);
}

void GUI::setVolumeFromMouseClick(float mouseX) {
    float volume = (mouseX - volumeSliderBackground.getPosition().x) / volumeSliderBackground.getSize().x;
    volume = std::clamp(volume, 0.0f, 1.0f);
    volumeFill.setSize(sf::Vector2f(volume * volumeSliderBackground.getSize().x, volumeFill.getSize().y));
    player.setVolume(volume * 100);
}

void GUI::activateSearchBar() {
    isSearchBarActive = true;
    currentPage = Page::Home;
    search.setString("");
    searchBar.setFillColor(sf::Color(80, 80, 80));
    cursorBlinkClock.restart();
}

void GUI::deactivateSearchBar() {
    search.setString("Search");
    isSearchBarActive = false;
    searchBar.setFillColor(sf::Color(50, 50, 50));
}

void GUI::updateProgressBar() {
    float progress = player.getPlaybackPosition();
    progressFill.setSize(sf::Vector2f(progress * progressBar.getSize().x, progressFill.getSize().y));
}

void GUI::updateProgressBarPreview(float mouseX) {
    float position = (mouseX - progressBar.getPosition().x) / progressBar.getSize().x;
    position = std::clamp(position, 0.0f, 1.0f);
    progressFill.setSize(sf::Vector2f(position * progressBar.getSize().x, progressFill.getSize().y));
}

void GUI::updateVolumeSliderPreview(float mouseX) {
    float volume = (mouseX - volumeSliderBackground.getPosition().x) / volumeSliderBackground.getSize().x;
    volume = std::clamp(volume, 0.0f, 1.0f);
    volumeFill.setSize(sf::Vector2f(volume * volumeSliderBackground.getSize().x, volumeFill.getSize().y));
    player.setVolume(volume * 100);
}

void GUI::handleHomePageClick(const sf::Event::MouseButtonEvent& mouseButton) {
    const auto& displayFiles = searchQuery.empty() ? player.getMusicFiles() : filteredMusicFiles;
    for (size_t i = 0; i < displayFiles.size(); ++i) {
        sf::Text songText;
        songText.setFont(font);
        songText.setString(getBaseName(displayFiles[i]));
        songText.setCharacterSize(30);
        songText.setFillColor(sf::Color::White);
        songText.setPosition(220.0f, 60.0f + i * 60.0f);

        if (songText.getGlobalBounds().contains(mouseButton.x, mouseButton.y)) {
            if (i != player.getCurrentIndex() || player.getStatus() != sf::SoundSource::Playing) {
                player.playSong(i);
                player.play();
                playPauseButton.setTexture(pauseTexture);
            }
            currentSong = getBaseName(displayFiles[i]);
            currentPage = Page::NowPlaying;
            clickedSongIndex = i;
            break;
        }
    }
}

