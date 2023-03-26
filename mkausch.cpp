/********************************************************************
*
*		Program: mkausch.cpp
*		Authors: Michael Kausch (Group 1 - DAHM)
*		Date Code Originated: 2/17/23
***********************************************************************/

/***************************************************************************
*       Classes: Menu
*       Uses:
*           Creates a menu object based on num text params, size and position
******************************************************************************/

//                 		INCLUDES

#include <iostream>
#include <string>
#include </usr/include/AL/alut.h>
#include <math.h>
#include <new>
#include <sstream>
#include <GL/glx.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <time.h>
#include <cstdlib>

#include "Global.h"
#include "mkausch.h"
//#include "hzhang.h"
#include "aparriott.h"


using namespace std;

#define BSIZE 5
#define PADDING 20
#define PI 3.1415926535

// #define USE_OPENAL_SOUND



Menu::Menu(unsigned int _n_texts,
            float w, float h,
            float x, float y,
            std::string* _words,
            int _centering)
    : n_texts{_n_texts}, pos{x, y, 0}, centering(_centering)
{
    // dynamially allocate boxes/rects for text display
    mainbox.setDim(w, h);
    mainbox.setPos(pos[0], pos[1], pos[2]);
    mainbox.setColor(47, 61, 64);
    boarder.setDim(w+BSIZE, h+BSIZE);
    boarder.setPos(pos[0], pos[1], pos[2]);
    boarder.setColor(69, 85, 89);


    try {
        t_boxs = new Box[n_texts];
        texts = new Rect[n_texts];
        words = new std::string[n_texts];


        float spacing = (2*h)/(n_texts+1);
        std::cout << "spacing: " << spacing << std::endl;

        for(int i = 0; i < n_texts; i++) {
            t_boxs[i].w = mainbox.w - PADDING;
            t_boxs[i].h = (spacing/2.0) - BSIZE;
            t_boxs[i].pos[0] = mainbox.pos[0];
            t_boxs[i].pos[1] = (pos[1]+mainbox.h)-((i+1)*spacing);
            t_boxs[i].setColor(61, 90, 115);
            t_boxs[i].id=i; // set box id for words array check
            std::cout << "t_box[" << i << "].pos[1]: "
                        << t_boxs[i].pos[1] << std::endl;

            words[i] = _words[i];
            // t_boxs[i].set_text(_words[i]);

            // Leaving this here for the next poor soul that
            // comes accross this issue:

            // apparently you need to set their position every single time
            // you print them or they fall off the screen for some stupid reason
            // so this code will now be found below in the draw() function

            // texts[i].bot = mainbox.pos[1]-(i*20);
            // texts[i].left = t_boxs[i].pos[0];
            // texts[i].center = 1;
        }

    } catch (std::bad_alloc& ba) {
        // if one was allocated and not the other than delete the one that
        if (texts) delete [] texts;
        if (t_boxs) delete [] t_boxs;
        // if (words) delete [] words;
        // print to screen for now until we have logging set up
        std::cerr << "Error allocating rectangles in Menu call\n"
                << ba.what() << '\n';
        texts = nullptr;
        t_boxs = nullptr;
        // words = nullptr;
    }
}

Menu::~Menu()
{
    if (texts)
        delete [] texts;

    if (t_boxs)
        delete [] t_boxs;

    if (words)
        delete [] words;

}

