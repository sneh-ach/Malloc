#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

void run_test_1()
{
    printf("1 - Running test 1: Simple malloc and free\n");

    char *ptr = (char *)malloc(65535);
    free(ptr);
}

void run_test_2()
{
    printf("2 - Running test 2: Exercise malloc and free\n");

    char *ptr = (char *)malloc(65535);

    char *ptr_array[1024];

    int i;
    for (i = 0; i < 1024; i++)
    {
        ptr_array[i] = (char *)malloc(1024);

        ptr_array[i] = ptr_array[i];
    }

    free(ptr);

    for (i = 0; i < 1024; i++)
    {
        if (i % 2 == 0)
        {
            free(ptr_array[i]);
        }
    }

    ptr = (char *)malloc(65535);
    free(ptr);
}

void run_test_3()
{
    printf("3 - Running test 3: Test coalesce\n");

    char *ptr1 = (char *)malloc(1200);
    char *ptr2 = (char *)malloc(1200);

    free(ptr1);
    free(ptr2);

    char *ptr3 = (char *)malloc(2048);

    free(ptr3);
}

void run_test_4()
{
    printf("4 - Running test 4: Block split and reuse\n");

    char *ptr1 = (char *)malloc(2048);

    free(ptr1);

    char *ptr2 = (char *)malloc(1024);

    free(ptr2);
}

void run_test_5()
{
    printf("BFWF - Running test 5: Best fit and worst fit\n");

    char *ptr1 = (char *)malloc(65535);
    char *buffer1 = (char *)malloc(1);
    char *ptr4 = (char *)malloc(65);
    char *buffer2 = (char *)malloc(1);
    char *ptr2 = (char *)malloc(6000);

    printf("Worst fit should pick this one: %p\n", ptr1);
    printf("Best fit should pick this one: %p\n", ptr2);

    free(ptr1);
    free(ptr2);

    buffer1 = buffer1;
    buffer2 = buffer2;
    ptr4 = ptr4;

    char *ptr3 = (char *)malloc(1000);
    printf("Chosen address: %p\n", ptr3);
}

void run_test_6()
{
    printf("Calloc - Running test 6\n");

    int *array;

    array = (int *)calloc(5, sizeof(int));

    assert(array[0] == 0);
    assert(array[1] == 0);
    assert(array[2] == 0);
    assert(array[3] == 0);
    assert(array[4] == 0);

    printf("calloc test PASSED\n");

    free(array);
}
void run_test_7()
{
    printf("FFNF - Running test 7: First fit and next fit\n");

    char *ptr1 = (char *)malloc(1000);
    char *buf1 = (char *)malloc(1);
    char *ptr6 = (char *)malloc(10);
    char *buf2 = (char *)malloc(1);
    char *ptr2 = (char *)malloc(6000);
    char *buf3 = (char *)malloc(1);
    char *ptr7 = (char *)malloc(10);
    char *buf4 = (char *)malloc(1);
    char *ptr3 = (char *)malloc(1000);

    printf("First fit should pick this one: %p\n", ptr1);
    printf("Next fit should pick this one: %p\n", ptr3);

    free(ptr1);
    free(ptr2);
    free(ptr3);

    char *ptr5 = (char *)malloc(6000);

    ptr5 = ptr5;
    ptr7 = ptr7;
    ptr6 = ptr6;
    buf1 = buf1;
    buf2 = buf2;
    buf3 = buf3;
    buf4 = buf4;

    char *ptr4 = (char *)malloc(1000);
    printf("Chosen address: %p\n", ptr4);
}

void run_test_8()
{
    printf("Realloc - Running test 8\n");

    int *ptr = (int *)malloc(sizeof(int) * 2);
    int i;
    int *ptr_new;

    *ptr = 10;
    *(ptr + 1) = 20;

    ptr_new = (int *)realloc(ptr, sizeof(int) * 3);
    *(ptr_new + 2) = 30;

    for (i = 0; i < 3; i++)
    {
        assert(*(ptr_new + i) == (i + 1) * 10);
    }

    printf("realloc test PASSED\n");

    free(ptr_new);
}
int main()
{
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    run_test_1();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Test 1 took %f seconds to execute.\n\n", cpu_time_used);

    start = clock();
    run_test_2();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Test 2 took %f seconds to execute.\n\n", cpu_time_used);

    start = clock();
    run_test_3();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Test 3 took %f seconds to execute.\n\n", cpu_time_used);

    start = clock();
    run_test_4();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Test 4 took %f seconds to execute.\n\n", cpu_time_used);

    start = clock();
    run_test_5();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Test 5 took %f seconds to execute.\n\n", cpu_time_used);

    start = clock();
    run_test_6();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Test 6 took %f seconds to execute.\n\n", cpu_time_used);

    start = clock();
    run_test_7();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Test 7 took %f seconds to execute.\n\n", cpu_time_used);

    start = clock();
    run_test_8();
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Test 8 took %f seconds to execute.\n\n", cpu_time_used);

    return 0;
}
