# 🚴 Bicycle Racing Game – C++ & SDL2

A university mini-project developed in C++ using SDL2.  
The player controls a bicycle and must avoid obstacles to survive as long as possible.

---

## 🎯 Project Objectives

- Apply Object-Oriented Programming (OOP) concepts  
- Use SDL2 for graphics rendering and event handling  
- Implement collision detection and sound effects  
- Structure a C++ project using a clean architecture  

---

## 🛠️ Technologies Used

- C++
- SDL2
- SDL2_image
- SDL2_mixer
- SDL2_ttf
- MinGW
- Visual Studio Code
- Git & GitHub

---

## 🎮 Features

- Real-time player movement  
- Obstacle spawning system  
- Collision detection  
- Score tracking  
- Sound effects integration  

---

## 📸 Game Screenshot

<p align="center">
  <img src="https://raw.githubusercontent.com/mohamedait-abbou/mini-projet-jeu-velo/main/bike_image.jpg" width="600">
</p>

---

## ▶️ Compilation & Execution

```bash
g++ main.cpp -Iinclude -Llib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -o game.exe
./game.exe

📂 Project Structure
assets/   → Images and sounds
headers/  → Header files (.h)
src/      → Source files (.cpp)
include/  → SDL libraries
lib/      → SDL compiled libraries
main.cpp  → Entry point
