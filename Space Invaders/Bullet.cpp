/*
 * ===================================================================================
 * ARCHIVO: Bullet.cpp
 * DESCRIPCIÓN: Implementación de la clase Bullet.
 * Define el comportamiento de las balas (del jugador y de los enemigos).
 * ===================================================================================
 */

#include "Bullet.hpp" // Inclusión del archivo de cabecera correspondiente.

 /**
  * @brief Constructor de la clase Bullet.
  * @param x Posición inicial en el eje X.
  * @param y Posición inicial en el eje Y.
  * @param texture Referencia a la textura (spritesheet) principal.
  * @param intRect El rectángulo específico dentro del spritesheet que usar?esta bala.
  * @param v La velocidad y dirección. (ej. -10 para el jugador, 10 para enemigos).
  */
Bullet::Bullet(int x, int y, Texture& texture, IntRect intRect, int v) {
	// 1. Configuración del Sprite:
	sprite.setTexture(texture);           // Asigna la hoja de sprites.
	sprite.setTextureRect(intRect);       // Selecciona el gráfico EXACTO de la bala.
	sprite.setPosition(x, y);             // Coloca la bala en su posición inicial.
	sprite.setScale(3, 3);                // Escala el sprite (ej. de 8x8 a 24x24).

	// 2. Configuración de la lógica:
	vel = v; // Asigna la velocidad vertical (negativa para subir, positiva para bajar).
}

/**
 * @brief Actualiza el estado de la bala en cada fotograma.
 * Esta función es llamada desde el bucle principal (main.cpp).
 */
void Bullet::Update() {
	// Mueve la bala verticalmente. No hay movimiento horizontal (0).
	sprite.move(0, vel);// Mueve la bala según su velocidad.
}

/**
 * @brief Método "getter" para obtener la posición actual de la bala.
 * @return Un Vector2f con las coordenadas (x, y) del sprite.
 */
Vector2f Bullet::Pos() {
	return sprite.getPosition(); // Devuelve la posición para comprobaciones (colisiones, salir de pantalla).
}

/**
 * @brief Función de dibujado (heredada de sf::Drawable).
 * Permite que SFML dibuje este objeto directamente.
 * @param rt El "RenderTarget" (generalmente la ventana del juego).
 * @param rs Los "RenderStates" (estados de renderizado, no usados aqu?.
 */
void Bullet::draw(RenderTarget& rt, RenderStates rs) const {
	// Dibuja el sprite de la bala en el objetivo (la ventana).
	rt.draw(sprite, rs);// Dibuja la bala en la ventana.
}// Fin de Bullet.cpp
