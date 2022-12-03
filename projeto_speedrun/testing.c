
// AED, August 2022 (Tomás Oliveira e Silva)
//
// First practical assignement (speed run)
//
// Compile using either
//    cc -Wall -O2 -D_use_zlib_=0 testing.c -o testing -lm
// or
//   cc -Wall -O2 -D_use_zlib_=1 testing.c -o testing -lm -lz
//
// Place your student numbers and names here
//   N.Mec. 108193  Name: Matilde Teixeira
//   N.Mec  107658  Name: Maria Rafaela Abrunhosa
//

//
// static configuration
//

#define _max_road_size_ 800 // the maximum problem size
#define _min_road_speed_ 2  // must not be smaller than 1, shouldnot be smaller than 2
#define _max_road_speed_ 9  // must not be larger than 9 (only because of the PDF figure)
#define STACK_EMPTY
#define EMPTY -1
//
// include files --- as this is a small project, we include the PDF generation code directly from make_custom_pdf.c
//

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "elapsed_time.h"
#include "make_custom_pdf.c"

//
// nossas variáveis
//

//static int last_speed = 0;
//static int last_position = 0;
//static int last_move_number = 0;

static int max_road_speed[1 + _max_road_size_]; // positions 0.._max_road_size_

void init_road_speeds(void)
{
  double speed;
  int i;

  for (i = 0; i <= _max_road_size_; i++)
  {
    speed = (double)_max_road_speed_ * (0.55 + 0.30 * sin(0.11 * (double)i) + 0.10 * sin(0.17 * (double)i + 1.0) + 0.15 * sin(0.19 * (double)i));
    max_road_speed[i] = (int)floor(0.5 + speed) + (int)((unsigned int)random() % 3u) - 1;
    if (max_road_speed[i] < _min_road_speed_)
      max_road_speed[i] = _min_road_speed_;
    if (max_road_speed[i] > _max_road_speed_)
      max_road_speed[i] = _max_road_speed_;
  }
}

// static int speed = 0;

//
//////////////////////changing it to match com distancia de travagem////////////////////////////////////////////////////
//
typedef struct
{
  int n_moves;                        // the number of moves (the number of positions is one more than the number of moves)
  int positions[1 + _max_road_size_]; // the positions (the first one must be zero)
}

solution_t;
static solution_t solution_2_best;
static double solution_2_elapsed_time; // time it took to solve the problem
static unsigned long solution_2_count; // effort dispended solving the problem

// 5 - 4 - 3 - 2 -1
bool testar_travagem(int new_speed, int positions, int move_number, int final_position)
{
  if (positions + new_speed > final_position)
  {
    return false;
  }
  else
  {
    for (int pos = positions; pos <= positions + new_speed; pos++)
    {
      solution_2_count++;
      if (new_speed > max_road_speed[pos])
      {
        return false;
      }
    }

    if (new_speed == 1)
    {
      return true;
    }
    else
    {
      positions = positions + new_speed;
      return testar_travagem(new_speed - 1, positions, move_number, final_position);
    }
  }
}

