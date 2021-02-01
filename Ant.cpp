// Include Standardheader, steht bei jedem C/C++-Programm am Anfang
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW, GLEW ist ein notwendiges Übel. Der Hintergrund ist, dass OpenGL von Microsoft
// zwar unterstützt wird, aber nur in einer Uralt-Version. Deshalb beinhaltet die Header-Datei,
// die vom Betriebssystem zur Verfügung gestellt wird, nur Deklarationen zu den uralten Funktionen,
// obwohl der OpenGL-Treiber, und damit die OpenGL-dll die neuesten Funktionen implementiert.
// Die neueren Funktionen werden deshalb über diese Header-Datei separat zur Verfügung gestellt.
#include <GL/glew.h>

// Include GLFW, OpenGL definiert betriebssystemunabhängig die graphische Ausgabe. Interaktive 
// Programme beötigen aber natürlich auch Funktionen für die Eingabe (z. B. Tastatureingaben)
// Dies geht bei jedem OS (z. B. Windows vs. MacOS/Unix) etwas anders. Um nun generell plattformunabhängig
// zu sein, verwenden wir GLFW, was die gleichen Eingabe-Funktionen auf die Implementierung unterschiedlicher
// OS abbildet. (Dazu gibt es Alternativen, glut wird z. B. auch häufig verwendet.)
#include <GLFW/glfw3.h>

// Include GLM, GLM definiert für OpenGL-Anwendungen Funktionen der linearen Algebra wie
// Transformationsmatrizen. Mann könnte GLM auch durch etaws anderes ersetzen oder aber in einem
// anderen Kontext verwenden.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// In C- und C++-Programmen ist die Reihenfolge der include-Direktiven wichtig.
// Dateien, die mit Spitzklammern includiert werden, werden in den System-Verzeichnissen
// gesucht, die mit doppelten Hochkommata im lokalen Projektverzeichnis
// (wo genau ist in den Projekteinstellungen zu finden und ggf. zu ändern.) 

// Diese Datei benötigen wir, um die Shader-Programme komfortabel in die Hardware zu laden.
// (Mit der rechten Mouse-taste können Sie in VS diese Datei öffnen, um nachzuschauen, was dort deklariert wird.)
#include "shader.hpp"

// Wuerfel und Kugel
#include "objects.hpp"

//Textures
#include "texture.hpp"


#include "Obj3D.hpp"


// die Rotation der View
float winkelX = 0;
float winkelY = 0;
float winkelZ = 0;


//position der Armeise
float antPosX = 0;
float antPosY = 0;
float antRotation = 0;

//Rotationswinkel, je einzelner Tastendruck/impuls
float keySensi = 2;

float initialFoV = 45.0f;
float mouseWheel = 0;
float sphereScaling = 1;


// Callback-Mechanismen gibt es in unterschiedlicher Form in allen möglichen Programmiersprachen,
// sehr häufig in interaktiven graphischen Anwendungen. In der Programmiersprache C werden dazu 
// Funktionspointer verwendet. Man übergibt einer aufgerufenen Funktion einer Bibliothek einen
// Zeiger auf eine Funktion, die zurückgerufen werden kann. Die Signatur der Funktion muss dabei
// passen. Dieser Mechanismus existiert auch in C++ und wird hier verwendet, um eine einfache
// Fehlerbehandlung durchzuführen. Diese Funktion gibt Fehler aus, die beim Aufruf von OpenGL-Befehlen
// auftreten.
void error_callback(int error, const char* description)
{
	// Mit fputs gibt man hier den String auf den Standarderror-Kanal aus.
	// In der C-Welt, aus der das hier übernommen ist, sind Strings Felder aus "char"s, die mit 
	// dem Wert null terminiert werden.
	fputs(description, stderr);
}

// für Berechnung von Richtungs-bewegung der Armeise
float PI = 3.14159265358979323846;

//ground size
const float groundSize = 25.0f;
const float groundEdge = 2.0f;
const float maxSphereSize = 35.0f;

