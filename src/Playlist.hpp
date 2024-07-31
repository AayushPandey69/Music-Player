// Playlist.hpp
#pragma once
#include <string>
#include <vector>

class Playlist {
public:
    Playlist(const std::string& name);
    void addSong(const std::string& songPath);
    void removeSong(const std::string& songPath);
    const std::string& getName() const;
    const std::vector<std::string>& getSongs() const;

private:
    std::string name;
    std::vector<std::string> songs;
};

// Playlist.cpp
#include "Playlist.hpp"
#include <algorithm>

Playlist::Playlist(const std::string& name) : name(name) {}

void Playlist::addSong(const std::string& songPath) {
    if (std::find(songs.begin(), songs.end(), songPath) == songs.end()) {
        songs.push_back(songPath);
    }
}

void Playlist::removeSong(const std::string& songPath) {
    auto it = std::find(songs.begin(), songs.end(), songPath);
    if (it != songs.end()) {
        songs.erase(it);
    }
}

const std::string& Playlist::getName() const {
    return name;
}

const std::vector<std::string>& Playlist::getSongs() const {
    return songs;
}