int a(int p1, int p2, int p3) {}
#define call_a(...) a(1,__VA_ARGS__)

int main() {
  call_a(2,3);
}
