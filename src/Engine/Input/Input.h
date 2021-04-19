#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <map>

class Input {
	public:
		Input();
		~Input();
		void updateEvents(); // capture keyboard & mouse events


		// getters
		bool shouldClose(SDL_Window* window) const; // return if we must stop program
		bool getKey(const SDL_Scancode key) const; // get the correspondant key of the entry
		bool getMouseButton(const Uint8 button) const; // get the current mouse button of the entry
		bool mouseMovement() const; // get mouse movement

		// Coords getters
		int getX() const;
		int getY() const;
		int getXRel() const;
		int getYRel() const;
		int wheelScroll() const; // up or down
		bool getMovementKeyPressed() const;

		// undirect setters
		void showPointer(bool response) const; // show or hide system cursor (default is to hide it)
		void capturePointer(bool response) const; // fixe system cursor to the middle of the window ? default is yes

		// setters
		void resetWheelEvent();

	private:
		SDL_Event m_events;
		bool m_keys[SDL_NUM_SCANCODES];
		bool m_mouseButtons[8];
		int m_wheelEvent;
		std::map<Uint32, bool> m_closeStates;

		int m_x;
		int m_y;
		int m_xRel;
		int m_yRel;
};

#endif // INPUT_H