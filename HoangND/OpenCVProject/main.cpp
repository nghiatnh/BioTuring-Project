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

const int stepx[] = { -1,0,1,-1,1,-1,0,1 };
const int stepy[] = { -1,-1,-1,0,0,1,1,1 };
Mat img;
Mat sign_mat;

int N, M, time_consumt=0;

struct point {
    int x;
    int y;
    short value;
    int sign;
    point() {
        x = -1;
        y = -1;
        sign = -1;
    }
    point(int nx, int ny) {
        x = nx;
        y = ny;
        sign = -1;
    }
};

queue<point> find_block(int size_of_block){
    queue<point> result;
    for (int i=0;i<=M/size_of_block;i++)
        for (int j=0;j<=N/size_of_block;j++){
            point index_point(i,j);
            int range_x = (i+1)*size_of_block;
            int range_y = (j+1)*size_of_block;
            int cnt = 0;
            for (int x = i*size_of_block;x<range_x;x++)
                for (int y = j*size_of_block;y<range_y;y++)
                    if (img.at<short>(x,y)==0) cnt++;
            if (cnt>0) 
                result.push(index_point);
        }
    return(result);
}

int read()
{
    string image_path = samples::findFile("00.tif");
    img = imread(image_path, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH);
    sign_mat = Mat::zeros(img.rows, img.cols, CV_64FC1);
    if (img.empty())
    {
        cout << "Could not read the image: " << image_path << std::endl;
        return 1;
    }
    //imshow("Display window", img);
    return 0;
}

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
    sign_mat.at<int>(root.x,root.y);
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
            if (sign_mat.at<int>(next_point.x,next_point.y) != -1) continue;
            if (img.at<short>(next_point.x,next_point.y) == 0) {
                cnt++;
                continue;
            }
            if (img.at<short>(next_point.x, next_point.y) == img.at<short>(root.x, root.y)) {
                sign_mat.at<int>(next_point.x,next_point.y) = 0;
                next_point.sign = 0;
                que.push(next_point);
            }

        }
        if (cnt > 0)
            list_of_candidate.push(current);
    }
}
void zeros_hull(int Start_x, int Start_y, int M, int N) {
    list<point> zeros_hull;
    for (int i = Start_x; i < M; i++)
        for (int j = Start_y; j < N; j++)
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
                if (no_relate_area == 0) sign_mat.at<int>(i,j)=-1;//sign_matrix[i][j] = -1;
                else if (no_relate_area == 1) {
                    point current(i, j);
                    current.value = relate_area;
                    sign_mat.at<int>(i,j)=1;
                    current.sign = 1;
                    zeros_hull.push_back(current);
                }
                else
                    sign_mat.at<int>(i,j)=-2;
            }
    for (auto pt : zeros_hull) {
        img.at<short>(pt.x, pt.y) = pt.value;
        list_of_candidate.push(pt);
    }

}

void clust_into_area(){
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            //if (sign_matrix[i][j] == -1 && img.at<short>(i,j) > 0) {
            if (sign_mat.at<int>(i,j)==-1&&img.at<short>(i,j) > 0) {
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
        if (sign_mat.at<int>(x,y) == -2) {
            img.at<short>(x, y) = 0;
            continue;
        }
        for (int i = 0; i < 8; i++) {
            int nx = x + stepx[i];
            int ny = y + stepy[i];
            point new_point(nx,ny);
            if (!valid(new_point)) continue;
            //if (sign_matrix[nx][ny] == -1) {
            if (sign_mat.at<int>(nx,ny)==-1){
                sign_mat.at<int>(nx,ny) = time_of_dilate;
                img.at<short>(nx, ny) = img.at<short>(x, y);
                new_candidate.push(new_point);
                continue;
            }
            //if (sign_matrix[nx][ny] > 0 && img.at<short>(nx, ny) != img.at<short>(x, y)) {
            if (sign_mat.at<int>(nx,ny)>0&&img.at<short>(nx, ny) != img.at<short>(x, y))  {
                sign_mat.at<int>(nx,ny) = -2;
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

    //Method 1
    //clust_into_area();

    //Method 2
    zeros_hull(0,0,M,N);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "1. Time: " << duration.count() << endl;
    //time_consumt += duration.count();
    dilate(1,1000);
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    


    cout << "Time taken by method: "
        << duration.count() << " microseconds" << endl;

    //imshow("Display window", img);
    imwrite("output_c.tif", img);
    //int k = waitKey();
}