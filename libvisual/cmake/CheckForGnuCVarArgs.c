int a(int p1, int p2, int p3) {}
#define call_a(params...) a(1,params)

int main() {
  call_a(2,3);
}
