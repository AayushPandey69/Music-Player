#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>

// Helper function to get the base name of a file
std::string getBaseName(const std::string& path) {
    return path.substr(path.find_last_of("/\\") + 1);
}

// Enum to represent the current page
enum class Page { Home, Playlists, NowPlaying };

class MusicPlayer {
public:
    MusicPlayer(const std::vector<std::string>& files)
        : musicFiles(files), currentIndex(0), isShuffled(false), isLooping(false) {
        if (!music.openFromFile(musicFiles[currentIndex])) {
            std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
        }
    }

    void play() {
        music.play();
    }

    void pause() {
        music.pause();
    }

    void next() {
        currentIndex = (currentIndex + 1) % musicFiles.size();
        if (!music.openFromFile(musicFiles[currentIndex])) {
            std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
        }
        music.play();
    }

    void previous() {
        currentIndex = (currentIndex - 1 + musicFiles.size()) % musicFiles.size();
        if (!music.openFromFile(musicFiles[currentIndex])) {
            std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
        }
        music.play();
    }

    void shuffle(bool on) {
        isShuffled = on;
    }

    void loop(bool on) {
        isLooping = on;
    }

    sf::SoundSource::Status getStatus() const {
        return music.getStatus();
    }

    void playSong(size_t index) {
        if (index < musicFiles.size()) {
            currentIndex = index;
            if (!music.openFromFile(musicFiles[currentIndex])) {
                std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
            }
            music.play();
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

private:
    std::vector<std::string> musicFiles;
    sf::Music music;
    size_t currentIndex;
    bool isShuffled;
    bool isLooping;
};

int main() {
    // Get desktop mode and reduce height by a bit to avoid overlapping the taskbar
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width-3, desktopMode.height-97), "SFML Music Player", sf::Style::Default);

    // Create the music player
    std::vector<std::string> musicFiles = {
        "Songs/High Hopes.wav",
        "Songs/Otherside.wav",
        "Songs/Purnimako Chandramalai Cover by Daya Sagar Baral.mp3",
        "Songs/Queen - I Want To Break Free (Official Video).mp3",
        "Songs/Kun Faya Kun Full Video Song Rockstar  Ranbir Kapoor  A.R. Rahman, Javed Ali, Mohit Chauhan.mp3",
        "Songs/The Weeknd - Blinding Lights (Official Video).mp3",
        "Songs/Kendrick Lamar - Not Like Us.mp3",
        "Songs/fein.mp3",
        "Songs/Creedence Clearwater Revival - Have You Ever Seen The Rain (Official).mp3"
    };
    MusicPlayer player(musicFiles);

