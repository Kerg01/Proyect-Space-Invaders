/*
 * ===================================================================================
 * ARCHIVO: Muro.cpp
 * DESCRIPCIÓN: Implementación de la clase Muro.
 * Define las barreras protectoras (búnkeres).
 * ===================================================================================
 */

#include "Muro.hpp" // Inclusión del archivo de cabecera correspondiente.

 /**
  * @brief Constructor de la clase Muro.
  * @param x Posición X de la esquina superior izquierda del búnker.
  * @param y Posición Y de la esquina superior izquierda del búnker.
  * @param texture Referencia a la textura (spritesheet) principal.
  */
Muro::Muro(int x, int y, Texture& texture) {
	// Un muro (búnker) se compone de 5 "ladrillos" o sprites.
	sprites.resize(5);// Reserva espacio para 5 sprites.

	// 'state' almacena el nivel de daño de cada uno de los 5 ladrillos.
	// Es un 'pair' (par):
	//   - state[i].first: Daño recibido por balas enemigas (up=true).
	//   - state[i].second: Daño recibido por balas del jugador (up=false).
	state.resize(5, { 0, 0 }); // Inicializa los 5 estados a {0, 0} (sin daño).

	// Configura cada uno de los 5 ladrillos.
	for (int i = 0; i < 5; i++) {
		sprites[i].setTexture(texture);// Asigna la hoja de sprites.
		// Selecciona el sprite inicial (sin daño) del spritesheet.
		sprites[i].setTextureRect(IntRect(16 * 8 + 9 + state[i].first * 9, 14 * 8 + 14 + state[i].second * 9, 8, 8));// Gráfico inicial.
		sprites[i].setScale(3, 3);// Escala el sprite (de 8x8 a 24x24).
	}

	// Coloca los 5 ladrillos en forma de "U" invertida o búnker.
	// [0] [1] [2]
	// [3]     [4]
	sprites[0].setPosition(x, y);         // Arriba-Izquierda
	sprites[1].setPosition(x + 24, y);      // Arriba-Centro
	sprites[2].setPosition(x + 48, y);      // Arriba-Derecha
	sprites[3].setPosition(x, y + 24);      // Abajo-Izquierda
	sprites[4].setPosition(x + 48, y + 24); // Abajo-Derecha
}

/**
 * @brief Actualiza el estado visual del muro.
 * Esta función "aplica" el daño almacenado en 'state' a los sprites visuales.
 */
void Muro::Update() {
	for (int i = 0; i < 5; i++) {
		// Vuelve a seleccionar el gráfico del spritesheet basado en el nivel de daño actual.
		// Los sprites de daño están organizados en el spritesheet como una cuadrícula.
		sprites[i].setTextureRect(IntRect(16 * 8 + 9 + state[i].first * 9, 14 * 8 + 14 + state[i].second * 9, 8, 8));// Actualiza el gráfico del ladrillo.
	}// Fin del bucle de actualización de sprites.
}// Fin de Muro::Update()

/**
 * @brief Obtiene las posiciones de los ladrillos "vivos" para la detección de colisiones.
 * @param pos Un vector (pasado por referencia) que se llenará con las posiciones.
 */
void Muro::Pos(vector<pair<int, Vector2f>>& pos) {
	pos.clear(); // Limpia el vector de posiciones de la comprobación anterior.

	for (int i = 0; i < 5; i++) {
		// Esta es la lógica de "vida" del ladrillo.
		// Si la suma de daños (enemigo + jugador) es menor a 5, el ladrillo sigue "vivo".
		if (state[i].first + state[i].second < 5) {
			// Añade el índice (0-4) y la posición (x,y) del ladrillo vivo al vector.
			// main.cpp usará esta lista para comprobar colisiones.
			pos.push_back({ i, sprites[i].getPosition() });// Añade la posición del ladrillo vivo.
		}// Fin del 'if' de vida del ladrillo.
	}// Fin del bucle de obtención de posiciones.
}// Fin de Muro::Pos()

/**
 * @brief Registra una colisión (un impacto de bala) en un ladrillo específico.
 * @param indice El índice (0-4) del ladrillo que fue golpeado.
 * @param up 'true' si fue una bala enemiga, 'false' si fue del jugador.
 */
void Muro::Colision(int indice, bool up) {
	if (up)
		state[indice].first++; // Incrementa el contador de daño enemigo.
	else
		state[indice].second++; // Incrementa el contador de daño del jugador.
}// Fin de Muro::Colision()

/**
 * @brief Función de dibujado (heredada de sf::Drawable).
 */
void Muro::draw(RenderTarget& rt, RenderStates rs) const {
	// Itera sobre los 5 ladrillos...
	for (int i = 0; i < 5; i++) {
		// ...pero solo dibuja los que siguen "vivos" (daño total < 5).
		if (state[i].first + state[i].second < 5) {
			rt.draw(sprites[i], rs);// Dibuja el ladrillo vivo.
		}
	}// Fin del bucle de dibujo de ladrillos.
}// Fin de Muro.cpp