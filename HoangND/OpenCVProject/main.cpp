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

const int mySizes[3] = {0,1000,1000 };
const int stepx[] = { -1,0,1,-1,1,-1,0,1 };
const int stepy[] = { -1,-1,-1,0,0,1,1,1 };
Mat img;

struct point {
    int x;
    int y;
    int value;
    point() {
        x = -1;
        y = -1;
    }
    point(int nx, int ny) {
        x = nx;
        y = ny;
    }
};


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


int N, M, time_consumt=0;
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
void relate_area(point root) {
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
            list_of_candidate.push(current);
    }
}
void zeros_hull() {
    list<point> zeros_hull;
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (img.at<short>(i, j) == 0) {
                short no_relate_area = 0;
                short relate_area = 0;
                for (int d = 0; d < 8; d++) {
                    int x = i + stepx[d];
                    int y = j + stepy[d];
                    point newpoint(x, y);
                    if (!valid(newpoint)) continue;
                    if (img.at<short>(x, y) != 0 &&img.at<short>(x, y) != relate_area) {
                        relate_area = img.at<short>(x, y);
                        no_relate_area++;
                    }
                }
                if (no_relate_area == 0) sign_matrix[i][j] = -1;
                else if (no_relate_area == 1) {
                    point current(i, j);
                    current.value = relate_area;
                    sign_matrix[i][j] = 1;
                    zeros_hull.push_back(current);
                }
                else
                    sign_matrix[i][j] = -2;
            }
    for (auto pt : zeros_hull) {
        img.at<short>(pt.x, pt.y) = pt.value;
        list_of_candidate.push(pt);
    }

}

void clust_into_area(){
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (sign_matrix[i][j] == -1 && img.at<short>(i,j) > 0) {
                point main_point(i,j);
                relate_area(main_point);
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
            point new_point(nx,ny);
            if (!valid(new_point)) continue;
            //if (sign_matrix[nx][ny] == -2 || sign_matrix[nx][ny] == 0) continue;
            if (sign_matrix[nx][ny] == -1) {
                sign_matrix[nx][ny] = time_of_dilate;
                img.at<short>(nx, ny) = img.at<short>(x, y);
                new_candidate.push(new_point);
                continue;
            }
            //if (sign_matrix[nx][ny] != time_of_dilate) continue;
            //if (img.at<short>(nx, ny) == img.at<short>(x, y)) continue;
            if (sign_matrix[nx][ny] > 0 && img.at<short>(nx, ny) != img.at<short>(x, y)) {
                sign_matrix[nx][ny] = -2;
                lt_candidate.push(new_point);
            }
        }
    }
    return new_candidate;
}
void dilate(int time_cur,int K) {
    while (time_cur < K + 1) {
        auto start = high_resolution_clock::now();
        queue<point> new_candidate = dilate(list_of_candidate, time_cur);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        if (new_candidate.empty()) break;
        cout << time_cur+1 << ". Time: " << duration.count() << endl;
        add_queue(list_of_candidate, new_candidate);
        time_cur++;
    }
}


int main() {
    read();
    M = img.rows;
    N = img.cols;
    Mat raw_image = img.clone();


    auto start = high_resolution_clock::now();
    fill(*sign_matrix, *sign_matrix + M * N, 0);

    //Method 1
    //clust_into_area();

    //Method 2
    zeros_hull();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "1. Time: " << duration.count() << endl;
    time_consumt += duration.count();
    dilate(1,1000);

    


    cout << "Time taken by method: "
        << time_consumt << " microseconds" << endl;

    imshow("Display window", img);
    imwrite("output_c.tif", img);
    int k = waitKey();
}