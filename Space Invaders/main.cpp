#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <time.h>      // Para 'srand' (semilla aleatoria)
#include <limits>    // Para 'numeric_limits' (validación de entrada)
#include <cstdlib>     // Para 'system' (comandos de consola)
#include <fstream>     // Para 'ifstream' y 'ofstream' (manejo de archivos)
#include <algorithm>   // Para 'sort' (ordenar puntuaciones)
#include "Player.hpp"   // Clase que maneja al jugador
#include "Bullet.hpp"   // Clase que maneja las balas
#include "Enemie.hpp"   // Clase que maneja a los enemigos
#include "Muro.hpp"     // Clase que maneja los muros de protección
#include "include/json.hpp" // Librería externa para manejar archivos JSON
// Colores para la consola 
#define BRIGHT_BLACK   "\033[90m"
#define BRIGHT_RED     "\033[91m"
#define BRIGHT_GREEN   "\033[92m"
#define BRIGHT_YELLOW  "\033[93m"
#define BRIGHT_BLUE    "\033[94m"
#define BRIGHT_MAGENTA "\033[95m"
#define BRIGHT_CYAN    "\033[96m"
#define BRIGHT_WHITE   "\033[97m"

using namespace std;
using namespace sf;

// Alias para la librería JSON de nlohmann
using json = nlohmann::json;

// --- Prototipos de Funciones ---

// Funciones de actualización del juego (lógica principal)
void UpdatePlayer(Player& player, Bullet& bulletPlayer); // Mover jugador y disparar
void UpdateBulletPlayer(Bullet& bulletPlayer, vector<vector<Enemie>>& enemies);// Mover bala jugador y colisiones
void UpdateEnemies(vector<vector<Enemie>>& enemies);// Mover enemigos y disparar
void UpdateBulletsEnemies(Player& player);// Mover balas enemigas y colisiones
void UpdateMuro(vector<Muro>& muro, Bullet& bulletPlayer);// Colisiones de balas con muros

// Funciones de menú y gestión
bool showTerminalMenu();    // Muestra el menú principal y devuelve 'true' si se va a jugar
void showScoreking();       // Muestra la lista de puntuaciones
void deleteMyScore();       // Permite al usuario eliminar sus puntuaciones
void showScorekingMenu();   // Muestra el submenú de Scoreking (ver, borrar, volver)
void saveScore(bool won);   // Guarda la puntuación del jugador en un archivo JSON
void showInstructions();    // Muestra la pantalla de instrucciones
void showCredits();         // Muestra la pantalla de créditos
int showPostGameMenu();     // Muestra el menú después de ganar o perder

// --- Variables Globales ---
Texture spritesheet; // Textura principal que contiene todos los gráficos del juego

int timer = 0;     // Temporizador general, usado para la cadencia de disparo enemiga
int cadencia = 125; // Define la velocidad de disparo de los enemigos (menos es más rápido)

int dirEnemies = 1; // Dirección de movimiento de los enemigos (1 = derecha, -1 = izquierda)
int maxX, minX;     // Coordenadas X del enemigo más a la derecha y más a la izquierda
int cantEnemies;    // Contador total de enemigos (usado para la condición de victoria)
int enemiesKilled = 0; // Contador de enemigos eliminados (para la puntuación)

vector<Bullet> bulletsEnemies; // Vector que almacena todas las balas enemigas activas

// Vector temporal para almacenar las posiciones de las partes de un muro (para colisiones)
vector<pair<int, Vector2f>> posicionMuro;// (índice de parte, posición (x, y))

Vector2f sectionSpritesheet; // Coordenada (x, y) para seleccionar el sprite de enemigo correcto

// Rectángulos de colisión para los diferentes objetos del juego
IntRect playerRect;
IntRect bulletRect;// Bala del jugador
IntRect enemieRect;// Enemigo
IntRect muroRect;// Parte de muro

// Flag (bandera) que indica si la bala del jugador está activa en pantalla
bool bulletActive = false;// true = activa, false = inactiva


