#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "MusicPlayer.hpp"
#include "GUI.hpp"
#include "Utilities.hpp"
#include <algorithm>

int main() {
    // Get desktop mode and reduce height by a bit to avoid overlapping the taskbar
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode(desktopMode.width - 3, desktopMode.height - 97), "SFML Music Player", sf::Style::Default);

    // Create the music player
    std::vector<std::string> musicFiles = {
        "../Songs/Pink_Floyd_High_Hopes_Official.wav",
        "../Songs/Red_Hot_Chili_Peppers - Otherside.wav",
        "../Songs/Purnimako Chandramalai Cover by Daya Sagar Baral.mp3",
        "../Songs/Queen - I Want To Break Free (Official Video).mp3",
        "../Songs/Kun Faya Kun Full Video Song Rockstar  Ranbir Kapoor  A.R. Rahman, Javed Ali, Mohit Chauhan.mp3",
        "../Songs/The Weeknd - Blinding Lights (Official Video).mp3",
        "../Songs/Kendrick Lamar - Not Like Us.mp3",
        "../Songs/Travis Scott - FE!N ft. Playboi Carti.mp3",
        "../Songs/Creedence Clearwater Revival - Have You Ever Seen The Rain (Official).mp3",
        "../Songs/Ed Sheeran - Perfect .mp3",
        "../Songs/China Company  Bro Sis  Nepali Song.mp3"
    };
    MusicPlayer player(musicFiles);

    GUI gui(window, player);

    // Start the game loop
    while (window.isOpen()) {
        gui.handleEvents();
        gui.update();
        gui.draw();
    }

    return 0;
}