    // Load button images
    sf::Texture playTexture, pauseTexture, nextTexture, prevTexture, shuffleTexture, loopTexture, volumeTexture;
    if (!playTexture.loadFromFile("Icons/play.png") ||
        !pauseTexture.loadFromFile("Icons/pause.png") ||
        !nextTexture.loadFromFile("Icons/skip.png") ||
        !prevTexture.loadFromFile("Icons/back.png") ||
        !shuffleTexture.loadFromFile("Icons/shuffle.png") ||
        !loopTexture.loadFromFile("Icons/loop.png") ||
        !volumeTexture.loadFromFile("Icons/volume.png")) {
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

    playPauseButton.setScale(buttonWidth / playPauseButton.getLocalBounds().width, buttonHeight / playPauseButton.getLocalBounds().height);
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
    volumeButton.setPosition(windowWidth - 100, yPosition);  // Right corner

    // Search bar
    sf::RectangleShape searchBar(sf::Vector2f(180.0f, 30.0f));
    searchBar.setPosition(10.0f, 10.0f);
    searchBar.setFillColor(sf::Color(50, 50, 50));

    // Sidebar buttons and text
    sf::Font font;
    if (!font.loadFromFile("Fonts/ARIAL.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return -1;
    }

    std::vector<std::string> sidebarOptions = { "Home", "Playlists", "Now Playing" };
    std::vector<sf::Text> sidebarTexts;

    for (size_t i = 0; i < sidebarOptions.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(sidebarOptions[i]);
        text.setCharacterSize(40);
        text.setFillColor(sf::Color::White);
        text.setPosition(10.0f, 60.0f + i * 100.0f);  // Adjusted spacing
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

    // Start the game loop
    Page currentPage = Page::Home;
    std::string currentSong = "";

    while (window.isOpen()) {
        // Process events
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
                else if (nextButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    player.next();
                    playPauseButton.setTexture(pauseTexture);
                }
                else if (prevButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    player.previous();
                    playPauseButton.setTexture(pauseTexture);
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
                // Check if a song is clicked
                if (currentPage == Page::Home) {
                    for (size_t i = 0; i < musicFiles.size(); ++i) {
                        sf::Text songText;
                        songText.setFont(font);
                        songText.setString(getBaseName(musicFiles[i]));  // Only show song names
                        songText.setCharacterSize(30);
                        songText.setFillColor(sf::Color::White);
                        songText.setPosition(220.0f, 60.0f + i * 60.0f);  // Adjusted spacing for song items

                        // Check if mouse click is within the song text bounds
                        if (songText.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            player.playSong(i);  // Play the selected song
                            playPauseButton.setTexture(pauseTexture);  // Change the button texture to pause
                            currentSong = getBaseName(musicFiles[i]);
                            currentPage = Page::NowPlaying;  // Change to Now Playing page
                            break;  // Exit the loop after playing the song
                        }
                    }
                }
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

        // Draw the home page content
        if (currentPage == Page::Home) {
            for (size_t i = 0; i < musicFiles.size(); ++i) {
                // Draw rectangle behind song text, full width of the window
                sf::RectangleShape songButton(sf::Vector2f(window.getSize().x - 210.0f, 50.0f));
                songButton.setPosition(210.0f, 60.0f + i * 60.0f);
                songButton.setFillColor(sf::Color(70, 70, 70));
                songButton.setOutlineColor(sf::Color::Black);
                songButton.setOutlineThickness(2.0f);

                // Draw shadow for song button
                sf::RectangleShape songShadow = songButton;
                songShadow.move(5.0f, 5.0f);
                songShadow.setFillColor(sf::Color(50, 50, 50, 100));  // Semi-transparent shadow

                window.draw(songShadow);
                window.draw(songButton);

                // Draw song text
                sf::Text songText;
                songText.setFont(font);
                songText.setString(getBaseName(musicFiles[i]));  // Only show song names
                songText.setCharacterSize(30);
                songText.setFillColor(sf::Color::White);
                songText.setPosition(220.0f, 60.0f + i * 60.0f + 10.0f);  // Adjusted position within the button
                window.draw(songText);
            }
        }

        // Draw the playlists page content
        else if (currentPage == Page::Playlists) {
            for (size_t i = 0; i < playlists.size(); ++i) {
                // Draw rectangle behind playlist text
                sf::RectangleShape playlistButton(sf::Vector2f(500.0f, 50.0f));
                playlistButton.setPosition(210.0f, 60.0f + i * 60.0f);
                playlistButton.setFillColor(sf::Color(70, 70, 70));
                playlistButton.setOutlineColor(sf::Color::Black);
                playlistButton.setOutlineThickness(2.0f);

                // Draw shadow for playlist button
                sf::RectangleShape playlistShadow = playlistButton;
                playlistShadow.move(5.0f, 5.0f);
                playlistShadow.setFillColor(sf::Color(50, 50, 50, 100));  // Semi-transparent shadow

                window.draw(playlistShadow);
                window.draw(playlistButton);

                // Draw playlist text
                sf::Text playlistText;
                playlistText.setFont(font);
                playlistText.setString(playlists[i]);
                playlistText.setCharacterSize(30);
                playlistText.setFillColor(sf::Color::White);
                playlistText.setPosition(220.0f, 60.0f + i * 60.0f);  // Adjusted spacing for playlist items
                window.draw(playlistText);
            }
        }

        // Draw the now playing page content
        else if (currentPage == Page::NowPlaying) {
            // Draw song name
            sf::Text songNameText;
            songNameText.setFont(font);
            songNameText.setString(currentSong);
            songNameText.setCharacterSize(50);
            songNameText.setFillColor(sf::Color::White);
            songNameText.setPosition((windowWidth - songNameText.getLocalBounds().width) / 2, 100.0f);
            window.draw(songNameText);

            // Draw a bouncing ball animation
            ball.move(ballVelocity);
            if (ball.getPosition().x <= 0 || ball.getPosition().x + ball.getRadius() * 2 >= windowWidth) {
                ballVelocity.x = -ballVelocity.x;
            }
            if (ball.getPosition().y <= 0 || ball.getPosition().y + ball.getRadius() * 2 >= windowHeight) {
                ballVelocity.y = -ballVelocity.y;
            }
            window.draw(ball);
        }

        // Update the window
        window.display();
    }

    return 0;
}
