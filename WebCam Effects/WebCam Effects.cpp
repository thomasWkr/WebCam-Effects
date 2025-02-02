#include <opencv2/opencv.hpp>
#include <fstream>
#define D_CONTRAST 30

using namespace cv;

static void on_trackbar(int, void*){}

int main(int argc, char** argv)
{
	int camera = 0; // Here you put the index of your WebCam (usually 0)
	VideoCapture cap;
	if (!cap.open(camera))
		return 0;

	namedWindow("Original Image", WINDOW_AUTOSIZE);
	int blurIntensity = 0, brightness=0, contrast=0;
	Mat srcframe, dstframe;
	char key=' ', effect=' ', lastkey=' ';
	Size blur(0,0);				// Gaussian Bluer Kernel
	int filters[11] = {0};		// Array to check wich filters are activated 
								// (if contrast and brightness are higher than zero, 
	VideoWriter video;
	int record=0;

	std::cout << "Functions and Filters:\n"
		<< "- L or l: Low-Pass filter (Gaussian Blur)\n"
		<< "- E or e: Edge detection filter (Canny)\n"
		<< "- S or s: Sobel filter\n"
		<< "- B or b: Brightness adjustment\n"
		<< "- C or c: Contrast adjustment\n"
		<< "- N or n: Negative filter\n"
		<< "- G or g: Grayscale filter\n"
		<< "- O or o: Zoom Out (2x)\n"
		<< "- T or t: Turn right (90 degrees rotation)\n"
		<< "- H or h: Horizontal flip\n"
		<< "- V or v: Vertical flip\n"
		<< "- R or r: Record video (1 click starts 2 clicks stops)\n";

	for (;;)
	{
		cap >> srcframe;
		if (srcframe.empty()) 
			break; // end of video stream
		cap >> dstframe;

		key = waitKey(1);
		if (key == 27)	// stop capturing by pressing ESC
			break;
		else if (key > 27) { //saves last effect chosen
			effect = key;
		}
		
		switch(toupper(effect)) {
			case 'L':
				filters[0] = 1;
				if (filters[0] && lastkey!=effect) {
					destroyWindow("Original Image");
					namedWindow("Original Image", WINDOW_AUTOSIZE);
					createTrackbar("Blur", "Original Image", &blurIntensity, 100, on_trackbar);
				}
				lastkey = effect;
				break;
			
			case 'E':
				filters[2] = 0;
				filters[1] = !filters[1];
				break;

			case 'S':
				filters[1] = 0;
				filters[2] = !filters[2];
				break;

			case 'B':
				if (!filters[3] && lastkey != effect) {
					destroyWindow("Original Image");
					namedWindow("Original Image", WINDOW_AUTOSIZE);
					createTrackbar("Brightness", "Original Image", &brightness, 255, on_trackbar);
					setTrackbarMin("Brightness", "Original Image", -255);
					setTrackbarPos("Brightness", "Original Image", 0);
				}
				else if (filters[3] && lastkey != effect) {
					destroyWindow("Original Image");
					namedWindow("Original Image", WINDOW_AUTOSIZE);
					createTrackbar("Brightness", "Original Image", &brightness, 255, on_trackbar);
					setTrackbarMin("Brightness", "Original Image", -255);
					setTrackbarPos("Brightness", "Original Image", brightness);
				}
				lastkey = effect;
				filters[3] = 1;
				break;

			case 'C':
				if (!filters[4] && lastkey != effect) {
					destroyWindow("Original Image");
					namedWindow("Original Image", WINDOW_AUTOSIZE);
					createTrackbar("Contrast", "Original Image", &contrast, 100, on_trackbar);
					setTrackbarPos("Contrast", "Original Image", D_CONTRAST);
				}
				else if (filters[4] && lastkey != effect){
					destroyWindow("Original Image");
					namedWindow("Original Image", WINDOW_AUTOSIZE);
					createTrackbar("Contrast", "Original Image", &contrast, 100, on_trackbar);
				}
				lastkey = effect;
				filters[4] = 1;
				break;

			case 'N':
				filters[5] = !filters[5];
				break;

			case 'G':
				filters[6] = !filters[6];
				break;

			case 'O':
				filters[7] = !filters[7];
				break;

			case 'T':
				filters[8] +=1;
				if (filters[8] == 4) filters[8] = 0;
				break;

			case 'H':
				filters[9] = !filters[9];
				break;

			case 'V':
				filters[10] = !filters[10];
				break;

			case 'R':
				if (record)
					video.release();			//finish video recording
				else {
					std::string filename = "webcamvideo.mp4";
					std::ifstream f(filename);
					int i = 2;
					while (f.good()) {
						filename = "webcamvideo"+std::to_string(i);
						filename = filename + ".mp4";
						f = std::ifstream(filename);
						i++;
					}
					video = VideoWriter(filename, VideoWriter::fourcc('m', 'p', '4', 'v'), 10, Size(dstframe.cols, dstframe.rows), true);
				}
				record = !record;
				break;

		}

		effect = ' ';

		if (blurIntensity % 2 == 0)
			blur = Size(blurIntensity + 1, blurIntensity + 1);
		else
			blur = Size(blurIntensity, blurIntensity);


		if (filters[7])
			resize(dstframe,dstframe,Size(dstframe.cols /2,dstframe.rows/2));

		if (filters[0])
			GaussianBlur(dstframe, dstframe, blur, 0);
		
		if (filters[3])
			dstframe.convertTo(dstframe, 16, 1, brightness);

		if (filters[4])
			dstframe.convertTo(dstframe, 16, (double) contrast/D_CONTRAST, 0);

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
			Sobel(dstframe, dstframe, -1, 1, 1, 5);
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