void Menu::draw()
{
    // draw boarder

    glColor3ubv(boarder.color);

    glPushMatrix();
    glTranslatef(boarder.pos[0], boarder.pos[1], boarder.pos[2]);
    glBegin(GL_QUADS);
        glVertex2f(-boarder.w, -boarder.h);
        glVertex2f(-boarder.w,  boarder.h);
        glVertex2f( boarder.w,  boarder.h);
        glVertex2f( boarder.w, -boarder.h);
    glEnd();
    glPopMatrix();

    // draw mainbox

    glColor3ubv(mainbox.color);

    glPushMatrix();
    glTranslatef(mainbox.pos[0], mainbox.pos[1], mainbox.pos[2]);
    glBegin(GL_QUADS);
        glVertex2f(-mainbox.w, -mainbox.h);
        glVertex2f(-mainbox.w,  mainbox.h);
        glVertex2f( mainbox.w,  mainbox.h);
        glVertex2f( mainbox.w, -mainbox.h);
    glEnd();
    glPopMatrix();

    // draw all t_boxes

    for(size_t i = 0; i < n_texts; i++) {
        glColor3ubv(t_boxs[i].color);

        glPushMatrix();
        glTranslatef(t_boxs[i].pos[0], t_boxs[i].pos[1], 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(-t_boxs[i].w, -t_boxs[i].h);
            glVertex2f(-t_boxs[i].w,  t_boxs[i].h);
            glVertex2f( t_boxs[i].w,  t_boxs[i].h);
            glVertex2f( t_boxs[i].w, -t_boxs[i].h);
        glEnd();
        glPopMatrix();

    }

    // draw texts - need to pass in texts still; this is only for testing

    // for(int i = 0; i < n_texts; i++) {
    //     ggprint8b((texts+i), 16, 0x00FFFFFF, "c0sm0t0asT");
    //     // std::cout << "tests[" << i << "].pos[1]: " << texts[i].pos[1] << std::endl;
    // }


    for(int i = 0; i < n_texts; i++) {
        texts[i].bot = t_boxs[i].pos[1] - 5;
        if (!centering)
            texts[i].left = t_boxs[i].pos[0]-t_boxs[i].w + 100;
        else
            texts[i].left = t_boxs[i].pos[0];
        texts[i].center = centering;


        // r[i].bot = t_boxs[i].pos[1] - 5;
        // r[i].left = t_boxs[i].pos[0];
        // //g.r[i].center = box[i].pos[0];

        // r[i].center = 1;

        ggprint8b(texts+i, 16, 0x00ffffff, words[i].c_str());
    }

}

// pass in mouse coords to check and see if they are within the bounds
// of the menu's text boxes
Box* Menu::check_t_box(int x, int y)
{
    Box * box_ptr = nullptr;
    size_t i;

    for (i = 0; i < n_texts; i++) {
        // mouse coords are
        if ((x > (t_boxs[i].pos[0]-t_boxs[i].w)) &&
            (x < (t_boxs[i].pos[0]+t_boxs[i].w)) &&
            (y > (t_boxs[i].pos[1]-t_boxs[i].h)) &&
            (y < (t_boxs[i].pos[1]+t_boxs[i].h))) {

            box_ptr = (t_boxs+i);
            break;
        }
    }

    // std::cout << "match for " << box_ptr << " aka " << t_boxs+i << std::endl;

    return box_ptr;
}

void Menu::set_highlight(Box * b)
{
    b->setColor(33,136,171);
}


void Menu::set_orig_color()
{
    for (size_t i = 0; i < n_texts; i++) {
        t_boxs[i].setColor(61,90,115);
    }
}



void Menu::setBcolor(int r, int g, int b)
{
    bcolor[0] = (char)r;
    bcolor[1] = (char)g;
    bcolor[2] = (char)b;
}

void Menu::setColor(int r, int g, int b)
{
    color[0] = (char)r;
    color[1] = (char)g;
    color[2] = (char)b;
}

void Menu::setPos(float x, float y, float z)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

std::string Menu::getInfo()
{
    std::ostringstream temp;
    temp <<             std::endl;

    return temp.str();
}


/***************************************************************************
*       Class: Timer
*       Uses:
*           Creates a Timer object with duration based on input
******************************************************************************/

Timer::Timer() : duration(-1), pause_duration(0.00),
                pause_timer(nullptr), paused(false)
{    // set starting time
    start = std::chrono::system_clock::now();
}

Timer::Timer(double sec) : duration(sec), pause_duration(0.00),
                pause_timer(nullptr), paused(false)
{    // set starting time
    start = std::chrono::system_clock::now();
}

// delete pause timer if it were active
Timer::~Timer()
{
    cerr << "in Timer destructor\n";
    if (pause_timer){
        cerr << "deleting pause timer\n";
        delete pause_timer;
        pause_timer = nullptr;
    }
}

/****************************** Setters *************************************/

// resets timer to current time
void Timer::reset()
{
    if (isPaused()) {
        unPause();
    }
    pause_duration = 0;
    start = std::chrono::system_clock::now();
}

/****************************** Getters *************************************/

// returns time that has elapsed since the start of the timer
int Timer::getTime(char time_code)
{
    int net_time = 0;
    int time = net_time;
    std::chrono::duration<double> total_elapsed = std::chrono::system_clock::now() - start;

    if (paused) {
        net_time = (total_elapsed.count() - pause_duration - pause_timer->getTime('n'));
    } else {
        net_time = (total_elapsed.count()-pause_duration);
    }

    // D.T - retrieve minutes, seconds, or net time
    // based on time_code passed in getTime parameter
    switch(time_code) {
        case 'm': time = net_time/60;
                  break;
        case 's': time = net_time % 60;
                  break;
        case 'n': time = net_time;
    }
    return time;
}

// checks if the timer has elapsed
// true if the timer has finished
// false if the timer hasn't
bool Timer::isDone()
{
    if (duration == -1) {   // return false for count up timers
        return false;
    } else {    // return net time for countdown timers
        return (getTime('n') > duration);
    }
}

void Timer::pause()
{
    paused = true;
    pause_timer = new Timer();
}

bool Timer::isPaused()
{
    return paused;
}

void Timer::unPause()
{
    if (paused) {
        paused = false;
        pause_duration += pause_timer->getTime('n');
        delete pause_timer;
        pause_timer = nullptr;
    }

}

#ifdef USE_OPENAL_SOUND
Sound::Sound()
{

    //Buffer holds the sound information.
    init_openal();
    current_track = -1;  // starting track number at splash screen
    is_music_paused = false;
    user_pause = false;
    is_intro = is_game = false;

    // make individual buffers of all sounds
    alBuffers[0] = alutCreateBufferFromFile(build_song_path(sound_names[0]).c_str());
    alBuffers[1] = alutCreateBufferFromFile(build_song_path(sound_names[1]).c_str());
    alBuffers[2] = alutCreateBufferFromFile(build_song_path(sound_names[2]).c_str());
    alBuffers[3] = alutCreateBufferFromFile(build_song_path(sound_names[3]).c_str());
    alBuffers[4] = alutCreateBufferFromFile(build_song_path(sound_names[4]).c_str());
    alBuffers[5] = alutCreateBufferFromFile(build_song_path(sound_names[5]).c_str());
    alBuffers[6] = alutCreateBufferFromFile(build_song_path(sound_names[6]).c_str());
    alBuffers[7] = alutCreateBufferFromFile(build_song_path(sound_names[7]).c_str());
    alBuffers[8] = alutCreateBufferFromFile(build_song_path(sound_names[8]).c_str());
    alBuffers[9] = alutCreateBufferFromFile(build_song_path(sound_names[9]).c_str());
    alBuffers[10] = alutCreateBufferFromFile(build_song_path(sound_names[10]).c_str());
    alBuffers[11] = alutCreateBufferFromFile(build_song_path(sound_names[11]).c_str());
    alBuffers[12] = alutCreateBufferFromFile(build_song_path(sound_names[12]).c_str());
   alBuffers[13] = alutCreateBufferFromFile(build_song_path(sound_names[13]).c_str());
    alBuffers[14] = alutCreateBufferFromFile(build_song_path(sound_names[14]).c_str());

    // songBuffers[0] = alBuffers[3];
    buffersDone = buffersQueued = 0;

    // generate number of sources
    alGenSources(NUM_SOUNDS, alSources);    // keep individual sources for now
    alGenSources(1, &menuQueueSource);

    alSourceQueueBuffers(menuQueueSource, 1, (alBuffers+1));
    alSourcef(menuQueueSource, AL_GAIN, 1.0f);
    alSourcef(menuQueueSource, AL_PITCH, 1.0f);

    alSourcei(alSources[0], AL_BUFFER, alBuffers[0]);   // bullet_fire
    alSourcef(alSources[0], AL_GAIN, 1.0f);
    alSourcef(alSources[0], AL_PITCH, 1.0f);
    alSourcei(alSources[0], AL_LOOPING, AL_TRUE);

    alSourcei(alSources[1], AL_BUFFER, alBuffers[1]);   // intro
    alSourcef(alSources[1], AL_GAIN, 0.8f);
    alSourcef(alSources[1], AL_PITCH, 1.0f);
    alSourcei(alSources[1], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[2], AL_BUFFER, alBuffers[2]);   // beep
    alSourcef(alSources[2], AL_GAIN, 1.0f);
    alSourcef(alSources[2], AL_PITCH, 1.0f);
    alSourcei(alSources[2], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[3], AL_BUFFER, alBuffers[3]); // boop
    alSourcef(alSources[3], AL_GAIN, 1.0f);
    alSourcef(alSources[3], AL_PITCH, 1.0f);
    alSourcei(alSources[3], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[4], AL_BUFFER, alBuffers[4]); // intro's loop
    alSourcef(alSources[4], AL_GAIN, 0.8f);
    alSourcef(alSources[4], AL_PITCH, 1.0f);
    alSourcei(alSources[4], AL_LOOPING, AL_TRUE);

    alSourcei(alSources[5], AL_BUFFER, alBuffers[5]); // AdhesiveWombat song
    alSourcef(alSources[5], AL_GAIN, 0.8f);
    alSourcef(alSources[5], AL_PITCH, 1.0f);
    alSourcei(alSources[5], AL_LOOPING, AL_TRUE);

    alSourcei(alSources[6], AL_BUFFER, alBuffers[6]); // zap1
    alSourcef(alSources[6], AL_GAIN, 1.0f);
    alSourcef(alSources[6], AL_PITCH, 1.0f);
    alSourcei(alSources[6], AL_LOOPING, AL_TRUE);


    alSourcei(alSources[7], AL_BUFFER, alBuffers[7]); // zap2
    alSourcef(alSources[7], AL_GAIN, 1.0f);
    alSourcef(alSources[7], AL_PITCH, 1.0f);
    alSourcei(alSources[7], AL_LOOPING, AL_TRUE);


    alSourcei(alSources[8], AL_BUFFER, alBuffers[8]); // zap3
    alSourcef(alSources[8], AL_GAIN, 1.0f);
    alSourcef(alSources[8], AL_PITCH, 1.0f);
    alSourcei(alSources[8], AL_LOOPING, AL_TRUE);


    alSourcei(alSources[9], AL_BUFFER, alBuffers[9]); // zap4
    alSourcef(alSources[9], AL_GAIN, 1.0f);
    alSourcef(alSources[9], AL_PITCH, 1.0f);
    alSourcei(alSources[9], AL_LOOPING, AL_TRUE);

    alSourcei(alSources[10], AL_BUFFER, alBuffers[10]); // shield
    alSourcef(alSources[10], AL_GAIN, 1.0f);
    alSourcef(alSources[10], AL_PITCH, 1.0f);
    alSourcei(alSources[10], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[11], AL_BUFFER, alBuffers[11]); // doosh
    alSourcef(alSources[11], AL_GAIN, 1.0f);
    alSourcef(alSources[11], AL_PITCH, 1.0f);
    alSourcei(alSources[11], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[12], AL_BUFFER, alBuffers[12]); // doosh2
    alSourcef(alSources[12], AL_GAIN, 3.0f);
    alSourcef(alSources[12], AL_PITCH, 1.0f);
    alSourcei(alSources[12], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[13], AL_BUFFER, alBuffers[13]); // doosh
    alSourcef(alSources[13], AL_GAIN, 1.0f);
    alSourcef(alSources[13], AL_PITCH, 1.0f);
    alSourcei(alSources[13], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[14], AL_BUFFER, alBuffers[14]); // doosh2
    alSourcef(alSources[14], AL_GAIN, 3.0f);
    alSourcef(alSources[14], AL_PITCH, 1.0f);
    alSourcei(alSources[14], AL_LOOPING, AL_FALSE);


    // //Generate a source, and store it in a buffer.
    // // set sfx/songs to not loop
    // int non_looping_sounds = 4;
    // for (int i = 0; i < non_looping_sounds; i++) {
    //     alSourcei(alSources[i], AL_BUFFER, alBuffers[i]);
    //     alSourcef(alSources[i], AL_GAIN, 1.0f);
    //     alSourcef(alSources[i], AL_PITCH, 1.0f);
    //     alSourcei(alSources[i], AL_LOOPING, AL_FALSE);
    // }

    // // make all songs to loop
    // for (int i = non_looping_sounds; i < NUM_SOUNDS; i++) {
    //     alSourcei(alSources[i], AL_BUFFER, alBuffers[i]);
    //     alSourcef(alSources[i], AL_GAIN, 0.25f);
    //     alSourcef(alSources[i], AL_PITCH, 1.0f);
    //     alSourcei(alSources[i], AL_LOOPING, AL_TRUE);
    // }
    //   [[------- END OLD -----------]]


    // check for errors after setting sources
    if (alGetError() != AL_NO_ERROR) {
        throw "ERROR: setting source\n";
    }
}

Sound::~Sound()
{
    for (int i = 0; i < NUM_SOUNDS; i++) {
        // delete sources
        alDeleteSources(i+1, (alSources+i));
        // delete buffers
        alDeleteBuffers(i+1, (alBuffers+i));
    }

    alDeleteSources(1, &menuQueueSource);

    close_openal();

}

void Sound::init_openal()
{
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
		throw "ERROR: alutInit()\n";
		// printf("ERROR: alutInit()\n");
		// return 0;
	}

	//Clear error state.
	alGetError();

	//Setup the listener.
	//Forward and up vectors are used.
	float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
}