void move(int direction) {


	if (direction > 0) {
		float deltaX = cos(antRotation * (PI / 180)) * 0.1 * direction;
		float deltaY = sin(antRotation * (PI / 180)) * 0.1 * direction;

		if (antPosX < groundSize && antPosX > -groundSize) {
			antPosX += deltaX;
		}
		else if (antPosX > -groundSize && deltaX < 0)
			antPosX += deltaX;
		else if (antPosX < groundSize && deltaX > 0)
			antPosX += deltaX;

		if (antPosY < groundSize && antPosY > -groundSize) {
			antPosY += deltaY;
		}
		else if (antPosY > -groundSize && deltaY < 0)
			antPosY += deltaY;
		else if (antPosY < groundSize && deltaY > 0)
			antPosY += deltaY;
		if (sphereScaling <= maxSphereSize) sphereScaling += 0.2;
	}
	else if (direction < 0) {
		//kein rueckwaertslaufen erlaubt!
		//antPosX -= cos(antRotation * (PI / 180)) * 0.02 * -direction;
		//antPosY -= sin(antRotation * (PI / 180)) * 0.02 * -direction;
	}
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	printf("%f\n", yoffset);
	mouseWheel += yoffset;
}

// Diese Funktion wird ebenfalls über Funktionspointer der GLFW-Bibliothek übergeben.
// (Die Signatur ist hier besonders wichtig. Wir sehen, dass hier drei Parameter definiert
//  werden müssen, die gar nicht verwendet werden.)
// Generell überlassen wir der GLFW-Bibliothek die Behandlung der Input-Ereignisse (Mouse moved,
// button click, Key pressed, etc.).
// Durch die Übergabe dieser Funktion können wir Keyboard-Events 
// abfangen. Mouse-Events z. B. erhalten wir nicht, da wir keinen Callback an GLFW übergeben.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//printf("%f\n", antRotation);
	//printf("%f\n", antPosX);
	//printf("%f\n", antPosY);

	switch (key)
	{
		// Mit rechte Mousetaste -> gehe zu Deklaration finden Sie andere Konstanten für Tasten.
	case GLFW_KEY_ESCAPE:
		// Das Programm wird beendet, wenn BenutzerInnen die Escapetaste betätigen.
		// Wir könnten hier direkt die C-Funktion "exit" aufrufen, eleganter ist aber, GLFW mitzuteilen
		// dass wir das Fenster schliessen wollen (siehe Schleife unten).
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
		// Mit taste R rotiere den Cube
	case GLFW_KEY_R:
		winkelZ -= keySensi;
		// Mit taste R rotiere den Cube
		break;
	case GLFW_KEY_T:
		winkelZ += keySensi;
		break;
	case GLFW_KEY_W:
		winkelX -= keySensi;
		break;
	case GLFW_KEY_A:
		winkelY += keySensi;
		break;
	case GLFW_KEY_S:
		winkelX += keySensi;
		break;
	case GLFW_KEY_D:
		winkelY -= keySensi;
		break;

	case GLFW_KEY_UP:
		move(1);
		break;
	case GLFW_KEY_DOWN:
		move(-1);
		break;
	case GLFW_KEY_RIGHT:
		antRotation += keySensi * 3;
		break;
	case GLFW_KEY_LEFT:
		antRotation -= keySensi * 3;
		break;
	default:
		break;
	}
}



// Diese drei Matrizen speichern wir global (Singleton-Muster), damit sie jederzeit modifiziert und
// an die Grafikkarte geschickt werden koennen. Ihre Bedeutung habe ich in der Vorlesung Geometrische
// Transformationen erklärt, falls noch nicht geschehen, jetzt anschauen !
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 Model;
GLuint programID; // OpenGL unterstützt unterschiedliche Shaderprogramme, zwischen denen man
				  // wechseln kann. Unser Programm wird mit der unsigned-integer-Variable programID
				  // referenziert.

// Ich habe Ihnen hier eine Hilfsfunktion definiert, die wir verwenden, um die Transformationsmatrizen
// zwischen dem OpenGL-Programm auf der CPU und den Shaderprogrammen in den GPUs zu synchronisieren.
// (Muss immer aufgerufen werden, bevor wir Geometriedaten in die Pipeline einspeisen.)
void sendMVP()
{
	// Zunächst können wir die drei Matrizen einfach kombinieren, da unser einfachster Shader
	// wirklich nur eine Transformationsmatrix benötigt, wie in der Vorlesung erklärt.
	// Später werden wir hier auch die Teilmatrizen an den Shader übermitteln müssen.
	// Interessant ist hier, dass man in C++ (wie auch in C#) den "*"-Operator überladen kann, so dass
	// man Klassenobjekte miteinander multiplizieren kann (hier Matrizen bzw. "mat4"), 
	// das ginge in JAVA so natürlich nicht. 
	glm::mat4 MVP = Projection * View * Model;

	// "glGetUniformLocation" liefert uns eine Referenz auf eine Variable, die im Shaderprogramm
	// definiert ist, in diesem Fall heisst die Variable "MVP".
	// "glUniformMatrix4fv" überträgt Daten, genauer 4x4-Matrizen, aus dem Adressraum unserer CPU
	// (vierter Parameter beim Funktionsaufruf, wir generieren mit "&" hier einen Pointer auf das erste 
	//  Element, und damit auf das gesamte Feld bzw den Speicherbereich) 
	// in den Adressraum der GPUs. Beim ersten Parameter 
	// muss eine Referenz auf eine Variable im Adressraum der GPU angegeben werden.
	glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);


	//Originale lichtquelle im Raum
	//glm::vec3 lightPos = glm::vec3(4, 4, -4);
	//glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y, lightPos.z);
}


