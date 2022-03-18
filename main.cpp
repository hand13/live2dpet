#include <iostream>
#include "game.h"
using namespace std;
int main() {
	if(!Game::getInstance()->init()) {
		cout<<"what the fuck"<<endl;
	}
	Game::getInstance()->show();
	Game::getInstance()->loop();
	return 0;
}