void Sound::close_openal()
{
	//Close out OpenAL itself.
	//Get active context.
	ALCcontext *Context = alcGetCurrentContext();
	//Get device for active context.
	ALCdevice *Device = alcGetContextsDevice(Context);
	//Disable context.
	alcMakeContextCurrent(NULL);
	//Release context(s).
	alcDestroyContext(Context);
	//Close device.
	alcCloseDevice(Device);
}

void Sound::rewind_game_music()
{
    alSourceStop(alSources[5]);
    alSourceRewind(alSources[5]);
    alSourcePlay(alSources[5]);
}

string Sound::build_song_path(string s)
{
    // format of the song
    // ./Songs/Edzes-64TheMagicNumber16kHz.wav"

    ostringstream path;
    string song_dir = "Songs";

    path << "./" << song_dir << "/" << s;

    return path.str();

}

void Sound::gun_play(int btype)
{
    // static int gun_start = 6;

    cerr << "gun shooting..." << endl;
    if (btype == 1) {
        alSourcePlay(alSources[6]);
    } else if (btype == 2) {
        alSourcePlay(alSources[7]);
        // alSourcePlay(alSources[gun_start+1]);
    } else if (btype == 3) {
        alSourcePlay(alSources[8]);
        // alSourcePlay(alSources[gun_start+1]);
        // alSourcePlay(alSources[gun_start+2]);
    } else if (btype == 4) {
        alSourcePlay(alSources[9]);
        // alSourcePlay(alSources[gun_start+3]);
    }
}

