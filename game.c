#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<assert.h>

/*simplifying work*/
#define for_x for(i = 0; i < s; i++)
#define for_y for(j = 0; j < s; j++)
#define for_xy for_x for_y

/*again, simplifying work*/
#define CELL(x, y) current_state[x*s + y]
#define SET_LIVE next_state[i*s + j] = 1
#define SET_DEAD next_state[i*s + j] = 0

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))


const char *usage = "Usage: \n"
        "./life -s size -n number_of_iterations -f\n"
        "\tsize - here you can define the size of the board. Write 30 to generate a 30x30 board. Default value is 20.\n"
        "\tn - the number of iterations. By default it is set to 10. \n"
        "\tf - if you type it, all generations will occur right away.\n";


/*counting the number of neighbours*/
int neighbours(const int *current_state, int s, int x, int y) {
    int neighbours_no, i, j;
    neighbours_no = 0;

    for (i = x - 1; i <= x + 1; i++) {
        for (j = y - 1; j <= y + 1; j++) {
            if ((i < s) && (j < s) && (i >= 0) && (j >= 0) && CELL(i, j))
                neighbours_no++;
        }
    }
    /*exclude targeted cell*/
    if (CELL(x, y))
        neighbours_no--;
    return neighbours_no;
}

/*next generations are being made here*/
void generate(int *current_state, int *next_state, int s) {
    int i, j, neighbour_no;
    for_xy {
            neighbour_no = neighbours(current_state, s, i, j);

            /*Setting cells' states*/
            if (CELL(i, j)) { /*current cell*/
                if (neighbour_no < 2 || neighbour_no > 3) SET_DEAD;
                else
                    SET_LIVE;
            } else {
                if (neighbour_no == 3) SET_LIVE; /*alive*/
                else
                    SET_DEAD; /*dead*/
            }
        }
}

/*printing board containing dead and alive cells*/
int print_board(const int *current_state, int s) {
    int i, j;

    for_x {
        for_y {
            if (CELL(i, j)) fprintf(stdout, "O "); /*if current_state = 1, print 0*/
            else printf(". ");
        }
        printf("\n");
    }
    return 0;
}

/*tiny perk so that the output fits in terminal*/
void get_terminal_size(int *ncols, int *nrows) {
    assert(ncols);
    assert(nrows);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *ncols = w.ws_col;
    *nrows = w.ws_row;
}


int main(int argc, char **argv) {
    int i, s, n, f, *current_state, *next_state, *tmp, opt, ncols, nrows; /*s - size, n - no. of generations, f - fast generation*/

    n = 10;
    s = 20;
    f = 0;

    get_terminal_size(&ncols, &nrows);

    if (argc < 1) {
        puts(usage);
        return EXIT_FAILURE;
    }

    while ((opt = getopt(argc, argv, "s:n:f")) != -1) {
        switch (opt) {
            case 's':
                s = atoi(optarg);
                if (s > (ncols - 4) || s > (nrows - 4)) {
                    fprintf(stderr,
                            "Too big size of the matrix given - it will not fit in console... Please reduce it to %d.\n",
                            MIN((ncols - 4), (nrows - 4)));
                    return EXIT_FAILURE;
                }
                break;
            case 'n':
                n = atoi(optarg);
                break;
            case 'f':
                f = 1;
                break;
            default:
                fprintf(stderr,
                        "%s: Incorrect try of induction. %c was incorrect. Please read the following and try again.\n",
                        argv[0], opt);
                puts(usage);
                return EXIT_FAILURE;
        }
    }

    current_state = malloc(s * s * sizeof(*current_state));
    next_state = malloc(s * s * sizeof(*next_state));

    srand((unsigned int) time(NULL));

    /*filling matrix randomly with dead or alive cells one space by one space*/
    for (i = 0; i < s * s; i++) {
        current_state[i] = rand() % 2; /*0 or 1*/
    }

    fprintf(stdout, "\nGeneration started. Matrix will be %dx%d. There will be %d iterations.\n\n", s, s, n);
    if (f == 0) sleep(1);

    for (i = 0; i < n; i++) {
        print_board(current_state, s);
        generate(current_state, next_state, s);
        tmp = current_state;
        current_state = next_state;
        next_state = tmp;
        fprintf(stdout, "\n\n");
        if ((i != n - 1) && (f == 0)) sleep(2);
        if (i == n - 1) {
            fprintf(stdout, "%d generations complete. The programme will end now. Bye!\n", n);
            if (f == 0) sleep(1);
        }
    }

    free(current_state);
    free(next_state);

    return 0;
}
