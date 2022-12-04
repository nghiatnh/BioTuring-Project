#include <iostream>
#include <list>
#include <queue>
#include <vector>
#include <chrono>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


using namespace cv;
using namespace std;
using namespace std::chrono;

const int mySizes[2] = { 1000,1000 };
const int stepx[] = { -1,0,1,-1,1,-1,0,1 };
const int stepy[] = { -1,-1,-1,0,0,1,1,1 };
Mat img;

struct point {
    int x;
    int y;
    int value;
};

void read_txt() {
    Mat m;
    FileStorage fs("myfile.txt", FileStorage::READ);
    fs["mat1"] >> m;
}

int read()
{
    string image_path = samples::findFile("G:\\bioturingVietNam\\Image\\data\\00.tif");

    img = imread(image_path, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH);
    if (img.empty())
    {
        cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }
    //imshow("Display window", img);
    return 0;
}

int N, M, K = 1000;
int sign_matrix[1000][1000];

bool valid(point a) {
    if (a.x < 0 || a.y < 0) return false;
    if (a.x > M - 1 || a.y > N - 1) return false;
    return true;
}
void add_queue(queue<point>& a, queue<point>& b) {
    if (b.size() == 0) return;
    while (!b.empty()) {
        point new_element = b.front();
        a.push(new_element);
        b.pop();
    }
}
queue<point> list_of_candidate;
queue<point> relate_area(point root) {
    queue <point> new_candidate;
    sign_matrix[root.x][root.y] = 0;
    queue <point> que;
    que.push(root);
    while (!que.empty()) {
        point current = que.front();
        que.pop();
        int cnt = 0;
        for (int i = 0; i < 8; i++) {
            point next_point;
            next_point.x = current.x + stepx[i];
            next_point.y = current.y + stepy[i];
            if (!valid(next_point)) continue;
            if (sign_matrix[next_point.x][next_point.y] != -1) continue;
            if (img.at<short>(next_point.x,next_point.y) == 0) {
                cnt++;
                continue;
            }
            if (img.at<short>(next_point.x, next_point.y) == img.at<short>(root.x, root.y)) {
                sign_matrix[next_point.x][next_point.y] = 0;
                que.push(next_point);
            }

        }
        if (cnt > 0)
            new_candidate.push(current);
    }
    return new_candidate;
}

void clust_into_area() {
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (sign_matrix[i][j] == -1 && img.at<short>(i,j) > 0) {
                point main_point;
                main_point.x = i;
                main_point.y = j;
                queue<point> area = relate_area(main_point);
                add_queue(list_of_candidate, area);
            }
}
queue<point> dilate(queue<point>& lt_candidate, int time_of_dilate) {
    queue<point> new_candidate;
    while (!lt_candidate.empty()) {
        point current = lt_candidate.front();
        lt_candidate.pop();
        int x = current.x;
        int y = current.y;
        if (sign_matrix[x][y] == -2) {
            img.at<short>(x, y) = 0;
            continue;
        }
        for (int i = 0; i < 8; i++) {
            int nx = x + stepx[i];
            int ny = y + stepy[i];
            point new_point;
            new_point.x = nx;
            new_point.y = ny;
            if (!valid(new_point)) continue;
            if (sign_matrix[nx][ny] == -2 || sign_matrix[nx][ny] == 0) continue;
            if (sign_matrix[nx][ny] == -1) {
                sign_matrix[nx][ny] = time_of_dilate;
                img.at<short>(nx, ny) = img.at<short>(x, y);
                new_candidate.push(new_point);
                continue;
            }
            if (sign_matrix[nx][ny] != time_of_dilate) continue;
            if (img.at<short>(nx, ny) == img.at<short>(x, y)) continue;
            sign_matrix[nx][ny] = -2;
            lt_candidate.push(new_point);
        }
    }
    return new_candidate;
}
void dilate() {
    int time_cur = 1;
    while (time_cur < K + 1) {
        queue<point> new_candidate = dilate(list_of_candidate, time_cur);
        if (new_candidate.empty()) break;
        //cout << time_cur << " " << new_candidate.size() << endl;
        //print2DArray(image);
        add_queue(list_of_candidate, new_candidate);
        time_cur++;
    }
}




int main() {
    read();
    M = img.rows;
    N = img.cols;
    fill(*sign_matrix, *sign_matrix + M * N, -1);
    Mat raw_image = img.clone();
    clust_into_area();

    auto start = high_resolution_clock::now();
    dilate();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: "
        << duration.count() << " microseconds" << endl;

    imshow("Display window", img);
    imwrite("./output.tif", img);
    int k = waitKey();
}