//####################################################################################################################################
//##################################################--teil3--#########################################################################

void drawCS() {
	double longSide = 2.0;
	double shortSide = 0.02;

	glm::mat4 Save = Model;
	Model = glm::scale(Model, glm::vec3(longSide, shortSide, shortSide));
	sendMVP();
	shaderHelper();
	drawSphere(10, 10);

	Model = Save;
	Model = glm::scale(Model, glm::vec3(shortSide, shortSide, longSide));
	sendMVP();
	shaderHelper();
	drawSphere(10, 10);

	Model = Save;
	Model = glm::translate(Model, glm::vec3(0, 1, 0));
	Model = glm::scale(Model, glm::vec3(shortSide, longSide / 2, shortSide));
	sendMVP();
	shaderHelper();
	drawSphere(10, 10);

	Model = Save;
}

void drawSeg(float h)
{

	glm::mat4 Save = Model;
	Model = glm::translate(Model, glm::vec3(0, h, 0));
	Model = glm::scale(Model, glm::vec3(h * 0.4, h, h * 0.4));
	sendMVP();
	shaderHelper();
	drawSphere(10, 10);
	Model = Save;
}

//FOOD DROPS
//random seed

int foodNumber = 0;
const int foodMax = 10;
const int foodLambda = 5; // every foodLambda a fruit will drop somehwere
float randomFoodX[foodMax]{};
float randomFoodY[foodMax]{};

//Balls

//glm::vec3 posBalls;
int sphereNumber = 0;
const int sphereMax = 100;
std::vector<glm::vec3> spherePos;
glm::vec3 spheresTrans[sphereMax];
glm::vec3 spheresScale[sphereMax];
glm::mat4 spheres[sphereMax];
//##################################################--teil3--#########################################################################
//####################################################################################################################################


