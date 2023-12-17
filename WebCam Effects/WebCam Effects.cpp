#include <opencv2/opencv.hpp>
#define D_CONTRAST 50
#define D_BRIGHT 127

using namespace cv;

static void on_trackbar(int, void*)
{
	
}

int main(int argc, char** argv)
{
	int camera = 1;
	VideoCapture cap;
	if (!cap.open(camera))
		return 0;

	namedWindow("Original Image", WINDOW_AUTOSIZE);
	int alpha_slider = 0;
	createTrackbar("Trackbar", "Original Image", &alpha_slider, 255, on_trackbar);
	Mat srcframe, dstframe;
	char key=' ', effect=' ';
	Size blur(0,0);				// Gaussian Bluer Kernel
	double filters[11] = {0};	// Array to check wich filters are activated 
								// (if contrast and brightness are higher than zero, 
	filters[4] = D_CONTRAST;

	VideoWriter video;
	int record=0;

	for (;;)
	{
		cap >> srcframe;
		if (srcframe.empty()) 
			break; // end of video stream
		cap >> dstframe;

		Size kernelSize(0, 0);
		if (alpha_slider % 2 == 0)	
			kernelSize = Size(alpha_slider+1, alpha_slider+1);
		else						
			kernelSize = Size(alpha_slider, alpha_slider);

		key = waitKey(1);
		if (key == 27)	// stop capturing by pressing ESC
			break;
		else if (key > 27) { //saves last effect chosen
			effect = key;
		}
		
		switch(toupper(effect)) {
			case 'L':
				filters[0] = 1;
				blur = kernelSize;
				break;
			
			case 'E':
				filters[2] = 0;
				filters[1] = !filters[1];
				effect = ' ';
				break;

			case 'S':
				filters[1] = 0;
				filters[2] = !filters[2];
				effect = ' ';
				break;

			case 'B':
				filters[3] = alpha_slider- D_BRIGHT;
				break;

			case 'C':
				filters[4] = (double) alpha_slider/ D_CONTRAST;
				break;

			case 'N':
				filters[5] = !filters[5];
				effect = ' ';
				break;

			case 'G':
				filters[6] = !filters[6];
				effect = ' ';
				break;

			case 'O':
				filters[7] = !filters[7];
				effect = ' ';
				break;

			case 'T':
				filters[8] +=1;
				if (filters[8] == 4) filters[8] = 0;
				effect = ' ';
				break;

			case 'H':
				filters[9] = !filters[9];
				effect = ' ';
				break;

			case 'V':
				filters[10] = !filters[10];
				effect = ' ';
				break;

			case 'R':
				if (record)
					video.release();			//finish video recording
				else {
					video = VideoWriter("webcamvideo.mp4", VideoWriter::fourcc('m', 'p', '4', 'v'), 10, Size(dstframe.cols, dstframe.rows), true);
				}
				record = !record;
				effect = ' ';
				break;

		}

		if (filters[7])
			resize(dstframe,dstframe,Size(dstframe.cols /2,dstframe.rows/2));

		if (filters[0])
			GaussianBlur(dstframe, dstframe, blur, 0);
		
		if (filters[3])
			dstframe.convertTo(dstframe, 16, 1, filters[3]);

		if (filters[4]!= D_CONTRAST)
			dstframe.convertTo(dstframe, 16, filters[4], 0);

		if (filters[5])
			dstframe.convertTo(dstframe, 16, -1, 255);

		if (filters[9])
			flip(dstframe, dstframe, 1);

		if (filters[10])
			flip(dstframe, dstframe, 0);

		if (filters[8])
			rotate(dstframe, dstframe, filters[8] - 1);

		if (filters[1]) {
			cvtColor(dstframe, dstframe, COLOR_BGR2GRAY);
			Canny(dstframe, dstframe, 100, 200);
			cvtColor(dstframe, dstframe, COLOR_GRAY2BGR);
		}
		else if (filters[2]) {
			cvtColor(dstframe, dstframe, COLOR_BGR2GRAY);
			Sobel(dstframe, dstframe, 0, 1, 1, 5);
			cvtColor(dstframe, dstframe, COLOR_GRAY2BGR);
		}
		else if (filters[6]) {
			cvtColor(dstframe, dstframe, COLOR_BGR2GRAY);
			cvtColor(dstframe, dstframe, COLOR_GRAY2BGR);
		}

		if (record)
			video.write(dstframe);

		imshow("Original Image", srcframe); imshow("New Image", dstframe);
		
	}
	cap.release(); // release the VideoCapture object
	return 0;
}