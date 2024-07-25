#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "MusicPlayer.hpp"

enum class Page { Home, Playlists, NowPlaying };

class GUI {
public:
    GUI(sf::RenderWindow& window, MusicPlayer& player);
    void handleEvents();
    void update();
    void draw();

private:
    void initializeGUI();
    void handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton);
    void handleMouseMove(const sf::Event::MouseMoveEvent& mouseMove);
    void handleMouseRelease(const sf::Event::MouseButtonEvent& mouseButton);
    void handleTextEntered(const sf::Event::TextEvent& text);
    void drawHomePage();
    void drawPlaylistsPage();
    void drawNowPlayingPage();
    void drawAnimationBars(const sf::Vector2f& position);
    void drawSearchCursor();
    void togglePlayPause();
    void toggleShuffle();
    void toggleLoop();
    void setProgressFromMouseClick(float mouseX);
    void setVolumeFromMouseClick(float mouseX);
    void activateSearchBar();
    void deactivateSearchBar();
    void updateProgressBar();
    void updateProgressBarPreview(float mouseX);
    void updateVolumeSliderPreview(float mouseX);
    void handleHomePageClick(const sf::Event::MouseButtonEvent& mouseButton);

    sf::RenderWindow& window;
    MusicPlayer& player;
    Page currentPage;
    bool isSearchBarActive;
    int clickedSongIndex;

    // SFML objects
    sf::Font font;
    sf::Texture playTexture, pauseTexture, nextTexture, prevTexture, shuffleTexture, loopTexture, volumeTexture;
    sf::Sprite playPauseButton, nextButton, prevButton, shuffleButton, loopButton, volumeButton;
    sf::RectangleShape sidebar, contentArea, controlBar, searchBar, progressBar, progressFill, volumeSliderBackground, volumeFill;
    sf::Text search, searchText;
    std::vector<sf::Text> sidebarTexts;
    std::vector<sf::RectangleShape> animationBars;

    // Other member variables
    sf::Clock clock;
    sf::Clock cursorBlinkClock;
    std::string searchQuery;
    std::string currentSong;
    std::vector<std::string> filteredMusicFiles;

    static constexpr float barMaxHeight = 20.0f;
};