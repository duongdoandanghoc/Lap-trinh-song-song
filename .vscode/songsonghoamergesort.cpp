#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

void merge(vector<int>& arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void merge_sort_omp(vector<int>& arr, int l, int r, int depth = 0) {
    if (l >= r) return;
    int m = l + (r - l) / 2;

    if (depth < 4) { 
        #pragma omp task shared(arr)
        merge_sort_omp(arr, l, m, depth + 1);

        #pragma omp task shared(arr)
        merge_sort_omp(arr, m + 1, r, depth + 1);

        #pragma omp taskwait
    } else {
        merge_sort_omp(arr, l, m, depth + 1);
        merge_sort_omp(arr, m + 1, r, depth + 1);
    }

    merge(arr, l, m, r);
}

void merge_sort_seq(vector<int>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    merge_sort_seq(arr, l, m);
    merge_sort_seq(arr, m + 1, r);
    merge(arr, l, m, r);
}

int main() {
    int n = 1000000;
    vector<int> arr(n), arr_copy;
    for (int i = 0; i < n; i++) arr[i] = rand() % 100000;
    arr_copy = arr;

    double start, end;

    start = omp_get_wtime();
    merge_sort_seq(arr, 0, n - 1);
    end = omp_get_wtime();
    cout << "Thoi gian MergeSort (tuan tu): " << end - start << " giay\n";

    arr = arr_copy;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        merge_sort_omp(arr, 0, n - 1);
    }
    end = omp_get_wtime();
    cout << "Thoi gian MergeSort (OMP tasks): " << end - start << " giay\n";

    return 0;
}