int main() {
	// --- FORZAR CODIFICACIÓN UTF-8 EN LA CONSOLA ---
	// Esto arregla los caracteres extraños como 'á' en la consola de Windows
	// El "> nul" oculta el mensaje "Active code page: 65001"
	system("chcp 65001 > nul");

	// --- BUCLE PRINCIPAL DE LA APLICACIÓN ---
	// Este bucle mantiene el programa activo hasta que el usuario elige "Salir".
	while (true)
	{
		// 1. MOSTRAR MENÚ PRINCIPAL
		// 'showTerminalMenu()' pausa el programa y espera la selección del usuario.
		// Devuelve 'false' si el usuario elige "Salir".
		if (!showTerminalMenu()) {
			cout << BRIGHT_BLUE << "5Saliendo del juego...\n" << BRIGHT_WHITE;
			return 0; // Termina la aplicación
		}

		// 2. INICIALIZACIÓN Y RESETEO DEL JUEGO
		// Cargar la textura
		if (!spritesheet.loadFromFile("spritesheet.png")) {
			cout << BRIGHT_RED << "Error al cargar la textura 'spritesheet.png'\n" << BRIGHT_WHITE;
			return 1; // Error crítico, salir
		}

		// --- RESETEO DE VARIABLES GLOBALES ---
		// ¡VITAL para que cada partida nueva empiece de cero!
		timer = 0;// Temporizador general
		cadencia = 125;// Velocidad de disparo enemiga
		dirEnemies = 1;// Dirección inicial de enemigos
		enemiesKilled = 0;// Resetear contador de enemigos eliminados
		bulletsEnemies.clear(); // Limpia las balas de la partida anterior
		bulletActive = false;// La bala del jugador empieza inactiva

		// 0 = jugando, 1 = ganó, 2 = perdió
		int gameResult = 0;// Variable que almacena el resultado del juego
		// ------------------------------------

		// Crear al jugador
		Player player(288, 555, spritesheet);// Posición inicial del jugador (288, 555)

		// Crear una bala "placeholder" para el jugador (se reemplazará al disparar)
		Bullet bulletPlayer(0, 0, spritesheet, IntRect(0, 0, 0, 0), 0);// Posición y tamaño irrelevantes

		// Crear la matriz 2D de enemigos (7 filas, 12 columnas)
		vector<vector<Enemie>> enemies(7, vector<Enemie>(12, Enemie(0, 0, spritesheet, Vector2f(0, 0))));// Inicializar con enemigos "placeholder"

		// Inicializar cada enemigo en la matriz
		for (int i = 0; i < (int)enemies.size(); i++) { // 'i' es la fila
			for (int j = 0; j < (int)enemies[i].size(); j++) { // 'j' es la columna

				// Seleccionar el sprite correcto según la fila
				if (i == 0) { // Fila 0 (la más lejana)
					sectionSpritesheet = Vector2f(0, 0);// Coordenada del sprite en el spritesheet
				}
				else if (i < 3) { // Filas 1 y 2
					sectionSpritesheet = Vector2f(0, 9 + (8 * 4 + 4));// Coordenada del sprite en el spritesheet
				}
				else if (i < 5) { // Filas 3 y 4
					sectionSpritesheet = Vector2f(0, 18 + (8 * 4 + 4) * 2);// Coordenada del sprite en el spritesheet
				}
				else if (i < 7) { // Filas 5 y 6 (las más cercanas)
					sectionSpritesheet = Vector2f(0, 27 + (8 * 4 + 4) * 3);// Coordenada del sprite en el spritesheet
				}

				// Crear el enemigo en su posición de cuadrícula
				enemies[i][j] = Enemie(j * 30 + 24, i * 30 + 24, spritesheet, sectionSpritesheet);// Posición (x, y) del enemigo
			}
		}

		// Crear los 3 muros
		vector<Muro> muro(3, Muro(0, 0, spritesheet));// Inicializar con muros "placeholder"
		for (int i = 0; i < 3; i++) {
			muro[i] = Muro(70 + 200 * i, 460, spritesheet);// Posición (x, y) de cada muro
		}

		// Crear la ventana del juego
		RenderWindow window(VideoMode(600, 600), "Space Invaders");// Título de la ventana
		window.setFramerateLimit(60); // Limitar a 60 FPS

		// 3. BUCLE DEL JUEGO (SFML)
		// Este bucle se ejecuta 60 veces por segundo (o hasta el límite)
		while (window.isOpen()) {

			// Procesar eventos (input, cerrar ventana, etc.)
			Event event;// Objeto para almacenar eventos
			while (window.pollEvent(event)) {
				// Manejar cierre manual de ventana (clic en la 'X')
				if (event.type == Event::Closed) {
					gameResult = 0; // 0 significa que el usuario cerró, no ganó ni perdió
					window.close();// Cerrar la ventana
				}
			}

			// Si el juego ya terminó (se estableció gameResult), salimos del bucle
			if (gameResult != 0) {
				break;// Salir del bucle 'while (window.isOpen())'
			}

			// --- Actualizar la lógica del juego ---
			UpdatePlayer(player, bulletPlayer);     // Mover jugador y disparar
			UpdateBulletPlayer(bulletPlayer, enemies); // Mover bala jugador y colisiones
			UpdateEnemies(enemies);                 // Mover enemigos y disparar
			UpdateBulletsEnemies(player);             // Mover balas enemigas y colisiones
			UpdateMuro(muro, bulletPlayer);         // Colisiones de balas con muros

			// --- LÓGICA DE FIN DE JUEGO ---

			// Condición de Pérdida 1: El jugador muere
			if (!player.Vivo()) {
				gameResult = 2; // 2 = Perdió
				window.close(); // Cerrar ventana INMEDIATAMENTE
				break;          // Salir del bucle 'while (window.isOpen())'
			}

			// Condición de Pérdida 2: Enemigos llegan abajo
			if (gameResult == 0) // Chequear solo si el juego no ha terminado
			{
				for (int i = 0; i < (int)enemies.size(); i++) {
					for (int j = 0; j < (int)enemies[i].size(); j++) {
						if (enemies[i][j].Pos().y >= 480) { // Si un enemigo toca la zona de muros
							gameResult = 2; // 2 = Perdió
							window.close(); // Cerrar ventana INMEDIATAMENTE
							break;// Salir del bucle interior
						}
					}
					if (gameResult != 0) break; // Salir del bucle exterior
				}
			}

			// Condición de Victoria: No quedan enemigos
			if (gameResult == 0) // Chequear solo si el juego no ha terminado
			{
				cantEnemies = 0; // Resetear contador
				// Contar cuántos enemigos quedan vivos
				for (int i = 0; i < (int)enemies.size(); i++) {
					cantEnemies += (int)enemies[i].size(); // Sumar el tamaño de cada vector fila
				}

				if (cantEnemies == 0) {
					gameResult = 1; // 1 = Ganó
					window.close(); // Cerrar ventana INMEDIATAMENTE
					break;// Salir del bucle 'while (window.isOpen())'
				}
			}

			// --- Dibujado (Render) ---
			window.clear(); // Limpiar el fotograma anterior

			// Dibujar balas enemigas
			for (int i = 0; i < (int)bulletsEnemies.size(); i++) {
				window.draw(bulletsEnemies[i]);// Dibujar cada bala enemiga
			}

			// Dibujar bala del jugador (si está activa)
			if (bulletActive) window.draw(bulletPlayer);// Dibujar bala del jugador

			// Dibujar a todos los enemigos
			for (int i = 0; i < (int)enemies.size(); i++) {
				for (int j = 0; j < (int)enemies[i].size(); j++) {
					window.draw(enemies[i][j]);// Dibujar cada enemigo
				}
			}

			// Dibujar los 3 muros
			for (int i = 0; i < 3; i++) window.draw(muro[i]);// Dibujar cada muro

			// Dibujar al jugador (al final para que esté por encima de todo)
			window.draw(player);// Dibujar al jugador

			// Mostrar el fotograma dibujado en la ventana
			window.display();
		} // --- FIN DE 'while (window.isOpen())' ---


		// 4. MOSTRAR RESULTADO Y GUARDAR PUNTUACIÓN
		// Esta sección se ejecuta DESPUÉS de que la ventana del juego se cerró.
		system("cls"); // Limpia la terminal

		if (gameResult == 1) // Si ganó
		{
			cout << BRIGHT_YELLOW << "============================\n";
			cout << "	GANASTE\n";
			cout << "============================\n" << BRIGHT_WHITE;
			saveScore(true); // Guardar puntuación (indicando victoria)
		}
		else if (gameResult == 2) // Si perdió
		{
			cout << BRIGHT_RED << "============================\n";
			cout << "	PERDISTE\n";
			cout << "============================\n" << BRIGHT_WHITE;
			saveScore(false); // Guardar puntuación (indicando derrota)
		}
		// Si gameResult == 0 (cierre manual), no hace nada y vuelve al menú.


		// 5. MENÚ POST-JUEGO
		// Solo mostrar el menú post-juego si se ganó o perdió (gameResult != 0)
		bool inPostGameMenu = (gameResult != 0);// true = en menú post-juego, false = salir al menú principal
		while (inPostGameMenu)// Bucle del menú post-juego
		{
			int postChoice = showPostGameMenu(); // Esperar selección

			switch (postChoice) {
			case 1: // "Volver al menú principal"
				inPostGameMenu = false; // Sale del bucle post-juego
				// El 'while(true)' principal se reiniciará, volviendo al menú
				break;
			case 2: // "Scoreking"
				showScorekingMenu(); // Muestra el submenú de puntuaciones
				// Permanece en el bucle post-juego
				break;
			case 3: // "Salir"
				cout << BRIGHT_BLUE << "Saliendo del juego...\n" << BRIGHT_WHITE;
				return 0; // Sale de toda la aplicación
			}
		} // --- FIN DE 'while (inPostGameMenu)' ---

	} // --- FIN DE 'while (true)' (Bucle de aplicación) ---

	return 0; // Fin del 'main'
}

