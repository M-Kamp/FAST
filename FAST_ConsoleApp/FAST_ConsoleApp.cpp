#include"pch.h"
#include<opencv2/opencv.hpp>
#include<iostream>
#include <opencv2/imgproc/imgproc.hpp>

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

#define optimised

int main()
{
	Mat img = imread("city.jpg");
	namedWindow("image", WINDOW_NORMAL);
	Mat greymat;
	cv::cvtColor(img, greymat, cv::COLOR_BGR2GRAY);

	int N = 15;
	int padding = 4;
	double Tthreshold = 0.2;

	auto start = std::chrono::high_resolution_clock::now();

	std::vector<InterestPoint> InterestPoints;
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

#ifdef optimised
			int Itest1 = greymat.at<uchar>(y + BresenhamCircle[0].Y, x + BresenhamCircle[0].X);
			int Itest5 = greymat.at<uchar>(y + BresenhamCircle[4].Y, x + BresenhamCircle[4].X);
			int Itest9 = greymat.at<uchar>(y + BresenhamCircle[8].Y, x + BresenhamCircle[8].X);
			int Itest13 = greymat.at<uchar>(y + BresenhamCircle[12].Y, x + BresenhamCircle[12].X);
			int Itests[4] = {Itest1, Itest5, Itest9, Itest13};

			for (size_t i = 0; i < 4; i++)
			{
				if (Ip + T < Itests[i])
				{
					interestAbove++;
				}

				if (Ip - T > Itests[i])
				{
					interestBelow++;
				}
			}

			if (interestAbove >= 3 || interestBelow >= 3)
			{
				
			}
			else
			{
				continue;
			}
#endif
			uchar UFStreak = 0;
			uchar UCStreak = 0;
			uchar UMaxStreak = 0;

			uchar OFStreak = 0;
			uchar OCStreak = 0;
			uchar OMaxStreak = 0;

			bool IsRejectedAbove = false;
			bool IsRejectedUnder = false;

			for (size_t i = 0; i < 16; i++)
			{
				int Itest = greymat.at<uchar>(y + BresenhamCircle[i].Y, x + BresenhamCircle[i].X);

				// then we have a valid pixel for the algorithm and need to do the streak logic for above
				if (Ip + T < Itest)
				{
					if (!IsRejectedAbove)
					{
						OFStreak++;
						if (OFStreak > OMaxStreak)
							OMaxStreak = OFStreak;
					}
					else
					{
						OCStreak++;
						if (OCStreak > OMaxStreak)
							OMaxStreak = OCStreak;
					}
				}
				else
				{
					
					if (IsRejectedAbove)
					{
						if (OFStreak > OMaxStreak)
							OMaxStreak = OFStreak;
					}
					else
					{
						if (OCStreak > OMaxStreak)
							OMaxStreak = OCStreak;

						OCStreak = 0;
					}
					
					IsRejectedAbove = true;
				}

				// then we have a valid pixel for the algorithm and need to do the streak logic for under
				if (Ip - T > Itest)
				{
					if (!IsRejectedUnder)
					{
						UFStreak++;
						if (UFStreak > UMaxStreak)
							UMaxStreak = UFStreak;
					
					}
					else
					{
						UCStreak++;
						if (UCStreak > UMaxStreak)
							UMaxStreak = UCStreak;
					}
				}
				else
				{
					if (IsRejectedUnder)
					{
						if (UFStreak > UMaxStreak)
							UMaxStreak = UFStreak;
					}
					else
					{
						if (UCStreak > UMaxStreak)
							UMaxStreak = UCStreak;

						UCStreak = 0;
					}

					IsRejectedUnder = true;
				}
			}

			bool IsInterestPixel = false;

			// now see if we have a streak bigger or the same as N
			if (OMaxStreak >= N || UMaxStreak >= N)
			{
				IsInterestPixel = true;
			}
			else if (OFStreak + OCStreak >= N || UFStreak + UCStreak >= N)
			{
				IsInterestPixel = true;
			}

			if (IsInterestPixel)
			{
				InterestPoint p = InterestPoint(x, y);
				InterestPoints.push_back(p);
				// add color to the original image?
				uchar& Color = greymat.at<uchar>(y, x);

				Color = 255;

				img.at<Vec3b>(y, x) = Vec3b(Color, Color, Color);
			}

		}
	}

#ifdef optimised
	std::cout << "Optimised Version";
	std::cout << std::endl;
#else
	std::cout << "Not Optimised Version";
	std::cout << std::endl;
#endif

	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";

	std::cout << "Done";
	std::cout << std::endl;
	std::cout << "We found " << InterestPoints.size() << " interest points";
	std::cout << std::endl;

	imshow("image", img);

	waitKey(0);
	return 0;
}