void Sound::gun_stop()
{
    static int gun_start = 6;
    static int num_guns = 4;
    cerr << "gun not shooting..." << endl;
    for (int i = 0; i < num_guns; i++) {
        alSourceStop(alSources[i+gun_start]);
    }
}

void Sound::beep()
{
    alSourcePlay(alSources[2]);
}

void Sound::boop()
{
    alSourcePlay(alSources[3]);
}

void Sound::doosh()
{
    int index = 12; // index of doosh sound effect 11 or 12 currently
    alSourcePlay(alSources[index]);
}


void Sound::shieldSFX()
{
    int index = 10; // index of doosh sound effect 
    alSourcePlay(alSources[index]);
}

void Sound::exploSFX()
{
    int index = 14; // index of doosh sound effect 13 or 14 currently
    alSourcePlay(alSources[index]);
}

bool Sound::check_intro_buffer_done()
{
    reset_buffer_done();
    alGetSourcei(menuQueueSource, AL_BUFFERS_PROCESSED, &buffersDone);
    // cerr << "checking intro buffer done, buffers is: " << buffersDone << endl;
    return (buffersDone == 1);
}

// resets buffers_done variable for further checks
void Sound::reset_buffer_done()
{
    buffersDone = 0;
}

// unqueue's intro beat so that only loop track is in the buffer queue
// loops buffer queue at this point
void Sound::loop_intro()
{

    alSourceStop(menuQueueSource);
    alSourceRewind(menuQueueSource);
    alSourcePlay(alSources[4]);
}

void Sound::setup_game_mode()
{
    // change bools for music state
    is_intro = false; is_game = true;

    // stop both the intro and loop if either are playing
    alSourceStop(alSources[4]);
    alSourceStop(menuQueueSource);
    alSourceRewind(alSources[4]);
    alSourceRewind(menuQueueSource);

    // play the game song
    alSourcePlay(alSources[5]);
}

void Sound::play_start_track()
{
    // stop game music if it's playing
    if (is_game == true) {
        alSourceStop(alSources[5]);
        alSourceRewind(alSources[5]);
    }

    is_intro = true; is_game = false;

    // begin playing menu music
    alSourcePlay(menuQueueSource);
}

// returns song names, only 2 songs for now
string Sound::get_song_name()
{
    string name;
    if (is_intro) {
        name = "Edzes-64TheMagicNumber";
    }
    if (is_game) {
        name = sound_names[5];
    }
    return name;
}

// pauses song (when going to pause menu for instance)
void Sound::pause()
{
    if (!is_music_paused) {
        is_music_paused = true;
        alSourcePause(alSources[5]);
    }
}

// unpauses
void Sound::unpause()
{
    if (!user_pause) {
        if (is_music_paused) {
            is_music_paused = false;
            alSourcePlay(alSources[5]);
        }
    }
}

// separate pause state for when user explicity mutes music
void Sound::toggle_user_pause()
{
    user_pause = (user_pause == true) ? false : true;
    if (user_pause)
        pause();
    else
        unpause();
}

// getter to return the pause state
bool Sound::get_pause()
{
    return is_music_paused;
}

#endif

PowerBar::PowerBar(const Item & _itm_, PBType _type_, float x, float y)
{
    // maybe put max_health of each enemy type in case were going to
    // use this healthbar for the boss as well
    itm = &_itm_;
    type = _type_;

    if (type == HEALTH) {
        total.setColor(255,0,0);   // set lost health to red
        health.setColor(0,255,0);  // set health to green
        total.setDim(75,10);
        total.setPos(x, y, 0);
    } else if (type == COOLDOWN) {
        total.setColor(108,122,137);
        health.setColor(0,0,0);
        total.setDim(75,4);
        total.setPos(x, y, 0);
    }
    // mimic other bar based on what health was set to
    health.setDim(total.w,total.h);
    health.setPos(total.pos[0],total.pos[1],total.pos[2]);

    text.bot = total.pos[1]-5;
    text.left = total.pos[0];
    text.center = 1;
    cerr << "finished itm constructor" << endl;
}

