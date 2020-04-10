#include"pch.h"
#include<opencv2/opencv.hpp>
#include<iostream>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

class FASTPoint
{
public:
	int X;
	int Y;

	FASTPoint(int x, int y)
	{
		X = x;
		Y = y;
	}

	FASTPoint()
	{
		X = 0;
		Y = 0;
	}
};

class InterestPoint
{
public:
	int X;
	int Y;

	InterestPoint(int x, int y)
	{
		X = x;
		Y = y;
	}

	InterestPoint()
	{
		X = 0;
		Y = 0;
	}
};

int main()
{
	Mat img = imread("city.jpg");
	namedWindow("image", WINDOW_NORMAL);
	Mat greymat;
	cv::cvtColor(img, greymat, cv::COLOR_BGR2GRAY);

	int N = 14;
	int padding = 4;
	double Tthreshold = 0.2;
	vector<InterestPoint> interestPoints;
	FASTPoint BresenhamCircle[16];

	BresenhamCircle[0] = FASTPoint(0, -3);
	BresenhamCircle[1] = FASTPoint(1, -3);
	BresenhamCircle[2] = FASTPoint(2, -2);
	BresenhamCircle[3] = FASTPoint(3, -1);
	BresenhamCircle[4] = FASTPoint(3, 0);
	BresenhamCircle[5] = FASTPoint(3, 1);
	BresenhamCircle[6] = FASTPoint(2, 2);
	BresenhamCircle[7] = FASTPoint(1, 3);
	BresenhamCircle[8] = FASTPoint(0, 3);
	BresenhamCircle[9] = FASTPoint(-1, 3);
	BresenhamCircle[10] = FASTPoint(-2, 2);
	BresenhamCircle[11] = FASTPoint(-3, 1);
	BresenhamCircle[12] = FASTPoint(-3, 0);
	BresenhamCircle[13] = FASTPoint(-3, -1);
	BresenhamCircle[14] = FASTPoint(-2, -2);
	BresenhamCircle[15] = FASTPoint(-1, -3);

	for (size_t x = padding; x < greymat.cols - padding; x++)
	{
		for (size_t y = padding; y < greymat.rows - padding; y++)
		{
			int Ip = greymat.at<uchar>(y, x);
			float T = Tthreshold * Ip;

			int interestAbove = 0;
			int interestBelow = 0;

			for (size_t i = 0; i < 16; i++)
			{
				int Itest = greymat.at<uchar>(y + BresenhamCircle[i].Y, x + BresenhamCircle[i].X);

				if (Ip + T < Itest)
				{
					interestAbove++;
				}

				if (Ip - T > Itest)
				{
					interestBelow++;
				}
			}

			if (interestAbove >= N || interestBelow >= N)
			{
				InterestPoint p = InterestPoint(x, y);
				interestPoints.push_back(p);
				// add color to the original image?
				uchar& color = greymat.at<uchar>(y, x);

				color = 255;
	

				img.at<Vec3b>(y, x) = color;
			}
		}
	}

	cout << "Done";
	cout << endl;
	cout << "We found " << interestPoints.size() << " interest points";
	cout << endl;

	imshow("image", img);

	waitKey(0);
	return 0;
}