// -------------------------------------------------------------
// FUNCIÓN PARA GUARDAR LA PUNTUACIÓN EN JSON
// -------------------------------------------------------------
void saveScore(bool won) {

	// 1. Calcular Puntuación
	int finalScore = enemiesKilled * 10;// Cada enemigo vale 10 puntos
	cout << "\nEnemigos eliminados: " << BRIGHT_CYAN << enemiesKilled << "\n" << BRIGHT_WHITE;
	cout << "Puntuacion obtenida: " << BRIGHT_CYAN << finalScore << "\n" << BRIGHT_WHITE;

	// No preguntar por nombre si la puntuación es 0 Y perdió
	if (finalScore == 0 && !won) {
		cout << BRIGHT_YELLOW "\nNo se guardara la puntuacion ya que es 0.\n";
		cout << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
		cin.get();// Pausa para que el jugador lea el mensaje
		return;// Salir de la función sin guardar
	}

	// 2. Pedir Nombre al Jugador
	string playerName;// Variable para almacenar el nombre del jugador
	cout << BRIGHT_GREEN << "\nIngresa tu nombre para el Scoreking: " << BRIGHT_WHITE;

	// 'getline' lee la línea completa (incluyendo espacios)
	// Funciona gracias al 'cin.ignore' en los menús
	getline(cin, playerName);// Leer el nombre del jugador

	// Asignar "Anonimo" si el jugador no ingresa nada
	if (playerName.empty()) {
		playerName = "Anonimo";// Nombre por defecto
	}

	// 3. Cargar el JSON existente (o inicializar uno nuevo)
	json scoreking;// Variable JSON para almacenar las puntuaciones
	ifstream inputFile("Scoreking.json"); // Abrir archivo para leer
	if (inputFile.is_open()) {
		try {
			// Intentar "parsear" (interpretar) el contenido del archivo como JSON
			scoreking = json::parse(inputFile);// Cargar el JSON desde el archivo
		}
		catch (const json::parse_error& e) {
			// Si el archivo está corrupto (JSON mal formado), se reinicia
			cerr << "ADVERTENCIA: Scoreking.json corrupto. Reiniciando historial.\n";
			scoreking = json::array(); // Crear un array JSON vacío
		}
		inputFile.close();// Cerrar el archivo después de leer
	}
	else {
		// Si el archivo no existe, empezar con un array JSON vacío
		scoreking = json::array();// Crear un array JSON vacío
	}

	// 4. Crear el nuevo registro de puntuación
	json newScore = {
		{"nombre", playerName},
		{"puntuacion", finalScore}
	};// Crear un objeto JSON con el nombre y la puntuación

	// 5. Agregar el nuevo registro al array JSON
	scoreking.push_back(newScore);// Agregar al final del array

	// 6. Guardar el JSON actualizado
	ofstream outputFile("Scoreking.json"); // Abrir archivo para escribir (sobrescribe)
	if (outputFile.is_open()) {
		// 'dump(4)' guarda el JSON con formato legible (4 espacios de indentación)
		outputFile << scoreking.dump(4);// Escribir el JSON en el archivo
		outputFile.close();// Cerrar el archivo después de escribir
		cout << BRIGHT_GREEN << "\nPuntuacion guardada exitosamente \n" << BRIGHT_WHITE;
	}
	else {
		// Error si no se puede escribir (ej. falta de permisos)
		cerr << "\nERROR: No se pudo abrir Scoreking.json para escritura. Verifica permisos.\n";
	}

	// Pausa para que el jugador lea el mensaje
	cout << "Presiona ENTER para continuar...";
	cin.get();// Esperar un ENTER
}

