#include <iostream>
#include <list>
#include <queue>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

using namespace cv;
using namespace std;

const int N = 10, M = 10, K = 10;
const int stepx[] = { -1,0,1,-1,1,-1,0,1 };
const int stepy[] = { -1,-1,-1,0,0,1,1,1 };
int sign_matrix[M][N];
int image[M][N] = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
                {0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 3, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 2, 0},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0} };

//void read() {
//    string image_path = "G:\bioturingVietNam\Image\data\00.tif";
//    Mat new_image = imread(image_path, IMREAD_ANYCOLOR | IMREAD_ANYDEPTH);
//    
//    imshow("Image", new_image);
//    waitKey(0);
//
//}

void print2DArray(int theArray[M][N]) {
    for (int x = 0; x < M; x++) {
        for (int y = 0; y < N; y++)
            cout << theArray[x][y] << " ";
        cout << endl;
    }
}
void print_queue(queue<vector<int> > a) {
    while (!a.empty()) {
        vector <int> point = a.front();
        cout << point[0] << " " << point[1] << " || ";
        a.pop();
    }
    cout << endl;
}

bool valid(int x, int y) {
    if (x < 0 || y < 0) return false;
    if (x > N - 1 || y > M - 1) return false;
    return true;
}

queue< vector<int> > list_of_candidate;

queue< vector<int> > relate_area(int x, int y) {
    cout << x << " " << y << endl;
    queue < vector<int> > new_candidate;
    
    vector <int> point = { x,y };
    queue < vector<int> > que;
    new_candidate.push(point);
    que.push(point);
    while (!que.empty()) {
        point = que.front();
        sign_matrix[point[0]][point[1]] = 0;
        que.pop();
        for (int i = 0; i < 8; i++) {
            int nx = point[0] + stepx[i];
            int ny = point[1] + stepy[i];
            if (!valid(nx, ny)) continue;
            if (sign_matrix[nx][ny] == -1 && image[nx][ny] == image[point[0]][point[1]]) {
                cout << nx << " " << ny << "||||" << point[0] << " " << point[1]<<endl;
                sign_matrix[nx][ny] = 0;
                vector <int> newpoint = { nx,ny };
                que.push(newpoint);
                for (int k = 0; k < 9; k++) {
                    int check_x = point[0] + stepx[k];
                    int check_y = point[1] + stepy[k];
                    if (!valid(check_x, check_y)) continue;
                    if (image[check_x][check_y] == 0) {
                        new_candidate.push(newpoint);
                        break;
                    }
                }
            }
        }
    }
    return new_candidate;
}
queue< vector<int> > dilate(queue< vector<int> > lt_candidate, int time_of_dilate) {
    queue< vector<int> > new_candidate;
    vector <int> point;
    while (!lt_candidate.empty()) {
        point = lt_candidate.front();
        lt_candidate.pop();
        int x = point[0];
        int y = point[1];
        if (sign_matrix[x][y] == -2) {
            image[x][y] = -1;
            continue;
        }
        for (int i = 0; i < 8; i++) {
            int nx = point[0] + stepx[i];
            int ny = point[1] + stepy[i];
            if (!valid(nx, ny)) continue;
            if (sign_matrix[nx][ny] == -2 || sign_matrix[nx][ny] == 0) continue;
            if (sign_matrix[nx][ny] == -1) {
                sign_matrix[nx][ny] = time_of_dilate;
                image[nx][ny] = image[x][y];
                vector<int> newpoint = { nx,ny };
                new_candidate.push(newpoint);
                continue;
            }
            if (sign_matrix[nx][ny] != time_of_dilate) continue;
            if (image[nx][ny] == image[x][y]) continue;
            sign_matrix[nx][ny] = -2;
        }
    }
    cout << 1 << endl;
    return new_candidate;
}
void add_queue(queue<vector<int> >& a, queue<vector<int> >& b) {
    while (!b.empty()) {
        vector <int> point = b.front();
        a.push(point);
        b.pop();
    }
}
void clust_into_area() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            if (sign_matrix[i][j] == -1 && image[i][j] > 0) {
                queue<vector<int> > area = relate_area(i, j);
                add_queue(list_of_candidate, area);
            }
}
void dilate() {
    int time_cur = 1;
    while (time_cur < K+1) {
        vector <int> point = list_of_candidate.front();
        list_of_candidate.pop();
        queue<vector<int> > new_candidate = dilate(list_of_candidate, time_cur);
        add_queue(list_of_candidate, new_candidate);
        if (list_of_candidate.empty()) break;
        cout << time_cur << endl;
        print2DArray(image);
        cout << endl;
        time_cur++;
    }
}

int main() {
    cout << "hello" << endl;
    print2DArray(image);
    fill(*sign_matrix, *sign_matrix + M * N, -1);
    int raw_image[M][N];
    copy(&image[0][0], &image[0][0] + M * N, &raw_image[0][0]);
    clust_into_area();
    print_queue(list_of_candidate);
    dilate();
    print2DArray(image);
}

