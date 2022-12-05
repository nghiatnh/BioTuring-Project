#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <time.h>

#define is_valid(x, y) (x >= 0 && x <= this->M - 1 && y >= 0 && y <= this->N - 1)
#define to_position(x, y) (x * this->N + y)
#define get_point(pos, x, y) \
    x = pos / this->N;       \
    y = pos % this->N;
#define NOT_CHECK 0
#define CHECKED 1
#define dirs_size 8
#define update_candidate(candidates)  \
    for (auto candidate : candidates) \
    this->output.at<short>(candidate.x, candidate.y) = candidate.value

using namespace cv;
using namespace std;
using namespace std::chrono;

const int dirs[dirs_size][2] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};

struct Candidate
{
    int x;
    int y;
    int value;

    Candidate(int x_, int y_, int value_)
    {
        x = x_;
        y = y_;
        value = value_;
    }

    Candidate() {}
};

class DilateSolver
{
private:
    int M, N, K;
    Mat input;
    Mat output;
    set<int> removed_candidates;
    Mat checked_list;

    vector<Candidate> search_first_areas_and_dilate(Mat input)
    {
        // Mat output = input.clone();
        vector<Candidate> areas = vector<Candidate>();
        for (int i = 0; i < this->M; i++)
        {
            for (int j = 0; j < this->N; j++)
            {
                if (input.at<short>(i, j) == 0)
                {
                    for (int it = 0; it < dirs_size; it++)
                    {
                        if (is_valid(i + dirs[it][0], j + dirs[it][1]) && input.at<short>(i + dirs[it][0], j + dirs[it][1]) != 0)
                        {
                            bool is_conflict = false;
                            for (int k = it + 1; k < dirs_size; k++)
                            {
                                if (is_valid(i + dirs[k][0], j + dirs[k][1]) && input.at<short>(i + dirs[k][0], j + dirs[k][1]) != 0 && input.at<short>(i + dirs[k][0], j + dirs[k][1]) != input.at<short>(i + dirs[it][0], j + dirs[it][1]))
                                {
                                    is_conflict = true;
                                    break;
                                }
                            }
                            this->checked_list.at<short>(i, j) = CHECKED;
                            if (!is_conflict)
                            {
                                areas.push_back(Candidate(i, j, input.at<short>(i + dirs[it][0], j + dirs[it][1])));
                                // output.at<short>(i, j) = input.at<short>(i + dirs[it][0], j + dirs[it][1]);
                            }
                            break;
                        }
                    }
                }
            }
        }

        // this->output = output;
        return areas;
    }

    vector<Candidate> search_areas_and_dilate(Mat input, vector<Candidate> candidates)
    {
        // Mat output = input.clone();
        vector<Candidate> areas = vector<Candidate>();
        for (auto candidate : candidates)
        {
            int i = candidate.x;
            int j = candidate.y;
            for (auto dir : dirs)
            {
                int x = i + dir[0];
                int y = j + dir[1];

                if (is_valid(x, y) && input.at<short>(x, y) == 0 && this->checked_list.at<short>(x, y) != CHECKED)
                {
                    bool is_conflict = false;
                    Candidate neighbor;
                    for (auto dir1 : dirs)
                    {
                        neighbor  = Candidate(x + dir1[0], y + dir1[1], input.at<short>(x + dir1[0], y + dir1[1]));
                        if (is_valid(neighbor.x, neighbor.y) && neighbor.value != 0 && neighbor.value != candidate.value)
                        {
                            is_conflict = true;
                            break;
                        }
                    }

                    this->checked_list.at<short>(x, y) = CHECKED;

                    if (!is_conflict)
                    {
                        areas.push_back(Candidate(x, y, candidate.value));
                        // output.at<short>(x, y) = input.at<short>(i, j);
                    }
                }
            }
        }

        // this->output = output;
        return areas;
    }

public:
    DilateSolver(Mat input, long K);
    DilateSolver(){};
    ~DilateSolver();

    Mat dilate()
    {
        this->output = this->input.clone();
        this->checked_list = Mat::zeros(this->M, this->N, CV_16U);

        auto start = high_resolution_clock::now();
        vector<Candidate> candidates = this->search_first_areas_and_dilate(this->output);
        update_candidate(candidates);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "k = 0, time: " << duration.count() << endl;
        for (int k = 1; k < this->K; k++)
        {
            auto start = high_resolution_clock::now();
            candidates = this->search_areas_and_dilate(this->output, candidates);
            update_candidate(candidates);
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << "k = " << k << ", time: " << duration.count() << endl;
            // cout << this->output << endl;
            if (candidates.size() == 0)
                break;
        }

        return this->output;
    }
};

DilateSolver::DilateSolver(Mat input, long K)
{
    this->M = input.rows;
    this->N = input.cols;
    this->input = input;
    this->K = K;
}

DilateSolver::~DilateSolver()
{
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    clock_t start = clock();
    Mat image;
    image = imread(argv[1], IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
    // cv::FileStorage file("./some_name.txt", cv::FileStorage::WRITE);

    // Write to file!
    // file << "image" << image;
    // return 0;
    // imwrite(argv[2], image);
    // image = Mat::zeros(8, 8, CV_16U);
    // image.at<short>(2, 2) = 1;
    // image.at<short>(2, 3) = 1;
    // image.at<short>(6, 6) = 2;
    // image.at<short>(6, 5) = 2;
    // cout << image << endl;
    if (!image.data)
    {
        printf("No image data \n");
        return -1;
    }
    DilateSolver solver = DilateSolver(image, 1000);
    // auto start = high_resolution_clock::now();
    // Mat output = solver.dilate();
    // auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<microseconds>(stop - start);
    Mat output = solver.dilate();
    clock_t stop = clock();
    cout << "execution time: " << (float)(stop - start) / CLOCKS_PER_SEC << endl;
    // cout << output << endl;
    // Mat img_rbg;
    // cvtColor(output, img_rbg, COLOR_GRAY2BGR555);
    // imwrite(argv[2], output);
    imshow("img Name", output);
    waitKey(0);

    return 0;
}
