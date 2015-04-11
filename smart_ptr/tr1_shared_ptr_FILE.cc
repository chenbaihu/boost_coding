#include <stdio.h>
#include <tr1/memory>

typedef std::tr1::shared_ptr<FILE> FILEPtr;

struct FreeFILE {
    void operator()(FILE* fp) {
        if (fp!=NULL) {
            fclose(fp);
        }  
    }  
};

int main(int argc, char *argv[])
{
    FILEPtr fp_ptr;
    fp_ptr.reset(fopen("./test.txt", "w"), FreeFILE());
    fprintf(fp_ptr.get(), "%s\n", "hello, this is a test");
    fp_ptr.reset();

    return 0;
}


