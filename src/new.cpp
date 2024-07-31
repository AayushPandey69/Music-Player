#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <locale>
#include <random>

// Helper function to get the base name of a file without extension
std::string getBaseName(const std::string& path) {
    std::string filename = path.substr(path.find_last_of("/\\") + 1);
    size_t lastDotPosition = filename.find_last_of(".");
    if (lastDotPosition != std::string::npos) {
        return filename.substr(0, lastDotPosition);
    }
    return filename;
}

// Helper function to wrap text
std::string wrapText(const std::string& text, unsigned int lineLength) {
    std::string result;
    std::string line;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        if (line.length() + word.length() > lineLength + 20) {
            result += line + "\n";
            line = word + " ";
        } else {
            line += word + " ";
        }
    }
    result += line;
    return result;
}

// Enum to represent the current page
enum class Page { Home, Playlists, NowPlaying };

class MusicPlayer {
public:
MusicPlayer(const std::vector<std::string>& files)
    : musicFiles(files), currentIndex(0), isShuffled(false), isLooping(false) {
    // Don't open or play any file initially
    for (size_t i = 0; i < musicFiles.size(); ++i) {
        shuffledIndices.push_back(i);
    }
}

   size_t getCurrentIndex() const {
        return currentIndex;
    }

void play() {
    if (music.getStatus() != sf::SoundSource::Playing) {
        music.play();
    }
}

    void pause() {
        music.pause();
    }

  void next() {
        if (isLooping) {
            // If looping, restart the current song
            if (!music.openFromFile(musicFiles[currentIndex])) {
                std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
            }
        } else {
            // Original next logic
            if (isShuffled) {
                size_t currentShuffledIndex = std::find(shuffledIndices.begin(), shuffledIndices.end(), currentIndex) - shuffledIndices.begin();
                currentShuffledIndex = (currentShuffledIndex + 1) % shuffledIndices.size();
                currentIndex = shuffledIndices[currentShuffledIndex];
            } else {
                currentIndex = (currentIndex + 1) % musicFiles.size();
            }
            if (!music.openFromFile(musicFiles[currentIndex])) {
                std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
            }
        }
        music.setLoop(isLooping);
        music.play();
    }

 void previous() {
        if (isLooping) {
            // If looping, restart the current song
            if (!music.openFromFile(musicFiles[currentIndex])) {
                std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
            }
        } else {
            // Original previous logic
            if (isShuffled) {
                size_t currentShuffledIndex = std::find(shuffledIndices.begin(), shuffledIndices.end(), currentIndex) - shuffledIndices.begin();
                currentShuffledIndex = (currentShuffledIndex - 1 + shuffledIndices.size()) % shuffledIndices.size();
                currentIndex = shuffledIndices[currentShuffledIndex];
            } else {
                currentIndex = (currentIndex - 1 + musicFiles.size()) % musicFiles.size();
            }
            if (!music.openFromFile(musicFiles[currentIndex])) {
                std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
            }
        }
        music.setLoop(isLooping);
        music.play();
    }


    void shufflePlaylist() {
    std::random_shuffle(shuffledIndices.begin(), shuffledIndices.end());
}

void shuffle(bool on) {
    isShuffled = on;
    if (isShuffled) {
        shufflePlaylist();
    }
}

    void loop(bool on) {
        isLooping = on;
        music.setLoop(isLooping);  // Set the loop state of the music
    }

    sf::SoundSource::Status getStatus() const {
        return music.getStatus();
    }

void playSong(size_t index) {
    if (index < musicFiles.size()) {
        currentIndex = index;
        if (isShuffled) {
            shuffledIndices.erase(std::remove(shuffledIndices.begin(), shuffledIndices.end(), index), shuffledIndices.end());
            shuffledIndices.insert(shuffledIndices.begin(), index);
        }
        if (!music.openFromFile(musicFiles[currentIndex])) {
            std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
        }
        music.setLoop(isLooping);
        // Don't call music.play() here
    }
}

    bool getIsShuffled() const {
        return isShuffled;
    }

    bool getIsLooping() const {
        return isLooping;
    }

    std::string getCurrentSong() const {
        return musicFiles[currentIndex];
    }

    float getPlaybackPosition() const {
        return music.getPlayingOffset().asSeconds() / music.getDuration().asSeconds();
    }

    void setPlaybackPosition(float position) {
        music.setPlayingOffset(sf::seconds(position * music.getDuration().asSeconds()));
    }

    float getVolume() const {
        return music.getVolume();
    }

