#include<unistd.h>

int main() {
    execlp("box64"," ","/home/toast/pol/bin/wine64", (char*)NULL);
    return;
}