PowerBar::PowerBar(const Toaster & _tos_, PBType _type_, float x, float y)
{
    // maybe put max_health of each enemy type in case were going to 
    // use this healthbar for the boss as well
    tos = &_tos_;
    itm = &_tos_;
    type = _type_;

    if (type == HEALTH) {
        total.setColor(255,0,0);   // set lost health to red
        health.setColor(0,255,0);  // set health to green
        total.setDim(75,10);
        total.setPos(x, y, 0);
    } else if (type == COOLDOWN) {
        total.setColor(196,145,2);
        health.setColor(255,255,0);
        total.setDim(75,8);
        total.setPos(x, y, 0);
    }
    // mimic other bar based on what health was set to
    health.setDim(total.w,total.h);
    health.setPos(total.pos[0],total.pos[1],total.pos[2]);

    text.bot = total.pos[1]-5;
    text.left = total.pos[0];
    text.center = 1;

    cerr << "finished tos constructor" << endl;
}


void PowerBar::draw()
{
    static int max_energy = 100;
    
    if (type == HEALTH) {
        glColor3ubv(total.color);
        glPushMatrix();
        glTranslatef(total.pos[0], total.pos[1], total.pos[2]);
        glBegin(GL_QUADS);
            glVertex2f(-total.w, -total.h);
            glVertex2f(-total.w,  total.h);
            glVertex2f( total.w,  total.h);
            glVertex2f( total.w, -total.h);
        glEnd();
        glPopMatrix();

        // draw mainbox
        // hp_resize();
        glColor3ubv(health.color);
        
        glPushMatrix();
        glTranslatef(health.pos[0]-health.w, health.pos[1], health.pos[2]);
        glBegin(GL_QUADS);
            glVertex2f(0, -health.h);
            glVertex2f(0,  health.h);
            glVertex2f( (((float)(itm->hp))/(float)(itm->starting_hp))*2.0f*health.w,  health.h);
            glVertex2f( (((float)(itm->hp))/(float)(itm->starting_hp))*2.0f*health.w, -health.h);
            
        glEnd();
        glPopMatrix();

        ggprint8b(&text, 0, 0x00000000, "%i/%i  Lives: %i", (int)(itm->hp), itm->starting_hp, itm->lives);
    } else if (type == COOLDOWN) {

        glColor3ubv(total.color);
        glPushMatrix();
        glTranslatef(total.pos[0], total.pos[1], total.pos[2]);
        glBegin(GL_QUADS);
            glVertex2f(-total.w, -total.h);
            glVertex2f(-total.w,  total.h);
            glVertex2f( total.w,  total.h);
            glVertex2f( total.w, -total.h);
        glEnd();
        glPopMatrix();



        glColor3ubv(health.color);
        glPushMatrix();
        glTranslatef(health.pos[0]-health.w, health.pos[1], health.pos[2]);
        glBegin(GL_QUADS);
            glVertex2f(0, -health.h);
            glVertex2f(0,  health.h);
            glVertex2f( (((tos->energy))/((float)(max_energy)))*2.0f*health.w,  health.h);
            glVertex2f( (((tos->energy))/((float)(max_energy)))*2.0f*health.w, -health.h);
            
        glEnd();
        glPopMatrix();

        ggprint8b(&text, 0, 0x00FF0000, "Jump Energy: %i/%i", (int)tos->energy, max_energy);
        // cerr << "tos->energy: " << tos->energy << " max_energy: " << max_energy << endl;
    }

}

// modified from hzhang's file by mkausch
bool Entity::collision(Item & a)
{
    // for (x0,y0,x1,y1)and(x2,y2,x3,y3) squares
    // if collison -->(x0-x3)*(x1-x2)<0
    // same for y
    bool x = (((pos[0]+dim[0])-(a.pos[0]-a.w))*((pos[0]-dim[0])-(a.pos[0]+a.w))) < 0;
  	bool y = (((pos[1]+dim[1])-(a.pos[1]-a.h))*((pos[1]-dim[1])-(a.pos[1]+a.h))) < 0;
  	return x&&y;
}

void Entity::setHP(float life)
{
    hp = life;
}

void Entity::setDamage(float x)
{
    damage = x;
}

void Entity::hpDamage(Item & a) {
    hp = hp - a.damage;
}

bool Entity::hpCheck()
{
    return (hp <= 0);
}

void Item::hpDamage(Entity & e)
{
    hp = hp - e.damage;
}

Blocky::Blocky(char type)
{
    srand(time(NULL));
    if (type == 'v') {
        setDim(25.0f, 100.0f);
    } else if (type == 'h') {
        setDim(100.0f, 25.0f);
    }
    set_rand_color(*this);
    set_rand_position();
    setAcc(0.0f,-0.25f,0.0f);
    setVel(0.0f, -4.0f, 0.0f);
    setDamage(20);
    starting_hp = 10;
    setHP(starting_hp);
    point = starting_hp;
    was_hit = false;
    lives = 2;
    explode_done = true;

    // sub box assignment
    // assignes itself and it's mirror image (i+4 in this case)
    int angle = 80;
    int rvel = 8;
    float deg_to_rad = (PI / 180.0f);
    for (int i = 0; i < 4; i++) {
        // set dim
        sub_boxes[i].setDim(w/2.0f, h/4.0f);   // should create a box 1/8 size
        sub_boxes[i+4].setDim(w/2.0f, h/4.0f);   // should create a box 1/8 size

        // set color
        sub_boxes[i].setColor(255,0,0);    // make them red for now
        sub_boxes[i+4].setColor(255,0,0);    // make them red for now

        // set accel
        // sub_boxes[i].set_acc(0, -0.25, 0);
        // sub_boxes[i+4].set_acc(0, -0.25, 0);
        sub_boxes[i].setAcc(0, 0, 0);
        sub_boxes[i+4].setAcc(0, 0, 0);

        // set angle first so we can calc vel vectors
        sb_angles[i] = angle;
        sb_angles[i+4] = -sb_angles[i];
        angle -= 20;

        // set velocity of x and y components based on above angle
        sub_boxes[i].setVel((rvel*cos(deg_to_rad * sb_angles[i])),
                                    (rvel*sin(deg_to_rad * sb_angles[i])), 0);
        sub_boxes[i+4].setVel((rvel*cos(deg_to_rad * sb_angles[i+4])),
                                    (rvel*sin(deg_to_rad * sb_angles[i+4])), 0);
    }


    init_rotation_vel();
}

