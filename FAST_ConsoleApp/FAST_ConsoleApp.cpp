#include"pch.h"
#include<opencv2/opencv.hpp>
#include<iostream>
#include <opencv2/imgproc/imgproc.hpp>

#define NonMaximalSupression
#define optimised

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

#ifdef NonMaximalSupression
	short arcStart;
	short arcEnd;
	bool shouldDelete = false;
#endif

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

#ifdef NonMaximalSupression
	int ComputeV(FASTPoint BresenhamCircle[], cv::Mat img, int BreshenhamCircleLength) const
	{
		int result = 0;

		if (arcStart > arcEnd)
		{
			for (int i = arcStart; i < BreshenhamCircleLength; i++)
			{
				result += img.at<uchar>(Y, X) - img.at<uchar>(Y + BresenhamCircle[i].Y, X + BresenhamCircle[i].X);
			}

			for (int i = 0; i < arcEnd; i++)
			{
				result += img.at<uchar>(Y, X) - img.at<uchar>( Y + BresenhamCircle[i].Y, X + BresenhamCircle[i].X);
			}
		}
		else
		{
			for (int i = arcStart; i < arcEnd; i++)
			{
				result += img.at<uchar>(Y, X) - img.at<uchar>(Y + BresenhamCircle[i].Y, X + BresenhamCircle[i].X);
			}
		}

		return result;
	}
#endif
};




int main()
{
	cv::Mat img = cv::imread("city.jpg");
	cv::namedWindow("image", cv::WINDOW_NORMAL);
	cv::Mat greymat;
	cv::cvtColor(img, greymat, cv::COLOR_BGR2GRAY);

	int N = 12;
	int padding = 4;
	float Tthreshold = 0.2;

	auto start = std::chrono::high_resolution_clock::now();

	//std::vector<InterestPoint> InterestPoints;
	std::map<int, InterestPoint> Ips;

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

	int w = greymat.cols;
	int h = greymat.rows;

	for (int x = padding; x < w - padding; x++)
	{
		for (int y = padding; y < h - padding; y++)
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

#ifdef NonMaximalSupression
			uchar UindexStartMax = 0;
			uchar OindexStartMax = 0;
#endif

			uchar UFStreak = 0;
			uchar UCStreak = 0;
			uchar UMaxStreak = 0;

			uchar OFStreak = 0;
			uchar OCStreak = 0;
			uchar OMaxStreak = 0;

			bool IsRejectedAbove = false;
			bool IsRejectedUnder = false;

			for (uchar i = 0; i < 16; i++)
			{
				int Itest = greymat.at<uchar>(y + BresenhamCircle[i].Y, x + BresenhamCircle[i].X);

				// then we have a valid pixel for the algorithm and need to do the streak logic for above
				if (Ip + T < Itest)
				{
					if (!IsRejectedAbove)
					{
						OFStreak++;
						if (OFStreak > OMaxStreak)
						{
							OMaxStreak = OFStreak;
#ifdef NonMaximalSupression
							OindexStartMax = i - OMaxStreak;
#endif
						}							
							
					}
					else
					{
						OCStreak++;
						if (OCStreak > OMaxStreak)
						{
							OMaxStreak = OCStreak;
#ifdef NonMaximalSupression
							OindexStartMax = i - OMaxStreak;
#endif
						}							

					}
				}
				else
				{
					
					if (IsRejectedAbove)
					{
						if (OFStreak > OMaxStreak)
						{
							OMaxStreak = OFStreak;
#ifdef NonMaximalSupression
							OindexStartMax = i - OMaxStreak;
#endif
						}
							
					}
					else
					{
						if (OCStreak > OMaxStreak)
						{
							OMaxStreak = OCStreak;
#ifdef NonMaximalSupression
							OindexStartMax = i - OMaxStreak;
#endif
						}
							

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
						{
							UMaxStreak = UFStreak;
#ifdef NonMaximalSupression
							UindexStartMax = i - UMaxStreak;
#endif
						}
							
					
					}
					else
					{
						UCStreak++;
						if (UCStreak > UMaxStreak)
						{
							UMaxStreak = UCStreak;
#ifdef NonMaximalSupression
							UindexStartMax = i - UMaxStreak;
#endif
						}
							
					}
				}
				else
				{
					if (IsRejectedUnder)
					{
						if (UFStreak > UMaxStreak)
						{
							UMaxStreak = UFStreak;
#ifdef NonMaximalSupression
							UindexStartMax = i - UMaxStreak;
#endif
						}
							
					}
					else
					{
						if (UCStreak > UMaxStreak)
						{
							UMaxStreak = UCStreak;
#ifdef NonMaximalSupression
							UindexStartMax = i - UMaxStreak;
#endif
						}
							

						UCStreak = 0;
					}

					IsRejectedUnder = true;
				}
			}

			// now see if we have a streak bigger or the same as N
			if (OFStreak + OCStreak >= N)
			{
				InterestPoint p = InterestPoint(x, y);

#ifdef NonMaximalSupression
				p.arcEnd = OFStreak;
				p.arcStart = 16 - OCStreak;
#endif
				Ips[y*w+x] = p;
				//InterestPoints.push_back(p);
				// Set the color of this pixel to red to indicate that it is an interest pixel
				img.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
				continue;
			}
			else if (UFStreak + UCStreak >= N)
			{
				InterestPoint p = InterestPoint(x, y);

#ifdef NonMaximalSupression
				p.arcEnd = UFStreak;
				p.arcStart = 16 - UCStreak;
#endif
				Ips[y*w + x] = p;
				//InterestPoints.push_back(p);
				// Set the color of this pixel to red to indicate that it is an interest pixel
				img.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
				continue;
			}
			else if (OMaxStreak >= N)
			{
				InterestPoint p = InterestPoint(x, y);

#ifdef NonMaximalSupression				
				p.arcStart = OindexStartMax;
				p.arcEnd = OindexStartMax + OMaxStreak;
#endif
				Ips[y*w + x] = p;
				//InterestPoints.push_back(p);
				// Set the color of this pixel to red to indicate that it is an interest pixel
				img.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
				continue;
			}
			else if (UMaxStreak >= N)
			{
				InterestPoint p = InterestPoint(x, y);

#ifdef NonMaximalSupression				
				p.arcStart = UindexStartMax;
				p.arcEnd = UindexStartMax + UMaxStreak;
#endif
				Ips[y*w + x] = p;
				//InterestPoints.push_back(p);
				// Set the color of this pixel to red to indicate that it is an interest pixel
				img.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
				continue;
			}
		}
	}

