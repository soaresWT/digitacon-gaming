#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <functional>

struct Pincel {
    sf::Font font;
    sf::Text text;
    sf::RenderWindow& window;
    
    Pincel(sf::RenderWindow& window) : window{window} {
        if(!font.loadFromFile("Roboto/rb.ttf"))
            std::cout << "Erro no carregamento da fonte!" << std::endl;
        text.setFont(font);
    }

    void write(std::string str, int x, int y, int size, sf::Color color) {
        text.setString(str);
        text.setCharacterSize(size);
        text.setPosition(x, y);
        text.setFillColor(color);
        window.draw(text);

    }

};

struct Bubble {
    int x;
    int y;
    char letter;
    int speed;
    bool alive { true };
    
    static const int radius { 15 };

    Bubble(int x, int y, char letter, int speed): x{x}, y{y}, letter{letter}, speed{speed} {
}

    void update() {
        this->y += this-> speed;
    }

	void draw(sf::RenderWindow& window) {
    sf::CircleShape circle(Bubble::radius);
    circle.setPosition(x, y);
    circle.setFillColor(sf::Color::Transparent);
    window.draw(circle);

    static Pincel pencil(window);
    pencil.write(std::string(1, letter), x + 0.5 * Bubble::radius, y, Bubble::radius * 1.5, sf::Color::White);
	}

};

struct Board {
    sf::RenderWindow& window;
    std::vector<Bubble>  bubbles;
    int hits { 0 };
    int misses { 0 };
	Pincel pencil;
    

  
    
    Board(sf::RenderWindow& window) : window {window}, pencil {window} {
        bubbles.push_back(Bubble(100, 100, 'A', 1));
        bubbles.push_back(Bubble(200, 100, 'B', 2));
        bubbles.push_back(Bubble(300, 100, 'C', 3));
    }

    void update() {
        if(this->check_new_bubble())
    		this->add_new_bubble();
		for(Bubble& bubble : bubbles) {
    		bubble.update();
		}
		this->mark_outside_bubbles();
		this->remove_dead_bubbles();
    }

	void remove_dead_bubbles() {
    	std::vector<Bubble> vivas;
    	for (Bubble& bubble : bubbles) {
        	if(bubble.alive) {
            vivas.push_back(bubble);
        	}
    	}
    	this->bubbles = vivas;
	}

	void mark_outside_bubbles() {
    	for(Bubble& bubble : bubbles) {
        	if (bubble.y + 2 * Bubble::radius > (int) this->window.getSize().y) {
            	if(bubble.alive) {
					bubble.alive = false;
            		this->misses++;
				}
        	}
    	}
	}

	void mark_by_hit( char letter) {
    	for (Bubble& bubble : bubbles) {
        	if (bubble.letter == letter) {
            	bubble.alive = false;
            	this->hits++;
            	break;
        	}
    	}
	}

	bool check_new_bubble() {
		static const int new_bubble_timeout { 30 };
    	static int new_bubble_timer { 0 };

    	new_bubble_timer--;
    	if (new_bubble_timer <= 0) {
        new_bubble_timer = new_bubble_timeout;
        return true;
    	}
    	return false;
	}

	void add_new_bubble() {
    	int x = rand() % ((int) this->window.getSize().x - 2 * Bubble::radius);
    	int y = - 2 * Bubble::radius;
    	int speed = rand() % 5 + 1;
    	char letter = rand() % 26 + 'A';
    	bubbles.push_back(Bubble(x, y, letter, speed));
	}

    void draw(){
       
		pencil.write("Hits: " + std::to_string(this->hits) + " Misses: " + std::to_string(this->misses), 10, 10, 20, sf::Color::White);
        pencil.write("Size: " + std::to_string(this->bubbles.size()), 10, 30, 20, sf::Color::White);
		for(Bubble& bubble : bubbles) {
            bubble.draw(window);
        }
	}
};

struct Game {
	sf::RenderWindow window;
	Board board;
	std::function<void()> on_update;
    sf::Texture background_texture;
    sf::Sprite background;
	Game() : window(sf::VideoMode(800, 600), "Bubbles"), board(window) {
		if(!background_texture.loadFromFile("assets/background.jpg")){
            std::cout << "Erro ao carregar a textura!" << std::endl;
        }
        background.setTexture(background_texture);
        auto dim = background.getLocalBounds();
        background.setScale(800/dim.width,600/dim.height);

        this->on_update = [&]() {
    		this->gameplay();
		};
		window.setFramerateLimit(30);
	}
	
	void process_events() {
		sf::Event event;
		    while (window.pollEvent(event)) {
			    if (event.type == sf::Event::Closed) {
				    window.close();
			    } else if(event.type == sf::Event::TextEntered) {
    				char code = static_cast<char>(event.text.unicode);
    				code = toupper(code);
    				board.mark_by_hit(code);
				}
		    }
    }


	void gameplay() {
		board.update();
        window.clear(sf::Color::Black);
        window.draw(background);
		board.draw();
		window.display();
		if(board.misses > 3) {
    		this->on_update = [&]() {
        		this->gameover();
    		};
		}
	}

	void gameover() {
		static Pincel pincel(window);
		window.clear(sf::Color::Black);
		pincel.write("Gamer Over", 280, 200, 50, sf::Color::White);
       
		window.display();
     
        
       
	}

	void main_loop() {
		while (window.isOpen()) {
			process_events();
			on_update();
		}
	}
};


int main() 
{

    
       
    

    Game game;
    game.main_loop();
	return 0;
}
//g++ -c main.cpp && g++ main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system && ./sfml-app