Blocky::~Blocky()
{

}

void Blocky::init_rotation_vel()
{
    // 0 the starting angle and assign random change in rotation angle
    for (int i = 0; i < 8; i++) {
        rot_angle[i] = 0;
        rot_speed[i] = -40 + (rand() % 41);
    }
}

void Blocky::set_rand_position()
{
    static int pm_dir = 1;
    float curr_player_xpos = tos.pos[0];
    int delta_from_xpos = rand() % 50;
    float block_xpos = curr_player_xpos + (delta_from_xpos * pm_dir);

    // set to be this new random position situated near the player char
    // that is above the yres and out of view of the screen
    setPos(block_xpos, g.yres+h,0);

    // if this block was generated in front of the player then
    // next time make it randomly behind the player (it'll keep switching)
    pm_dir *= -1;

}

void set_rand_color(Item & it)
{
    // colors based on color scheme defined at the bottom
    // int color[5][3] = {{61, 89, 114},
    //                     {47, 61, 63},
    //                     {68, 84, 89},
    //                     {40, 63, 61},
    //                     {24, 38, 36}};

    static int color[5][3] =   {{242, 4, 159},
                        {4, 177, 216},
                        {4, 216, 78},
                        {242, 202, 4},
                        {242, 135, 4}};
    static int index = rand() % 5;
    it.setColor(color[index][0], color[index][1], color[index][2]);
    index = (index + 1) % 5;
}

bool Blocky::sub_ScreenIn()
{
    bool subs_onscreen = false;


    for (int i = 0; i < 8; i++) {
        subs_onscreen = sub_boxes[i].screenIn();
        if (subs_onscreen)
            break;
    }

    return subs_onscreen;
}

void Blocky::draw()
{
    // static int rot_angle = 0;
    // draw item

        // reset blocky if he's out of screen

    // draw big blocky
    if (is_alive() && explode_done) {
        if (screenOut()) {
            reset();
        }

        set_rand_color(*this);
        glPushMatrix();
        glColor3ub(color[0], color[1], color[2]);
        glTranslatef(pos[0], pos[1], pos[2]);
        glBegin(GL_QUADS);
                glVertex2f(-w, -h);
                glVertex2f(-w,  h);
                glVertex2f( w,  h);
                glVertex2f( w, -h);
        glEnd();
        glPopMatrix();


    } else {    // draw little blockies
        // cerr << "checking if sub boxes are in the screen...\n";
        if (sub_ScreenIn()) {

            for (int i = 0; i < 8; i++) {
                set_rand_color(sub_boxes[i]);
                glPushMatrix();
                glColor3ub(sub_boxes[i].color[0],
                            sub_boxes[i].color[1],
                            sub_boxes[i].color[2]);
                glTranslatef(sub_boxes[i].pos[0], sub_boxes[i].pos[1], sub_boxes[i].pos[2]);
                glMatrixMode(GL_MODELVIEW);
                glRotatef(rot_angle[i], 0, 0, 1.0);
                glBegin(GL_QUADS);
                        glVertex2f(-sub_boxes[i].w, -sub_boxes[i].h);
                        glVertex2f(-sub_boxes[i].w,  sub_boxes[i].h);
                        glVertex2f( sub_boxes[i].w,  sub_boxes[i].h);
                        glVertex2f( sub_boxes[i].w, -sub_boxes[i].h);
                glEnd();
                glPopMatrix();
                rot_angle[i] -= rot_speed[i];
            }
        } else {
            // rot_angle = 0;
            init_rotation_vel();
            explode_done = true;
            // reset_sub_boxes();
        }
    }
}

// void Blocky::reset_sub_boxes()
// {

// }

void Blocky::reset()
{
    if (hpCheck()) {
        lives--;
        explode();
        cerr << "explode called\n";
        explode_done = false;
        if (lives > 0) {
            hp = starting_hp;   // give back full health
        }
    }

    setVel(0.0f, -4.0f, 0.0f);
    set_rand_position();    // put at a new random position
    was_hit = false;
}

void Blocky::gamereset()
{
    lives = 2;
    hp = starting_hp;
    setVel(0.0f, -4.0f, 0.0f);
    set_rand_position();    // put at a new random position
    was_hit = false;
}

bool Blocky::did_damage()
{
    return was_hit;
}

void Blocky::move()
{
        // move main blocky
    if (is_alive() && explode_done) {
        pos[0] += vel[0];
        pos[1] += vel[1];
        pos[2] += vel[2];
        vel[0] += acc[0];
        vel[1] += acc[1];
        vel[2] += acc[2];
    } else if (!explode_done) { // move sub boxes until they fall off screen
        if (sub_ScreenIn()) {
            for (int i = 0; i < 8; i++) {
                sub_boxes[i].pos[0] += sub_boxes[i].vel[0];
                sub_boxes[i].pos[1] += sub_boxes[i].vel[1];
                sub_boxes[i].pos[2] += sub_boxes[i].vel[2];
                sub_boxes[i].vel[0] += sub_boxes[i].acc[0];
                sub_boxes[i].vel[1] += sub_boxes[i].acc[1];
                sub_boxes[i].vel[2] += sub_boxes[i].acc[2];
            }
        }
    }
}

void Item::hpDamage(Blocky & bf)
{
    if (!bf.did_damage()) {
        hp = hp - bf.damage;
        bf.set_hit();
    }
}

bool Blocky::is_alive()
{
    return (lives > 0);
}

void Blocky::set_hit()
{
    was_hit = true;
}

