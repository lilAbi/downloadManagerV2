#pragma once

//this should one be passed to a thread

/*
 *  ideally this would be a function object that i can pass to a thread
 *  contains the download metadata
 *  stored in a vector
 *
 */

#include <thread>

class Downloader {
public:
    Downloader() = default;
    ~Downloader() = default;

    void operator()() const {
        std::cout << "Downloading: " << url_ << '\n';
    }

    /*
    //delete copy/move assignment/constructors
    Downloader(const Downloader&) = delete;
    Downloader(Downloader&&) = delete;
    Downloader& operator=(const Downloader&) = delete;
    Downloader& operator=(Downloader&&) = delete;
    */

private:

};



