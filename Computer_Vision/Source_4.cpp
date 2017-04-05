#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

// Linear conversion
double ComputeLsRGB(double val)
{
	if (val < 0.03928)
		val	= val / 12.92;
	else
		val = pow((val + 0.055) / 1.055, 2.4);
	return val;
}

// Non Linear conversion
double computeNLsRGB(double val)
{
	if (val < 0.00304)
		val = 12.92 * val;
	else
		val = 1.055*pow(val, 1 / 2.4) - 0.055;
	return val;
}

// Limit value between 1 and 0
double limit(double val)
{
	if (val > 1)
		val = 1;
	else if (val < 0)
		val = 0;
	return val;
}

void runOnWindow(int W1, int H1, int W2, int H2, Mat inputImage, char *outName) {
	int rows = inputImage.rows;
	int cols = inputImage.cols;

	vector<Mat> i_planes;
	split(inputImage, i_planes);
	Mat iB = i_planes[0];
	Mat iG = i_planes[1];
	Mat iR = i_planes[2];

	// dynamically allocate RGB arrays of size rows x cols
	int** R = new int*[rows];
	int** G = new int*[rows];
	int** B = new int*[rows];
	for (int i = 0; i < rows; i++) {
		R[i] = new int[cols];
		G[i] = new int[cols];
		B[i] = new int[cols];
	}

	// Temporary array to store the calculated values of xyY
	// dynamically allocate RGB arrays of size rows x cols
	double** tempR = new double*[rows];
	double** tempG = new double*[rows];
	double** tempB = new double*[rows];
	for (int i = 0; i < rows; i++) {
		tempR[i] = new double[cols];
		tempG[i] = new double[cols];
		tempB[i] = new double[cols];
	}

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			R[i][j] = iR.at<uchar>(i, j);
			G[i][j] = iG.at<uchar>(i, j);
			B[i][j] = iB.at<uchar>(i, j);
		}

	//	   The transformation should be based on the
	//	   historgram of the pixels in the W1,W2,H1,H2 range.
	//	   The following code goes over these pixels
	double Ymin = 0xffffffff, Ymax = -999.0;

	double Y = 0;

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {

			double r = R[i][j]/255.0; 
			double g = G[i][j]/255.0;
			double b = B[i][j]/255.0;
			
			r = ComputeLsRGB(r);
			g = ComputeLsRGB(g);
			b = ComputeLsRGB(b);

			double X = (0.412453*r) + (0.35758*g) + (0.180423*b);
			double Y = (0.212671*r) + (0.71516*g) + (0.072169*b);
			double Z = (0.019334*r) + (0.119193*g) + (0.950227*b);

			// Find Ymin and Ymax of the specified window
			if (i >= H1 && i <= H2 && j >= W1 && j <= W2)
			{
				if (Y < Ymin)
					Ymin = Y;
				else if (Y > Ymax)
					Ymax = Y;
			}

			// Compute x and y
			double x = (X) / (X + Y + Z);
			double y = (Y) / (X + Y + Z);

			tempR[i][j] = x;
			tempG[i][j] = y;
			tempB[i][j] = Y;
		}	// Scaling and converting Luv to RGB
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {

			// Linear scaling of Luv values			// x = ((x-a)(B-A)/(b-a)) + A			// B-A = 1.0 for this case
			if (R[i][j] < Ymin)
				R[i][j] = 0;
			else
				if (R[i][j] > Ymax)
					R[i][j] = 100;
			else
				R[i][j] = static_cast<int>(((R[i][j] - Ymin) * 1.0) / (Ymax - Ymin));

			double xX = tempR[i][j];
			double yY = tempG[i][j];
			double Y = tempB[i][j];

			//convert to XYZ
			double X = (xX*Y) / yY;
			double Z = ((1 - xX - yY)*Y) / yY;

			// convert XYZ to RGB
			double temp = (3.240479*X) + ((-1.53715)*Y) + ((-0.498535)*Z);
			R[i][j] = static_cast<int>(computeNLsRGB(limit(temp)) * 255);

			temp = ((-0.969256)*X) + (1.875991*Y) + (0.041556*Z);
			G[i][j] = static_cast<int>(computeNLsRGB(limit(temp)) * 255);

			temp = (0.055648*X) + ((-0.204043)*Y) + (1.057311*Z);
			B[i][j] = static_cast<int>(computeNLsRGB(limit(temp)) * 255);
		}
	//cout << Lmax << ":" << Lmin;
	Mat oR(rows, cols, CV_8UC1);
	Mat oG(rows, cols, CV_8UC1);
	Mat oB(rows, cols, CV_8UC1);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			oR.at<uchar>(i, j) = R[i][j];;
			oG.at<uchar>(i, j) = G[i][j];;
			oB.at<uchar>(i, j) = B[i][j];;
		}

	Mat o_planes[] = { oB, oG, oR };
	Mat outImage;
	merge(o_planes, 3, outImage);

	namedWindow("output", CV_WINDOW_AUTOSIZE);
	imshow("output", outImage);
	imwrite(outName, outImage);
}

int main(int argc, char** argv) {
	if (argc != 7) {
		cerr << argv[0] << ": "
			<< "got " << argc - 1
			<< " arguments. Expecting six: w1 h1 w2 h2 ImageIn ImageOut."
			<< endl;
		cerr << "Example: proj1b 0.2 0.1 0.8 0.5 fruits.jpg out.bmp" << endl;
		return(-1);
	}
	double w1 = atof(argv[1]);
	double h1 = atof(argv[2]);
	double w2 = atof(argv[3]);
	double h2 = atof(argv[4]);
	char *inputName = argv[5];
	char *outputName = argv[6];

	if (w1<0 || h1<0 || w2 <= w1 || h2 <= h1 || w2>1 || h2>1) {
		cerr << " arguments must satisfy 0 <= w1 < w2 <= 1"
			<< " ,  0 <= h1 < h2 <= 1" << endl;
		return(-1);
	}

	Mat inputImage = imread(inputName, CV_LOAD_IMAGE_UNCHANGED);
	if (inputImage.empty()) {
		cout << "Could not open or find the image " << inputName << endl;
		return(-1);
	}

	string windowInput("input: ");
	windowInput += inputName;

	namedWindow(windowInput, CV_WINDOW_AUTOSIZE);
	imshow(windowInput, inputImage);

	if (inputImage.type() != CV_8UC3) {
		cout << inputName << " is not a standard color image  " << endl;
		return(-1);
	}

	int rows = inputImage.rows;
	int cols = inputImage.cols;
	int W1 = (int)(w1*(cols - 1));
	int H1 = (int)(h1*(rows - 1));
	int W2 = (int)(w2*(cols - 1));
	int H2 = (int)(h2*(rows - 1));

	runOnWindow(W1, H1, W2, H2, inputImage, outputName);

	waitKey(0); // Wait for a keystroke
	return(0);
}