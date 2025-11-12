/*
 * ===================================================================================
 * ARCHIVO: Enemie.hpp
 * DESCRIPCIÓN: Archivo de cabecera (definición) de la clase Enemie.
 * Define la "interfaz" y los miembros de la clase.
 * ===================================================================================
 */

#include <iostream>
#include <SFML/Graphics.hpp> // Necesario para sf::Drawable, sf::Sprite, etc.

using namespace std;
using namespace sf;// Usar el espacio de nombres 'sf' para evitar prefijos 'sf::'.

// --- DEFINICIÓN DE LA CLASE Enemie ---
// Hereda de sf::Drawable para poder usar 'window.draw(enemie)'.
class Enemie : public Drawable {

	// Miembros privados: gestionan el estado interno del enemigo.
private:
	Sprite sprite;   // El sprite visual del enemigo.
	int vel;         // Velocidad de movimiento horizontal.
	int state;       // El fotograma actual de la animación (0 o 1).
	int cadencia;    // El "límite" del temporizador para moverse (cuanto más bajo, más rápido).
	int timer;       // Un contador de fotogramas que se compara con 'cadencia'.
	Vector2f point;  // La coordenada (x,y) del sprite de este enemigo EN EL SPRITESHEET.

	// Miembros públicos: las acciones que el enemigo puede realizar.
public:
	/**
	 * @brief Constructor de la clase.
	 */
	Enemie(int x, int y, Texture& texture, Vector2f p);// Parámetros: posición (x, y), textura, punto en spritesheet

	/**
	 * @brief Prototipo de la función que actualiza el movimiento y la animación.
	 */
	void Update();// Mueve el enemigo y actualiza la animación.

	/**
	 * @brief Prototipo de la función que invierte la velocidad y baja al enemigo.
	 */
	void ChangeDir();// Invierte la dirección y baja al enemigo.

	/**
	 * @brief Prototipo del "getter" para la posición.
	 */
	Vector2f Pos();// Devuelve la posición actual del sprite.

	/**
	 * @brief Prototipo de la función que "acelera" al enemigo (reduce 'cadencia').
	 */
	void AumentarCadencia();// Reduce 'cadencia' para aumentar la velocidad.

	/**
	 * @brief Prototipo de la función de dibujado (requerida por sf::Drawable).
	 */
	virtual void draw(RenderTarget& rt, RenderStates rs) const;// Dibuja el sprite en la ventana.
};// Fin de la clase Enemie
