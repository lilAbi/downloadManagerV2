#pragma once

#include <thread>
//this should one be passed to a thread

/*
 *  ideally this would be a function object that i can pass to a thread
 *  contains the download metadata
 *  stored in a vector
 *
 *  use an event queue to communicate between DownloadController and thread for new task or pauses/cancels
 *
 */


class Downloader {
public:
    Downloader() = default;
    ~Downloader() = default;

    void operator()(std::stop_token stop_token) const;


    /*
    //delete copy/move assignment/constructors
    Downloader(const Downloader&) = delete;
    Downloader(Downloader&&) = delete;
    Downloader& operator=(const Downloader&) = delete;
    Downloader& operator=(Downloader&&) = delete;
    */

private:

    //shared event queue

};