/**
 * @brief Muestra el menú principal en la terminal.
 * @return true si el usuario quiere jugar, false si quiere salir.
 */
bool showTerminalMenu() {
	while (true) { // Bucle infinito hasta que se retorne un valor
		system("cls"); // Limpiar la consola
		cout << BRIGHT_CYAN << "============================\n";
		cout << BRIGHT_MAGENTA << "	SPACE INVADERS\n";
		cout << BRIGHT_CYAN << "============================\n" << BRIGHT_WHITE;
		cout << BRIGHT_GREEN << "1. Empezar a Jugar\n" << BRIGHT_WHITE;
		cout << "2. Scoreking\n";
		cout << "3. Instrucciones\n";
		cout << "4. Creditos\n";
		cout << BRIGHT_RED << "5. Salir\n" << BRIGHT_WHITE;
		cout << BRIGHT_CYAN << "============================\n" << BRIGHT_WHITE;
		cout << BRIGHT_YELLOW << "Elige una opcion: " << BRIGHT_WHITE;

		int choice = 0;// Variable para almacenar la elección del usuario
		cin >> choice;// Leer la elección del usuario

		// --- Validación de entrada ---
		if (cin.fail()) { // Si el usuario no ingresó un número (ej. "abc")
			cin.clear(); // Limpiar la bandera de error de 'cin'
			// Ignorar todo lo que está en el buffer de entrada hasta el salto de línea
			cin.ignore(numeric_limits<streamsize>::max(), '\n');// Limpiar el buffer
			cout << BRIGHT_RED << "Entrada invalida. Por favor, escribe un numero.\n" << BRIGHT_WHITE;
			cout << BRIGHT_YELLOW << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
			cin.get(); // Esperar un ENTER
		}
		else {
			// Limpiar el buffer INMEDIATAMENTE después de leer el número
			// Esto es VITAL para que 'getline' funcione correctamente más tarde
			cin.ignore(numeric_limits<streamsize>::max(), '\n');// Limpiar el buffer

			switch (choice) {
			case 1:
				return true; // Indica "Jugar"

			case 2:
				showScorekingMenu(); // Llama al submenú de puntuaciones
				break; // Vuelve al 'while(true)' de este menú

			case 3:
				showInstructions();// Muestra las instrucciones
				break; // Vuelve al 'while(true)' de este menú

			case 4:
				showCredits();// Muestra los créditos
				break; // Vuelve al 'while(true)' de este menú

			case 5:
				return false; // Indica "Salir"

			default:
				cout << BRIGHT_RED << "Opcion invalida. Por favor, elige un numero del menu.\n" << BRIGHT_WHITE;
				cout << BRIGHT_YELLOW << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
				cin.get();// Esperar un ENTER
				break; // Vuelve al 'while(true)' de este menú
			}
		}
	}
}

/**
 * @brief Actualiza la lógica del jugador (movimiento y disparo).
 */
void UpdatePlayer(Player& player, Bullet& bulletPlayer) {
	player.Update(); // Maneja el input (izquierda/derecha) y actualiza la posición

	// Si el jugador presiona 'disparar' Y la bala no está activa...
	if (player.Shoot() && !bulletActive) {
		// Crear una nueva bala en la posición del jugador
		Bullet bullet(player.Pos().x + 24, player.Pos().y + 12, spritesheet, IntRect(13 * 8 + 16, 6 * 8 + 6, 8, 8), -10);// Posición (x, y), sprite y velocidad (hacia arriba)
		// Asignarla a la variable 'bulletPlayer' (que está en 'main')
		bulletPlayer = bullet;// Reemplazar la bala "placeholder"
		// Marcar la bala como activa
		bulletActive = true;// La bala ahora está en pantalla
	}
}

/**
 * @brief Actualiza la bala del jugador (movimiento y colisión con enemigos).
 */
void UpdateBulletPlayer(Bullet& bulletPlayer, vector<vector<Enemie>>& enemies) {
	// Solo hacer algo si la bala está activa
	if (bulletActive) {
		bulletPlayer.Update(); // Mover la bala (hacia arriba)

		// Desactivar la bala si sale por la parte superior de la pantalla
		if (bulletPlayer.Pos().y < -24) bulletActive = false;// Fuera de pantalla

		// Definir el rectángulo de colisión de la bala
		bulletRect = IntRect(bulletPlayer.Pos().x, bulletPlayer.Pos().y, 3, 8);// Tamaño de la bala

		// --- Comprobar colisión con todos los enemigos ---
		for (int i = 0; i < (int)enemies.size(); i++) { // Iterar filas
			for (int j = 0; j < (int)enemies[i].size(); j++) { // Iterar columnas
				// Definir el rectángulo del enemigo actual
				enemieRect = IntRect(enemies[i][j].Pos().x, enemies[i][j].Pos().y, 24, 24);// Tamaño del enemigo

				// Comprobar si los rectángulos se intersectan
				if (enemieRect.intersects(bulletRect)) {
					// Eliminar al enemigo del vector de su fila
					enemies[i].erase(enemies[i].begin() + j);// Eliminar enemigo
					enemiesKilled++;      // Incrementar puntuación
					bulletActive = false; // Desactivar la bala
					break; // Salir del bucle 'j' (columnas)
				}
			}
			if (!bulletActive) break; // Salir del bucle 'i' (filas) si ya hubo colisión
		}

		// Si la bala golpeó a un enemigo (se desactivó)
		if (!bulletActive) {
			// Aumentar la velocidad de todos los enemigos restantes
			for (int i = 0; i < (int)enemies.size(); i++) {
				for (int j = 0; j < (int)enemies[i].size(); j++) {
					enemies[i][j].AumentarCadencia();// Aumentar velocidad de movimiento
				}
			}
			// Reducir el tiempo entre disparos enemigos (aumentar cadencia)
			cadencia--;// Disparos más frecuentes
		}
	}
}