void Blocky::explode()
{
    int rvel = 8;
    float deg_to_rad = PI/180.0f;
    int pixel_offset = 8;   // sets origin of offset to be 8 left and 8 down
    int xcoord = pos[0] - pixel_offset;
    int ycoord = pos[1] - pixel_offset;
    int rand_offset;    // pixel_offset pixel offset randomly from center of blocky

    for (int i = 0; i < pixel_offset; i++) {
        rand_offset = rand() % (pixel_offset * 2);
        sub_boxes[i].setPos(pos[0]+rand_offset, pos[1]+rand_offset, 0);
        sub_boxes[i].setVel((rvel*cos(deg_to_rad * sb_angles[i])),
                                    (rvel*sin(deg_to_rad * sb_angles[i])), 0);
    }
}

#ifdef USE_OPENAL_SOUND

void check_sound(void)
{
	static bool initial_play = false;
	static bool loop_set = false;
	static bool initial_game_setup = false;
	static int prev_btype = 1;
    static int exploded = 0;


    // Main menu / music SFX loop check
	if (g.state == SPLASH || g.state == MAINMENU || g.state == GAMEOVER) {
		// init_game_setup will unque intro buffers and queue game songs
		initial_game_setup = false;	// switch to false if it was prev true
		if (initial_play == false) {
			cerr << "calling play_start_track()" << endl;
			sounds.play_start_track();	// queues intro songs and plays
			initial_play = true;
		}
		if (sounds.check_intro_buffer_done() && !loop_set) {
			// sounds.reset_buffer_done();
			cerr << "sounds.checkintobuffer == true" << endl;
			cerr << "calling loop_intro()" << endl;
			sounds.loop_intro();
			loop_set = true;
		}
	} else if (g.state == GAME && initial_game_setup == false) {
			// reset initial play so that intro plays
		initial_play = loop_set = false;
		initial_game_setup = true;
		cerr << "calling setup_game_mode()" << endl;
		sounds.setup_game_mode();

	}




	// *******     SFX NOISES      **********//

    if (g.state == GAME) {
        
        // start playing new sound if leveled up gun
        if ((tos.bullet_type_prime != prev_btype) && (sounds.gun_shooting)) {
            sounds.gun_stop();
            sounds.gun_play(tos.bullet_type_prime);
            prev_btype = tos.bullet_type_prime;
        }
        // if space bar is pressed down and gun not already shooting
        if ((g.keys[XK_space] == 1) && (!sounds.gun_shooting)) {
            cerr << "tos.bullet_type_prime: " << tos.bullet_type_prime << endl;
            sounds.gun_play(tos.bullet_type_prime);
            sounds.gun_shooting = true;
            // if spacebar not pressed down and gun noise currently set to shoot
        } else if (g.keys[XK_space] == 0 && (sounds.gun_shooting)) {
            sounds.gun_stop();
            sounds.gun_shooting = false;
        }


        if (blocky->explode_done == false && exploded == 0) {
            sounds.exploSFX();
            exploded = true;
        } else if (blocky->explode_done == true && exploded == 1) {
            exploded = false;
        }

    } else {
        if (sounds.gun_shooting == true) {
            sounds.gun_stop();
        }
    }


}
#endif

// directs enemies to be present during specified states
// also changes enemy settings if it's relevent
void check_level()
{
    static bool lvl_change = false;


    if (g.substate != DEBUG) {
        int level_duration = 10; // 20 second levels at the moment
        int level_time = g.gameTimer.getTime('n');
        

        static int lvl_change_time;


        // wait until the next clock tick
        if (lvl_change && lvl_change_time != level_time) {
            lvl_change = false;
            cerr << "lvl_change toggled to false\n";
            
        }

        if (g.state == GAME && 
            lvl_change == false && 
            level_time != 0 &&
            ((level_time % (level_duration)) == 0)) {

            lvl_change = true;
            cerr << "lvl change being toggled to true\n";
            lvl_change_time = level_time; 
            // level up handler
            switch (g.level) {
                case LEVEL1:
                    // Level2: Bread(2)
                    g.level = LEVEL2;
                    // change bread vars
                    break;
                case LEVEL2:
                    // Level3: Entities(1) + Bread(1)
                    g.level = LEVEL3;
                    g.entity_active = true;
                    break;
                case LEVEL3:
                    // Level4: Entities(2) + Bread(2)
                    g.level = LEVEL4;
                    g.entity_active = true;
                    // change entity vars
                    break;
                case LEVEL4:
                    // Level5: Blocky(1) + Bread(2) + Entities(2)
                    g.level = LEVEL5;
                    g.entity_active = true;
                    g.mike_active = true;
                    break;
                case LEVEL5:
                    // Level6: Blocky(2) + Bread(2) + Entities(2)
                    g.level = LEVEL6;
                    blocky->gamereset();
                    g.entity_active = true;
                    g.mike_active = true;
                    // change blocky vars
                    break;
                case LEVEL6:
                    // Level7: HBlocky(1) + Bread(2) + Entities(2)
                    g.level = LEVEL7;
                    g.entity_active = true;
                    blocky = &hblocky;
                    blocky_health = &hblocky_health;
                    // change blocky to horizontal
                    blocky->gamereset();
                    g.mike_active = true;
                    break;
                case LEVEL7:
                    // Level8: HBlocky(2) + Bread(2) + Entities(2)
                    g.level = LEVEL8;
                    g.entity_active = true;
                    // change HBlocky vars
                    blocky->gamereset();
                    g.mike_active = true;
                    break;
                case LEVEL8:
                    // Level9: Boss
                    g.level = LEVEL9;
                    // unleash bossman randy savage
                    g.huaiyu_active = true;

                    break;
                case LEVEL9:
                    // should transition to game over
                    // g.level = LEVEL1;
                    break;
                default:    // Level 1 behavior (Bread(1))   // shouldn't need
                    g.level = LEVEL1;

                    break;
            }
        }        
    }

    if (g.state == GAMEOVER) {
            g.level = LEVEL1;
            g.huaiyu_active = false;
            g.entity_active = false;
            g.dtorres_active = false;
            g.mike_active = false;
            blocky = &vblocky;
            blocky_health = &vblocky_health;
    }

}

HighScore::HighScore(string n, int s)
    : uname(n), score(s) { }

