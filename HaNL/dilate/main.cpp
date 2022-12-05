#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <queue>
#include <chrono>

using namespace cv;
using namespace std;
using namespace std::chrono;

#define maxRow 1000
#define maxColumn 1000
#define k 1000 //number of loop

Mat readFile(string filename) {
	Mat image;
	image = imread(filename, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
	//image = imread(filename);
	return image;
}

struct point {
	int x;
	int y;
	int value;
};

Mat checkMatrix(Mat image) {
	Mat b = image.clone();
	//1 == has value or can not dilate
	//0 == can dilate in the future
	for (int i = 0; i < maxRow; i++) {
		for (int j = 0; j < maxColumn; j++) {
			if (image.at<short>(i, j) > 0) {
				b.at<short>(i, j) = 1;
			}
			else {
				b.at<short>(i, j) = 0;
			}
		}
	}
	return b;
}

bool check(Mat image, Mat b, int x, int y) {
	//check whether image(i,j) can be dilate in next step or not
	int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int check = 0;
	for (int i = 0; i <= 8; i++) {
		if (x + dx[i] > -1 && x + dx[i] < maxRow && y + dy[i] > -1 && y + dy[i] < maxColumn) {
			if (image.at<short>(x + dx[i], y + dy[i]) > 0) {
				if (check == 0) {
					//surround image(x, y) has a value > 0
					check = image.at<short>(x + dx[i], y + dy[i]);
				}
				else if (check == image.at<short>(x + dx[i], y + dy[i])) {
					//suround image(x, y) has another value but the same
					continue;
				}
				else {
					//suround image(x,y) has >= 2 value
					//can not dilate
					b.at<short>(x, y) = 1;
					return false;
				}
			}
		}
	}
	if (check > 0) {
		//has 1 value around
		return true;
	}
	//does not have any value around (all is 0)
	return false;
}

int mask(Mat image, int x, int y) {
	//find the value that image(x,y) change
	int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
	int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
	int result = 0;
	for (int i = 0; i <= 8; i++) {
		if (x + dx[i] > -1 && x + dx[i] < maxRow && y + dy[i] > -1 && y + dy[i] < maxColumn) {
			if (image.at<short>(x + dx[i], y + dy[i]) > 0) {
				result = image.at<short>(x + dx[i], y + dy[i]);
				break;
			}
		}
	}
	return result;
}

queue<point> findQueue(Mat image, Mat b) {
	queue<point> q;
	for (int i = 0; i < maxRow; i++) {
		for (int j = 0; j < maxColumn; j++) {
			if (b.at<short>(i, j) == 0 && check(image, b, i, j)) {
				point p;
				p.x = i;
				p.y = j;
				p.value = mask(image, i, j);
				q.push(p);
			}
		}
	}
	return q;
}

void dilate(Mat image, Mat b, queue<point> q) {
	while (!q.empty()) {
		point u = q.front();
		q.pop();
		image.at<short>(u.x, u.y) = u.value;
		b.at<short>(u.x, u.y) = 1;
	}

}

void output(Mat image) {
	for (int i = 0; i < maxRow; i++) {
		for (int j = 0; j < maxColumn; j++) {
			printf("%d\t", image.at<short>(i, j));
		}
		printf("\n");
	}
}

int main() {
	auto start = high_resolution_clock::now();
	string filename = "00.tif";
	Mat image = readFile(filename);
	Mat b = checkMatrix(image);
	int count = 0;
	queue<point> q;
	do {
		count++;
		q = findQueue(image, b);
		if (q.size() == 0) {
			break;
		}
		else {
			dilate(image, b, q);
		}
	} while (count < k);
	//output(image);
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	cout << "Time taken by function: "
		<< duration.count() << " microseconds" << endl;
	//imshow("result", image);
	//waitKey(0);
	imwrite("output_ha.tif", image);
	return 0;
}