/**
 * @brief Actualiza la lógica de los enemigos (movimiento, cambio de dirección y disparo).
 */
void UpdateEnemies(vector<vector<Enemie>>& enemies) {
	// 1. Encontrar los bordes (el enemigo más a la izquierda y más a la derecha)
	maxX = 0;// Inicializar al mínimo posible
	minX = 600;// Inicializar al máximo posible
	for (int i = 0; i < (int)enemies.size(); i++) {
		for (int j = 0; j < (int)enemies[i].size(); j++) {
			maxX = max(maxX, (int)enemies[i][j].Pos().x + 24 * dirEnemies);// +24 para el ancho del enemigo
			minX = min(minX, (int)enemies[i][j].Pos().x + 24 * dirEnemies);// +24 para el ancho del enemigo
		}
	}

	// 2. Comprobar si el enjambre ha tocado un borde de la pantalla
	if (minX < 24 || maxX > 576) {
		// Si toca, mover a TODOS los enemigos hacia abajo
		for (int i = 0; i < (int)enemies.size(); i++) {
			for (int j = 0; j < (int)enemies[i].size(); j++) {
				enemies[i][j].ChangeDir(); // Esta función los mueve hacia abajo
			}
		}
		dirEnemies *= -1; // Invertir la dirección horizontal
	}

	// 3. Mover a todos los enemigos horizontalmente
	for (int i = 0; i < (int)enemies.size(); i++) {
		for (int j = 0; j < (int)enemies[i].size(); j++) {
			enemies[i][j].Update(); // Esta función los mueve horizontalmente
		}
	}

	// 4. Lógica de disparo enemigo
	timer++; // Incrementar el temporizador de disparo

	// Comprobar si el temporizador alcanzó la cadencia y si quedan enemigos
	if (timer >= cadencia && (int)enemies.size() > 0 && (int)enemies[0].size() > 0) {
		timer = 0; // Reiniciar temporizador
		srand(time(NULL)); // Nueva semilla aleatoria
		// Elegir una columna aleatoria (basado en el tamaño de la primera fila)
		int enem = rand() % (int)enemies[0].size();// Índice de columna aleatoria
		// Crear una bala desde el enemigo en la FILA 0 de esa columna aleatoria
		// (Nota: Esta lógica hace disparar al enemigo de la fila superior)
		Bullet bullet = Bullet(enemies[0][enem].Pos().x + 9, enemies[0][enem].Pos().y + 24, spritesheet, IntRect(13 * 8 + 8, 8 * 2 + 2, 8, 8), 10);// Posición (x, y), sprite y velocidad (hacia abajo)
		// Añadir la nueva bala al vector de balas enemigas
		bulletsEnemies.push_back(bullet);// Agregar bala enemiga
	}// Fin de la lógica de disparo
}

/**
 * @brief Actualiza las balas enemigas (movimiento, colisión con jugador, limpieza).
 */
void UpdateBulletsEnemies(Player& player) {
	// 1. Mover todas las balas enemigas
	for (int i = 0; i < (int)bulletsEnemies.size(); i++) {
		bulletsEnemies[i].Update(); // Mover (hacia abajo)
	}

	// 2. Limpiar balas que salen de la pantalla
	for (int i = 0; i < (int)bulletsEnemies.size(); i++) {
		if (bulletsEnemies[i].Pos().y > 600) {
			bulletsEnemies.erase(bulletsEnemies.begin() + i);// Eliminar la bala
			// ¡IMPORTANTE! Decrementar 'i' para no saltarse el siguiente elemento
			i--;// Decrementar índice
		}
	}

	// 3. Comprobar colisión con el jugador
	playerRect = IntRect(player.Pos().x, player.Pos().y + 9, 48, 15);// Área de colisión del jugador (solo la parte superior)
	for (int i = 0; i < (int)bulletsEnemies.size(); i++) {
		bulletRect = IntRect(bulletsEnemies[i].Pos().x, bulletsEnemies[i].Pos().y, 3, 24);// Área de colisión de la bala enemiga
		// Si la bala del enemigo toca al jugador...
		if (playerRect.intersects(bulletRect)) {
			bulletsEnemies.erase(bulletsEnemies.begin() + i); // Eliminar la bala
			player.QuitarVida(); // Restar una vida al jugador
			i--; // Decrementar 'i' para no saltarse el siguiente elemento
		}// Fin de la comprobación de colisión
	}// Fin de la iteración de balas enemigas
}

/**
 * @brief Actualiza los muros (colisión con bala de jugador y balas de enemigos).
 */
