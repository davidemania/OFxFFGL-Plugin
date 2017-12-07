/*
 *  ofxFFGLWindow.cpp
 *
 *  Created by Daniel Berio 2010
 *
 // Modified by Davide Mani� for OpenFrameworks 080
 // email: software@cogitamus.it
 */

#include "ofxFFGLWindow.h"
#include "ofMain.h"


ofFFGLWindow::ofFFGLWindow()
{	
	windowPos.set(0, 0);
	windowSize.set(0, 0);
	screenSize.set(0, 0);
	
	nFrameCount				= 0;
	windowMode				= 0;
	timeNow, timeThen, fps	= 0.0f;
	
	frameRate				= 0;
	lastFrameTime		= 0.0;
}


void ofFFGLWindow::setupOpenGL(int w, int h, int screenMode)
{
    windowMode = screenMode;
	windowSize.set(w, h);
}

void ofFFGLWindow::initializeWindow()
{	
}

void  ofFFGLWindow::runAppViaInfiniteLoop(ofBaseApp * appPtr)
{

}



void ofFFGLWindow::update()
{	

}

void ofFFGLWindow::draw()
{
	timeNow = ofGetElapsedTimef();
	double diff = timeNow-timeThen;
	if( diff  > 0.00001 ){
		fps			= 1.0 / diff;
		frameRate	*= 0.9f;
		frameRate	+= 0.1f*fps;
	 }
	lastFrameTime	= diff;
	timeThen		= timeNow;
	 
	// increase the overall frame count
	nFrameCount++;			
}

int ofFFGLWindow::getHeight()
{
    return (int)windowSize.y;
}

int ofFFGLWindow::getWidth()
{
    return (int)windowSize.x;
}

void ofFFGLWindow::setWindowPosition(int x, int y)
{
	windowPos.set(x, y);
}

ofPoint	ofFFGLWindow::getWindowPosition() 
{
	return windowPos;
}

int	ofFFGLWindow::getWindowMode()
{
	return windowMode;
}

float ofFFGLWindow::getFrameRate()
{
	return frameRate;
}

double ofFFGLWindow::getLastFrameTime()
{
	return lastFrameTime;
}

int ofFFGLWindow::getFrameNum()
{
	return nFrameCount;
}


void ofFFGLWindow::setFrameRate(float targetRate)
{	
}

void ofFFGLWindow::setWindowShape(int w, int h)
{
	windowSize.set(w, h);
}

ofPoint	ofFFGLWindow::getWindowSize()
{
	return windowSize;
}

ofPoint	ofFFGLWindow::getScreenSize()
{
	return screenSize;
}

