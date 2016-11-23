#include <utils.h>
#include <cmath>
#include <common.h>
#include <fstream>
#include "gtest/gtest.h"
#include <algorithm>
#include <omp.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/graphviz.hpp>

typedef boost::undirected_graph<> Graph;
typedef boost::undirected_graph<>::vertex_descriptor Vertex;

class FemFixture : public ::testing::Test {
protected:
    virtual void TearDown() {
        if (G1 != NULL)
            delete[] G1;
        if (G2 != NULL)
            delete[] G2;
        if (G3 != NULL)
            delete[] G3;
        if (G4 != NULL)
            delete[] G4;
    }

    virtual void SetUp() {
    }

public:
    FemFixture() : Test() {
    }
};

void print_params() {
    printf("\nNXxNY = %dx%d\n", NX, NY);
    printf("(U, V) = (%le, %le)\n", U, V);
    printf("(HX, HY) = (%le, %le)\n", HX, HY);
    printf("TAU = %le\n", TAU);
    printf("TIME_STEP_CNT = %d\n", TIME_STEP_CNT);
    printf("INTEGR_TYPE = %d\n", INTEGR_TYPE);
    printf("IDEAL_SQ_SIZE = %dx%d\n", IDEAL_SQ_SIZE_X, IDEAL_SQ_SIZE_Y);
    printf("CENTER_OFFSET = %le, %le\n", CENTER_OFFSET_X, CENTER_OFFSET_Y);
    printf("NX3 = %d\n", NX3);
    printf("NX3_1 = %d\n", NX3_1);
    printf("NY3 = %d\n", NY3);
    printf("NY3_1 = %d\n", NY3_1);
    printf("XY = %d\n", XY);
    printf("R_LVL = %d\n", R_LVL);
    printf("R = %d\n", R);
    printf("EPS_GRID = %e\n", EPS_GRID);
    printf("RES_EPS = %e\n", RES_EPS);
    printf("APPROX_TYPE = %d\n", APPROX_TYPE);
}

void init_boundary_arrays_and_cp(int nx, int ny) {
    G1 = new int[nx];
    G2 = new int[ny];
    G3 = new int[nx];
    G4 = new int[ny];
    for (int i = 0; i < nx; ++i) {
        G1[i] = 0;
        G3[i] = 0;
    }
    for (int j = 0; j < ny; ++j) {
        G2[j] = 0;
        G4[j] = 0;
    }
    CP00 = 0;
    CP10 = 0;
    CP01 = 0;
    CP11 = 0;
}

void run_solver_1(int d) {
    assert(d >= 50);

    A = 0.;
    B = 1.;
    C = 0.;
    D = 1.;
    R_SQ = 0.099 * 0.099;
    INN_DENSITY = 1.;
    OUT_DENSITY = 0.;
    R_LVL = 2;

    NX = d;
    NY = d;
    NX_1 = NX + 1;
    NY_1 = NY + 1;
    HX = (B - A) / NX;
    HY = (D - C) / NY;
    IDEAL_SQ_SIZE_X = 64;
    IDEAL_SQ_SIZE_Y = 64;
    EPS_GRID = 0.5;
    RES_EPS = 1.e-9;

    int sz = d;
    sz = sz * ((int) std::pow(3., R_LVL));
    NX3 = sz;
    NY3 = sz;
    NX3_1 = NX3 + 1;
    NY3_1 = NY3 + 1;
    XY = NX3_1 * NY3_1;
    R = (int) std::pow(3., R_LVL);
    HX_SMALLEST = (B - A) / (NX * std::pow(3., R_LVL));
    HY_SMALLEST = (D - C) / (NY * std::pow(3., R_LVL));

    CENTER_OFFSET_X = 0.3;
    CENTER_OFFSET_Y = 0.3;

    U = 1.;
    V = 1.;
    TAU = 1.e-3;

    TIME_STEP_CNT = 5;

    APPROX_TYPE = 1;

    init_boundary_arrays_and_cp(NX3_1, NY3_1);
    print_params();

    int *grid = new int[XY];
    int *gridPr = new int[XY];

    double *density = solve_1(grid, gridPr);
    double *exact0 = calc_exact_1(grid, 0, NX3_1, NY3_1, HX_SMALLEST, HY_SMALLEST, R_LVL);
    double *exactT = calc_exact_1(grid, TAU * TIME_STEP_CNT, NX3_1, NY3_1, HX_SMALLEST, HY_SMALLEST, R_LVL);

    double x0 = get_center_x();
    double y0 = get_center_y();
    print_surface("exact", NX, NY, HX, HY, 0, A, C, x0, y0, TAU, U, V, exact0);
    print_surface("exact", NX, NY, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U, V, exactT);

    delete[] density;
    delete[] exact0;
    delete[] exactT;
    delete[] grid;
    delete[] gridPr;
}

