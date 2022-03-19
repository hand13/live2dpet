#include <iostream>
#include "game.h"
using namespace std;
int main() {
	CoInitialize(NULL);
	if(!Game::getInstance()->init()) {
		cout<<"what the fuck"<<endl;
	}
	Game::getInstance()->show();
	Game::getInstance()->loop();
	CoUninitialize();
	return 0;
}