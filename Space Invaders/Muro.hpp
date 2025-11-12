/*
 * ===================================================================================
 * ARCHIVO: Muro.hpp
 * DESCRIPCIÓN: Archivo de cabecera (definición) de la clase Muro.
 * Define la "interfaz" y los miembros de la clase.
 * ===================================================================================
 */

#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>            // Necesario porque usamos std::vector.

using namespace std;
using namespace sf;// Usar el espacio de nombres 'sf' para evitar prefijos 'sf::'.

// --- DEFINICIÓN DE LA CLASE Muro ---
// Hereda de sf::Drawable para poder usar 'window.draw(muro)'.
class Muro : public Drawable {

	// Miembros privados: el estado interno del muro.
private:
	// Un muro no es un solo sprite, sino un conjunto de ellos (5 en este caso).
	vector<Sprite> sprites;// Los 5 sprites que componen el muro.

	// Un vector paralelo que almacena el estado de daño de CADA sprite.
	// El 'pair<int, int>' guarda el daño de balas enemigas y del jugador por separado.
	vector<pair<int, int>> state;// Estado de daño de cada uno de los 5 sprites.

	// Miembros públicos: cómo interactuamos con el muro.
public:
	/**
	 * @brief Constructor de la clase.
	 */
	Muro(int x, int y, Texture& texture);// Parámetros: posición (x, y), textura

	/**
	 * @brief Prototipo de la función que "llena" un vector con las posiciones de los ladrillos vivos.
	 * Se pasa por referencia (&) para ser eficiente y modificar el original.
	 */
	void Pos(vector<pair<int, Vector2f>>& pos);// Llena 'pos' con índices y posiciones de ladrillos vivos.

	/**
	 * @brief Prototipo de la función que actualiza el gráfico de los ladrillos según su daño.
	 */
	void Update();// Actualiza los sprites según el estado de daño.

	/**
	 * @brief Prototipo de la función que registra un golpe en un ladrillo específico.
	 */
	void Colision(int indice, bool up);// Registra un impacto en el ladrillo 'indice'. 'up' indica si es bala enemiga (true) o del jugador (false).

	/**
	 * @brief Prototipo de la función de dibujado (requerida por sf::Drawable).
	 * Esta función dibujará solo los ladrillos "vivos".
	 */
	virtual void draw(RenderTarget& rt, RenderStates rs) const;// Dibuja los sprites del muro en la ventana.
};// Fin de la clase Muro
