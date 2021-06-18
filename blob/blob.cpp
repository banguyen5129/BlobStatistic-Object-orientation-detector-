////////////////////////////////////////////////////////////////
// Skeleton program for TRC3500
// Grabs images from a USB camera using OpenCV
// Written by Andy Russell 09th February 2006
// Modified by Michael Curtis 2011-2012 - updated for newer OpenCV
/////////////////////////////////////////////////////////////////
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include <stdio.h>
using namespace std;
#include <math.h>
#define PI 3.14159265

////////////////////////////////////////////////////////////////
// main - initialises OpenCV and captures an image and changes it
////////////////////////////////////////////////////////////////

int main()
{
	CvCapture* capture = 0;
	IplImage* processedImage = 0;

	cout << "Andy's USB camera program" << endl << "Press 'q' to quit" << endl;

	// Initializes capturing video from camera
	capture = cvCaptureFromCAM(0);
	if (!capture) {
		fprintf(stderr, "Could not initialize capturing...\n");
		return -1;
	}


	// Creates window
	cvNamedWindow("Camera image");//, 1);

	// Camera image
	IplImage* frame = 0;

	// Grabs and returns a frame from camera
	frame = cvQueryFrame(capture);

	//frame = cvLoadImage("Image_1.jpg");

	// Print details of image
	cout << "image width =" << frame->width << " height =" << frame->height;
	cout << " depth =" << frame->depth << " channels =" << frame->nChannels << endl;

	do {
		// Grabs and returns a frame from camera
		frame = cvQueryFrame(capture);

		if (!frame) {
			break;
		}

		// Declarations
		int min_threshold = 127;
		double m_10 = 0;
		double m_00 = 0;
		double m_01 = 0;
		double m_11 = 0;
		double m_02 = 0;
		double m_20 = 0;
		double i_center, j_center;

		// Convert the image to gray
		for (int y = 0; y < frame->height; y++) {
			for (int x = 0; x < frame->width; x++) {
				// This is a pointer to the start of the current row.
				//  Note: The image is stored as a 1-D array which is mapped back
				//  into 2-space by multiplying the widthStep (the image width rounded to
				//  a "nice" value, eg a multiple of 4 or 8 depending on the OS and CPU)
				//  by the row number.
				uchar *row = (uchar*)(frame->imageData + frame->widthStep * y);

				int gray = (row[x * 3] + row[x * 3 + 1] + row[x * 3 + 2]) / 3;
				//printf("%d\n", gray);
				if ((gray < min_threshold)) {
					gray = 0;

					// calculate moments
					m_00++;
					m_01 += x;
					m_10 += y;
					m_11 += x * y;
					m_02 += x * x;
					m_20 += y * y;
				}
				else
					gray = 255;


				row[x * 3] = gray;
				row[x * 3 + 1] = gray;
				row[x * 3 + 2] = gray;



			}
		}
		// locate center
		i_center = m_10 / m_00; //y center

		j_center = m_01 / m_00; //x center

		// Find axis of rotation
		double a = m_20 * m_00 - m_10 * m_10;
		double b = 2 * (m_11 * m_00 - m_10 * m_01);
		double c = m_02 * m_00 - m_01 * m_01;
		double z = a - c;

		double k = 0.5;
		double theta = k * atan(b / z);
		if (a < c) theta += PI / 2;
		double theta2 = theta * 180 / PI;

		printf("m_01 = %lu , m_10 = %lu, m_11 = %lu, m_20 = %lu , m_02 = %lu\n", m_01, m_10, m_11, m_20, m_02);
		printf(" theta = %f\n", theta2);


		// new point for drawing rotation axis
		int length = 100;
		int x2 = j_center + length * sin(theta);
		int y2 = i_center + length * cos(theta);



		// Shows the resulting image in the window
		cvCircle(frame, cvPoint(j_center, i_center), 5, CV_RGB(0, 255, 0), 1);
		cvLine(frame, cvPoint(j_center, i_center), cvPoint(x2, y2), CV_RGB(255, 0, 0), 1);

		//cvLine(frame, cvPoint(2 *j_center - m_02/10e4, 2 * i_center - m_20/10e4), cvPoint(m_02/10e4, m_20/10e4), CV_RGB(255, 0, 0), 2);
		cvShowImage("Camera image", frame);



	} while ('q' != cvWaitKey(10));

	//tidy up

	// Releases the CvCapture structure
	cvReleaseCapture(&capture);
	// Destroys all the HighGUI windows
	cvDestroyAllWindows();

	return 0;

} //end of main 