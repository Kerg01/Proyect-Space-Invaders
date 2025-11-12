/*
 * ===================================================================================
 * ARCHIVO: Player.hpp
 * DESCRIPCIÓN: Archivo de cabecera (definición) de la clase Player.
 * Define la "interfaz" y los miembros de la clase.
 * ===================================================================================
 */

#include <iostream>
#include <SFML/Graphics.hpp> // Necesario para sf::Drawable, sf::Sprite, etc.

using namespace std;
using namespace sf;// Usar el espacio de nombres 'sf' para evitar prefijos 'sf::'.

// --- DEFINICIÓN DE LA CLASE Player ---
// Hereda de sf::Drawable para poder usar 'window.draw(player)'.
class Player : public Drawable {

	// Miembros privados: el estado interno del jugador.
private:
	Sprite sprite; // El sprite visual del jugador.
	int vida;      // Contador de vidas restantes.
	int vel;       // Velocidad de movimiento horizontal.

	// Bandera para controlar el disparo (evita disparos múltiples si se mantiene pulsada la tecla).
	// Se inicializa directamente aquí (una característica moderna de C++).
	bool shoot = false;// true = ya se disparó, false = listo para disparar

	// Miembros públicos: las acciones y consultas que podemos hacerle al jugador.
public:
	/**
	 * @brief Constructor de la clase.
	 */
	Player(int x, int y, Texture& texture);// Parámetros: posición (x, y), textura

	/**
	 * @brief Prototipo de la función que actualiza al jugador (comprueba teclado para moverse).
	 */
	void Update();// Mueve el jugador según la entrada del teclado.

	/**
	 * @brief Prototipo de la función que comprueba si el jugador quiere disparar.
	 * @return 'true' si se acaba de pulsar la tecla de disparo, 'false' si no.
	 */
	bool Shoot();// Comprueba si el jugador ha disparado.

	/**
	 * @brief Prototipo de la función que resta una vida al jugador.
	 */
	void QuitarVida();// Resta una vida al jugador.

	/**
	 * @brief Prototipo del "getter" que comprueba si al jugador le quedan vidas.
	 * @return 'true' si vida > 0, 'false' si no.
	 */
	bool Vivo();// Comprueba si el jugador sigue vivo (vidas > 0).

	/**
	 * @brief Prototipo del "getter" que devuelve la posición actual.
	 */
	Vector2f Pos();// Devuelve la posición actual del sprite.

	/**
	 * @brief Prototipo de la función de dibujado (requerida por sf::Drawable).
	 */
	virtual void draw(RenderTarget& rt, RenderStates rs) const;// Dibuja el sprite en la ventana.
};// Fin de la clase Player