    void setVolume(float volume) {
        music.setVolume(volume);
    }

private:
    std::vector<std::string> musicFiles;
    std::vector<size_t> shuffledIndices;
    sf::Music music;
    size_t currentIndex;
    bool isShuffled;
    bool isLooping;
};

std::vector<int> clickedSongIndex;
std::vector<sf::RectangleShape> animationBars(3);
float barMaxHeight = 20.0f;

std::string searchQuery = "";
std::vector<std::string> filteredMusicFiles;

void filterMusicFiles(const std::vector<std::string>& musicFiles, const std::string& query, std::vector<std::string>& filtered) {
    filtered.clear();
    std::string lowercaseQuery = query;
    std::transform(lowercaseQuery.begin(), lowercaseQuery.end(), lowercaseQuery.begin(),
        [](unsigned char c){ return std::tolower(c); });

    for (const auto& file : musicFiles) {
        std::string lowercaseFile = getBaseName(file);
        std::transform(lowercaseFile.begin(), lowercaseFile.end(), lowercaseFile.begin(),
            [](unsigned char c){ return std::tolower(c); });

        if (lowercaseFile.find(lowercaseQuery) != std::string::npos) {
            filtered.push_back(file);
        }
    }
}

int main() {

    bool isSearchBarActive = false;
    sf::Clock cursorBlinkClock;

    // Get desktop mode and reduce height by a bit to avoid overlapping the taskbar
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width - 3, desktopMode.height - 97), "SFML Music Player", sf::Style::Default);

    // Create the music player
    std::vector<std::string> musicFiles = {
        "../Songs/High Hopes.wav",
        "../Songs/Otherside.wav",
        "../Songs/Purnimako Chandramalai Cover by Daya Sagar Baral.mp3",
        "../Songs/Queen - I Want To Break Free (Official Video).mp3",
        "../Songs/Kun Faya Kun Full Video Song Rockstar  Ranbir Kapoor  A.R. Rahman, Javed Ali, Mohit Chauhan.mp3",
        "../Songs/The Weeknd - Blinding Lights (Official Video).mp3",
        "../Songs/Kendrick Lamar - Not Like Us.mp3",
        "../Songs/fein.mp3",
        "../Songs/Creedence Clearwater Revival - Have You Ever Seen The Rain (Official).mp3"
    };
    MusicPlayer player(musicFiles);

    // Load button images
    sf::Texture playTexture, pauseTexture, nextTexture, prevTexture, shuffleTexture, loopTexture, volumeTexture;
    if (!playTexture.loadFromFile("../Icons/play.png") ||
        !pauseTexture.loadFromFile("../Icons/pause.png") ||
        !nextTexture.loadFromFile("../Icons/skip.png") ||
        !prevTexture.loadFromFile("../Icons/back.png") ||
        !shuffleTexture.loadFromFile("../Icons/shuffle.png") ||
        !loopTexture.loadFromFile("../Icons/loop.png") ||
        !volumeTexture.loadFromFile("../Icons/volume.png")) {
        std::cerr << "Error loading images" << std::endl;
        return -1;
    }

    // Create sprites for buttons
    sf::Sprite playPauseButton(playTexture);  // Use play texture initially
    sf::Sprite nextButton(nextTexture);
    sf::Sprite prevButton(prevTexture);
    sf::Sprite shuffleButton(shuffleTexture);
    sf::Sprite loopButton(loopTexture);
    sf::Sprite volumeButton(volumeTexture);

    // Set button sizes
    float buttonWidth = 80.0f;
    float buttonHeight = 80.0f;

    playPauseButton.setScale(buttonWidth / playPauseButton.getLocalBounds().width, buttonHeight /
     playPauseButton.getLocalBounds().height);
    nextButton.setScale(buttonWidth / nextButton.getLocalBounds().width, buttonHeight / nextButton.getLocalBounds().height);
    prevButton.setScale(buttonWidth / prevButton.getLocalBounds().width, buttonHeight / prevButton.getLocalBounds().height);
    shuffleButton.setScale(buttonWidth / shuffleButton.getLocalBounds().width, buttonHeight / shuffleButton.getLocalBounds().height);
    loopButton.setScale(buttonWidth / loopButton.getLocalBounds().width, buttonHeight / loopButton.getLocalBounds().height);
    volumeButton.setScale(buttonWidth / volumeButton.getLocalBounds().width, buttonHeight / volumeButton.getLocalBounds().height);

    // Sidebar
    sf::RectangleShape sidebar(sf::Vector2f(200.0f, window.getSize().y));
    sidebar.setFillColor(sf::Color(30, 30, 30));

    // Main content area
    sf::RectangleShape contentArea(sf::Vector2f(window.getSize().x - 200.0f, window.getSize().y));
    contentArea.setPosition(200.0f, 0.0f);
    contentArea.setFillColor(sf::Color(40, 40, 40));

    // Media control bar
    sf::RectangleShape controlBar(sf::Vector2f(window.getSize().x, 100.0f));
    controlBar.setPosition(0.0f, window.getSize().y - 100.0f);
    controlBar.setFillColor(sf::Color(20, 20, 20));

    // Set button positions
    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;
    float yPosition = windowHeight - buttonHeight - 10.0f;  // Adjust y position as needed

    playPauseButton.setPosition((windowWidth - buttonWidth) / 2, yPosition);  // Centered
    prevButton.setPosition(playPauseButton.getPosition().x - 2 * buttonWidth, yPosition);  // Left of play/pause
    nextButton.setPosition(playPauseButton.getPosition().x + 2 * buttonWidth, yPosition);  // Right of play/pause
    shuffleButton.setPosition(playPauseButton.getPosition().x - 4 * buttonWidth, yPosition);  // Far left
    loopButton.setPosition(playPauseButton.getPosition().x + 4 * buttonWidth, yPosition);  // Far right
    volumeButton.setPosition(10, yPosition);  // Left side of control bar

    // Search bar
    sf::RectangleShape searchBar(sf::Vector2f(180.0f, 30.0f));
    searchBar.setPosition(10.0f, 10.0f);
    searchBar.setFillColor(sf::Color(50, 50, 50));

    sf::Font font;
    if (!font.loadFromFile("../Fonts/ARIAL.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return -1;
    }

    sf::Text search;
 search.setFont(font);
 search.setString("Search");
 search.setCharacterSize(20);
search.setFillColor(sf::Color(200, 200, 200));
    search.setPosition(15.0f, 15.0f);


    sf::Text searchText;
    searchText.setFont(font);
    searchText.setCharacterSize(20);
    searchText.setFillColor(sf::Color::White);
    searchText.setPosition(15.0f, 15.0f);

    // Sidebar buttons and text
    std::vector<std::string> sidebarOptions = { "Home", "Playlists", "Now\nPlaying" };
    std::vector<sf::Text> sidebarTexts;

    for (size_t i = 0; i < sidebarOptions.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(sidebarOptions[i]);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::White);
        text.setPosition(10.0f, 60.0f + i * 120.0f);  // Increased spacing
        sidebarTexts.push_back(text);
    }

    // Playlist vector
    std::vector<std::string> playlists = {
        "Favorites",
        "Chill Vibes",
        "Workout Mix"
    };

    // Animation state for the now playing page
    sf::CircleShape ball(50.0f);
    ball.setFillColor(sf::Color::Green);
    ball.setPosition((windowWidth - ball.getRadius() * 2) / 2, (windowHeight - ball.getRadius() * 2) / 2);

    sf::Vector2f ballVelocity(0.1f, 0.1f);

   // Progress bar and volume slider
    sf::RectangleShape progressBar(sf::Vector2f(window.getSize().x - 240.0f, 15.0f));
    progressBar.setPosition(220.0f, windowHeight - 135.0f);
    progressBar.setFillColor(sf::Color(60, 60, 60));

    sf::RectangleShape progressFill(sf::Vector2f(0, 15.0f));
    progressFill.setPosition(220.0f, windowHeight - 135.0f);
    progressFill.setFillColor(sf::Color(29, 185, 84)); // Spotify green color

    sf::RectangleShape volumeSliderBackground(sf::Vector2f(120.0f, 10.0f));
    volumeSliderBackground.setPosition(120.0f, windowHeight - 53.0f);
    volumeSliderBackground.setFillColor(sf::Color(60, 60, 60));

    sf::RectangleShape volumeFill(sf::Vector2f(80.0f, 10.0f));
    volumeFill.setPosition(120.0f, windowHeight - 53.0f);
    volumeFill.setFillColor(sf::Color(29, 185, 84)); // Spotify green color

    sf::Clock clock;  // Add this line

    // Initialize the animation bars
    for (int i = 0; i < 3; ++i) {
        animationBars[i].setSize(sf::Vector2f(5.0f, 0.0f));
        animationBars[i].setFillColor(sf::Color(29, 185, 84)); // Spotify green
    }

    int clickedSongIndex = -1; // -1 means no song is clicked

    // Start the game loop
    Page currentPage = Page::Home;
    std::string currentSong = "";

    while (window.isOpen()) {
        // Process events
        sf::Event event;
        bool isDraggingProgressBar = false;
        bool isDraggingVolumeSlider = false;

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
                if (playPauseButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    if (player.getStatus() == sf::SoundSource::Playing) {
                        player.pause();
                        playPauseButton.setTexture(playTexture);
                    }
                    else {
                        player.play();
                        playPauseButton.setTexture(pauseTexture);
                    }
                }
// In the next button handler
else if (nextButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
    player.next();
    playPauseButton.setTexture(pauseTexture);
    currentSong = getBaseName(player.getCurrentSong());
    clickedSongIndex = player.getCurrentIndex(); // Add this line
}

// In the previous button handler
else if (prevButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
    player.previous();
    playPauseButton.setTexture(pauseTexture);
    currentSong = getBaseName(player.getCurrentSong());
    clickedSongIndex = player.getCurrentIndex(); // Add this line
}
                else if (shuffleButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    bool shuffleState = !player.getIsShuffled();
                    player.shuffle(shuffleState);
                    shuffleButton.setColor(shuffleState ? sf::Color::Green : sf::Color::White);
                }
                else if (loopButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    bool loopState = !player.getIsLooping();
                    player.loop(loopState);
                    loopButton.setColor(loopState ? sf::Color::Green : sf::Color::White);
                }
                else if (sidebarTexts[0].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    currentPage = Page::Home;
                }
                else if (sidebarTexts[1].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    currentPage = Page::Playlists;
                }
                else if (sidebarTexts[2].getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    currentPage = Page::NowPlaying;
                }
                // Check if the progress bar is clicked
                else if (progressBar.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && player.getStatus() == sf::SoundSource::Playing) {
                    float position = (event.mouseButton.x - progressBar.getPosition().x) / progressBar.getSize().x;
                    position = std::clamp(position, 0.0f, 1.0f);
                    player.setPlaybackPosition(position);
                }
                // Check if the volume slider is clicked
                else if (volumeSliderBackground.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    float volume = (event.mouseButton.x - volumeSliderBackground.getPosition().x) / volumeSliderBackground.getSize().x;
                    volume = std::clamp(volume, 0.0f, 1.0f);
                    volumeFill.setSize(sf::Vector2f(volume * volumeSliderBackground.getSize().x, volumeFill.getSize().y));
                    player.setVolume(volume * 100);  // Volume ranges from 0 to 100
                }
                else if (searchBar.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
    isSearchBarActive = true;
    currentPage = Page::Home;
     search.setString("");
    searchBar.setFillColor(sf::Color(80, 80, 80)); // Lighter color when active
    cursorBlinkClock.restart();
}
else {
     search.setString("Search");
    isSearchBarActive = false;
    searchBar.setFillColor(sf::Color(50, 50, 50)); // Original color when inactive
}

                // Check if a song is clicked
// Check if a song is clicked
        if (currentPage == Page::Home) {
            const auto& displayFiles = searchQuery.empty() ? musicFiles : filteredMusicFiles;
            for (size_t i = 0; i < displayFiles.size(); ++i) {
                sf::Text songText;
                songText.setFont(font);
                songText.setString(getBaseName(displayFiles[i]));
                songText.setCharacterSize(30);
                songText.setFillColor(sf::Color::White);
                songText.setPosition(220.0f, 60.0f + i * 60.0f);

                if (songText.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
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
            }
            else if (event.type == sf::Event::MouseMoved) {
                if (progressBar.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y) && player.getStatus() == sf::SoundSource::Playing) {
                    float position = (event.mouseMove.x - progressBar.getPosition().x) / progressBar.getSize().x;
                    position = std::clamp(position, 0.0f, 1.0f);
                    progressFill.setSize(sf::Vector2f(position * progressBar.getSize().x, progressFill.getSize().y));
                }
                if (volumeSliderBackground.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) {
                    float volume = (event.mouseMove.x - volumeSliderBackground.getPosition().x) / volumeSliderBackground.getSize().x;
                    volume = std::clamp(volume, 0.0f, 1.0f);
                    volumeFill.setSize(sf::Vector2f(volume * volumeSliderBackground.getSize().x, volumeFill.getSize().y));
                    player.setVolume(volume * 100);  // Volume ranges from 0 to 100
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased) {
                if (progressBar.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) && player.getStatus() == sf::SoundSource::Playing) {
                    float position = (event.mouseButton.x - progressBar.getPosition().x) / progressBar.getSize().x;
                    position = std::clamp(position, 0.0f, 1.0f);
                    player.setPlaybackPosition(position);
                }
            }
         else if (event.type == sf::Event::TextEntered && isSearchBarActive) {
    if (event.text.unicode == 8 && !searchQuery.empty()) { // Backspace
        searchQuery.pop_back();
    } else if (event.text.unicode < 128) {
        searchQuery += static_cast<char>(event.text.unicode);
    }
    searchText.setString(searchQuery);
    filterMusicFiles(musicFiles, searchQuery, filteredMusicFiles);
}
        }

        // Clear the screen
        window.clear();


        // Draw the sidebar
        window.draw(sidebar);
        for (const auto& text : sidebarTexts) {
            window.draw(text);
        }

        // Draw the content area
        window.draw(contentArea);

        // Draw the control bar
        window.draw(controlBar);

        // Draw the buttons
        window.draw(playPauseButton);
        window.draw(nextButton);
        window.draw(prevButton);
        window.draw(shuffleButton);
        window.draw(loopButton);
        window.draw(volumeButton);

        // Draw search bar
        window.draw(searchBar);
        window.draw(search);
        window.draw(searchText);

        if (isSearchBarActive) {
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

        // Update progress bar
        float progress = player.getPlaybackPosition();
        progressFill.setSize(sf::Vector2f(progress * progressBar.getSize().x, progressFill.getSize().y));

        // Draw the home page content
        if (currentPage == Page::Home) {
            const auto& displayFiles = searchQuery.empty() ? musicFiles : filteredMusicFiles;
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
                    for (int j = 0; j < 3; ++j) {
                        float height = (std::sin(clock.getElapsedTime().asSeconds() * 5 + j) + 1) * barMaxHeight / 2;
                        animationBars[j].setSize(sf::Vector2f(5.0f, height));
                        animationBars[j].setPosition(window.getSize().x - 50.0f + j * 10.0f, songButton.getPosition().y + 25.0f - height / 2);
                        window.draw(animationBars[j]);
                    }
                }
            }
        }
        // Draw the playlists page content
        else if (currentPage == Page::Playlists) {
            for (size_t i = 0; i < playlists.size(); ++i) {
                // Draw rectangle behind playlist text
                sf::RectangleShape playlistButton(sf::Vector2f(window.getSize().x - 220.0f, 50.0f));
                playlistButton.setPosition(210.0f, 60.0f + i * 60.0f);
                playlistButton.setFillColor(sf::Color(70, 70, 70));
                playlistButton.setOutlineColor(sf::Color::Black);
                playlistButton.setOutlineThickness(2.0f);

                window.draw(playlistButton);

                // Draw playlist text
                sf::Text playlistText;
                playlistText.setFont(font);
                playlistText.setString(playlists[i]);
                playlistText.setCharacterSize(30);
                playlistText.setFillColor(sf::Color::White);
                playlistText.setPosition(220.0f, 60.0f + i * 60.0f + 10.0f);  // Adjusted positioning
                window.draw(playlistText);
            }
        }
        // Draw the now playing page content
        else if (currentPage == Page::NowPlaying) {
            // Draw song name
            sf::Text songNameText;
            songNameText.setFont(font);
            std::string wrappedSongName = wrapText(currentSong, 30);  // Limit to 30 characters per line
            songNameText.setString(wrappedSongName);
            songNameText.setCharacterSize(50);
            songNameText.setFillColor(sf::Color::White);
            songNameText.setPosition((windowWidth - songNameText.getLocalBounds().width) / 2, 100.0f);
            window.draw(songNameText);

            // Draw larger animation bars
            float largeBarMaxHeight = 100.0f;
            for (int i = 0; i < 20; ++i) {
                float height = (std::sin(clock.getElapsedTime().asSeconds() * 5 + i) + 1) * largeBarMaxHeight / 2;
                sf::RectangleShape bar(sf::Vector2f(15.0f, height));
                bar.setFillColor(sf::Color(29, 185, 84)); // Spotify green
                bar.setPosition(windowWidth / 2 - 200.0f + i * 20.0f, windowHeight / 2 - height / 2);
                window.draw(bar);
            }
        }

        // Draw the progress bar
        window.draw(progressBar);
        window.draw(progressFill);

        // Draw the volume slider
        window.draw(volumeSliderBackground);
        window.draw(volumeFill);

        // Update the window
        window.display();
    }

    return 0;
}