// overloaded < operator to compare scores in algorithm lib
bool HighScore::operator < (const HighScore & rhs)
{
    return (score < rhs.score);
}

// overloaded = operator used to see if it's exactly equal to the last
// score in the highscores list.. if it is then it keeps the new val
bool HighScore::operator == (const HighScore & rhs)
{
    return (score == rhs.score);
}

bool HighScore::operator == (int val)
{
    return score == val;
}

bool HighScore::operator == (string str)
{
    return uname == str;
}

Gamerecord::Gamerecord()
{
	bool read_success = getRecord();
	
	// make new blank record with fake names
	if (!read_success) {
		genFakeNames();	// generate and load fake scores into file so that there's always 10
		writeRecord();	// write file to disk so that it now exists for the future
	}

	memset(gamer,' ',9); gamer[9] = '\0';
	highscore = scores[scores.size()-1].score;
	user_score = nullptr;
    hs_menu = nullptr;
    place = -1;
    makeMenu();
}

Gamerecord::~Gamerecord()
{
    cerr << "constructor called..." << endl;
	writeRecord();
	if (user_score) {
		delete user_score;
		user_score = nullptr;
	}

    if (hs_menu) {
        delete hs_menu;
        hs_menu = nullptr;
    }

    cerr << "gamerecord constructor finishing..." << endl;
}

// reads highscores from local file and loads them into the scores vector
bool Gamerecord::getRecord()
{
	// ****--------------------------[[ TODO: ]]---------------------------****
	// replace this line of code with the query to odin to retrieve the high scores
	ifstream fin("Highscore.txt");

	if (!fin) {
		return false;
	}

	string user;
	int score;
	int count = 0;

	while (fin >> user >> score) {
        // cerr << user << " : " << score << endl;
		HighScore entry = HighScore(user, score);
		scores.push_back(entry);
		count++;
	}

	if (count < 10) {
		genFakeNames();
	}

	return true;

}

// writes top ten records to disk
void Gamerecord::writeRecord()
{
	ofstream fout("Highscore.txt");

	if (!fout) {
		throw "could not write to highscore file";
	}

	// only write top 10 scores
	for (int i = 0; i < scores.size(); i++) {
		fout << scores[i].uname << "\t" << scores[i].score;
        if (i != (scores.size() - 1)){
            fout << endl;
        }
	}

	cerr << "Highscore.txt written successfully...\n";
}


void Gamerecord::submitRecord(int s)
{
	if (user_score == nullptr) {
		user_score = new HighScore(string(gamer), s);
	} else {
        delete user_score;
		user_score = new HighScore(string(gamer), s);
	}

    cerr << user_score->uname << "'s score is " << user_score->score << endl;

    cerr << "adding to records..." << endl;
	addRecord(*user_score);
    for (int i = 0; i < scores.size(); i++) {
        if ((scores[i].score == user_score->score) && 
                (scores[i].uname == user_score->uname))
            place = i;
    }

    
    if (isHighScore()) {
        highscore = s;
    }

    cerr << "making new menu... " << endl;
    makeMenu(); // make the menu with 11 people

	scores.pop_back();	// delete the lowest
}

// sorts the records
void Gamerecord::sortRecord()
{
	sort(scores.begin(), scores.end());	// sorts in ascending
	reverse(scores.begin(), scores.end());	// changes to descending

    cerr << "finished sorting scores\n";
}

// adds a record then sorts the records
void Gamerecord::addRecord(HighScore s)
{
	scores.push_back(s);
    cerr << "finished adding score\n";
	sortRecord();
}

// tests to see if the user's score is equal to the high score
bool Gamerecord::isHighScore()
{
	// return (place == (*user_score).score);
    if (place == -1)
        return false;
    
    return (place == 0);
        
}

bool Gamerecord::isTopTen()
{
    if (place == -1)
        return false;

	return (place >= 0 && place < 11);
}

void Gamerecord::genFakeNames()
{
	string names[10] = {"Amy", "Mike", "Grayson", "Gavin", "Dan", 
						"Huaiyu", "Ailand", "Newb1234", "tehBest", "Gordon"};
	int nums[10] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
	int i = 0;

	while (scores.size() < 10) {
		// make sure the dummy value is unique,.
		auto it = find(scores.begin(), scores.end(), nums[i]);
		if (it == scores.end())
			scores.push_back(HighScore(names[i],nums[i]));
		i++;
	}
	
	sortRecord();
}

void Gamerecord::makeMenu()
{
    ostringstream temp;
    string name_list[scores.size()];


    for (int i = 0; i < scores.size(); i++) {
        temp << left << setw(12) << scores[i].uname 
             << right << setw(60) << scores[i].score;
        cerr << left << setw(12) << scores[i].uname 
             << right << setw(60) << scores[i].score << endl;

        name_list[i] = temp.str();
        temp.str("");
    }
    
    // allocate mem for new menu
    if (!hs_menu) {
        // cerr << "menu set to null so far" << endl;
        hs_menu = new Menu(scores.size(), 300.0, 300.0, 
                            g.xres/2.0, g.yres/2.0, name_list, 1);
    } else if (hs_menu) {   // make a new menu
        // cerr << "deleting the prev menu" << endl;
        delete hs_menu;
        // cerr << "now making a new menu" << endl;
        hs_menu = new Menu(scores.size(), 300.0, 300.0, 
                            g.xres/2.0, g.yres/2.0, name_list, 1);
    }

    // cerr << "checking if high score" << endl;
    if (isHighScore()) {
        // cerr << "setting high score color" << endl;
        (hs_menu->t_boxs[0]).setColor(124,10,2);
    } else if (isTopTen()) {
        // cerr << "setting top ten color" << endl;
        (hs_menu->t_boxs[place]).setColor(178,222,39);
    }

    // set 11th element to yellow (will be deleted)
    // cerr << "setting 11th element color" << endl;
    if (scores.size() == 11)
        (hs_menu->t_boxs[scores.size()-1]).setColor(189,195,199);

    // cerr << "finished making menu...\n" << endl;
}