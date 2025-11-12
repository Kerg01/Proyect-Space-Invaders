/*
 * ===================================================================================
 * ARCHIVO: Bullet.hpp
 * DESCRIPCIÓN: Archivo de cabecera (definición) de la clase Bullet.
 * Define la "interfaz" y los miembros de la clase.
 * ===================================================================================
 */

#include <iostream>          // Incluido (aunque no es estrictamente necesario en este .hpp)
#include <SFML/Graphics.hpp> // Necesario para sf::Drawable, sf::Sprite, sf::Texture, etc.

using namespace std;
using namespace sf;// Usar el espacio de nombres 'sf' para evitar prefijos 'sf::'.

// --- DEFINICIÓN DE LA CLASE Bullet ---
// "public Drawable" significa que esta clase hereda de sf::Drawable.
// Esto nos permite pasar objetos 'Bullet' directamente a 'window.draw()'.
// A cambio, nos "obliga" a implementar la función 'draw'.
class Bullet : public Drawable {

	// Los miembros privados solo son accesibles desde dentro de la clase (en Bullet.cpp).
private:
	Sprite sprite; // El objeto de SFML que contiene la textura, posición, escala, etc.
	int vel;       // La velocidad vertical de la bala (positiva o negativa).

	// Los miembros públicos son la "interfaz" de la clase.
	// Pueden ser llamados desde cualquier parte (como en main.cpp).
public:
	/**
	 * @brief Constructor de la clase. Se llama al crear un nuevo objeto Bullet.
	 */
	Bullet(int x, int y, Texture& texture, IntRect intRect, int v);// Parámetros: posición (x, y), textura, rectángulo del sprite, velocidad

	/**
	 * @brief Prototipo de la función que actualiza la lógica (movimiento).
	 */
	void Update();// Mueve la bala según su velocidad.

	/**
	 * @brief Prototipo de la función "getter" que devuelve la posición actual.
	 */
	Vector2f Pos();// Devuelve la posición actual del sprite.

	/**
	 * @brief Prototipo de la función de dibujado requerida por 'sf::Drawable'.
	 * 'virtual' permite que clases hijas la sobrescriban (aunque aqu?no hay).
	 * 'const' significa que esta función "promete" no modificar el objeto.
	 */
	virtual void draw(RenderTarget& rt, RenderStates rs) const;// Dibuja el sprite en la ventana.
};// Fin de la clase Bullet
