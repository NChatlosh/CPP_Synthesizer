#include <SFML\Audio.hpp>
#include <SFML\System.hpp>
#include <math.h>
#include <SFML\Window.hpp>
#include <vector>
#include <functional>
#include "Generator.h"
#include "UI.h"

using namespace sf;

int main()
{

	Generator gen(0);
	UI ui;
	Window window(VideoMode(300, 300), "My window");
	ui.DisplayValues();
	while (window.isOpen()) {
		
		Event event;
		while (window.pollEvent(event))
		{
			
			if (event.type == Event::KeyPressed)
			{
				ui.FindKey(&event);
				ui.DisplayValues();
			}
			
		 }
				}
	
	system("PAUSE");
	return 0;
}