void UpdateMuro(vector<Muro>& muro, Bullet& bulletPlayer) {
	// --- 1. Colisión de la BALA DEL JUGADOR con los muros ---
	if (bulletActive) {
		bulletRect = IntRect(bulletPlayer.Pos().x, bulletPlayer.Pos().y, 3, 8);// Rectángulo de la bala del jugador
		for (int i = 0; i < 3; i++) { // Iterar los 3 muros
			// Obtener las posiciones de todas las partes del muro 'i'
			muro[i].Pos(posicionMuro);// Llenar el vector 'posicionMuro'
			for (int j = 0; j < (int)posicionMuro.size(); j++) { // Iterar las partes del muro
				muroRect = IntRect(posicionMuro[j].second.x, posicionMuro[j].second.y, 24, 24);// Rectángulo de la parte del muro
				// Si la bala del jugador toca una parte del muro...
				if (muroRect.intersects(bulletRect)) {
					muro[i].Colision(posicionMuro[j].first, false); // Dañar el muro
					bulletActive = false; // Desactivar la bala
				}// Fin de la comprobación de colisión
			}// Fin de la iteración de partes del muro
			if (!bulletActive) break; // Salir si la bala ya colisionó
		}// Fin de la iteración de muros
	}// Fin de la colisión de la bala del jugador

	// --- 2. Colisión de las BALAS ENEMIGAS con los muros ---
	bool elim = false; // Flag para saber si una bala fue eliminada

	for (int h = 0; h < (int)bulletsEnemies.size(); h++) { // Iterar TODAS las balas enemigas
		bulletRect = IntRect(bulletsEnemies[h].Pos().x, bulletsEnemies[h].Pos().y, 3, 8);// Rectángulo de la bala enemiga 'h'
		for (int i = 0; i < 3; i++) { // Iterar los 3 muros
			muro[i].Pos(posicionMuro); // Obtener partes del muro 'i'
			for (int j = 0; j < (int)posicionMuro.size(); j++) { // Iterar las partes del muro
				muroRect = IntRect(posicionMuro[j].second.x, posicionMuro[j].second.y, 24, 24);// Rectángulo de la parte del muro
				// Si la bala enemiga 'h' toca una parte del muro...
				if (muroRect.intersects(bulletRect)) {
					muro[i].Colision(posicionMuro[j].first, true); // Dañar el muro
					bulletsEnemies.erase(bulletsEnemies.begin() + h); // Eliminar la bala
					elim = true; // Marcar como eliminada
					h--; // Decrementar 'h' para no saltarse la siguiente bala
					break; // Salir del bucle 'j' (partes del muro)
				}// Fin de la comprobación de colisión
			}// Fin de la iteración de partes del muro
			if (elim) { // Si la bala fue eliminada
				elim = false; // Resetear el flag
				break; // Salir del bucle 'i' (muros) e ir a la siguiente bala
			}// Fin de la comprobación de eliminación
		}// Fin de la iteración de muros
	}// Fin de la iteración de balas enemigas

	// 3. Actualizar el estado visual de los muros (mostrar daño)
	for (int i = 0; i < 3; i++) muro[i].Update();// Actualizar cada muro
}// Fin de 'UpdateMuro()'

/**
 * @brief Muestra la pantalla de puntuaciones (Scoreking).
 */
void showScoreking() {
	system("cls");// Limpiar la consola
	cout << BRIGHT_GREEN << "============================\n";
	cout << "	SCOREKING\n";
	cout << "============================\n" << BRIGHT_WHITE;

	json scoreking;// Variable JSON para almacenar las puntuaciones
	ifstream inputFile("Scoreking.json");// Abrir archivo para leer

	// Cargar el archivo JSON con manejo de errores
	if (inputFile.is_open()) {
		try {
			scoreking = json::parse(inputFile);// Intentar parsear el JSON
			inputFile.close();// Cerrar el archivo después de leer
		}
		catch (const json::parse_error& e) {
			cout << BRIGHT_RED << "El archivo Scoreking.json esta corrupto o vacio.\n" << BRIGHT_WHITE;
			inputFile.close();// Cerrar el archivo
			scoreking = json::array(); // Asegura que 'scoreking' sea un array
		}// Fin del manejo de errores
	}// Fin de la carga del archivo JSON
	else {
		// Si el archivo no existe, tratarlo como un array vacío
		scoreking = json::array();// Crear un array JSON vacío
	}// Fin de la comprobación de apertura de archivo

	// Comprobar que 'scoreking' es un array y no está vacío
	if (scoreking.is_array() && !scoreking.empty()) {

		// Crear un vector de pares (puntuacion, nombre) para poder ordenarlo
		vector<pair<int, string>> scores;// Vector para almacenar puntuaciones y nombres
		for (const auto& record : scoreking) {
			// Validar que el registro tenga los campos correctos
			if (record.contains("puntuacion") && record.contains("nombre") && record["puntuacion"].is_number_integer()) {
				// Filtro adicional: No mostrar nombres vacíos
				if (!record["nombre"].get<string>().empty()) {
					scores.push_back({ record["puntuacion"], record["nombre"] });// Agregar al vector
				}// Fin del filtro de nombre vacío
			}// Fin de la validación de campos
		}// Fin de la iteración de registros

		if (scores.empty()) {
			cout << BRIGHT_BLUE << "Aun no hay puntuaciones registradas.\n" << BRIGHT_WHITE;
			cout << BRIGHT_GREEN << "¡Juega para ser el primero en el Scoreking!\n" << BRIGHT_WHITE;
		}// Fin de la comprobación de vector vacío
		else {
			// Ordenar el vector de mayor a menor puntuación
			sort(scores.begin(), scores.end(), [](const pair<int, string>& a, const pair<int, string>& b) {
				return a.first > b.first; // Orden descendente
				});// Fin de la función de comparación

			// Mostrar la tabla formateada (Top 10)
			cout << BRIGHT_CYAN << "| PUESTO | PUNTUACION | NOMBRE\n";
			cout << "|--------|------------|--------------------\n" << BRIGHT_WHITE;
			for (size_t i = 0; i < min((size_t)10, scores.size()); ++i) {
				// 'printf' para formatear la salida en columnas alineadas
				printf("| %-6zu | %-10d | %s\n", i + 1, scores[i].first, scores[i].second.c_str());// Mostrar puesto, puntuación y nombre
			}// Fin del bucle de visualización
		}// Fin de la comprobación de scores no vacío
	}// Fin de la comprobación de array no vacío
	else {
		// Mensaje si 'Scoreking.json' no existe o está vacío
		cout << BRIGHT_BLUE << "Aun no hay puntuaciones registradas.\n" << BRIGHT_WHITE;
		cout << BRIGHT_GREEN << "¡Juega para ser el primero en el Scoreking!\n" << BRIGHT_WHITE;
	}

	cout << BRIGHT_YELLOW << "\nPresiona ENTER para volver al menu..." << BRIGHT_WHITE;
	cin.get(); // Pausa
}// Fin de 'showScoreking()'