#ifdef NonMaximalSupression	
	/*
	for (auto it = Ips.cbegin(); it != Ips.cend(); ++it)
	{
		std::cout << it->first << " " << it->second << " ""\n";
	}
	*/


	for (auto item : Ips)
	{
		if (Ips.find(item.first - w) != Ips.end())
		{
			int val = item.second.ComputeV(BresenhamCircle, greymat, 16);
			int val2 = Ips[item.first - w].ComputeV(BresenhamCircle, greymat, 16);

			if (val < val2)
			{
				Ips[item.first - w].shouldDelete = true;
			}
			else
			{
				Ips[item.first].shouldDelete = true;
			}
		}

		if (Ips.find(item.first - 1) != Ips.end())
		{
			short val = item.second.ComputeV(BresenhamCircle, greymat, 16);
			short val2 = Ips[item.first - 1].ComputeV(BresenhamCircle, greymat, 16);

			if (val < val2)
			{
				Ips[item.first - 1].shouldDelete = true;
			}
			else
			{
				Ips[item.first].shouldDelete = true;
			}
		}
	}

	for (auto it = Ips.cbegin(); it != Ips.cend(); )
	{
		if (it->second.shouldDelete)
		{
			Ips.erase(it++);    // or "it = m.erase(it)" since C++11
		}
		else
		{
			++it;
		}
	}

#endif

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
	std::cout << "We found " << Ips.size() << " interest points";
	std::cout << std::endl;

	imshow("image", img);

	cv::waitKey(0);
	return 0;
}



