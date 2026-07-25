# Design Doc: `DownloadManagerV2`

---

## 0. TODO

* add filesystem navigator
* Make reactor shutdown and libcurl lifetime fully RAII-safe. 
* Implement completion/error/state handling. 
* Fix EventManager callback locking and subscriber lifetimes. (need to confirm)
* Download Progress not reflecting true size

---

## 1. Objective

Multi-Platform download manager that supports reliable HTTP/HTTPS downloads,using SDL3.

## 1. Build Instructions

1. setup conan:
   conan install . --build=missing -s:h build_type=Debug --output-folder=.out

2. 