// Einstiegspunkt für C- und C++-Programme (Funktion), Konsolenprogramme könnte hier auch Parameter erwarten
int main(void)
{

	// Initialisierung der GLFW-Bibliothek
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	// Fehler werden auf stderr ausgegeben, s. o.
	glfwSetErrorCallback(error_callback);


	int window_width = 1280;
	int window_height = 720;
	// Öffnen eines Fensters für OpenGL, die letzten beiden Parameter sind hier unwichtig
	// Diese Funktion darf erst aufgerufen werden, nachdem GLFW initialisiert wurde.
	// (Ggf. glfwWindowHint vorher aufrufen, um erforderliche Resourcen festzulegen -> MacOSX)
	GLFWwindow* window = glfwCreateWindow(window_width, // Breite
		720,  // Hoehe
		"Ant Game", // Ueberschrift
		NULL,  // windowed mode
		NULL); // shared window

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Wir könnten uns mit glfwCreateWindow auch mehrere Fenster aufmachen...
	// Spätestens dann wäre klar, dass wir den OpenGL-Befehlen mitteilen müssen, in
	// welches Fenster sie "malen" sollen. Wir müssen das aber zwingend auch machen,
	// wenn es nur ein Fenster gibt.

	// Bis auf weiteres sollen OpenGL-Befehle in "window" malen.
	// Ein "Graphic Context" (GC) speichert alle Informationen zur Darstellung, z. B.
	// die Linienfarbe, die Hintergrundfarbe. Dieses Konzept hat den Vorteil, dass
	// die Malbefehle selbst weniger Parameter benötigen.
	// Erst danach darf man dann OpenGL-Befehle aufrufen !
	glfwMakeContextCurrent(window);

	// Initialisiere GLEW
	// (GLEW ermöglicht Zugriff auf OpenGL-API > 1.1)
	glewExperimental = true; // Diese Zeile ist leider notwendig.

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Auf Keyboard-Events reagieren (s. o.)
	glfwSetKeyCallback(window, key_callback);

	// auf mausrad-events reagieren
	glfwSetScrollCallback(window, scroll_callback);

	// Setzen von Dunkelblau als Hintergrundfarbe (erster OpenGL-Befehl in diesem Programm).
	// Beim späteren Löschen gibt man die Farbe dann nicht mehr an, sondern liest sie aus dem GC
	// Der Wertebereich in OpenGL geht nicht von 0 bis 255, sondern von 0 bis 1, hier sind Werte
	// fuer R, G und B angegeben, der vierte Wert alpha bzw. Transparenz ist beliebig, da wir keine
	// Transparenz verwenden. Zu den Farben sei auf die entsprechende Vorlesung verwiesen !
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glViewport(0, 0, window_width, window_height);

	// Kreieren von Shadern aus den angegebenen Dateien, kompilieren und linken und in
	// die Grafikkarte übertragen.  
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Diesen Shader aktivieren ! (Man kann zwischen Shadern wechseln.) 
	glUseProgram(programID);

	// Load the texture
	GLuint TextureMandrill = loadBMP_custom(".\\textures\\mandrill.bmp");
	GLuint TextureGrass = loadBMP_custom(".\\textures\\grass.bmp");
	GLuint TextureAntSkin = loadBMP_custom(".\\textures\\antSkin.bmp");
	GLuint TexturePrairie = loadBMP_custom(".\\textures\\prairie.bmp");

	//Obj3D anthill("/models/anthill.obj");
	//Obj3D teapot(".\\models\\teapot.obj");
	Obj3D ant(".\\models\\ant.obj");
	Obj3D banana(".\\models\\banana.obj");
	Obj3D birthdayCake(".\\models\\cherry.obj");
	Obj3D cherry(".\\models\\cherry.obj");
	Obj3D chicken(".\\models\\cherry.obj");
	Obj3D egg(".\\models\\cherry.obj");
	Obj3D lettuce(".\\models\\cherry.obj");
	Obj3D melon(".\\models\\melon.obj");
	Obj3D pineapple(".\\models\\cherry.obj");
	Obj3D pizza(".\\models\\cherry.obj");
	Obj3D ground(".\\models\\ground.obj");
	Obj3D wall(".\\models\\wall.obj");

	Obj3D food[] = { banana,birthdayCake,cherry,chicken,egg,lettuce,melon,pineapple,pizza,banana };



	//FOOD DROPS brauchen Random float für Position
	// innnerhalb des grounds, daher -0.X
	srand(71);
	float min = -groundSize + 0.5;
	float max = groundSize - 0.5;

	//Time in seconds
	double t = glfwGetTime();
	double lastTFPS = t;
	double lastT = t;

	//frames
	int nbFrames = 0;


	// Alles ist vorbereitet, jetzt kann die Eventloop laufen...
	while (!glfwWindowShouldClose(window))
	{
		//bad zoom
		float FoV = initialFoV - 1 * mouseWheel;

		//let Food objects appear after time
		//compute timing
		t = glfwGetTime();
		double deltaT = t - lastT;

		//get framePerSec
		//nbFrames++;
		//if (t - lastTFPS >= 1.0) {
			// printf and reset timer
		//	printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		//	nbFrames = 0;
		//	lastTFPS += 1.0;
		//}

		//nach x Sekunden und bei weniger als y food drops auf dem Feld
		if (deltaT > foodLambda && foodNumber < foodMax) {
			//der Teiler von Rand_Max bestimmt die maximale Zahl (von Null bis max 10 z.B)
			float randomX = min + rand() / (float(RAND_MAX / (max - min)));
			float randomY = min + rand() / (float(RAND_MAX / (max - min)));

			//Anzzahl der food drops erhöhen
			foodNumber++;
			//Food drop position setzen
			randomFoodX[foodNumber - 1] = randomX;
			randomFoodY[foodNumber - 1] = randomY;

			printf("X= %f", randomX);
			printf(" and Y= %f \n\n", randomY);

			lastT = t;
		}


		// Löschen des Bildschirms (COLOR_BUFFER), man kann auch andere Speicher zusätzlich löschen, 
		// kommt in späteren Übungen noch...
		// Per Konvention sollte man jedes Bild mit dem Löschen des Bildschirms beginnen, muss man aber nicht...
		//glClear(GL_COLOR_BUFFER_BIT);	
		//z Buffer für tiefe aktivieren, um engstes Pixel am Betrachter dran zu zeigen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//OpenGL soll z-Test aktivieren
		glEnable(GL_DEPTH_TEST);
		//pixel mit kleineren z-Werten akzeptieren (eigentlich default), statt größeren! 
		glDepthFunc(GL_LESS);

		// Einstellen der Geometrischen Transformationen
		// Wir verwenden dazu die Funktionen aus glm.h
		// Projektionsmatrix mit 45Grad horizontalem Öffnungswinkel, 4:3 Seitenverhältnis, 
		// Frontplane bai 0.1 und Backplane bei 100. (Das sind OpenGL-Einheiten, keine Meter oder der gleichen.)
		Projection = glm::perspective(FoV, ((float)window_width) / window_height, 0.01f, 100.0f);


		// Modelmatrix : Hier auf Einheitsmatrix gesetzt, was bedeutet, dass die Objekte sich im Ursprung
		// des Weltkoordinatensystems befinden.
		Model = glm::mat4(1.0f);
		glm::mat4 ModelNoRotation = Model;




		// Diese Informationen (Projection, View, Model) müssen geeignet der Grafikkarte übermittelt werden,
		// damit sie beim Zeichnen von Objekten berücksichtigt werden können.
		//sendMVP();
		//Unser neuer Shader
		//	kommt nicht mehr mit der zusammengefassten MVP‐Matrix alleine klar, sondern für die Beleuchtung
		//	benötigt er die Einzelmatrizen.Deshalb müssen wir Sie ihm immer schicken, wenn wir unseren
		//	Zustand im Adressraum mit seinem synchronisieren. Diese drei Zeilen gehören also nach „sendMVP“.
		//shaderHelper();


		//edit the Model, but before store the initial Model
		glm::mat4 Save = Model;
		drawCS();

		// Bind our texture in Texture Unit 0	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureAntSkin);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);


		//ants moving sphere
		glm::vec3 translate1 = glm::vec3(antPosX, 0, antPosY);
		Model = glm::translate(Model, translate1);

		glm::mat4 AntAnchor = Model;

		// Viewmatrix, beschreibt wo die Kamera steht, wo sie hinschaut, und wo oben ist. 
		// Man muss angeben, wo oben ist, da es eine Mehrdeutigkeit gäbe, wenn man nur beschreiben
		// würde, wo die Kamera steht und wo sie hinschaut. Denken Sie an ein Flugzeug. Die Position 
		// des/r Piloten/in in der Welt ist klar, es ist dann auch klar, wo er/sie hinschaut. Das Flugzeug 
		// kann sich aber z. B. auf die Seite legen, dann würde der Horizont "kippen". Dieser Aspekt wird
		// mit dem up-Vektor (hier "oben") gesteuert.
		View = glm::lookAt(
			glm::vec3(translate1 - glm::vec3((1 + mouseWheel) * cos(antRotation / 57), -7, (1 + mouseWheel) * sin(antRotation / 57))), // die Kamera ist bei (0,0,-5), in Weltkoordinaten
			glm::vec3(translate1),  // und schaut in den Ursprung
			glm::vec3(0, 1, 0)); // Oben ist bei (0,1,0), das ist die y-Achse

			//Rotate the View
		//Model = glm::rotate(Model, winkelX, glm::vec3(1, 0, 0));
		//Model = glm::rotate(Model, winkelY, glm::vec3(0, 1, 0));
		//Model = glm::rotate(Model, winkelZ, glm::vec3(0, 0, 1));



		//Lichtpunkt
		glm::vec4 lightPos = Model * glm::vec4(0, 5.0f, 0, 1);
		glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y,
			lightPos.z);

		glm::vec3 scaling1 = glm::vec3((5 + sphereScaling) / 100, (5 + sphereScaling) / 100, (5 + sphereScaling) / 100);
		Model = glm::scale(Model, scaling1);
		Model = glm::translate(Model, glm::vec3(0, 1, 0));
		sendMVP();
		shaderHelper();
		drawSphere(10, 10);



		if (sphereScaling > maxSphereSize && sphereNumber < sphereMax) {

			spheres[sphereNumber] = AntAnchor;
			spheresTrans[sphereNumber] = translate1;
			spheresScale[sphereNumber] = scaling1;
			sphereNumber++;

			//sphereModels.push_back(Model);
			//setze aktuelle model transformations:
			//spherePos.push_back((glm::vec3)Model[3]);
			sphereScaling = 1;
		}


		//draw the builded spheres
		for (size_t i = 0; i < sizeof(spheres) / sizeof(spheres[0]); i++)
		{
			//printf("%f\n", ((float)i));

			glm::vec3 translating = spheresTrans[i];
			glm::vec3 scaling = spheresScale[i];
			Model = Save;
			Model = glm::translate(Model, translating);
			Model = glm::scale(Model, scaling);
			Model = glm::translate(Model, glm::vec3(0, 1, 0));

			//Rotate the View
			//Model = glm::rotate(Model, winkelX, glm::vec3(1, 0, 0));
			//Model = glm::rotate(Model, winkelY, glm::vec3(0, 1, 0));
			//Model = glm::rotate(Model, winkelZ, glm::vec3(0, 0, 1));
			//Model = glm::translate(Model, translating);
			//Model = glm::scale(Model, scaling);

			sendMVP();
			shaderHelper();
			drawSphere(10, 10);
		}



		//the Ant
		Model = AntAnchor;
		Model = glm::rotate(Model, antRotation, glm::vec3(0, -1, 0));
		Model = glm::translate(Model, glm::vec3(-(sphereScaling / 100 + 0.3), 0.0, 0.0));
		Model = glm::rotate(Model, 90.0f, glm::vec3(0, 1, 0));

		Model = glm::rotate(Model, 0.0f, glm::vec3(-1, 0, 0));
		Model = glm::rotate(Model, 0.0f, glm::vec3(0, 0, 1));
		Model = glm::scale(Model, glm::vec3(1.0 / 20, 1.0 / 20, 1.0 / 20));

		sendMVP();
		shaderHelper();
		ant.display();

		//// Bind our texture in Texture Unit xx	
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE1, 0);
		////glDeleteTextures(1, &TextureGrass);
		//// Set our "myTextureSampler" sampler to user Texture Unit xx
		//glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 1);

		//Model = Save;
		//Model = glm::translate(Model, glm::vec3(2, 0, 0));
		//Model = glm::rotate(Model, 90.0f, glm::vec3(-1, 0, 0));
		//Model = glm::scale(Model, glm::vec3(1.0 / 10, 1.0 / 10, 1.0 / 10));
		//sendMVP();
		//shaderHelper();
		//banana.display();


		//// Bind our texture in Texture Unit 2	
		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_2D, TextureGrass);
		//// Set our "myTextureSampler" sampler to user Texture Unit 2
		//glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 2);

		//Model = Save;
		//Model = glm::translate(Model, glm::vec3(0, 0, 3));
		//Model = glm::rotate(Model, 90.0f, glm::vec3(-1, 0, 0));
		//Model = glm::scale(Model, glm::vec3(1.0 / 10, 1.0 / 10, 1.0 / 10));
		//sendMVP();
		//shaderHelper();
		//birthdayCake.display();


		// Bind our texture in Texture Unit 3
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, TextureGrass);
		// Set our "myTextureSampler" sampler to user Texture Unit 3
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 3);

		//the ground
		Model = Save;
		Model = glm::translate(Model, glm::vec3(0, -0.001, 0));
		Model = glm::scale(Model, glm::vec3((groundSize + groundEdge) / 100, 1, (groundSize + groundEdge) / 100));
		sendMVP();
		shaderHelper();
		ground.display();


		// Bind our texture in Texture Unit 4
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, TexturePrairie);
		// Set our "myTextureSampler" sampler to user Texture Unit 4
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 4);

		//the walls
		Model = Save;
		Model = glm::translate(Model, glm::vec3(0, 0.9, 0));

		//
		glm::mat4 Wall = Model;
		Model = glm::translate(Model, glm::vec3(0, 0, groundSize + groundEdge));
		Model = glm::rotate(Model, 90.0f, glm::vec3(0, 1, 0));
		Model = glm::scale(Model, glm::vec3(1, 0.3, (groundSize + groundEdge) / 100));
		sendMVP();
		shaderHelper();
		wall.display();

		//
		Model = Wall;
		Model = glm::translate(Model, glm::vec3(-(groundSize + groundEdge), 0, 0));
		Model = glm::rotate(Model, 0.0f, glm::vec3(0, 1, 0));
		Model = glm::scale(Model, glm::vec3(1, 0.3, (groundSize + groundEdge) / 100));
		sendMVP();
		shaderHelper();
		wall.display();

		//
		Model = Wall;
		Model = glm::translate(Model, glm::vec3(groundSize + groundEdge, 0, 0));
		Model = glm::rotate(Model, 0.0f, glm::vec3(0, 1, 0));
		Model = glm::scale(Model, glm::vec3(1, 0.3, (groundSize + groundEdge) / 100));
		sendMVP();
		shaderHelper();
		wall.display();

		//
		Model = Wall;
		Model = glm::translate(Model, glm::vec3(0, 0, -(groundSize + groundEdge)));
		Model = glm::rotate(Model, 90.0f, glm::vec3(0, 1, 0));
		Model = glm::scale(Model, glm::vec3(1, 0.3, (groundSize + groundEdge) / 100));
		sendMVP();
		shaderHelper();
		wall.display();


		//draw the FoodDrops
		for (size_t i = 0; i < foodNumber; i++)
		{
			Model = Save;
			Model = glm::translate(Model, glm::vec3(randomFoodX[i], 0.0, randomFoodY[i]));
			Model = glm::scale(Model, glm::vec3(0.1, 0.1, 0.1));
			Model = glm::rotate(Model, i * 15.0f, glm::vec3(0, 1, 0));
			sendMVP();
			shaderHelper();
			food[i].display();
			//drawSphere(10, 10);
		}



		//draw the builded spheres
		//if (sphereScaling > 20) {
		//	
		//	Model = BallModel;
		//	sphereScaling = 0;
		//	posBalls.push_back((glm::vec3)BallModel[3]);
		//	sphereScale = ((glm::vec3)BallModel[0]);
		//	//posBalls[sphereNumber] = (glm::vec3)BallModel[3];;	
		//}


		// Bildende. 
		// Bilder werden in den Bildspeicher gezeichnet (so schnell wie es geht.). 
		// Der Bildspeicher wird mit der eingestellten Bildwiederholfrequenz (also z. B. 60Hz)
		// ausgelesen und auf dem Bildschirm dargestellt. Da beide Frequenzen nicht übereinstimmen, würde
		// man beim Auslesen auf unfertige Bilder stoßen. Das wäre als Flimmern auf dem Bildschirm zu
		// erkennen. (War bei älteren Grafikkarten tatsächlich so.)
		// Dieses Problem vermeidet man, wenn man zwei Bildspeicher benutzt, wobei in einen gerade
		// gemalt wird, bzw. dort ein neues Bild entsteht, und der andere auf dem Bildschirm ausgegeben wird.
		// Ist man mit dem Erstellen eines Bildes fertig, tauscht man diese beiden Speicher einfach aus ("swap").
		glfwSwapBuffers(window);

		// Hier fordern wir glfw auf, Ereignisse zu behandeln. GLFW könnte hier z. B. feststellen,
		// das die Mouse bewegt wurde und eine Taste betätigt wurde.
		// Da wir zurzeit nur einen "key_callback" installiert haben, wird dann nur genau diese Funktion
		// aus "glfwPollEvents" heraus aufgerufen.
		glfwPollEvents();


	}

	//textur loeschen
	glDeleteTextures(1, &TextureMandrill);

	// Wenn der Benutzer, das Schliesskreuz oder die Escape-Taste betätigt hat, endet die Schleife und
	// wir kommen an diese Stelle. Hier können wir aufräumen, und z. B. das Shaderprogramm in der
	// Grafikkarte löschen. (Das macht zurnot das OS aber auch automatisch.)
	glDeleteProgram(programID);

	// Schießen des OpenGL-Fensters und beenden von GLFW.
	glfwTerminate();

	return 0; // Integer zurückgeben, weil main so definiert ist
}

//Unser neuer Shader
//	kommt nicht mehr mit der zusammengefassten MVP‐Matrix alleine klar, sondern für die Beleuchtung
//	benötigt er die Einzelmatrizen.Deshalb müssen wir Sie ihm immer schicken, wenn wir unseren
//	Zustand im Adressraum mit seinem synchronisieren.Diese drei Zeilen gehören also nach „sendMVP“.
void shaderHelper()
{
	glUniformMatrix4fv(glGetUniformLocation(programID, "M"), 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &Projection[0][0]);
}

