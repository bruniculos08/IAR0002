#include <bits/stdc++.h>

using namespace std;


#define HEIGHT 10
#define WIDTH 5


char ***create()
{
    char ***ptr;
    ptr = (char ***) malloc(sizeof(char **) * HEIGHT);
    for(int y = 0; y < HEIGHT; y++)
    {
        *(ptr + y) = (char **) malloc(sizeof(char *) * WIDTH);
        for(int x = 0; x < WIDTH; x++)
        {
            *(*(ptr + y) + x) = (char *) malloc(sizeof(char));
            *(*(*(ptr + y) + x)) = 'a';
        }
    }
    return ptr;
}

int main(void){
    char ***data;
    data = create();
    for(int y = 0; y < HEIGHT; y++)
    {
        for(int x = 0; x < WIDTH; x++)
        {
            cout << *(data[y][x]) << " ";
        }
        cout << endl;
    }
}