void run_solver_2(int d) {
    assert(d >= 50);

    A = 0.;
    B = 1.;
    C = 0.;
    D = 1.;
    R_SQ = 0.099 * 0.099;
    INN_DENSITY = 1.;
    OUT_DENSITY = 0.;
    U = 1.;
    V = 1.;
    TAU = 1.e-3;
    TIME_STEP_CNT = 5;
    APPROX_TYPE = 1;
    NX = d;
    NY = d;
    NX_1 = NX + 1;
    NY_1 = NY + 1;
    HX = (B - A) / NX;
    HY = (D - C) / NY;
    IDEAL_SQ_SIZE_X = 64;
    IDEAL_SQ_SIZE_Y = 64;
    RES_EPS = 1.e-9;

    R_LVL = 2;
    HX_SMALLEST = (B - A) / (NX * std::pow(3., R_LVL));
    HY_SMALLEST = (D - C) / (NY * std::pow(3., R_LVL));
    EPS_GRID = 0.5;

    int sz = d;
    sz = sz * ((int) std::pow(3., R_LVL));
    NX3 = sz;
    NY3 = sz;
    NX3_1 = NX3 + 1;
    NY3_1 = NY3 + 1;
    XY = NX3_1 * NY3_1;
    R = (int) std::pow(3., R_LVL);

    CENTER_OFFSET_X = 0.3;
    CENTER_OFFSET_Y = 0.3;

    init_boundary_arrays_and_cp(NX3_1, NY3_1);
    print_params();

    int *grid = new int[XY];
    int *gridPr = new int[XY];

    double *density = solve_2(grid, gridPr);
    double *exact0 = calc_exact_2(grid, 0, NX3_1, NY3_1, HX_SMALLEST, HY_SMALLEST, R_LVL);
    double *exactT = calc_exact_2(grid, TAU * TIME_STEP_CNT, NX3_1, NY3_1, HX_SMALLEST, HY_SMALLEST, R_LVL);

    double x0 = get_center_x();
    double y0 = get_center_y();
    print_surface("exact", NX, NY, HX, HY, 0, A, C, x0, y0, TAU, U, V, exact0);
    print_surface("exact", NX, NY, HX, HY, TIME_STEP_CNT, A, C, x0, y0, TAU, U, V, exactT);

    delete[] density;
    delete[] exact0;
    delete[] exactT;
    delete[] grid;
    delete[] gridPr;
}

// убрано притягивание сетки
// убрана рекурсия
// деревья вместо массивов?
//
TEST_F(FemFixture, test1) {
    for (int i = 1; i < 2; ++i) {
        double d = 0;
        switch (i) {
            case 0:
                d = 50.;
                break;
            case 1:
                d = 100.;
                break;
            case 2:
                d = 200.;
                break;
            case 3:
                d = 400.;
                break;
            case 4:
                d = 800.;
                break;
            case 5:
                d = 1600.;
                break;
            default:
                return;
        }
        run_solver_1(d);
    }
}

// убрано притягивание сетки
// убрана рекурсия
// деревья вместо массивов?
//
TEST_F(FemFixture, test2) {
    for (int i = 1; i < 2; ++i) {
        double d = 0;
        switch (i) {
            case 0:
                d = 50.;
                break;
            case 1:
                d = 100.;
                break;
            case 2:
                d = 200.;
                break;
            case 3:
                d = 400.;
                break;
            case 4:
                d = 800.;
                break;
            case 5:
                d = 1600.;
                break;
            default:
                return;
        }
        run_solver_2(d);
    }
}

TEST_F(FemFixture, openmp) {
    int nthreads, tid;
#pragma omp parallel private(nthreads, tid)
    {
        tid = omp_get_thread_num();
        printf("Hello World from thread = %d\n", tid);

        if (tid == 0) {
            nthreads = omp_get_num_threads();
            printf("Number of threads = %d\n", nthreads);
        }
    }

    double d = 100.;
    NX = (int) d;
    NY = (int) d;
    NX_1 = NX + 1;
    NY_1 = NY + 1;
    XY = NX_1 * NY_1;

    // тестируем сетку ввиде дерева
    {
        NX = 3;
        NY = 3;
        NX_1 = NX + 1;
        NY_1 = NY + 1;
        XY = NX_1 * NY_1;
        using namespace boost;
        typedef adjacency_list<vecS, vecS, bidirectionalS> Graph;
        Graph g(XY);
        for (int i = 0; i < NX_1; ++i) {
            int stride = i*NX_1;
            for (int j = 0; j < NY_1 - 1; ++j) {
                add_edge(j + stride, j + stride + 1, g);
            }
        }
        auto es = edges(g);
        for(auto it = es.first; it != es.second; it++)
            std::cout << (*it);

        write_graphviz(std::cout, g);
    }

    Graph g;
    Vertex v1 = g.add_vertex();
    Vertex v2 = g.add_vertex();
    Vertex v3 = g.add_vertex();
    g.add_edge(v1, v2);
    g.add_edge(v2, v3);

    for (int i = 0; i < NX_1; ++i) {
        for (int j = 0; j < NY_1; ++j) {
            if (i >= 0 && i <= NX_1 - 1 && j >= 0 && j <= NY_1 - 1) {
                Vertex v1 = g.add_vertex();
                Vertex v2 = g.add_vertex();
                Vertex v3 = g.add_vertex();
                Vertex v4 = g.add_vertex();
            }
        }
    }
}