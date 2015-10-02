//cs335 Spring 2015 Lab-1
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 4000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
	float x, y, z;
};

struct Shape {
	float width, height;
	float radius;
	Vec center;
};

struct Particle {
	Shape s;
	Vec velocity;
};

struct Game {
	Shape box[4];
	Shape circle;
	Particle particle[MAX_PARTICLES];
	int n;
	int lastMousex, lastMousey;
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);


int main(void)
{
	int done=0;
	srand(time(NULL));
	initXWindows();
	init_opengl();
	//declare game object
	Game game;
	game.n=0;

	//declare a box shape
	//game.box[0].width = 100;
	//game.box[0].height = 50;
	//game.box[0].center.x = 120 + 5*65;
	//game.box[0].center.y = 500 - 5*60;
	
	game.box[0].width = 100;
	game.box[0].height = 25;
	game.box[0].center.x = 100;
	game.box[0].center.y = 500;
	
	game.box[1].width = 100;
	game.box[1].height = 25;
	game.box[1].center.x = 250;
	game.box[1].center.y = 400;
	
	game.box[2].width = 100;
	game.box[2].height = 25;
	game.box[2].center.x = 400;
	game.box[2].center.y = 300;
	
	game.box[3].width = 100;
	game.box[3].height = 25;
	game.box[3].center.x = 550;
	game.box[3].center.y = 200;
	
	game.circle.center.x = 725;
	game.circle.center.y = 20;
	game.circle.radius = 125;

	//start animation
	while(!done) {
		while(XPending(dpy)) {
			XEvent e;
			XNextEvent(dpy, &e);
			//check_mouse(&e, &game);
			done = check_keys(&e, &game);
			//makeParticle(&game, 25, 600);
		}
		
		movement(&game);
		render(&game);
		glXSwapBuffers(dpy, win);
	}
	cleanupXWindows();
	return 0;
}

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "335 Lab1   LMB for particle");
}

void cleanupXWindows(void) {
	//do not change
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void) {
	//do not change
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cout << "\n\tcannot connect to X server\n" << std::endl;
		exit(EXIT_FAILURE);
	}
	Window root = DefaultRootWindow(dpy);
	XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
	if(vi == NULL) {
		std::cout << "\n\tno appropriate visual found\n" << std::endl;
		exit(EXIT_FAILURE);
	} 
	Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	XSetWindowAttributes swa;
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
							ButtonPress | ButtonReleaseMask |
							PointerMotionMask |
							StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
					InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);
}

#define rnd() (float)rand() / (float)RAND_MAX

void makeParticle(Game *game, int x, int y)
{

	std::cout << "makeParticle() " << x << " " << y << std::endl;
	//position of particle
	while (game->n <= MAX_PARTICLES) {
	Particle *p = &game->particle[game->n];
	p->s.center.x = x;
	p->s.center.y = y;
	p->velocity.y = 0 + rnd()*1.0 - 0.5;
	p->velocity.x =  1.0 + rnd()*0.1;
	game->n++;
	
	  	if (game->n >= MAX_PARTICLES) {
		return;
		}
	}
}

void check_mouse(XEvent *e, Game *game)
{

	makeParticle(game, 25, 600);
	return;
  
	/*static int savex = 0;
	static int savey = 0;

	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed
			int y = WINDOW_HEIGHT - e->xbutton.y;
			for (int i=0; i<10; i++) {
			  makeParticle(game, e->xbutton.x, y);
			}
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed
			return;
		}
	}
	//Did the mouse move?
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		savex = e->xbutton.x;
		savey = e->xbutton.y;
		int y = WINDOW_HEIGHT - e->xbutton.y;
		for (int i=0; i<10; i++) {
			makeParticle(game, e->xbutton.x, y);
		}
		//if (++n < 10)
		//	return;
		game->lastMousex = e->xbutton.x;
		game->lastMousey = y;
	}*/
}

int check_keys(XEvent *e, Game *game)
{
	//Was there input from the keyboard?
	if (e->type == KeyPress) {
		int key = XLookupKeysym(&e->xkey, 0);
		if (key == XK_Escape) {
			return 1;
		}
		
		if (key == XK_b) {
			while
			makeParticle(game, 25, 600);
		}
		//You may check other keys here.

	}
	return 0;
}

