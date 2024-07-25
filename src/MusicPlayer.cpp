#include "MusicPlayer.hpp"
#include <iostream>

MusicPlayer::MusicPlayer(const std::vector<std::string>& files)
    : musicFiles(files), currentIndex(0), isShuffled(false), isLooping(false) {
    for (size_t i = 0; i < musicFiles.size(); ++i) {
        shuffledIndices.push_back(i);
    }
    // Do not load or play any music here
}

void MusicPlayer::play() {
    if (music.getStatus() != sf::SoundSource::Playing) {
        music.play();
    }
}

void MusicPlayer::pause() {
    music.pause();
}

void MusicPlayer::next() {
    if (isShuffled) {
        auto it = std::find(shuffledIndices.begin(), shuffledIndices.end(), currentIndex);
        if (it != shuffledIndices.end()) {
            currentIndex = shuffledIndices[(std::distance(shuffledIndices.begin(), it) + 1) % shuffledIndices.size()];
        }
    } else {
        currentIndex = (currentIndex + 1) % musicFiles.size();
    }

    if (!music.openFromFile(musicFiles[currentIndex])) {
        std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
    }
    music.setLoop(isLooping);
    if (music.getStatus() == sf::SoundSource::Playing) {
        music.play();
    }
}

void MusicPlayer::previous() {
    if (!isLooping) {
        if (isShuffled) {
            size_t currentPosition = std::find(shuffledIndices.begin(), shuffledIndices.end(), currentIndex) - shuffledIndices.begin();
            currentIndex = shuffledIndices[(currentPosition - 1 + shuffledIndices.size()) % shuffledIndices.size()];
        } else {
            currentIndex = (currentIndex - 1 + musicFiles.size()) % musicFiles.size();
        }
    }
    
    if (!music.openFromFile(musicFiles[currentIndex])) {
        std::cerr << "Error loading music file: " << musicFiles[currentIndex] << std::endl;
    }
    music.setLoop(isLooping);
    music.play();
}

void MusicPlayer::shufflePlaylist() {
    std::random_shuffle(shuffledIndices.begin(), shuffledIndices.end());
}

void MusicPlayer::shuffle(bool on) {
    isShuffled = on;
    if (isShuffled) {
        shufflePlaylist();
    }
}

void MusicPlayer::loop(bool on) {
    isLooping = on;
    music.setLoop(isLooping);
}

void MusicPlayer::playSong(size_t index) {
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
        // Note: We're not calling play() here
    }
}

sf::SoundSource::Status MusicPlayer::getStatus() const {
    return music.getStatus();
}

size_t MusicPlayer::getCurrentIndex() const {
    return currentIndex;
}

bool MusicPlayer::getIsShuffled() const {
    return isShuffled;
}

bool MusicPlayer::getIsLooping() const {
    return isLooping;
}

std::string MusicPlayer::getCurrentSong() const {
    return musicFiles[currentIndex];
}

float MusicPlayer::getPlaybackPosition() const {
    return music.getPlayingOffset().asSeconds() / music.getDuration().asSeconds();
}

void MusicPlayer::setPlaybackPosition(float position) {
    music.setPlayingOffset(sf::seconds(position * music.getDuration().asSeconds()));
}

float MusicPlayer::getVolume() const {
    return music.getVolume();
}

void MusicPlayer::setVolume(float volume) {
    music.setVolume(volume);
}