/**
 * @brief Muestra el submenú de Scoreking (Mostrar, Eliminar, Volver)
 */
void showScorekingMenu() {
	while (true) {
		system("cls");// Limpiar la consola
		cout << BRIGHT_GREEN << "===============================\n" << BRIGHT_WHITE;
		cout << "	SCOREKING (Menu)\n";
		cout << BRIGHT_GREEN << "===============================\n" << BRIGHT_WHITE;
		cout << "1. Mostrar Scoreking\n";
		cout << BRIGHT_RED << "2. Eliminar mi Scoreking\n" << BRIGHT_WHITE;
		cout << BRIGHT_YELLOW << "3. Volver al menu principal\n" << BRIGHT_WHITE;
		cout << BRIGHT_GREEN << "===============================\n" << BRIGHT_WHITE;
		cout << BRIGHT_YELLOW << "Elige una opcion: " << BRIGHT_WHITE;

		int choice = 0;// Variable para almacenar la elección del usuario
		cin >> choice;// Leer la elección del usuario

		// Validación de entrada estándar
		if (cin.fail()) {
			cin.clear();// Limpiar la bandera de error de 'cin'
			cin.ignore(numeric_limits<streamsize>::max(), '\n');// Limpiar buffer
			cout << BRIGHT_RED << "Entrada invalida. Por favor, escribe un numero.\n" << BRIGHT_WHITE;
			cout << BRIGHT_YELLOW << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
			cin.get();// Esperar un ENTER
		}// Fin de la validación de entrada
		else {
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar buffer

			switch (choice) {
			case 1:
				showScoreking(); // Mostrar la lista
				break;
			case 2:
				deleteMyScore(); // Borrar entradas
				break;
			case 3:
				return; // Salir de este menú y volver al anterior
			default:
				cout << BRIGHT_RED << "Opcion invalida. Por favor, elige un numero del menu.\n" << BRIGHT_WHITE;
				cout << BRIGHT_YELLOW << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
				cin.get();// Esperar un ENTER
				break;
			}// Fin del 'switch'
		}// Fin de la validación de entrada	
	}// --- FIN DE 'while (true)' ---
}// Fin de 'showScorekingMenu()'

/**
 * @brief Pide un nombre al usuario y elimina todas las entradas coincidentes de Scoreking.json
 */
void deleteMyScore() {
	system("cls");// Limpiar la consola
	cout << BRIGHT_YELLOW << "=================================\n" << BRIGHT_WHITE;
	cout << BRIGHT_RED << "	ELIMINAR PUNTUACION\n" << BRIGHT_WHITE;
	cout << BRIGHT_YELLOW << "=================================\n" << BRIGHT_WHITE;

	string nameToDelete;// Variable para almacenar el nombre a eliminar
	cout << BRIGHT_YELLOW << "Ingresa el nombre EXACTO que deseas eliminar del Scoreking: " << BRIGHT_WHITE;
	getline(cin, nameToDelete); // 'getline' para capturar nombres con espacios

	if (nameToDelete.empty()) {
		cout << BRIGHT_YELLOW << "\nNombre vacio. No se elimino nada.\n";
		cout << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
		cin.get();// Pausa
		return;// Salir de la función sin eliminar
	}// Fin de la comprobación de nombre vacío

	// 1. Cargar el JSON
	json scoreking;// Variable JSON para almacenar las puntuaciones
	ifstream inputFile("Scoreking.json");// Abrir archivo para leer
	if (!inputFile.is_open()) {
		cout << BRIGHT_YELLOW << "\nNo se encontro Scoreking.json. No hay nada que eliminar.\n";
		cout << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
		cin.get();// Pausa
		return;// Salir de la función
	}// Fin de la comprobación de apertura de archivo

	// Manejo de JSON corrupto
	try {
		scoreking = json::parse(inputFile);// Intentar parsear el JSON
	}
	catch (const json::parse_error& e) {
		cout << BRIGHT_RED << "\nArchivo Scoreking.json corrupto. No se puede eliminar.\n" BRIGHT_WHITE;
		cout << BRIGHT_YELLOW << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
		cin.get();// Pausa
		inputFile.close();// Cerrar el archivo
		return;// Salir de la función
	}// Fin del manejo de errores
	inputFile.close();// Cerrar el archivo después de leer

	if (!scoreking.is_array() || scoreking.empty()) {
		cout << BRIGHT_YELLOW << "\nScoreking esta vacio. No hay nada que eliminar.\n";
		cout << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
		cin.get();// Pausa
		return;// Salir de la función
	}// Fin de la comprobación de array vacío

	// 2. Crear un nuevo JSON filtrado
	json newScoreking = json::array(); // Un array vacío
	bool found = false;// Flag para saber si se encontró alguna coincidencia
	// Iterar sobre el JSON cargado
	for (const auto& record : scoreking) {
		// Si el nombre NO coincide, se añade al nuevo array
		if (record.contains("nombre") && record["nombre"].get<string>() == nameToDelete) {
			found = true;// Se encontró una coincidencia
			// No agregar este registro (esto lo "elimina")
		}// Si no coincide...
		else {
			// Conservar este registro
			newScoreking.push_back(record);
		}// Fin de la comprobación de nombre
	}// Fin de la iteración de registros

	// 3. Informar al usuario y guardar
	if (!found) {
		cout << "\nNo se encontro ninguna puntuacion con el nombre: " << BRIGHT_RED << nameToDelete << "\n" << BRIGHT_WHITE;
	}
	else {
		// 4. Sobrescribir el archivo con el nuevo JSON (filtrado)
		ofstream outputFile("Scoreking.json");// Abrir archivo para escribir
		if (outputFile.is_open()) {
			outputFile << newScoreking.dump(4); // Guardar el array sin las entradas eliminadas
			outputFile.close();// Cerrar el archivo
			cout << BRIGHT_GREEN << "\nSe eliminaron todas las puntuaciones de: " << BRIGHT_WHITE << nameToDelete << "\n";
		}// Fin de la comprobación de apertura de archivo
		else {
			cerr << BRIGHT_RED << "\nERROR: No se pudo guardar el Scoreking.json actualizado.\n" << BRIGHT_WHITE;
		}// Fin de la comprobación de apertura de archivo
	}// Fin de la comprobación de coincidencias

	cout << BRIGHT_YELLOW << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
	cin.get();// Pausa
}// Fin de 'deleteMyScore()'