void solution_2_travagem(int speed, int position, int move_number, int final_position)
{
  while (position != final_position)
  {
  // try all legal speeds
    for (int new_speed = speed + 1; new_speed >= speed - 1; new_speed--)
    {
      if (new_speed >= 1 && new_speed <= _max_road_speed_)
      {
        if (testar_travagem(new_speed, position, move_number, final_position) == true)
        {
          solution_2_best.positions[move_number++] = new_speed; 
          solution_2_best.n_moves = move_number;
          speed = new_speed;
          position += new_speed;
          break;
        }
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

static void solve_2(int final_position)
{
  if (final_position < 1 || final_position > _max_road_size_)
  {
    fprintf(stderr, "solve_2: bad final_position\n");
    exit(1);
  }
  solution_2_elapsed_time = cpu_time();
  solution_2_count = 0ul;
  solution_2_best.n_moves = final_position + 100;
  solution_2_travagem(0, 0, 0, final_position);
  // solution_2_travagem(last_speed, last_position, last_move_number, final_position);
  solution_2_elapsed_time = cpu_time() - solution_2_elapsed_time;
}

//
// example of the slides
//

static void example(void)
{
  int i, final_position;

  srandom(0xAED2022);
  init_road_speeds();
  final_position = 30;
  solve_2(final_position);
  make_custom_pdf_file("example.pdf", final_position, &max_road_speed[0], solution_2_best.n_moves, &solution_2_best.positions[0], solution_2_elapsed_time, solution_2_count, "Plain recursion");
  printf("mad road speeds:");
  for (i = 0; i <= final_position; i++)
    printf(" %d", max_road_speed[i]);
  printf("\n");
  printf("positions:");
  for (i = 0; i <= solution_2_best.n_moves; i++)
    printf(" %d", solution_2_best.positions[i]);
  printf("\n");
}

//
// main program
//

int main(int argc, char *argv[argc + 1])
{
#define _time_limit_ 3600.0
  int n_mec, final_position, print_this_one;
  char file_name[64];

  // generate the example data
  if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'e' && argv[1][2] == 'x')
  {
    example();
    return 0;
  }
  // initialization
  n_mec = (argc < 2) ? 0xAED2022 : atoi(argv[1]);
  srandom((unsigned int)n_mec);
  init_road_speeds();
  // run all solution methods for all interesting sizes of the problem
  final_position = 1;
  solution_2_elapsed_time = 0.0;
  printf("    + --- ---------------- --------- +\n");
  printf("    |                plain recursion |\n");
  printf("--- + --- ---------------- --------- +\n");
  printf("  n | sol            count  cpu time |\n");
  printf("--- + --- ---------------- --------- +\n");
  while (final_position <= _max_road_size_ /* && final_position <= 20*/)
  {
    print_this_one = (final_position == 10 || final_position == 20 || final_position == 50 || final_position == 100 || final_position == 200 || final_position == 400 || final_position == 800) ? 1 : 0;
    printf("%3d |", final_position);
    // first solution method (very bad)
    if (solution_2_elapsed_time < _time_limit_)
    {
      solve_2(final_position);
      if (print_this_one != 0)
      {
        sprintf(file_name, "%03d_1.pdf", final_position);
        make_custom_pdf_file(file_name, final_position, &max_road_speed[0], solution_2_best.n_moves, &solution_2_best.positions[0], solution_2_elapsed_time, solution_2_count, "Plain recursion");
      }
      printf(" %3d %16lu %9.3e |", solution_2_best.n_moves, solution_2_count, solution_2_elapsed_time);
    }
    else
    {
      solution_2_best.n_moves = -1;
      printf("                                |");
    }
    // second solution method (less bad)
    // ...

    // done
    printf("\n");
    fflush(stdout);
    // new final_position
    if (final_position < 50)
      final_position += 1;
    else if (final_position < 100)
      final_position += 5;
    else if (final_position < 200)
      final_position += 10;
    else
      final_position += 20;
  }
  printf("--- + --- ---------------- --------- +\n");
  return 0;
#undef _time_limit_
}


























// -------------------------------------------------------------------------


// #define _max_road_size_  800  // the maximum problem size
// #define _min_road_speed_   2  // must not be smaller than 1, should not be smaller than 2
// #define _max_road_speed_   9  // must not be larger than 9 (only because of the PDF figure)


// #include <math.h>
// #include <stdio.h>
// #include "elapsed_time.h"
// #include "make_custom_pdf.c"


// static int max_road_speed[1 + _max_road_size_]; // positions 0.._max_road_size_

// static void init_road_speeds(void)
// {
//   double speed;
//   int i;

//   for(i = 0;i <= _max_road_size_;i++)
//   {
//     speed = (double)_max_road_speed_ * (0.55 + 0.30 * sin(0.11 * (double)i) + 0.10 * sin(0.17 * (double)i + 1.0) + 0.15 * sin(0.19 * (double)i));
//     max_road_speed[i] = (int)floor(0.5 + speed) + (int)((unsigned int)random() % 3u) - 1;
//     if(max_road_speed[i] < _min_road_speed_)
//       max_road_speed[i] = _min_road_speed_;
//     if(max_road_speed[i] > _max_road_speed_)
//       max_road_speed[i] = _max_road_speed_;
//   }
// }

// typedef struct
// {
//   int n_moves;
//   int positions[1 + _max_road_size_];
// }
// solution_t;

// static solution_t solution_1,solution_1_best;
// static double solution_1_elapsed_time; // time it took to solve the problem
// static unsigned long solution_1_count; // effort dispended solving the problem

// static void solution_1_recursion_otimizado(int move_number,int position,int speed,int final_position)
// {
//   int i,new_speed;

//   if (solution_1_best.n_moves != final_position + 100) return;

//   solution_1_count++;
//   solution_1.positions[move_number] = position;
   
//   if(position == final_position && speed == 1)
//   {
//     // is it a better solution?
//     if(move_number < solution_1_best.n_moves)
//     {
//       solution_1_best = solution_1;
//       solution_1_best.n_moves = move_number;
//     }
//     return;
//   }

//   //try Rafaela
//   if(final_position <= 3){
//     solution_1_best.n_moves = final_position;
//   }else{
//     for(new_speed = speed + 1;new_speed >= speed - 1;new_speed--) // testar primeiro quando a velocidade aumenta
//       if(new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position){
//         for(i = 0; new_speed <= max_road_speed[position + i]; i++)
//           ;
        
//         if(solution_1_best.positions[move_number] > solution_1.positions[move_number]){
//           return;
//         }

//         if(i>=new_speed){
//           //printf("valor do i: %d ", i);
//           solution_1_recursion_otimizado(move_number + 1, position + new_speed, new_speed, final_position);
//         }
//       }
//     }
// }

// // static solution_t solution_2,solution_2_best;
// // static double solution_2_elapsed_time; // time it took to solve the problem
// // static unsigned long solution_2_count;

// // static void solution_2_function(int move_number,int position,int speed,int final_position){
// //   int i,new_speed;

// //   solution_2.positions[move_number] = position;
  
// //   //second try Rafaela
// //   // if(final_position <= 3){
// //   //   solution_2_count++;
// //   //   solution_2_best.n_moves = final_position;
// //   // }else{
// //     while((position != final_position)){

// //       solution_2_count++;
// //       solution_2.positions[move_number] = position;
// //       // solution_2_best.n_moves = move_number;

// //       for(new_speed = speed + 1;new_speed >= speed - 1;new_speed--){
// //         if(new_speed >= 1 && new_speed <= _max_road_speed_ && position + new_speed <= final_position){
// //           for(i = 0; new_speed <= max_road_speed[position + i]; i++)
// //             ;
          
// //           if(solution_2_best.positions[move_number] > solution_2.positions[move_number]){ // como é que eu atualizo o valor do best.positions[move_number]
// //             return;
// //           }
          
// //           if(i >= new_speed){
// //             //printf("valor do i: %d ", i);
// //             move_number = move_number + 1;
// //             position = position + new_speed;
// //             speed = new_speed;
// //             //printf("nova posicao: %d ", position);
// //           }


// //           if(position == final_position - 1 && new_speed == 2){ // esta condição precisa de estar fora do ciclo while porque a position não será igual à final position
// //             // is it a better solution?
// //             if(move_number < solution_2_best.n_moves)
// //             {
// //               solution_2_best = solution_2;
// //               solution_2_best.n_moves = move_number;
// //             }
// //             return;
// //           }

// //           if (solution_2_best.n_moves != final_position + 100) return;
// //         }
// //       }
// //     }
// //   }
// // //}


// static void solve_1(int final_position){
//   if(final_position < 1 || final_position > _max_road_size_)
//   {
//     fprintf(stderr,"solve_1: bad final_position\n");
//     exit(1);
//   }
//   solution_1_elapsed_time = cpu_time();
//   solution_1_count = 0ul;
//   solution_1_best.n_moves = final_position + 100;
//   solution_1_recursion_otimizado(0,0,0,final_position);
//   solution_1_elapsed_time = cpu_time() - solution_1_elapsed_time;
// }


// //
// // example of the slides
// //

// static void example(void)
// {
//   int i,final_position;

//   srandom(0xAED2022);
//   init_road_speeds();
//   final_position = 800;
//   solve_1(final_position);
//   make_custom_pdf_file("example.pdf",final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
//   printf("mad road speeds:");
//   for(i = 0;i <= final_position;i++)
//     printf(" %d",max_road_speed[i]);
//   printf("\n");
//   printf("positions:");
//   for(i = 0;i <= solution_1_best.n_moves;i++)
//     printf(" %d",solution_1_best.positions[i]);
//   printf("\n");
// }


// //
// // main program
// //

// int main(int argc,char *argv[argc + 1])
// {
// # define _time_limit_  3600.0
//   int n_mec,final_position,print_this_one;
//   char file_name[64];

//   // generate the example data
//   if(argc == 2 && argv[1][0] == '-' && argv[1][1] == 'e' && argv[1][2] == 'x')
//   {
//     example();
//     return 0;
//   }
//   // initialization
//   n_mec = (argc < 2) ? 0xAED2022 : atoi(argv[1]);
//   srandom((unsigned int)n_mec);
//   init_road_speeds();
//   // run all solution methods for all interesting sizes of the problem
//   final_position = 1;
//   solution_1_elapsed_time = 0.0;
//   printf("    + --- ---------------- --------- +\n");
//   printf("    |                plain recursion |\n");
//   printf("--- + --- ---------------- --------- +\n");
//   printf("  n | sol            count  cpu time |\n");
//   printf("--- + --- ---------------- --------- +\n");
//   while(final_position <= _max_road_size_/* && final_position <= 20*/)
//   {
//     print_this_one = (final_position == 4 || final_position == 15 || final_position == 35 || final_position == 400) ? 1 : 0;
//     printf("%3d |",final_position);
//     // first solution method (very bad)
//     if(solution_1_elapsed_time < _time_limit_)
//     {
//       solve_1(final_position);
//       if(print_this_one != 0)
//       {
//         sprintf(file_name,"%03d_1.pdf",final_position);
//         make_custom_pdf_file(file_name,final_position,&max_road_speed[0],solution_1_best.n_moves,&solution_1_best.positions[0],solution_1_elapsed_time,solution_1_count,"Plain recursion");
//       }
//       printf(" %3d %16lu %9.3e |",solution_1_best.n_moves,solution_1_count,solution_1_elapsed_time);
//     }
//     else
//     {
//       solution_1_best.n_moves = -1;
//       printf("                                |");
//     }
//     // second solution method (less bad)
//     // ...

//     // done
//     printf("\n");
//     fflush(stdout);
//     // new final_position
//     if(final_position < 50)
//       final_position += 1;
//     else if(final_position < 100)
//       final_position += 5;
//     else if(final_position < 200)
//       final_position += 10;
//     else
//       final_position += 20;
//   }
//   printf("--- + --- ---------------- --------- +\n");
//   return 0;
// # undef _time_limit_
// }