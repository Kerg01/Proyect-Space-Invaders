/*
 * ===================================================================================
 * ARCHIVO: Player.cpp
 * DESCRIPCIÓN: Implementación de la clase Player.
 * Define el comportamiento del jugador (nave).
 * ===================================================================================
 */

#include "Player.hpp" // Inclusión del archivo de cabecera correspondiente.

 /**
  * @brief Constructor de la clase Player.
  * @param x Posición inicial en el eje X.
  * @param y Posición inicial en el eje Y.
  * @param texture Referencia a la textura (spritesheet) principal.
  */
Player::Player(int x, int y, Texture& texture) {
	// 1. Configuración del Sprite:
	sprite.setTexture(texture);
	// Selecciona el gráfico de la nave del jugador.
	sprite.setTextureRect(IntRect(13 * 8 + 8, 7 * 8 + 7, 16, 8));
	sprite.setPosition(x, y); // Posición inicial.
	sprite.setScale(3, 3);    // Escala el sprite.

	// 2. Configuración de la lógica:
	vida = 3;  // Vidas iniciales.
	vel = 5;   // Velocidad de movimiento horizontal (píxeles por fotograma).
}

/**
 * @brief Actualiza el estado del jugador en cada fotograma.
 * Controla el movimiento basado en la entrada del teclado.
 */
void Player::Update() {
	// 1. Comprobar movimiento a la derecha.
	if (Keyboard::isKeyPressed(Keyboard::Right) && sprite.getPosition().x + vel < 552) {
		// Mueve el sprite SÓLO si la tecla está pulsada Y no se sale del borde derecho.
		// 552 = 600 (ancho ventana) - 48 (ancho sprite escalado)
		sprite.move(vel, 0);
	}

	// 2. Comprobar movimiento a la izquierda.
	if (Keyboard::isKeyPressed(Keyboard::Left) && sprite.getPosition().x + vel * -1 > 0) {
		// Mueve el sprite SÓLO si la tecla está pulsada Y no se sale del borde izquierdo.
		sprite.move(vel * -1, 0);
	}

	// 3. Depuración: Muestra las vidas en la consola.
	system("cls"); // Limpia la consola (esto puede ser lento, se recomienda usar sf::Text).
	cout << vida << '\n';// Muestra las vidas restantes.
}

/**
 * @brief Comprueba si el jugador ha presionado la tecla de disparo.
 * @return 'true' si el jugador acaba de disparar, 'false' en caso contrario.
 */
bool Player::Shoot() {
	// Comprueba si se presiona "Espacio" Y si la bandera 'shoot' es falsa.
	// La bandera 'shoot' previene que se dispare continuamente si se deja pulsada la tecla.
	if (Keyboard::isKeyPressed(Keyboard::Space) && !shoot) {
		shoot = true; // Activa la bandera (bloquea el disparo hasta que se suelte la tecla).
		return true;  // Informa a main.cpp que debe crear una bala.
	}
	else if (!Keyboard::isKeyPressed(Keyboard::Space)) {
		// Si el jugador suelta la tecla "Espacio", resetea la bandera.
		shoot = false;// Permite futuros disparos.
	}
	return false; // No se disparó en este fotograma.
}

/**
 * @brief Método "getter" para obtener la posición actual del jugador.
 * @return Un Vector2f con las coordenadas (x, y) del sprite.
 */
Vector2f Player::Pos() {
	return sprite.getPosition();// Devuelve la posición para crear balas en la posición correcta.
}

/**
 * @brief Reduce la vida del jugador.
 * Llamado desde main.cpp cuando una bala enemiga golpea al jugador.
 */
void Player::QuitarVida() {
	vida--; // Decrementa el contador de vidas.
}

/**
 * @brief Comprueba si el jugador sigue vivo.
 * @return 'true' si 'vida' no es 0, 'false' si es 0.
 */
bool Player::Vivo() {
	return (vida != 0); // Devuelve el resultado de la comparación.
}

/**
 * @brief Función de dibujado (heredada de sf::Drawable).
 */
void Player::draw(RenderTarget& rt, RenderStates rs) const {
	// Dibuja el sprite del jugador en la ventana.
	rt.draw(sprite, rs);// Dibuja la nave del jugador.
}// Fin de Player.cpp