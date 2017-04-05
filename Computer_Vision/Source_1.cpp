#include "opencv2/highgui.hpp"
#include <iostream>
#include<math.h>
using namespace cv;
using namespace std;

// Limit val between 1 and 0
double limit(double val)
{
	double result= val;
	if (val > 1.0)
	{
		result = 1.0;
	}
	else if (val < 0.0)
	{
		result = 0.0;
	}
	return result;
}

double compute(double val)
{
	double result = val;
	val = limit(val);
	if (val < 0.00304)
	{
		result = 12.92*val;
	}
	else
	{
		result = 1.055*pow(val, 0.417) - 0.055;
	}
	return result;
}
int main(int argc, char** argv) {
	if (argc != 3) {
		cout << argv[0] << ": "
			<< "got " << argc - 1 << " arguments. Expecting two: width height."
			<< endl;
		return(-1);
	}

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	int** RED1 = new int*[height];
	int** GREEN1 = new int*[height];
	int** BLUE1 = new int*[height];
	int** RED2 = new int*[height];
	int** GREEN2 = new int*[height];
	int** BLUE2 = new int*[height];

	for (int i = 0; i < height; i++) {
		RED1[i] = new int[width];
		GREEN1[i] = new int[width];
		BLUE1[i] = new int[width];
		RED2[i] = new int[width];
		GREEN2[i] = new int[width];
		BLUE2[i] = new int[width];
	}

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			int r1, g1, b1;
			int r2, g2, b2;

			double x = (double)j / (double)width;
			double y = (double)i / (double)height;
			double Y = 1.0;

			double L = 90;
			double u = x * 512 - 255;
			double v = y * 512 - 255;


			/* Your code should be placed here
			It should translate xyY to byte sRGB
			and Luv to byte sRGB
			*/

			double u_dash = (u + 13 * 0.198*L) / (13 * L);
			double v_dash = (v + 13 * 0.468*L) / (13 * L);
			double y2 = pow(((L + 16) / 116), 3)*1.0;
			double x2 = 0, z2 = 0;
			if (v_dash > 0)
			{
				x2 = y2*2.25*u_dash / v_dash;
				z2 = (y2*(3 - 0.75*u_dash - 5 * v_dash)) / v_dash;

			}
			double tempR = (3.240479)*x2 - (1.53715)*y2 - (0.498535)*z2;
			double tempG = (1.875991)*y2 + (0.041556)*z2 - (0.969256)*x2;
			double tempB = (0.055648)*x2 - (0.204043)*y2 + (1.057311)*z2;

			tempR = compute(tempR);
			tempG = compute(tempG);
			tempB = compute(tempB);

			r2 = static_cast<int>(tempR * 255);
			g2 = static_cast<int>(tempG * 255);
			b2 = static_cast<int>(tempB * 255);
			
			//Convert xyY to RGB
			double x1 = x / y;
			double y1 = 1.0;
			double z1 = (1 - y - x) / y;
			double r = (3.240479)*x1 - (1.53715)*y1 - (0.498535)*z1;
			double g = (1.875991)*y1 + (0.041556)*z1 - (0.969256)*x1;
			double b = (0.055648)*x1 - (0.204043)*y1 + (1.057311)*z1;

			r = compute(r);
			g = compute(g);
			b = compute(b);

			r1 = static_cast<int>(r * 255);
			g1 = static_cast<int>(g * 255);
			b1 = static_cast<int>(b * 255);

			// this is the end of your code
			RED1[i][j] = r1;
			GREEN1[i][j] = g1;
			BLUE1[i][j] = b1;
			RED2[i][j] = r2;
			GREEN2[i][j] = g2;
			BLUE2[i][j] = b2;
		}


	Mat R1(height, width, CV_8UC1);
	Mat G1(height, width, CV_8UC1);
	Mat B1(height, width, CV_8UC1);

	Mat R2(height, width, CV_8UC1);
	Mat G2(height, width, CV_8UC1);
	Mat B2(height, width, CV_8UC1);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {

			R1.at<uchar>(i, j) = RED1[i][j];
			G1.at<uchar>(i, j) = GREEN1[i][j];
			B1.at<uchar>(i, j) = BLUE1[i][j];

			R2.at<uchar>(i, j) = RED2[i][j];
			G2.at<uchar>(i, j) = GREEN2[i][j];
			B2.at<uchar>(i, j) = BLUE2[i][j];
		}

	Mat xyY;
	Mat xyY_planes[] = { B1, G1, R1 };
	merge(xyY_planes, 3, xyY);
	namedWindow("xyY", CV_WINDOW_AUTOSIZE);
	imshow("xyY", xyY);

	Mat Luv;
	Mat Luv_planes[] = { B2, G2, R2 };
	merge(Luv_planes, 3, Luv);
	namedWindow("Luv", CV_WINDOW_AUTOSIZE);
	imshow("Luv", Luv);
	waitKey(0); // Wait for a keystroke
	return(0);
}