#include <GL/gl.h>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <valarray>
#include <iostream>
#include <cmath>

class bichito{
	public:
		bichito():health(10),position(2){
			sprite.SetPosition(0,0);
			sprite.SetSize( sf::Vector2f(20,20) );
			sprite.SetOutlineColor(sf::Color::Red);
			sprite.SetFillColor(sf::Color::Blue);
			sprite.SetOrigin(10,10);
		}
		void move( std::valarray<double> delta){
			double half_size[] = {sprite.GetSize().x/2,sprite.GetSize().y/2};

			position += delta;
			for(size_t K=0; K<position.size(); ++K){
				if(position[K]-half_size[K]<0) position[K] = half_size[K];
				if(position[K]+half_size[K]>400) position[K] = 400-half_size[K];
			}
			sprite.SetPosition( position[0], position[1] );
			//std::cerr << position[0] << ';' << position[1] << ' ';
		}
		std::valarray<double> position;
		sf::RectangleShape sprite;
		int health;
};

bool colision(sf::RectangleShape a, sf::RectangleShape b){
	sf::Vector2f dim = (a.GetSize()+b.GetSize())/2.f;
	sf::Vector2f dif = a.GetPosition()-b.GetPosition();
	dif.x = std::abs(dif.x);
	dif.y = std::abs(dif.y);
	return dif.x<dim.x and dif.y<dim.y;
}

class terreno{
public:
	terreno(){
		sprite.SetPosition(200,200);
		sprite.SetOrigin(200,50);
		sprite.SetSize( sf::Vector2f(400,100) );
		sprite.SetFillColor(sf::Color::Green);
	}
	sf::RectangleShape sprite;
};

class pelota{
public:
	sf::Vector2f position, velocity;
	pelota(){
		sprite.SetPosition(position);
		sprite.SetRadius(10);
		sprite.SetOrigin(5,5);
		sprite.SetFillColor(sf::Color::Yellow);
	}
	pelota(sf::Vector2f p, sf::Vector2f v): position(p), velocity(v){
		sprite.SetPosition(position);
		sprite.SetRadius(10);
		sprite.SetOrigin(5,5);
		sprite.SetFillColor(sf::Color::White);

		std::cerr << velocity.x << ' ' << velocity.y << '\n';
	}
	void update(int time){
		position += velocity;
		sprite.SetPosition(position);
	}
	void texture( sf::Texture *texture ){
		sprite.SetTexture(texture);
	}
	void Draw(sf::RenderWindow &w){
		w.Draw(sprite);
	}
	sf::CircleShape sprite;
};


int main(int argc, char **argv){
	sf::RenderWindow window( sf::VideoMode(400,400,32), "pool_friction");
	window.SetFramerateLimit(60);
	bichito player;
	terreno obstaculo;
	pelota bala;

	sf::Image image;
	image.LoadFromFile("pelota.png");
	sf::Texture texture;
	texture.LoadFromImage(image);
	bala.texture( &texture );

	while( window.IsOpen() ){
		window.Clear( sf::Color::Black );
		sf::Event event;
		while( window.PollEvent(event) ){
			switch(event.Type){
				case sf::Event::Closed:
					window.Close();
					break;
				case sf::Event::MouseMoved:
				{
					//std::cerr << event.MouseMove.X << ';' << event.MouseMove.Y << ' ';
					sf::RectangleShape line;
					line.SetPosition(player.sprite.GetPosition().x, player.sprite.GetPosition().y);
					line.SetSize( sf::Vector2f(50,5) );
					line.SetFillColor(sf::Color::Green);

					line.Rotate( 
						180+std::atan2(
							player.sprite.GetPosition().y-event.MouseMove.Y,
							player.sprite.GetPosition().x-event.MouseMove.X
						)*180/M_PI
					);
					/*glColor3f(0,1,0);
					glBegin(GL_LINE);{
						glVertex2f(player.sprite.GetPosition().x, player.sprite.GetPosition().y);
						glVertex2f(event.MouseMove.X, event.MouseMove.Y);
					}glEnd();*/
					window.Draw(line);
				}
					break;
				case sf::Event::MouseButtonPressed:
					bala = pelota( 
						player.sprite.GetPosition(), 
						sf::Vector2f( 
							(event.MouseButton.X-player.sprite.GetPosition().x)/2,
							(event.MouseButton.Y-player.sprite.GetPosition().y)/2)
						);
					bala.texture( &texture );
					break;
				/*case sf::Event::KeyPressed:
					//std::cerr << "Key pressed ";
					switch( event.Key.Code ){
						case sf::Keyboard::Left:
							delta[0] = -1;
							delta[1] = 0;
							break;
						case sf::Keyboard::Right:
							delta[0] = 1;
							delta[1] = 0;
							break;
						case sf::Keyboard::Up:
							delta[0] = 0;
							delta[1] = -1;
							break;
						case sf::Keyboard::Down:
							delta[0] = 0;
							delta[1] = 1;
							break;
					}
					player.move( 5.*delta );*/
			}
		}
		std::valarray<double> delta(2);
		if( sf::Keyboard::IsKeyPressed(sf::Keyboard::Left) ){
			delta[0] = -1;
			delta[1] = 0;
		}
		if( sf::Keyboard::IsKeyPressed(sf::Keyboard::Left) and
			sf::Keyboard::IsKeyPressed(sf::Keyboard::Down) ) {
			delta[0] = -1;
			delta[1] = 1;
		}
		if( sf::Keyboard::IsKeyPressed(sf::Keyboard::Right) ){
			delta[0] = 1;
			delta[1] = 0;
		}
		if( sf::Keyboard::IsKeyPressed(sf::Keyboard::Up) ){
			delta[0] = 0;
			delta[1] = -1;
		}
		if( sf::Keyboard::IsKeyPressed(sf::Keyboard::Down) ){
			delta[0] = 0;
			delta[1] = 1;
		}
		player.move( 5.*delta );
		if( colision(player.sprite, obstaculo.sprite) ){
			std::cerr << 'X';
		}

		bala.update(1);
		window.Draw(obstaculo.sprite);
		window.Draw(player.sprite);
		bala.Draw(window);
		window.Display();
	}
	return 0;
}


