#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define maxline 1000

typedef struct position
{
    int i;
    int j;
} position;

int x1rows, x1columns;
int x1[maxline][maxline];

int x2rows, x2columns;
int x2[maxline][maxline];

float result1_time = 0;
float result2_time = 0;
int result1[maxline][maxline];
int result2[maxline][maxline];

void read_file(char filename[]);

void write_file();

void *matrix_multiply_by_element(void *arg);

void *matrix_multiply_by_row(void *arg);

int main()
{
    // reading file name from user
    printf("Enter file name: ");

    char filename[maxline];

    fgets(filename, maxline, stdin);
    if ((strlen(filename) > 0) && (filename[strlen(filename) - 1] == '\n'))
        filename[strlen(filename) - 1] = '\0';

    read_file(filename);

    // check the multiplication property
    if (x1columns != x2rows)
    {
        printf("ERROR: sizes does not match for Matrix Multiplication\n");
        return 0;
    }

    // getting current time before starting the first approach
    clock_t start, end;
    start = clock();

    // creating threads for each element in the matrix
    pthread_t th1[x1rows][x2columns];
    position current_position[maxline][maxline];
    for (int i = 0; i < x1rows; i++)
    {
        for (int j = 0; j < x2columns; j++)
        {
            current_position[i][j].i = i;
            current_position[i][j].j = j;
            pthread_create(&th1[i][j], NULL, &matrix_multiply_by_element, &current_position[i][j]);
            //printf("thread no.: %ld\n", th1[i][j]);
        }
    }

    //joining the threads to wait for the whole multtiplication process to finish
    for (int i = 0; i < x1rows; i++)
    {
        for (int j = 0; j < x2columns; j++)
        {
            pthread_join(th1[i][j], NULL);
        }
    }
    //printf("\n\n");

    // getting current time after the multiplication finish and calculating the time taken
    end = clock();
    result1_time = ((float)(end - start)) / CLOCKS_PER_SEC;

    // getting current time before starting the second approach
    start = clock();

    // creating threads for each row in the matrix
    pthread_t th2[x1rows];
    int currentrow[maxline];
    for (int i = 0; i < x1rows; i++)
    {
        currentrow[i] = i;
        pthread_create(&th2[i], NULL, &matrix_multiply_by_row, &currentrow[i]);
        //printf("thread no.: %ld\n", th2[i]);
    }

    //joining the threads to wait for the whole multtiplication process to finish
    for (int i = 0; i < x1rows; i++)
    {
        pthread_join(th2[i], NULL);
    }

    // getting current time after the multiplication finish and calculating the time taken
    end = clock();
    result2_time = ((float)(end - start)) / CLOCKS_PER_SEC;

    write_file();

    return 0;
}

void read_file(char filename[])
{
    // opening the file for reading
    FILE *fptr;
    fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("ERROR: file not found or being used.");
    }
    else
    {
        // reading the size of the first matrix
        char x1size[maxline];
        fgets(x1size, maxline, fptr);
        if ((strlen(x1size) > 0) && (x1size[strlen(x1size) - 1] == '\n'))
            x1size[strlen(x1size) - 1] = '\0';

        char *ptr;
        ptr = strtok(x1size, " ");
        int i = 0;
        while (ptr != NULL)
        {
            if (i == 0)
                x1rows = atoi(ptr);

            else if (i == 1)
                x1columns = atoi(ptr);
            ptr = strtok(NULL, " ");
            i++;
        }

        // reading the first matrix
        char row1[maxline];
        int j;
        for (i = 0; i < x1rows; i++)
        {
            j = 0;
            fgets(row1, maxline, fptr);
            if ((strlen(row1) > 0) && (row1[strlen(row1) - 1] == '\n'))
                row1[strlen(row1) - 1] = '\0';
            ptr = strtok(row1, " ");
            while (ptr != NULL && j < x1columns)
            {
                x1[i][j] = atoi(ptr);
                ptr = strtok(NULL, " ");
                j++;
            }
        }

        // reading the size of the second matrix
        char x2size[maxline];
        fgets(x2size, maxline, fptr);
        if ((strlen(x2size) > 0) && (x2size[strlen(x2size) - 1] == '\n'))
            x2size[strlen(x2size) - 1] = '\0';

        ptr = strtok(x2size, " ");
        i = 0;
        while (ptr != NULL)
        {
            if (i == 0)
                x2rows = atoi(ptr);

            else if (i == 1)
                x2columns = atoi(ptr);
            ptr = strtok(NULL, " ");
            i++;
        }

        //reading the second matrix
        char row2[maxline];
        for (i = 0; i < x2rows; i++)
        {
            j = 0;
            fgets(row2, maxline, fptr);
            if ((strlen(row2) > 0) && (row2[strlen(row2) - 1] == '\n'))
                row2[strlen(row2) - 1] = '\0';
            ptr = strtok(row2, " ");
            while (ptr != NULL && j < x2columns)
            {
                x2[i][j] = atoi(ptr);
                ptr = strtok(NULL, " ");
                j++;
            }
        }
    }
    fclose(fptr);
}

void write_file()
{
    // open file for writting
    FILE *fptr;
    fptr = fopen("matrix_output.txt", "w");
    if (fptr == NULL)
    {
        printf("ERROR: file not found or being used.");
    }
    else
    {
        // printing the result matrix of first approach
        for (int i = 0; i < x1rows; i++)
        {
            for (int j = 0; j < x2columns; j++)
            {
                fprintf(fptr, "%d\t\t", result1[i][j]);
            }
            fprintf(fptr, "\n");
        }

        // printing the time taken by first approach
        fprintf(fptr, "END1\t%.6f\n", result1_time);

        // printing the result matrix of second approach
        for (int i = 0; i < x1rows; i++)
        {
            for (int j = 0; j < x2columns; j++)
            {
                fprintf(fptr, "%d\t\t", result2[i][j]);
            }
            fprintf(fptr, "\n");
        }

        // printing the time taken by second approach
        fprintf(fptr, "END2\t%.6f", result2_time);
    }
    fclose(fptr);
}

void *matrix_multiply_by_element(void *arg)
{
    // converting parameters
    position *current_position = arg;
    int i = current_position->i;
    int j = current_position->j;

    // giving the element intial value
    result1[i][j] = 0;

    // multiply
    for (int k = 0; k < x2rows; k++)
    {
        result1[i][j] += x1[i][k] * x2[k][j];
    }

    return NULL;
}

void *matrix_multiply_by_row(void *arg)
{
    // converting parameters
    int *current_row = arg;
    int i = *current_row;

    // looping on every column
    for (int j = 0; j < x2columns; j++)
    {
        // giving the element intial value
        result2[i][j] = 0;

        // multiply
        for (int k = 0; k < x2rows; k++)
        {
            result2[i][j] += x1[i][k] * x2[k][j];
        }
    }

    return NULL;
}