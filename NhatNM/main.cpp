#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>
#include <time.h>

using namespace std;
using namespace cv;


int main() {

	std::cout << "Image: ";
	string name;
	cin >> name;

	std::cout << "\nNum of round: ";
	int maxRound;
	cin >> maxRound;

	string namefile = "E:/Working/Loang/Input/" + name;

	Mat img = imread(namefile, IMREAD_ANYDEPTH);

	int col = img.cols;
	int row = img.rows;

	clock_t start = clock();

	int hull[60000];
	int hullIndex = 0;
	//<40k
	int newHull[80000];
	int newIndex = 0;
	//<60k
	int common[32768];
	int commonIndex = 0;
	//<20k

	//cout << "\n1\n";
	//First step
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (img.at<unsigned short>(Point(i, j)) == 0) {
				
				unsigned short cur = 0; //current value of beside point
				int m = 0;
				int n = 0;
				int status = true;

				if (i == 0) {
					if (j == 0) {
						int dir[6] = {0, 1, 1, 0, 1, 1};
						for (int index = 0; index < 6; index += 2) {
							m = i + dir[index];
							n = j + dir[index + 1];
							unsigned short value = img.at<unsigned short>(Point(m, n));
							if (value != 0) {
								if (cur == 0) {
									cur = value;
								}
								else {
									if (cur != value) {
										common[commonIndex++] = i;
										common[commonIndex++] = j;
										break;
									}
								}
							}
						}
					}
					else if (j == col - 1) {
						int dir[6] = { 0, -1, 1, -1, 1, 0 };
						for (int index = 0; index < 6; index += 2) {
							m = i + dir[index];
							n = j + dir[index + 1];
							unsigned short value = img.at<unsigned short>(Point(m, n));
							if (value != 0) {
								if (cur == 0) {
									cur = value;
								}
								else {
									if (cur != value) {
										common[commonIndex++] = i;
										common[commonIndex++] = j;
										break;
									}
								}
							}
						}
					}
					else {
						int dir[10] = { 0, -1, 0, 1, 1, -1, 1, 0, 1, 1 };
						for (int index = 0; index < 10; index += 2) {
							m = i + dir[index];
							n = j + dir[index + 1];
							unsigned short value = img.at<unsigned short>(Point(m, n));
							if (value != 0) {
								if (cur == 0) {
									cur = value;
								}
								else {
									if (cur != value) {
										common[commonIndex++] = i;
										common[commonIndex++] = j;
										break;
									}
								}
							}
						}
					}
				}
				else if (i == row - 1) {
					if (j == 0) {
						int dir[6] = { -1, 0, -1, 1, 0, 1 };
						for (int index = 0; index < 6; index += 2) {
							m = i + dir[index];
							n = j + dir[index + 1];
							unsigned short value = img.at<unsigned short>(Point(m, n));
							if (value != 0) {
								if (cur == 0) {
									cur = value;
								}
								else {
									if (cur != value) {
										common[commonIndex++] = i;
										common[commonIndex++] = j;
										break;
									}
								}
							}
						}
					}
					else if (j == col - 1) {
						int dir[6] = { -1, -1, -1, 0, 0, -1 };
						for (int index = 0; index < 6; index += 2) {
							m = i + dir[index];
							n = j + dir[index + 1];
							unsigned short value = img.at<unsigned short>(Point(m, n));
							if (value != 0) {
								if (cur == 0) {
									cur = value;
								}
								else {
									if (cur != value) {
										common[commonIndex++] = i;
										common[commonIndex++] = j;
										break;
									}
								}
							}
						}
					}
					else {
						int dir[10] = { -1, -1, -1, 0, -1, 1, 0, -1, 0, 1 };
						for (int index = 0; index < 10; index += 2) {
							m = i + dir[index];
							n = j + dir[index + 1];
							unsigned short value = img.at<unsigned short>(Point(m, n));
							if (value != 0) {
								if (cur == 0) {
									cur = value;
								}
								else {
									if (cur != value) {
										common[commonIndex++] = i;
										common[commonIndex++] = j;
										break;
									}
								}
							}
						}
					}
				}
				else {
					if (j == 0) {
						int dir[10] = { -1, 0, -1, 1, 0, 1, 1, 0, 1, 1 };
						for (int index = 0; index < 10; index += 2) {
							m = i + dir[index];
							n = j + dir[index + 1];
							unsigned short value = img.at<unsigned short>(Point(m, n));
							if (value != 0) {
								if (cur == 0) {
									cur = value;
								}
								else {
									if (cur != value) {
										common[commonIndex++] = i;
										common[commonIndex++] = j;
										break;
									}
								}
							}
						}
					}
					else if (j == col - 1) {
						int dir[10] = { -1, -1, -1, 0, 0, -1, 1, -1, 1, 0 };
						for (int index = 0; index < 10; index += 2) {
							m = i + dir[index];
							n = j + dir[index + 1];
							unsigned short value = img.at<unsigned short>(Point(m, n));
							if (value != 0) {
								if (cur == 0) {
									cur = value;
								}
								else {
									if (cur != value) {
										common[commonIndex++] = i;
										common[commonIndex++] = j;
										break;
									}
								}
							}
						}
					}
					else {
						int dir[16] = { -1, -1, -1, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, 0, 1, 1 };
						for (int index = 0; index < 16; index += 2) {
							m = i + dir[index];
							n = j + dir[index + 1];
							unsigned short value = img.at<unsigned short>(Point(m, n));
							if (value != 0) {
								if (cur == 0) {
									cur = value;
								}
								else {
									if (cur != value) {
										common[commonIndex++] = i;
										common[commonIndex++] = j;
										status = false;
										break;
									}
								}
							}
						}
					}
				}

				if (cur != 0) {
					if (status) {
						newHull[newIndex++] = i;
						newHull[newIndex++] = j;
						newHull[newIndex++] = cur;

						hull[hullIndex++] = i;
						hull[hullIndex++] = j;
					}
				}
			}
		}
	}

	//std::cout << "\nRuning Time: " << (double)(clock() - start) / CLOCKS_PER_SEC;

	for (int i = 0; i < commonIndex; i += 2) {
		img.at<unsigned short>(Point(common[i], common[i + 1])) = 65535;
	}

	for (int i = 0; i < newIndex; i += 3) {
		img.at<unsigned short>(Point(newHull[i], newHull[i + 1])) = newHull[i + 2];
	}

	newIndex = 0;

	//Splash
	for (int k = 1; k < maxRound; k++) {
		//cout << k + 1 << "\n";
		for (int in = 0; in < hullIndex; in += 2) {
			int i = hull[in];
			int j = hull[in + 1];
			int m = 0;
			int n = 0;
			unsigned short value = img.at<unsigned short>(Point(i, j));
			unsigned short cur;

			if (i == 0) {
				if (j == 0) {
					int dir[6] = { 0, 1, 1, 0, 1, 1 };
					for (int index = 0; index < 6; index += 2) {
						m = i + dir[index];
						n = j + dir[index + 1];
						cur = img.at<unsigned short>(Point(m, n));
						if (cur == 0) {
							newHull[newIndex++] = m;
							newHull[newIndex++] = n;
							img.at<unsigned short>(Point(m, n)) = 65535 - value;
							//newIndex += 3;
						}
						else {
							if (cur > 32768) {
								if (cur != 65535 - value) {
									img.at<unsigned short>(Point(m, n)) = 65535;
								}
							}
						}
					}
				}
				else if (j == col - 1) {
					int dir[6] = { 0, -1, 1, -1, 1, 0 };
					for (int index = 0; index < 6; index += 2) {
						m = i + dir[index];
						n = j + dir[index + 1];
						cur = img.at<unsigned short>(Point(m, n));
						if (cur == 0) {
							newHull[newIndex++] = m;
							newHull[newIndex++] = n;
							img.at<unsigned short>(Point(m, n)) = 65535 - value;
							//newIndex += 3;
						}
						else {
							if (cur > 32768) {
								if (cur != 65535 - value) {
									img.at<unsigned short>(Point(m, n)) = 65535;
								}
							}
						}
					}
				}
				else {
					int dir[10] = { 0, -1, 0, 1, 1, -1, 1, 0, 1, 1 };
					for (int index = 0; index < 10; index += 2) {
						m = i + dir[index];
						n = j + dir[index + 1];
						cur = img.at<unsigned short>(Point(m, n));
						if (cur == 0) {
							newHull[newIndex++] = m;
							newHull[newIndex++] = n;
							img.at<unsigned short>(Point(m, n)) = 65535 - value;
							//newIndex += 3;
						}
						else {
							if (cur > 32768) {
								if (cur != 65535 - value) {
									img.at<unsigned short>(Point(m, n)) = 65535;
								}
							}
						}
					}
				}
			}
			else if (i == row - 1) {
				if (j == 0) {
					int dir[6] = { -1, 0, -1, 1, 0, 1 };
					for (int index = 0; index < 6; index += 2) {
						m = i + dir[index];
						n = j + dir[index + 1];
						cur = img.at<unsigned short>(Point(m, n));
						if (cur == 0) {
							newHull[newIndex++] = m;
							newHull[newIndex++] = n;
							img.at<unsigned short>(Point(m, n)) = 65535 - value;
							//newIndex += 3;
						}
						else {
							if (cur > 32768) {
								if (cur != 65535 - value) {
									img.at<unsigned short>(Point(m, n)) = 65535;
								}
							}
						}
					}
				}
				else if (j == col - 1) {
					int dir[6] = { -1, -1, -1, 0, 0, -1 };
					for (int index = 0; index < 6; index += 2) {
						m = i + dir[index];
						n = j + dir[index + 1];
						cur = img.at<unsigned short>(Point(m, n));
						if (cur == 0) {
							newHull[newIndex++] = m;
							newHull[newIndex++] = n;
							img.at<unsigned short>(Point(m, n)) = 65535 - value;
							//newIndex += 3;
						}
						else {
							if (cur > 32768) {
								if (cur != 65535 - value) {
									img.at<unsigned short>(Point(m, n)) = 65535;
								}
							}
						}
					}
				}
				else {
					int dir[10] = { -1, -1, -1, 0, -1, 1, 0, -1, 0, 1 };
					for (int index = 0; index < 10; index += 2) {
						m = i + dir[index];
						n = j + dir[index + 1];
						cur = img.at<unsigned short>(Point(m, n));
						if (cur == 0) {
							newHull[newIndex++] = m;
							newHull[newIndex++] = n;
							img.at<unsigned short>(Point(m, n)) = 65535 - value;
							//newIndex += 3;
						}
						else {
							if (cur > 32768) {
								if (cur != 65535 - value) {
									img.at<unsigned short>(Point(m, n)) = 65535;
								}
							}
						}
					}
				}
			}
			else {
				if (j == 0) {
					int dir[10] = { -1, 0, -1, 1, 0, 1, 1, 0, 1, 1 };
					for (int index = 0; index < 10; index += 2) {
						m = i + dir[index];
						n = j + dir[index + 1];
						cur = img.at<unsigned short>(Point(m, n));
						if (cur == 0) {
							newHull[newIndex++] = m;
							newHull[newIndex++] = n;
							img.at<unsigned short>(Point(m, n)) = 65535 - value;
							//newIndex += 3;
						}
						else {
							if (cur > 32768) {
								if (cur != 65535 - value) {
									img.at<unsigned short>(Point(m, n)) = 65535;
								}
							}
						}
					}
				}
				else if (j == col - 1) {
					int dir[10] = { -1, -1, -1, 0, 0, -1, 1, -1, 1, 0 };
					for (int index = 0; index < 10; index += 2) {
						m = i + dir[index];
						n = j + dir[index + 1];
						cur = img.at<unsigned short>(Point(m, n));
						if (cur == 0) {
							newHull[newIndex++] = m;
							newHull[newIndex++] = n;
							img.at<unsigned short>(Point(m, n)) = 65535 - value;
							//newIndex += 3;
						}
						else {
							if (cur > 32768) {
								if (cur != 65535 - value) {
									img.at<unsigned short>(Point(m, n)) = 65535;
								}
							}
						}
					}
				}
				else {
					int dir[16] = { -1, -1, -1, 0, -1, 1, 0, -1, 0, 1, 1, -1, 1, 0, 1, 1 };
					for (int index = 0; index < 16; index += 2) {
						m = i + dir[index];
						n = j + dir[index + 1];
						cur = img.at<unsigned short>(Point(m, n));
						if (cur == 0) {
							newHull[newIndex++] = m;
							newHull[newIndex++] = n;
							img.at<unsigned short>(Point(m, n)) = 65535 - value;
							//newIndex += 3;
						}
						else {
							if (cur > 32768) {
								if (cur != 65535 - value) {
									img.at<unsigned short>(Point(m, n)) = 65535;
								}
							}
						}
					}
				}
			}

		}
		hullIndex = 0;

		for (int i = 0; i < newIndex; i += 2) {
			int m = newHull[i];
			int n = newHull[i + 1];
			unsigned short value = img.at<unsigned short>(Point(m, n));
			if (value == 65535) {
				common[commonIndex++] = m;
				common[commonIndex++] = n;
			}
			else {
				hull[hullIndex++] = m;
				hull[hullIndex++] = n;
				img.at<unsigned short>(Point(m, n)) = 65535 - value;
			}
		}

		newIndex = 0;

		if (hullIndex == 0) {
			break;
		}
	}

	for (int i = 0; i < commonIndex; i += 2) {
		img.at<unsigned short>(Point(common[i], common[i + 1])) = 0;
	}

	clock_t end = clock();

	std::cout << "\nRuning Time: " << (double)(end - start) / CLOCKS_PER_SEC;

	//string output = namefile + "_output.tif";

	//imwrite(output, img);

	Mat img2 = imread("E:/Working/Loang/Input/output" + name, IMREAD_ANYDEPTH);

	int count = 0;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (img.at<unsigned short>(Point(i, j)) != (img2.at<unsigned short>(Point(i, j)))) count++;
		}
	}

	std::cout << "\nDiffirences: " << count;

	cv::imshow("Result: ", img);
	cv::waitKey(0);

	return 0;
}