/**
 * @brief Muestra la pantalla de instrucciones.
 */
void showInstructions() {
	system("cls");// Limpiar la consola
	cout << BRIGHT_CYAN << "==============================\n" << BRIGHT_WHITE;
	cout << BRIGHT_MAGENTA << "	INSTRUCCIONES\n" << BRIGHT_WHITE;
	cout << BRIGHT_CYAN << "==============================\n" << BRIGHT_WHITE;
	cout << "- Muevete con las flechas Izquierda y Derecha.\n";
	cout << "- Dispara con la tecla Espacio.\n";
	cout << "- Destruye a todos los aliens para ganar.\n";
	cout << "- Si un alien te toca o llega al fondo, pierdes.\n";
	cout << "- Los muros te protegen, pero se destruyen.\n";
	cout << BRIGHT_YELLOW << "\n\nPresiona ENTER para volver al menu..." << BRIGHT_WHITE;
	cin.get(); // Pausa
}// Fin de 'showInstructions()'

/**
 * @brief Muestra la pantalla de créditos.
 */
void showCredits() {
	system("cls");// Limpiar la consola
	cout << BRIGHT_CYAN << "============================\n" << BRIGHT_WHITE;
	cout << BRIGHT_MAGENTA << "	CREDITOS\n" << BRIGHT_WHITE;
	cout << BRIGHT_CYAN << "============================\n" << BRIGHT_WHITE;
	cout << BRIGHT_MAGENTA << "Juego creado por:\n" << BRIGHT_WHITE;
	cout << BRIGHT_BLUE << "(Tu Nombre Aqui)\n" << BRIGHT_WHITE;
	cout << BRIGHT_BLUE << "(Tu Nombre Aqui)\n" << BRIGHT_WHITE;
	cout << BRIGHT_BLUE << "(Tu Nombre Aqui)\n" << BRIGHT_WHITE;
	cout << BRIGHT_MAGENTA << "Programado en C++ con SFML.\n" << BRIGHT_WHITE;
	cout << BRIGHT_YELLOW << "\n\nPresiona ENTER para volver al menu..." << BRIGHT_WHITE;
	cin.get(); // Pausa
}

/**
 * @brief Muestra el menú de fin de juego y devuelve la selección del usuario.
 * @return 1 (Menú Principal), 2 (Scoreking), 3 (Salir)
 */
int showPostGameMenu() {
	while (true) {
		system("cls");// Limpiar la consola
		cout << BRIGHT_CYAN << "============================\n" << BRIGHT_WHITE;
		cout << BRIGHT_GREEN << "	JUEGO TERMINADO\n" << BRIGHT_WHITE;
		cout << BRIGHT_CYAN << "============================\n" << BRIGHT_WHITE;
		cout << "1. Volver al menu principal\n";
		cout << "2. Ver Scoreking\n";
		cout << BRIGHT_RED << "3. Salir del juego\n" << BRIGHT_WHITE;
		cout << BRIGHT_CYAN << "==========================\n" << BRIGHT_WHITE;
		cout << BRIGHT_YELLOW << "Elige una opcion: " << BRIGHT_WHITE;

		int choice = 0;// Variable para almacenar la elección del usuario
		cin >> choice;// Leer la elección del usuario

		// Validación de entrada estándar
		if (cin.fail()) {
			cin.clear();// Limpiar la bandera de error de 'cin'
			cin.ignore(numeric_limits<streamsize>::max(), '\n');// Limpiar buffer
			cout << BRIGHT_RED << "Entrada invalida. Por favor, escribe un numero.\n" << BRIGHT_WHITE;
			cout << BRIGHT_YELLOW << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
			cin.get();// Esperar un ENTER
		}// Fin de la validación de entrada
		else {
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar buffer

			switch (choice) {
			case 1:
				return 1; // Volver al menú
			case 2:
				return 2; // Ver Scoreking (el bucle en 'main' llamará a 'showScorekingMenu')
			case 3:
				return 3; // Salir
			default:
				cout << BRIGHT_RED << "Opcion invalida. Por favor, elige un numero del menu.\n" << BRIGHT_WHITE;
				cout << BRIGHT_YELLOW << "Presiona ENTER para continuar..." << BRIGHT_WHITE;
				cin.get();// Esperar un ENTER
				break;
			}// Fin del 'switch'
		}// Fin de la validación de entrada
	}// --- FIN DE 'while (true)' ---
}// Fin de 'showPostGameMenu()'|