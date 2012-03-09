#include <GL/gl.h>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <valarray>
#include <vector>
#include <iostream>
#include <cmath>

class objeto{
public:
	typedef std::valarray<double> vector;

	objeto();
	objeto(const vector&, const vector&);
	vector position() const;
	vector dimension() const;
protected:
	vector position_, dimension_;
};

class bichito: public objeto{
public:
	bichito();
	bichito(const vector &pos, const vector &dim);
	vector move( const vector &delta);
private:
	int health;
};

bool colision(sf::RectangleShape a, sf::RectangleShape b){
	sf::Vector2f dim = (a.GetSize()+b.GetSize())/2.f;
	sf::Vector2f dif = a.GetPosition()-b.GetPosition();
	dif.x = std::abs(dif.x);
	dif.y = std::abs(dif.y);
	return dif.x<dim.x and dif.y<dim.y;
}

class pelota: public objeto{
public:
	pelota(const vector &pos, const vector &vel);
	pelota();
	pelota::vector update(double time);
private:
	vector velocity_;
};

class dibujo{
public:
	dibujo( objeto &obj, sf::Shape &sprite): sprite(sprite), obj(obj) {
		sync();
		sprite.SetOrigin(0.5,0.5);
		sprite.SetScale( obj.dimension()[0], obj.dimension()[1] );
	}
	void Draw(sf::RenderWindow &w){
		sync();
		w.Draw(sprite);
	}
private:
	void sync(){
		objeto::vector pos = obj.position(), dim = obj.dimension()/2.;
		//sprite.SetOrigin(dim[0], dim[1]);
		sprite.SetPosition(pos[0], pos[1]);
	}
	sf::Shape &sprite;
	const objeto &obj;
};


int main(int argc, char **argv){
	sf::RenderWindow window( sf::VideoMode(400,400,32), "pool_friction");
	window.SetFramerateLimit(60);
	bichito player;
	objeto obstaculo;
	pelota bala;

	std::vector<pelota> bullets;

	sf::Image pelota_image;
	sf::Texture pelota_texture;
	pelota_image.LoadFromFile("pelota.png");
	pelota_texture.LoadFromImage(pelota_image);

	sf::CircleShape bullet_sprite(1);
	sf::RectangleShape player_sprite( sf::Vector2f(1,1) );

	bullet_sprite.SetFillColor( sf::Color::White );
	player_sprite.SetFillColor( sf::Color::Green );

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
					line.SetPosition( player.position()[0], player.position()[1] );
					line.SetSize( sf::Vector2f(150,2) );
					line.SetFillColor(sf::Color::Green);

					line.Rotate( 
						180+std::atan2(
							player.position()[0]-event.MouseMove.Y,
							player.position()[1]-event.MouseMove.X
						)*180/M_PI
					);
					window.Draw(line);
				}
					break;
				case sf::Event::MouseButtonPressed:
				{
					pelota::vector pos = player.position(), direction(2);
					direction[0] = event.MouseButton.X-pos[0];
					direction[1] = event.MouseButton.Y-pos[1];

					double norm = (direction*direction).sum();

					bullets.push_back(pelota( pos, direction/sqrt(norm)));
					break;
				}
			}
		}
		std::valarray<double> delta(2);
		if( sf::Keyboard::IsKeyPressed(sf::Keyboard::Left) ){
			delta[0] = -1;
		}
		if( sf::Keyboard::IsKeyPressed(sf::Keyboard::Right) ){
			delta[0] = 1;
		}
		if( sf::Keyboard::IsKeyPressed(sf::Keyboard::Up) ){
			delta[1] = -1;
		}
		if( sf::Keyboard::IsKeyPressed(sf::Keyboard::Down) ){
			delta[1] = 1;
		}
		player.move( 5.*delta );
		/*if( colision(player.sprite, obstaculo.sprite) ){
			std::cerr << 'X';
		}*/

		for(size_t K=0; K<bullets.size(); ++K){
			bullets[K].update(8);
			dibujo(bullets[K], player_sprite).Draw(window);
		}
		//dibujo(player, player_sprite).Draw(window);
		dibujo(player, bullet_sprite).Draw(window);

		window.Display();
	}
	return 0;
}

objeto::objeto():position_(2), dimension_(2){}
objeto::objeto(const vector &pos, const vector &dim):position_(pos), dimension_(dim){}
objeto::vector objeto::position() const{
	return position_;
}
objeto::vector objeto::dimension() const{
	return dimension_;
}
bichito::bichito():health(10){
	position_[0] = position_[1] = 0;
	dimension_[0] = dimension_[1] = 20;
}
bichito::bichito(const vector &pos, const vector &dim): objeto(pos,dim), health(10){}
bichito::vector bichito::move( const vector &delta){
	position_ += delta;
	return position_;
}
pelota::pelota(const vector &pos, const vector &vel): objeto(pos, vector(2)), velocity_(vel){
	dimension_[0] = dimension_[1] = 10;
}
pelota::pelota(): velocity_(2){
	dimension_[0] = dimension_[1] = 10;
}
pelota::vector pelota::update(double time){
	position_ += time*velocity_;
	return position_;
}

