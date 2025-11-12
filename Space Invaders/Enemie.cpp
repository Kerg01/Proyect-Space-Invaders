/*
 * ===================================================================================
 * ARCHIVO: Enemie.cpp
 * DESCRIPCIÓN: Implementación de la clase Enemie.
 * Define el comportamiento de los aliens enemigos.
 * ===================================================================================
 */

#include "Enemie.hpp" // Inclusión del archivo de cabecera correspondiente.

 /**
  * @brief Constructor de la clase Enemie.
  * @param x Posición inicial en el eje X.
  * @param y Posición inicial en el eje Y.
  * @param texture Referencia a la textura (spritesheet) principal.
  * @param p Un Vector2f que indica la esquina superior izquierda del sprite *dentro del spritesheet*.
  */
Enemie::Enemie(int x, int y, Texture& texture, Vector2f p) {
	// 'point' almacena la coordenada (ej. 0,0) del primer fotograma de animación
	// de este tipo de enemigo en el spritesheet.
	point = p;// Almacena la posición del sprite en el spritesheet.

	// 1. Configuración del Sprite:
	sprite.setTexture(texture);// Asigna la hoja de sprites.
	// Asigna el primer fotograma de animación (estado 0).
	sprite.setTextureRect(IntRect(point.x, point.y, 8, 8));// Selecciona el gráfico EXACTO del enemigo.
	sprite.setPosition(x, y); // Posición en la pantalla.
	sprite.setScale(3, 3);    // Escala el sprite.

	// 2. Configuración de la lógica:
	state = 0;   // Estado inicial de la animación (fotograma 0).
	timer = 0;   // Temporizador interno para controlar la velocidad de movimiento.
	vel = 24;    // Velocidad horizontal (positiva = derecha).

	// 'cadencia' aqu?no es de disparo, sino la *velocidad de movimiento*.
	// Representa cuántos fotogramas del juego deben pasar antes de que el enemigo se mueva.
	// Un número más bajo significa que se mueven más rápido.
	cadencia = 150;// Velocidad inicial de movimiento.
}

/**
 * @brief Actualiza el estado del enemigo en cada fotograma.
 * Controla el movimiento y la animación.
 */
void Enemie::Update() {
	// Comprueba si el temporizador ha alcanzado el límite de 'cadencia'.
	if (timer >= cadencia) {
		// 1. Mover el enemigo horizontalmente.
		sprite.move(vel, 0);// Mueve el sprite según su velocidad.

		// 2. Actualizar la animación.
		state++;      // Avanza al siguiente fotograma.
		state %= 2;   // Alterna el estado entre 0 y 1 (para 2 fotogramas de animación).

		// 3. Aplicar el nuevo fotograma de animación al sprite.
		// Si state=0, usa point.x. Si state=1, usa point.x + 9.
		sprite.setTextureRect(IntRect(point.x + state * 9, point.y, 8, 8));// Cambia el fotograma.

		// 4. Reiniciar el temporizador.
		timer = 0;
	}// Fin del 'if(timer >= cadencia)'
	// Incrementa el temporizador en cada fotograma del juego.
	timer++;// Incrementa el temporizador.
}// Fin de Enemie::Update()

/**
 * @brief Cambia la dirección de movimiento.
 * Llamado desde main.cpp cuando el bloque de enemigos toca un borde.
 */
void Enemie::ChangeDir() {
	vel *= -1; // Invierte la velocidad horizontal (ej. 24 a -24).

	// Mueve el enemigo hacia abajo una cantidad fija (el ancho de un enemigo).
	sprite.move(0, abs(vel));// Mueve hacia abajo (usa abs para evitar negativos).
}// Fin de Enemie::ChangeDir()

/**
 * @brief Método "getter" para obtener la posición actual del enemigo.
 * @return Un Vector2f con las coordenadas (x, y) del sprite.
 */
Vector2f Enemie::Pos() {
	return sprite.getPosition();// Devuelve la posición para comprobaciones (bordes, colisiones).
}// Fin de Enemie::Pos()

/**
 * @brief Aumenta la velocidad de movimiento del enemigo.
 * Llamado desde main.cpp cada vez que un enemigo es destruido.
 */
void Enemie::AumentarCadencia() {
	// Reduce el contador de 'cadencia'. Al ser más bajo, el 'if(timer>=cadencia)'
	// se cumplir?más a menudo, haciendo que el enemigo se mueva más rápido.
	cadencia--;// Aumenta la velocidad de movimiento.
}// Fin de Enemie::AumentarCadencia()

/**
 * @brief Función de dibujado (heredada de sf::Drawable).
 */
void Enemie::draw(RenderTarget& rt, RenderStates rs) const {
	// Dibuja el sprite del enemigo en la ventana.
	rt.draw(sprite, rs);// Dibuja el enemigo en la ventana.
}// Fin de Enemie.cpp