void movement(Game *game)
{
	Particle *p;

	if (game->n <= 0)
		return;
	
	for (int i=0; i<10; i++) {
	  makeParticle(game, game->lastMousex, game->lastMousey);
	}

	for (int i = 0; i < game->n; i++) {
	  p = &game->particle[i];
	  p->s.center.x += p->velocity.x;
	  p->s.center.y += p->velocity.y;
	  p->velocity.y -= GRAVITY;

	  //check for collision with shapes...
	  for (int j=0; j<4; j++) {
	    	  Shape *s = &game->box[j];
	  
	    if (p->s.center.y < s->center.y + s->height &&
		p->s.center.y > s->center.y - s->height &&
		p->s.center.x >= s->center.x - s->width &&
		p->s.center.x <= s->center.x + s->width) {
	      p->velocity.y *= -0.4;
	      p->s.center.y = s->center.y + s->height + 0.01;
	    }    
	  }
	  
	 //check for circle collision
	  
	  float d0,d1,dist;
	  d0 = p->s.center.x - game->circle.center.x;
	  d1 = p->s.center.y - game->circle.center.y;
	  dist = sqrt(d0*d0 + d1*d1);
	  if (dist <= game->circle.radius) {
	    //p->velocity.y *= -1; 
	    //float v[2];
	    d0 /= dist;
	    d1 /= dist;
	    d0 *= game->circle.radius * 1.01; 
	    d1 *= game->circle.radius * 1.01;
	    p->s.center.x = game->circle.center.x + d0;
	    p->s.center.y = game->circle.center.y + d1;
	    p->velocity.x += d0 * 0.01;
	    p->velocity.y += d1 * 0.01;
	  }
	  
	  
	  //check for off-screen
	  if (p->s.center.y < 0.0 || p->s.center.y > WINDOW_HEIGHT) {
		  std::cout << "off screen" << std::endl;
		  memcpy(&game->particle[i], &game->particle[game->n-1], sizeof(Particle));
		  game->n--;
	  }
	}
	
}

void render(Game *game)
{
  float w, h;
  glClear(GL_COLOR_BUFFER_BIT);
  //Draw shapes...

  //draw circle
  static int firsttime=1;
  static int vertices[150][2];
  static int n = 150;
    if (firsttime) {
    float angle = 0.0;
    float inc = (3.14159 * 2.0) / (float)n;
    for (int i=0; i<n; i++) {
      vertices[i][0] = cos(angle) * game->circle.radius + 725;
      vertices[i][1] = sin(angle) * game->circle.radius + 20;
      angle += inc;
      
    }
	
    firsttime = 0;
	  
    }
    
  glColor3ub(90,140,90);
  glPushMatrix();
  glBegin(GL_TRIANGLE_FAN);
    for (int i=0; i<n; i++) {
      glVertex2i(vertices[i][0], vertices[i][1]);
    }
  glEnd();
  glPopMatrix();
	
	
	
	//draw box
	for (int j=0; j<4; j++) { //a loop to make four boxes
	Shape *s;
	glColor3ub(90,140,90);
	
	s = &game->box[j];
	
	glPushMatrix();
	glTranslatef(s->center.x, s->center.y, s->center.z);
	w = s->width;
	h = s->height;
	glBegin(GL_QUADS);
	  glVertex2i(-w,-h);
	  glVertex2i(-w, h);
	  glVertex2i( w, h);
	  glVertex2i( w,-h);
	glEnd();
	glPopMatrix();
	}

	for (int i = 0; i < game->n; i++) {
	  //draw all particles here
	  glPushMatrix();
	  glColor3ub(150,160,220);
	  Vec *c = &game->particle[i].s.center;
	  w = 2;
	  h = 2;
	  glBegin(GL_QUADS);
		  glVertex2i(c->x-w, c->y-h);
		  glVertex2i(c->x-w, c->y+h);
		  glVertex2i(c->x+w, c->y+h);
		  glVertex2i(c->x+w, c->y-h);
	  glEnd();
	  glPopMatrix();
	}
}



